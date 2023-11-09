#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t hijos[4]; // Un arreglo para mantener los PIDs de los cuatro hijos

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

            // Usar un switch para determinar si el hijo es H2 o H3 y crear nietos
            switch (i) {
                case 1:
                    // Código para el hijo H2
                    {
                        pid_t nieto2 = fork();
                        if (nieto2 == -1) {
                            perror("Error al crear el nieto 2");
                            exit(1);
                        }
                        if (nieto2 == 0) {
                            // Este código se ejecuta en el nieto 2 (n2)
                            printf("Nieto 1 (N2) con PID %d\n", getpid());
                            exit(0);
                        }
                    }
                    break;
                case 2:
                    // Código para el hijo H3
                    {
                        pid_t nieto3 = fork();
                        if (nieto3 == -1) {
                            perror("Error al crear el nieto 3");
                            exit(1);
                        }
                        if (nieto3 == 0) {
                            // Este código se ejecuta en el nieto 3 (n3)
                            printf("Nieto 2 (N3) con PID %d\n", getpid());
                            exit(0);
                        }
                    }
                    break;
                default:
                    break;
            }

            exit(0);
        }
    }

    // El código a continuación se ejecuta solo en el proceso padre
    for (int i = 0; i < 4; i++) {
        int status;
        waitpid(hijos[i], &status, 0);
    }

    return 0;
}