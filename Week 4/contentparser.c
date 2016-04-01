//
// Created by janco on 25-3-16.
//
#include "contentparser.h"
#include "ntp.h"
#include "network.h"
#include "jsmn.h"
#include "mp3stream.h"
#include "rtc.h"
#include "alarm.h"
#include "menuHandler.h"
#include "vs10xx.h"
#include "twitch.h"
#include "Twitter.h"
#include <string.h>

void parseAlarmJson(char* content){
    int r;
    int i = 2;
	
	int startidx = 0;
	int charAmount = 0;

    int usedAlarms[maxAlarms()];
    int j;
    
    jsmn_parser p;
    jsmntok_t token[160]; /* We expect no more than 128 tokens */

    jsmn_init(&p);
    r = jsmn_parse(&p, content, strlen(content), token, sizeof(token)/sizeof(token[0]));
    if (r <= 0) {
        printf("Failed to parse JSON: %d \n", r);
        return;
    }else{
        printf("Aantal tokens found: %d \n", r);
    }


    struct _tm time = GetRTCTime();
    for(j = 0; j < maxAlarms(); j++){
        usedAlarms[j] = 0;
    }
    for(i = 2; i < r; i++)
    {
        int id = 0;
        u_short port = 0;
        char url[24];
        char ip[24];
        char name[17];
        char str2[16];
        char st = -1;
        char oo = -1;
        memset(url, 0, 24);
        memset(ip, 0, 24);
        memset(name, 0, 17);

        for (i = i; (st == -1 && i < r); i+=2) {                                //Zodra ST is gevonden, betekent dit de laatste token van een alarm.
            if (jsoneq(content, &token[i], "YYYY") == 0) {
                time.tm_year= getIntegerToken(content, &token[i + 1]) - 1900;
            }else if (jsoneq(content, &token[i], "MM") == 0) {
                time.tm_mon=  getIntegerToken(content, &token[i + 1]) - 1;
            }else if (jsoneq(content, &token[i], "DD") == 0) {
                time.tm_mday =  getIntegerToken(content, &token[i + 1]);
            }else if (jsoneq(content, &token[i], "hh") == 0) {
                time.tm_hour = 	getIntegerToken(content, &token[i + 1]);
            }else if (jsoneq(content, &token[i], "mm") == 0) {
                time.tm_min = getIntegerToken(content, &token[i + 1]);
            }else if (jsoneq(content, &token[i], "ss") == 0) {
                time.tm_sec = getIntegerToken(content, &token[i + 1]);
            }else if (jsoneq(content, &token[i], "id") == 0) {
                id = getIntegerToken(content, &token[i + 1]);
            }else if (jsoneq(content, &token[i], "port") == 0) {
                port = getIntegerToken(content, &token[i + 1]);
            }else if (jsoneq(content, &token[i], "ip") == 0) {
                getStringToken(content, &token[i + 1], ip, 24);
            }else if (jsoneq(content, &token[i], "url") == 0) {
                getStringToken(content, &token[i + 1], url, 24);
            }else if (jsoneq(content, &token[i], "name") == 0) {
                getStringToken(content, &token[i + 1], name, 18);
            }else if (jsoneq(content, &token[i], "oo") == 0) {
                oo = getIntegerToken(content, &token[i + 1]);
            }else if (jsoneq(content, &token[i], "st") == 0) {
                st = getIntegerToken(content, &token[i + 1]);
            }
        }

        int idx = alarmExist(id);
        if(idx == -1){
            printf("New alarm found!\n");
            printf("Alarm time is: %02d:%02d:%02d\n", time.tm_hour, time.tm_min, time.tm_sec);
            printf("Alarm date is: %02d.%02d.%02d\n", time.tm_mday, (time.tm_mon + 1), (time.tm_year + 1900));
            printf("Alarm stream data is: %s:%d%s\n", ip, port, url);
            printf("Alarm id and name and st is: %d %s %d\n\n", id, name, st);
			
			charAmount = 0;
			for (i = 0; i < 16;i++){
				if (name[i] != 0){
					charAmount = charAmount + 1;
				}
			}
			
			startidx = (8-(charAmount/2));
			
			charAmount = 0;
			for(i = 0; i < 16; i++){
				if (i >= startidx){
					if (name[charAmount] != 0){
						str2[i] = name[charAmount];
					} else {
						str2[i] = ' ';
					}	
					charAmount++;
				} else {
					str2[i] = ' ';
				}
			}
			

            //zoek naar een vrije plaats in de alarm array
            for(j = 0; j < maxAlarms(); j++){
                if(usedAlarms[j] == 0){ //Dit is een lege plaats, hier kunnen we ons nieuwe alarm plaatsen
					if (oo == 1){
						eenmaligAlarm(time,str2,ip,port,url,st,id,j);
					} else {
						setAlarm(time, str2, ip, port, url, st, id, j);
					}
                    usedAlarms[j] = 1;
                    j = 10;             //Uit de for loop
                }
            }
        }else{
            usedAlarms[idx] = 1; //Alarm bestaat al, dus we houden deze plaats vrij voor dat alarm
        }
    }
    for(j = 0; j < maxAlarms(); j++){ //Alle overige plaatsen, die wij niet gezet hebben, verwijderen.
        if(usedAlarms[j] == 0){
            deleteAlarm(j);
        };
    }
}

void parseCommandQue(char* content){
    int r;
    int i;
    jsmn_parser p;
    jsmntok_t token[150]; /* We expect no more than 128 tokens */

    jsmn_init(&p);
    r = jsmn_parse(&p, content, strlen(content), token, sizeof(token)/sizeof(token[0]));
    if (r <= 0) {
        printf("Failed to parse JSON: %d \n", r);
        return;
    }else{
        printf("Aantal tokens found: %d \n", r);
    }

    for(i = 0; i < r; i++)
    {
        if (jsoneq(content, &token[i], "command") == 0) {
            if(jsoneq(content, &token[i + 1], "volume") == 0){
                char vol = getIntegerToken(content, &token[i + 3]);
                printf("Updating volume: \n");
                setVolumeManual(vol);
                //vol = 128 - ((vol * 128) / 100);
                //VsSetVolume(vol, vol);
                i += 3;
            }else if(jsoneq(content, &token[i + 1], "stopstream") == 0){
                killPlayerThread();
                i += 3;
            }else if(jsoneq(content, &token[i + 1], "startstream") == 0){
                u_short port = getIntegerToken(content, &token[i + 9]);
                char url[24];
                char ip[24];
                getStringToken(content, &token[i + 7], url, 24);
                getStringToken(content, &token[i + 5], ip, 24);
                bool success = connectToStream(ip, port, url);
                if (success == true){
                    play();
                }else {
                    printf("ConnectToStream failed. Aborting.\n\n");
                }
                i += 9;
            }
        }
    }
}

void parsetimezone(char* content)
{
    int timezone = atoi(content); //parsing string to int (only works when everything is int)
    if(strlen(content) == 0)
    {
        setTimeZone(50);
    }
    else {
        setTimeZone(timezone);
    }
}

void parseTwitch(char* content) {
    if (!strcmp("null", content)) {
        printf("Nobody is streaming");
        return;
    }
    int r;
    int i;
    jsmn_parser p;
    jsmntok_t token[20]; /* We expect no more than 20 tokens */

    jsmn_init(&p);
    r = jsmn_parse(&p, content, strlen(content), token, sizeof(token) / sizeof(token[0]));
    if (r <= 0) {
        printf("Failed to parse JSON: %d \n", r);
        return;
    } else {
        printf("Aantal tokens found: %d \n", r);
    }

    char name[20];
    char title[20];
    char game[20];
    char date[15];
    memset(name, 0, 20);
    memset(title, 0, 20);
    memset(game, 0, 20);
    memset(date, 0, 15);

    for (i = 1; i < r; i++) {
        if (jsoneq(content, &token[i], "Name") == 0) {
            getStringToken(content, &token[i + 1], name, 20);
            i++;
        }
        else if (jsoneq(content, &token[i], "Title") == 0) {
            getStringToken(content, &token[i + 1], title, 20);
            i++;
        }
        else if (jsoneq(content, &token[i], "Game") == 0) {
            getStringToken(content, &token[i + 1], game, 20);
            i++;
        }
        else if (jsoneq(content, &token[i], "Date") == 0) {
            getStringToken(content, &token[i + 1], date, 15);
            i++;
        }
    }
    printf("%s", date);
    if(strncmp(date, streamid, 15) != 0)
    {
        strcpy(data.title, title);
        strcpy(data.game, game);
        strcpy(data.name, name);
        printf("%s - %s - %s", name, title, game);
        strcpy(streamid, date);
        setCurrentDisplay(DISPLAY_Twitch, 100);
    }
}

void TwitterParser(char* content)
{
    char tweet[140];
    memset(tweet, 0, 140);
    strcpy(TweetFeed.tweet,content);
    printf("%s", TweetFeed.tweet);
}
