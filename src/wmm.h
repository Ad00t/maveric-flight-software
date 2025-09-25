#ifndef WMM_H
#define WMM_H

/*
 * WMM
 * Wave-matching method for mode analysis of dielectric waveguides
 * Manfred Lohmeyer 
 * University of Osnabrueck, Department of Physics
 * Barbarastrasse 7, D-49069 Osnabrueck, Germany
 * (1999)
 * Manfred Hammer
 * University of Twente, Faculty of Mathematical Sciences
 * P.O. Box 217, 7500AE Enschede, The Netherlands
 * (2002)
 */

/*
 * wmm.h
 * WMM mode analysis
 */

/* parameters for WMM mode analysis */

/* expression used for normalization */
enum Vecnormmode {NRMMH, NRMME, NRMMZ, NRMMS};

/* field components imposed to be continuous */
enum Contcomp {CCALL, CCHXYZEZ, CCEXYZHZ};

class WMM_Parameters 
{
public:
	// for vectorial computations: formulation of the vectorial problem
	Vecform vform;
	// for vectorial computations: expression used for normalization 
	Vecnormmode vnorm;
	// for vectorial computations: components expl. enforced as continuous 
	Contcomp ccomp;

	// for an initial survey: coarse spectral discretization parameters 
	double ini_d_alpha;
	int    ini_N_alpha;
	double ini_alpha_max;

	// number of error evaluations for the initial bracketing    
	int ini_steps;
	// behaviour of the propagation constant fixing procedure
	//    number of spectral discretization refinements ini->fin
	int ref_num;
	//    exponent for consecutive search stepsize refinement
	double ref_exp;
	//    factor for spectral discretization refinements
	double ref_sdf;

	// for the final results: spectral discretization parameters 
	double fin_d_alpha;
	int    fin_N_alpha;
	double fin_alpha_max;

	// relative tolerance for the fixing of propagation constants
	double btol;

	// shifting parameter to enforce numerical positivity of the least
	// squares expression
	double mshift;

	// penalty factors for selection of closely spaced modes
	Dmatrix penfacTE;
	Dmatrix penfacTM;

	// set default values
	WMM_Parameters();

	// free allocated memory
	void strip();
};

/* ................................................................... */

/* complete WMM mode analysis, returns number of found modes */
int WMM_modeanalysis(Waveguide wg,       // the structure under consideration
                     Polarization pol,   // polarization type: QTE, QTM, VEC
                     Symmetry sym,       // enforced symmetry: SYM, ASY, NOS
		     double nmin,        // interval investigated for
		     double nmax,        //   effective mode indices 
		     WMM_Parameters par, // further WMM parameters
                     unsigned char ext0,          // extensions for the 
		     unsigned char ext1,          //   lsqerror log file 
                     WMM_ModeArray& mode);  // found modes 

/* WMM mode analysis, find highest (fundamental) mode in [k0*nmin, k0*nmax], 
   returns number of found modes */
int WMM_findfundmode(Waveguide wg,       // the structure under consideration
                     Polarization pol,   // polarization type: QTE, QTM, VEC
                     Symmetry sym,       // enforced symmetry: SYM, ASY, NOS
		     double nmin,        // interval investigated for
		     double nmax,        //   effective mode index 
		     WMM_Parameters par, // further WMM parameters
                     unsigned char ext0,          // extensions for the 
		     unsigned char ext1,          //   lsqerror log file 
                     WMM_ModeArray& mode);  // found mode 

/* WMM mode analysis, survey mju_beta on interval [k0*nmin, k0*nmax] */
void      WMM_survey(Waveguide wg,       // the structure under consideration
                     Polarization pol,   // polarization type: QTE, QTM, VEC
                     Symmetry sym,       // enforced symmetry: SYM, ASY, NOS
		     double nmin,        // interval investigated for
		     double nmax,        //   effective mode indices 
		     WMM_Parameters par, // further WMM parameters
                     unsigned char ext0,          // extensions for the 
		     unsigned char ext1);         //   lsqerror log file 

#endif // WMM_H

