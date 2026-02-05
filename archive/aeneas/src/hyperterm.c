
/* Hyperterm.c

Contains functions that apply to a PC Hyperterminal.

*/

#include "hyperterm.h"
#module



//========== Clear Characters =============================
void clearChars(int port, int x, int y, int blanks) {
	int i;

    locxy(port, x, y);
    for (i=0; i < blanks; i++) sendByte(port, ' ');
}

//========== Clear a Line =============================
void clearLine(int port, int x, int y)
{
	clearChars(port,x,y,80);
}

//========== Locate Cursor =============================
void locxy(int port, unsigned char row, unsigned char col) {
    unsigned char xx[8];
    unsigned char temp;

    xx[0] = '\x1b';
    xx[1] = '[';
    temp = row / 10;
    xx[2] = temp + 0x30;
    xx[3] = (row - 10 * temp) + 0x30;
    xx[4] = ';';
    temp = col / 10;
    xx[5] = temp + 0x30;
    xx[6] = (col - 10 * temp) + 0x30;
    xx[7] = 'f';
    sendMSG(port, xx, 8);
}


//=========Clear the screen on the terminal===========
void clrscrn(int port) {
	unsigned char str[7] = "\x1b[2J";
    sendMSG(port, str, 5); //?? don't know why we should only send 5 characters?
}


//============Locate String==============================
void locateString(int port, unsigned char row, unsigned char col, unsigned char *buf) {
    locxy(port, row, col);
    sendMSG(port, buf, strlen(buf));
}


//============PlaceString==============================
// Effectively the same as locateString now.  Originally would only do so if certain
// parameters were set correctly on the port.
void placeString(int port, unsigned char row, unsigned char col, unsigned char *buf) {

if(col==CENTERED)
	col = (SCREEN_WIDTH/2)-(strlen(buf)/2);

if(row==CENTERED)
	row = (SCREEN_HEIGHT/2);

#ifdef DEBUG
locateString(port, row, col, buf);

// Not sure what all the different port settings can be, so until they 
// are formalized in comments or a struct I am removing the if clause
//
//   if (send_Data[port] && format[port] != 'B' && port != globals.LANPort[0]) {
//        locateString(port, row, col, buf);
//    }
#endif
}


//============ HexPrint ==============================
void HexPrint(int port, unsigned char * msg, int len)
{
	int i;
	unsigned char tmp[5];	

	for(i=0;i<len;i++)
	{
		sprintf(tmp,"%2x ",*(msg+i));
		sendDBGALL(port,tmp);
	}


	return;
}
