#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include "define.h"
#include<iostream>
#include<algorithm>
#include<vector>
#include<iterator>

/* according to the rules of cpp, the variables are declared here 
 * and also in the header file as extern such that they are avl to
 * the other functions.
 */
int *next[2*DIM+1];
int *nextCHK[2*DIM+1];
int *chk2lin,*lin2chk;
int LX,LY,VOL,VOL2;
// Labels the winding number sectors
// lookup[LX/2+wx][LY/2+wy] refers to the (wx,wy) winding number sector
int nWindSector;
int **lookup;
std::vector<WindNo> Wind;
unsigned int nWind;
double lam,Ti,Tf,dT;
int NTOT,NH;
std::vector<std::vector<bool>> basis;
std::vector<std::vector<bool>> basis_nonflip;
std::vector<std::vector<bool>> basis_flip;
int STORE_SVD;
int CHKDIAG;
// initial state is specified by W and LR
// W is the amount of y-flux=Wy. 
// LR=0 flux to the left (subsystem LA), LR=1 flux to the right (subsystem LB).
int WX, WY, LR;

int main(){
  FILE *fptr;
  char string[50];
  int i,d,p,q;
  int x,y;
  int sector;
  WindNo SectorZero;
  extern void initneighbor(void);
  extern void conststates(void);
  extern void printbasis(void);
  extern int** allocateint2d(int, int);
  extern void deallocateint2d(int**,int,int);
  extern int calc_WindNo(int,int);

  fptr = fopen("QUEUE_Tev","r");
  if(fptr == NULL){
      printf("could not open QUEUE FILE to open\n");
      exit(1);
  }
  fscanf(fptr,"%s %d\n",string,&LX);
  fscanf(fptr,"%s %d\n",string,&LY);
  fscanf(fptr,"%s %lf\n",string,&lam);
  fscanf(fptr,"%s %d\n",string,&LEN_A);
  fscanf(fptr,"%s %d\n",string,&WX);
  fscanf(fptr,"%s %d\n",string,&WY);
  fscanf(fptr,"%s %d\n",string,&LR);
  fclose(fptr);
  if(( LX%2 != 0 )||( LY%2 !=0 )) { printf("Code does not work with odd LX and/or LY. \n"); exit(0); }
  if(LX<LY) printf("Please make sure LX >= LY. Unforseen errors can occur otherwise. \n");
  VOL = LX*LY;
  VOL2 = VOL/2;

  // decide whether to check the results of the diagonalization 
  CHKDIAG=0;

  /* Initialize nearest neighbours */
  for(i=0;i<=2*DIM;i++){
    next[i] = (int *)malloc(VOL*sizeof(int)); 
    nextCHK[i] = (int *)malloc(VOL*sizeof(int));
  }

  /* Initialize checkerboard co-ordinates */
  lin2chk = (int *)malloc(VOL*sizeof(int));
  chk2lin = (int *)malloc(VOL*sizeof(int));
  initneighbor();
  
  /* Winding number sectors */
  lookup = allocateint2d(LX+1,LY+1);

  /* build basis states satisfying Gauss' Law */
  conststates();
  
  /* get number of winding number sectors */
  nWind = calc_WindNo(LX,LY);
  Wind.reserve(nWind); 
  winding_no_decompose();
 
  printf("Chosen (Wx,Wy) sector = (%d,%d)\n",WX,WY); 
  sector = lookup[LX/2+WX][LY/2+WY];
  constH(sector);
  // calculate the <psi_n| O_flip |psi_n>, for every eigenstate psi_n
  calc_Oflip(sector);
  
  // real-time evolution of Entanglement Entropy (storing the SVD coefficients)
  evolve_Eent(sector, WX, WY);

  // real time evolution of <PHI| O_flip(t) |PHI>, 
  //  where |PHI> is an initial cartoon state
  //calc_Oflipt(sector, WX, WY);

  // real-time evolution of cartoon states and Locshmidt Echo in (wx,wy)=(0,0) 
  // if((WX==0)&&(WY==0)) evolve_cartoons(sector);


  // real-time correlation function
  //evolve_corrf1(sector);

  // calculate the Entanglement Entropy for the states
  //entanglementEntropy(sector);

  /* Clear memory */
  for(i=0;i<=2*DIM;i++){  free(next[i]); free(nextCHK[i]); }
  free(chk2lin); free(lin2chk);
  deallocateint2d(lookup,LX+1,LY+1);
  Wind.clear();

  return 0;
}
