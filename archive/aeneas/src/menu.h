/***************************************************************************

									menu.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide generic menu-making functions for 1-unsigned char
  		 menus

To use: 
		UNDER DEVELOPMENT.  DO NOT USE YET.

Changelog:

Date 	 |  Au.  |	Notes
03-08-10	mra		Created.

****************************************************************************
*/

#define DISP_MENU_ITEM(itemnum,itemdesc) sendMSGALL(port,"\r\n##itemnum##. itemdesc##");

void MENU_DISPLAY(int port)
{
	DISP_MENU_ITEM(1,Test 1);
	DISP_MENU_ITEM(2,Test 2);
}


#define MENU_ITEM(casenum,function) case casenum: function##(); break;

void MENU_PROCESS(int port,struct menu_s * m)
{
	unsigned char c;
	if(getByte(port,&c))
	{
		switch(c)
		{
		MENU_ITEM(1,test1);
		MENU_ITEM(2,test2);
		MENU_ITEM(3,test3);
		MENU_ITEM(4,test3);	
		}	

	}

}
