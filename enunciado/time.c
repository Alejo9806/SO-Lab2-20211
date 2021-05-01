#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    //Inicializcion de las variables y metodo gettimeofday() para saber cuanto tarda en ejecutarse una operacion.
    struct timeval current_time;
    struct timeval end_process_time;
    gettimeofday(&current_time, NULL);
    int build_process = fork();
  if (build_process  < 0) {
    // Si hay un fallo en el fork se termina el programa con la funcion exit()
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (build_process  == 0) {
      //Se ejecuta con exito y devuelve 0 al proceso hijo.
    char *arguments[argc];

    for (int i=1; i<argc; i++){
        arguments[i-1] = argv[i];
    }    

    arguments[argc-1] = NULL;
    execvp(arguments[0], arguments);//proporcionamos un vector de punteros a cadenas de caracteres terminadas en cero, que representan la lista de argumentos disponible para el nuevo programa.
    
  } else {
    //Proceso padre. 
    wait(NULL); // espera por la finalización de cualquier hijo y retorna el PID del proceso hijo.
    gettimeofday(&end_process_time, NULL);
    int seconds;
    int micro_seconds;
    seconds = end_process_time.tv_sec - current_time.tv_sec; //Se obtiene los segundos tardados de la operacion, utilizando las variables instanciadas al inicio de la struct timeval y utlizamos una propiedad para obetener el valor en segundos.
    micro_seconds = end_process_time.tv_usec - current_time.tv_usec;// Lo mismo pero usamos la propiedad para obtener su valor en microsegundos.
    printf("Segundos necesarios: %d Microsegundos necesarios: %d \n ", seconds,micro_seconds);//imprimimos los tiempos en consola
    }
  return 0;
}