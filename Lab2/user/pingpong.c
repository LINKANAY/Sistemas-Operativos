#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include "stdbool.h"

bool prints = false;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("ERROR: Debe ingresar solo un número\n");
        return 1;
    }

    int N = atoi(argv[1]);

    if (N <= 1)
    {
        printf("ERROR: El número debe ser mayor a 1\n");
        return 1;
    }

    // Definimos los semáforos
    int sem_ping = sem_get(1);
    int sem_pong = sem_get(0);

    if (prints) // valores de ping y pong
    {
        printf("ping: %d, pong: %d\n", sem_ping, sem_pong);
    }
    

    // Crear proceso
    int pid = fork();

    if (pid == -1)
    {
        printf("ERROR: al crear el proceso\n");
        return 1;
    }
    else if (pid == 0)
    {
        // Proceso hijo
        if (prints)
        {
            printf("Soy el proceso hijo\n");
        }
        
        for (unsigned int i = 0; i < N; i++)
        {
            sem_down(sem_pong); // Esperar el semáforo de ping
            printf("\tpong\n");
            sem_up(sem_ping); // Liberar el semáforo de pong
        }
        return 0;
    }
    else
    {
        // Proceso padre
        if (prints)
        {
        printf("Soy el proceso padre\n");
        }
        
        for (unsigned int i = 0; i < N; i++)
        {
            sem_down(sem_ping); // Esperar el semáforo de pong
            printf("ping\n");
            sem_up(sem_pong); // Liberar el semáforo de ping
        }
    }

    wait(0);

    // Cerrar semáforos
    if (prints)
    {
        printf("Cerrando semáforos\n");
    }
    
    sem_close(sem_ping);
    sem_close(sem_pong);
    
    return 0;
}
