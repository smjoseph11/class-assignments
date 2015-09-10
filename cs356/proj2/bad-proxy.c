//Stanlin Joseph
//Matthew Lacamu
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void HandleTCPClient(int clntSocket, int verbose);
#define MAXPENDING 5

int main(int argc, char *argv[])
{
    int c;
    int proxPort; 
    int verbose; 

    while((c = getopt(argc, argv, "p:v:")) != -1){
        switch(c){
            case 'p':
                proxPort = atoi(optarg);
                break;
            case 'v':
                verbose = atoi(optarg);
                break;
        }
    }
    if(!((verbose == 1) || (verbose == 0))){
        printf("verbose needs to be 1 or 0\n");
        return 0; 
    }

    int proxSock;
    int clntSock;
    struct sockaddr_in proxAddr;
    struct sockaddr_in clntAddr;
    unsigned int clntLen;

    //create socket for proxy server
    if((proxSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP )) < 0){
      printf("socket() failed");
      return 0;
    }

    memset(&proxAddr, 0, sizeof(proxAddr));
    proxAddr.sin_family = AF_INET;
    proxAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    proxAddr.sin_port = htons(proxPort);

    //bind proxy socket
    if(bind(proxSock, (struct sockaddr *) &proxAddr, sizeof(proxAddr)) < 0){
        printf("bind() failed");
        return 0;
    }

    //listen on proxy socket
    if(listen(proxSock, MAXPENDING) < 0){
        printf("listen() failed");
        return 0;
    }

    //accepts client sockets 
    for(;;)
    {
        clntLen = sizeof(clntAddr);

        if((clntSock = accept(proxSock, (struct sockaddr *) &clntAddr, &clntLen)) < 0){
            printf("accept() failed");
            return 0;
        }

        HandleTCPClient(clntSock, verbose);
    }
}

