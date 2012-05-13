/* Usage: lmddrv < lm.data */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cminpack.h"
#include "vec.h"
#define real __cminpack_real__

/*     ********** */

/*     this program tests codes for the solution of n nonlinear */
/*     equations in n variables. it consists of a driver and an */
/*     interface subroutine fcn. the driver reads in data, calls the */
/*     nonlinear equation solver, and finally prints out information */
/*     on the performance of the solver. this is only a sample driver, */
/*     many other drivers are possible. the interface subroutine fcn */
/*     is necessary to take into account the forms of calling */
/*     sequences used by the function subroutines in the various */
/*     nonlinear equation solvers. */

/*     subprograms called */

/*       user-supplied ...... fcn */

/*       minpack-supplied ... dpmpar,enorm,hybrd1,initpt,vecfcn */

/*       fortran-supplied ... dsqrt */

/*     argonne national laboratory. minpack project. march 1980. */
/*     burton s. garbow, kenneth e. hillstrom, jorge j. more */

/*     ********** */

int fcn(void *p, int n, const real *x, real *fvec, int iflag);

struct refnum {
    int nprob, nfev, njev;
};

/* Main program */
int main(int argc, char **argv)
{

    int i,ic,k,n,ntries;
    struct refnum hybrdtest;
    int info;

    int na[60];
    int nf[60];
    int nj[60];
    int np[60];
    int nx[60];

    real factor,fnorm1,fnorm2,tol;

    real fnm[60];
    real fvec[40];
    real x[40];

    real wa[2660];
    const int lwa = 2660;

    int num5, ilow, numleft;

    tol = sqrt(dpmpar(1));

    ic = 0;

    for (;;) {
        scanf("%5d%5d%5d\n", &hybrdtest.nprob, &n, &ntries);
        if (hybrdtest.nprob <= 0.)
            break;

        factor = 1.;

        for (k = 0; k < ntries; ++k, ++ic) {
            hybipt(n,x,hybrdtest.nprob,factor);

            vecfcn(n,x,fvec,hybrdtest.nprob);

            fnorm1 = enorm(n,fvec);

            printf("\n\n\n\n      problem%5d      dimension%5d\n\n", hybrdtest.nprob, n);

            hybrdtest.nfev = 0;
            hybrdtest.njev = 0;

            info = hybrd1(fcn,&hybrdtest,n,x,fvec,tol,wa,lwa);

            fnorm2 = enorm(n,fvec);

            np[ic] = hybrdtest.nprob;
            na[ic] = n;
            nf[ic] = hybrdtest.nfev;
            hybrdtest.njev /= n;
            nj[ic] = hybrdtest.njev;
            nx[ic] = info;

            fnm[ic] = fnorm2;

            printf("\n      initial l2 norm of the residuals%15.7e\n"
                   "\n      final l2 norm of the residuals  %15.7e\n"
                   "\n      number of function evaluations  %10d\n"
                   "\n      number of Jacobian evaluations  %10d\n"
                   "\n      exit parameter                  %10d\n"
                   "\n      final approximate solution\n\n",
                   fnorm1, fnorm2, hybrdtest.nfev, hybrdtest.njev, info);
            num5 = n/5;

            for (i = 0; i < num5; ++i) {

                ilow = i*5;
                printf("     %15.7e%15.7e%15.7e%15.7e%15.7e\n",
                       x[ilow+0], x[ilow+1], x[ilow+2], x[ilow+3], x[ilow+4]);

            }

            numleft = n%5;
            ilow = n - numleft;

            switch (numleft) {
                case 1:
                    printf("     %15.7e\n",
                           x[ilow+0]);
                    break;
                case 2:
                    printf("     %15.7e%15.7e\n",
                           x[ilow+0], x[ilow+1]);
                    break;
                case 3:
                    printf("     %15.7e%15.7e%15.7e\n",
                           x[ilow+0], x[ilow+1], x[ilow+2]);
                    break;
                case 4:
                    printf("     %15.7e%15.7e%15.7e%15.7e\n",
                           x[ilow+0], x[ilow+1], x[ilow+2], x[ilow+3]);
                    break;
            }

            factor *= 10.;

        }

    }

    printf("\f summary of %d calls to hybrd1\n", ic);
    printf("\n nprob   n    nfev   njev  info  final l2 norm\n\n");
    for (i = 0; i < ic; ++i) {
        printf("%4d%6d%7d%7d%6d%16.7e\n",
               np[i], na[i], nf[i], nj[i], nx[i], fnm[i]);
    }
    exit(0);
}


int fcn(void *p, int n, const real *x, real *fvec, int iflag)
{
/*     ********** */

/*     the calling sequence of fcn should be identical to the */
/*     calling sequence of the function subroutine in the nonlinear */
/*     equation solver. fcn should only call the testing function */
/*     subroutine vecfcn with the appropriate value of problem */
/*     number (nprob). */

/*     subprograms called */

/*       minpack-supplied ... vecfcn */

/*     argonne national laboratory. minpack project. march 1980. */
/*     burton s. garbow, kenneth e. hillstrom, jorge j. more */

/*     ********** */
    struct refnum *hybrdtest = (struct refnum *)p;
    vecfcn(n, x, fvec, hybrdtest->nprob);
    if (iflag == 1) {
        hybrdtest->nfev++;
    }
    if (iflag == 2) {
        hybrdtest->njev++;
    }

    return 0;
} /* fcn_ */
