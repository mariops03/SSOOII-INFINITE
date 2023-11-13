#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <string.h>

pid_t H1, H2, N2, H3, N3, H4, H3;
pid_t pidpadre;
sigset_t maskPadre;
sigset_t maskTestigo;
int pidDisparo;
struct sigaction manejadoraT;
struct sigaction manejadoraA;
struct sigaction terminateHandler;
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
    sigdelset(&maskTestigo,SIGTERM); 
    sigdelset(&maskTestigo,SIGKILL);
   // sigprocmask(SIG_SETMASK,&maskTestigo,NULL);  // activamos la mascara
}
void funcionTestigo(){
    kill(pidDisparo,SIGUSR1);
    //exit(0);

}
void funcionAlarma(){
    //Tengo que matar a todos los procesos
    printf("ALARMA, time DONE\n");
    exit(0);
    //kill(H3,SIGTERM);
    

}
void alarmHandler(){
    manejadoraA.sa_handler = &funcionAlarma;
    manejadoraA.sa_flags = SA_RESTART;
    sigaction(SIGALRM,&manejadoraA,NULL);
}
void manejadoraTestigo(){
    manejadoraT.sa_handler = &funcionTestigo;
    manejadoraT.sa_flags = SA_RESTART;
    sigaction(SIGUSR1,&manejadoraT,NULL);
}
void terminar(){
    //SI tengo un hijo lo mato
    //aqui tienen que entrar H2 y H3
    
    //kill(temp,SIGINT);
  
    

}
void fterminateHandler(){
    terminateHandler.sa_handler = &terminar;
    terminateHandler.sa_flags = SA_RESTART;
    sigaction(SIGTERM,&terminateHandler,NULL);
}
int main() {
    mascarabloqueo();
    mascaratestigo();
    manejadoraTestigo();
    alarmHandler();
    fterminateHandler();
    sigprocmask(SIG_SETMASK,&maskPadre,NULL);  // activamos la mascara y la hereda el hijo

    //creamos a H1 y H2 a la vez
    

    (H1 = fork()) && (H4 = fork())&& (H2 = fork()) && (H3 = fork()) ; // Creates 4 children

   // sleep(10);
    if (H1 == 0) { //si le importa
        /* Child H1 code goes here */
        pidDisparo = getppid();
        while(1){
            sigsuspend(&maskTestigo);
          //  kill(pidDisparo,SIGUSR1);

        }
    }else if(H4 == 0){ //se la pela
        pidDisparo = getppid();
        while(1){
           // printf("PID disparo: %d\n",pidDisparo);
          sigsuspend(&maskTestigo);
            //kill(pidDisparo,SIGUSR1);
        }
    }
     else if (H2 == 0) {
        N2 = fork();
        if (N2 == 0) { // si le importa
            /* Grandchild N2 code goes here */
            pidDisparo = H1;
            while(1){
                 sigsuspend(&maskTestigo);
              //  kill(pidDisparo,SIGUSR1);

            }
        } else { //si le importa
            /* Child H2 code goes here */
            pidDisparo = N2; 
            while(1){
                sigsuspend(&maskTestigo);
                //kill(pidDisparo,SIGUSR1);

            }
        }
    } else if(H3 == 0){
        N3 = fork();
        if(N3 == 0){ //se la pela
            /* Grandchild N3 code goes here */
            pidDisparo = H4;
            while(1){
                //printf("Pid H4 %d",H4);
               // sleep(10);
               // printf("PID disparo: %d\n",pidDisparo);
              sigsuspend(&maskTestigo);
                //kill(pidDisparo,SIGUSR1);
            }
        }
        if(N3 > 0){ //se la pela
         // se la pela
            /* Child H3 code goes here */
            pidDisparo = N3;

            while(1){

              sigsuspend(&maskTestigo);

            }
        }
    }
    else if(H1 == -1 || H2 == -1 || H3 == -1 || H4 == -1){
        perror("fork");
        return 1;
    }
    else {
        /* Parent code goes here */
        alarm(25);
        pidDisparo = H2;
        kill(pidDisparo,SIGUSR1);
        while(1){
            
            if(contador%2==0){ //si es par
               // printf("voy para H2\n");
               // sleep(5);
                pidDisparo = H3;
            }else{
               // printf("voy para H3\n");
               // sleep(5);
                pidDisparo = H2;
            }
            printf("VUELTA: %d\n",contador/2);
           // printf("H1 %d H2 %d H3 %d H4 %dN2 %d N3 %d\n",H1,H2,H3,H4,N2,N3);
            sigsuspend(&maskTestigo);
            contador++;
        }
    }
}
