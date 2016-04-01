//
// Created by aares on 1-4-2016.
//

#include "play.h"
#include "display.h"
#include "mp3stream.h"
#include "menuHandler.h"

int isSongPlaying = 0;

void showPlay()
{
    LcdArrayLineTwo("Play", 4);
}

void showPlayMenu()
{
    ClearLcd();
    LcdArrayLineOne("Press OK to", 11);
    LcdArrayLineTwo("start stream", 12);
}

void showSong()
{
    ClearLcd();
    LcdArrayLineOne("Playing", 7);
    LcdArrayLineTwo("Press Ok to stop", 16);
}

void clickOkPlay()
{
    if(isSongPlaying == 1)
    {
        printf("test\n");
        killPlayerThread();
        isSongPlaying = 0;
        setCurrentDisplay(DISPLAY_MainMenu, 1000);
    }
    else if(connectToStream("62.195.226.247",80,"/test5.mp3"))
    {
        if(play())
        {
            setCurrentDisplay(DISPLAY_Song, 1000);
            isSongPlaying = 1;
        }
    }
}

int getIsSongPlaying()
{
    return isSongPlaying;
}

void setIsSongPlaying(int value)
{
    isSongPlaying = value;
}
