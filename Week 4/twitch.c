//
// Created by aares on 30-3-2016.
//

#include "twitch.h"
#include <string.h>

char streamid[15];
struct streamdata data;

void setID(char* id)
{
    strcpy(streamid, id);
}

char* getID()
{
    return streamid;
}