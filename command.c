#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "tokens.h"
#include "cd.h"
#include "mem.h"

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

Command* initCommand(Command *command, void** argv)
{
    const char *x = getCommandTypeName(*(TokenType*)argv[0]);
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


int executeCommand(void** argv)
{
    Command *command = shell_calloc(1, sizeof(Command));
    initCommand(command, argv);
    addCommand(&commandList, command);

    int err = 0;
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
    case NOTSET:
    default:
        printf("Invalid Command\n");
        break;
    }

end:
    return err;
}