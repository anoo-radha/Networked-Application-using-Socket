//
//  server_test.cpp
//  Socket_prog_test1
//
//  Created by Anu on 9/15/15.
//
// Server Application in the internet address domain using TCP(connection-oriented) protocol

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


void newProcess(int);
inline void errorMsg(const char*);

int main(int argc, char *argv[])
{
    int socketHndl, newSocketHndl, portNum, pid = 0;
    socklen_t len;
    struct sockaddr_in serverAddr, clientAddr;
    
    if (argc < 2)
    {
        //printf("ERROR: No Port Provided\n Provide port number as argument\n");
        portNum = 5000;     //providing default port number
    }
    else
    {
        portNum = atoi(argv[1]);
    }
    memset(&serverAddr, '0', sizeof(serverAddr));
    
    socketHndl = socket(AF_INET, SOCK_STREAM, 0);
    if (socketHndl < 0)
        errorMsg("ERROR opening socket");
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(portNum);
   
    if ((bind(socketHndl, (struct sockaddr *) &serverAddr,sizeof(serverAddr))) < 0)
    {
        errorMsg("ERROR: when binding socket to server");
    }
    
    listen(socketHndl,5);
    len = sizeof(clientAddr);
    while(1)
    {
        newSocketHndl = accept(socketHndl,(struct sockaddr *) &clientAddr,&len);
        if (newSocketHndl < 0)
        {
            errorMsg("ERROR: on accept");
        }
        
        //from linuxhowtos.org
        pid = fork();
        if (pid < 0)
            errorMsg("ERROR: ocurred when forking new process");
        if (pid == 0)
        {
            close(socketHndl);
            newProcess(newSocketHndl);
            exit(0);
        }
        else
        {
            close(newSocketHndl);
        }
        sleep(1);
    }
    //    close(socketHndl);
    return 0;
}

void newProcess (int socketHndl)
{
    static int processNo = 0;
    char buffer[256]={0}, sendBuffer[1025]={0};
    time_t currTime;
    
    if (read(socketHndl,buffer,255) < 0)
    {
        errorMsg("ERROR: reading from socket");
    }
    currTime = time(NULL);
    printf("Message: %s (received at %.25s)\n",buffer, ctime(&currTime));
    snprintf(sendBuffer, sizeof(sendBuffer), "Acknowledging message (at %.25s)\n", ctime(&currTime));
    
    if (write(socketHndl, sendBuffer, strlen(sendBuffer)) < 0)
    {
        errorMsg("ERROR: writing to socket");
    }
#if DEBUG
    printf("Finished process %d\n", ++processNo);
#endif
}

inline void errorMsg(const char *msg)
{
    perror(msg);
    exit(1);
}


