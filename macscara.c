#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t terminado1 = 0;
volatile sig_atomic_t terminado2 = 0;

void manejador(int signum) {
    if (signum == SIGUSR1) {
        if (terminado1 == 0) {
            terminado1 = 1;
        } else if (terminado2 == 0) {
            terminado2 = 1;
        }
    }
}

int main() {
    int pid1, pid2;

    // Configurar el manejador de señales para los hijos
    struct sigaction sa;
    sa.sa_handler = manejador;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    // Crear el primer proceso hijo
    if ((pid1 = fork()) == 0) {
        // Código del primer proceso hijo
        printf("Proceso hijo 1 creado. PID: %d\n", getpid());
        sigsuspend(&sa.sa_mask); // Esperar indefinidamente hasta que llegue una señal
        printf("Proceso hijo 1 terminando.\n");
        exit(0);
    } else if (pid1 < 0) {
        perror("Error al crear el primer proceso hijo");
        exit(EXIT_FAILURE);
    }

    // Crear el segundo proceso hijo
    if ((pid2 = fork()) == 0) {
        // Código del segundo proceso hijo
        printf("Proceso hijo 2 creado. PID: %d\n", getpid());
        sigsuspend(&sa.sa_mask); // Esperar indefinidamente hasta que llegue una señal
        printf("Proceso hijo 2 terminando.\n");
        exit(0);
    } else if (pid2 < 0) {
        perror("Error al crear el segundo proceso hijo");
        exit(EXIT_FAILURE);
    }

    // Código del proceso padre
    printf("Procesos hijos creados. PID padre: %d\n", getpid());
    sleep(5); //para ver que churrula
    // Despertar a los procesos hijos enviando una señal
    kill(pid1, SIGUSR1);
    kill(pid2, SIGUSR1);

    return 0;
}
