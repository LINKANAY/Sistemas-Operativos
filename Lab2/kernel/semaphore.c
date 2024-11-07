#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"

#include "stdbool.h"

#define MAX_SEM 256

bool prints = false;

struct semaphore
{
    bool in_use;
    int value;
    struct spinlock lock;
};

struct semaphore sem_arr[MAX_SEM];

int sem_open(int sem, int value)
{

    if (sem >= MAX_SEM || sem < 0)
    {
        printf("ERROR: sem value is invalid\n");
        return 0;
    }

    int ret = 0;

    acquire(&(sem_arr[sem].lock));

    if (sem_arr[sem].in_use == false)
    {
        sem_arr[sem].value = value;
        sem_arr[sem].in_use = true;
        ret = 1;
    }

    release(&(sem_arr[sem].lock));
    return ret;
}

int sem_close(int sem)
{

    if (sem >= MAX_SEM || sem < 0)
    {
        printf("ERROR: sem value is invalid\n");
        return 0;
    }
    acquire(&(sem_arr[sem].lock));

    sem_arr[sem].value = -1;
    sem_arr[sem].in_use = false;

    release(&(sem_arr[sem].lock));
    return 1;
}

int sem_up(int sem)
{


    acquire(&(sem_arr[sem].lock));

    if (sem_arr[sem].value == -1)
    {
        printf("ERROR: semaforo cerrado\n");
        release(&(sem_arr[sem].lock));
        return 0;
    }
    if (sem_arr[sem].value == 0)
    {
        wakeup(&sem_arr[sem]);
        sem_arr[sem].in_use = true;
    }
    ++(sem_arr[sem].value);

    release(&(sem_arr[sem].lock));
    return 1;
}

int sem_down(int sem)
{
    acquire(&(sem_arr[sem].lock));

    if (sem_arr[sem].value == -1)
    {
        printf("ERROR: semaforo cerrado\n");
        release(&(sem_arr[sem].lock));
        return 0;
    }

    while (sem_arr[sem].value == 0)
    {
        sleep(&(sem_arr[sem]), &(sem_arr[sem].lock));
    }

    --(sem_arr[sem].value);

    release(&(sem_arr[sem].lock));

    return 1;
}

int sem_get(int value)
{
    int sem = 0;

    while (sem < MAX_SEM && !sem_open(sem, value))
    {
        sem++;
    }

    return (sem == MAX_SEM) ? -1 : sem;
}