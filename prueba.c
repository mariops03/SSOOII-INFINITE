#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile int contador_nietos_terminados = 0;  // Variable compartida

void nonada() {}

void manejador_SIGALRM(int signo) {
    // En este punto, han pasado los 25 segundos
    printf("Manejador SIGALRM en el proceso %d\n", getpid());

    // Enviar SIGTERM a todos los procesos hijos
    kill(0, SIGTERM);
}

void manejador_SIGCHLD(int signo) {
    // Incrementar el contador de nietos terminados
    contador_nietos_terminados++;
}

int nuevosleep(int segundos) {
    sigset_t conjunto_vacio, conjunto_SIGALRM, conjunto_viejo,
             conjunto_sin_SIGALRM;
    struct sigaction accion_nueva, accion_vieja;

    sigemptyset(&conjunto_SIGALRM);
    sigaddset(&conjunto_SIGALRM, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &conjunto_SIGALRM, &conjunto_viejo) == -1)
        return -1;

    conjunto_sin_SIGALRM = conjunto_viejo;
    sigdelset(&conjunto_sin_SIGALRM, SIGALRM);

    sigemptyset(&conjunto_vacio);
    accion_nueva.sa_handler = nonada;
    accion_nueva.sa_mask = conjunto_vacio;
    accion_nueva.sa_flags = 0;
    if (sigaction(SIGALRM, &accion_nueva, &accion_vieja) == -1)
        return -1;

    // Configurar el temporizador
    alarm(segundos);

    // Pausar el proceso hasta que se reciba la señal SIGALRM
    pause();

    // Restaurar la configuración de señales
    if (sigaction(SIGALRM, &accion_vieja, NULL) == -1)
        return -1;
    if (sigprocmask(SIG_SETMASK, &conjunto_viejo, NULL) == -1)
        return -1;

    return 0; /* Ejecución satisfactoria */
}

int main() {
    // Configurar manejador_SIGALRM para el proceso padre
    struct sigaction accion_padre, accion_nieto;
    accion_padre.sa_handler = manejador_SIGALRM;
    accion_padre.sa_flags = 0;
    sigaction(SIGALRM, &accion_padre, NULL);

    // Configurar manejador_SIGCHLD para el proceso padre
    accion_nieto.sa_handler = manejador_SIGCHLD;
    accion_nieto.sa_flags = 0;
    sigaction(SIGCHLD, &accion_nieto, NULL);

    // Configurar alarma para 25 segundos en el proceso padre
    alarm(25);

    // Crear procesos hijos
    for (int i = 0; i < 3; i++) {
        if (fork() == 0) {
            printf("Soy el proceso hijo %d\n", getpid());
            nuevosleep(25);


            // Crear dos hijos de los hijos (H2 y H3)
            for (int j = 0; j < 2; j++) {
                if (fork() == 0) {
                    printf("Soy el proceso nieto %d de H%d\n", getpid(), i + 1);
                    nuevosleep(25);
                }
            }
        }
    }

    // Esperar a que todos los nietos terminen
    while (contador_nietos_terminados < 6) {
        sleep(1); // Esperar un segundo antes de revisar nuevamente
    }

    // Esperar a que todos los procesos hijos terminen
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    printf("Soy el proceso padre %d y todos los nietos han muerto.\n", getpid());
    return 0;
}
