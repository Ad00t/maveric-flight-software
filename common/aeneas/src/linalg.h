/***************************************************************************
					Generic Linear Algebra Functions

Author: Michael Aherne
Company: USC ISI
License: Open Source

Purpose: To provide some simple functions for vectors and matrices.

Usage: See the descriptions of each function below.

Changelog:

Date 	 |  Au.  |	Notes
10-06-11	mra		Created from old Aeolus code.

*******************************************************************************
*/



#ifndef __LINEAR_ALG__
#define __LINEAR_ALG__

//========================================
//  			Dependencies
//========================================
#include "common.h"
//#include "pins.h"
//#include "errors.h"						// Return_t error type, for error trapping
//#include "ports.h"						// Functions for writing to ports in general



//========================================
//  			Definitions
//========================================



//========================================
//  		Implementation
//========================================

/* ---------------------------- MATRIX MATH ROUTINES ------------------------*/
void MatrixMultiply(float* A, float* B, int m, int p, int n, float* C)
{
// A = input matrix (m x p)
// B = input matrix (p x n)
// m = number of rows in A
// p = number of columns in A = number of rows in B
// n = number of columns in B
// C = output matrix = A*B (m x n)
int i, j, k;
for (i=0;i<m;i++)
   for(j=0;j<n;j++)
      {
      C[n*i+j]=0;
      for (k=0;k<p;k++)
         C[n*i+j]= C[n*i+j]+A[p*i+k]*B[n*k+j];
      }
}

void MatrixAddition(float* A, float* B, int m, int n, float* C)
{
// A = input matrix (m x n)
// B = input matrix (m x n)
// m = number of rows in A = number of rows in B
// n = number of columns in A = number of columns in B
// C = output matrix = A+B (m x n)
int i, j;
for (i=0;i<m;i++)
   for(j=0;j<n;j++)
      C[n*i+j]=A[n*i+j]+B[n*i+j];
}

void MatrixSubtraction(float* A, float* B, int m, int n, float* C)
{
// A = input matrix (m x n)
// B = input matrix (m x n)
// m = number of rows in A = number of rows in B
// n = number of columns in A = number of columns in B
// C = output matrix = A-B (m x n)
int i, j;
for (i=0;i<m;i++)
   for(j=0;j<n;j++)
      C[n*i+j]=A[n*i+j]-B[n*i+j];
}

void MatrixTranspose(float* A, int m, int n, float* C)
{
// A = input matrix (m x n)
// m = number of rows in A
// n = number of columns in A
// C = output matrix = the transpose of A (n x m)
int i, j;
for (i=0;i<m;i++)
   for(j=0;j<n;j++)
      C[m*j+i]=A[n*i+j];
}

int MatrixInversion(float* A, int n, float* AInverse)
{
#define MAX_MATRIX 16

// A = input matrix (n x n)
// n = dimension of A
// AInverse = inverted matrix (n x n)
// This function inverts a matrix based on the Gauss Jordan method.
// The function returns 1 on success, 0 on failure.
int i, j, iPass, imx, icol, irow;
float det, temp, pivot, factor;
//unsigned long ac_base_addr;
float ac[MAX_MATRIX];                     // NOTE: IF nxn is greater than MAX_MATRIX, there will be a huge problem

//+++MRA assert(MAX_MATRIX>n*n);   //Test for array overrun

//Check for scalar
if (n==1)
   {
   AInverse[0] = 1/A[0];
   return 1;
   }


det = 1;
for (i = 0; i < n; i++)
   {
   for (j = 0; j < n; j++)
      {
      AInverse[n*i+j] = 0;
      ac[n*i+j] = A[n*i+j];
      }
   AInverse[n*i+i] = 1;
   }
// The current pivot row is iPass.
// For each pass, first find the maximum element in the pivot column.
for (iPass = 0; iPass < n; iPass++)
   {
   imx = iPass;
   for (irow = iPass; irow < n; irow++)
      {
      if (fabs(A[n*irow+iPass]) > fabs(A[n*imx+iPass])) imx = irow;
      }
   // Interchange the elements of row iPass and row imx in both A and AInverse.
   if (imx != iPass)
      {
      for (icol = 0; icol < n; icol++)
         {
         temp = AInverse[n*iPass+icol];
         AInverse[n*iPass+icol] = AInverse[n*imx+icol];
         AInverse[n*imx+icol] = temp;
         if (icol >= iPass)
            {
            temp = A[n*iPass+icol];
            A[n*iPass+icol] = A[n*imx+icol];
            A[n*imx+icol] = temp;
            }
         }
      }
   // The current pivot is now A[iPass][iPass].
   // The determinant is the product of the pivot elements.
   pivot = A[n*iPass+iPass];
   det = det * pivot;
   if (det == 0)
      {
      return 0;
      }
   for (icol = 0; icol < n; icol++)
      {
      // Normalize the pivot row by dividing by the pivot element.
      AInverse[n*iPass+icol] = AInverse[n*iPass+icol] / pivot;
      if (icol >= iPass) A[n*iPass+icol] = A[n*iPass+icol] / pivot;
      }
   for (irow = 0; irow < n; irow++)
      // Add a multiple of the pivot row to each row.  The multiple factor
      // is chosen so that the element of A on the pivot column is 0.
      {
      if (irow != iPass) factor = A[n*irow+iPass];
      for (icol = 0; icol < n; icol++)
         {
         if (irow != iPass)
            {
            AInverse[n*irow+icol] -= factor * AInverse[n*iPass+icol];
            A[n*irow+icol] -= factor * A[n*iPass+icol];
            }
         }
      }
   }
   return 1;
}

/*
This function will scale and rotate a vector
   INPUTS:
	- 3 float pointers are the vector x,y,z
    - a pointer to the scale (an array of 3 floats)
    - a pointer to the rotation matrix (an array of 9 floats)
   OUTPUT:
	- changes x,y,z to be scaled and rotated accordingly
*/
void ScaleAndRotate(float * x, float * y, float * z,
					float * scale,
					float * rot)
{

	float new[3];
	float output[3];

	// Adjust the scale
	new[0] = *x * scale[0];
	new[1] = *y * scale[1];
	new[2] = *z * scale[2];

	// Rotate the vector
		// A = input matrix (m x p)
		// B = input matrix (p x n)
		// m = number of rows in A
		// p = number of columns in A = number of rows in B
		// n = number of columns in B
		// C = output matrix = A*B (m x n)
	MatrixMultiply(new, rot, 1, 3, 3, output);

	// Copy output back to arguments
	*x = output[0];
	*y = output[1];
	*z = output[2];

	return;
}






#endif 

// EOF
