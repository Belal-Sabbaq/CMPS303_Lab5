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
void conv(char *msg, int size) {
int i;
for (i=0; i<size; ++i)
if (islower(msg[i]))
msg[i] = toupper(msg[i]);
else if (isupper(msg[i]))
msg[i] = tolower(msg[i]);

}



struct msgbuff
{
    long mtype;
    char mtext[MAX_MSG_SIZE + 1];
};
void sigint_handler(int signum) {
    // Delete message queues
    msgctl(up_queue_id, IPC_RMID, NULL);
    msgctl(down_queue_id, IPC_RMID, NULL);
    printf("\nServer terminated.\n");
    exit(0);
}


int main(){
    up_queue_id = msgget(UP_QUEUE_KEY, IPC_CREAT | 0666);
    down_queue_id = msgget(DOWN_QUEUE_KEY, IPC_CREAT | 0666);
    signal(SIGINT, sigint_handler);

    struct msgbuff msg;
    while (1) {
        
        msgrcv(up_queue_id, &msg, MAX_MSG_SIZE, 0, 0);

        conv(msg.mtext,sizeof(msg.mtext));
        // Send converted message back to client
        msg.mtype = msg.mtype; // Use same client identifier as message type
        msgsnd(down_queue_id, &msg, strlen(msg.mtext) + 1, !IPC_NOWAIT);
    }


return 0;

}