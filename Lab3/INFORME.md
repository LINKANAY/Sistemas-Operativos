# Informe Laboratorio 3 - Sistemas Operativos 2024

## Primera parte

1. ¿Qué política de planificación utiliza xv6-riscv para elegir el próximo proceso a ejecutarse?

- La política de planificación utilizada en xv6 es simple, ejecutar cada proceso por turnos. Esta política es llamada round robin.

2. ¿Cúales son los estados en los que un proceso puede permanecer en xv6-riscv y qué los hace cambiar de estado?

- Los posibles estados en los que puede permanecer un proceso en xv6 y las funcioes que generan cambio de estado son:
| Estados | Funciones |
| ------- | --------- |
| unused  | freeproc(), procinit() |
| used    | allocproc() |
| sleep   | sleep() |
| runble  | userinit(), fork(), yield(), wakeup(), kill() |
| run     | scheduler() |
| zombie  | exit() |

3. ¿Qué es un quantum?

- Un quantum es un intervalo de tiempo donde un proceso se ejecuta en la CPU hasta que es interrumpido para que otro proceso pueda ejecutarse.

4. ¿Dónde se define en el código?

- El código esta definido en (kernel/start.c:63)

5. ¿Cuánto dura un quantum en xv6-riscv?

- Un quantum tiene una duración de 1000000 ciclos, aproximadamente 1/10 de segundo en qemu (kernel/start.c:69)

6. ¿En qué parte del código ocurre el cambio de contexto en xv6-riscv?

- El cambio de contexto en xv6-riscv ocurre en las funciones scheduler() y swtch(), donde scheduler es el planificador que decide qué proceso sigue, mientras que switch guarda el estado actual del proceso y carga el estado del siguiente proceso a ejecutar.

7. ¿En qué funciones un proceso deja de ser ejecutado?

- Las funciones donde un proceso deja de ser ejecutado son:

    | Funciones |
    |   -----   |
    |   sched   |
    |   sleep   |
    |   exit    |
    |   yield   |

8. ¿En qué funciones se elige el nuevo proceso a ejecutar?

- La función encargada es scheduler(), que se encuentra en (kernel/proc.c). scheduler() recorre la lista de procesos hasta que se encuentra uno que pueda correr y utiliza la función switch para cambiar a ese proceso.

9. ¿El cambio de contexto consume tiempo de un quantum?

- Sí consume tiempo. Durante el cambio de contexto, la CPU necesita realizar tareas de guardado y restauración de registros y otros datos de estado del proceso, lo cual ocupa tiempo de la CPU.

## Segunda Parte

### Experimento 1

Todas las mediciones realizadas están en el siguiente excel <https://docs.google.com/spreadsheets/d/1fOn0z6zN5NizW12LDfLsuVLh9fy-w6rxF4_k2iLI7d8/edit?usp=sharing>

1. Describa los parámetros de los programas cpubench e iobench para este experimento (o sea, los define al principio y el valor de N).

 En esta consigna se usa la cantidad de operaciones por tick como métrica. Los resultados de las mediciones están en la Hoja 1 del excel.
En iobench tenemos los siguientes parámetros:  
 IO_OPSIZE: establece el tamaño del buffer utilizado en las syscalls write() y read(). Al aumentar su tamaño, el tiempo necesario para completar un proceso I/O tiende a disminuir. En los resultados obtenidos, al usar como métrica la cantidad de operaciones de input/output por tick, se observa que un buffer más grande permite realizar más operaciones por tick, lo cual disminuye el tiempo total de ejecución de las operaciones.

IO_EXPERIMENT_LEN: Este parámetro indica la cantidad de operaciones input/output que realiza el experimento. Sea m = IO_EXPERIMENT_LEN, se realizan m operaciones de escritura y m operaciones de lectura, dando un total de 2 * m operaciones.

Vemos que los elapsed ticks aumentan a medida que IO_EXPERIMENT_LEN se hace más grande. Por el contrario, un IO_EXPERIMENT_LEN más pequeño resulta en un elapsed ticks más bajo.

N: indica la cantidad de veces que se ejecuta el experimento.

En cpubench tenemos los siguientes parámetros:

CPU_MATRIX_SIZE: Define el tamaño de las matrices. Cuanto más grande es su valor, más tiempo le lleva finalizar a la CPU.

Al incrementar el tamaño de la matriz de 128 X 128 a 256 X 256 se observa un aumento significativo en el tiempo promedio de ejecución medido en elapsed_ticks. La cantidad de operaciones por tick no tiene cambios significativos en este caso.

CPU_EXPERIMENT_LEN: Define la cantidad de veces que serán multiplicadas las matrices. Similar a la variable anterior, si el tamaño del experimento es muy grande, la cantidad de operaciones por tick no varí­a significativamente, pero el tiempo de ejecución aumenta.

Vemos que al ser el valor de cpu_experiment_len demasiado pequeño, los tiempos de ejecución se redondean a valores muy bajos.

N: Indica la cantidad de veces que se ejecuta el experimento.

### variables constantes:

| Variable           |   | valor |
|--------------------|---|-------|
| IO_OPSIZE          |   | 64    |
| IO_EXPERIMENTO_LEN |   | 512   |
| CPU_MATRIX_SIZE    |   | 128   |
| CPU_EXPERIMENT_LEN |   | 256   |
| MEASURE_PERIOD     |   | 1000  |
| N                  |   | 10    |

2. ¿Los procesos se ejecutan en paralelo?

- Ya que estamos trabajando con un sólo núcleo, los procesos no son ejecutados en paralelo. Lo que hace el sistema operativo es destinar los recursos durante un determinado tiempo a cada proceso. Este tiempo es llamado quantum y es igual para todos los procesos. Si un proceso no termina su ejecución antes de que se acabe su quantum, vuelve al final de la cola y debe esperar a que los recursos sean liberados. Este cambio entre procesos se realiza rápidamente, generando la ilusión de que los procesos son ejecutados paralelamente.

3. ¿En promedio, qué proceso o procesos se ejecutan primero?

- El proceso que se ejecuta primero es cpubench, se puede tomar en detalle la Hoja 2 del excel. Con lo cual se puede concluir de lo visualizado en la tabla del documento que por más que se pase primero como parámetro la función iobench() ésta queda después de la ejecución de cpubench().

4. ¿Cambia el rendimiento de los procesos iobound con respecto a la cantidad y tipo de procesos que se están ejecutando en paralelo? ¿Por qué?

- Sí cambia el rendimiento, debido a que al tener un sólo CPU habilitado los procesos estarán compitiendo por recursos. Por ello, entre más procesos, más largos serán los tiempos de ejecución. Además, si hay procesos de cpubench también en ejecución, estos estarán compitiendo por tiempo de CPU. Esto hace que los procesos iobench aumenten su tiempo de ejecución y causará aún más retrasos, esto se puede visualizar en la `Hoja 4`del excel.

5. ¿Cambia el rendimiento de los procesos cpubound con respecto a la cantidad y tipo de procesos que se están ejecutando en paralelo? ¿Por qué?

- El rendimiento sí­ cambia. Podemos analizar esto ejecutando los siguientes comandos:
  - cpubench 10
  - cpubench 10 &; iobench 10 > out.txt &; iobench 10 > out.txt &
  - cpubench 10 &; cpubench 10 > out.txt &; cpubench 10 > out.txt &
  Los resultados de las mediciones están en la Hoja 3 del excel.
Se observa que la cantidad de operaciones por tick varí­a dependiendo de cuantos procesos se ejecutan en paralelo y el tipo de los mismos. Cuando ejecutamos cpubench 10 obtenemos un promedio aproximado de 2849461 Kops por tick, mientras que cpubench 10 &; cpubench 10 &; cpubench 10 & nos da un promedio aproximado de 1659597 Kops por tick. Por último, cpubench 10 &; iobench 10 &; iobench 10 & parece quedar en el medio con un promedio aproximado de 2301911 Kops por tick. Vemos que si los procesos que se ejecutan en paralelo son cpubound, la cantidad de operaciones por tick es menor que cuando son iobound. Naturalmente, los tiempos de ejecución también varí­an. Si comparamos los casos en que los procesos ejecutados en paralelo son cpubound con aquellos en los que son iobound, vemos que los elapsed ticks de este último son menores. Esto se debe a que los procesos iobound ceden el cpu mientras esperan a que se completen las operaciones de entrada y salida. En este tiempo, el cpu queda libre para seguir ejecutando los procesos cpubound. En cambio, si todos los procesos que se ejecutan en paralelo son cpubound, el cpu es cedido sólo cuando se acaba el quantum del proceso.

6. ¿Es adecuado comparar la cantidad de operaciones de cpu con la cantidad de operaciones iobound?

- Comparar directamente la cantidad de operaciones de CPU con la cantidad de operaciones iobound no es adecuado. Al ser diferentes las métricas de medición de rendimiento de ambos procesos, comparar las operaciones a partir de la cantidad no sería correcto.

### Experimento 2

1. ¿Fue necesario modificar las métricas para que los resultados fueran comparables? ¿Por qué?

 Inicialmente, se definieron las métricas de rendimiento de la siguiente manera:

 - Para cpubench: metric = (total_cpu_kops/elapsed_ticks).
 - Para iobench: metric = (total_iops/ elapsed_ticks)

Sin embargo, al llegar al Experimento 2 y ejecutar con el nuevo quantum, las métricas resultaban constantemente en valores cercanos a 0, lo que dificultaba obtener información útil para el análisis. Por esta razón, actualizamos la fórmula de las métricas para obtener datos más acordes con los objetivos del experimento, quedando de la siguiente manera:
 
 - Para cpubench: metric = ((total_cpu_kops*1000)/elapsed_ticks)
 - Para iobench: metric = ((total_iops*1000)/ elapsed_ticks)
Con esta nueva definición de las métricas, realizamos nuevamente las mediciones del Experimento 1 y procedimos a implementar el análisis en el Experimento 2.

2. ¿Qué cambios se observan con respecto al experimento anterior?

- Respecto al experimento anterior (`Hoja 4` del excel) se obervan los siguientes cambios (`Hoja 7`):
quantum 10000:

- La métrica de cpubench se redujo en gran medida, pero los tiempos en ticks aumenta.
- La métrica de iobench tambien se redujo y aumentó los tiempos de ticks.
- La métrica de iobench ya no esperó hasta que terminara de ejecutarse todo el cpubench si no que se fue intercalando con esta.

quantum 1000:

- La métrica cpubench ya no se ejecuta en primer lugar, como en los dos casos anteriores.
- La métrica iobench se aproxima cada vez mas a 0.
- Todos los tiempos se dispararon en gran medida, causando demoras en la obtención de los datos de ejecución, por ejemplo en este caso tardó más de 20 minutos en ejecutar todo.

3. ¿Qué comportamientos se mantienen iguales?

- Respecto al experimento anterior (`Hoja 4`) con el nuevo experimento (`Hoja 7`) se mantuvo la disminución en la métrica y el alza en los tiempos de tick.

4. ¿Con un quantum más pequeño, se ven beneficiados los procesos iobound o los procesos cpubound?

- Respecto al experimento anterior (`Hoja 4`) solo se nota que los procesos iobench tienden a llegar a competir más con los cpubench y llegan a ejecutarse primero.

## Tercera Parte

En esta parte del laboratorio se procedio a implementar una rama en el repositorio de nombre mlfq.

Se generaron las siguiente reglas:

- MLFQ regla 3: Cuando un proceso se inicia, su prioridad sera maxima.
- MLFQ regla 4: Descender de prioridad cada vez que el proceso pasa todo un quantum realizando computo. Ascender de prioridad cada vez que el proceso se bloquea antes de terminar su quantum.

Se comprobó esto editando la salida de invacación del comando CTRL+P.

## Cuarta Parte

Por último se procede a implementar la planificación para que xv6 utilice MLFQ.

1. En primer lugar en agregar las últimas 2 reglas del mlfq:

- MLFQ regla 1: si el proceso A tiene mayor prioridad que el proceso B, corre A (y no B).
- MLFQ regla 2: Si dos procesos A y B tienen la misma prioridad, corre el que menos veces fue elegido por el planificador.

2. Repetir las mediciones de la segunda parte para ver las propiedades del nuevo planificador.

- Se procede a realizar las mediciones del experimento 2 con lo implementado del planificador (los resultados obtenidos estan en la `Hoja 8`del excel), obteniendo:
Para un quantum de 10000:
- Ejecutando el comando: iobench 10 &; iobench 10 > out.txt &: la métrica aumenta pero el tiempo transcurrido en ticks disminuye

- Ejecutando el comando: iobench 10 &; cpubench 10 &: la métrica disminuye levemente pero el tiempo transcurrido en ticks aumenta.

- Ejecutando el comando: cpubench 10 &; iobench 10 > out.txt &; iobench 10 > out.txt &: la métrica disminuye levemente pero el tiempo transcurrido en ticks aumenta.

Para un quntum de 1000:
- Todos los comandos ejecutados exceden lo 10 minutos ya sea para terminar o apenas para iniciar su ejecución, se procedió a abortar estas ejecuciones  ya que no es un tiempo razonable.

3. ¿Se puede producir `starvation`en el nuevo planificador?

- Sí, starvation puede ocurrir. Como el sistema prioriza a los procesos de I/O, los procesos en CPU pierden prioridad y se ejecutan con menos frecuencia, dado que los procesos con alta prioridad se ejecutan primero.

## Detalles finales 

### Hardware de uso

- Kernel: 5.15.0-124-generic x86_64 bits: 64 compiler: gcc v: 11.4.0 Desktop: Cinnamon 5.8.4

- CPU: dual core model: Intel Core i3-2310M bits: 64 type: MT MCP arch: Sandy Bridge rev: 7
    cache: L1: 128 KiB L2: 512 KiB L3: 3 MiB
  Speed (MHz): avg: 798 min/max: 800/2100 cores: 1: 798 2: 798 3: 798 4: 798 

- Graphics: Intel 2nd Generation Core Processor Family Integrated Graphics vendor: CLEVO/KAPOK

