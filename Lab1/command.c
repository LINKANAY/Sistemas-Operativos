#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <glib.h>
#include "command.h"
#include "tests/syscall_mock.h"

struct scommand_s
{
    GSList *comms;       /*Comando y argumentos*/
    char *output;        /*Redirectorio de salida*/
    char *input;         /*Redirectorio de entrada*/
    unsigned int length; /*Tamalo del scommand*/
};

scommand scommand_new(void)
{
    scommand new_command = malloc(sizeof(struct scommand_s));
    assert(new_command != NULL);
    new_command->comms = NULL;
    new_command->input = NULL;
    new_command->output = NULL;
    new_command->length = 0;
    return new_command;
}

scommand scommand_destroy(scommand self)
{
    assert(self != NULL);
    g_slist_free_full(self->comms, g_free);
    free(self->output);
    free(self->input);
    free(self);
    self = NULL;
    return self;
}

void scommand_push_back(scommand self, char *argument)
{
    assert(self != NULL && argument != NULL);
    self->comms = g_slist_append(self->comms, argument);
    ++self->length;
}

void scommand_pop_front(scommand self)
{
    assert(self != NULL && !scommand_is_empty(self));
    gpointer killme_data = self->comms->data;   //  Obtiene el primer elemento del comando
    self->comms = g_slist_remove(self->comms, killme_data); // Elimina el elemento elegido del comando
    free(killme_data);  // Libera el espacio ocupado por el elemento eliminado
    --self->length;
}

void scommand_set_redir_in(scommand self, char *filename)
{
    assert(self != NULL);
    free(self->input);
    self->input = filename;
}
void scommand_set_redir_out(scommand self, char *filename)
{
    assert(self != NULL);
    free(self->output);
    self->output = filename;
}

bool scommand_is_empty(const scommand self)
{
    assert(self != NULL);
    return self->length == 0;
}

unsigned int scommand_length(const scommand self)
{
    assert(self != NULL);
    return self->length;
}

char *scommand_front(const scommand self)
{
    assert(self != NULL && !scommand_is_empty(self));
    return self->comms->data;
}

char *scommand_get_redir_in(const scommand self)
{
    assert(self != NULL);
    return self->input;
}
char *scommand_get_redir_out(const scommand self)
{
    assert(self != NULL);
    return self->output;
}

char *scommand_to_string(const scommand self)
{
    assert(self != NULL);
    unsigned int res_length = 0;
    unsigned int *comms_length = calloc(self->length, sizeof(unsigned int));
    GSList *comm_elem = self->comms;
    for (unsigned int i = 0; i < self->length; i++)
    {
        comms_length[i] = strlen((char *)comm_elem->data);
        res_length += comms_length[i] + 1;
        comm_elem = comm_elem->next;
    }

    unsigned int input_length = 0;
    unsigned int output_length = 0;
    if (self->input != NULL)
    {
        input_length = strlen(self->input);
        res_length += input_length + 3;
    }

    if (self->output != NULL)
    {
        output_length = strlen(self->output);
        res_length += output_length + 3;
    }

    if (res_length == 0)
    {
        free(comms_length);
        return calloc(1, sizeof(char));
    }

    char *res = calloc(res_length, sizeof(char));
    unsigned int copy_pos = 0;
    comm_elem = self->comms;
    for (unsigned int i = 0; i < self->length; i++)
    {
        char *comm_data = (char *)comm_elem->data;
        strcpy(res + copy_pos, comm_data);
        copy_pos += comms_length[i] + 1;
        res[copy_pos - 1] = ' ';
        comm_elem = comm_elem->next;
    }

    if (self->input != NULL)
    {
        res[copy_pos] = '<';
        res[copy_pos + 1] = ' ';
        copy_pos += 2;
        strcpy(res + copy_pos, self->input);
        copy_pos += input_length + 1;
        res[copy_pos - 1] = ' ';
    }

    if (self->output != NULL)
    {
        res[copy_pos] = '>';
        res[copy_pos + 1] = ' ';
        copy_pos += 2;
        strcpy(res + copy_pos, self->output);
        copy_pos += output_length + 1;
        res[copy_pos - 1] = ' ';
    }
    res[copy_pos - 1] = 0;

    free(comms_length);
    return res;
}
struct pipeline_s
{
    GSList *commands;  /* lista de comandos */
    bool wait;         /* indica si debo esperar para finalizar */
    unsigned int size; /* Cantidad de comandos */
};

bool pipeline_is_empty(const pipeline self)
{
    assert(self != NULL);
    return self->size == 0;
}

unsigned int pipeline_length(const pipeline self)
{
    assert(self != NULL);
    return self->size;
}

void pipeline_set_wait(pipeline self, const bool w)
{
    assert(self != NULL);
    self->wait = w;
}

bool pipeline_get_wait(const pipeline self)
{
    assert(self != NULL);
    return self->wait;
}

scommand pipeline_front(const pipeline self)
{
    assert(self != NULL && !pipeline_is_empty(self));

    scommand front = (scommand)self->commands->data; 

    assert(front != NULL);
    return front;
}

void pipeline_pop_front(pipeline self)
{
    assert(self != NULL && !pipeline_is_empty(self));
    scommand killme = (scommand)self->commands->data; /* Conseguir primer comando */

    // Eliminar el primer comando de la lista
    self->commands = g_slist_remove(self->commands, killme); /* Elimina el comando obtenido de self*/

    scommand_destroy(killme); /* Destruir primer comando */

    self->size--; /* Actualizar el tamaño */
}

void pipeline_push_back(pipeline self, scommand sc)
{
    assert(self != NULL && sc != NULL);
    self->commands = g_slist_append(self->commands, sc); /* Agregar comando al final de la lista */
    self->size++;                                        /* Actualizar el tamaño */
    assert(!pipeline_is_empty(self));
}

pipeline pipeline_new(void)
{
    pipeline newPipeline = malloc(sizeof(struct pipeline_s));
    newPipeline->commands = NULL;
    newPipeline->size = 0;
    newPipeline->wait = true;

    assert(newPipeline != NULL && pipeline_is_empty(newPipeline) && pipeline_get_wait(newPipeline));
    return newPipeline;
}

pipeline pipeline_destroy(pipeline self)
{
    assert(self != NULL);
    while (self->size != 0)
    {
        pipeline_pop_front(self); 
    }
    assert(pipeline_is_empty(self) || pipeline_get_wait(self));
    free(self);
    return NULL;
}

char *pipeline_to_string(const pipeline self)
{
    assert(self != NULL);

    GString *outline = g_string_new(""); /* linea vacia */
    GSList *current = self->commands;    /* cargar la lista de comandos */

    while (current != NULL)
    {
        scommand sc = (scommand)current->data; /* sacar comando */
        char *sc_str = scommand_to_string(sc); /* convertir comando a string */

        g_string_append(outline, sc_str); /* concatenar */

        if (current->next != NULL)
        {
            g_string_append(outline, " | ");
        }

        current = current->next; /* pasar al siguiente comando */
        free(sc_str);            /* Liberar la cadena */
    }

    if (!pipeline_get_wait(self))
    {
        g_string_append(outline, " &");
    }

    char *res = g_string_free(outline, FALSE); /* convertir de gstring a char */
    assert(pipeline_is_empty(self) || pipeline_get_wait(self) || strlen(res) > 0);
    return res;
}
