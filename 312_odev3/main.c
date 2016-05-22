/*İhsan Burak Çağlar
 * 150120057
 * Blg312 - İşletim Sistemleri
 * Ödev 3
 * 05.05.2016
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define SEMKEYKAYNAK 15
#define SEMKEYABONE 314
#define KAYNAKDISLA 35

#define MEMHABER 73

#define LENGTH 100

void sem_signal(int semid, int semnum, int val){
    struct sembuf semafor;
    semafor.sem_num = semnum;
    semafor.sem_op = val;
    semafor.sem_flg = 1;
    semop(semid,&semafor,1);
}

void sem_wait(int semid, int semnum, int val){
    struct sembuf semafor;
    semafor.sem_num = semnum;
    semafor.sem_op = -1*val;
    semafor.sem_flg = 1;
    semop(semid,&semafor,1);
}


int main()
{
    char* shm = NULL;
    int shmid,order,f=1,sem,t,sem2;
    int n,m;
    int abone = 0;
    printf("\n  Haber kaynagi proses sayisini giriniz: ");
    scanf("%d",&m);
    printf("\n  Abone proses sayisini giriniz: ");
    scanf("%d",&n);
    printf("\n");
    const int shmsize = sizeof(char)*LENGTH;
    for(order=0;order<n;order++){

        // abonelerin yaratildigi kisim

        if(f>0){
            f = fork();
        }
        if(f==-1){
            printf("fork error!!\n");
            return 1;
        }
        else if(f==0){
            abone=1;
            break;
        }
        else{
            if(order==0){

                // kaynak icin bir semafor ve bu semafora erisim icin bir semafor yaratiliyor
                // aboneleri kontrol etmek icin abone sayisina bagi olarak semafor seti aliniyor

                sem = semget(SEMKEYKAYNAK,1,0666|IPC_CREAT);
                semctl(sem,0,SETVAL,0);
                sem = semget(KAYNAKDISLA,1,0666|IPC_CREAT);
                semctl(sem,0,SETVAL,0);
                sem = semget(SEMKEYABONE,n,0666|IPC_CREAT);
                for(t=0;t<n;t++) semctl(sem,t,SETVAL,0);
                shmid = shmget(MEMHABER, shmsize, IPC_CREAT | 0666);
                shm = (char*) shmat(shmid,0,0);
                memset(shm, '\0', sizeof(shm));
                shmdt(shm);

            }
        }
    }
    if(f>0){

        // haber kaynaklarinin yaratildigi kisim

        for(order=0;order<n;order++){
            if(f>0){
                f = fork();
            }
            if(f==-1){
                printf("fork error!!\n");
                return 1;
            }
            else if(f==0){
                abone = 0;
                break;
            }
            if(f>0 && order==n-1){
                sem = semget(KAYNAKDISLA,1,0);
                sem_signal(sem,0,2);
                sem = semget(SEMKEYKAYNAK,1,0);
                sem_signal(sem,0,n);
                sleep(2*n);
                printf("\n  Islem tamamlandi, cikiliyor.\n\n");
                shmctl(MEMHABER,IPC_RMID,0);
                semctl(SEMKEYKAYNAK,0,IPC_RMID,0);
                for(t=0;t<n;t++) semctl(SEMKEYABONE,t,IPC_RMID,0);
                return 0;
            }
        }

    }
    if(abone==0){

        //haberi_yayinla() fonksiyonu

        sem2 = semget(KAYNAKDISLA,1,0);
        sem_wait(sem2,0,1);
        sem = semget(SEMKEYKAYNAK,1,0);
        int durum = semctl(sem,0,GETVAL);
        shmid = shmget(MEMHABER,shmsize,0);
        shm = (char*) shmat(shmid,0,0);
        if(durum<n){
            printf("\n  Haber kaynagi-%d   ==> %s herkes tarafindan okunmadi, bekliyor.\n\n",order+1,shm);
        }
        sem_wait(sem,0,n);
        memset(shm, '\0', sizeof(shm));
        sprintf(shm, "Haber-%d ",order+1);
        printf("\n  Haber kaynagi %d  ==> %s yayinlandi.\n\n",order+1,shm);
        shmdt(shm);
        sem = semget(SEMKEYABONE,n,0);
        for(t=0;t<n;t++) sem_signal(sem,t,1);
            sem_signal(sem2,0,1);
            return 0;
        }
    else{

        // haberi_oku() foksiyonu

        sem = semget(SEMKEYABONE,n,0);
        for(t=0;t<m;t++){
            int durum = semctl(sem,order,GETVAL);
            if(durum==0){
                printf("  ----Abone-%d   haber okumak istedi ama yeni haber yok, bekliyor.\n",order+1);
            }
            sem_wait(sem,order,1);
            shmid = shmget(MEMHABER,shmsize,0);
            shm = (char*) shmat(shmid,0,0);
            printf("  Abone-%d,   %s okudu.\n",order+1,shm);
            sem2 = semget(SEMKEYKAYNAK,1,0);
            sem_signal(sem2,0,1);
            sleep(1);
        }
        shmdt(shm);
        return 0;
    }
    return 0;
}
