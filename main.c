#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"
#include "mem.h"
#include "parser.h"
#include "cd.h"
#include "command.h"

CommandList commandList;

int main(int arhc, char **argv)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    initWorkingDir();

    TokenList tokenlist;
    initTokenList(&tokenlist);

    initCommandList(&commandList);

    int err = 0;
    while ((read = getline(&line, &len, stdin)) != -1) {
        err = 0;
        regulateInput(line);
        tokenizer(line, &tokenlist);
        reorderTokenList(&tokenlist);
        err = parse(&tokenlist);
        if (err != 0)
            goto end;
        clearTokenList(&tokenlist);
    }

end:
    shell_free(line);
    uninitTokenList(&tokenlist);
    uninitCommandList(&commandList);
    return 0;
}