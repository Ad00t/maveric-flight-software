// Macros.c
// A place for macro functionality


#include "macros.h"



//=====================Check Macros==============================
void checkMacros(void) {
    int n;					

//    checkSystemEvents();

    for (n = 0; n < NUM_MACROS; n++) {
        switch (Macro_state[n]) {
        case MACRO_RUN:         		// If MACRO_RUN we must reset the PowerSave count to stay awake
        case MACRO_BACKGROUND: 			// Background and run modes do not reset the PowerSave counter
  		  if (delay[n] > 0)delay[n]--; 	// Decrement delay   
          if (delay[n] == 0)Macro(Macro_state[n], n); // resume processing
          break;

        default:
            Macro_state[n] = MACRO_STOP;
            break;
        }
    }
//    globalEventRegister = 0;
}

//=================== Execute Macro commands=========================
void Macro(unsigned char state, int ID) {
    unsigned char cmd[MAX_MACRO_SIZE];
    unsigned char len;
	unsigned char ptr;
    const int cmd_start = 1; // First command of stored Macro
//    unsigned char *MacroCmds;				// MRA: saving space
//    MacroCmds = LbufPlus128;		// MRA: saving space
    unsigned char MacroCmds[MAX_MACRO_SIZE];	// MRA: using this instead of LbufPlus128 to allow compiler to reuse memory
	unsigned char c;	// Using this instead of nested array for "delay[MacroCflag[ID]]" because of compiler bug
 
    switch (state) {
      case MACRO_RUN: //== If commanded, start or resume Macro processing
      case MACRO_BACKGROUND:
       if (readMacro('M',ID, MacroCmds) > 0) {
            delay[ID] = 0;
          while (delay[ID] == 0) // Process commands until finished or a delay is encountered
            {
 				ptr= Macro_ptr[ID];
                len = MacroCmds[ptr];		// This has got to be the failure point

                Macro_ptr[ID]++; // Point to start of command
 
                if (Macro_ptr[ID] <= MacroCmds[0]) // Must be less than total length
                {
                  if (len < MAX_MACRO_SIZE-9) {
                        memcpy(cmd + 9, MacroCmds + Macro_ptr[ID], len);
                        cmd[0] = len + 8;
                        processCmd(USER_PORT, globals.RTU_ID, cmd, 1, 0);
                    } else {
                   		 //sprintf(dbgbuf, "macro %d, bad length:%d", ID, len);
                    	//sendDBGALL(USER_PORT, dbgbuf);
                    }
                }

                Macro_ptr[ID] += len; // Point to the next command length

                if (Macro_ptr[ID] >= MacroCmds[0] && delay[ID] == 0) // at the end of the macro and not processing delay
                {
                  if (Macro_state[ID] == MACRO_RUN) // Not a Background
                    {
                      Macro_state[ID] = MACRO_STOP; // Stop Macro if it is not a Background Macro
                        if (Macro_loop[ID] > 0)
                            Macro_loop[ID]--; // Decrement loop counter
                    }
                    if (Macro_loop[ID] > 0){
   					if (ID > NUM_MACROS - 1)return;
						//    if ((mode != MACRO_STOP) && sleep) __wakeUpAndResetPS(wakeMacroProcessing); // We must wake up to start a Macro
    					        delay[ID] = 0; // reset delay
        						MacroCflag[ID] = 0xff; // Disables continuation
        						Macro_loop[ID] = 0;
        					Macro_ptr[ID] = cmd_start; // reset Macro_ptr to the beginning
       						 Macro_state[ID] = MACRO_STOP; // Reset Macro_State
    					Macro_state[ID] = MACRO_RUN; // mode is either Background, Run, (Background lets unit fall asleep)
    					Macro_loop[ID] = Macro_loop[ID]; // Set the loop counter
					}
                    else // If not restarting the Macro, Bailout to process others
                    {
						c=MacroCflag[ID];
						Macro_loop[ID] = 0;
                        if (MacroCflag[ID] < NUM_MACROS)
                            delay[c] = 0; // Reset delay on waiting Macro
                        MacroCflag[ID] = 0xff; // reset Macro "continuation" flag
                        break;
                    }
                }
				
            } //while
            return;
        }

      case MACRO_STOP: //==If commanded, stop Macro Processing
        MacroStop(ID);
        break;

      default:
        break;
    }
}

unsigned char readMacro(unsigned char type, unsigned char macroId, unsigned char * macro)
{
	unsigned long addr;
	int i;
    unsigned char CRC[2];
    unsigned char macroLength = 0;

    // Try reading each macro twice
	for (i=0; i<2;i++) {

        switch (type) {
          case 'M':
            addr = (unsigned long)Macros_ADDR + ((unsigned long)macroId * (unsigned long)MAX_MACRO_SIZE);
            break;
//          case 'E':
//            addr = EVENT_TABLE_ADDR + (macroId * MAX_MACRO_SIZE);
//            break;
        }

		// Read the macro out of flash
		flashRead(addr,MAX_MACRO_SIZE,macro);

		// Set the length
        macroLength = macro[0];
			
		// CRC Check
        if (macroLength > 0 &&  macroLength < MAX_MACRO_SIZE) 	// Safety size check
		{
            getCRC(macro + 1, macroLength, CRC);				// Compute CRC
        	if (memcmp(CRC, macro+macroLength+1, 2) == 0) 		// Compare CRC
			{	 
            	return macroLength;								// If comparison clears, return a non-zero length
            }
        }
    }//for

    return 0;
}

void RunMacro(int ID, unsigned char mode, unsigned char loop) // (called by code)
{
    MacroCmdRun(ID, mode, loop, globals.RTU_ID);
}

// This function is ridiculous.  It only clears the macro if the *buf handed to it has
// a 0 in buf[0]!!!  I'm rewriting it so that it actually does what it says, and doesn't
// need a buf input.
void clearMacro(unsigned char macroId) {
	
	unsigned char dummy[4];
	dummy[0]=0;

    writeMacro('M',macroId, dummy);                       //Use write instead of load.. to avoid inserting a CRC
}

void writeMacro(unsigned char type, unsigned char macroId, unsigned char * macro)
{

	unsigned long addr;
	unsigned char macroLength;
    macroLength = macro[0];	
    if (macroLength + 3 < MAX_MACRO_SIZE) {
        getCRC(macro + 1, macroLength, macro + macroLength + 1);

        switch (type) {
          case 'M':
            addr = (unsigned long)Macros_ADDR + ((unsigned long)macroId * (unsigned long)MAX_MACRO_SIZE);
            break;
//          case 'E':
//            addr = EVENT_TABLE_ADDR + macroId * MAX_MACRO_SIZE;
//            break;
        }
		if(!CheckFlashEmpty(addr,macroLength + 3))
        	flashErasePage(FlashAddrToPage(addr));	// Pre-Erase Page if not empty.  TODO: Upgrade to flashCopy() to have sub-page resolution to handle adjacent macros on a page!
		flashWrite(addr, macroLength + 3, macro);	// Write Page
		//flash('p', addr, macroLength + 3, macro);
        //waitForFlash();
    }
}

void loadMacro(unsigned char macroId, unsigned char * buf, unsigned char * commands, unsigned char commandsLength)
{
    if (macroId < NUM_MACROS) {
        MacroStop(macroId);

        buf[0] = commandsLength;

        if (commandsLength < MAX_MACRO_SIZE - 3) {
            memcpy(buf + 1, commands, commandsLength);
            writeMacro('M',macroId, buf);
        }
    }
}


//========== Initializes and starts Macros Running ===
void MacroCmdRun(int ID, unsigned char mode, unsigned char loop, unsigned char * source) // (called by command)
{

    if (ID > NUM_MACROS - 1)return; //bail if too big
    if (ID < 0) return;				// bail if negative

    MacroStop(ID);                  // Before Running stop Macro to reset all states
    Macro_state[ID] = mode;         // mode is either Background, Run, (Background lets unit fall asleep)
    Macro_loop[ID] = loop;          // Set the loop counter
}

//========== Stops Macros and resets states of Macros that are running
void MacroStop(int ID){
   int cmd_start = 1;
   
   if (ID > NUM_MACROS - 1)return;	// bail if too big
   if (ID < 0) return;				// bail if negative

   delay[ID] = 0;                       // reset delay
   MacroCflag[ID] = 0xff;               // Disables continuation
   Macro_loop[ID] = 0;
   Macro_ptr[ID] = cmd_start;      // reset Macro_ptr to the beginning
   Macro_state[ID] = MACRO_STOP;   // Reset Macro_State
}

/*
void loadMacroEventMask(unsigned char macroId, macroEventInfo_t * eventInfo)
{
    if (macroId < NUM_MACROS) {

        if(eventInfo->len > 2)eventInfo->len = sizeof(macroEventInfo_t) - 3;  // sets length to 4
        else eventInfo->len=0;                                                // To clear..length
                                                                              // needs to be zero
        writeMacro('E',macroId, (unsigned char*)eventInfo);
    }
}
*/
