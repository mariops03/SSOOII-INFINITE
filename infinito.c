#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
//#include <sys/prctl.h>
#include <string.h>

pid_t H1, H2, N2, H3, N3, H4, H3;
sigset_t maskPadre;
sigset_t maskTestigo;
int pidDisparo;
int temp = 1;
struct sigaction manejadoraT;
struct sigaction manejadoraA;
struct sigaction terminateHandler;
int contador=0;
void mascarabloqueo(){

    sigfillset(&maskPadre); 
    sigdelset(&maskPadre,SIGINT); 

}

void mascaratestigo(){

    sigfillset(&maskTestigo); 
    sigdelset(&maskTestigo,SIGUSR1); 
    sigdelset(&maskTestigo,SIGINT); 
    sigdelset(&maskTestigo,SIGALRM); 
    sigdelset(&maskTestigo,SIGTERM); 
    sigdelset(&maskTestigo,SIGKILL);
}
void funcionTestigo(){

    kill(pidDisparo,SIGUSR1);

}
void funcionAlarma(){
    //Tengo que matar a todos los procesos
    temp = -1;
    //printf("ALARMA, time DONE\n");
    printf("        @@@@@@@@@@@@@@@@                            @@@@@@@@@@@@@@@@@         \n");
    printf("     @@@@@@@@@@@@@@@@@@@@@@@                    @@@@@@@@@@@@@@@@@@@@@@@@,     \n");
    printf("   @@@@@@@@@(       @@@@@@@@@@@.             @@@@@@@@@@@         @@@@@@@@@.   \n");
    printf(" .@@@@@@@                @@@@@@@@@        @@@@@@@@@@                @@@@@@@.  \n");
    printf(" @@@@@@                     @@@@@@@@@   @@@@@@@@@                     @@@@@@@ \n");
    printf("@@@@@@                         @@@@@@@@@@@@@@@                        .@@@@@@ \n");
    printf("@@@@@@                           @@@@@@@@@@@                           @@@@@@ \n");
    printf("@@@@@@                              %d                              @@@@@@ \n",contador/2);
    printf("@@@@@@                           @@@@@@@@@@@                           @@@@@@ \n");
    printf("@@@@@@@                        @@@@@@@@@@@@@@@@                       @@@@@@@ \n");
    printf(" @@@@@@@                    @@@@@@@@@    @@@@@@@@@                   @@@@@@@  \n");
    printf("  @@@@@@@@               @@@@@@@@@.        @@@@@@@@@@              @@@@@@@@   \n");
    printf("    .@@@@@@@@@@*  .&@@@@@@@@@@@&              @@@@@@@@@@@@@&#&@@@@@@@@@@@     \n");
    printf("       @@@@@@@@@@@@@@@@@@@@@                     &@@@@@@@@@@@@@@@@@@@@@       \n");
    printf("           @@@@@@@@@@@@                                @@@@@@@@@@@&            \n");
    //printf("                                     %d\n",contador/2);


    kill(H1,SIGTERM);
    kill(H2,SIGTERM);
    kill(H3,SIGTERM);
    kill(H4,SIGTERM);
    waitpid(H1,NULL,0);
    waitpid(H2,NULL,0);
    waitpid(H3,NULL,0);
    waitpid(H4,NULL,0);
    //printf("Fin programa\n");
    exit(0);

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
void terminar() {

    if(N2>0){ //aqui entra si soy H2
        kill(N2,SIGINT);
        waitpid(N2,NULL,0);
        exit(0);
    }else if(N3>0){
        kill(N3,SIGINT);
        waitpid(N3,NULL,0);
        exit(0);
    }
    else{
        kill(getpid(),SIGINT);
    }
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
    if (H1 == 0) { 
        /* Child H1 code goes here */
        pidDisparo = getppid();
        while(temp){
            sigsuspend(&maskTestigo);

        }
    }else if(H4 == 0){ //se la pela
        pidDisparo = getppid();
        while(temp){
          sigsuspend(&maskTestigo);
        }
    }
     else if (H2 == 0) {
        N2 = fork();
        if (N2 == 0) { 
            /* Grandchild N2 code goes here */
            pidDisparo = H1;
            while(temp){
                 sigsuspend(&maskTestigo);

            }
        } else if(N2 > 0){ 
            /* Child H2 code goes here */
            pidDisparo = N2; 
            while(temp){
                sigsuspend(&maskTestigo);

            }
        }else if(N2 == -1){
            perror("Error en N2");
            return 1;
        }
    } else if(H3 == 0){
        N3 = fork();
        if(N3 == 0){
            /* Grandchild N3 code goes here */
            pidDisparo = H4;
            while(temp){
              sigsuspend(&maskTestigo);
            }
        }
        else if(N3 > 0){
            /* Child H3 code goes here */
            pidDisparo = N3;

            while(temp){

              sigsuspend(&maskTestigo);

            }
        }
        else  if(N3 == -1){
            perror("Error en N3");
            return 1;
        }
    }
    else if(H1 == -1 || H2 == -1 || H3 == -1 || H4 == -1){
        perror("Error en algun H");
        return 1;
    }
    else {
        /* Parent code goes here */
        alarm(25);
        pidDisparo = H2;
        kill(pidDisparo,SIGUSR1);
        while(temp){
            
            if(contador%2==0){ //si es par
                pidDisparo = H3;
                //system("clear");    
               
            }else{
                pidDisparo = H2;
            }
            
            sigsuspend(&maskTestigo);
            contador++;
        }
    }
}
