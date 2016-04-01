#include <stdio.h>
#include "menu.h"
#include "keyboard.h"
#include "display.h"

#define MAX_MENU_KEY 	4
#define LCD_MAX_LINES	2

// Let op: confict met display.h ???
typedef enum { LINE1, LINE2, LINE3, LINE4 } LCD_LINE_ENUM; 


void lcdPrintfPos( LCD_LINE_ENUM line, unsigned char col, char *s );
void lcdSetCursorPos( LCD_LINE_ENUM line, unsigned char col );


/* text bottom line */
			    // 01234567890123456789 
char tbl_eudo[]	= "Esc  Up  Down     OK";
char tbl_e[]	= "Esc                 ";
char tbl_eud[]	= "Esc  Up  Down       ";
char tbl_ef[]	= "Esc           FooBar";


/* Menu item 5 */
void menuID5OnKeyOK(void)
{ 
	printf("Volume UP");
}

/* Menu item 6 */
void menuID6OnKeyOK(void) 
{ 
	printf("Volume DOWN");
}


typedef struct
{
	unsigned int menuID;						/* (Uniek) MenuID			*/
	void (*fp_onEntry)(void);					/* On entry function call	*/
	void (*fp_onExit)(void);					/* On exit function call	*/

	void (*fp_onKeyESC)(void);					/* function call for key's  */
	void (*fp_onKeyUP)(void);	
	void (*fp_onKeyDOWN)(void);	
	void (*fp_onKeyOK)(void);

	unsigned int newMenuIdKey[MAX_MENU_KEY];	/* New menuId when key pressed */ 
	char *txt[LCD_MAX_LINES];						/* LCD_MAX_LINES lines of LCD text (or NULL) */

} MENU_ITEM_STRUCT;


MENU_ITEM_STRUCT menu[] = {

	// menuId #1
	{
		1,
		0L,
		0L,
		0L,
		0L,
		0L,
		0L,
		{
			0xFFFF,0,1,0
		},
		{
			/* 
			 01234567890123456789 
			*/
			"Welkom Simple Menu ",
			tbl_eud,
		}
	},

	// menuId #2
	{
		2,
		0L,
		0L,
		0L,
		0L,
		0L,
		0L,
		{
			0xFFFF,1,3,5
		},
		{
			/* 
			 01234567890123456789 
			*/
			"Set VOLUME",
			tbl_eudo,
		}
	},

	// menuId #3
	{
		3,
		0L,
		0L,
		0L,
		0L,
		0L,
		0L,
		{
			0xFFFF,2,4,3
		},
		{
			/* 
			 01234567890123456789 
			*/
			"Set BASS",
			tbl_eud,
		}
	},

	// menuId #4
	{
		4,
		0L,
		0L,
		0L,
		0L,
		0L,
		0L,
		{
			0xFFFF,3,2,0
		},
		{
			/* 
			 01234567890123456789 
			*/
			"Set TREBBLE",
			tbl_eud,
		}
	},

	// menuId #5
	{
		5,
		0L,
		0L,
		0L,
		0L,
		0L,
		menuID5OnKeyOK,
		{
			2,6,6,5
		},
		{
			/* 
			 01234567890123456789 
			*/
			"Volume UP",
			tbl_eudo,
		}
	},

	// menuId #6
	{
		6,
		0L,
		0L,
		0L,
		0L,
		0L,
		menuID6OnKeyOK,
		{
			2,5,5,6
		},
		{
			/* 
			 01234567890123456789 
			*/
			"Volume DOWN",
			tbl_eudo,
		}
	},
};


void handleMenu(void)
{
	unsigned int idx = 0;	
	unsigned int idy = 0;
	unsigned int menuloop_done = 0;
	unsigned int keyloop_done = 0;
	unsigned int bl_delay = 0;
	unsigned int curMenuId = 1;
	
	while( !menuloop_done )
	{
		/* Clear display */

		
		idx = 0;
		/* Lookup current menuID in the static array of menu items */
		while( menu[idx].menuID != curMenuId ) idx++;
		
		/* Call the onEntry function */
		if( *menu[idx].fp_onEntry )
			(*menu[idx].fp_onEntry)();
		
		/* Display the x lines of static text */
		for( idy = 0; idy < 4; idy++ )
		{
			if( menu[idx].txt[idy] != NULL )
			{
				lcdPrintfPos(LINE_1+idy,0, menu[idx].txt[idy]);
			}
		}

		/* Loop for key events and act according */
		keyloop_done = 0;
		while( !keyloop_done )
		{
			switch( KbGetKey() )
			{
				case KEY_ESC:
					if( *menu[idx].fp_onKeyESC )
						(*menu[idx].fp_onKeyESC)();

					
					curMenuId = menu[idx].newMenuIdKey[0];
					bl_delay = 0;
					keyloop_done = 1;
					break;

				case KEY_UP:
					if( *menu[idx].fp_onKeyUP )
						(*menu[idx].fp_onKeyUP)();

					curMenuId = menu[idx].newMenuIdKey[1];
					bl_delay = 0;
					keyloop_done = 1;
					break;

				case KEY_DOWN:
					if( *menu[idx].fp_onKeyDOWN )
						(*menu[idx].fp_onKeyDOWN)();
					
					curMenuId = menu[idx].newMenuIdKey[2];
					bl_delay = 0;
					keyloop_done = 1;
					break;

				case KEY_OK:
					if( *menu[idx].fp_onKeyOK )
						(*menu[idx].fp_onKeyOK)();
					
					curMenuId = menu[idx].newMenuIdKey[3];
					bl_delay = 0;
					keyloop_done = 1;
					break;
				
				default:
					break;
			}
		
			/* Idle loop function */

			/* Backlight switch off */
			if( bl_delay >= BL_SWITCH_OFF_DELAY )
			{
				LcdBackLight(LCD_BACKLIGHT_OFF);
			}
			else
			{
				LcdBackLight(LCD_BACKLIGHT_ON);
				bl_delay++;
			}
			
			/* Loop delay */
			NutSleep(100);
		}

		/* Keypress is handled, call the menu's exit function. */
		if( *menu[idx].fp_onExit )
			(*menu[idx].fp_onExit)();

		/* Return is curMenuId = 0xFFFF */
		if( curMenuId == 0xFFFF )
		{
			menuloop_done = 1;
		}
	}
}


/*******************************************************************/
void lcdPrintfPos( LCD_LINE_ENUM line, unsigned char col, char *s )
/*
 * Input:	
 * Output:	
 * Description:
 * 		
 *******************************************************************/
{
	lcdSetCursorPos( line, col );

	while( *s != '\0' )
	{
		LcdChar( *s );
		s++;
	}
}

/*******************************************************************/
void lcdSetCursorPos( LCD_LINE_ENUM line, unsigned char col )
{
	unsigned char ddram_addr = 0;

	switch(line)
	{
		case LINE1:
			ddram_addr = 0x00 + col;
			break;
		case LINE2:
			ddram_addr = 0x40 + col;
			break;
		case LINE3:
			ddram_addr = 0x14 + col;
			break;
		case LINE4:
			ddram_addr = 0x54 + col;
			break;
	}
	LcdWriteByte(WRITE_COMMAND,ddram_addr );
}
