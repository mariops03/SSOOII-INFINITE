#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

pid_t hijos[4]; // Arreglo para mantener los PIDs de los hijos

void sigalrm_handler(int signum) {
    for (int i = 0; i < 4; i++) {
        if (hijos[i] > 0) {
            kill(hijos[i], SIGKILL);
        }
    }
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sigalrm_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    // Configura el manejador de señales SIGALRM
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("Error al configurar el manejador de señales");
        exit(1);
    }

    pid_t padre = getpid();
    // Crear cuatro hijos
    for (int i = 0; i < 4; i++) {
        hijos[i] = fork();

        if (hijos[i] == -1) {
            perror("Error al crear un hijo");
            exit(1);
        }

        if (hijos[i] == 0) {
            // Este código se ejecuta en el hijo
            printf("H%d con PID %d\n", i + 1, getpid());

            // Si el hijo es H2 o H3, crear nietos
            if (i == 1 || i == 2) {
                pid_t nieto = fork();
                if (nieto == -1) {
                    perror("Error al crear el nieto");
                    exit(1);
                }
                if (nieto == 0) {
                    printf("Nieto con PID %d\n", getpid());
                    // El nieto se duerme
                    int status;
                    waitpid(getppid(), &status, 0);
                    waitpid(getpid(), &status, 0);
                }
            } else {
                // Si el hijo no tiene nietos, se duerme
                int status;
                waitpid(getpid(), &status, 0);
            }
        }
    }

    // Configura la alarma para activar SIGALRM después de 5 segundos solo si es el padre
    if (getpid() == padre) {
        alarm(5);
        for (int i = 0; i < 2; i++) {
            kill(hijos[i], SIGALRM); // Despierta a los hijos
        }
    }

    // Esperar a que todos los procesos hijos terminen
    for (int i = 0; i < 4; i++) {
        wait(NULL);
    }

    return 0;
}
