//
// Created by Jordy Sipkema on 26/02/16.
//
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "display.h"
#include "displayHandler.h"
#include "ntp.h"
#include "log.h"
#include "mp3stream.h"
#include "rtc.h"
#include "alarm.h"
#include "network.h"
#include "twitch.h"
#include "Twitter.h"

struct _tm lastDisplayTime;
viewDisplays currentViewDisplay;
u_long displayTime;

void setCurrentDisplay(viewDisplays d, u_long dt){
    X12RtcGetClock(&lastDisplayTime);
    LcdBackLight(LCD_BACKLIGHT_ON);
    currentViewDisplay = d;
    displayTime = dt;
}

viewDisplays getCurrentDisplay(void){
    return currentViewDisplay;
}

void refreshScreen(){
    if(timerStruct(lastDisplayTime) > displayTime && currentViewDisplay != DISPLAY_Alarm){
        currentViewDisplay = DISPLAY_DateTime;
        LcdBackLight(LCD_BACKLIGHT_OFF);
    }

    if(currentViewDisplay == DISPLAY_DateTime){
        displayDateTime();
    } else if(currentViewDisplay == DISPLAY_Volume){
        displayVolume();
    } else if(currentViewDisplay == DISPLAY_Alarm){
        displayAlarm(getRunningAlarmID());
    } else if(currentViewDisplay == DISPLAY_Twitch){
        displayTwitch(data.name, data.title, data.game);
    } else if(currentViewDisplay == DISPLAY_Twitter){
        displayTwitter(TweetFeed.tweet);
    } else if(currentViewDisplay == DISPLAY_StreamInfo){
        displayStreamInfo();
    }
}

long timerStruct(struct _tm s){
    struct _tm ct;
    X12RtcGetClock(&ct);

    long stime = (s.tm_hour * 3600) + (s.tm_min * 60) + s.tm_sec;
    long ctime = (ct.tm_hour * 3600) + (ct.tm_min * 60) + ct.tm_sec;

 /*   if(ctime < 0){
        return 0;
    }*/
    
    return ctime - stime;
}

void (*write_display_ptr[2])(char*, int) = {LcdArrayLineOne, LcdArrayLineTwo};

void displayDateTime(void){
    tm time;
    X12RtcGetClock(&time);

    char str1[16];
    char str2[16];
    if (1){
        sprintf(str1, "    %02d:%02d:%02d    ", time.tm_hour, time.tm_min, time.tm_sec);
        sprintf(str2, "   %02d-%02d-%04d      ", time.tm_mday, time.tm_mon + MONTH_OFFSET, time.tm_year + YEAR_OFFSET);
    } else {
        sprintf(str1, "    ??:??:??    ");
        sprintf(str2, "    ??:??:??    ");
    }
    if (NtpIsSyncing()) {
        str2[1] = 'S';
    } else if(NetworkIsReceiving()){
        str2[1] = 'N';
    }

    (*write_display_ptr[0])(str1, 16);
    (*write_display_ptr[1])(str2, 16);
}

void displayAlarm(char idx)
{
    if (idx == -1){
        currentViewDisplay = DISPLAY_DateTime;
    }
	int i;

    char str[16];
    struct _alarm *am = getAlarm(idx);

    sprintf(str, "    %02d:%02d:%02d    ", am->time.tm_hour, am->time.tm_min, am->time.tm_sec);
    (*write_display_ptr[0])(str, 16);

    char str2[16];
	for (i = 0; i < 16;i++){
		str2[i] = am->name[i];
	}
		

    (*write_display_ptr[1])(str2, 16);
}

void displayVolume()
{
    u_char pos = getVolume();
    int i;
    LcdArrayLineOne("     Volume     ", 16);

    char characters[17];

    for(i = 0; i < 17; i++)
    {
        if(i < pos) {
            characters[i] = 0xFF;
        }else {
            characters[i] = ' ';
        }
    }
    LcdArrayLineTwo(characters,16);
}

void displayTwitter(char* text)
{
    ClearLcd();
    LcdBackLight(LCD_BACKLIGHT_ON);
    LcdArrayLineOne("     Twitter    ", 16);
    int j = 0;
    int i;
    char text1[16];
    for(i = 0; i<140;i++){
        if (text[i] != 0){
            j++;
        }
    }

        for(i = 0; i < 16; i++){
            if (text[Scroller+i]!= 0) {
                text1[i] = text[Scroller + i];
            } else {
                text1[i] = ' ';
            }
        }
        LcdArrayLineTwo(text1,16);
        Scroller++;
        if (Scroller > j){
            Scroller = 0;
        }
}

void displayTwitch(char name[], char title[], char game[]) {

    if (timerStruct(lastDisplayTime) % 10 < 5) {
        ClearLcd();
        if(strlen(name) < 16) {
            LcdArrayLineOne(name, strlen(name));
        }else {
            LcdArrayLineOne(name, 16);
        }
        LcdArrayLineTwo("is streaming    ", 16);
    }
    else {
        ClearLcd();
        if(strlen(title) > 16) {
            LcdArrayLineOne(title, 16);
        }else {
            LcdArrayLineOne(title, strlen(title));
        }if(strlen(game) > 16) {
            LcdArrayLineTwo(game, 16);
        }else{
            LcdArrayLineTwo(game, strlen(game));
        }
    }
    LcdBackLight(LCD_BACKLIGHT_ON);
}

void displayStreamInfo(){
    LcdBackLight(LCD_BACKLIGHT_ON);
    char offset = getScrollOffset();


    if (offset == 0)
        (*write_display_ptr[1])("                ", 17);

    (*write_display_ptr[0])("  Station Info  ", 17);

    //char* streamInfo = getStreamInfo();

    // I have to copy the StreamInfo buffer, I kept overwriting it.
    char streamInfo[48] = "    No  info    ";
    char* streamInfoPtr = &streamInfo[0];
    strncpy(streamInfo, getStreamInfo(), 48); // copy the streamInfo buffer.
    streamInfo[48] = '\0'; // To be sure...

    if (offset >= 0)
        streamInfoPtr += offset;
    streamInfoPtr[16] = '\0';

    (*write_display_ptr[1])(streamInfoPtr, 17);

    incrementScrollOffset();

    NutDelay(500);
}
