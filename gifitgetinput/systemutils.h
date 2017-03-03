#ifndef SYSTEMUTILS_H
#define SYSTEMUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>


pid_t start_external_program(char *str);

void kill_external_program(pid_t pid);

char* run_external_program(char *str);

#endif
