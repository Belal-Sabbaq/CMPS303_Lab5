#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

struct msgbuff
{
    long mtype;
    char mtext[70];
};

int main()
{
    key_t key_id;
    int rec_val, msgq_id;

    key_id = ftok("keyfile", 65);               //create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); //create message queue and return id

    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    printf("Message Queue ID = %d\n", msgq_id);

    struct msgbuff message;
    while(1)
    {
        /* 0 -> receive all types of messages */
        rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);
        // IPC_NOWAIT: return -1 immediately if the queue is empty and the message can't be received
        // !IPC_NOWAIT : block until the message can be received
        // 0 : ignore
        // MSG_NOERROR : truncate the message if it is too long to fit in the queue (default: return -1 and the msg is not received and stays in queue)

        if (rec_val == -1)
            perror("Error in receive");
        else
            printf("\nMessage received: %s\n", message.mtext);
    }
    struct msqid_ds ctrl_status_ds;
    msgctl(msgq_id, IPC_STAT, &ctrl_status_ds);

    // Printing some parts of the status
    printf("Message Queue ID: %d\n", msgq_id);
    printf("Access Permissions: %o\n", ctrl_status_ds.msg_perm.mode);
    printf("Current number of bytes in queue: %ld\n", ctrl_status_ds.__msg_cbytes);
    printf("Current number of messages in queue: %ld\n", ctrl_status_ds.msg_qnum);
    printf("Maximum number of bytes allowed in queue: %ld\n", ctrl_status_ds.msg_qbytes);
    printf("Last msgsnd time: %ld\n", ctrl_status_ds.msg_stime);
    printf("Last msgrcv time: %ld\n", ctrl_status_ds.msg_rtime);
    
    ctrl_status_ds.msg_perm.mode = 0664;
    msgctl(msgq_id, IPC_SET, &ctrl_status_ds);
    printf("Access Permissions: %o\n", ctrl_status_ds.msg_perm.mode);

    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);
    // IPC_STAT -> get queue status inside the struct
    // IPC_SET -> change the values for example permissions
    // IPC_RMID -> removes the queue

    return 0;
}
