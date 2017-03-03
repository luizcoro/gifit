#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "timeutils.h"
#include "options.h"
#include "xutils.h"
#include "imlibutils.h"
#include "geometryutils.h"
#include "commandqueue.h"
#include "gmutils.h"

typedef struct
{
    char *filename;
} Screenshot;

typedef struct
{
    X11Data *x11data;
    CommandQueue *command_queue;

    bool *can_terminate;
} GlobalCommandHandlerArgs;

void *handle_global_commands(void *pargs)
{
    GlobalCommandHandlerArgs *args = (GlobalCommandHandlerArgs*) pargs;

    X11Data *x11data = args->x11data;
    CommandQueue *command_queue = args->command_queue;

    bool *can_terminate = args->can_terminate;

    while(!(* can_terminate))
    {
        if (x11data->window == None)
            handle_focus(x11data);
        else
            handle_event(x11data, command_queue);
    }

    return NULL;
}

void create_command_handle_thread(X11Data *x11data, CommandQueue *command_queue, bool *thread_can_terminate)
{
    pthread_t thread;

    GlobalCommandHandlerArgs *pargs = (GlobalCommandHandlerArgs*) malloc(sizeof(GlobalCommandHandlerArgs));

    pargs->x11data = x11data;
    pargs->command_queue = command_queue;
    pargs->can_terminate = thread_can_terminate;


    pthread_create(&thread, NULL, handle_global_commands, (void*) pargs);
}

char ** take_shots(GifitOpts *options, X11Data *x11data, Geometry *geometry, int *total_screenshots)
{

    bool thread_can_terminate = false;

    struct timeval now, later, diff, real_sleep;
    struct timeval sleep_time = micro_to_timeval((1.0f/options->screenshot_rate)*1000000);

    int i;
    char c = '0';
    char filename[1024];
    bool is_zoomed = false;

    char *strtok_context;
    char click_delimiter[] = " ";

    char zoom_in_command[] = "paplay sounds/zoom_in.ogg";
    char zoom_out_command[] = "paplay sounds/zoom_out.ogg";

    float zoom_step = timeval_to_float_sec(&sleep_time) * options->zoom_speed;

    CommandQueue *command_queue = create_command_queue(20);
    create_command_handle_thread(x11data, command_queue, &thread_can_terminate);

    Geometry current_geometry = *geometry;
    Geometry target_geometry = *geometry;

    char **screenshots = malloc(sizeof(char*) * 1024);

    if(options->show_keys_enable)
        start_external_program("screenkey");

    for(i = 0; c != 'q' && i < 1024; ++i)
    {
        if(timeval_subtract(&real_sleep, &sleep_time, &diff) == 0)
        {
            usleep(timeval_to_micro(&real_sleep));
        }

        timeval_now(&now);

        sprintf(filename, "%s/%05d.pnm", options->tmp_dir, i);

        current_geometry = zoom_towards(&current_geometry, &target_geometry, zoom_step);
        save_screenshot(filename, &current_geometry);

        screenshots[i] = (char*) malloc(strlen(filename) + 1);
        strcpy(screenshots[i], filename);

        printf("%dx%d+%d+%d\n", current_geometry.width, current_geometry.height, current_geometry.x, current_geometry.y);
        /* printf("\rPress [Ctrl+q] to stop recording\ttotal shots = %d", i); */
        fflush(stdout);

        while(!is_empty(command_queue))
        {
            Command command = front(command_queue);
            dequeue(command_queue);

            if(strcmp(command.message, "q") == 0)
            {
                c = 'q';
            }
            else
            {
                if(is_zoomed)
                {
                    start_external_program(zoom_out_command);

                    target_geometry = *geometry;
                }
                else
                {
                    start_external_program(zoom_in_command);

                    Click click;
                    click.x = atoi(strtok_r(command.message, click_delimiter, &strtok_context));
                    click.y = atoi(strtok_r(NULL, click_delimiter, &strtok_context));

                    target_geometry = zoom_by_level(geometry, &click, options->zoom_factor);
                }

                is_zoomed = !is_zoomed;
            }
        }

        timeval_now(&later);

        timeval_subtract(&diff, &later, &now);
    }

    *total_screenshots = i;


    if(options->show_keys_enable)
        start_external_program("killall screenkey");

    thread_can_terminate = true;
    sleep(1);
    return screenshots;
}

int main(int argc, char *argv[])
{

    GifitOpts options = parse_args(argc, argv);

    X11Data x11data = create_default_x11data();
    init_imlib();

    Geometry geometry = get_geometry_by_mode(0);


    int n_screenshots;

    char **screenshots = take_shots(&options, &x11data, &geometry, &n_screenshots);

    int i;

    init_gm();

    Image *images = load_images(screenshots, n_screenshots);

    Image *resized_images = resize_images(images, n_screenshots, geometry.width, geometry.height);

    unsigned int gif_speed = (options.gif_speed/options.screenshot_rate)*100;

    char filename[1024];
    sprintf(filename, "%s/%s.gif", options.tmp_dir, "out");

    struct timeval now, later, diff;

    timeval_now(&now);
    create_gif(resized_images, n_screenshots, gif_speed, filename);
    timeval_now(&later);

    timeval_subtract(&diff, &later, &now);

    printf("tempo=%d", timeval_to_mili(&diff));

    free_gm();



    return 0;
}


