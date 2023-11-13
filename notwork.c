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
pid_t hijos[2];
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
    //prctl(PR_SET_NAME, name, NULL, NULL, NULL);
    printf("Bien creado:  %s\n", name);
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

    int i;
    for(i=0;i<2;i++){
        hijos[i]=fork();

        if(hijos[i]==0){
            //Codigo de los hijos
            if(i==0){
                //Codigo de H1
                set_process_name("H1");
                sigprocmask(SIG_SETMASK,&maskTestigo,NULL);  
                //disparo al padre
                pidDisparo = getppid();
                while(1){
                    sigsuspend(&maskTestigo);
                   // printf("Disparo al padre\n");
                    kill(pidDisparo,SIGUSR1);

                }
            }
            if(i==1){
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
        }
    }
         //COdigo del padre
        //El padre tiene de hijo a H2
            printf("Soy el padre y mi hijo es H1: %d H2:%d\n",H1, H2);
         //impriro el pid de H2
            printf("Soy el padre y mi hijo es %d\n",H2);
            //imprimo el pid de H1
            printf("Soy el padre y mi hijo es %d\n",H1);
            sleep(2);
            while(1){
                pidDisparo = H2;

               // printf("Soy el padre y disparo a H2 : %d\n",pidDisparo);
                printf("Circulo %d\n",contador);
                contador++;
                kill(pidDisparo,SIGUSR1);
                sigsuspend(&maskTestigo);
            }
}

  