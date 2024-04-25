#ifndef TOKENIZER_H
#define TOKENIZER_H


#include "tokens.h"

typedef struct Token {
    TokenType type;
    char *value;
    void (*setToken)(struct Token *token, TokenType type, char *value);
}Token;

typedef struct TokenList {
    Token **tokens;
    int length;
    size_t idx;
    void (*addToken)(struct TokenList *tokenList, Token *token);
    void (*freeTokens)(struct TokenList *tokenList);
    void (*printTokens)(struct TokenList *tokenList);
    void (*nextToken)(struct TokenList *tokenList);
}TokenList;

///< Token

void initToken(Token *token, TokenType type);

void setupToken(Token *token);

///< TokenList

void initTokenList(TokenList *tokenList);

void uninitTokenList(TokenList *tokenList);

void tokenizer(char *input, TokenList *tokenList);

Token* getCurrentToken(TokenList *tokenList);

Token* peekNextToken(TokenList *tokenList);

void freeTokens(TokenList *tokenList);

void clearTokenList(TokenList *tokenList);

void printTokens(TokenList *tokenList);

void setuptokenList(TokenList *tokenList);

void nextToken(TokenList *TokenList);

///< Helpers

int isNumber(char *input);

void regulateInput(char *input);

void reorderTokenList(TokenList *tokenList);

#endif