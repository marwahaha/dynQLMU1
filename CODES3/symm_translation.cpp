#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include "define.h"
#include<iostream>
#include<algorithm>
#include<vector>
#include<iterator>

 
 /* Decompose the Winding Number sector into states connected by translation flags */
 void trans_decompose(int sector){
   int i,flag,count;
   int ix,iy,q;
   std::vector<bool> init(2*VOL);
   std::vector<bool> new1(2*VOL);
   std::vector<bool> new2(2*VOL);

   // initialize the translation flag
   Wind[sector].initTflag();
   std::cout<<"No of basis states in this sector = "<<Wind[sector].nBasis<<std::endl;

   // Note that with the calculation of the degeneracy one has to be careful. 
   // Start the loop from (Tx,Ty)=(0,0), which corresponds to no translation. 
   // Every time a state is generated by translation, label it with the same flag.
   // and increase degeneracy in the loop. 
   flag=1;
   for(i=0;i<Wind[sector].nBasis;i++){
      // when the flag is non-zero, this has already been considered
      if(Wind[sector].Tflag[i]) continue;
      init = Wind[sector].basisVec[i];
      // set flag for a unassigned state
      Wind[sector].Tflag[i]=flag;
      // go through all possible translations, in x and in y
      for(iy=0;iy<LY;iy++){
      for(ix=0;ix<LX;ix++){
         Wind[sector].WindNo::TransX(init,new1,ix);   
         Wind[sector].WindNo::TransY(new1,new2,iy);   
         // check which basis state it corresponds to
         // note that the function binscan is much faster!
         //q = Wind[sector].scan(new2);
         q = Wind[sector].binscan(new2);
         // set flag of the new state
         if(q==-100){ printf("Error. Translated state not in Winding no sector. \n"); exit(0); }
         Wind[sector].Tflag[q]=flag;
         // increase degeneracy
         Wind[sector].Tdgen[q]++;
      }}
      flag++;
   }
   flag--;
   Wind[sector].trans_sectors = flag;
   // print and display statement to check, debug etc
   std::cout<<"No of (kx,ky) sectors="<<Wind[sector].trans_sectors<<std::endl;
   //Wind[sector].disp_Tprop();
 }

 // the inidividual basis (ice) states in the Winding number sectors are initially labelled with 0
 // the first bag of states labelled by lattice translations is labelled as 1, the second one as 2,
 // and so on. the total number of bags is the highest counter. 
 // the degeneracy is initialized with zero.
 void WindNo::initTflag(){
    int i;
    for(i=0;i<nBasis;i++){
      Tflag.push_back(0); 
      Tdgen.push_back(0);
    }
 }
 
 // Translate a basis state in a given winding number in the x-direction 
 // state --> stateTx;  by lattice translations (lx,0) and return the translated state
 void WindNo::TransX(std::vector<bool> &state,std::vector<bool> &stateT,int lx){
    int ix,iy,p,q;
    for(iy=0;iy<LY;iy++){
    for(ix=0;ix<LX;ix++){
       p = iy*LX + ix;         // initial point
       q = iy*LX + (ix+lx)%LX; // shifted by lx lattice units in +x dir
       stateT[2*q]=state[2*p]; stateT[2*q+1]=state[2*p+1];  
    }}
 }

 // Translate a basis state in a given winding number in the y-direction 
 // state --> stateTx;  by lattice translations (0,ly) and return the translated state
 void WindNo::TransY(std::vector<bool> &state,std::vector<bool> &stateT,int ly){
    int ix,iy,p,q;
    for(iy=0;iy<LY;iy++){
    for(ix=0;ix<LX;ix++){
       p = iy*LX + ix;            // initial point
       q = ((iy+ly)%LY)*LX + ix;  // shifted by ly lattice units in +y dir
       stateT[2*q]=state[2*p]; stateT[2*q+1]=state[2*p+1];  
    }}
 }

 int WindNo::binscan(std::vector<bool> &newstate){
     unsigned int m;
     // binary search of the sorted array  
     std::vector<std::vector<bool>>::iterator it; 
     it = std::lower_bound(basisVec.begin(),basisVec.end(),newstate);
     m  = std::distance(basisVec.begin(),it);
     if(it == basisVec.end()){
       std::cout<<"Element not found here! "<<std::endl; 
       return -100;
     }
     return m;
 }

 // Alternate implementation of the search. Find where the required element 
 // exists in the vector with the iterator, then the index.  
 int WindNo::binscan2(std::vector<bool> &newstate){
     unsigned int m;
     std::vector<std::vector<bool>>::iterator it; 
     it = std::find(basisVec.begin(),basisVec.end(),newstate);
     if(it == basisVec.end()){
       //std::cout<<"Element not found here! "<<std::endl; 
       return -100;
     }
     m  = std::distance(basisVec.begin(),it);
     return m;
 }


 void WindNo::disp_Tprop(){
   int i;
   for(i=0;i<nBasis;i++){
     std::cout<<"flag for basis state "<<i<<" = "<<Tflag[i]<<" Degeneracy ="<<Tdgen[i]<<std::endl;
   }
 }

 void trans_Hamil(int sector){
  extern void diag_LAPACK(int,std::vector<std::vector<double>>&,std::vector<double>&,
      std::vector<double>&);
  int i,j,k,l;
  double ele;

  std::cout<<"========================================================="<<std::endl;
  std::cout<<"Constructing the Hamiltonian in the (kx,ky)=(0,0) sector."<<std::endl;
  std::cout<<"Dimension of matrix = "<<Wind[sector].trans_sectors<<std::endl;

  // allocate space for hamil_Kxy
  Wind[sector].allocate_Kxy(); 

  // check the extraction of the Hamiltonian
  // for e.g. print out the whole matrix for a 2x2 system
  //Wind[sector].check_getH();

  // construct the hamil_kxy matrix
  for(i=0;i<Wind[sector].nBasis;i++){
     k=Wind[sector].Tflag[i]-1; 
     // though the flags start from 1; the indices start from 0
     for(j=0;j<Wind[sector].nBasis;j++){
       l   = Wind[sector].Tflag[j]-1;
       ele = Wind[sector].getH(i,j);
       //if(ele==0.0) continue;
       Wind[sector].hamil_Kxy[k][l] +=  ele*sqrt(Wind[sector].Tdgen[i]*Wind[sector].Tdgen[j])/VOL ;  
   }
  }

  // print matrix in translation basis
  //for(k=0;k<Wind[sector].trans_sectors;k++){
  //for(l=0;l<Wind[sector].trans_sectors;l++){
  //    printf("% .4lf ",Wind[sector].hamil_Kxy[k][l]);
  //  }
  //  printf("\n");
  // };
 
  // diagonalize the matrix with a LAPACK routine  
  diag_LAPACK(Wind[sector].trans_sectors,Wind[sector].hamil_Kxy,
    Wind[sector].evals,Wind[sector].evecs);
 
 }

 double WindNo::getH(int p,int q){
   double ele;
   double row1,row2;
   int c;

   ele=0.0;
   row1 = rows[p]-1; row2 = rows[p+1]-1;
   for(c=row1;c<row2;c++){
     if((q+1)==cols[c]) { ele=hamil[c]; break; }
   }
   return ele;
 }

 void WindNo::check_getH(){
    int i,j;
    double ele;
    printf("==================\n");
    printf("Printing the full matrix for checking \n");
    for(i=0;i<nBasis;i++){
    for(j=0;j<nBasis;j++){
      ele = getH(i,j);   
      printf("% .1lf ",ele);
    }
    printf("\n");
  }
 }

 void WindNo::allocate_Kxy(){
  unsigned int i,j;
  std::vector<double> myvec;
  if(trans_sectors <= 0){ printf("Error in allocation. \n"); exit(0); }
  for(i=0; i<trans_sectors; i++){
  for(j=0; j<trans_sectors; j++){
     myvec.push_back(0.0);
   } 
   hamil_Kxy.push_back(myvec); 
  }
  //printf("Successul allocation. \n");
  //printf("======================\n");
 }
