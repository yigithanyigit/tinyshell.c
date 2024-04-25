#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

void *shell_calloc(size_t count, size_t size)
{
    // Memory Allocator
    void *ptr = calloc(count, size);

    if (ptr == NULL) {
        printf("Memory Allocation Failed\n");
        exit(1);
    }
    return ptr;
}

void *shell_malloc(size_t size)
{
    // Memory Allocator
    void *ptr = malloc(size);
    if (ptr == NULL) {
        printf("Memory Allocation Failed\n");
        exit(1);
    }
    return ptr;
}

void* shell_realloc(void *ptr, size_t size)
{
    // Memory Reallocator
    void *new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        printf("Memory Reallocation Failed\n");
        exit(1);
    }
    return new_ptr;
}

void shell_free(void *ptr)
{
    // Memory Deallocator
    free(ptr);
}