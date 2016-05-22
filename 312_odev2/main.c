/*İhsan Burak Çağlar
 * 150120057
 * Blg312 - İşletim Sistemleri
 * Ödev 2
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <wait.h>
#include <stdio.h>
#include <unistd.h>

#define SEMKEYA 12
#define SEMKEYB 34
#define SEMKEYC 56

#define TMEMA 789
#define TMEMB 456

#define LENGTH 10

void sem_signal(int semid, int val){
    struct sembuf semafor;
    semafor.sem_num = 0;
    semafor.sem_op = val;
    semafor.sem_flg = 1;
    semop(semid,&semafor,1);
}

void sem_wait(int semid, int val){
    struct sembuf semafor;
    semafor.sem_num = 0;
    semafor.sem_op = -1*val;
    semafor.sem_flg = 1;
    semop(semid,&semafor,1);
}


int main()
{
    int* shm = NULL;
    int* shm2=NULL;
    int shmid,shmid2,order,f=1,sem,t;
    const int shmsize = sizeof(int)*LENGTH;
    for(order=0;order<2;order++){
        if(f>0){
            f = fork();
        }
        if(f==-1){
            printf("fork error!!\n");
            return 1;
        }
        else if(f==0){
            break;
        }
        else{
            if(order==0){
                sem = semget(SEMKEYC,1,0700|IPC_CREAT);
                semctl(sem,0,SETVAL,0);
                sem = semget(SEMKEYB,1,0700|IPC_CREAT);
                semctl(sem,0,SETVAL,0);
                sem = semget(SEMKEYA,1,0700|IPC_CREAT);
                semctl(sem,0,SETVAL,0);
            }
        }
    }
    if(f>0){
        printf("Proses1 calisiyor..\n");
        if ((shmid2 = shmget(TMEMB, shmsize, IPC_CREAT | 0666)) < 0) {
            perror("shmget");
            return 1;
        }
        if ((shmid = shmget(TMEMA, shmsize, IPC_CREAT | 0666)) < 0) {
            perror("shmget");
            return 1;
        }
        shm = shmat(shmid, NULL, 0);
        printf("T1 alanindaki sayilar:   ");
        for(t = 0;t<LENGTH;t++){
            shm[t] = t;
            printf("%d ",shm[t]);
        }
        printf("\n");
        shmdt(shm);
        shmdt(shm2);
        sem = semget(SEMKEYB,1,0);
        sem_signal(sem,1);
    }
    else{
        if(order==0){
            sem = semget(SEMKEYB,1,0);
            sem_wait(sem,1);
            int temp[10];
            printf("Proses2 calisiyor...\n");
            shmid = shmget(TMEMA,shmsize,0);
            shm = (int*) shmat(shmid,0,0);
            printf("T2 alanindaki sayilar:   ");
            for(t= 0;t<LENGTH;t++){
                int total = 0,k=0;
                for(k = 0;k<=t;k++){
                    total += shm[k];
                }
                temp[t] = total;
            }
            shmdt(shm);
            shmctl(TMEMA,IPC_RMID,0);
            shmid2 = shmget(TMEMB,shmsize,0);
            shm2 = (int*)shmat(shmid2,0,0);
            for(t=0;t<LENGTH;t++){
                shm2[t] = temp[t];
                printf("%d ",shm2[t]);
            }
            printf("\n");
            shmdt(shm2);
            sem = semget(SEMKEYC,1,0);
            sem_signal(sem,1);
            return 0;
        }
        else{
            sem = semget(SEMKEYC,1,0);
            sem_wait(sem,1);
            shmid = shmget(TMEMB,shmsize,0);
            shm = (int*) shmat(shmid,0,0);
            printf("Proses3 calisiyor...\nHesaplama Sonucu:        ");
            for(t=1;t<LENGTH;t++){
                printf("%d ",shm[t]+shm[t-1]);
            }
            shmdt(shm);
            printf("\n\n");
            shmctl(TMEMB,IPC_RMID,0);
            sem = semget(SEMKEYA,1,0);
            sem_signal(sem,1);
            return 0;
        }
    }
    sem = semget(SEMKEYA,1,0);
    sem_wait(sem,1);
    wait(NULL);
    semctl(SEMKEYB,0,IPC_RMID,0);
    semctl(SEMKEYC,0,IPC_RMID,0);
    semctl(SEMKEYA,0,IPC_RMID,0);
    return 0;
}
