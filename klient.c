#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "pthread.h"
#define MAX_LENGTH_MESSAGE 25
#define SERVER 1
#define CLIENT 2

typedef struct StrukturaWiadomosci
{
    long int odbiorca;
    long int nadawca;
    char wiadomosc[MAX_LENGTH_MESSAGE];
} StrukturaWiadomosci;

int queueID;
key_t key;

void signalHandler();
void *nadawca();
void *odbiorca();
void createKey();
void getAccessToQueue();


//main
int main()
{
    createKey();
    getAccessToQueue();    
    
    signal(SIGINT, signalHandler); //handling CTRL + C
    
    pthread_t watekNadawcy, watekOdbiorcy;
    
    if(pthread_create(&watekNadawcy, NULL, nadawca, NULL) != 0)
    {
        perror("Blad utowrzenia watku nadawcy!");
        exit(1);
    }
    
    if(pthread_create(&watekOdbiorcy, NULL, odbiorca, NULL) != 0)
    {
        perror("Blad utworzenia watku odbiorcy!");
        exit(1);
    }
    
    if(pthread_join(watekNadawcy, NULL) != 0)
    {
        perror("Blad podlaczenia watku nadawcy!");
        exit(2);
    }
    
    if(pthread_join(watekOdbiorcy, NULL) != 0)
    {
        perror("Blad podlaczenia watku odbiorcy!");
        exit(2);
    }

    if(pthread_detach(watekNadawcy) == -1){
        perror("Problem with detach sender thread!");
        exit(3);
    }

    if(pthread_detach(watekOdbiorcy) == -1){
        perror("Problem with detach recipient thread!");
        exit(3);
    }
    
}

void getAccessToQueue(){
    queueID = msgget(key, 0600);
    if(queueID == -1)
    {
        perror("Blad podlaczenia do kolejki komunikatow!\nSprawdz czy serwer pracuje!");
        exit(-1);
    }
    printf("Podlaczono sie do kolejki! \n");
}

void createKey(){
    key = ftok(".", 'A');
    if(key == -1){
        perror("Blad utworzenia klucza!");
        exit(-1);
    }
}

void signalHandler()
{
    printf("\n KLIENT: %d ZAKONCZYL PRACE!.\n", getpid());
    exit(0);
}

void *nadawca()
{
    struct msqid_ds buffer;
    
    StrukturaWiadomosci msg;
    
    // calculate max value of queue
    msgctl(queueID, IPC_STAT, &buffer);
    int max = buffer.msg_qbytes/ (sizeof(StrukturaWiadomosci)-sizeof(long int));
    max--; // space for response from server

    printf("Podaj wiadomosc: \n");
    while(1){
        fgets(msg.wiadomosc, MAX_LENGTH_MESSAGE, stdin);
        
        msg.odbiorca = SERVER;
        msg.nadawca = getpid();  // pid client app
        
        //pobranie danych do bufora z kolejki
        msgctl(queueID, IPC_STAT, &buffer);

        // sprawdzanie czy mozna wyslac wiadomosc
        if(buffer.msg_qnum >= max){ //msg_qnum - number of message
            printf("The queue is full!!\n Please wait for empty space in queue!\n");
            while(buffer.msg_qnum >= max){
                msgctl(queueID, IPC_STAT, &buffer);
            }
        }
        

        int status;
        // wysylanie wiadomosci 
        while((status = msgsnd(queueID, &msg, (sizeof(StrukturaWiadomosci)-sizeof(long int)), IPC_NOWAIT)) == -1 && errno == EINTR);
        
        // obsluga bledow
        if(status == -1){
            if(errno == EAGAIN){   
                perror("The queue is full!\n I cannot send a message!");
                while(
                (status = msgsnd(queueID, &msg, (sizeof(StrukturaWiadomosci)-sizeof(long int)), 0)) == -1
                && (errno == EAGAIN || errno == EINTR));
                if(status == -1){
                    perror("Problem with send a message");
                    exit(EXIT_FAILURE);
                }
            }
            else{
                perror("Blad wysylania wiadomosci!");
                exit(EXIT_FAILURE);
            }
        }
        
        printf("++++++++++++Wiadomosc zostala wyslana!++++++++++++\n");
        
    }
}

void *odbiorca()
{
    StrukturaWiadomosci msg;
    int status;
    
    while(1){
      
        while(
        (status = msgrcv(queueID, &msg, (sizeof(StrukturaWiadomosci)-sizeof(long int)), getpid(), 0)) == -1
        && errno == EINTR);
        
        if(status == -1){
            perror("Server is unactivated!");
            exit(EXIT_FAILURE);
        }
        
        printf("++++++++++++Otrzymalem odpowiedz:\n%s\n", msg.wiadomosc);
        printf("Podaj wiadomosc: \n");
    }
}
