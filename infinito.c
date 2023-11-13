#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <string.h>
pid_t H1, H2, N2;

void set_process_name(const char* name) {
    prctl(PR_SET_NAME, name, NULL, NULL, NULL);
    printf("Nombre del proceso cambiado a %s\n", name);
}

void process_code(const char* name) {
    printf("Soy el proceso %s con PID %d\n", name, getpid());
    //si el nombre es igual a N2
    if(strcmp(name,"N2")==0){
        printf("Soy nieto\n");
    }
    while(1) {
        sleep(1);
    }
}

int main() {
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
            process_code("H2");
        }
        if(H2 == 0){
            //Codigo de N2
            set_process_name("N2");
            process_code("N2");
        }

    }
    if(H1 == 0){
        //Codigo de H1
        set_process_name("H1");
        process_code("H1");
    }
}
