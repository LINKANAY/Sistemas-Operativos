#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"

#include <unistd.h>

#include "obfuscated.h"

static char curworkdir[1000];

static void show_prompt(void) {
    char* buf_ptr = getcwd(curworkdir, 1000);
    if (buf_ptr == NULL)
        printf("my_bash> ");
    else
        printf ("%s> ", curworkdir);
    fflush (stdout);
}

int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input;
    bool quit = false;

    input = parser_new(stdin);
    while (!quit) {
        //ping_pong_loop(NULL);
        show_prompt();
        pipe = parse_pipeline(input);

        if(pipe != NULL){
            printf("%s\n", pipeline_to_string(pipe)); /* Ver el pipeline como string */
            execute_pipeline(pipe);
            pipeline_destroy(pipe);
        }
        quit = parser_at_eof(input);
    }
    parser_destroy(input); 
    input = NULL;
    return EXIT_SUCCESS;
}