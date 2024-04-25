#ifndef CD_H
#define CD_H

int changeDir(char *path);

int getHomeEnv(char *home);

int getPwdEnv(char *pwd);

int updatePwdEnv(char *path);

int printPwd();

int initWorkingDir();

#endif