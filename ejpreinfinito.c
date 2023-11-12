#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/prctl.h>

sigset_t maskPadre;
sigset_t maskTestigo;

//mascara que bloquee todo menos SIGINT
void mascarabloqueo(){

    sigfillset(&maskPadre); //cargamos la mascara con todas las señales
    sigdelset(&maskPadre,SIGINT); //quitamos SIGINT
    sigprocmask(SIG_SETMASK,&maskPadre,NULL);  // activamos la mascara
}
//mascara para pasar el testigo
void mascaratestigo(){

    sigfillset(&maskTestigo); //cargamos la mascara con todas las señales
    sigdelset(&maskTestigo,SIGUSR1); //quitamos SIGUSR1
    sigdelset(&maskTestigo,SIGINT); //quitamos SIGINT
    //tendremos que manejar SIGALARM (de momento no)
    sigprocmask(SIG_SETMASK,&maskTestigo,NULL);  // activamos la mascara
}
void funcionTestigo(pid_t pidpasado){
    while(1){
        printf("Le paso el testigo a %d\n",pidpasado);

        kill(pidpasado,SIGUSR1);
        sigsuspend(&maskTestigo);
    }
}

//manejadora de SIGALARM
//manejadora de SIGUSR1
void manejadoraTestigo(){
    struct sigaction manejadoraT;
    manejadoraT.sa_handler = &funcionTestigo;
    manejadoraT.sa_flags = SA_RESTART;
    sigaction(SIGUSR1,&manejadoraT,NULL);
}

void codigohijo(pid_t pidpadre) {
    sigsuspend(&maskTestigo);
    printf("MAscara del juego ACTIVADA\n");
    while (1) {
        funcionTestigo(pidpadre); //le pasamos el pid del padre
    }
}

void codigopadre(pid_t pidhijo) {
    printf("Soy el padre y empiezo el juego de testigos\n");
    while (1) {
        funcionTestigo(pidhijo); //le pasamos el pid del hijo
    }
}

int main() {
    mascarabloqueo(); //Inicialmente esta mascara
    pid_t pid = fork();
    //aqui el hijo ya esta creado
    if (pid == -1) {
        perror("fork");
        return 1;
    }
    //pause(); //lo use para comprobar que estaba ya creado el hijo y funcionaba la mascara
    if (pid > 0) {
        // Código del proceso padre
        codigopadre(pid);
    } else if (pid == 0) {
        // Código del proceso hijo
        codigohijo(getppid());
    }

    return 0;
}