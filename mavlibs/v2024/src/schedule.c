/***************************************************************************

									schedule.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne

Purpose: Provide hardware interface to ADIS16260 gyroscope.
Changelog:

Date 	 |  Au.  |	Notes
07-19-11	mra		Initial revision.		
10-10-11	mra		Major update to be more OOP.

****************************************************************************


Let's talk about how Fred's schedules works.

The schedule provides access to the ProcessCmd() function based off the time
stored in the global time[] array.  Under normal use, the checkSchedule()
function is called once every second, and it compares the stored schedules
to the current time.  If they match (including some optional wildcards), 
the command stored in the schedule will run, essentially by calling ProcessCmd()
and passing it the rest of the bytes in the stored schedule.

The schedule itself is a two-dimensional array, where each row is a seperate Time-and-Command
combination.  The array is stored in a variable called globals, whose typedef is a 
custom-defined struct called TFlashProtected.  As the name suggested, this struct
can be backed up to flash, and by doing so the schedules are saved through reboot.

We are going to refactor this setup to add functionality and object-orientedness.
Eventually, we hope that the scheduling functions will work on ANY two-dimensional array of
a big enough size, and that these functions check array sizes and boundary conditions
so there's no overflows.

Each field in a scheduled row has the following bytes:
Length - Run Flag - Time (7 bytes) - Command (varies) - CRC (2 bytes)

Length -- The length of everything but the CRC, including the length byte. (Maybe?)
Run Flag -- Whether or not the command has been run (for non-wildcard commands.  Wildcard commands are automatically considered as "Ran").
Time -- See fredtime.h for a description.  7 bytes, MM DD YY WKday HH MM SS.  (0xff is considered a wildcard here)
Command -- A valid command from ProcessCmd()
CRC -- the checksunm

Example:
Say we want to schedule a display of the Time MISC packet.  That is Command 165 2.
Currently we send:
r 253 1 10 11 12 1 2 3 4 165 2
Format: r [schedule cmd] [which row in the schedule] [time tag 7 bytes] [cmd and parameters] 

The bytes stored from this command are:
[len] [run] [time tag----------] [cmd-] [crc-]
09    00    0a 0b 0c 01 02 03 04  a5 02  7b e5

Notice the Length is 7 bytes more than the length of the command (which is only 2 bytes).


*/


#include <schedule.h>		// Public header

#module					// !!! Important: This command makes everything below scoped only to this file.

//================================
//  	Private Functions
//================================
// TODO: Rewrite the checkschedule function to be clearer.

//================================
//  	Private Defines
//================================
#define HAS_NOT_RAN			0
#define HAS_RAN				1
#define HAS_WILDCARDS		2

#define LENGTHBYTE			0
#define RANBYTE				1
#define TIMEBYTE			2
#define COMMANDBYTE			9

#define HEADER_SIZE			2			// 1 for length, 1 for "ran" byte, 
#define TIME_SIZE			7			// Size of time vector		
#define CRC_SIZE			2			// 2 bytes for crc


//========================================
// 			Driver Functions
//========================================



/*=================== setSchedule ===================
  PURPOSE:  Given a schedule row, a command and a time, will
  			correctly set the schedule, including CRC bytes.
  UNMODIFIED VARIABLES: none
  MODIFIED VARIABLES: schedule_row
  GLOBALS: none
  RETURN VALUE: SUCCESS if schedule is set, ERROR VALUE
				if an error is encountered.
  NOTES:	
	Time is 7 bytes of the format: MM DD YY WKday HH MM SS
	The Weekday is often ignored in matching schedules, but you should check that before assuming it.
	A wildcard in the time array is 0xff.

Example input:
	command = 0xA502;
	CmdSize = 2;
	time = 10 11 12 1 2 3 4  (In hex: 0a 0b 0c 01 02 03 04)
Expected output:
	Srow = 09 00 0a 0b 0c 01 02 03 04 a5 02 7b e5

Example usage: 
	setSchedule(&globals.StoredCmds[4][0],MAX_STORED_CMD_SIZE, &eps_cmd[0][0], 2, epst);  // Set the Fourth Domino Schedule (must use DeltaTime)
				 -----Schedule Row-----   ---- Maximum Size-- --Cmd Pointer-- Sz  TimeVector
*/
ReturnErr_t setSchedule(unsigned char * Srow, 
						int MaxSize,				// Maximum size of the Srow array, so we don't overflow
						unsigned char * command,
						unsigned char CmdSize,		// Size of the Command and parameters (does not include the length byte or CRC)
						unsigned char * time)		
{
	// Declarations
	ReturnErr_t retval;
	int i;

//	Check inputs
	if (MaxSize <0) return FAILURE;					// Too small?  Bail.
	if (CmdSize+TIME_SIZE+CRC_SIZE+HEADER_SIZE>MaxSize) return OUT_OF_BOUNDS;	// Too big?  Bail.	

	// Set the Length Byte
	Srow[0] = CmdSize+TIME_SIZE;	// Command Size + Time vector
	
	// Check for wildcards in the time (except wkday--time[3]) and set the Run byte
	Srow[1]=HAS_NOT_RAN;	// Initial setting
	for (i=0;i<TIME_SIZE;i++)
	{
		if(time[i]==0xff && i!=FRED_WKDAY)
		{
			Srow[1]=HAS_WILDCARDS;	// Set the Run Byte
			break;
		}
	}

	// Set the time bytes
	memcpy(&Srow[2],time,TIME_SIZE);	

	// Set the command bytes
	memcpy(&Srow[9],command,CmdSize);

	// Set the CRC of the command and time
	getCRC(&Srow[2], CmdSize+TIME_SIZE, &Srow[TIME_SIZE+HEADER_SIZE+CmdSize]); 

	//Initializations
	retval = SUCCESS;

}


/*=================== timeMatches ===================
  PURPOSE:  compares two given times too see if they match, including wildcards
  UNMODIFIED VARIABLES: none
  MODIFIED VARIABLES: none
  GLOBALS: none
  RETURN VALUE: 0 for no match, 1 for match
  NOTES:	A specific byte in the timetag will match if either set to 0xff.
-----------------------------------------------------------
*/
unsigned char timeMatches(unsigned char * t1, unsigned char * t2)
{
	unsigned char matches;
	signed int j;

	matches=0;		
	for(j=6;j>=0;j--)		// Check seconds field first..least likely to match
		{
		if(t1[j]==0xff || t1[j]==t2[j] || j==FRED_WKDAY || t2[j]==0xff)matches++; // count time tag matches, ignoring the weekday (j=3)
		else break;		// Neither a blank or match, don't continue to check
		}

//	sprintf(dbgbuf,"\r\nMatches=%i",matches); sendDBGALL(USER_PORT,dbgbuf);

	if (matches<7) return 0;
	else return 1;
}



/*=================== checkSchedule ===================
  PURPOSE:  Compares the current value of time to the values stored in globals.StoredCmds[][]
  UNMODIFIED VARIABLES: none
  MODIFIED VARIABLES: globals
  GLOBALS: globals structure
  RETURN VALUE: none
  NOTES:	The timetag will match if set to 0xff.
-----------------------------------------------------------
*/
void checkSchedule() {
	unsigned char i, matches,m,ran;
	unsigned char  CRC[2];
	unsigned char rbcmd[MAX_STORED_CMD_SIZE];
	unsigned char * p_gtime;		// Pointer to global time vector
	//if((time[6] % 10) != 0) return; /// only run once every 10 seconds. For testing running commands we missed.
	
	for(i=0;i<NUM_STORED_CMDS;i++)			//== For each of the stored commands 
		{
		m= globals.StoredCmds[i][0];		// Temporary variable representing command length
		ran=globals.StoredCmds[i][1];		// Temporary variable representing command RunFlag
		p_gtime=&globals.StoredCmds[i][2];	// Pointer to time vector
		matches = FALSE;						// Assume no match to start
	
		// Check if this is a valid stored command, meaning it meets the following conditions:
		// - Command Length is greater than 0
		// - Command length is less than the size of the array
		// - Command has not yet ran (so it's either HAS_NOT_RAN or HAS_WILDCARDS
		if(m>0 && m<MAX_STORED_CMD_SIZE-1 && ran!=HAS_RAN)	// First record is normally length or "null" if no command.  Also check for/avoid trap conflict.
		{
			// Ok. Now we know we have a valid command.  It's either got wildcards or not, we
			// will fork based on that.
//			sendDBGALL(USER_PORT,"A"); delay_ms(100);
	
			if(ran==HAS_WILDCARDS)
			{
				// If it has wildcards, we need to check for an exact match of the time.  Use timeMatches()
				if(timeMatches(time,p_gtime)) matches = TRUE;
			}
			else
			{
//				sendDBGALL(USER_PORT,"N"); delay_ms(100);
				// If it has no wildcards, we need to check if it's in the past or an exact match.  Use diffTime().
				if(diffTime(time,p_gtime)>=0) matches = TRUE;
			}
//			sendDBGALL(USER_PORT,"D"); delay_ms(100);

			if(matches==TRUE)		// Time tag passes checks, check CRC
				{
				//sendDBGALL(USER_PORT,"\r\nMatches=7\r\n");
				getCRC(&globals.StoredCmds[i][2],m,CRC);     //
				if(globals.StoredCmds[i][m+2]==CRC[0]) // Run only if a valid command (for speed check only LSB of CRC)
					{

					// Build and run the command -------------------
					rbcmd[0]= m+1;	// Set Lengt
					memcpy(&rbcmd[1],globals.Dest_ID,4);
					memcpy(&rbcmd[5],globals.RTU_ID,4);
					memcpy(&rbcmd[9],&globals.StoredCmds[i][9],rbcmd[0]-7);  // Moves the pointer along j by 8 units.  Note the length is equal to m-6
						// Notes: A command looks like:
						// (length) (4 byte source) (4 byte dest) (cmd ID) (command parameters)
					//sendDBGALL(USER_PORT,"\r\nProcessing Scheduled Command...");
					//HexPrint(USER_PORT,rbcmd,rbcmd[0]);
					// mark that we have in fact run the cmd (if no wildcards)
					if(ran==HAS_NOT_RAN) globals.StoredCmds[i][1] = HAS_RAN;
					processCmd(SCHEDULED_PORT,globals.RTU_ID,rbcmd,1,0);		 // Process command for this RTU, don't ACK
					//if (PC[USER_PORT]=='F' && !sleep) placeString(USER_PORT, 24, 2, "iDAC command processed...              ");
	       			}
	    		else {
					sendDBGALL(USER_PORT,"\r\nBAD CRC!");	
					//placeString(USER_PORT, 20, 2, &"Bad CRC in stored command              ");
					}
				}		// timetag matches	
			} 		//   if a command is detected
		}	// for i...
} // checkSchedule


/*=================== clearSchedule ===================
  PURPOSE:  Zero
  UNMODIFIED VARIABLES: none
  MODIFIED VARIABLES: Srow
  GLOBALS: none
  RETURN VALUE: none
  NOTES:	The timetag will match if set to 0xff.
-----------------------------------------------------------
*/
ReturnErr_t clearSchedule(unsigned char * Srow)
{
	Srow[0]=0;	// Clear out the length byte
	return SUCCESS;
}

//========================================
// 		Development Functions
//========================================

// Note: The below functions are only necessary for testing and development.  They
// can be removed during actual production

#ifdef DEBUG





#endif // DEBUG
