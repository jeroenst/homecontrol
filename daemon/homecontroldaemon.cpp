#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <string.h>
#define BAUDRATE B9600
#define FALSE 0
#define TRUE 1
#include <getopt.h>
#include <ctype.h>
#include <iostream>       // std::cin, std::cout
#include <queue>          // std::queue
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <mqueue.h>
#include <sys/ipc.h>
#include "homecontroldaemon.h"

using namespace std;

std::queue<string> arduino_queue;
int fd, res, i;
int waitforanswer = 0;

//
volatile int STOP=FALSE;
void signal_handler_IO (int status);
int wait_flag=TRUE;
char devicename[80] = "/dev/ttyACM1", ch;
int status;
//
int main(int argc, char *argv[])
{
struct termios newtio;
struct sigaction saio;

mqd_t mqd;
struct mq_attr attr;
int ret;
char msg[MQ_MESSAGE_MAX_LENGTH];
ssize_t msg_len;


if (argc > 1)
{
    strcpy (devicename, argv[1]);
}
else
{
    printf ("Usage: homecontroldaemon <device like /dev/ttyUSB0>\n");
    exit(1);
}
//
//open the device in non-blocking way (read will return immediately)
fd = open(devicename, O_RDWR | O_NOCTTY | O_NONBLOCK);
if (fd < 0)
{
perror(devicename);
exit(1);
}
//
//install the serial handler before making the device asynchronous
//saio.sa_handler = signal_handler_IO;
//sigemptyset(&saio.sa_mask);   //saio.sa_mask = 0;
//saio.sa_flags = 0;
//saio.sa_restorer = NULL;
//sigaction(SIGIO,&saio,NULL);
//
// allow the process to receive SIGIO
//fcntl(fd, F_SETOWN, getpid());
//
// make the file descriptor asynchronous
//fcntl(fd, F_SETFL, FASYNC);
//
// set new port settings for canonical input processing
newtio.c_cflag = B9600 | CRTSCTS | CS8 | CLOCAL | CREAD;
newtio.c_iflag = IGNPAR;
newtio.c_oflag = 0;
newtio.c_lflag = 0;
newtio.c_cc[VMIN]=1;
newtio.c_cc[VTIME]=0;
tcflush(fd, TCIFLUSH);
tcsetattr(fd,TCSANOW,&newtio);
//
// loop while waiting for input. normally we would do something useful here



        // Form the queue attributes
        attr.mq_flags = 0; //O_NONBLOCK; // i.e mq_send will be block if message queue is full
        attr.mq_maxmsg = MQ_MAX_NUM_OF_MESSAGES;
        attr.mq_msgsize = MQ_MESSAGE_MAX_LENGTH;
        attr.mq_curmsgs = 0; // mq_curmsgs is dont care

        // Create message queue
        system ("mkdir /dev/mqueue >/dev/null 2>&1");
        system ("mount -t mqueue none /dev/mqueue >/dev/null 2>&1");
        system (string("sudo chmod 666 /dev/mqueue" + string(MQ_NAME) + " >/dev/null 2>&1").c_str());
        mqd = mq_open(MQ_NAME, O_RDONLY | O_CREAT | O_NONBLOCK, MQ_MODE, &attr);
        if( mqd != (mqd_t)-1 )
        {
                printf(" Message Queue Opened\n");

                printf(" Receiving message....\n");
                while(1)
		{
		    fd_set set;
		    struct timeval timeout;
		    /* Initialize the file descriptor set. */
		    FD_ZERO (&set);
		    FD_SET (mqd, &set);
		    FD_SET (fd, &set);

		    /* Initialize the timeout data structure. */
	            timeout.tv_sec = 5;
		    timeout.tv_usec = 0;

		    if (select (FD_SETSIZE, &set, NULL, NULL, &timeout) == 0)
		    {
                       // If timeout send next message to arduino if no response on previous command
                       waitforanswer = 0;   		        
		    }

                    msg_len = mq_receive(mqd, msg, MQ_MESSAGE_MAX_LENGTH, 0);
                    if(msg_len > 0) // if a message is received from message queueu put it in arduino queue
                    {
                        msg[msg_len] = '\0';
                        printf(" Successfully received %d bytes: %s", (int)msg_len, msg);
                        arduino_queue.push(msg);

                    }

                    int flags = fcntl(fd, F_GETFL, 0);
                    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
                    char buf[4096];
                    res = read(fd,buf,4096);
                    if (res > 0)
                    {
                          buf[res] = 0;
                          printf(" Received from arduino: %s", buf);
                          waitforanswer = 0;
                    }

                    if (waitforanswer == 0) //wait with sending until arduino has send an answer and is ready for next command
                    {
                        if (!arduino_queue.empty())
                        {
                                string message = arduino_queue.front();
                                arduino_queue.pop();
                                printf (" Writing to arduino: %s", message.c_str());
                                write (fd, message.c_str(), strlen(message.c_str()));
                                waitforanswer = 1;
                        }
                    }

                }
                
                // Close the message queue
                ret = mq_close(mqd);
                if(ret)
                        perror(" Message queue close failed\n");
                else
                        printf(" Message Queue Closed\n");
        
                ret = mq_unlink(MQ_NAME);
                if(ret)
                        perror(" Message queue unlink failed\n");
                else
                        printf(" Message Queue unlinked\n");
        }
        else
        {
                perror(" Message queue open failed ");
        }

        close(fd);
        return 0;
}
