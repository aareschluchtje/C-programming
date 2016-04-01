//
// Created by Jordy Sipkema on 26/02/16.
//

#ifndef MUTLI_OS_BUILD_DISPLAYHANDLER_H
#define MUTLI_OS_BUILD_DISPLAYHANDLER_H
#include "ntp.h"

#include <time.h>
#include "alarm.h"
#define MONTH_OFFSET 1
#define YEAR_OFFSET 1900

typedef enum {
    DISPLAY_DateTime,
    DISPLAY_Alarm,
    DISPLAY_Volume,
    DISPLAY_Twitch,
    DISPLAY_Twitter,
    DISPLAY_StreamInfo,
} viewDisplays;

long timerStruct(struct _tm s);
void setCurrentDisplay(viewDisplays d, u_long dt);
viewDisplays getCurrentDisplay(void);
void refreshScreen(void);

void displayDateTime(void);
void displayAlarm(char idx);
void displayVolume(void);
void displayTwitter(char text[]);
void displayTwitch(char name[], char title[], char game[]);
void displayStreamInfo(void);

#endif //MUTLI_OS_BUILD_DISPLAYHANDLER_H
