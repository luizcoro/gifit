typedef struct
{
    char filename[100];
    char str[20];
} PthreadArgs;

void* append_to_file(void *pargs);

void create_writer_thread(char *filename, char *str);

void create_exit_writer_thread(char *filename);

void create_zoom_writer_thread(char *filename, int x, int y);

