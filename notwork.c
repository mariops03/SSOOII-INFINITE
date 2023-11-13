#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <string.h>

pid_t H1, H2, N2;
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
void set_process_name(const char* name) {
    prctl(PR_SET_NAME, name, NULL, NULL, NULL);
    printf("Nombre del proceso cambiado a %s\n", name);
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
    sigprocmask(SIG_SETMASK,&maskPadre,NULL);  // activamos la mascara y la hereda el hijo

   //EL padre tiene tiene de hijo a H1
    H1=fork();
    if(H1 > 0){
        //COdigo del padre
        //El padre tiene de hijo a H2
         H2=fork();
         
        //H2 tiene de hijo a N2
        if(H2 >0){
            //Codigo de H2
            set_process_name("H2");
            sigprocmask(SIG_SETMASK,&maskTestigo,NULL);  
            pidDisparo = N2;
            while(1){
                sigsuspend(&maskTestigo);
                printf("Disparo a N2\n");
                kill(pidDisparo,SIGUSR1);
                
            }
        }
        if(H2 == 0){
            //Codigo de N2
            set_process_name("N2");
            pidDisparo = H1;
            while(1){
                sigsuspend(&maskTestigo);
                printf("Disparo a H1\n");
                kill(pidDisparo,SIGUSR1);
                
            }
        }


    }
    if(H1 == 0){
        //Codigo de H1
        set_process_name("H1");
        sigprocmask(SIG_SETMASK,&maskTestigo,NULL);  
        //disparo al padre
        pidDisparo = getppid();
        while(1){
            sigsuspend(&maskTestigo);
            printf("Disparo al padre\n");
            kill(pidDisparo,SIGUSR1);

        }
    }
}
