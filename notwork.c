#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <string.h>

pid_t H1, H2, N2;
pid_t pidpadre;
sigset_t maskPadre;
sigset_t maskTestigo;
int pidDisparo;
struct sigaction manejadoraT;
struct sigaction manejadoraA;
pid_t pid;
int contador=0;
void mascarabloqueo(){

    sigfillset(&maskPadre); //cargamos la mascara con todas las señales
    sigdelset(&maskPadre,SIGINT); //quitamos SIGINT
   // sigprocmask(SIG_SETMASK,&maskPadre,NULL);  // activamos la mascara
}
//mascara para pasar el testigo
void mascaratestigo(){

    sigfillset(&maskTestigo); //cargamos la mascara con todas las señales
    sigdelset(&maskTestigo,SIGUSR1); //quitamos SIGUSR1
    sigdelset(&maskTestigo,SIGINT); //quitamos SIGINT
    sigdelset(&maskTestigo,SIGALRM); //quitamos SIGALRM
   // sigprocmask(SIG_SETMASK,&maskTestigo,NULL);  // activamos la mascara
}
void funcionTestigo(){
    kill(pidDisparo,SIGUSR1);
    //exit(0);

}

void process_code(const char* name) {
    printf("Soy el proceso %s con PID %d\n", name, getpid());
    //si el nombre es igual a N2
    if (strcmp(name, "N2") == 0) {
        printf("Soy nieto\n");
    }
}
void manejadoraTestigo(){
    manejadoraT.sa_handler = &funcionTestigo;
    manejadoraT.sa_flags = SA_RESTART;
    sigaction(SIGUSR1,&manejadoraT,NULL);
}

int main() {
    mascarabloqueo();
    mascaratestigo();
    manejadoraTestigo();
    sigprocmask(SIG_SETMASK,&maskPadre,NULL);  // activamos la mascara y la hereda el hijo

    //creamos a H1 y H2 a la vez
    
    (H1 = fork()) && (H2 = fork()); // Creates two children

    if (H1 == 0) {
        /* Child H1 code goes here */
        pidDisparo = getppid();
        while(1){
            sigsuspend(&maskTestigo);
            kill(pidDisparo,SIGUSR1);

        }
    } else if (H2 == 0) {
        N2 = fork();
        if (N2 == 0) {
            /* Grandchild N2 code goes here */
            pidDisparo = H1;
            while(1){
                sigsuspend(&maskTestigo);
                kill(pidDisparo,SIGUSR1);

            }
        } else {
            /* Child H2 code goes here */
            pidDisparo = N2;
            while(1){
                sigsuspend(&maskTestigo);
                kill(pidDisparo,SIGUSR1);

            }
        }
    } else {
        /* Parent code goes here */
        pidDisparo = H2;
        while(1){
           // printf("Vuelta: %d\n",contador++);
            printf("PID disparo: %d\n",pidDisparo);
           // printf("VUELTA: %d\n",contador++);
            kill(pidDisparo,SIGUSR1);
            sigsuspend(&maskTestigo);
        }
    }
}
