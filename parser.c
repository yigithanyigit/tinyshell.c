#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "parser.h"
#include "tokenizer.h"
#include "command.h"
#include "mem.h"
#include "queue.h"

// TODO make int return and err check for prevent SEGV
void consume(TokenList *tokenList, TokenType type)
{
    Token *token = getCurrentToken(tokenList);
    if (token == NULL)
    {
        printf("Invalid Parse, Token is NULL");
    }
    if (token->type != type)
    {
        printf("Invalid Parse, Given token and type does not match\n");
    }
    tokenList->nextToken(tokenList);
}

int parse(TokenList* tokenList)
{
    int err = 0;
    Queue *queue = createQueue();
    while (tokenList->length != tokenList->idx)
    {
        err = 0;
        Token *token = getCurrentToken(tokenList);
        void **argv = shell_calloc(4, sizeof(void *));
        if (token->type == CD) {
            consume(tokenList, CD);
            Token *directory = getCurrentToken(tokenList);
            if (directory != NULL) {
                /*
                if (directory->type == DIRECTORY) {
                    consume(tokenList, DIRECTORY);
                } else {
                    consume(tokenList, STRING);
                }
                argv[0] = &token->type;
                argv[1] = directory->value;
                */
                consume(tokenList, STRING);
                argv[0] = &token->type;
                argv[1] = directory->value;
            } else {
                argv[0] = &token->type;
            }
            //err = executeCommand(argv);
            //if (err != 0)
            //    goto error;

        } else if (token->type == PWD) {
            consume(tokenList, PWD);
            Token *directory = getCurrentToken(tokenList);
            argv[0] = &token->type;
            //err = executeCommand(argv);
            //if (err != 0)
            //   goto error;

        } else if (token->type == HISTORY) {
            consume(tokenList, HISTORY);
            argv[0] = &token->type;
            //err = executeCommand(argv);
            //if (err != 0)
            //    goto error;
        } else if (token->type == EXIT) {
            consume(tokenList, EXIT);
            argv[0] = &token->type;
            //err = executeCommand(argv);
            //if (err != 0)
            //    goto error;
        }
        else if (token->type == STRING) {
            char *str = shell_calloc(0, sizeof(char));
            while (getCurrentToken(tokenList) != NULL && getCurrentToken(tokenList)->type == STRING) {
                Token *token = getCurrentToken(tokenList);
                str = shell_realloc(str, strlen(str) + strlen(token->value) + 2);
                strcat(str, token->value);
                strcat(str, " ");
                consume(tokenList, STRING);
            }
            str[strlen(str) - 1] = '\0'; // remove the last space

            argv[0] = &token->type;
            argv[1] = str;
            //printf("System Command: %s\n", command->command);
            //err = executeCommand(argv);
            //shell_free(str);
            //if (err != 0)
                //goto error;
        } else if (token->type == PIPE) {
            argv[0] = &token->type;
            argv[1] = token->value;
            consume(tokenList, PIPE);
            //err = executeCommand(argv);
            //if (err != 0)
            //    goto error;
        } else if (token->type == AND) {
            argv[0] = &token->type;
            argv[1] = token->value;
            consume(tokenList, AND);
            //err = executeCommand(argv);
            //if (err != 0)
            //    goto error;
        } else if (token->type == BACKGROUND) {
            argv[0] = &token->type;
            argv[1] = token->value;
            consume(tokenList, BACKGROUND);
        } else {
            printf("Invalid Parse, Unknown Token Type\n");
            err = 1;
            goto error;
        }
        enqueue(queue, argv);
    }
    err = executeCommands(queue);

error:
    return err;
}