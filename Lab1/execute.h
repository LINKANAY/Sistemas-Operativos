/* Ejecuta comandos simples y pipelines.
 * No toca ningún comando interno.
 */

#ifndef EXECUTE_H
#define EXECUTE_H

#include "command.h"


void execute_pipeline(pipeline apipe);
/*
 * Ejecuta un pipeline, identificando comandos internos, forkeando, y
 *   redirigiendo la entrada y salida. puede modificar `apipe' en el proceso
 *   de ejecución.
 *   apipe: pipeline a ejecutar
 * Requires: apipe!=NULL
 */

char **scommand_to_args(scommand cmd);
/*
* Toma los argumentos de cmd y los pone en un arreglo de strings
*
*/

void execute_command(scommand cmd);
/*
* Ejecuta un pipe de un solo comando
*
*/

#endif /* EXECUTE_H */
