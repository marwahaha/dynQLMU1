#ifndef __DEFINE_H_INCLUDED__
#define __DEFINE_H_INCLUDED__

#include <vector>
#include <iostream>
#include "mkl.h"
#include "mkl_types.h"
#define DIM 2
#define PI 2*asin(1.0)

extern int *next[2*DIM+1];
extern int *nextCHK[2*DIM+1];
extern int *chk2lin,*lin2chk;
extern int LX,LY,VOL,VOL2;
extern int nWindSector;
extern unsigned int nWind;
extern int **lookup;
extern double lam,range;
extern int SEED;
extern std::vector<double> lam_loc;
/* NTOT = total no of basis states 
 * NH   = states not killed by H  */
extern int NTOT;

extern std::vector<std::vector<bool>> basis;
extern std::vector<std::vector<bool>> basis_nonflip;
extern std::vector<std::vector<bool>> basis_flip;

/* user defined classes */
class WindNo{
  public:
    // labeling the sectors
    int Wx,Wy;

    // basis states
    long int nBasis;
    std::vector<std::vector<bool>> basisVec;
    // no of flippable plaquettes
    std::vector<int> nflip;
    // index of the basis state
    std::vector<int> cflip;
    // diagonal part of the Hamiltonian
    std::vector<double> lamH;
    // Hamiltonian in the sector
    std::vector<MKL_INT> rows,cols;
    std::vector<double> hamil;
    double getH(int,int);
    // simple routine to check the direct way to extract 
    // elements from a sparse matrix in the CSC representation
    void check_getH();

    // eigenvalues and eigenvectors
    // LAPACK stores the evecs as 1-d arrays
    std::vector<double> evals;
    std::vector<double> evecs;

    // function to display variables
    void display(){
     printf("(Wx,Wy)=(% d,% d) with #-of-states= %ld \n",Wx,Wy,nBasis);
    } 

    // function to count flippable plaquettes
    void flip_plaq(); 

    // function to sort the basis states 
    void sortbasis();

    // function to calculate the off-diagonal matrix elements of the Hamiltonian
    // in the winding number basis
    int scan(std::vector<bool>&);
    int binscan(std::vector<bool>&);

    // Default constructor
    WindNo(){
      Wx = -LX; Wy = -LY;
      nBasis = 0;
    }
   
    // Destructor 
    ~WindNo(){
       basisVec.clear();
       rows.clear();
       cols.clear();
       hamil.clear();
       evals.clear();
       evecs.clear();
     }
};

extern std::vector<WindNo> Wind;

/* routines */
void initneighbor(void);
void conststates(void);
void constH(int);
void winding_no_decompose(void);
void calc_Oflip(int);
#endif 