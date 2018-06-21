#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<iterator>
#include "mkl_lapacke.h"
#include "define.h"

/* Auxiliary routines prototypes */
extern void print_matrix( char* desc, MKL_INT m, MKL_INT n, double* a, MKL_INT lda );
extern void fileprint_matrix( char* desc, MKL_INT m, MKL_INT n, double* a, MKL_INT lda );
extern void eig_print(std::vector<double>&,std::vector<double>&,int);

void diag_LAPACK(int sector, std::vector<double>& matrix, std::vector<double>& evals, std::vector<double>& evecs){
    unsigned i,j;
    // variabes to pass to LAPACK routine
    MKL_INT N, LDA, info;
    double *W, *A;
    N=Wind[sector].nBasis; 
    LDA=N;
    W = (double*)malloc(N*sizeof(double));
    A = (double*)malloc(N*LDA*sizeof(double));
    // convert the matrix to a long row
    for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      A[i*N+j]=Wind[sector].getH(i,j);
    }}
    /* print full matrix */
    //print_matrix("Full matrix", N, N, A, LDA);

    /* Executable statements */
    printf( "LAPACKE_dsyev (column-major, high-level) Example Program Results\n" );

    /* Solve eigenproblem */
    info = LAPACKE_dsyev( LAPACK_COL_MAJOR, 'V', 'U', N, A, LDA, W );
    printf("Back from the routine and working properly. Info = %d. Eval[0]=%f\n",info,W[0]);
    /* Check for convergence */
    if( info > 0 ) {
        printf( "The algorithm failed to compute eigenvalues.\n" );
        exit( 1 );
    }
    // copy eigenvalues and eigenvectors to main code
    evals.insert(evals.begin(), W, W+N);
    evecs.insert(evecs.begin(), A, A+N*N);

    // although these routines seem trivial, they are extremely important to
    // check the allocation of the eigenvectors and eigenvalues (on small lattices)
    // print copied evals and evecs
    //eig_print(evals,evecs,N);
    
    /* Print eigenvalues */
    //fileprint_matrix( "Eigenvalues.dat", 1, N, W, 1 );
    //print_matrix( "Eigenvalues", 1, N, W, 1 );
    /* Print eigenvectors */
    //print_matrix( "Eigenvectors (stored columnwise)", N, N, A, LDA );

    free(A); free(W);
}

// this routine uses RRR to diagonalize the matrix
void diag_LAPACK_RRR(MKL_INT N, MKL_INT NSQ, std::vector<double>& a, std::vector<double>& w, std::vector<double>& z){
    unsigned i,j;
    MKL_INT LDZ, LDA, NSELECT, info;    
    MKL_INT il, iu, m;
    double abstol, vl, vu;

    /* set array sizes */
    LDZ = N;
    NSELECT = N;
    LDA = N;

    /* print full matrix */
    double* A = &a[0];
    //print_matrix("Full matrix", N, N, A, LDA);

    /* allocate space for the arrays */
    std::vector<MKL_INT> isuppz(2*N); 

    /* Solve eigenproblem */
    MKL_INT* ISUPPZ = &isuppz[0];
    double* W = &w[0];
    double* Z = &z[0];
    info = LAPACKE_dsyevr( LAPACK_COL_MAJOR, 'V', 'A', 'U', N, A, LDA,
                        vl, vu, il, iu, abstol, &m, W, Z, LDZ, ISUPPZ );

    /* Check for convergence */
    if( info > 0 ) {
        printf( "The algorithm failed to compute eigenvalues.\n" );
        exit( 1 );
    }
    printf("Back from the routine and working properly. Info = %d. Eval[0]=%f\n",info,W[0]);

    /* Print eigenvalues */
    //fileprint_matrix( "Eigenvalues.dat", 1, N, W, 1 );
    //print_matrix( "Eigenvalues", 1, N, W, 1 );
    /* Print eigenvectors */
    //print_matrix( "Eigenvectors (stored columnwise)", N, N, Z, LDA );
    isuppz.clear();
}

// Notes on printing the eigenvalues and eigenvectors: Note that we have chosen a column-major
// layout, which means that the eigenvectors are arranged column-wsie. This means that the routine
// to print the matrix must have the form = i + j*N, where i is the index over row, and j is the
// index over columns. While printing the symmetric matrix, this however does not matter.
// Also, I think 'U' in the routine specifications means that the upper diagonal is free to be
// used as a workspace for the routine. Thus, only specifying the lower-diagonal is enough for
// the lapack routine.
/* Auxiliary routine: printing a matrix */
void print_matrix( char* desc, MKL_INT m, MKL_INT n, double* aa, MKL_INT lda ) {
        MKL_INT i, j;
        printf( "\n %s\n", desc );
        for( i = 0; i < m; i++ ) {
                for( j = 0; j < n; j++ ) printf( " % f", aa[i+j*lda] );
                printf( "\n" );
        }
}

/* Auxiliary routine: printing a matrix to a file */
void fileprint_matrix( char* desc, MKL_INT m, MKL_INT n, double* aa, MKL_INT lda ) {
        MKL_INT i, j;
        FILE *outfile;
        outfile = fopen(desc,"w");
        for( i = 0; i < m; i++ ) {
                for( j = 0; j < n; j++ ) fprintf(outfile, " %f \n", aa[i+j*lda] );
                fprintf(outfile, "\n" );
        }
}

 void eig_print(std::vector<double> &evals,std::vector<double> &evecs,int size){
   unsigned i,j;
   printf("Eigenvalues \n");
   for(i=0;i<size;i++) printf("% lf \n",evals[i]);
   printf("\n");
   printf("Eigenvectors \n");
   for(i=0;i<size;i++){ for(j=0;j<size;j++){
      printf("% lf \n",evecs[i*size+j]);
    }
    printf("\n");
   }
 };
