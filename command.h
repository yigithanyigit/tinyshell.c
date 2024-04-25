#ifndef COMMAND_H
#define COMMAND_H

typedef struct Command
{
    char* command;

} Command;

typedef struct CommandList
{
    Command** commands;
    int length;
    size_t idx;
    void (*addCommand)(struct CommandList *commandList, Command *command);
    void (*printCommands)(struct CommandList *commandList);
    void (*freeCommands)(struct CommandList *commandList);
    void (*nextCommand)(struct CommandList *commandList);
} CommandList;

Command* initCommand(Command *command, void** argv);

void uninitCommand(Command *command);

CommandList* initCommandList(CommandList *commandList);

void uninitCommandList(CommandList *commandList);

void addCommand(CommandList *commandList, Command *command);

void freeCommands(CommandList *commandList);

void printCommands(CommandList *commandList);

int executeCommand(void** argv);

#endif