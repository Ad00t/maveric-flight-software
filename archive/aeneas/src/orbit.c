/***************************************************************************

									orbit.c

Company: USC/ISI/iControl
License: Proprietary
Author(s): Michael Aherne (mra), Fred Tubb, Tatiana Kichkaylo (tk)

****************************************************************************
*/
// GHA = Greenwich Hour Angle!!! (Thanks Lucy!)

#include "orbit.h"

//short orbitInitFlag = 0;

#module

//================================
//  	Private Globals
//================================






//================================
//  	Private Functions
//================================
int Rint(float num);
void m2feet(float *a);
void feet2m(float *a);
float fmodfred(float a,float b);
//void tsec(int *epoch,float *time);
float degcos(float angle);
float degsin(float angle);
void eclip2eci(float latin,float longin,float *val);
void cartesian(float theta,float delta,float *outvec);
//void utc2gmt(float utctime,int *gmt);
//void dcm2q(float a[3][3],float *qout);

// New Prototypes 9/19
float inrange(float angle);
long int GMT2SEC(unsigned char *gmt);
void rk4(float *x,float dt,float *tom,float *dom);


void initOrbit(unsigned char * OrbitItime, float * pos_eci, float * vel_eci, unsigned char * currentTime)
{
	long i;
	long sec;
//	long period;
//	long prop_sec;
	unsigned char local_copy_init_time[7];
//	float dt;

	// Store position and velocity in global
	for(i=0;i<3;i++){
		orbitData[i+6]=pos_eci[i];
		orbitData[9+i]=vel_eci[i];
	}

	// Copy the Orbit Init Time to the local vector (so as to preserve original)
	local_copy_init_time[0]=OrbitItime[0];
	local_copy_init_time[1]=OrbitItime[1];
	local_copy_init_time[2]=OrbitItime[2];
	local_copy_init_time[3]=OrbitItime[3];
	local_copy_init_time[4]=OrbitItime[4];
	local_copy_init_time[5]=OrbitItime[5];
	local_copy_init_time[6]=OrbitItime[6];

	// Check the number of seconds between Orbit Initial time and now
	sec = diffTime(local_copy_init_time,currentTime);
	
	// ---------------- Debug
//	sprintf(dbgbuf,"\r\nSeconds difference = %li",sec); 
//	sendDBGALL(USER_PORT,dbgbuf);
	// ----------------

	// If the number of seconds is negative, the orbit initialization is from the past.
	// We want to initialize the orbit at that time, then propogate till now, but only if
	// it's not going to take forever!!!

	// To save time, we compute the orbital period and take the modulus of the seconds
	// with it.  For example, if the orbital period was exactly 90 minutes, and the
	// time difference between then and now is 6755 minutes, we do not propogate
	// the entire 6755 minutes, but rather assume the orbit is constant over this
	// entire time. (6755 mod 90 = 75 orbits, 5 minutes remainder).  We just propogate
	// the last 5 minutes.
	// Using this method we will never have to propogate more than the orbital period.

// DISABLED FOR NOW
/*
	if(sec<0)
	{
		// Initialize orbit at previous time
		orbit(ORBIT_INIT,local_copy_init_time,0.0,orbitData);

		// Compute orbital period from position and velocity.
		period = orbitPeriod(orbitData);

		// Find the remaining seconds we need to propogate
		prop_sec = -sec%period;

		// ---------------- Debug
		sprintf(dbgbuf,"\r\nOrbit Initialized at "); 
		sendDBGALL(USER_PORT,dbgbuf);
		PrintTime(local_copy_init_time,USER_PORT);
		sprintf(dbgbuf,"\r\nPeriod: %li  Full Orbits: %li  Remaining Seconds: %li",period,(-sec/period),prop_sec); 
		sendDBGALL(USER_PORT,dbgbuf);
		// ----------------
		
		// Now our resolution should be associated with how many seconds we have to propogate.
		// We want to aim for about 500 loops
		// So 0 --> 125 : propogate with dt = .25
		//  125 --> 250 : propogate with dt = 0.5
		// 250 --> 500  : propogate with dt = 1.0
		// 500 --> 1000 : propogate with dt = 2.0
		// 1000 --> 5000 : propogate with dt = 5.0
		// over 5000     : propogate with dt = 10.0
		// It reduces accuracy, but it will allow us to boot faster.
		if(prop_sec<125) dt = 0.25;
		else if (prop_sec<250) dt = 0.5;
		else if (prop_sec<500) dt = 1.0;
		else if (prop_sec<1000) dt = 2.0;
		else if (prop_sec<5000) dt = 5.0;
		else 					dt = 10.0;

		for(i=0;i<prop_sec;i++)
		{
			restart_wdt();
			orbit(ORBIT_NO_INIT,currentTime,dt,orbitData);
		}
		// ---------------- Debug
		sprintf(dbgbuf,"\r\nOrbit Propogated to "); 
		sendDBGALL(USER_PORT,dbgbuf);
		PrintTime(currentTime,USER_PORT);
		sendDBGALL(USER_PORT,"\r\nOrbit Data: ");
		orbitPrintData(1);
		// ----------------		
	}
	else	// If the time difference is zero or positive, or way outside what we want to propogate,
*/			// the orbit initialization just needs to be done with respect to the currentTime.
	{
		// Call orbit with the initialization flag and current time
		orbit(ORBIT_INIT,currentTime,0.0,orbitData);
	}
}

//==========================================================================
void orbit(unsigned char initFlag, unsigned char *DateTime, float dt, float *orbit){

   int i;				
   float x[9];
   static float gha=0.0;
   static float LastStep;
   static unsigned char initialized=0;
   const float GHAatEpoch= 1.75488;			// Epoch Jan 1 00:00:00 2010
   float *r_tod, *v_tod, *a_tod, *tom, *dom;

   tom=   &orbit[0];
   dom=   &orbit[3];
   r_tod= &orbit[6];
   v_tod= &orbit[9];
   a_tod= &orbit[12];

   	if (initFlag){ 				// Compute stuff since model epoch based on init DateTime
		for(i=0;i<3;i++){
			tom[i]=       0.0; 	// Thrust
			dom[i]=       0.0;	// Drag
			}
        gha = (float)GMT2SEC(DateTime)*ERATE + GHAatEpoch; 
		gha = fmodfred(gha,TWOPI);
		LastStep=(float)DateTime[6];	// This is the seconds part of time vector
	    orbit[15]= gha;   
		initialized=1;
	  	return;
   	}

	if(!initialized){			// Zero data and bail out here if never loaded ephemeris
		for(i=0;i<16;i++){
			orbit[i]=0.0;
			}
		return;
	}

//===Resolve integration step size based on RTC or ISR ======

    if(dt==0.0){				// The RTC decides integration step in this case
		dt= (float)DateTime[6]-LastStep;
		if(dt<0.0)dt+=60.0;
		LastStep= (float)DateTime[6];
	}
	else LastStep+=dt;
   if(dt<=0.0)return;			// Do nothing, wait for time to catch up
   
   gha+= dt*ERATE;
   gha = fmodfred(gha,TWOPI);
   orbit[15]= gha;
 
   m2feet(r_tod);
   m2feet(v_tod);
   m2feet(a_tod);
  
   for (i = 0; i < 3; i++){			// convert meters to feet
      x[i] = 	r_tod[i];
      x[i + 3] = v_tod[i];
      x[i + 6] = a_tod[i];
    }

   rk4(x, dt, tom, dom);	// This is a rk4 orbit propagator (uses feet, not meters...)

   for (i = 0; i < 3; i++)			
   {
      r_tod[i] = x[i];
      v_tod[i] = x[i + 3];
      a_tod[i] = x[i + 6];
    }
   feet2m(r_tod);					// Back to meters
   feet2m(v_tod);
   feet2m(a_tod);
}

void orbitPrintData(unsigned char format=0)
{
	
	int i;

	switch(format)
	{
		case 0: // Default, everything comma-seperated
			sendDBGALL(USER_PORT,"\r\n");
			for(i=0;i<ORBIT_DATA_SIZE;i++)
			{	
				printFloat(dbgbuf,orbitData[i],6);
				sendDBGALL(USER_PORT,dbgbuf);
				if(i!=ORBIT_DATA_SIZE-1)
			    	sendDBGALL(USER_PORT,", ");
			}
			sendDBGALL(USER_PORT,"\r\n");
			break;

		case 1: // For Excel, just the orbit parameters (no time, thrust or drag), 
				// no leading CR, comma-seperated 
			for(i=6;i<ORBIT_DATA_SIZE;i++)
			{
				printFloat(dbgbuf,orbitData[i],6);
				sendDBGALL(USER_PORT,dbgbuf);
				if(i!=ORBIT_DATA_SIZE-1)
			    	sendDBGALL(USER_PORT,", ");
			}
			sendDBGALL(USER_PORT,"\r\n");
			break;

		case 2: // HexPrint
			HexPrint(USER_PORT,orbitData,ORBIT_DATA_SIZE*sizeof(float));
			break;

		default: break;
	}
}



// What does this function do?
/**
 */
void eval (float *x,float *f,float *tom,float *dom)
{
   int i;
   float pertx, perty, pertz, r[3], reor, reor2, reor3, reor4, rmag; 
   float re, rmag3, px1, px2, px3, px31, px4, pz1, pz2, pz3, pz4; 
   float pz5, pz6, v[3], zor, zor2, zor4;
   
   re = REM * CNMF;

   for (i = 0; i < 3; i++)
   {
      r[i] = x[i];
      v[i] = x[i + 3];
      f[i] = v[i];
   }
   
   rmag = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
   reor = re/rmag;
   reor2 = reor*reor;
   reor3 = reor2*reor;    
   reor4 = reor2*reor2;  

   zor = (r[2])/rmag;
   zor2 = zor*zor;
   zor4 = zor2*zor2;
   rmag3 = rmag*rmag*rmag;

   px1 = J2*reor2*1.5e0*(1.e0 - 5.e0*zor2);
   px2 = J3*zor*reor3*2.5e0*(3.e0 - 7.e0*zor2);  
   px3 = J4*reor4*0.625e0*(-3.e0 + 42.e0*zor2 - 63.e0*zor4);   
   px31 = -693.e0*zor4 + 630.e0*zor2 - 105.e0;   
   px4 = J5*reor4*reor*zor*0.125e0*px31;   
   pertx = -G_M*r[0]/rmag3*(px1 + px2 + px3 + px4);
   
   f[3] = -G_M*r[0]/rmag3 + pertx + tom[0] + dom[0];

             
   if (r[0] == 0.0)perty = pertx;
   else perty = pertx*r[1]/r[0];
   
   f[4] = -G_M*r[1]/rmag3 + perty + tom[1] + dom[1];

   if (r[2] == 0.e0)
   {
      pertz = 0.e0;
   }
   else 
   {
      pz1 = J2*reor2*1.5e0*(3.e0 - 5.e0*zor2);
      pz2 = -1.e0 + 10.e0*zor2 - 35.e0/3.e0*zor4;
      pz3 = J3*reor3*rmag/r[2]*1.5e0*pz2;
      pz4 = J4*reor4*.625e0*(70.e0*zor2 - 63.e0*zor4 -15.e0);
      pz5 = 15.e0 - 315.e0*zor2 + 945.e0*zor4 - 693.e0*zor2*zor4;
      pz6 = J5*reor4*reor*rmag/r[2]* 0.125*pz5;
      
      pertz = -G_M*r[2]/rmag3*(pz1 + pz3 + pz4 + pz6);
      
   }
   
   f[5] = -G_M*r[2]/rmag3 + pertz + tom[2] + dom[2];

   return;
} 


/**
 * This method is called only from ephem.
 *
 * x is a vector of 9 floats originally from orbitData: 3 for position, then
 * 3 for velocity, then 3 for acceleration.
 *
 * time is a local variable of ephem, initialized to 0
 *
 * dt is the delta-t, which originally comes from main
 *
 * tom and dom and thrust and drag from the orbitData, 3 floats each
 */
void rk4(float *x,float dt,float *tom,float *dom)
{
   int i, j, l, m, n;
   float f[6]={0.0,0.0,0.0,0.0,0.0,0.0};
// TK not used   float re;
   float k[4][6];
   float a[4][4];
  float xx[6];
  float indexFighter;
 
	memset(k,0x00,4*6*sizeof(float));	// Clear out matrix
	memset(xx,0x00,6*sizeof(float));	// Clear out matrix

    a[0][0]=0.15e0;
    a[0][1]=0.1536e0;
    a[0][2]=6.7452657112e0;
    a[0][3]=1.4143518519e0;
    a[1][0]=0.e0;
    a[1][1]=0.0384e0;
    a[1][2]= -38.7783195429e0;
    a[1][3]= -9.5860566449e0;
    a[2][0]=0.e0;
    a[2][1]=0.e0;
    a[2][2]=33.0330538318e0;
    a[2][3]=8.9527181885e0;
    a[3][0]=0.e0;
    a[3][1]=0.e0;
    a[3][2]=0.e0;
    a[3][3]=0.2189866045e0;
// TK will be overridden, do not bother initialing here   m = 0;
// TK not used   re = REM * CNMF;

   // x is input - 9 floats, xx is local variable. Load xx with position and
   // velocity
  for (i = 0; i < 6; i++)
		xx[i] = x[i];

   for (j = 0; j < 4; j++)
   {
	 eval(x, f, tom, dom);	

      m = j + 1;		// goes as high as j+1=4

//-------------------------------------------
// Not sure why, but the compiler has issues with
// this block of code...

	for (n = 0; n < 6; n++)	{
        k[j][n] = ((f[n])*(dt));  //This line causes a reboot, needs fixing
        x[n] = xx[n];				// This line also causes problems in latest compiler
		 for (l = 0; l < m; l++) {	// This one too... ugh...
//			x[n] = x[n] +  a[l][j]*k[l][n];		// Sets pos and vel

// x is 9 floats: vel, pos, accel; a is [4][4], k is [4][6]
// n is 0..5, j is 0..3, l is 0..m-1 = 0..j=0..4
// The compiler has issues with multiple indexes in one expression.
			 indexFighter = a[l][j];
			 indexFighter = indexFighter * k[l][n];
			 x[n] = indexFighter + x[n];
		 }
      }
   }

//-------------------------------------------

   for (i=0; i < 3; i++)
		x[i + 6] = f[i + 3];	// Set Accels

//  	*time = *time + dt;		// Not much purpose in incrementing the time.  It isn't used anywhere in the calling function.

   return;
}


//==========================================================================
// How does this model work?
void sunmodel(unsigned char *gmt, float *sunvec){
	float sunlat,sunlong,suntranom;
	float sunmanom,dse;
	float dcl_ra[2];
	long tin;	
	
	// Note: This function is meant to take time in GMT (Greenwich Mean Time), not local time.
	tin= GMT2SEC(gmt);						// epochsec= Jan 1, 2000 00:00:00
    dse = (float)tin/86400.0;				// This is days since epoch

	// (360/365.2644)[just under 1] * days since epoch + Sun At Epoch (280) - Long At Perigee (282)
    sunmanom = (360.0/TROPICALYR) * dse + SUNATEPOCH - LONGATPERIGEE;
	sunmanom=inrange(sunmanom); 			// must be between -180 ->180

    suntranom = sunmanom + (360.0/PI) * SUNECC * degsin(sunmanom);
	suntranom= inrange(suntranom);

    sunlong = suntranom + LONGATPERIGEE;
	sunlong= inrange(sunlong);

    sunlat = 0.0;

    eclip2eci(sunlat,sunlong,dcl_ra);
	dcl_ra[1]= inrange(dcl_ra[1]);
	
	cartesian(dcl_ra[1],dcl_ra[0],sunvec);		
		
	sunvec[0]=sunvec[0]*1000.0;
	sunvec[1]=sunvec[1]*1000.0;
	sunvec[2]=sunvec[2]*1000.0;
}

     
/*********************************************************/

/*
void moonsun(float jtime,float *sundcl,float *sunra,float *moondcl,float *moonra,float *moonph)
{
    //extern double degsin();
    //extern double degcos();
	//float esec,
    float cLOCAL[10],sunlat,sunlong,suntranom;
    float moonmlong,deltlong,moonclong,mooneclong;
    float mooneclat,moonage,moontrlong;
    float epochsec,sunmanom,dse,moonmanom,mooncnode;
    float mooncanom,moonmnode;
    float ev,ec,a42,a3,ae,v,tempa,tempb;
    int epochA[7];     
                                      
    epochA[0]=1;epochA[1]=0;epochA[2]=1980;epochA[3]=0;epochA[4]=0;
    epochA[5]=0;epochA[6]=0;

    cLOCAL[0]=13.1763966;cLOCAL[1]=0.1114041;cLOCAL[2]=0.0529539;cLOCAL[3]=0.1858;
    cLOCAL[4]=0.37;cLOCAL[5]=6.2886;cLOCAL[6]=0.214;cLOCAL[7]=0.6583;cLOCAL[8]=0.16;cLOCAL[9]=1.2739;

    gmt2utc(epochA,&epochsec);
    dse = (jtime - epochsec)/86400.e0;

    sunmanom = (360.e0/TROPICALYR) * dse + SUNATEPOCH - LONGATPERIGEE;
    inrange(&sunmanom);

    suntranom = sunmanom + (360.e0/PI) * SUNECC * degsin(sunmanom);

    inrange(&suntranom);

    sunlong = suntranom + LONGATPERIGEE;
    inrange(&sunlong);

    sunlat = 0.e0;
    eclip2eci(sunlat,sunlong,sundcl,sunra);
    inrange(sunra);

    moonmlong = cLOCAL[0]*dse + L0;
    inrange(&moonmlong);

    moonmanom = moonmlong - cLOCAL[1]*dse - P0;
    inrange(&moonmanom);

    moonmnode = N0 - cLOCAL[2] * dse;
    inrange(&moonmnode);

    deltlong = moonmlong - sunlong;
    tempa = 2.e0 *deltlong;
    tempb = tempa - moonmanom;

 //eft   ev = cLOCAL[9] * degsin(temp2);
	ev = cLOCAL[9] * degsin(tempb);
    ae = cLOCAL[3] * degsin(sunmanom);
    a3 = cLOCAL[4] * degsin(sunmanom);

    mooncanom = moonmanom + ev - ae - a3;

    ec = cLOCAL[5] * degsin(mooncanom);
    a42 = cLOCAL[6] * degsin(2.e0 * mooncanom);

    moonclong = moonmlong + ev + ec - ae + a42;
    v = cLOCAL[7] * degsin(2.e0 * (moonclong - sunlong));
    moontrlong = moonclong + v;
    mooncnode = moonmnode - cLOCAL[8] * degsin(sunmanom);

    tempa = degsin(moontrlong - mooncnode)*degcos(MOONINC);
    tempb = degcos(moontrlong - mooncnode);

    mooneclong = atan2(tempa,tempb)*RAD2DEG + mooncnode;
    mooneclat = asin(degsin(moontrlong - mooncnode)*degsin(MOONINC));
    mooneclat = mooneclat * RAD2DEG;

    inrange(&mooneclong);
    eclip2eci(mooneclat,mooneclong,moondcl,moonra);
    inrange(moonra);

    moonage = (moontrlong - sunlong);

    *moonph = 0.5e0 * (1.0 - degcos(moonage));

    return;

}

void SUNMOON (int *iinfo,float *rinfo,float *u,int *nu,float *x,float *xdot,int *nx,float *y,int *ny,float *rpar,int *ipar)
{

  int epoch[7],hmstime[7];
  float sunvec[3],moonvec[3];
  float sundcl,sunra,moondcl,moonra,moonph,dt;
  static float tin = 0.0;
  int init;
  int state;
  int output;

  init      = iinfo[1]==1;
  state     = iinfo[2]==1;
  output    = iinfo[3]==1;

    
  //extern void gmt2utc();
  //extern void cartesian();
  //extern double degcos();
  //extern double degsin();
  //extern void utc2gmt();
  //extern void moonsun();
     
  dt = 0.1;

  if ((int)u[2] < 1958)
  {
    return;
  }

  if( (init)  || (u[7] > 0.5) )
  {
    tin      = 0.0;
    epoch[0] = (int) u[0];
    epoch[1] = (int) u[1];
    epoch[2] = (int) u[2];
    epoch[3] = (int) u[3];
    epoch[4] = (int) u[4];
    epoch[5] = (int) u[5];
    epoch[6] = (int) u[6]*10.0;		// entered centiseconds, gmt2utc wants milliseconds 
 
    gmt2utc(epoch,&tin);
  }


//  if (!first)

  {
    utc2gmt(tin,hmstime);

    moonsun(tin,&sundcl,&sunra,&moondcl,&moonra,&moonph);

    cartesian(sunra,sundcl,sunvec);
    cartesian(moonra,moondcl,moonvec);

    y[0] = (float) sunvec[0];
    y[1] = (float) sunvec[1];
    y[2] = (float) sunvec[2];

    y[3] = (float) moonvec[0];
    y[4] = (float) moonvec[1];
    y[5] = (float) moonvec[2];
      
    y[6] = (float) moonph;
    y[7] = (float) sundcl;
    y[8] = (float) sunra;
    y[9] = (float) moondcl;
    y[10] = (float) moonra;  
    y[11] = (float) tin;

    y[12] = (float) hmstime[0];
    y[13] = (float) hmstime[1];
    y[14] = (float) hmstime[2];
    y[15] = (float) hmstime[3];
    y[16] = (float) hmstime[4];
    y[17] = (float) hmstime[5];
    y[18] = (float) hmstime[6];

    tin = tin + dt;

  }        
  return;
}

*/

/* ****************************************************************** */
/*   m2feet,f2meter,tsec                                       */
/*  degsin, degcos, inrange, eclip2eci,  cartesian            */
/*  dcm2q                                                             */
/* ****************************************************************** */ 
//#define DEG2RAD 0.01745329
//#define RAD2DEG 57.2957787
#define SPY 31536000
#define SPD 86400

/* ****************************************************************** */
/** Round the foat number to integer */
int Rint(float num)
{
   int rnumber;
   float modnum;
   float rvalue;
   rvalue = 0.0e0;

   modnum = modf(num,&rvalue);
   
   if (modnum >= 0.5)
     rnumber = (int) (num + 1.0);
   else
     rnumber = (int) (num); 
   
   return(rnumber);

}
 
/* *********************************************************** */
  
void m2feet(float *a)
{
   int i;
   for (i = 0; i < 3; i = i + 1)
   {
      a[i] = a[i] / 0.3048;
    }
   return;
}  

/* *************************************************************** */

void feet2m(float *a)
{
   int i;
   for (i = 0; i < 3; i = i + 1)
   {
      a[i] = a[i] * 0.3048;
    }
   return;
}  
/* *************************************************************** */
/** Reimplementation of % ( a modulo d ) */
float fmodfred(float a,float b)
{
   if(a < b)return a ;    
   // TK: need to return something even if a==b, removing "if(a > b)"
   return (a/b - (int)(a/b))*b;
}


/* *************************************************************** */

float degcos(float angle)
{
    float dgcos;

    dgcos = cos((angle*DEG2RAD));
    
    return(dgcos);

}

/* *************************************************************** */
// Takes an angle in degrees, converts to radians, then takes the sine of that angle.
float degsin(float angle)
{
    float dgsin;

    dgsin = sin((angle*DEG2RAD));

    return(dgsin);

}

/* ***************************************************************** */
// Takes an angle and scales it to +- 180 (I think...)
float inrange(float angle)
{

      //extern float fmodfred();
      float ang;

      ang = fmodfred(angle,360.0);
     
      if (ang < 0.00) ang = ang + 360.0;
	  if (ang >180.0) ang=180.0-ang;		// Keep within +- 180

      return ang;
} 
    
/* **************************************************************** */

void eclip2eci(float latin,float longin,float *val)
{
    float lat,longt;
	float phi;
	float temp1, temp2;

    phi = 23.441884 * DEG2RAD;
    lat = latin *     DEG2RAD;    
    longt = longin *  DEG2RAD;

    temp1 = sin(lat)*cos(phi) + cos(lat)*sin(phi)*sin(longt);
    val[0] = asin(temp1)*RAD2DEG;							// declination
    temp1 = sin(longt)*cos(phi) - tan(lat)*sin(phi);
    temp2 = cos(longt);								
    val[1] = atan2(temp1,temp2)*RAD2DEG;					// right ascension

}

/* *************************************************************** */

void cartesian(float theta,float delta,float *outvec)
{
    //extern float degcos();
    //extern float degsin();
    
    /*  theta = right ascension   */
    /*  delta = declination       */

    outvec[0] = degcos(theta)*degcos(delta);
    outvec[1] = degsin(theta)*degcos(delta);
    outvec[2] = degsin(delta);

    return;

}

//======================================================================
long int GMT2SEC(unsigned char *gmt)		// gmt format is iControl mmddyywdhhmmss
{
int years,days,leap_days,total_days;
int days_in_month[]={0,0,31,59,90,120,151,181,212,243,273,304,334};
long int seconds;
unsigned char gmtzero;

days=  (int)gmt[1]-1;			// Days ranges from 1-31
years= (int)gmt[2]-10;			// Buf[2] is 0-255 This converts seconds since 2010
gmtzero=gmt[0];
days+=days_in_month[gmtzero];	// Add days from previous month

if((gmt[2]%4==0) && (gmt[0]>2))days+=1;			// Add feb29 if necessary (for this year)
if(years>0)leap_days=  (years-1)/4 ;			// Add previous years leap days
else leap_days =0;
total_days= years*365 + days + leap_days; // 

seconds= (long)total_days*86400 + (long)gmt[4]* 3600 + (long)gmt[5]* 60 + (long)gmt[6];

return seconds;
}



// Compute orbital period (to nearest second) from position and velocity.
// Method:
//  -- Compute radius in meters
//  -- Compute period (T= squre root of 4,pi-squared,r-cubed,over universal gravitational constant and planet mass)
long orbitPeriod(float* oD)
{
	double radius;
	double period;
	const double gm = 398600.4418; //km^3/s^2

	// Compute Radius
	radius = sqrt(oD[ORBIT_POS_X_INDEX]*oD[ORBIT_POS_X_INDEX]+
				  oD[ORBIT_POS_Y_INDEX]*oD[ORBIT_POS_Y_INDEX]+
				  oD[ORBIT_POS_Z_INDEX]*oD[ORBIT_POS_Z_INDEX]);

	radius = radius/1000; // convert to kilometers

	// Compute Period in seconds
	period = sqrt(4*PI*PI*radius*radius*radius/gm);

	// Truncate to long
	return (long) period;
}