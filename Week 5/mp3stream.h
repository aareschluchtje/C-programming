//
// Created by Jordy Sipkema on 28/03/16.
//

#ifndef MUTLI_OS_BUILD_MP3STREAM_H
#define MUTLI_OS_BUILD_MP3STREAM_H

#include "typedefs.h"

#include <netinet/tcp.h>
#include <sys/socket.h>

#include <stdio.h>
#include <string.h>

bool play(void);
bool connectToStream(char* ipaddr, u_short port, char *radioUrl);
void killPlayerThread(void);

void volumeUp(void);
void volumeDown(void);
void setVolumeManual(char);
void setVolume(void); // Do not use this one, this is invoked by volumeUp/Down
u_char getVolume(void);
char* getStreamInfo(void);
char getScrollOffset(void);
void incrementScrollOffset(void);

#endif //MUTLI_OS_BUILD_MP3STREAM_H
