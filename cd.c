#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "cd.h"

int changeDir(char *path)
{

    if (path == NULL)
    {
        char home[1024];
        if (getHomeEnv(home) != 0)
            goto error;
        printf("Home: %s\n", home);
        if (chdir(home) != 0)
        {
            perror("cd");
            goto error;
        }
        if (updatePwdEnv(home) != 0)
            goto error;
        goto success;
    }

    char str[1024]; // 1024 is max path length
    memset(str, 0, 1024);

    if(path[0] != '/'){
        if (strcmp(path, ".") == 0) {
            if (getPwdEnv(str) != 0)
                goto error;
            goto change;
        } else if (strcmp(path, "..") == 0) {
            if (getPwdEnv(str) != 0)
                goto error;
            char *last = strrchr(str, '/');
            if (last != NULL) {
                *last = '\0';
            }
            goto change;
        } else if (strcmp(path, "~") == 0) {
            if (getHomeEnv(str) != 0)
                goto error;
            goto change;
        } else if (isalnum(path[0])) {
            getPwdEnv(str);
        } else {
            while (path[0] == '.' && path[1] == '/') {
                if (getPwdEnv(str) != 0)
                    goto error;
                path += 2;
            }
            while (path[0] == '.' && path[1] == '.' && path[2] == '/') {
                if (getPwdEnv(str) != 0)
                    goto error;
                char *last = strrchr(str, '/');
                if (last != NULL) {
                    *last = '\0';
                }
                path += 3;
                printf("Path: %s\n", path);
            }
        }
        strcat(str, "/");
        strcat(str, path);

    } else {
        strcpy(str, path);
    }

change:
    if(chdir(str) != 0) {
        perror("cd");
        goto error;
    }
    if(updatePwdEnv(str) != 0)
        goto error;
success:
    return 0;
error:
    return 1;
}

int getHomeEnv(char *home)
{
    char* env = getenv("HOME");
    if(env == NULL){
        perror("cd");
        return 1;
    }
    strcpy(home, env);
    return 0;
}

int getPwdEnv(char *pwd)
{
    char* env = getenv("PWD");
    if(env == NULL){
        perror("cd");
        return 1;
    }
    strcpy(pwd, env);
    return 0;
}

int updatePwdEnv(char *path)
{
    if(setenv("PWD", path, 1) != 0){
        perror("cd");
        return 1;
    }
    //printf("PWD: %s\n", getenv("PWD"));
    return 0;
}

int initWorkingDir()
{
    char path[1024];
    if(getcwd(path, 1024) == NULL){
        perror("cd");
        return 1;
    }
    return updatePwdEnv(path);
}

int printPwd()
{
    char path[1024];
    if(getPwdEnv(path) != 0)
        return 1;
    printf("PWD: %s\n", path);
    return 0;
}