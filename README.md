## P1
In preemptive priority scheduling, a higher priority process can execute ahead of an already executing lower priority process. If lower priority process keeps waiting for higher priority processes, starvation occur

Una desventaje de preemptive priority scheduling es que un proceso con prioridad alta, puede ejecturase por sobre otro proceso que se esta ejectuando con menor prioridad. Y si, despues le sigen proceso con mayor prioridad entonces ocurre lo que se llama "starvation", donde el proceso de menor prioridad no se ejecuta en mucho tiempo. 
Una manera de solucionar esto es que ademas de ordenar por prioridad definida anteriormente, que exista un coeficiente que le entrega mayor prioridad a un proceso que debe para de ejecutarse por quantum. Con esto, el proceso "gana" prioridad por haberse ejectuado con anterioridad y no pasa "starvation".

Otra desventaja es que si el quantum es muy grande, entonces el scheduler es basicamente non-preemtive y se puede quedar en un proceso por mucho tiempo.

Una manera de solucionar esta desventaja, es tener un estimador estadistico de el valor de quantum para cada proceso. Analizar los datos del usuario de los tiempos de CPU por proceso y computar un estimador que sea representativo.
Entonces tendriamos una funcion Q(x) , que dado un proceso x, nos entrega un quantum estimado para ese proceso.

## P2
Para que la implementacion soporte multicores, la idea es que el scheduler tenga un array de largo = numero de cores. 
Despues para poder elegir un core, se puede tener un Min heap que guarde en la raiz el core "mas cercano" disponible. Con mas cercano me refiero al core con menor indice.
Se tiene un arreglo para que se pueda accedar en O(1) desde el valor que nos entrega el heap.

Los procesos se eljien usando una cola de prioridades, que ojala se implemente como heap por temas de eficiencia. Usando las mismas decisiones que se tomaron en esta tarea para elegir procesos.

Entonces si un proceso quiere ejecturase, le pide al heap el proximo core disponible.
- Si existe un core disponible, este proceso se ejecuta en ese core.
- Si no debe esperar.

Cuando un proceso termina de ejecutarse en un core, le manda ese "id" de el core al heap, y este se guarda manteniendo la propiedad de heap.

## P3




