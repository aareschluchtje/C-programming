/* Alarm get/set status values */

#ifndef _ALARM_DEFINED
#define _ALARM_DEFINED
struct _alarm
{
	struct _tm time;
	char ip[24];
	u_short port;
	char url[24];
	char name[16];
	int snooze;
	int id;
	int state;
};

void handleAlarm(int idx);
int checkAlarms(void);
void setAlarm(struct _tm time, char* name, char* ip, u_short port, char* url, int snooze, int id, int idx);
int alarmExist(int id);
void deleteAlarm(int idx);
void eenmaligAlarmCheck(int idx);
void eenmaligAlarm(struct _tm time, char* name, char* ip, u_short port, char* url, int snooze, int id, int idx);
int compareTime(tm t1, tm t2);
void setState(int idx);
int getState(int idx);
struct _alarm* getAlarm(int idx);
char getRunningAlarmID(void);
int maxAlarms(void);
int isLeapYear(int y);
void setSnooze(int idx);
#endif
