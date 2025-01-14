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
extern double lam,Ti,Tf,dT;
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
    // numbering the total (kx,ky) sectors
    int trans_sectors;
    // basis states
    long int nBasis;
    std::vector<std::vector<bool>> basisVec;
    std::vector<int> nflip;
    // labelling the Translation flags and the degeneracy
    std::vector<long int> Tflag;
    std::vector<int> Tdgen;

    // Hamiltonian in the WindNo sector
    std::vector<MKL_INT> rows,cols;
    std::vector<double> hamil;
    double getH(int,int);
    // simple routine to check the direct way to extract 
    // elements from a sparse matrix in the CSC representation
    void check_getH();
    // Hamiltonian in a (kx,ky) sector
    std::vector<std::vector<double>> hamil_Kxy;
    void allocate_Kxy(); 
    
    // eigenvalues and eigenvectors; at the moment they are used
    // for the Hamiltonian in the (kx,ky) basis
    std::vector<double> evals;
    std::vector<double> evecs;

    // function to display variables
    void display(){
     printf("(Wx,Wy)=(% d,% d) with #-of-states= %ld \n",Wx,Wy,nBasis);
    } 

    // Translation in X,Y
    void initTflag();
    void TransX(std::vector<bool>&, std::vector<bool>&, int);
    void TransY(std::vector<bool>&, std::vector<bool>&, int);
    void disp_Tprop();

    // function to count flippable plaquettes
    void flip_plaq();   
 
    // function to sort the basis states 
    void sortbasis();

    // function to search a transformed state in the winding number basis
    // the binary search implemented on the sorted basis states clearly 
    // outperforms the linear search
    int scan(std::vector<bool>&);
    int binscan(std::vector<bool>&);
    int binscan2(std::vector<bool>&);

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

// basis for subsystems A and B
extern std::vector<std::vector<bool>> eA;
extern std::vector<std::vector<bool>> eB;
// LEN_A, LEN_B are the sizes of subsystems A and B
// DA and DB are the respective Hilbert spaces
extern unsigned int LEN_A,LEN_B,VOL_A,VOL_B;
extern unsigned int DA, DB;

// Schmidt matrix and decomposed SVD 
extern double sel_eval;
extern std::vector<double> sel_evec;
extern double EE;
extern double shanonE, IPR, structE;

// functions to create the basis for subsystems A & B
extern void createBasis(int);
extern void schmidtDecom(std::vector<double>&, std::vector<std::vector<bool>>&, std::vector<std::vector<bool>>&,int);
extern void printvec(std::vector<double>&);


/* routines */
void initneighbor(void);
void conststates(void);
void constH(int);
void evolve_cartoons(std::vector<double>&, std::vector<std::vector<double>>&);
void winding_no_decompose(void);
void trans_decompose(int);
void trans_Hamil(int);
void calc_Oflip(int);
void entanglementEntropy(int);
void structuralEntropy(int);
#endif 
