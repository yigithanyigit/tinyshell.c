#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "tokenizer.h"
#include "mem.h"
#include "parser.h"
#include "cd.h"
#include "command.h"

CommandList commandList;
TokenList tokenList;

int main(int arhc, char **argv)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    initShell();

    int err = 0;
    while (printf("myshell>") && (read = getline(&line, &len, stdin)) != -1) {
        err = 0;
        regulateInput(line);
        tokenizer(line, &tokenList);
        reorderTokenList(&tokenList);
        //printTokens(&tokenList);
        err = parse(&tokenList);
        if (err == 2) // Only exit command will return 2 (unrecoverable error)
            goto end;
        clearTokenList(&tokenList);
    }

end:
    shell_free(line);
    uninitShell();
    return 0;
}

void initShell()
{
    initWorkingDir();
    initTokenList(&tokenList);
    initCommandList(&commandList);
}

void uninitShell()
{
    uninitTokenList(&tokenList);
    uninitCommandList(&commandList);
}