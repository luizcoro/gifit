#ifndef WRITER_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "writer.h"

#endif

void* append_to_file(void *pargs)
{
    PthreadArgs args = *((PthreadArgs*) pargs);

    FILE *f = fopen(args.filename, "a");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    fprintf(f, "%s\n", args.str);

    fclose(f);

    free((PthreadArgs*) pargs);

    return NULL;
}

void create_writer_thread(char *filename, char *str)
{
    pthread_t thread;

    PthreadArgs *pargs = (PthreadArgs*) malloc(sizeof(PthreadArgs));

    strcpy(pargs->filename, filename);
    strcpy(pargs->str, str);

    pthread_create(&thread, NULL, append_to_file, (void*) pargs);
}

void create_exit_writer_thread(char *filename)
{
    create_writer_thread(filename, "q");
}

void create_zoom_writer_thread(char *filename, int x, int y)
{
    char str[20];
    sprintf(str, "%d %d", x, y);

    create_writer_thread(filename, str);
}

