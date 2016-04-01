/*! \mainpage SIR firmware documentation
 *
 *  \section intro Introduction
 *  A collection of HTML-files has been generated using the documentation in the sourcefiles to
 *  allow the developer to browse through the technical documentation of this project.
 *  \par
 *  \note these HTML files are automatically generated (using DoxyGen) and all modifications in the
 *  documentation should be done via the sourcefiles.
 */

/*! \file
 *  COPYRIGHT (C) SaltyRadio 2016
 *  \date 20-02-2016
 */

#define LOG_MODULE  LOG_MAIN_MODULE

/*--------------------------------------------------------------------------*/
/*  Include files                                                           */
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <io.h>

#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/version.h>
#include <dev/irqreg.h>
#include <dev/nvmem.h>
#include <dev/board.h>

// Note: Please keep the includes in alphabetical order!    - Jordy
#include "alarm.h"
#include "contentparser.h"
#include "display.h"
#include "displayHandler.h"
#include "gotosleep.h"
#include "keyboard.h"
#include "led.h"
#include "log.h"
#include "mp3stream.h"
#include "network.h"
#include "ntp.h"
#include "portio.h"
#include "rtc.h"
#include "spidrv.h"
#include "system.h"
#include "typedefs.h"
#include "uart0driver.h"
#include "vs10xx.h"
#include "watchdog.h"



/*-------------------------------------------------------------------------*/
/* local routines (prototyping)                                            */
/*-------------------------------------------------------------------------*/
static void SysMainBeatInterrupt(void*);
static void SysControlMainBeat(u_char);

/*-------------------------------------------------------------------------*/
/* Stack check variables placed in .noinit section                         */
/*-------------------------------------------------------------------------*/

/*!
 * \addtogroup System
 */

/*@{*/


/*-------------------------------------------------------------------------*/
/*                         start of code                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* global variable definitions                                             */
/*-------------------------------------------------------------------------*/
bool isAlarmSyncing = false;
bool initialized = false;
bool running = false;

/*!
 * \brief ISR MainBeat Timer Interrupt (Timer 2 for Mega128, Timer 0 for Mega256).
 *
 * This routine is automatically called during system
 * initialization.
 *
 * resolution of this Timer ISR is 4,448 msecs
 *
 * \param *p not used (might be used to pass parms from the ISR)
 */
static void SysMainBeatInterrupt(void *p)
{
    KbScan();
}

/*!
 * \brief Initialise Digital IO
 *  init inputs to '0', outputs to '1' (DDRxn='0' or '1')
 *
 *  Pull-ups are enabled when the pin is set to input (DDRxn='0') and then a '1'
 *  is written to the pin (PORTxn='1')
 */
void SysInitIO(void)
{
    /*
     *  Port B:     VS1011, MMC CS/WP, SPI
     *  output:     all, except b3 (SPI Master In)
     *  input:      SPI Master In
     *  pull-up:    none
     */
    outp(0xF7, DDRB);

    /*
     *  Port C:     Address bus
     */

    /*
     *  Port D:     LCD_data, Keypad Col 2 & Col 3, SDA & SCL (TWI)
     *  output:     Keyboard colums 2 & 3
     *  input:      LCD_data, SDA, SCL (TWI)
     *  pull-up:    LCD_data, SDA & SCL
     */
    outp(0x0C, DDRD);
    outp((inp(PORTD) & 0x0C) | 0xF3, PORTD);

    /*
     *  Port E:     CS Flash, VS1011 (DREQ), RTL8019, LCD BL/Enable, IR, USB Rx/Tx
     *  output:     CS Flash, LCD BL/Enable, USB Tx
     *  input:      VS1011 (DREQ), RTL8019, IR
     *  pull-up:    USB Rx
     */
    outp(0x8E, DDRE);
    outp((inp(PORTE) & 0x8E) | 0x01, PORTE);

    /*
     *  Port F:     Keyboard_Rows, JTAG-connector, LED, LCD RS/RW, MCC-detect
     *  output:     LCD RS/RW, LED
     *  input:      Keyboard_Rows, MCC-detect
     *  pull-up:    Keyboard_Rows, MCC-detect
     *  note:       Key row 0 & 1 are shared with JTAG TCK/TMS. Cannot be used concurrent
     */
#ifndef USE_JTAG
    sbi(JTAG_REG, JTD); // disable JTAG interface to be able to use all key-rows
    sbi(JTAG_REG, JTD); // do it 2 times - according to requirements ATMEGA128 datasheet: see page 256
#endif //USE_JTAG

    outp(0x0E, DDRF);
    outp((inp(PORTF) & 0x0E) | 0xF1, PORTF);

    /*
     *  Port G:     Keyboard_cols, Bus_control
     *  output:     Keyboard_cols
     *  input:      Bus Control (internal control)
     *  pull-up:    none
     */
    outp(0x18, DDRG);
}

/*!
 * \brief Starts or stops the 4.44 msec mainbeat of the system
 * \param OnOff indicates if the mainbeat needs to start or to stop
 */
static void SysControlMainBeat(u_char OnOff)
{
    int nError = 0;

    if (OnOff==ON)
    {
        nError = NutRegisterIrqHandler(&OVERFLOW_SIGNAL, SysMainBeatInterrupt, NULL);
        if (nError == 0)
        {
            init_8_bit_timer();
        }
    }
    else
    {
        // disable overflow interrupt
        disable_8_bit_timer_ovfl_int();
    }
}

/*-------------------------------------------------------------------------*/
/* local variable definitions                                              */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* Thread init                                                             */
/*-------------------------------------------------------------------------*/
THREAD(StartupInit, arg)
{
    NutThreadSetPriority(5);

    NetworkInit();

    initialized = true;

    NutThreadExit();
}

THREAD(AlarmCheck, arg)
{
    NutThreadSetPriority(100);
    for(;;){
        if(checkAlarms() == 1){
          setCurrentDisplay(DISPLAY_Alarm, 1000);
        }


        NutSleep(1000);
    }

}

THREAD(AlarmSync, arg)
{
    NutThreadSetPriority(50);

    while(initialized == false){
        NutSleep(1000);
    }
    int dayCounter = 0;
    int counter = 0;
    while(!NtpSync() && counter < 10)
    {
        NutSleep(1000);
        counter++;
    }
    counter = 0;
    for(;;)
    {

        if((initialized == true) && (hasNetworkConnection() == true))
        {
            checkSleep();
            isAlarmSyncing = true;
            char url[49];
            sprintf(url, "/getAlarmen.php?radiomac=%s&tz=%d", getMacAdress(), getTimeZone());
            httpGet(url, parseAlarmJson);
            isAlarmSyncing = false;

            char url2[43];
            sprintf(url2, "/getTwitch.php?radiomac=%s", getMacAdress());
            httpGet(url2, parseTwitch);
            char url3[43];
            sprintf(url3,"/getTwitter.php?radiomac=%s", getMacAdress());
            httpGet(url3,TwitterParser);

            //Command que (Telegram) sync
            sprintf(url, "%s%s", "/getCommands.php?radiomac=", getMacAdress());
            httpGet(url, parseCommandQue);
        }
        while(dayCounter > 28800 && (hasNetworkConnection() == true))
        {
            while(!NtpSync() && counter < 10)
            {
                NutSleep(1000);
                counter++;
            }
            dayCounter = 28800;
            counter = 0;
        }
        dayCounter++;
        NutSleep(3000);
    }
    NutThreadExit();
}

/*-------------------------------------------------------------------------*/
/* Global functions                                                        */
/*-------------------------------------------------------------------------*/

int timer(time_t start){
    time_t diff = time(0) - start;
    return diff;
}

int main(void)
{
    struct _tm timeCheck;

    WatchDogDisable();

    NutDelay(100);

    SysInitIO();

	SPIinit();

	LedInit();

	LcdLowLevelInit();

    Uart0DriverInit();
    Uart0DriverStart();
	LogInit();

    X12Init();

    VsPlayerInit();

    NtpInit();

    NutThreadCreate("BackgroundThread", StartupInit, NULL, 1024);
    NutThreadCreate("BackgroundThread", AlarmSync, NULL, 2500);
    NutThreadCreate("BackgroundThread", AlarmCheck, NULL, 256);

	KbInit();

    SysControlMainBeat(ON);             // enable 4.4 msecs heartbeat interrupt

    /*
     * Increase our priority so we can feed the watchdog.
     */
    NutThreadSetPriority(1);

	/* Enable global interrupts */
	sei();
	
	LcdBackLight(LCD_BACKLIGHT_ON);
    setCurrentDisplay(DISPLAY_DateTime, 5);

    X12RtcGetClock(&timeCheck);
    int hours;
    int mins;
    int secs;
    if(!NutNvMemLoad(100, &hours, sizeof(hours)))
    {
        printf("uren: %d", hours);
    }
    if(!NutNvMemLoad(105, &mins, sizeof(mins)))
    {
        printf(" minuten: %d", mins);
    }
    if(!NutNvMemLoad(110, &secs, sizeof(secs)))
    {
        printf(" seconden %d", secs);
    }
    printf("Welcome to Saltyradio.\nI'm using mac address:  %s\n\n\n", getMacAdress());

    for (;;)
    {
        //Key detecten
        if(KbGetKey() != KEY_UNDEFINED){
            //Backlight aanzetten.
            LcdBackLight(LCD_BACKLIGHT_ON);
            if(getCurrentDisplay() == DISPLAY_Alarm){
                if(KbGetKey() == KEY_01 || KbGetKey() == KEY_02 || KbGetKey() == KEY_03 || KbGetKey() == KEY_04 || KbGetKey() == KEY_05 || KbGetKey() == KEY_ALT){
                    setSnooze(getRunningAlarmID());
                    killPlayerThread();
                    setCurrentDisplay(DISPLAY_DateTime, 2);
                }else if(KbGetKey() == KEY_ESC){
                    handleAlarm(getRunningAlarmID());
                    killPlayerThread();
                    setCurrentDisplay(DISPLAY_DateTime, 5);
                }
            }else{

                if(KbGetKey() == KEY_DOWN){
                    setCurrentDisplay(DISPLAY_Volume, 5);
                    volumeDown();
                }else if(KbGetKey() == KEY_UP){
                    setCurrentDisplay(DISPLAY_Volume, 5);
                    volumeUp();
                }else if(KbGetKey() == KEY_LEFT){
                    setCurrentDisplay(DISPLAY_Twitter,20);
                }else{
                    setCurrentDisplay(DISPLAY_DateTime, 5);
                }
                if(KbGetKey() == KEY_01){
                    setSleep();
                }
                if(KbGetKey() == KEY_02){
                    changeChanel();
                }
            }
        }
        refreshScreen();
        WatchDogRestart();
        NutSleep(100);
    }
    return(0);
}
