#ifndef MEM_H
#define MEM_H

void* shell_calloc(int count, int size);
void* shell_malloc(int size);
void* shell_realloc(void *ptr, int size);
void shell_free(void *ptr);

#endif