//
// Created by aares on 1-4-2016.
//

#include "mainMenu.h"
#include "display.h"
#include "settings.h"
#include "play.h"
#include "time.h"
#include "menuHandler.h"
#include "ntp.h"

int isSettings = 0;

void showMenu()
{
    ClearLcd();
    LcdArrayLineOne("Main menu", 9);
    switch (isSettings)
    {
        case 0:
        {
            tm time;
            X12RtcGetClock(&time);
            char str1[16];
            sprintf(str1, "<   %02d:%02d:%02d   >", time.tm_hour, time.tm_min, time.tm_sec);
            LcdArrayLineTwo(str1, 16);
            break;
        }
        case 1:
        {
            showSettings();
            break;
        }
        case 2:
        {
            showPlay();
            break;
        }
    }
}

void switchItem()
{
    if(isSettings < 2)
    {
        isSettings++;
    }
    else
    {
        isSettings = 0;
    }
}

void switchLeft()
{
    if(isSettings > 0)
    {
        isSettings--;
    }
    else
    {
        isSettings = 2;
    }
}

void clickOk()
{
    switch (isSettings)
    {
        case 0:
        {
            break;
        }
        case 1:
        {
            setCurrentDisplay(DISPLAY_SettingsMenu, 1000);
            break;
        }
        case 2:
        {
            if(getIsSongPlaying() == 0)
            {
                setCurrentDisplay(DISPLAY_Play, 1000);
                printf("to play");
            }
            else
            {
                setCurrentDisplay(DISPLAY_Song, 1000);
                printf("to song");
            }
            break;
        }
    }
}