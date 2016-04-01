#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/version.h>
#include <dev/irqreg.h>
#include "log.h"
#include "rtc.h"
#include "alarm.h"
#include "display.h"
#include "mp3stream.h"
#include "gotosleep.h"

struct _tm sleepTime;
struct _tm count;
int i = 1;
int x = 0;
int mins;
int hours;
int days;
bool sleepOn = false;
void setSleep(void)
{
	if (sleepOn == true)
	{
		printf("closed1");
			killPlayerThread();
			sleepOn = false;
	}
	else{
	X12RtcGetClock(&count);
	sleepTime = count;
	AddMinutes(60);
	printf("%d", sleepTime.tm_hour);
	playChanel();
	x = sleepTime.tm_min;
	mins = 	sleepTime.tm_min;
		hours = sleepTime.tm_hour;
		days = sleepTime.tm_mday;
	sleepOn = true;
	}
}
void AddMinutes(int minutes){
	if (sleepTime.tm_min + minutes >= 60){ //Checks if minutes is >= 60 else minute
		sleepTime.tm_hour += 1;
		sleepTime.tm_min = ((sleepTime.tm_min + minutes) % 60);
		if (sleepTime.tm_hour >= 24){ //Checks if hours is >= 24
			sleepTime.tm_hour = 0;
			if ((sleepTime.tm_mday + 1) <= daysInMonth((sleepTime.tm_mon+1), (sleepTime.tm_year+1900))){ //Checks if day+1 smaller or even is to the amount of days in the month
				sleepTime.tm_mday += 1;
			} else { //If the days+1 is bigger than the amount of days in the month, day = 1 & month is + 1
				sleepTime.tm_mday = 1;
				if (sleepTime.tm_mon + 1 > 11){//If month+1 is bigger than 11 (month is 0-11) then month = 0 & year + 1
					sleepTime.tm_mon = 0;
					sleepTime.tm_year += 1;
				} else {
					sleepTime.tm_mon += 1;
				}
			}
		}
	} else {
		sleepTime.tm_min += minutes;
	}
}
void checkSleep(void)
{
	X12RtcGetClock(&count);
	if(count.tm_min == mins && count.tm_hour == hours && count.tm_mday == days){
		printf("closed");
		killPlayerThread();
		volumeUp();
		volumeUp();
		volumeUp();
		volumeUp();
		volumeUp();
		volumeUp();

		sleepOn = false;
	}
	if(count.tm_min == x){
		volumeDown();
		if (x >= 60){
			x = 0;
		}
		x += 10;
	}


}
void changeChanel(void){
	i++;
	if (i > 5){
		i = 1;
	}
}
void playChanel(void){

	switch(i) {
   case 1  :
      connectToStream("62.195.226.247",80,"/test7.mp3");
	  play();
      break;
   case 2  :
      connectToStream("62.195.226.247",80,"/test5.mp3");
	  play();
      break;
		case 3  :
			connectToStream("62.195.226.247",80,"/test4.mp3");
			play();
			break;
		case 4  :
			connectToStream("62.195.226.247",80,"/test10.mp3");
			play();
			break;
		case 5  :
			connectToStream("62.195.226.247",80,"/test11.mp3");
			play();
			break;
}

}
