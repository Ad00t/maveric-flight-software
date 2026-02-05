// labview.h
// Contains declarations for HITL testing with the LabView Simulation

#ifndef __LABVIEW__
#define __LABVIEW__



// the uart port for testing
#define HITLPORT 1



// Globals
unsigned char NewLabData=0;
float LabViewData[NUMIN];

// do the test. call from test menu
void dotest();

// second version of test.  Just to show comms
void dotest2();

// get 4 bytes off the uart, in correct endian
void get4bytes(int port, unsigned char *dest);

// send 4 bytes off the uart, in correct endian
void send4bytes(int port, unsigned char *src); 

// Loads a specified number of single-precision floats from labview into data[0] through data[num-1]
void loadLabViewFloats(int32 num, float * data);

// Sends a specified number of single-precision floats back to Labview
// The floats sent are data[0] through data[num-1]
void sendLabViewFloats(int32 num,float * data);
#endif // __LABVIEW__
