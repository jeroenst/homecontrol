#include <errno.h>
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
int fd = -1, res, i;
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

        // Create message queue
        attr.mq_flags = 0; //O_NONBLOCK; // i.e mq_send will be block if message queue is full
        attr.mq_maxmsg = MQ_MAX_NUM_OF_MESSAGES;
        attr.mq_msgsize = MQ_MESSAGE_MAX_LENGTH;
        attr.mq_curmsgs = 0; // mq_curmsgs is dont care
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
		    /* Initialize the file descriptor set. */
		    fd_set set;
		    FD_ZERO (&set);
		    FD_SET (mqd, &set);
		    if (fd >= 0) FD_SET (fd, &set);

		    /* Initialize the timeout data structure. */
		    struct timeval timeout;
	            timeout.tv_sec = 5;
		    timeout.tv_usec = 0;
		    if (fd < 0)  timeout.tv_sec = 1;
		    
		    // Wait for timeout or data on filedescriptors
		    if (select (FD_SETSIZE, &set, NULL, NULL, &timeout) == 0)
		    {
                       // If timeout send next message to arduino if no response on previous command
                       printf ("Select timeout!\n");
                       waitforanswer = 0;   		        
		    }

                    msg_len = mq_receive(mqd, msg, MQ_MESSAGE_MAX_LENGTH, 0);
                    if(msg_len > 0) // if a message is received from message queueu put it in arduino queue
                    {
                        msg[msg_len] = '\0';
                        printf(" Received from message queue: %s",  msg);
                        arduino_queue.push(msg);

                    }

                    if (fd < 0)
                    {
                        printf (" Opening device: %s...\n", devicename);
                        //
                        //open the device in non-blocking way (read will return immediately)
                        fd = open(devicename, O_RDWR | O_NOCTTY | O_NONBLOCK);
                        // set new port settings for canonical input processing
                        int flags = fcntl(fd, F_GETFL, 0);
                        newtio.c_cflag = B9600 | CRTSCTS | CS8 | CLOCAL | CREAD;
                        newtio.c_iflag = IGNPAR;
                        newtio.c_oflag = 0;
                        newtio.c_lflag = 0;
                        newtio.c_cc[VMIN]=1;
                        newtio.c_cc[VTIME]=0;
                        tcflush(fd, TCIFLUSH);
                        tcsetattr(fd,TCSANOW,&newtio);
                        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
                    }
                    
                    
                    if (fd >= 0)
                    {
                        char buf[4096];
                        res = read(fd,buf,4096);
                        if ((res < 0) && (errno != EWOULDBLOCK))
                        {
                                printf (" Error while reading arduino, device %s closed...\n", devicename);
                                close(fd);
                                fd = -1;
                        }
                        if (res > 0)
                        {
                              buf[res] = 0;
                              printf(" Received from arduino: %s", buf);
                              waitforanswer = 0;
                        }
                    }

                    //wait with sending until arduino has send an answer and is ready for next command
                    if ((waitforanswer == 0) && (fd >= 0))
                    {
                        if (!arduino_queue.empty())
                        {
                                string message = arduino_queue.front();
                                printf (" Writing to arduino: %s", message.c_str());
                                if (write (fd, message.c_str(), strlen(message.c_str())) >= 0)
                                {
                                        waitforanswer = 1;
                                        arduino_queue.pop();
                                }
                                else 
                                {
                                        printf (" Writing to arduino failed, device %s closed...\n", devicename);
                                        close (fd);
                                        fd = -1;
                                }
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
