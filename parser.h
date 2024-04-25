#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"

void consume(TokenList *tokenList, TokenType type);

int parse(TokenList *tokenList);

#endif