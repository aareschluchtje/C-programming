//
// Created by aares on 1-4-2016.
//

#include "settings.h"
#include "display.h"
#include "menuHandler.h"

void showSettings()
{
    LcdArrayLineTwo("Settings", 8);
}

void showSettingsMenu()
{
    ClearLcd();
    LcdArrayLineOne("Settings", 8);
    LcdArrayLineTwo("Volume", 6);
}

void clickOkSettings()
{
    setCurrentDisplay(DISPLAY_Volume, 1000);
}
