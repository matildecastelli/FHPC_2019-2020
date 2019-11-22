
/* ────────────────────────────────────────────────────────────────────────── *
 │                                                                            │
 │ This file is part of the exercises for the Lectures on                     │
 │   "Foundations of High Performance Computing"                              │
 │ given at                                                                   │
 │   Master in HPC and                                                        │
 │   Master in Data Science and Scientific Computing                          │
 │ @ SISSA, ICTP and University of Trieste                                    │
 │                                                                            │
 │ contact: luca.tornatore@inaf.it                                            │
 │                                                                            │
 │     This is free software; you can redistribute it and/or modify           │
 │     it under the terms of the GNU General Public License as published by   │
 │     the Free Software Foundation; either version 3 of the License, or      │
 │     (at your option) any later version.                                    │
 │     This code is distributed in the hope that it will be useful,           │
 │     but WITHOUT ANY WARRANTY; without even the implied warranty of         │
 │     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          │
 │     GNU General Public License for more details.                           │
 │                                                                            │
 │     You should have received a copy of the GNU General Public License      │
 │     along with this program.  If not, see <http://www.gnu.org/licenses/>   │
 │                                                                            │
 * ────────────────────────────────────────────────────────────────────────── */

#if defined(__STDC__)
#  if (__STDC_VERSION__ >= 199901L)
#     define _XOPEN_SOURCE 700
#  endif
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <omp.h>


#if !defined(USE_THREAD_BASED_TIMER)
#define CPU_TIME (clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts ), (double)ts.tv_sec +	\
		  (double)ts.tv_nsec * 1e-9)
#else
#define CPU_TIME (clock_gettime( CLOCK_THREAD_CPUTIME_ID, &ts ), (double)ts.tv_sec +	\
		  (double)ts.tv_nsec * 1e-9)
#endif


double heavy_work_0( int );
double heavy_work_1( int );
double heavy_work_2( int );

#if !defined( LARGE_WORK )
#define LARGE_WORK 20000000
#endif

int main( int argc, char **argv )
{
  struct timespec ts;
  double results[3] = {0};

  int check_addresses = 0;
  if ( argc > 1 )
    check_addresses = atoi( *(argv+1) );
  else
    printf("\nrun me with an argument != 0 if you want me to show that\n"
	   " the addresses of the internal variables in heavy_work_*\n"
	   " functions are all different for the different threads\n"
	   " that may be executing the same functions.\n"
	   "i.e., if you give me an argument != 0 I'll show the dynamic\n"
	   " extent of the parallel regions.\n\n");
  
  double tstart = CPU_TIME;
  
#ifdef _OPENMP                                   // ---------- parallel execution
#pragma omp parallel
  {
#pragma omp master    
    printf("running %d threads\n", omp_get_num_threads() );
    
    int myid = omp_get_thread_num();
    int arg  = ( check_addresses? myid : -1 );
    double result;
    
    if( myid % 3 == 0)
      result = heavy_work_0(arg);
    else if ( myid % 3 == 1 )
      result = heavy_work_1(arg);
    else if ( myid % 3 == 2 )
      result = heavy_work_2(arg);

    if ( myid < 3 )
      results[myid] = result;
  }
#else                                            // ---------- serial execution

  results[0] = heavy_work_0();
  results[1] = heavy_work_1();
  results[2] = heavy_work_2();
  
#endif                                           // ---------------------------
  
  double tend = CPU_TIME;
  
  printf("final result is (%g, %g, %g) %g\nachieved in %g seconds\n",
	 results[0], results[1], results[2],
	 results[0] + results[1] + results[2],
	 tend-tstart);
  
  
  return 0;
}


double heavy_work_0( int id )
{
  double guess = 3.141572 / 3;
  if ( id >= 0)
    printf("\t[thread %2d] my guess address in %s is %p\n", id, __func__, &guess);
  
  for( int i = 0; i < LARGE_WORK; i++ )
    {
      guess = exp( guess );
      guess = sin( guess );

    }

  return guess;
}

double heavy_work_1( int id )
{
  double guess = 3.141572 / 3;
  if ( id >= 0)
    printf("\t[thread %2d] my guess address in %s is %p\n", id, __func__, &guess);

  for( int i = 0; i < LARGE_WORK; i++ )
    {
      guess = log( guess );
      guess = exp( sqrt(guess)/guess );
    }

  return guess;
}

double heavy_work_2( int id )
{
  double guess = 3.141572 / 3;
  if ( id >= 0)
    printf("\t[thread %2d] my guess address in %s is %p\n", id, __func__, &guess);

  for( int i = 0; i < LARGE_WORK; i++ )
    {
      guess = sqrt( guess );
      guess = exp( 1+1.0/guess );
    }

  return guess;
}