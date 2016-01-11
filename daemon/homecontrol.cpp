#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <ctype.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#include "homecontroldaemon.h"

using namespace std;

int main(int argc, char **argv) 
{
    mqd_t mqd;
    int ret;
    
    char msg[MQ_MESSAGE_MAX_LENGTH];
    ssize_t msg_len;
    if (argc > 1) 
    {
        strcpy (msg, argv[1]);
        if (argc > 2) { strcat (msg, " "); strcat (msg, argv[2]); }
        if (argc > 3) { strcat (msg, " "); strcat (msg, argv[3]); }
        if (argc > 4) { strcat (msg, " "); strcat (msg, argv[4]); }
        strcat (msg, "\n");
        	/* Open the message queue. Message queue is already created from a different process */
        	mqd = mq_open(MQ_NAME, O_WRONLY);
	        if( mqd != (mqd_t)-1 )
        	{
                        printf(" Message Queue Opened\n");

                	printf(" Sending message to homecontroldaemon: %s", msg);
	                ret = mq_send(mqd, msg, strlen(msg), MQ_MESSAGE_PRIORITY);
        	        if(ret)
                	        perror("Failed\n");
	                else
        	                printf("Done\n");
                
                	/* Close the message queue */
	                ret = mq_close(mqd);
        	        if(ret)
				perror(" Message queue close failed\n");
                	else
				printf(" Message Queue Closed\n");
        
        	}
	        else
        	{
	                perror(" Message queue open failed, is homecontroldaemon running?");
			exit(4);
        	}
	
    }
	return 0;
}
