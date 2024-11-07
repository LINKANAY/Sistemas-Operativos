#include <stdlib.h>
#include <stdbool.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"

static scommand parse_scommand(Parser p) {
    scommand command;
    arg_kind_t type;
    char* argument;
    bool pipe, background;

    command = scommand_new();

    parser_op_background(p, &background);
    parser_op_pipe(p, &pipe);
    if (pipe) {
        return NULL;
    }
    if (background) {
        return NULL;
    }

    while (!parser_at_eof(p)) {
        parser_skip_blanks(p);
        argument = parser_next_argument(p, &type);

        if (argument == NULL) {
            break;
        }
        if (type == ARG_NORMAL) {
            scommand_push_back(command, argument);
        } else if (scommand_get_redir_in(command) == NULL && type == ARG_INPUT) {
            scommand_set_redir_in(command, argument);
        } else if (scommand_get_redir_in(command) != NULL && type == ARG_INPUT) {
            command = scommand_destroy(command);
            break;
        } else if (scommand_get_redir_out(command) == NULL && type == ARG_OUTPUT) {
            scommand_set_redir_out(command, argument);
        } else if (scommand_get_redir_out(command) != NULL && type == ARG_OUTPUT) {
            command = scommand_destroy(command);
            break;
        } 
        
    }

    /* Devuelve NULL cuando hay un error de parseo */
    return command;
}

pipeline parse_pipeline(Parser p) {
    pipeline result = pipeline_new();
    scommand cmd = NULL;
    bool error, another_pipe=true, background;

    while (another_pipe) {
        cmd = parse_scommand(p);  
        error = (cmd == NULL);
        if (error) {
            pipeline_destroy(result);
            return NULL;
        }
        parser_skip_blanks(p);
        pipeline_push_back(result, cmd);
        parser_op_pipe(p, &another_pipe);
    }

    /* Opcionalmente un OP_BACKGROUND al final */
    parser_op_background(p, &background);
    pipeline_set_wait(result, !background);
   
    /* Tolerancia a espacios posteriores */
    bool garbage;
    parser_garbage(p, &garbage);/* Consumir todo lo que hay inclusive el \n */
    
    if (pipeline_length(result) == 1 && scommand_length(pipeline_front(result)) == 0) {
        pipeline_destroy(result);
        return NULL;
    }
    
    if (garbage) {
        result = pipeline_destroy(result);
        return NULL;
    } /* Si hubo error, hacemos cleanup */

    return result; 
}
