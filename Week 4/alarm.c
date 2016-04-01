#define LOG_MODULE  LOG_MAIN_MODULE

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <math.h>

#include "log.h"
#include "rtc.h"
#include "alarm.h"
#include "display.h"
#include "mp3stream.h"

#define n 5


struct _snooze
{
	struct _tm snoozeTime;
};

struct _alarm alarm[n];
struct _snooze snooze[n];


int checkAlarms(){
	int i = 0;
	int check = 0;
	for (i = 0; i < n; i++){
		if (alarm[i].time.tm_year == 0){
			alarm[i].state = 0;
		}else{
			setState(i);
			eenmaligAlarmCheck(i);
		}
		if (alarm[i].state == 1 || alarm[i].state == 4){
			check = 1;
		}
	}
	if (check == 1){
		return 1;
	}
	
	return 0;
}

int alarmExist(int id){
	int g;
	for (g = 0; g < n; g++){
		if (alarm[g].id == id){
			return g;
		}
	}
	return -1;
}

struct _alarm* getAlarm(int idx){
	return &alarm[idx];
}

char getRunningAlarmID(){
	char idx;
	for (idx = 0; idx < 5; idx++) {
		if (getState(idx) == 1 || getState(idx) == 4) {
			return idx;
		}
	}
	return -1;
}

int getState(int idx){
	return alarm[idx].state;
}

int maxAlarms(){
	return n;
}

void setSnooze(int idx){
	struct _tm ct;
	X12RtcGetClock(&ct);
	
	if (alarm[idx].state < 3){
		alarm[idx].state = 2;
		snooze[idx].snoozeTime = ct;
		snooze[idx].snoozeTime.tm_min += alarm[idx].snooze;
	}
}

int daysInMonth(int m, int y) {
    if(m == 2 && isLeapYear(y))
        return 29 + (int)(m + floor(m/8)) % 2 + 2 % m + 2 * floor(1/m);
    return 28 + (int)(m + floor(m/8)) % 2 + 2 % m + 2 * floor(1/m);
}

int daysInYear(int y){
    if(isLeapYear(y))
        return 366;
    return 365;
}

int isLeapYear(int y){
    return (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
}


void AddSnoozeMinutes(int idx, int minutes){
	if (snooze[idx].snoozeTime.tm_min + minutes >= 60){ //Checks if minutes is >= 60 else minute
		snooze[idx].snoozeTime.tm_hour += 1;
		snooze[idx].snoozeTime.tm_min = ((snooze[idx].snoozeTime.tm_min + minutes) % 60);
		if (snooze[idx].snoozeTime.tm_hour >= 24){ //Checks if hours is >= 24
			snooze[idx].snoozeTime.tm_hour = 0;
			if ((snooze[idx].snoozeTime.tm_mday + 1) <= daysInMonth((snooze[idx].snoozeTime.tm_mon+1), (snooze[idx].snoozeTime.tm_year+1900))){ //Checks if day+1 smaller or even is to the amount of days in the month
				snooze[idx].snoozeTime.tm_mday += 1;
			} else { //If the days+1 is bigger than the amount of days in the month, day = 1 & month is + 1
				snooze[idx].snoozeTime.tm_mday = 1;
				if (snooze[idx].snoozeTime.tm_mon + 1 > 11){//If month+1 is bigger than 11 (month is 0-11) then month = 0 & year + 1
					snooze[idx].snoozeTime.tm_mon = 0;
					snooze[idx].snoozeTime.tm_year += 1;
				} else {
					snooze[idx].snoozeTime.tm_mon += 1;
				}
			}
		}
	} else {
		snooze[idx].snoozeTime.tm_min += minutes;
	}
}

void setState(int idx){
	struct _tm ct;
	X12RtcGetClock(&ct);
	
	//Set snooze time for snooze alarm
	if (alarm[idx].state == 0  && alarm[idx].state < 3){
		snooze[idx].snoozeTime = alarm[idx].time;
		AddSnoozeMinutes(idx,1);
	}
	
	if (compareTime(ct, alarm[idx].time) >= 1 && alarm[idx].time.tm_year != 0 && alarm[idx].state != 2 && alarm[idx].state < 3){
		if(alarm[idx].state != 1) {
			alarm[idx].state = 1;
			printf("\n\nAlarm gaat nu af!\n\n");
			bool success = connectToStream(alarm[idx].ip, alarm[idx].port, alarm[idx].url);
			if (success == true){
				play();
			}else {
				printf("ConnectToStream failed. Aborting.\n\n");
			}
		}
	} else if (alarm[idx].state != 2 && alarm[idx].state < 3 ){
		alarm[idx].state = 0;
	}
	
	//Check if alarm has to snooze
	if (compareTime(alarm[idx].time,snooze[idx].snoozeTime) >= 1 && alarm[idx].state < 3){
		alarm[idx].state = 2;
	}
	
	//Check if alarm has to snooze
	if (alarm[idx].state == 1 && compareTime(ct, snooze[idx].snoozeTime) >= 1 && alarm[idx].state < 3){
		printf("Alarm stopt nu!");
		alarm[idx].state = 2;
		snooze[idx].snoozeTime = ct;
		AddSnoozeMinutes(idx, alarm[idx].snooze);
		LcdBackLight(LCD_BACKLIGHT_OFF);
		killPlayerThread();
	}
	
	if (alarm[idx].state == 2 && compareTime(ct, snooze[idx].snoozeTime) >= 1 && alarm[idx].state < 3){
		AddSnoozeMinutes(idx, 1);
		printf("Alarm komt nu uit snooze!!");
		if(alarm[idx].state != 1){
			bool success = connectToStream(alarm[idx].ip, alarm[idx].port, alarm[idx].url);
			if (success == true){
				play();
			}else {
				printf("ConnectToStream failed. Aborting.\n\n");
			}
			alarm[idx].state = 1;
		}
	}
	
}



void setAlarm(struct _tm time, char* name, char* ip, u_short port, char* url, int snooze, int id, int idx){
	alarm[idx].time = time;
	
	strncpy(alarm[idx].name, name, sizeof(alarm[idx].name));
	strncpy(alarm[idx].ip, ip, sizeof(alarm[idx].ip));
	alarm[idx].port = port;
	strncpy(alarm[idx].url, url, sizeof(alarm[idx].url));

	alarm[idx].snooze = snooze;
	alarm[idx].id = id;
	alarm[idx].state = 0;
}

void eenmaligAlarm(struct _tm time, char* name, char* ip, u_short port, char* url, int snooze, int id, int idx){
	alarm[idx].time = time;
	
	strncpy(alarm[idx].name, name, sizeof(alarm[idx].name));
	strncpy(alarm[idx].ip, ip, sizeof(alarm[idx].ip));
	alarm[idx].port = port;
	strncpy(alarm[idx].url, url, sizeof(alarm[idx].url));

	alarm[idx].id = id;
	alarm[idx].state = 3;
}


//Checks if there is an alarm that has to go off
void eenmaligAlarmCheck(int idx){
	struct _tm ct;
	X12RtcGetClock(&ct);
	
	//Check if alarm goes off, compares the RTC time to the alarm time
	if (compareTime(ct, alarm[idx].time) >= 1 && alarm[idx].time.tm_year != 0 && alarm[idx].state == 3){
		alarm[idx].state = 4;
		snooze[idx].snoozeTime = ct;
		AddSnoozeMinutes(idx,2);
	}
	
	//Delete alarm after 30 minutes, compares the RTC time to the snooze
	if (compareTime(ct, snooze[idx].snoozeTime) >= 1 && alarm[idx].state == 4){
		deleteAlarm(idx);
		LcdBackLight(LCD_BACKLIGHT_OFF);
		killPlayerThread();
	}
}


void deleteAlarm(int idx){
	struct _tm tm;
	tm.tm_year = 0;
	alarm[idx].time = tm;
	alarm[idx].port = 0;
	alarm[idx].snooze = 5;
	alarm[idx].id = -1;
	alarm[idx].state = -1;
}

void handleAlarm(int idx){
	if (alarm[idx].state < 3){
		alarm[idx].state = 0;
		alarm[idx].time.tm_mday += 1;
	} else if (alarm[idx].state == 4){
		deleteAlarm(idx);
	}
	killPlayerThread();
}

int compareTime(tm t1,tm t2){
    if (t1.tm_year > t2.tm_year){
        return 1;
	}
    if (t1.tm_year == t2.tm_year && t1.tm_mon > t2.tm_mon){
        return 2;
	}
    if (t1.tm_year == t2.tm_year && t1.tm_mon == t2.tm_mon && t1.tm_mday > t2.tm_mday){
        return 3;
	}
    if (t1.tm_year == t2.tm_year && t1.tm_mon == t2.tm_mon && t1.tm_mday == t2.tm_mday && t1.tm_hour > t2.tm_hour){
        return 4;
	}
    if (t1.tm_year == t2.tm_year && t1.tm_mon == t2.tm_mon && t1.tm_mday == t2.tm_mday && t1.tm_hour == t2.tm_hour && t1.tm_min > t2.tm_min){
        return 5;
	}
    if (t1.tm_year == t2.tm_year && t1.tm_mon == t2.tm_mon && t1.tm_mday == t2.tm_mday && t1.tm_hour == t2.tm_hour && t1.tm_min == t2.tm_min &&t1.tm_sec > t2.tm_sec){
        return 6;
	}

    return 0;
}

