#ifndef __PID_H__
#define __PID_H__

typedef struct 
{
	float  Ref;   			// Input: Reference input
	float  Fdb;   			// Input: Feedback input
	float  Err;				// Variable: Error
	
	float  Kp;				// Parameter: Proportional gain
	float  Ki;			    // Parameter: Integral gain
	float  Kd; 		        // Parameter: Derivative gain
	
	float  Up;				// Variable: Proportional output
	float  Ui;				// Variable: Integral output
	float  Ud;				// Variable: Derivative output
	float  OutPreSat; 		// Variable: Pre-saturated output
	float  OutMax;		    // Parameter: Maximum output
	float  OutMin;	    	// Parameter: Minimum output
	float  Out;   			// Output: PID output
	float  SatErr;			// Variable: Saturated difference
	float  Kc;		     	// Parameter: Integral correction gain
	float  Up1;		   	    // History: Previous proportional output
	void  (*calc)();	  	// Pointer to calculation function
	void  (*clear)();
} PID;
/*------------------------------------------------------------------------------
Prototypes for the functions in PID.C
------------------------------------------------------------------------------*/
void PidCalc(PID *);
void PidClear(PID *v);

#endif
