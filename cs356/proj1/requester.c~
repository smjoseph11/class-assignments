//Matthew Lacamu
//Stanlin Joseph
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
int main (int argc, char **argv){
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    int c;
    char *serverip = NULL;
    int port;
    char *filename = NULL;
    int messageCount;
    //parses command line arguments
    while ((c = getopt (argc, argv, "a:p:f:n:")) != -1){
        switch (c)
        {
            case 'a':
            serverip = optarg;
            break;
            case 'p':
            port = atoi(optarg);
            break;
            case 'f':
            filename = optarg;
            break;
            case 'n':
            messageCount = atoi(optarg);
            break;
            case '?':
            break;
            case ':':
            printf("error!\n");
            break;
            return 1;
            default:
            printf("getopt() returned %c\n", c);
        }
    }
    int times[messageCount]; 
    FILE *txtfile;
    txtfile = fopen(filename, "r");
    struct sockaddr_in addr;
    memset((void *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    addr.sin_addr.s_addr = INADDR_ANY;
    //binds socket
    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        printf("didn't bind\n");
        return 0;
    }
    struct sockaddr_in servaddr;
    socklen_t addrlen = sizeof(servaddr);
    memset((char *)&servaddr, 0, sizeof(addr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = port;
    inet_aton(serverip, &servaddr.sin_addr);
    int i;
    bool atEnd = false;
    //starts timer
    clock_t start = clock();
    //sends message to server messageCount number of times
    for(i = 0; i<messageCount; i++){
        int packetCount = 0;
        int startWindow = 0;
        int endWindow = startWindow + 4;
        unsigned char buffer[304];
        memset(buffer, 0, sizeof(buffer));
        atEnd = false;
        fseek(txtfile, 0, SEEK_SET);
        //client loop that sends full text file in packets
        while(!atEnd)
        {
            //sends a window's length of packets
            while(startWindow != (endWindow + 1)){
                if(fgets(buffer, 300, txtfile) != NULL)
                {
                    int * index = (int*)(&buffer[300]);
                    *index = packetCount;
                    if(sendto(sock, buffer, 304, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
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
            //listens for request for next window of packets from server
            for(;;)
            {
                int meslen = recvfrom(sock, buffer, 304, 0, (struct sockaddr *)&servaddr, &addrlen);
                if(meslen > 0)
                {
                    int * packetRecieved = (int *) (&buffer[300]);
                    //resends packets starting from requested packet number
                    if(*packetRecieved < endWindow)
                    {
                        startWindow = *packetRecieved;
                        fseek(txtfile, ((*packetRecieved - (endWindow + 1))*300), SEEK_CUR);
                        break;
                    }
                    //sets new end and start of window when server requests
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
        //sends EOF char to indicate entire file has been sent 
        if(sendto(sock, buffer, 304, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
            printf("couldn't send EOF buffer\n");
            return 0;
        }
        memset(buffer, 0, sizeof(buffer));
        int expectedPacket = 0;
        endWindow = 4;
        printf("\n\nMessage %d:\n", i);
        //listens for server to echo packets back to client
        for(;;){
            memset(buffer, 0, sizeof(buffer));
            int meslen = recvfrom(sock, buffer, 304, 0, (struct sockaddr *)&servaddr, &addrlen);
            if(meslen > 0)
            {
                //if EOF char is read, break out of the loop
                if(buffer[0] == '\0')
                {
                    expectedPacket = 0;
                    endWindow = 4;
                    break;
                }
                else
                {
                    int * packetRecieved = (int *) (&buffer[300]);
                    //prints text from packets if expected packet is recieved
                    if(*packetRecieved == expectedPacket)
                    {
                        buffer[300] = '\0';
                        printf("%s", buffer);
                        expectedPacket++;
                        //adjusts end of window if window is complete, sends request for next window to server
                        if(expectedPacket == (endWindow + 1))
                        {
                            endWindow = endWindow + 5;
                            memset(buffer, 0, sizeof(buffer));
                            int * index = (int*)(&buffer[300]);
                            *index = expectedPacket;
                            if(sendto(sock, buffer, 304, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
                            {
                                printf("couldn't send\n");
                                return 0;
                            }
                        }
                    }
                    //if unexpected packet is recieved, send request for expected packet
                    else
                    {
                        memset(buffer, 0, sizeof(buffer));
                        int * index = (int*)(&buffer[300]);
                        *index = expectedPacket;
                        if(sendto(sock, buffer, 304, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
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
        }
        //records timing data
        if(i == 0)
        {
            times[0] = clock() - start;
        }
        else
        {
            times[i] = clock() - times[i-1];
        }
        printf("\nRTT %d ms\n", (int)(times[i] * 1000 / CLOCKS_PER_SEC));
    }
    printf("\n");
    close(sock);
    double avg = 0;
    int j;
    int min = times[0];
    int max = times[0];
    //calculates min, max, and avg times
    for(j = 0; j < messageCount; j++)
    {
        avg += times[j];
        if(times[j] < min)
            min = times[j];
        if(times[j] > max)
            max = times[j];
    }
    printf("AveRTT: %.1f ms MinRTT: %d ms MaxRTT: %d ms\n", (double) (avg/messageCount * 1000 / CLOCKS_PER_SEC) , (int)(min * 1000 / CLOCKS_PER_SEC), (int)(max * 1000 / CLOCKS_PER_SEC));
}
