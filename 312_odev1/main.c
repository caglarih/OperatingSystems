/*
 * Ihsan Burak Caglar
 * 150120057
 *
 * Blg312 Isletim Sistemleri
 * Odev 1
 * Soru 2
 */

#include <stdio.h>
#include <unistd.h>
#include  <sys/types.h>
#include  <sys/wait.h>

int main(int argc, char** argv){
    pid_t pidsag = 1;
    pid_t pidsol = 0,temp;
    int status;
    for (int i = 0; i < 3; ++i){
        pidsol = fork();
        if(pidsol == 0){
            printf("Sol cocuk proses. Seviye: %d PID: %d Annem: %d\n", i+1, getpid(),getppid());
            return 0;
        }
        else{
            pidsag = fork();
            if(pidsag == 0){
                printf("Sag cocuk proses. Seviye: %d PID: %d Annem: %d\n", i+1, getpid(),getppid());
            }
            else{
                printf("Anne proses. Seviye %d PID: %d Sag cocuk %d  Sol cocuk: %d\n",i,getpid(),pidsag,pidsol);
                waitpid(pidsag,&status,0);
                return 0;
            }
        }
    }
    return 0;
}
