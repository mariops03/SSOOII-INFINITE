#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

pid_t H1, H2, N2, H3, N3, H4;
sigset_t maskPadre;
sigset_t maskTestigo;
int pidDisparo;
int temp = 1;
struct sigaction manejadoraT;
struct sigaction manejadoraA;
struct sigaction terminateHandler;
int contador = 0;

void mascarabloqueo() {
    if (sigfillset(&maskPadre) == -1) {
        perror("Error en sigfillset");
        exit(-1);
    }
    if (sigdelset(&maskPadre, SIGINT) == -1) {
        perror("Error en sigdelset");
        exit(-1);
    }
}

void mascaratestigo() {
    if (sigfillset(&maskTestigo) == -1) {
        perror("Error en sigfillset");
        exit(-1);
    }
    if (sigdelset(&maskTestigo, SIGUSR1) == -1 || sigdelset(&maskTestigo, SIGINT) == -1 ||
        sigdelset(&maskTestigo, SIGALRM) == -1 || sigdelset(&maskTestigo, SIGTERM) == -1 ||
        sigdelset(&maskTestigo, SIGKILL) == -1) {
        perror("Error en sigdelset");
        exit(-1);
    }
}

void funcionTestigo() {
    if (kill(pidDisparo, SIGUSR1) == -1) {
        perror("Error en kill");
        exit(-1);
    }
}

void funcionAlarma() {
    temp = -1;
    printf("        @@@@@@@@@@@@@@@@                            @@@@@@@@@@@@@@@@@         \n");
    printf("     @@@@@@@@@@@@@@@@@@@@@@@                    @@@@@@@@@@@@@@@@@@@@@@@@,     \n");
    printf("   @@@@@@@@@(       @@@@@@@@@@@.             @@@@@@@@@@@         @@@@@@@@@.   \n");
    printf(" .@@@@@@@                @@@@@@@@@        @@@@@@@@@@                @@@@@@@@  \n");
    printf(" @@@@@@                     @@@@@@@@@   @@@@@@@@@                     @@@@@@% \n");
    printf("@@@@@@                         @@@@@@@@@@@@@@@                        .@@@@@@ \n");
    printf("@@@@@@                           .@@@@@@@@@@                           @@@@@@ \n");
    printf("@@@@@@                           %d                             @@@@@@ \n", contador / 2);
    printf("@@@@@@                           @@@@@@@@@@@                           @@@@@@ \n");
    printf("@@@@@@@                        @@@@@@@@@@@@@@@@                       @@@@@@@ \n");
    printf(" @@@@@@@                    @@@@@@@@@    @@@@@@@@@                   @@@@@@@  \n");
    printf("  @@@@@@@@               @@@@@@@@@.        @@@@@@@@@@              @@@@@@@@   \n");
    printf("    .@@@@@@@@@@*  .&@@@@@@@@@@@&              @@@@@@@@@@@@@&#&@@@@@@@@@@@     \n");
    printf("       @@@@@@@@@@@@@@@@@@@@@                     &@@@@@@@@@@@@@@@@@@@@@       \n");
    printf("           @@@@@@@@@@@@                                @@@@@@@@@@@&            \n");

    if (kill(H1, SIGTERM) == -1) {
        perror("Error en kill (H1)");
    }
    if (kill(H2, SIGTERM) == -1) {
        perror("Error en kill (H2)");
    }
    if (kill(H3, SIGTERM) == -1) {
        perror("Error en kill (H3)");
    }
    if (kill(H4, SIGTERM) == -1) {
        perror("Error en kill (H4)");
    }

    waitpid(H1, NULL, 0);
    waitpid(H2, NULL, 0);
    waitpid(H3, NULL, 0);
    waitpid(H4, NULL, 0);

    exit(0);
}

void alarmHandler() {
    manejadoraA.sa_handler = &funcionAlarma;
    manejadoraA.sa_flags = SA_RESTART;
    if (sigaction(SIGALRM, &manejadoraA, NULL) == -1) {
        perror("Error en sigaction (SIGALRM)");
        exit(-1);
    }
}

void manejadoraTestigo() {
    manejadoraT.sa_handler = &funcionTestigo;
    manejadoraT.sa_flags = SA_RESTART;
    if (sigaction(SIGUSR1, &manejadoraT, NULL) == -1) {
        perror("Error en sigaction (SIGUSR1)");
        exit(-1);
    }
}

void terminar() {
    if (N2 > 0) { //aqui entra si soy H2
        if (kill(N2, SIGINT) == -1) {
            perror("Error en kill (N2)");
        }
        waitpid(N2, NULL, 0);
        exit(0);
    } else if (N3 > 0) {
        if (kill(N3, SIGINT) == -1) {
            perror("Error en kill (N3)");
        }
        waitpid(N3, NULL, 0);
        exit(0);
    } else {
        if (kill(getpid(), SIGINT) == -1) {
            perror("Error en kill (padre)");
        }
    }
}

void fterminateHandler() {
    terminateHandler.sa_handler = &terminar;
    terminateHandler.sa_flags = SA_RESTART;
    if (sigaction(SIGTERM, &terminateHandler, NULL) == -1) {
        perror("Error en sigaction (SIGTERM)");
        exit(-1);
    }
}

int main() {
    mascarabloqueo();
    mascaratestigo();
    manejadoraTestigo();
    alarmHandler();
    fterminateHandler();
    if (sigprocmask(SIG_SETMASK, &maskPadre, NULL) == -1) {
        perror("Error en sigprocmask");
        exit(-1);
    }

    (H1 = fork()) && (H4 = fork()) && (H2 = fork()) && (H3 = fork()); // Creates 4 children

    if (H1 == 0) {
        /* Child H1 code goes here */
        pidDisparo = getppid();
        while (temp) {
            if (sigsuspend(&maskTestigo) == -1 && errno != EINTR) {
                perror("Error en sigsuspend (H1)");
                exit(-1);
            }
        }
    } else if (H4 == 0) {
        /* Child H4 code goes here */
        pidDisparo = getppid();
        while (temp) {
            if (sigsuspend(&maskTestigo) == -1 && errno != EINTR) {
                perror("Error en sigsuspend (H4)");
                exit(-1);
            }
        }
    } else if (H2 == 0) {
        N2 = fork();
        if (N2 == 0) {
            /* Grandchild N2 code goes here */
            pidDisparo = H1;
            while (temp) {
                if (sigsuspend(&maskTestigo) == -1 && errno != EINTR) {
                    perror("Error en sigsuspend (N2)");
                    exit(-1);
                }
            }
        } else if (N2 > 0) {
            /* Child H2 code goes here */
            pidDisparo = N2;
            while (temp) {
                if (sigsuspend(&maskTestigo) == -1 && errno != EINTR) {
                    perror("Error en sigsuspend (H2)");
                    exit(-1);
                }
            }
        } else if (N2 == -1) {
            perror("Error en fork (N2)");
            exit(-1);
        }
    } else if (H3 == 0) {
        N3 = fork();
        if (N3 == 0) {
            /* Grandchild N3 code goes here */
            pidDisparo = H4;
            while (temp) {
                if (sigsuspend(&maskTestigo) == -1 && errno != EINTR) {
                    perror("Error en sigsuspend (N3)");
                    exit(-1);
                }
            }
        } else if (N3 > 0) {
            /* Child H3 code goes here */
            pidDisparo = N3;
            while (temp) {
                if (sigsuspend(&maskTestigo) == -1 && errno != EINTR) {
                    perror("Error en sigsuspend (H3)");
                    exit(-1);
                }
            }
        } else if (N3 == -1) {
            perror("Error en fork (N3)");
            exit(-1);
        }
    } else if (H1 == -1 || H2 == -1 || H3 == -1 || H4 == -1) {
        perror("Error en algun fork (H)");
        exit(-1);
    } else {
        /* Parent code goes here */
        alarm(25);
        pidDisparo = H2;
        if (kill(pidDisparo, SIGUSR1) == -1) {
            perror("Error en kill (H2)");
            exit(-1);
        }
        while (temp) {
            if (contador % 2 == 0) { //si es par
                pidDisparo = H3;
            } else {
                pidDisparo = H2;
            }

            if (sigsuspend(&maskTestigo) == -1 && errno != EINTR) {
                perror("Error en sigsuspend (padre)");
                exit(-1);
            }

            contador++;
        }
    }
}
