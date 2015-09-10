//Stanlin Joseph
//Matthew Lacamu
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#define BUFSIZE 304
int main(int argc, char **argv){
    int c;
    int port;
    //reads argument from command line
    if((c = getopt(argc, argv, "p:")) != -1){
        switch(c)
        {
            case 'p':
            port = atoi(optarg);
            break;
            case '?':
            break;
            default:
            printf("getopt() returned %c\n", c);
        }
    }
    else{
        printf("need to put in an argument\n");
        return 0;
    }
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr, clientaddr;
    socklen_t addrlen = sizeof(clientaddr);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = INADDR_ANY;
    
    FILE *servFile;
    
    //binds socket
    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        printf("didn't bind\n");
        return 0;
    }
    unsigned char buffer[BUFSIZE];
    int expectedPacket = 0;
    int endWindow = 4;
    //listens for packets from client and echoes them back when the file is complete
    for(;;){
        servFile = fopen("servtext" , "a+");
        int meslen = recvfrom(sock, buffer, BUFSIZE, 0, (struct sockaddr *)&clientaddr, &addrlen);
        if(meslen > 0)
        {
            //if end of file character is read echo back file
            if(buffer[0] == '\0')
            {
                int packetCount = 0;
                int startWindow = 0;
                endWindow = startWindow + 4;
                memset(buffer, 0, sizeof(buffer));
                bool atEnd = false;
                fseek(servFile, 0, SEEK_SET);
                //server loop that sends full text in packets
                while(!atEnd)
                {
                    //send a window's length of packets
                    while(startWindow != (endWindow + 1)){
                        if(fgets(buffer, 300, servFile) != NULL)
                        {
                            int * index = (int*)(&buffer[300]);
                            *index = packetCount;
                            printf("%s", buffer);
                            if(sendto(sock, buffer, 304, 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0){
                                printf("couldn't send\n");
                                return 0;
                            }
                            memset(buffer, 0, sizeof(buffer));
                            packetCount++;
                            startWindow++;
                        }
                        else
                        {
                            atEnd = true;
                            break;
                        }
                    }
                    if(atEnd)
                    {
                        break;
                    }
                    //listens for request for next window of packet from client 
                    for(;;)
                    {
                        int meslen = recvfrom(sock, buffer, 304, 0, (struct sockaddr *)&clientaddr, &addrlen);
                        if(meslen > 0)
                        {
                            int * packetRecieved = (int *) (&buffer[300]);
                            //resends packets starting from requested packet
                            if(*packetRecieved < endWindow)
                            {
                                startWindow = *packetRecieved;
                                fseek(servFile, ((*packetRecieved - (endWindow + 1))*300), SEEK_CUR);
                                break;
                            }
                            //sets new end and start of window whenever client requests
                            else if(*packetRecieved == (endWindow + 1))
                            {
                                startWindow = endWindow + 1;
                                endWindow = startWindow + 4;
                                break;
                            }
                            else
                            {
                                printf("packetRecieved > endWindow+1\n");
                                break;
                            }
                        }
                        else{
                            printf("meslen<=0\n");
                            break;
                        }
                    }
                }
                buffer[0] = '\0';
                
                expectedPacket = 0;
                endWindow = 4;
                printf("\n");

                //sends EOF character to indicate entire file has been sent 
                if(sendto(sock, buffer, 304, 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0) {
                    printf("couldn't send EOF buffer\n");
                    return 0;
                }
                remove("servtext");
            }
            //write packet contents to text file
            else
            {
                int * packetRecieved = (int *) (&buffer[300]);
                //writes to file if expected packet is recieved
                if(*packetRecieved == expectedPacket)
                {
                    buffer[300] = '\0';
                    int numChars = fprintf(servFile, "%s", buffer);
                    expectedPacket++;
                    //adjusts window if end of window is reached, and requests next window  
                    if(expectedPacket == (endWindow + 1))
                    {
                        endWindow = endWindow + 5;
                        memset(buffer, 0, sizeof(buffer));
                        int * index = (int*)(&buffer[300]);
                        *index = expectedPacket;
                        if(sendto(sock, buffer, 304, 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0)
                        {
                            printf("couldn't send\n");
                            return 0;
                        }
                    }
                }
                //if unexpected packet request expected packet
                else
                {
                    memset(buffer, 0, sizeof(buffer));
                    int * index = (int*)(&buffer[300]);
                    *index = expectedPacket;
                    if(sendto(sock, buffer, 304, 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0)
                    {
                        printf("couldn't send\n");
                        return 0;
                    }
                }
            }
        }
        else{
            printf("meslen<=0\n");
        }
        memset(buffer, 0, sizeof(buffer));
        fclose(servFile);
    }
    close(sock);
}
