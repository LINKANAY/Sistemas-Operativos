#include "execute.h"
#include "command.h"
#include "builtin.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "tests/syscall_mock.h"

void execute_pipeline(pipeline apipe){
    assert(apipe != NULL);

    if (pipeline_is_empty(apipe)){
        return;
    }

    scommand first_cmd = pipeline_front(apipe);
    if (builtin_is_internal(first_cmd)){
        builtin_run(first_cmd); /* Ejecuto comando interno */
        return;
    }

    int cont_args = pipeline_length(apipe);
    int pipefds[2 * (cont_args - 1)];
    pid_t pid;
    int* status = malloc(cont_args * sizeof(int));

    for (int i = 0; i < cont_args - 1; i++){
        if (pipe(pipefds + i * 2) == -1){
            perror("failed pipe\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < cont_args; i++){
        scommand cmd = pipeline_front(apipe);
        if (scommand_length(cmd) == 0) {
            pipeline_pop_front(apipe); // Ignora comandos vacios
        }

        pid = fork();
        if (pid == -1){
            perror("failed fork\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0){
            if (i > 0){
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) == -1){
                    perror("failed dup2 input");
                    exit(EXIT_FAILURE);
                }
            }
            if (i < cont_args - 1){
                if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) == -1){
                    perror("failed dup2 output");
                    exit(EXIT_FAILURE);
                }
            }

            /* Cerar pipes */
            for (int j = 0; j < 2 * (cont_args - 1); j++){
                close(pipefds[j]);
            }

            /* Ejecutar comando */
            execute_command(cmd);
            exit(EXIT_FAILURE);
        }

        /* Cerrar todos los pipes */
        if (i > 0){
            close(pipefds[(i - 1) * 2]);
        }
        if (i < cont_args - 1){
            close(pipefds[i * 2 + 1]);
        }
        status[i] = pid;
        pipeline_pop_front(apipe);
    }

    for (int i = 0; i < cont_args; i++){
        if (pipeline_get_wait(apipe)){
            waitpid(status[i], NULL, 0);
        }
    }
    free(status);
}

char **scommand_to_args(scommand cmd)
{
    assert(cmd != NULL);
    int len = scommand_length(cmd);
    char **args = calloc(len + 1, sizeof(char *));
    for (int i = 0; i < len; i++){
        args[i] = strdup(scommand_front(cmd));
        scommand_pop_front(cmd);
    }
    args[len] = NULL;
    return args;
}

void execute_command(scommand cmd) {
    char **args = scommand_to_args(cmd);        //paso los argumentos a un arreglo de strings
    char *redir_in = scommand_get_redir_in(cmd);        //obtener el nombre del archivo de redirección de entrada
    char *redir_out = scommand_get_redir_out(cmd);      //obtener el nombre del archivo de redirección de salida
    if (redir_in != NULL) {                         //redirección a in
        int fd_in = open(redir_in, O_RDONLY, 0644);       //abro el archivo "redir_in" solo para lectura
        dup2(fd_in, STDIN_FILENO);                  //duplico fd_in y lo asigno al file descriptor de stdin
        close(fd_in);                               //cierro fd_in
    } 
    if (redir_out != NULL) {     //redirección a out
        int fd_out = open(redir_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);     //abro el archivo "redir_out" para escritura, si no existe es creado. Tiene permisos de escritura y lectura solo para el propietario
        dup2(fd_out, STDOUT_FILENO);            //duplico fd_out y lo asigno al file descriptor de stdout
        close(fd_out);                          //cierro fd_out
    }
    if (execvp(args[0], args) == -1){
        printf("command not found\n");
        return;
    }                  //ejecuto el comando
}
