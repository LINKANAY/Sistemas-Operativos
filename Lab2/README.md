# Laboratorio 2 - Sistemas Operativos UNC - 2024

## Objetivos
- El trabajo fue realizado en la versión RISC-V de XV6. 
- Se implementó un sistema de semáforos para espacio de usuario, que sirven como mecanismo de sincronización entre procesos.
- Se implementó un programa "pingpong" para espacio de usuario.

## Desarrollo

### Syscalls

Se implementaron 4 syscalls basicas:
- sem_open().
- sem_close().
- sem_up().
- sem_down().
Además fue agregada una syscall auxiliar:
- sem_get().

Se optó por crear el archivo semaphore.c que contiene todas sus implementaciones.
Fueron modificados los siguientes archivos:
- Se asigna un número a cada system call en **syscall.h**.
- En **syscall.c** se actualiza la tabla de system calls.
- En **user.h** se declaran los prototipos para espacio de usuario.
- En **defs.h** se declaran los prototipos para espacio de kernel.
- **sysproc.c** contiene las implementaciones a nivel de kernel.

### pingpong

- El programa "pingpong" sincroniza la escritura de "ping" y "pong" utilizando el sistema de semáforos implementado.
- Es llamado con el comando pingpong, que toma como argumento un número entero N correspondiente a la cantidad de veces que se imprimirá "ping" y "pong" en pantalla.

## Autores

- Giuliano Frizzera
- Sebastián Sierra Sierra
- Juan Cruz Brocal

## Detalles sobre XV6
xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.

Se clona repositorio de xv6
https://github.com/mit-pdos/xv6-riscv.git
Se instala qemu
sudo apt-get install qemu-system-riscv64 gcc-riscv64-linux-gnu
