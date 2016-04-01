//
// Created by janco on 25-2-16.
//
#include <dev/board.h>
#include <dev/debug.h>
#include <sys/timer.h>
#include <sys/confnet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <net/route.h>

#include <dev/nicrtl.h>

#include <stdio.h>
#include <io.h>
#include <sys/version.h>
#include <dev/irqreg.h>
#include <arpa/inet.h>
#include <pro/dhcp.h>
#include <pro/sntp.h>

#include "ntp.h"
#include "network.h"
#include "jsmn.h"
#include "rtc.h"
#include "alarm.h"
#include "contentparser.h"

bool isReceiving;
bool hasNetwork;

void NetworkInit() {
    hasNetwork = false;
    /* Register de internet controller. */
    if (NutRegisterDevice(&DEV_ETHER, 0, 0)) {
        printf("Registering  failed. \n");
    }/* Netwerk configureren op dhcp */
    else if (NutDhcpIfConfig(DEV_ETHER_NAME, NULL, 0)) {
        printf("DHCP failed. \n");
    }else {
        printf("Ik heb een internet connectie. Ip is: %s \nMac address is:  %s\n\n", inet_ntoa(confnet.cdn_ip_addr),  ether_ntoa(confnet.cdn_mac));
    }
    NutSleep(2000);
    hasNetwork = true;
}

char* getMacAdress(){
    return ether_ntoa(confnet.cdn_mac);
}

void httpGet(char address[], void (*parser)(char*)){
    u_long rx_to = 10000;
    isReceiving = true;
    printf("\n\n #-- HTTP get -- #\n");

    TCPSOCKET* sock = NutTcpCreateSocket();

    char buffer[2];
    char* content = (char*) malloc(900);
    char enters = 0;
    int t = 0;

    if(content == 0){
        printf("Can't malloc memory\n");
    }else if (NutTcpConnect(sock, inet_addr("62.195.226.247"), 80)) {
        printf("Can't connect to server\n");
    }else if (NutTcpSetSockOpt(sock, SO_RCVTIMEO, &rx_to, sizeof(rx_to))){
        printf("Can't set sock options\n");
    }else{
        FILE *stream;
        stream = _fdopen((int) sock, "r+b");

        //Writing http GET to stream
        fprintf(stream, "GET %s HTTP/1.1\r\nHost: saltyradio.jancokock.me \r\n\r\n", address);
        fflush(stream);

        printf("Headers writed. Now reading.");
        //Removing header:
        while(fgets(buffer, sizeof(buffer), stream) != NULL) {
            if(enters == 4) {
                content[t] = buffer[0];
                t++;
            }else {
                if (buffer[0] == '\n' || buffer[0] == '\r') {
                    enters++;
                }
                else {
                    enters = 0;
                }
            }
        }
        fclose(stream);

        content[t] = '\0';
        printf("\nContent size: %d, Content: %s \n", t, content);
        parser(content);
    }
    free(content);
    NutTcpCloseSocket(sock);
    isReceiving = false;
}

bool NetworkIsReceiving(void){
    return isReceiving;
}

bool hasNetworkConnection(void){
    return hasNetwork;
}
