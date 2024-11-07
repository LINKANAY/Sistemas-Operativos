#include "builtin.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   /* Para chdir() */
#include "tests/syscall_mock.h"

/* Comandos manuales */
static const char *BUILTIN[] = {"cd", "help", "exit"};



bool builtin_is_internal(scommand cmd){
    assert(cmd != NULL);
    for(unsigned int i = 0; i < 3; i++){
        if(!strcmp(scommand_front(cmd), BUILTIN[i])){
            return true;
        }
    }
    return false;
}


bool builtin_alone(pipeline p){
    assert(p != NULL);
    if (pipeline_length(p) != 1){
        return false;
    } else if(builtin_is_internal(pipeline_front(p))){
        return false;
    }
    return true;
}


void builtin_run(scommand cmd){
    assert(builtin_is_internal(cmd));
    char *command;
    if (!strcmp(scommand_front(cmd), BUILTIN[0])){
        scommand_pop_front(cmd);        //retiro el nombre del comando, debería quedar el path para cd
        
        command = scommand_to_string(cmd);      //transformo los argumentos a char para luego usarlos en chdir()
        int redirect = chdir(command); 
        if (redirect != 0) {
            perror("Error");     //si hubo un error en el cambio de directorio, termina
        }

    }else if (!strcmp(scommand_front(cmd), BUILTIN[1])) {
        printf("Built-in instruccions manual by Giuliano Frizzera, Juan Cruz Brocal & Sebastian Sierra Sierra.\n"
                "Available built-in instructions:\n"
                "1) 'cd <path>': Change the current directory.\n"
                "2) 'help': Opens the manual for built-in commands.\n"
                "3) 'exit': Close and exit the Bash.\n");
    }else if (!strcmp(scommand_front(cmd), BUILTIN[2])) {
        exit(EXIT_SUCCESS);
    }
}