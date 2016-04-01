//
// Created by janco on 25-2-16.
//
#include <dev/board.h>
#include <dev/debug.h>
#include <dev/nvmem.h>

#include <sys/timer.h>

#include <arpa/inet.h>
#include <net/route.h>
#include <pro/dhcp.h>
#include <pro/sntp.h>

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <time.h>

#include "log.h"
#include "ntp.h"
#include "contentparser.h"
#include "alarm.h"
#include "network.h"

int TIME_ZONE = 1;
#define LOG_MODULE  LOG_NTP_MODULE

typedef struct _Eeprom_tm {
    size_t len;
    tm tm_struct;
} Eeprom_tm;

bool isSyncing;
bool validTime = false;
time_t ntp_time = 0;
tm *ntp_datetime;
uint32_t timeserver = 0;

void NtpInit(void) {
    puts("Func: NtpInit(void)");
    /* Timezone van nederland (gmt 1) */
    _timezone = -TIME_ZONE * 3600;
    NtpCheckValidTime();
}

bool NtpIsSyncing(void){
    return isSyncing;
}

void NtpCheckValidTime(void){

    Eeprom_tm eeprom_tm_struct;

    NutNvMemLoad(256, &eeprom_tm_struct, sizeof(eeprom_tm_struct));

    if (eeprom_tm_struct.len != sizeof(eeprom_tm_struct)){
        // Size mismatch: There is no valid configuration present.
        puts("NtpCheckValidTime(): Size mismatch \n");
        validTime = false;
        return;
    }

    // Valid configuration available.
    puts("NtpCheckValidTime(): Valid config available \n");
    tm stored_tm = eeprom_tm_struct.tm_struct;

    // Check time is valid;
    tm current_tm;
    X12RtcGetClock(&current_tm);

    validTime = compareTime(current_tm, stored_tm);
    if (validTime){
        puts("NtpCheckValidTime(): Time was valid \n");
    }else {
        puts("NtpCheckValidTime(): Invalid time! \n");
    }
}

bool NtpTimeIsValid(void){
    return validTime;
}

int NtpSync(void){
    /* Ophalen van pool.ntp.org */
    isSyncing = true;
    setTimeZone(50);
    httpGet("/gettimezone.php", parsetimezone);
    _daylight = 0;
    printf("Timezone is: %d", TIME_ZONE);
    if(TIME_ZONE == 50)
    {
        return 0;
    }
    NutDelay(100);
    //puts("Tijd ophalen van pool.ntp.org (213.154.229.24)");
    timeserver = inet_addr("213.154.229.24");

    for (;;) {
        if (NutSNTPGetTime(&timeserver, &ntp_time) == 0) {
            break;
        } else {
            NutSleep(400);
            puts("Fout bij het ontvangen van de tijd");
        }
    }
    //puts("Opgehaald.\n");

    ntp_datetime = localtime(&ntp_time);

    printf("NTP time is: %02d:%02d:%02d\n", ntp_datetime->tm_hour, ntp_datetime->tm_min, ntp_datetime->tm_sec);
    printf("NTP date is: %02d.%02d.%02d\n\n", ntp_datetime->tm_mday, (ntp_datetime->tm_mon + 1), (ntp_datetime->tm_year + 1900));

    X12RtcSetClock(ntp_datetime);
    NtpWriteTimeToEeprom(*ntp_datetime);

    isSyncing = false;
    validTime = true;
    return 1;
}

void NtpWriteTimeToEeprom(tm time_struct){
    Eeprom_tm eeprom_tm_struct;

    eeprom_tm_struct.len = sizeof(eeprom_tm_struct);
    eeprom_tm_struct.tm_struct = time_struct;

    int success = NutNvMemSave(256, &eeprom_tm_struct, sizeof(eeprom_tm_struct));
    if (success == 0){ puts("NtpWriteTimeToEeprom: Time succesfully written to eeprom \n"); }

    NutDelay(100);
}

void setTimeZone(int timezone){
    TIME_ZONE = timezone;
    _timezone = -1*timezone * 3600;
}

int getTimeZone(){
    return TIME_ZONE;
}
