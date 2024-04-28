#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenizer.h"
#include "tokens.h"
#include "mem.h"

void initToken(Token *token, TokenType type)
{
    token->type = type;
    token->value = shell_calloc(0, sizeof(char));
    setupToken(token);
}

void setToken(Token *token, TokenType type, char *value)
{
    token->value = shell_realloc(token->value, (strlen(value) + 1) * sizeof(char));
    strncpy(token->value, value, strlen(value) + 1);
    token->type = type;
}

void initTokenList(TokenList *tokenList)
{
    tokenList->length = 0;
    tokenList->idx = 0;
    tokenList->tokens = shell_calloc(0, sizeof(Token*));
    setuptokenList(tokenList);
}

void uninitTokenList(TokenList *tokenList)
{
    freeTokens(tokenList);
    shell_free(tokenList->tokens);
}

void addToken(TokenList *tokenList, Token *token)
{
    tokenList->tokens = shell_realloc(tokenList->tokens, (tokenList->length + 1) * sizeof(Token*));
    tokenList->tokens[tokenList->length] = token;
    tokenList->length++;
}

void freeTokens(TokenList *tokenList)
{
    for (int i = 0; i < tokenList->length; i++) {
        shell_free(tokenList->tokens[i]->value);
    }
}

void printTokens(TokenList *tokenList)
{
    for (int i = 0; i < tokenList->length; i++) {
        printf("Token type: %d, Token value: %s\n", tokenList->tokens[i]->type, tokenList->tokens[i]->value);
    }
}

void setuptokenList(TokenList *tokenList)
{
    tokenList->addToken = addToken;
    tokenList->freeTokens = freeTokens;
    tokenList->printTokens = printTokens;
    tokenList->nextToken = nextToken;
}

void setupToken(Token *token)
{
    token->setToken = setToken;
}

void tokenizer(char *input, TokenList *tokenList)
{
    ///< Tokenize the input string
    ///< and store the tokens in tokenList
    int i = 0;
    TokenType type = NOTSET;
    size_t whitespace = 0;
    while (input[i] != '\0') {
        if (isspace(input[i])) {
            while(isspace(input[i]) || input[i] == '\n') {
                i++;
                whitespace++;
            }
            tokenizer(input + i, tokenList); // TODO: Make this more readible, call from a driver function
            input[i - whitespace] = '\0';
            break;
        }
        i++;
    };
    Token *token = shell_calloc(1, sizeof(Token));
    initToken(token ,type);
    int ret;
    //if ((ret = isNumber(input)) != 0) {
    //    type = NUMBER;
    if (strcmp(input, "&") == 0) {
        type = BACKGROUND;
    } else if (strcmp(input, "|") == 0) {
        type = PIPE;
    } else if (strcmp(input, "&&") == 0) {
        type = AND;
    } else if (strcmp(input, "cd") == 0) {
        type = CD;
    } else if (strcmp(input, "pwd") == 0) {
        type = PWD;
    } else if (strcmp(input, "history") == 0) {
        type = HISTORY;
    } else if (strcmp(input, "exit") == 0) {
        type = EXIT;
    }  else if (type == NOTSET) {
        // Its a either a path operator or a command or a invalid token
        /*
        for (int j = 0; j < i; j++) {
            if (input[j] == '/') {
                type = DIRECTORY;
                break;
            }
        }
        if (type == NOTSET)
            type = STRING;
        */
        type = STRING;
    } else {
        printf("Invalid token %s\n", input);
        shell_free(token);
        return;
    }
    token->setToken(token, type, input);
    tokenList->addToken(tokenList, token);
}

void regulateInput(char *input)
{
    ///< remove leading and trailing whitespaces
    int i = 0;
    while (isspace(input[i])) {
        i++;
    }
    int j = 0;
    while (input[i] != '\0') {
        input[j] = input[i];
        i++;
        j++;
    }
    input[j] = '\0';
    i = j - 1;
    while (isspace(input[i])) {
        input[i] = '\0';
        i--;
    }
}

Token* getCurrentToken(TokenList *tokenList)
{
    if (tokenList->idx >= tokenList->length)
        return NULL;
    return tokenList->tokens[tokenList->idx];
}

Token* peekNextToken(TokenList *tokenList)
{
    if (tokenList->idx + 1 >= tokenList->length)
        return NULL;
    Token* token =  tokenList->tokens[tokenList->idx + 1];
    return token;
}

void reorderTokenList(TokenList *tokenList)
{
    TokenList *tokenListTemp = shell_calloc(1, sizeof(TokenList));
    initTokenList(tokenListTemp);

    for (int i = tokenList->length - 1; i >= 0; i--) {
        Token *token = shell_calloc(1, sizeof(Token));
        initToken(token, tokenList->tokens[i]->type);
        token->setToken(token, tokenList->tokens[i]->type, tokenList->tokens[i]->value);
        tokenListTemp->addToken(tokenListTemp, token);
    }

    // Uninit the old tokenList and assign old tokenlist pointer to new one
    freeTokens(tokenList);
    tokenList->tokens = tokenListTemp->tokens;
    tokenList->length = tokenListTemp->length;
    tokenList->idx = 0;
    shell_free(tokenListTemp);
}

void clearTokenList(TokenList *tokenList)
{
    freeTokens(tokenList);
    tokenList->length = 0;
    tokenList->idx = 0;
}

void nextToken(TokenList *tokenList)
{
    tokenList->idx++;
}

int isNumber(char *input)
{
    ///< chech is number or not
    while (*input != '\0') {
        if (!isdigit(*input)) {
            return 0;
        }
        input++;
    }
    return 1;
}

