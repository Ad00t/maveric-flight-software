/***************************************************************************

									geomag.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: Geomagnetic model

Changelog:

Date 	 |  Au.  |	Notes
09-20-10	mra		Created.		

****************************************************************************
*/

// Dependency
#include "geomag.h"

#module

void geomag(float *B, float *posECI ,float GHA, float days, short ReadGeomagFromFlash)
{
// function [Br,Bt,Bp] = magnet(r,theta,phi,days)
// calculates magnetic field strength in local spherical coordinates
// Inputs
// posECI (x,y,z) in meters
// days Decimal days since January 1, 2000
//
// Outputs - magnetic field strength in local tangential coordinates
// Br B in radial direction
// Bt B in theta direction
// Bp B in phi direction
// Checks to see if located at either pole to avoid singularities
int x, m,n;
const unsigned char ORD=8;			// Order of model
const float a=6371.2;		// Reference radius used in IGRF
float r, theta, phi;
float cost, sint, sinmp,cosmp, LST;
float gvali_x, gsvi_x,hvali_x, hsvi_x;		
float g[10][10], h[10][10], F_temp1,F_temp2;		//  800 bytes
float Br,Bt,Bp, lat, mph;
float P11, P10, dP10, dP11, P20, dP2, P2, dP20,K;
unsigned char temp[20];								        //  20 bytes
unsigned char CRC[2];

float temp_f;

const float coeff_f[44][4] = {
	{-29619.4,13.3,0.0,0.0},
	{-1728.2,11.6,5186.1,-21.2},
	{-3401.55,-21.6,0.0,0.0},
	{5314.62,-6.41,-4298.26,-39.32},
	{1447.04,-3.12,-396.64,-9.61},
	{3349.0,-2.75,0.0,0.0},
	{-7005.54,-10.72,-696.88,17.15},
	{2424.68,-2.32,568.17,-8.71},
	{564.86,-6.72,-388.25,-6.32},
	{4078.81,-11.81,0.0,0.0},
	{4354.14,12.17,1508.56,9.41},
	{978.28,-31.31,-907.45,3.91},
	{-842.94,9.41,250.58,10.67},
	{82.31,-1.41,-224.66,-0.22},
	{-1723.05,-11.03,0.0,0.0},
	{3572.54,7.12,445.3,-3.05},
	{1708.42,-19.98,1321.09,11.53},
	{-613.69,-5.65,-626.4,9.41},
	{-374.04,0.0,-87.19,8.43},
	{-9.05,-0.14,74.58,-0.35},
	{1043.83,5.78,0.0,0.0},
	{1289.19,5.67,-328.91,-13.23},
	{1108.86,10.46,951.95,-26.9},
	{-1603.02,18.93,648.58,-1.99},
	{-32.2,-9.28,-333.96,-2.18},
	{39.32,-1.16,1.63,-0.47},
	{-60.72,0.47,29.42,1.01},
	{2118.19,5.36,0.0,0.0},
	{-2624.75,-3.55,-2291.34,24.83},
	{0.0,-8.69,-700.85,8.69},
	{681.93,22.53,126.97,2.05},
	{112.38,8.64,296.37,3.7},
	{42.6,3.09,91.38,-4.94},
	{17.68,-0.73,-61.51,-0.24},
	{-0.78,0.32,-3.75,0.13},
	{1226.67,5.03,0.0,0.0},
	{442.41,13.41,797.67,-13.41},
	{-515.96,-28.04,-1205.77,5.61},
	{-327.21,8.28,352.07,12.43},
	{-443.82,-10.69,-574.83,10.69},
	{134.96,2.97,229.87,1.48},
	{48.06,3.43,61.1,-2.06},
	{-19.8,-1.75,-37.35,1.0},
	{-4.39,0.25,-1.32,0.25}
};


const unsigned char coeff_n[44][2] = {
	{1,0},
	{1,1},
	{2,0},
	{2,1},
	{2,2},
	{3,0},
	{3,1},
	{3,2},
	{3,3},
	{4,0},
	{4,1},
	{4,2},
	{4,3},
	{4,4},
	{5,0},
	{5,1},
	{5,2},
	{5,3},
	{5,4},
	{5,5},
	{6,0},
	{6,1},
	{6,2},
	{6,3},
	{6,4},
	{6,5},
	{6,6},
	{7,0},
	{7,1},
	{7,2},
	{7,3},
	{7,4},
	{7,5},
	{7,6},
	{7,7},
	{8,0},
	{8,1},
	{8,2},
	{8,3},
	{8,4},
	{8,5},
	{8,6},
	{8,7},
	{8,8},
	};

Br= 0.0;
Bt= 0.0;
Bp= 0.0;

r= sqrt(posECI[0]*posECI[0]+ posECI[1]*posECI[1] + posECI[2]*posECI[2]);		// compute orbital radius
if(r<6378000.0)return;															// Gotta be on orbit
lat= asin(posECI[2]/r);											  				// latitude (degrees)
theta= PI/2.0-lat;																// Geomag works with co-latitude (radians)
F_temp1= posECI[0]*cos(GHA) + posECI[1]*sin(GHA);								// x compenent rotate ECI into ECF
F_temp2= -1.0*posECI[0]*sin(GHA) + posECI[1]*cos(GHA);							// y component rotate ECI into ECF
phi=  atan2(F_temp2, F_temp1);													// longitude (-PI -> PI)
r=r/1000.0;																		// Convert to km.


for(n=0;n<ORD+1;n++){		// Zero everything
for(m=0;m<ORD+1;m++){
	g[n][m]=0.0;
	h[n][m]=0.0;
	}
}

//memset(g,0x00,ORD*ORD*sizeof(float));	// Zero Everything (faster?)
//memset(h,0x00,ORD*ORD*sizeof(float));	// Zero Everything (faster?)


	// This section of the code reads in the g and h Schmidt
	// quasi-normalized coefficients from external flash
	for(x=0;x<44;x++)  // up to 43 for 8th order
	{	
		// Read the constants from Flash
		if(ReadGeomagFromFlash)
		{
			flashRead(MAG_COEFF_ADDR +(long)(32*x), 20, temp);
	
			//flash('r',MAG_COEFF_ADDR +(long)(32*x), 20, temp);				// add crc read and check here
	
			getCRC(temp, 18, CRC);											// append CRC to end of coeff
			 //DEBUG
			//sprintf(dbgbuf,"\r\n  %02d READ=",x);
			//sendMSGALL(USER_PORT,dbgbuf);
			//HexPrint(USER_PORT,temp,20);
			//sendMSGALL(USER_PORT,"\r\n  CRC=");
			//HexPrint(USER_PORT,CRC,2);
	
			if(memcmp(CRC, temp+18, 2)){
				//sprintf(dbgbuf,"\r\nLine %d failed CRC: ",x);
				//sendDBGALL(USER_PORT,dbgbuf);
				sendByte(USER_PORT,'x');
				globalCRCErrorCounter++;
				//HexPrint(USER_PORT,temp,20);
				//sendDBGALL(USER_PORT,"  CRC=");
				//HexPrint(USER_PORT,CRC,2);

	//			sprintf(dbgbuf,"\r\nSubstituting hard-coded defaults");
	//			sendDBGALL(USER_PORT,dbgbuf);
				
				// This section loads the hardcoded defaults in case of flash failure
				temp[0] = coeff_n[x][0];
				temp[1] = coeff_n[x][1];
				temp_f = coeff_f[x][0];
				memcpy(&temp[2],&temp_f,sizeof(float));
				temp_f = coeff_f[x][1];
				memcpy(&temp[6],&temp_f,sizeof(float));
				temp_f = coeff_f[x][2];
				memcpy(&temp[10],&temp_f,sizeof(float));
				temp_f = coeff_f[x][3];
				memcpy(&temp[14],&temp_f,sizeof(float));
				// --------------------------------------------------------------------
			}//CRC check
		}// if(ReadGeomagFromFlash)
		// Else, Read the constants from ROM
		else
		{

			temp[0] = coeff_n[x][0];
			temp[1] = coeff_n[x][1];
			temp_f = coeff_f[x][0];
			memcpy(&temp[2],&temp_f,sizeof(float));
			temp_f = coeff_f[x][1];
			memcpy(&temp[6],&temp_f,sizeof(float));
			temp_f = coeff_f[x][2];
			memcpy(&temp[10],&temp_f,sizeof(float));
			temp_f = coeff_f[x][3];
			memcpy(&temp[14],&temp_f,sizeof(float));
		}// else

		
		n=(unsigned char)temp[0];
		m=(unsigned char)temp[1]+1;
		memcpy(&gvali_x,temp+2,4);
		memcpy(&gsvi_x,temp+6,4);
		memcpy(&hvali_x,temp+10,4);
		memcpy(&hsvi_x,temp+14,4);
		if(n>9 || m>9){			    // safety
			sendDBGALL(USER_PORT,"\r\nExceeded GeoArray Size");
			return;			
		}// if
		g[n][m] = gvali_x + gsvi_x*(float)days/365.0;
		h[n][m] = hvali_x + hsvi_x*(float)days/365.0;
	}// for


P11=1.0; 
P10=P11;
dP11=0.0; 
dP10=dP11;

// Calculate Legendre polynomials and derivatives recursively
cost=cos(theta);	// Do this once to speed up the algorithm
sint=sin(theta);

for(m=0;m<ORD;m++){
  for(n=1;n<ORD;n++){		// ORD=8..n=7
  if(m<=n){
  	if(n==m){
   		P2 = sint*P11;
   		dP2 = sint*dP11 + cost*P11;
   		P11=P2; P10=P11; P20=0.0;
   		dP11=dP2; dP10=dP11; dP20=0.0;
   		}
  	else if(n==1){
		P2 = cost*P10;
		dP2 = cost*dP10 - sint*P10;
		P20=P10; P10=P2;
		dP20=dP10; dP10=dP2;
   		}
  	else{
    	K = (float)((n-1)*(n-1) - m*m)/(float)((2*n-1)*(2*n-3));
    	P2 = cost*P10 - K*P20;
   		dP2 = cost*dP10 - sint*P10 - K*dP20;
   		P20=P10; P10=P2;
   		dP20=dP10; dP10=dP2;
   	}
	mph=m*phi;
	mph=fmodfred(mph,(float)TWOPI);
	sinmp=  sin(mph);
	cosmp=	cos(mph);	

	F_temp1=    pow(a/r,(float)(n+2));					// Calculate Br, Bt, and Bp
    F_temp2=	g[n][m+1]*cosmp + h[n][m+1]*sinmp;
	Br +=  F_temp1 *F_temp2* (float)(n+1) *P2;
	Bt +=  F_temp1 *F_temp2* dP2;
	Bp +=  F_temp1 * (float)m *(h[n][m+1]*cosmp - g[n][m+1]*sinmp)* P2;   
	}	// if m<=n
}	// for(n=1
}	// for(m

    Bt= -1.0*Bt;
	Bp= -1.0*Bp/sint;
// Coordinate transformation for test purposes compare to 
// (http://www.ngdc.noaa.gov/geomagmodels/IGRFWMM.jsp)
//   B[0]= -1.0*Bt;			// North
//   B[1]= Bp;				    // East
//   B[2]= -1.0*Br;			// Down
						// GHA and lat are in radians from orbit model
						// This transformation converts the B field to ECI (page 782 Wertz)
	LST=GHA+phi;  		// This is the Local Sidereal Time (GHA + longitude)
	B[0] = (Br*cos(lat)+Bt*sin(lat))*cos(LST) - Bp*sin(LST);   // BX
	B[1] = (Br*cos(lat)+Bt*sin(lat))*sin(LST) + Bp*cos(LST);   // BY
	B[2] = (Br*sin(lat)-Bt*cos(lat));						   // BZ
}
