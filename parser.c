#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "tokenizer.h"
#include "command.h"

void consume(TokenList *tokenList, TokenType type)
{
    Token *token = getCurrentToken(tokenList);
    if (token->type != type)
    {
        printf("Invalid Parse, Given token and type does not match");
        // TODO decide assert or exit at this point
    }
    tokenList->nextToken(tokenList);
}

int parse(TokenList* tokenList)
{
    int err = 0;
    while (tokenList->length != tokenList->idx)
    {
        err = 0;
        Token *token = getCurrentToken(tokenList);
        void *argv[3];
        memset(argv, 0, sizeof(argv));
        if (token->type == CD) {
            // It can be either string or directory, normally it supposed to be
            // directory but users can enter string without putting '/'
            // but in this case I am just gonna implement directory case
            printf("CD\n");
            consume(tokenList, CD);

            Token *directory = getCurrentToken(tokenList);
            TokenType cd = CD;
            if (directory != NULL) {
                if (directory->type == DIRECTORY) {
                    consume(tokenList, DIRECTORY);
                } else {
                    consume(tokenList, STRING);
                }
                argv[0] = &cd;
                argv[1] = directory->value;
            } else {
                argv[0] = &cd;
            }
            err = executeCommand(argv);
            if (err != 0)
                goto error;

        } else if (token->type == PWD) {
            consume(tokenList, PWD);
            Token *directory = getCurrentToken(tokenList);
            TokenType pwd = PWD;
            argv[0] = &pwd;
            err = executeCommand(argv);
            if (err != 0)
                goto error;

        } else if (token->type == HISTORY) {
            consume(tokenList, HISTORY);
            TokenType history = HISTORY;
            argv[0] = &history;
            err = executeCommand(argv);
            if (err != 0)
                goto error;
        } else if (token->type == EXIT) {
            consume(tokenList, EXIT);
            TokenType exit = EXIT;
            argv[0] = &exit;
            err = executeCommand(argv);
            if (err != 0)
                goto error;
        }
        else if (token->type == STRING) {
            printf("System Comamnd\n");
            break;
        }
    }

error:
    return err;
}