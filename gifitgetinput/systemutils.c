#include "systemutils.h"

pid_t start_external_program(char *str)
{
    pid_t pid = fork();

    if(pid == 0){
        if(system(str) == 0)
            exit(0);
        else
            exit(1);
    }

    return pid;
}

void kill_external_program(pid_t pid)
{
    kill(pid, SIGTERM);
}

char* run_external_program(char *str)
{
    char *output = (char*) malloc(sizeof(char)); //allocate '\0'

    char buffer[140];
    FILE *in;

    if(! (in = popen(str, "r"))){
        exit(1);
     }

     while(fgets(buffer, sizeof(buffer), in) != NULL)
     {
        int size = strlen(output) + strlen(buffer) + 1;
        output = (char*) realloc(output, sizeof(char) * size);
        strcat(output, buffer);
     }

     pclose(in);

     return output;
}

