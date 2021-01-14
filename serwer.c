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
#define MAX_LENGTH_MESSAGE 40
#define SERVER 1
#define CLIENT 2

typedef struct StrukturaWiadomosci
{
    long int odbiorca;
    long int nadawca;
    char wiadomosc[MAX_LENGTH_MESSAGE];
} StrukturaWiadomosci;


key_t key;
int queueID;

void removeMessageQueue(int queueID);
char *toUpperCase(char *messageForUpper, size_t sizeText);
void signalHandler();
void createKey();
void createQueue();

//main:

int main()
{
    createKey();
    createQueue();
    
    signal(SIGINT, signalHandler); //handling CTRL + C

    StrukturaWiadomosci msg;
    int returnStatus;
    
    while(1)
    {
        printf("Czekam na wiadomosc... \n");
        while(
            (returnStatus = msgrcv(queueID, &msg,(sizeof(StrukturaWiadomosci)-sizeof(long int)), 1, 0)) == -1
                && errno == EINTR);
      
        if(returnStatus == -1)
        {
            perror("Blad odbioru wiadomosci!");
            removeMessageQueue(queueID);
            exit(-1);
        }
        
        printf("Mam wiadomosc od %ld o tresci:\n%s", msg.nadawca, msg.wiadomosc);
        msg.odbiorca = msg.nadawca;
        toUpperCase(msg.wiadomosc, sizeof(msg.wiadomosc));
        
        while(
            (returnStatus = msgsnd(queueID, &msg, (sizeof(StrukturaWiadomosci)-sizeof(long int)), IPC_NOWAIT)) == -1
             && errno == EINTR);
        
        if(returnStatus == -1)
        {
            perror("Problem with send a message! Queue may be full!");
            removeMessageQueue(queueID);
            exit(-1);
        }
        
        printf("Wyslalem wiadomosc zwrotna do %ld\n", msg.nadawca);
    }
}

// functions:

void createQueue(){
    queueID = msgget(key, IPC_CREAT|IPC_EXCL|0600);
    if(queueID == -1) {
        perror("Blad utworzenia kolejki komunikatow!");
        exit(-1);
    }
    printf("Utworzylem kolejke komunikatow!\n");
}

void createKey(){
    key = ftok(".", 'A');
    if(key == -1) {
        perror("Blad utworzenia klucza!");
        exit(-1);
    }
}

void signalHandler()
{
    removeMessageQueue(queueID);
    
    printf("\nKONIEC PRACY SERWERA!!\n");
    exit(EXIT_SUCCESS);
}

void removeMessageQueue(int queueID)
{
    if(msgctl(queueID, IPC_RMID, 0 == -1)) {
        perror("Blad usuwania kolejki komunikatow!");
        exit(-2);
    }
    
    printf("Usunalem kolejke komunikatow: %d\n", queueID);
}


char *toUpperCase(char *messageForUpper, size_t sizeText)
{
    char *messageAfterUpper = messageForUpper;
    
    while(*messageForUpper && --sizeText>0) {
        *messageAfterUpper++ = toupper(*messageForUpper++);
    }
    
    *messageAfterUpper = 0;
    return messageForUpper;
}
