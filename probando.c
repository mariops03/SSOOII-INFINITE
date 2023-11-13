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
int pidDisparo;
struct sigaction manejadoraT;
pid_t pid;
int contador=0;
//mascara que bloquee todo menos SIGINT
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
    //tendremos que manejar SIGALARM (de momento no)
   // sigprocmask(SIG_SETMASK,&maskTestigo,NULL);  // activamos la mascara
}
void funcionTestigo(){
    kill(pidDisparo,SIGUSR1);
    //exit(0);

}

//manejadora de SIGALARM
//manejadora de SIGUSR1
void manejadoraTestigo(){

    manejadoraT.sa_handler = &funcionTestigo;
    manejadoraT.sa_flags = SA_RESTART;
    sigaction(SIGUSR1,&manejadoraT,NULL);
    printf("Manejadora de SIGUSR1\n");
}

void codigohijo(pid_t pidpadre) {

    sigsuspend(&maskTestigo);

    pidDisparo = pidpadre;

    while(1){
        //printf("PID disparo: %d\n",pidDisparo); 
        kill(pidDisparo,SIGUSR1);
        sigsuspend(&maskTestigo);
    }


}
void terminar(){
    //el padre mata al hijo
    kill(pid,SIGKILL);
    printf("HE MATADO AL HIJO\n");
    printf("TERMINO yo tmbn\n");
    exit(0);
}
void codigopadre(pid_t pidhijo) {
    printf("Soy el padre y empiezo el juego de testigos\n");
    pidDisparo = pidhijo;

    while(1){
       // printf("PID disparo: %d\n",pidDisparo); 
        printf("VUELTA: %d\n",contador++);
        if(contador==10) terminar();
        kill(pidDisparo,SIGUSR1);
        sigsuspend(&maskTestigo);
    }

}

int main() {
    mascarabloqueo(); //Inicialmente esta mascara
    mascaratestigo(); //mascara para pasar el testigo
    manejadoraTestigo(); //manejadora de SIGUSR1
    sigprocmask(SIG_SETMASK,&maskPadre,NULL);  // activamos la mascara y la hereda el hijo
 
    pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }
    //pause(); //lo use para comprobar que estaba ya creado el hijo y funcionaba la mascara
    if (pid > 0) {
        // Código del proceso padre
        printf("PIDPADRE: %d\n",getpid());
        printf("PIDHIJO: %d\n",pid);
        codigopadre(pid);
    } else if (pid == 0) {
        // Código del proceso hijo
        codigohijo(getppid());
    }

    return 0;
}