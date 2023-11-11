#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// Variable global para indicar que se ha presionado Ctrl+C
int ctrl_c_pressed = 0;

// Manejador de señales para Ctrl+C
void ctrl_c_handler(int signo) {
    if (signo == SIGINT) {
        ctrl_c_pressed = 1;
    }
}

int main() {
    int i;

    // Configurar el manejador de señales para Ctrl+C
    signal(SIGINT, ctrl_c_handler);

    // Crear 4 hijos
    for (i = 0; i < 4; i++) {
        pid_t child_pid = fork();

        // Verificar si hubo un error en la creación del proceso hijo
        if (child_pid == -1) {
            perror("Error al crear el proceso hijo");
            exit(EXIT_FAILURE);
        }
    }

        // Código que se ejecutará en el proceso hijo
        if (child_pid == 0) {
            printf("Soy el hijo %d con PID %d. Mi padre es %d.\n", i + 1, getpid(), getppid());

            // Si es el hijo H2 o H3, crear un hijo adicional
            if ((i == 1 || i == 2) && !ctrl_c_pressed) {
                pid_t grandchild_pid = fork();

                // Verificar si hubo un error en la creación del nieto
                if (grandchild_pid == -1) {
                    perror("Error al crear el nieto");
                    exit(EXIT_FAILURE);
                }

                // Código que se ejecutará en el nieto
                if (grandchild_pid == 0) {
                    printf("Soy el nieto de %d con PID %d. Mi padre es %d.\n", getpid(), getpid(), getppid());

                    // Bucle infinito hasta que se haga Ctrl+C
                    while (!ctrl_c_pressed) {
                        sleep(1);
                    }

                    printf("El nieto con PID %d ha terminado.\n", getpid());
                    exit(EXIT_SUCCESS);
                }

                // El proceso hijo (H2 o H3) espera a que su hijo (nieto) termine
                wait(NULL);
            }

            // Bucle infinito hasta que se haga Ctrl+C
            while (!ctrl_c_pressed) {
                sleep(1);
            }

            printf("El hijo con PID %d ha terminado.\n", getpid());
            exit(EXIT_SUCCESS);
        }
    }

    // El proceso padre espera a que se presione Ctrl+C
    while (!ctrl_c_pressed) {
        sleep(1);
    }

    // El proceso padre espera a que todos los hijos terminen
    for (i = 0; i < 4; i++) {
        wait(NULL);
    }

    printf("El proceso padre ha terminado.\n");

    return 0;
}
