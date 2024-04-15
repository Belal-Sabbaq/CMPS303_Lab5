#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define MAX_MSG_SIZE 256
#define UP_QUEUE_KEY 1234
#define DOWN_QUEUE_KEY 5678
int up_queue_id, down_queue_id;
int client_id;



struct msgbuff
{
    long mtype;
    char mtext[MAX_MSG_SIZE + 1];
};
void sigint_handler(int signum) {
    //Don't  Delete message queues
    printf("\nClient terminated.\n");

    exit(0);
}


int main(){
    client_id = getpid() % 10000;
    up_queue_id = msgget(UP_QUEUE_KEY, IPC_CREAT | 0666);
    down_queue_id = msgget(DOWN_QUEUE_KEY, IPC_CREAT | 0666);
    signal(SIGINT, sigint_handler);

    struct msgbuff msg; 
    while (1) {
        
        printf("Enter message: ");
        fgets(msg.mtext, MAX_MSG_SIZE, stdin);
        msg.mtext[strcspn(msg.mtext, "\n")] = '\0'; // Remove newline character

        // Send message to server with client ID as message type
        msg.mtype = client_id;
        msgsnd(up_queue_id, &msg, strlen(msg.mtext) + 1, 0);

        // Wait for response from server
        msgrcv(down_queue_id, &msg, MAX_MSG_SIZE, client_id,!IPC_NOWAIT);
        printf("Response: %s\n", msg.mtext);
    }


return 0;

}