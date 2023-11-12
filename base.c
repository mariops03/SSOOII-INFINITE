#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/prctl.h>

#define SIGTESTIGO SIGUSR2

volatile sig_atomic_t terminado1 = 0;
volatile sig_atomic_t terminado2 = 0;
volatile sig_atomic_t terminado3 = 0;
volatile sig_atomic_t terminado4 = 0;
pid_t hijos[4]; // Un arreglo para mantener los PIDs de los cuatro hijos
pid_t nietos[2]; // Un arreglo para mantener los PIDs de los cuatro hijos
void manejador(int signum) {
    if (signum == SIGUSR1) {
        if (terminado1 == 0) {
            terminado1 = 1;
        } else if (terminado2 == 0) {
            terminado2 = 1;
        } else if (terminado3 == 0) {
            terminado3 = 1;
        } else if (terminado4 == 0) {
            terminado4 = 1;
        }
    }
    
}
int main(int argc, char* argv[]) {

    struct sigaction sa;
    sa.sa_handler = manejador;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    if (strcmp(argv[0], "H1") == 0) {
        printf("Soy el proceso H1 con PID %d\n", getpid());
        sigsuspend(&sa.sa_mask); // Esperar indefinidamente hasta que llegue una señal
        printf("Proceso H1 terminando.\n");
        exit(0);

    }

    if (strcmp(argv[0], "H2") == 0) {
        printf("Soy el proceso H2 con PID %d\n", getpid());
        //Tiene un hijo llamado N2
        nietos[1] = fork();
        if (nietos[1] == 0) {
            execlp("./base", "N2", NULL);
        }else{
            sigsuspend(&sa.sa_mask); // Esperar indefinidamente hasta que llegue una señal
            printf("Proceso H2 terminando.\n");
            exit(0);
        }
    }
    if (strcmp(argv[0], "H3") == 0) {
        printf("Soy el proceso H3 con PID %d\n", getpid());
        //Tiene un hijo llamado N3
        nietos[0]= fork();
        if (nietos[0] == 0) {
            execlp("./base", "N3", NULL);
        }else{
            sigsuspend(&sa.sa_mask); // Esperar indefinidamente hasta que llegue una señal
            printf("Proceso H3 terminando.\n");
            exit(0);
        }
        
    }
    if (strcmp(argv[0], "H4") == 0) {
        printf("Soy el proceso H4 con PID %d\n", getpid());
        sigsuspend(&sa.sa_mask); // Esperar indefinidamente hasta que llegue una señal
        printf("Proceso H4 terminando.\n");
        exit(0);
    }
    if(strcmp(argv[0], "N2") == 0){
        printf("Soy el proceso N2 con PID %d y soy nieto de Cachamin\n", getpid());
        sigsuspend(&sa.sa_mask); // Esperar indefinidamente hasta que llegue una señal
        printf("Proceso N2 terminando.\n");
        exit(0);
    }
    if(strcmp(argv[0], "N3") == 0){
        printf("Soy el proceso N3 con PID %d y soy nieto de Cachamin\n", getpid());
        sigsuspend(&sa.sa_mask); // Esperar indefinidamente hasta que llegue una señal
        printf("Proceso N3 terminando.\n");
        exit(0);
    }
    if(strcmp(argv[0], "./base") == 0){
        printf("SUPUESTAMENTE SOY PADRE\n");
    int pid;
    int i;
    for (i = 0; i < 4; i++) {
        hijos[i] = fork();
        if (hijos[i]> 0) {
            printf("SOy PADRE\n");
        } else {
            char process_name[10]; 
            sprintf(process_name, "H%d", i + 1);
            execlp("./base", process_name, NULL);
           // prctl("./base", process_name, NULL);
            //prctl(PR_SET_NAME, process_name, NULL,NULL,NULL);
        }
    }
    // Esperar a que terminen los procesos hijos
    alarm(5);
    //despertar a los hijos
    for (i = 0; i < 4; i++) {
        kill(hijos[i], SIGUSR1);
    }
    for(i=0;i<3;i++){
        kill(nietos[i], SIGUSR1);
    }
    return 0;
        
    }
   
}
