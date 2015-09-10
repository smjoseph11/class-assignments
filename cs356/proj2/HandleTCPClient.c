//Stanlin Joseph
//Matthew Lacamu
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include <stdlib.h>

#define RCVBUFSIZE 100

void HandleTCPClient(int clntSocket, int verbose){
    int webSock; 
    char buffer[RCVBUFSIZE];
    char getMessage[RCVBUFSIZE]; 
    char hostMessage[RCVBUFSIZE]; 
    char hostName[RCVBUFSIZE];
    int requestSize = sizeof(getMessage) + sizeof(hostMessage);
    char requestString[requestSize]; 
    int recvMsgSize;
    struct addrinfo *result, hints;
    bool replied = false;

    //opens file for writing responses to be sent back to client
    FILE *webfile;
    webfile = fopen("webfile", "w+");

    memset(buffer, 0, sizeof(buffer));
    memset(requestString, 0, sizeof(requestString));
    memset(&hints, 0, sizeof(hints));
    memset(getMessage, 0, sizeof(getMessage));
    memset(hostMessage, 0, sizeof(hostMessage));
    memset(hostName, 0, sizeof(hostName));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    //recieves request message from client
    if ((recvMsgSize = recv(clntSocket, buffer, RCVBUFSIZE, 0)) < 0){
        printf("recv() failed");
        close (clntSocket);
        return;
    }

    strcpy(getMessage, buffer);        
    memset(buffer, 0, sizeof(buffer));

    //recieves host for request from client
    if ((recvMsgSize = recv(clntSocket, buffer, RCVBUFSIZE, 0)) < 0){
        printf("recv() failed");
        close (clntSocket);
        return;
    }
    strcpy(hostMessage, buffer);

    if(verbose){
        printf("%s at %s\n", getMessage, hostMessage);
    }

    strcpy(hostName, hostMessage+6);

    hostName[strlen(hostName)-2] = '\0';

    strcpy(requestString, getMessage);
    strcat(requestString, hostMessage);
    strcat(requestString, "\r\n");

    //gets ip address and other info for the host
    if(getaddrinfo(hostName, "80", &hints, &result)!=0){
        printf("couldn't get addrinfo");
        close (clntSocket);
        return;
    }

    //create socket for communication with the host
    webSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(webSock == -1){
        printf("socket() failed");
        close (webSock);
        close (clntSocket);
        return;
    }

    //connects to host
    if (connect(webSock, result->ai_addr, result->ai_addrlen) == -1){
        printf("couldn't connect() to webSock");
        close (webSock);
        close (clntSocket);
        return;
    }
    //sends request from client to host through the proxy
    if(send(webSock, requestString, requestSize, 0) != requestSize){
        printf("send() failed");
        close (webSock);
        close (clntSocket);
        return;
    }
    //Start listening for response from web
    memset(buffer, 0, sizeof(buffer));

    if((recvMsgSize = recv(webSock, buffer, RCVBUFSIZE, 0 )) < 0){
        printf("recv() failed\n");
        close (webSock);
        close (clntSocket);
        return;
    }
    int j;
    //writes host's response to file 
    while(recvMsgSize > 0){
        fwrite(buffer, 1, sizeof(buffer), webfile);
        memset(buffer, 0, sizeof(buffer));
        if((recvMsgSize = recv(webSock, buffer, RCVBUFSIZE, 0 )) < 0){
                printf("recv() failed\n");
                close (webSock);
                close (clntSocket);
                return;
            }
    }
    fseek(webfile, 0, SEEK_SET);
    char *line = NULL;
    size_t len = 100;
    char* needle;
    bool headerOver = false;
    //reads line by line from file and sends text back to client with Google changed to elgooG
    while((getline(&line, &len, webfile)) != -1 ){
        if(headerOver){
            needle = strstr(line, "Google");
            if(needle != 0){
                memcpy(needle, "elgooG", 6);
                if(verbose == 1){
                    printf("Page found with Google");
                    fflush(stdout);
                }
            }
        }
        else{
            if(!strcmp(line, "\r\n"))
                headerOver = true;
        }
        memset(buffer, 0, sizeof(buffer));
        int lineChars = strlen(line);
        while(lineChars > RCVBUFSIZE){
            memcpy(buffer, line, RCVBUFSIZE);
            lineChars-=RCVBUFSIZE;
            line = line + RCVBUFSIZE;
            if(send(clntSocket, buffer, RCVBUFSIZE, 0) != RCVBUFSIZE){
                printf("send() failed");
                close (webSock);
                close (clntSocket);
                return;
            }
            memset(buffer, 0, sizeof(buffer));
        }

        memcpy(buffer, line, lineChars);

        if(send(clntSocket, buffer, RCVBUFSIZE, 0) != RCVBUFSIZE){
            printf("send() failed");
            close (webSock);
            close (clntSocket);
            return;
        }
        line = NULL;
    }

    fclose(webfile);
    close (webSock);
    close (clntSocket);
}
