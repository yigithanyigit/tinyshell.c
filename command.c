#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "command.h"
#include "tokenizer.h"
#include "parser.h"
#include "tokens.h"
#include "cd.h"
#include "mem.h"
#include "queue.h"

const char* getCommandTypeName(TokenType commandType) {
    switch (commandType) {
        case CD: return "cd";
        case PWD: return "pwd";
        case HISTORY: return "history";
        case EXIT: return "exit";
        default: return "Unknown";
    }
}

extern CommandList commandList;
extern TokenList tokenList;

Command* initCommand(Command *command, void** argv)
{
    const char *x = getCommandTypeName(*(TokenType*)argv[0]);
    if (strcmp(x, "Unknown") == 0) {
        char *cmd = shell_calloc(strlen((char*)argv[1]) + 1, sizeof(char));
        strcpy(cmd, (char*)argv[1]);
        command->command = cmd;
    } else  {
        int len = strlen(x) + 1;
        if (argv[1] != NULL)
            len += strlen((char*)argv[1]) + 1;
        char* copy = shell_calloc(len, sizeof(char));
        strcpy(copy, x);
        if (argv[1] != NULL) {
            strcat(copy, " ");
            strcat(copy, (char*)argv[1]);
        }
        command->command = copy;
    }
    return command;
}

void uninitCommand(Command *command)
{
    shell_free(command->command);
    shell_free(command);
}


void printCommands(CommandList *commandList)
{
    for (int i = 0; i < commandList->length && i < 10; i++) {
        printf("Command: %s\n", commandList->commands[i]->command);
    }
}

void freeCommands(CommandList *commandList)
{
    for (int i = 0; i < commandList->length; i++) {
        uninitCommand(commandList->commands[i]);
    }
}

void addCommand(CommandList *commandList, Command *command)
{
    commandList->commands = shell_realloc(commandList->commands, (commandList->length + 1) * sizeof(Command*));
    commandList->commands[commandList->length] = command;
    commandList->length++;
}

void setupCommandList(CommandList *commandList)
{
    commandList->addCommand = addCommand;
    commandList->freeCommands = freeCommands;
    commandList->printCommands = printCommands;
}

CommandList* initCommandList(CommandList *commandList)
{
    commandList->length = 0;
    commandList->idx = 0;
    commandList->commands = shell_calloc(0, sizeof(Command*));
    setupCommandList(commandList);
    return commandList;
}

void uninitCommandList(CommandList *commandList)
{
    commandList->freeCommands(commandList);
    shell_free(commandList->commands);
}

int printHistory()
{
    commandList.printCommands(&commandList);
    return 0;
}

int executePipe(Queue* pipe_commands)
{
    int i;
    int err = 0;
    int fd[2];
    pid_t pid;

    int n = 0;

    int fid = 0;

    while(!isEmpty(pipe_commands)) {
        void **argv = (void**)dequeue(pipe_commands);
        n++;
        pipe(fd);
        if (argv != NULL && (pid = fork()) == 0) {
            if (n == 1) {
                // if it is the first command
                // then read from stdin
                // and write to pipe
                dup2(fd[1], STDOUT_FILENO); // copy file descriptor to the standard output so it able to write to pipe
            } else {
                // if it is not the first command
                // then read from pipe
                // and write to pipe
                dup2(fid, STDIN_FILENO);
                if (!isEmpty(pipe_commands)) {
                    dup2(fd[1], STDOUT_FILENO);
                }
            }
            close(fd[0]);
            close(fd[1]);
            err = executeCommand(argv);
            if (err != 0) {
                break;
            }
            exit(0);
        } else if (pid > 0) {
            if (n == 1) {
                close(fd[1]);
            } else {
                close(fid);
                close(fd[1]);
            }
            for (i = 0; i < n; i++) {
                wait(NULL);
            }
            fid = fd[0];
        } else {
            printf("Fork failed\n");
            err = 1;
            break;
        }
    }

end:
    return err;
}

int executeAnd(void **argv1, void **argv2)
{
    int err = 0;
    err = executeCommand(argv1);
    if (err == 0) {
        err = executeCommand(argv2);
    }
    return err;
}

int executeCommand(void** argv)
{

    int err = 0;
    TokenType type = *(TokenType*)argv[0];
    switch (*(TokenType*)argv[0])
    {
    case CD:
        err = changeDir((char *)argv[1]);
        goto end;
    case PWD:
        err = printPwd();
        goto end;
    case HISTORY:
        err = printHistory();
        goto end;
    case EXIT:
        err = 2;
        goto end;
    case PIPE:
    case AND:
    case BACKGROUND:
    case STRING:
    {
        char *cmd_argv[] = {getenv("SHELL"), "-c", (char*)argv[1], NULL};
        pid_t pid;
        if ((pid = fork()) == 0) {
            if (argv[3] != NULL && strcmp((char*)argv[3], "1") == 0) {
                // write to /dev/null which is a black hole
                int dev_null = open("/dev/null", O_WRONLY);
                dup2(dev_null, STDIN_FILENO);
                dup2(dev_null, STDOUT_FILENO);
                dup2(dev_null, STDERR_FILENO);
                close(dev_null);
            }
            execvp(cmd_argv[0], cmd_argv);
            exit(1);
        } else if (pid > 0) {
            if (argv[3] != NULL && strcmp((char*)argv[3], "1") == 0) {
                printf("Background process %d\n", pid);
                goto end;
            } else {
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                    //printf("Command %s failed with status %d\n", (char*)argv[1], WEXITSTATUS(status));
                }
            }
        } else {
            printf("Fork failed\n");
            err = 1;
            goto end;
        }
        break;
    }
    default:
        printf("Invalid Command\n");
        printf("Command: %s\n", (char*)argv[0]);
        return err;
    }

end:
    {
        Command *command = shell_calloc(1, sizeof(Command));
        initCommand(command, argv);
        addCommand(&commandList, command);
    }
    return err;
}

int executeCommands(Queue *args_list)
{
    int err = 0;
    while (!isEmpty(args_list)) {
        void **args1 = (void**)dequeue(args_list);
        // Check if the next command is a pipe or and
        void **args2 = NULL;
        void **args3 = NULL;
        TokenType type = *(TokenType*)args1[0];
        if (type == PIPE || type == AND || type == BACKGROUND && !isEmpty(args_list)) {
            args2 = args1;
            args1 = (void**)dequeue(args_list);

        }
        if (!isEmpty(args_list) && (args2 = (void**)front(args_list)) != NULL){

            if (*(TokenType*)args2[0] == PIPE) {
                Queue *pipe_commands = createQueue();
                enqueue(pipe_commands, args1);
                while (!isEmpty(args_list) && (args2 = (void**)front(args_list)) != NULL
                    && *(TokenType*)args2[0] == PIPE) {
                    dequeue(args_list);
                    // Make sure command is not like "ls |"
                    if (!isEmpty(args_list)) {
                        args2 = (void**)dequeue(args_list);
                        enqueue(pipe_commands, args2);
                    }
                }

                err = executePipe(pipe_commands);
                if (err != 0) {
                    break;
                }
                shell_free(args1);
                shell_free(args2);
                continue;
            } else if (*(TokenType*)args2[0] == AND) {
                dequeue(args_list);
                args3 = (void**)dequeue(args_list);
                err = executeAnd(args1, args3);
                if (err != 0) {
                    break;
                }
                shell_free(args1);
                shell_free(args2);
                shell_free(args3);
                continue;
            } else if (*(TokenType*)args2[0] == BACKGROUND) {
                dequeue(args_list);
                args1[3] = "1";
            } else {
                shell_free(args2);
            }
        }
        err = executeCommand(args1);
        shell_free(args1);
        if (err == 2) {
            break;
        }
    }
    return err;
}