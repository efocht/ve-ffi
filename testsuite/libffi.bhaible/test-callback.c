/*
 * Copyright 1993 Bill Triggs <Bill.Triggs@inrialpes.fr>
 * Copyright 1995-2017 Bruno Haible <bruno@clisp.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* { dg-do run } */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ffi.h>
#include "alignof.h"

#include "testcases.c"

/* libffi testsuite local changes -------------------------------- */
#ifdef DGTEST
/* Redefine exit(1) as a test failure */
#define exit(V) (void)((V) ? (abort(), 1) : exit(0))
int count = 0;
char rbuf1[2048];
char rbuf2[2048];
#define FPRINTF(STREAM,FMT,ARGS...)			\
  { fprintf(STREAM, FMT, ##ARGS) ;			\
    switch (count++)					\
      {							\
        case 0:						\
        case 1:						\
	  sprintf(&rbuf1[strlen(rbuf1)], FMT, ##ARGS);	\
	  break ;					\
        case 2:						\
	  sprintf(rbuf2, FMT, ##ARGS);	                \
	  break;					\
        case 3:						\
	  sprintf(&rbuf2[strlen(rbuf2)], FMT, ##ARGS);	\
	  if (strcmp (rbuf1, rbuf2)) abort();		\
	  break;					\
      }							\
  }
#else
#define FPRINTF fprintf
#endif
/* --------------------------------------------------------------- */

/* Definitions that ought to be part of libffi. */
static ffi_type ffi_type_char;
#define ffi_type_slonglong ffi_type_sint64
#define ffi_type_ulonglong ffi_type_uint64

/* libffi does not support arrays inside structs. */
#define SKIP_EXTRA_STRUCTS

#define FFI_PREP_CIF(cif,argtypes,rettype) \
  if (ffi_prep_cif(&(cif),FFI_DEFAULT_ABI,sizeof(argtypes)/sizeof(argtypes[0]),&rettype,argtypes) != FFI_OK) abort()
#define FFI_PREP_CIF_VAR(cif,nfixedargs,argtypes,rettype) \
  if (ffi_prep_cif_var(&(cif),FFI_DEFAULT_ABI,nfixedargs,sizeof(argtypes)/sizeof(argtypes[0]),&rettype,argtypes) != FFI_OK) abort()
#define FFI_PREP_CIF_NOARGS(cif,rettype) \
  if (ffi_prep_cif(&(cif),FFI_DEFAULT_ABI,0,&rettype,NULL) != FFI_OK) abort()

#if defined(__sparc__) && defined(__sun) && defined(__SUNPRO_C) /* SUNWspro cc */
/* SunPRO cc miscompiles the simulator function for X_BcdB: d.i[1] is
 * temporarily stored in %l2 and put onto the stack from %l2, but in between
 * the copy of X has used %l2 as a counter without saving and restoring its
 * value.
 */
#define SKIP_X
#endif
#if defined(__mipsn32__) && !defined(__GNUC__)
/* The X test crashes for an unknown reason. */
#define SKIP_X
#endif


/* These functions simulate the behaviour of the functions defined in testcases.c. */

/* void tests */
void v_v_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&v_v) { fprintf(out,"wrong data for v_v\n"); exit(1); }
  FPRINTF(out,"void f(void):\n");
  fflush(out);
}

/* int tests */
void i_v_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&i_v) { fprintf(out,"wrong data for i_v\n"); exit(1); }
 {int r=99;
  FPRINTF(out,"int f(void):");
  fflush(out);
  *(ffi_arg*)retp = r;
}}
void i_i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&i_i) { fprintf(out,"wrong data for i_i\n"); exit(1); }
  int a = *(int*)(*args++);
  int r=a+1;
  FPRINTF(out,"int f(int):(%d)",a);
  fflush(out);
  *(ffi_arg*)retp = r;
}
void i_i2_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&i_i2) { fprintf(out,"wrong data for i_i2\n"); exit(1); }
 {int a = *(int*)(*args++);
  int b = *(int*)(*args++);
  int r=a+b;
  FPRINTF(out,"int f(2*int):(%d,%d)",a,b);
  fflush(out);
  *(ffi_arg*)retp = r;
}}
void i_i4_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&i_i4) { fprintf(out,"wrong data for i_i4\n"); exit(1); }
 {int a = *(int*)(*args++);
  int b = *(int*)(*args++);
  int c = *(int*)(*args++);
  int d = *(int*)(*args++);
  int r=a+b+c+d;
  FPRINTF(out,"int f(4*int):(%d,%d,%d,%d)",a,b,c,d);
  fflush(out);
  *(ffi_arg*)retp = r;
}}
void i_i8_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&i_i8) { fprintf(out,"wrong data for i_i8\n"); exit(1); }
 {int a = *(int*)(*args++);
  int b = *(int*)(*args++);
  int c = *(int*)(*args++);
  int d = *(int*)(*args++);
  int e = *(int*)(*args++);
  int f = *(int*)(*args++);
  int g = *(int*)(*args++);
  int h = *(int*)(*args++);
  int r=a+b+c+d+e+f+g+h;
  FPRINTF(out,"int f(8*int):(%d,%d,%d,%d,%d,%d,%d,%d)",a,b,c,d,e,f,g,h);
  fflush(out);
  *(ffi_arg*)retp = r;
}}
void i_i16_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&i_i16) { fprintf(out,"wrong data for i_i16\n"); exit(1); }
 {int a = *(int*)(*args++);
  int b = *(int*)(*args++);
  int c = *(int*)(*args++);
  int d = *(int*)(*args++);
  int e = *(int*)(*args++);
  int f = *(int*)(*args++);
  int g = *(int*)(*args++);
  int h = *(int*)(*args++);
  int i = *(int*)(*args++);
  int j = *(int*)(*args++);
  int k = *(int*)(*args++);
  int l = *(int*)(*args++);
  int m = *(int*)(*args++);
  int n = *(int*)(*args++);
  int o = *(int*)(*args++);
  int p = *(int*)(*args++);
  int r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  FPRINTF(out,"int f(16*int):(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
          a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p);
  fflush(out);
  *(ffi_arg*)retp = r;
}}

/* float tests */
void f_f_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f) { fprintf(out,"wrong data for f_f\n"); exit(1); }
 {float a = *(float*)(*args++);
  float r=a+1.0;
  FPRINTF(out,"float f(float):(%g)",a);
  fflush(out);
  *(float*)retp = r;
}}
void f_f2_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f2) { fprintf(out,"wrong data for f_f2\n"); exit(1); }
 {float a = *(float*)(*args++);
  float b = *(float*)(*args++);
  float r=a+b;
  FPRINTF(out,"float f(2*float):(%g,%g)",a,b);
  fflush(out);
  *(float*)retp = r;
}}
void f_f4_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f4) { fprintf(out,"wrong data for f_f4\n"); exit(1); }
 {float a = *(float*)(*args++);
  float b = *(float*)(*args++);
  float c = *(float*)(*args++);
  float d = *(float*)(*args++);
  float r=a+b+c+d;
  FPRINTF(out,"float f(4*float):(%g,%g,%g,%g)",a,b,c,d);
  fflush(out);
  *(float*)retp = r;
}}
void f_f8_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f8) { fprintf(out,"wrong data for f_f8\n"); exit(1); }
 {float a = *(float*)(*args++);
  float b = *(float*)(*args++);
  float c = *(float*)(*args++);
  float d = *(float*)(*args++);
  float e = *(float*)(*args++);
  float f = *(float*)(*args++);
  float g = *(float*)(*args++);
  float h = *(float*)(*args++);
  float r=a+b+c+d+e+f+g+h;
  FPRINTF(out,"float f(8*float):(%g,%g,%g,%g,%g,%g,%g,%g)",a,b,c,d,e,f,g,h);
  fflush(out);
  *(float*)retp = r;
}}
void f_f16_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f16) { fprintf(out,"wrong data for f_f16\n"); exit(1); }
 {float a = *(float*)(*args++);
  float b = *(float*)(*args++);
  float c = *(float*)(*args++);
  float d = *(float*)(*args++);
  float e = *(float*)(*args++);
  float f = *(float*)(*args++);
  float g = *(float*)(*args++);
  float h = *(float*)(*args++);
  float i = *(float*)(*args++);
  float j = *(float*)(*args++);
  float k = *(float*)(*args++);
  float l = *(float*)(*args++);
  float m = *(float*)(*args++);
  float n = *(float*)(*args++);
  float o = *(float*)(*args++);
  float p = *(float*)(*args++);
  float r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  FPRINTF(out,"float f(16*float):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g)",a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p);
  fflush(out);
  *(float*)retp = r;
}}
void f_f24_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f24) { fprintf(out,"wrong data for f_f24\n"); exit(1); }
 {float a = *(float*)(*args++);
  float b = *(float*)(*args++);
  float c = *(float*)(*args++);
  float d = *(float*)(*args++);
  float e = *(float*)(*args++);
  float f = *(float*)(*args++);
  float g = *(float*)(*args++);
  float h = *(float*)(*args++);
  float i = *(float*)(*args++);
  float j = *(float*)(*args++);
  float k = *(float*)(*args++);
  float l = *(float*)(*args++);
  float m = *(float*)(*args++);
  float n = *(float*)(*args++);
  float o = *(float*)(*args++);
  float p = *(float*)(*args++);
  float q = *(float*)(*args++);
  float s = *(float*)(*args++);
  float t = *(float*)(*args++);
  float u = *(float*)(*args++);
  float v = *(float*)(*args++);
  float w = *(float*)(*args++);
  float x = *(float*)(*args++);
  float y = *(float*)(*args++);
  float r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+s+t+u+v+w+x+y;
  FPRINTF(out,"float f(24*float):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g)",a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,s,t,u,v,w,x,y);
  fflush(out);
  *(float*)retp = r;
}}

/* double tests */
void d_d_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_d) { fprintf(out,"wrong data for d_d\n"); exit(1); }
 {double a = *(double*)(*args++);
  double r=a+1.0;
  FPRINTF(out,"double f(double):(%g)",a);
  fflush(out);
  *(double*)retp = r;
}}
void d_d2_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_d2) { fprintf(out,"wrong data for d_d2\n"); exit(1); }
 {double a = *(double*)(*args++);
  double b = *(double*)(*args++);
  double r=a+b;
  FPRINTF(out,"double f(2*double):(%g,%g)",a,b);
  fflush(out);
  *(double*)retp = r;
}}
void d_d4_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_d4) { fprintf(out,"wrong data for d_d4\n"); exit(1); }
 {double a = *(double*)(*args++);
  double b = *(double*)(*args++);
  double c = *(double*)(*args++);
  double d = *(double*)(*args++);
  double r=a+b+c+d;
  FPRINTF(out,"double f(4*double):(%g,%g,%g,%g)",a,b,c,d);
  fflush(out);
  *(double*)retp = r;
}}
void d_d8_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_d8) { fprintf(out,"wrong data for d_d8\n"); exit(1); }
 {double a = *(double*)(*args++);
  double b = *(double*)(*args++);
  double c = *(double*)(*args++);
  double d = *(double*)(*args++);
  double e = *(double*)(*args++);
  double f = *(double*)(*args++);
  double g = *(double*)(*args++);
  double h = *(double*)(*args++);
  double r=a+b+c+d+e+f+g+h;
  FPRINTF(out,"double f(8*double):(%g,%g,%g,%g,%g,%g,%g,%g)",a,b,c,d,e,f,g,h);
  fflush(out);
  *(double*)retp = r;
}}
void d_d16_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_d16) { fprintf(out,"wrong data for d_d16\n"); exit(1); }
 {double a = *(double*)(*args++);
  double b = *(double*)(*args++);
  double c = *(double*)(*args++);
  double d = *(double*)(*args++);
  double e = *(double*)(*args++);
  double f = *(double*)(*args++);
  double g = *(double*)(*args++);
  double h = *(double*)(*args++);
  double i = *(double*)(*args++);
  double j = *(double*)(*args++);
  double k = *(double*)(*args++);
  double l = *(double*)(*args++);
  double m = *(double*)(*args++);
  double n = *(double*)(*args++);
  double o = *(double*)(*args++);
  double p = *(double*)(*args++);
  double r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  FPRINTF(out,"double f(16*double):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g)",a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p);
  fflush(out);
  *(double*)retp = r;
}}

#if HAVE_LONG_DOUBLE
/* long double tests */
void ld_ld_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ld_ld) { fprintf(out,"wrong data for ld_ld\n"); exit(1); }
 {long double a = *(long double*)(*args++);
  long double r=a+1.0;
  FPRINTF(out,"long double f(long double):(%g)",(double)a);
  fflush(out);
  *(long double*)retp = r;
}}
void ld_ld2_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ld_ld2) { fprintf(out,"wrong data for ld_ld2\n"); exit(1); }
 {long double a = *(long double*)(*args++);
  long double b = *(long double*)(*args++);
  long double r=a+b;
  FPRINTF(out,"long double f(2*long double):(%g,%g)",(double)a,(double)b);
  fflush(out);
  *(long double*)retp = r;
}}
void ld_ld4_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ld_ld4) { fprintf(out,"wrong data for ld_ld4\n"); exit(1); }
 {long double a = *(long double*)(*args++);
  long double b = *(long double*)(*args++);
  long double c = *(long double*)(*args++);
  long double d = *(long double*)(*args++);
  long double r=a+b+c+d;
  FPRINTF(out,"long double f(4*long double):(%g,%g,%g,%g)",(double)a,(double)b,(double)c,(double)d);
  fflush(out);
  *(long double*)retp = r;
}}
void ld_ld8_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ld_ld8) { fprintf(out,"wrong data for ld_ld8\n"); exit(1); }
 {long double a = *(long double*)(*args++);
  long double b = *(long double*)(*args++);
  long double c = *(long double*)(*args++);
  long double d = *(long double*)(*args++);
  long double e = *(long double*)(*args++);
  long double f = *(long double*)(*args++);
  long double g = *(long double*)(*args++);
  long double h = *(long double*)(*args++);
  long double r=a+b+c+d+e+f+g+h;
  FPRINTF(out,"long double f(8*long double):(%g,%g,%g,%g,%g,%g,%g,%g)",(double)a,(double)b,(double)c,(double)d,(double)e,(double)f,(double)g,(double)h);
  fflush(out);
  *(long double*)retp = r;
}}
void ld_fldx5_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ld_fldx5) { fprintf(out,"wrong data for ld_fldx5\n"); exit(1); }
 {float a = *(float*)(*args++);
  long double b = *(long double*)(*args++);
  float c = *(float*)(*args++);
  long double d = *(long double*)(*args++);
  float e = *(float*)(*args++);
  long double f = *(long double*)(*args++);
  float g = *(float*)(*args++);
  long double h = *(long double*)(*args++);
  float i = *(float*)(*args++);
  long double j = *(long double*)(*args++);
  long double r=a+b+c+d+e+f+g+h+i+j;
  FPRINTF(out,"long double f(5*(float,long double)):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g)",(double)a,(double)b,(double)c,(double)d,(double)e,(double)f,(double)g,(double)h,(double)i,(double)j);
  fflush(out);
  *(long double*)retp = r;
}}
void ld_ld16_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ld_ld16) { fprintf(out,"wrong data for ld_ld16\n"); exit(1); }
 {long double a = *(long double*)(*args++);
  long double b = *(long double*)(*args++);
  long double c = *(long double*)(*args++);
  long double d = *(long double*)(*args++);
  long double e = *(long double*)(*args++);
  long double f = *(long double*)(*args++);
  long double g = *(long double*)(*args++);
  long double h = *(long double*)(*args++);
  long double i = *(long double*)(*args++);
  long double j = *(long double*)(*args++);
  long double k = *(long double*)(*args++);
  long double l = *(long double*)(*args++);
  long double m = *(long double*)(*args++);
  long double n = *(long double*)(*args++);
  long double o = *(long double*)(*args++);
  long double p = *(long double*)(*args++);
  long double r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  FPRINTF(out,"long double f(16*long double):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g)",(double)a,(double)b,(double)c,(double)d,(double)e,(double)f,(double)g,(double)h,(double)i,(double)j,(double)k,(double)l,(double)m,(double)n,(double)o,(double)p);
  fflush(out);
  *(long double*)retp = r;
}}
#endif // HAVE_LONG_DOUBLE
#if HAVE_COMPLEX
#if 1
/* float complex tests */
void fq_fq_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&fq_fq) { fprintf(out,"wrong data for fq_fq\n"); exit(1); }
 {float complex a = *(float complex*)(*args++);
  float complex r=a+1.0;
  FPRINTF(out,"float complex f(float complex):(%g+%g*I)",(double)crealf(a),(double)cimagf(a));
  fflush(out);
  *(float complex*)retp = r;
}}
void fq_fq2_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&fq_fq2) { fprintf(out,"wrong data for fq_fq2\n"); exit(1); }
 {float complex a = *(float complex*)(*args++);
  float complex b = *(float complex*)(*args++);
  float complex r=a+b;
  FPRINTF(out,"float complex f(2*float complex):(%g+%g*I,%g+%g*I)",(double)crealf(a),(double)cimagf(a),(double)crealf(b),(double)cimagf(b));
  fflush(out);
  *(float complex*)retp = r;
}}
void fq_ffq_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&fq_ffq) { fprintf(out,"wrong data for fq_fq2\n"); exit(1); }
 {float a = *(float*)(*args++);
  float complex b = *(float complex*)(*args++);
  float complex r=a+b;
  FPRINTF(out,"float complex f(float, float complex):(%g,%g+%g*I)",(double)a,(double)crealf(b),(double)cimagf(b));
  fflush(out);
  *(float complex*)retp = r;
}}
void fq_fq4_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&fq_fq4) { fprintf(out,"wrong data for fq_fq4\n"); exit(1); }
 {float complex a = *(float complex*)(*args++);
  float complex b = *(float complex*)(*args++);
  float complex c = *(float complex*)(*args++);
  float complex d = *(float complex*)(*args++);
  float complex r=a+b+c+d;
  FPRINTF(out,"float complex f(4*float complex):(%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I)",(double)crealf(a),(double)cimagf(a),(double)crealf(b),(double)cimagf(b),(double)crealf(c),(double)cimagf(c),(double)crealf(d),(double)cimagf(d));
  fflush(out);
  *(float complex*)retp = r;
}}
void fq_fq8_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&fq_fq8) { fprintf(out,"wrong data for fq_fq8\n"); exit(1); }
 {float complex a = *(float complex*)(*args++);
  float complex b = *(float complex*)(*args++);
  float complex c = *(float complex*)(*args++);
  float complex d = *(float complex*)(*args++);
  float complex e = *(float complex*)(*args++);
  float complex f = *(float complex*)(*args++);
  float complex g = *(float complex*)(*args++);
  float complex h = *(float complex*)(*args++);
  float complex r=a+b+c+d+e+f+g+h;
  FPRINTF(out,"float complex f(8*float complex):(%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I)",(double)crealf(a),(double)cimagf(a),(double)crealf(b),(double)cimagf(b),(double)crealf(c),(double)cimagf(c),(double)crealf(d),(double)cimagf(d),(double)crealf(e),(double)cimagf(e),(double)crealf(f),(double)cimagf(f),(double)crealf(g),(double)cimagf(g),(double)crealf(h),(double)cimagf(h));
  fflush(out);
  *(float complex*)retp = r;
}}
void fq_ffqx5_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&fq_ffqx5) { fprintf(out,"wrong data for fq_ffqx5\n"); exit(1); }
 {float a = *(float*)(*args++);
  float complex b = *(float complex*)(*args++);
  float c = *(float*)(*args++);
  float complex d = *(float complex*)(*args++);
  float e = *(float*)(*args++);
  float complex f = *(float complex*)(*args++);
  float g = *(float*)(*args++);
  float complex h = *(float complex*)(*args++);
  float i = *(float*)(*args++);
  float complex j = *(float complex*)(*args++);
  float complex r=a+b+c+d+e+f+g+h+i+j;
  FPRINTF(out,"float complex f(5*(float,float complex)):(%g,%g+%g*I,%g,%g+%g*I,%g,%g+%g*I,%g,%g+%g*I,%g,%g+%g*I)",(double)a,(double)crealf(b),(double)cimagf(b),(double)c,(double)crealf(d),(double)cimagf(d),(double)e,(double)crealf(f),(double)cimagf(f),(double)g,(double)crealf(h),(double)cimagf(h),(double)i,(double)crealf(j),(double)cimagf(j));
  fflush(out);
  *(float complex*)retp = r;
}}
void fq_fq16_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&fq_fq16) { fprintf(out,"wrong data for fq_fq16\n"); exit(1); }
 {float complex a = *(float complex*)(*args++);
  float complex b = *(float complex*)(*args++);
  float complex c = *(float complex*)(*args++);
  float complex d = *(float complex*)(*args++);
  float complex e = *(float complex*)(*args++);
  float complex f = *(float complex*)(*args++);
  float complex g = *(float complex*)(*args++);
  float complex h = *(float complex*)(*args++);
  float complex i = *(float complex*)(*args++);
  float complex j = *(float complex*)(*args++);
  float complex k = *(float complex*)(*args++);
  float complex l = *(float complex*)(*args++);
  float complex m = *(float complex*)(*args++);
  float complex n = *(float complex*)(*args++);
  float complex o = *(float complex*)(*args++);
  float complex p = *(float complex*)(*args++);
  float complex r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  FPRINTF(out,"float complex f(16*float complex):(%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I)",(double)crealf(a),(double)cimagf(a),(double)crealf(b),(double)cimagf(b),(double)crealf(c),(double)cimagf(c),(double)crealf(d),(double)cimagf(d),(double)crealf(e),(double)cimagf(e),(double)crealf(f),(double)cimagf(f),(double)crealf(g),(double)cimagf(g),(double)crealf(h),(double)cimagf(h),(double)crealf(i),(double)cimagf(i),(double)crealf(j),(double)cimagf(j),(double)crealf(k),(double)cimagf(k),(double)crealf(l),(double)cimagf(l),(double)crealf(m),(double)cimagf(m),(double)crealf(n),(double)cimagf(n),(double)crealf(o),(double)cimagf(o),(double)crealf(p),(double)cimagf(p));
  fflush(out);
  *(float complex*)retp = r;
}}
#endif
#endif // HAVE_COMPLEX

/* pointer tests */
void vp_vpdpcpsp_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&vp_vpdpcpsp) { fprintf(out,"wrong data for vp_vpdpcpsp\n"); exit(1); }
 {void* a = *(void* *)(*args++);
  double* b = *(double* *)(*args++);
  char* c = *(char* *)(*args++);
  Int* d = *(Int* *)(*args++);
  void* ret = (char*)b + 1;
  FPRINTF(out,"void* f(void*,double*,char*,Int*):(0x%p,0x%p,0x%p,0x%p)",a,b,c,d);
  fflush(out);
  *(void* *)retp = ret;
}}

/* mixed number tests */
void uc_ucsil_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&uc_ucsil) { fprintf(out,"wrong data for uc_ucsil\n"); exit(1); }
 {uchar a = *(unsigned char *)(*args++);
  ushort b = *(unsigned short *)(*args++);
  uint c = *(unsigned int *)(*args++);
  ulong d = *(unsigned long *)(*args++);
  uchar r = (uchar)-1;
  FPRINTF(out,"uchar f(uchar,ushort,uint,ulong):(%u,%u,%u,%lu)",a,b,c,d);
  fflush(out);
  *(ffi_arg *)retp = r;
}}
void d_iidd_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_iidd) { fprintf(out,"wrong data for d_iidd\n"); exit(1); }
 {int a = *(int*)(*args++);
  int b = *(int*)(*args++);
  double c = *(double*)(*args++);
  double d = *(double*)(*args++);
  double r=a+b+c+d;
  FPRINTF(out,"double f(int,int,double,double):(%d,%d,%g,%g)",a,b,c,d);
  fflush(out);
  *(double*)retp = r;
}}
void d_iiidi_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_iiidi) { fprintf(out,"wrong data for d_iiidi\n"); exit(1); }
 {int a = *(int*)(*args++);
  int b = *(int*)(*args++);
  int c = *(int*)(*args++);
  double d = *(double*)(*args++);
  int e = *(int*)(*args++);
  double r=a+b+c+d+e;
  FPRINTF(out,"double f(int,int,int,double,int):(%d,%d,%d,%g,%d)",a,b,c,d,e);
  fflush(out);
  *(double*)retp = r;
}}
void d_idid_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_idid) { fprintf(out,"wrong data for d_idid\n"); exit(1); }
 {int a = *(int*)(*args++);
  double b = *(double*)(*args++);
  int c = *(int*)(*args++);
  double d = *(double*)(*args++);
  double r=a+b+c+d;
  FPRINTF(out,"double f(int,double,int,double):(%d,%g,%d,%g)",a,b,c,d);
  fflush(out);
  *(double*)retp = r;
}}
void d_fdi_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_fdi) { fprintf(out,"wrong data for d_fdi\n"); exit(1); }
 {float a = *(float*)(*args++);
  double b = *(double*)(*args++);
  int c = *(int*)(*args++);
  double r=a+b+c;
  FPRINTF(out,"double f(float,double,int):(%g,%g,%d)",a,b,c);
  fflush(out);
  *(double*)retp = r;
}}
void us_cdcd_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&us_cdcd) { fprintf(out,"wrong data for us_cdcd\n"); exit(1); }
 {char a = *(char*)(*args++);
  double b = *(double*)(*args++);
  char c = *(char*)(*args++);
  double d = *(double*)(*args++);
  ushort r = (ushort)(a + b + c + d);
  FPRINTF(out,"ushort f(char,double,char,double):('%c',%g,'%c',%g)",a,b,c,d);
  fflush(out);
  *(ffi_arg *)retp = r;
}}
void ll_iiilli_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ll_iiilli) { fprintf(out,"wrong data for ll_iiilli\n"); exit(1); }
 {int a = *(int*)(*args++);
  int b = *(int*)(*args++);
  int c = *(int*)(*args++);
  long long d = *(long long *)(*args++);
  int e = *(int*)(*args++);
  long long r = (long long)(int)a + (long long)(int)b + (long long)(int)c + d + (long long)e;
  FPRINTF(out,"long long f(int,int,int,long long,int):(%d,%d,%d,0x%lx%08lx,%d)",a,b,c,(long)(d>>32),(long)(d&0xffffffff),e);
  fflush(out);
  *(long long *)retp = r;
}}
void ll_flli_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ll_flli) { fprintf(out,"wrong data for ll_flli\n"); exit(1); }
 {float a = *(float*)(*args++);
  long long b = *(long long *)(*args++);
  int c = *(int*)(*args++);
  long long r = (long long)(int)a + b + (long long)c;
  FPRINTF(out,"long long f(float,long long,int):(%g,0x%lx%08lx,0x%lx)",a,(long)(b>>32),(long)(b&0xffffffff),(long)c);
  fflush(out);
  *(long long *)retp = r;
}}
void f_fi_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_fi) { fprintf(out,"wrong data for f_fi\n"); exit(1); }
 {float a = *(float*)(*args++);
  int z = *(int*)(*args++);
  float r=a+z;
  FPRINTF(out,"float f(float,int):(%g,%d)",a,z);
  fflush(out);
  *(float*)retp = r;
}}
void f_f2i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f2i) { fprintf(out,"wrong data for f_f2i\n"); exit(1); }
 {float a = *(float*)(*args++);
  float b = *(float*)(*args++);
  int z = *(int*)(*args++);
  float r=a+b+z;
  FPRINTF(out,"float f(2*float,int):(%g,%g,%d)",a,b,z);
  fflush(out);
  *(float*)retp = r;
}}
void f_f3i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f3i) { fprintf(out,"wrong data for f_f3i\n"); exit(1); }
 {float a = *(float*)(*args++);
  float b = *(float*)(*args++);
  float c = *(float*)(*args++);
  int z = *(int*)(*args++);
  float r=a+b+c+z;
  FPRINTF(out,"float f(3*float,int):(%g,%g,%g,%d)",a,b,c,z);
  fflush(out);
  *(float*)retp = r;
}}
void f_f4i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f4i) { fprintf(out,"wrong data for f_f4i\n"); exit(1); }
 {float a = *(float*)(*args++);
  float b = *(float*)(*args++);
  float c = *(float*)(*args++);
  float d = *(float*)(*args++);
  int z = *(int*)(*args++);
  float r=a+b+c+d+z;
  FPRINTF(out,"float f(4*float,int):(%g,%g,%g,%g,%d)",a,b,c,d,z);
  fflush(out);
  *(float*)retp = r;
}}
void f_f7i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f7i) { fprintf(out,"wrong data for f_f7i\n"); exit(1); }
 {float a = *(float*)(*args++);
  float b = *(float*)(*args++);
  float c = *(float*)(*args++);
  float d = *(float*)(*args++);
  float e = *(float*)(*args++);
  float f = *(float*)(*args++);
  float g = *(float*)(*args++);
  int z = *(int*)(*args++);
  float r=a+b+c+d+e+f+g+z;
  FPRINTF(out,"float f(7*float,int):(%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,z);
  fflush(out);
  *(float*)retp = r;
}}
void f_f8i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f8i) { fprintf(out,"wrong data for f_f8i\n"); exit(1); }
 {float a = *(float*)(*args++);
  float b = *(float*)(*args++);
  float c = *(float*)(*args++);
  float d = *(float*)(*args++);
  float e = *(float*)(*args++);
  float f = *(float*)(*args++);
  float g = *(float*)(*args++);
  float h = *(float*)(*args++);
  int z = *(int*)(*args++);
  float r=a+b+c+d+e+f+g+h+z;
  FPRINTF(out,"float f(8*float,int):(%g,%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,h,z);
  fflush(out);
  *(float*)retp = r;
}}
void f_f12i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f12i) { fprintf(out,"wrong data for f_f12i\n"); exit(1); }
 {float a = *(float*)(*args++);
  float b = *(float*)(*args++);
  float c = *(float*)(*args++);
  float d = *(float*)(*args++);
  float e = *(float*)(*args++);
  float f = *(float*)(*args++);
  float g = *(float*)(*args++);
  float h = *(float*)(*args++);
  float i = *(float*)(*args++);
  float j = *(float*)(*args++);
  float k = *(float*)(*args++);
  float l = *(float*)(*args++);
  int z = *(int*)(*args++);
  float r=a+b+c+d+e+f+g+h+i+j+k+l+z;
  FPRINTF(out,"float f(12*float,int):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,h,i,j,k,l,z);
  fflush(out);
  *(float*)retp = r;
}}
void f_f13i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f13i) { fprintf(out,"wrong data for f_f13i\n"); exit(1); }
 {float a = *(float*)(*args++);
  float b = *(float*)(*args++);
  float c = *(float*)(*args++);
  float d = *(float*)(*args++);
  float e = *(float*)(*args++);
  float f = *(float*)(*args++);
  float g = *(float*)(*args++);
  float h = *(float*)(*args++);
  float i = *(float*)(*args++);
  float j = *(float*)(*args++);
  float k = *(float*)(*args++);
  float l = *(float*)(*args++);
  float m = *(float*)(*args++);
  int z = *(int*)(*args++);
  float r=a+b+c+d+e+f+g+h+i+j+k+l+m+z;
  FPRINTF(out,"float f(13*float,int):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,h,i,j,k,l,m,z);
  fflush(out);
  *(float*)retp = r;
}}
void d_di_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_di) { fprintf(out,"wrong data for d_di\n"); exit(1); }
 {double a = *(double*)(*args++);
  int z = *(int*)(*args++);
  double r=a+z;
  FPRINTF(out,"double f(double,int):(%g,%d)",a,z);
  fflush(out);
  *(double*)retp = r;
}}
void d_d2i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_d2i) { fprintf(out,"wrong data for d_d2i\n"); exit(1); }
 {double a = *(double*)(*args++);
  double b = *(double*)(*args++);
  int z = *(int*)(*args++);
  double r=a+b+z;
  FPRINTF(out,"double f(2*double,int):(%g,%g,%d)",a,b,z);
  fflush(out);
  *(double*)retp = r;
}}
void d_d3i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_d3i) { fprintf(out,"wrong data for d_d3i\n"); exit(1); }
 {double a = *(double*)(*args++);
  double b = *(double*)(*args++);
  double c = *(double*)(*args++);
  int z = *(int*)(*args++);
  double r=a+b+c+z;
  FPRINTF(out,"double f(3*double,int):(%g,%g,%g,%d)",a,b,c,z);
  fflush(out);
  *(double*)retp = r;
}}
void d_d4i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_d4i) { fprintf(out,"wrong data for d_d4i\n"); exit(1); }
 {double a = *(double*)(*args++);
  double b = *(double*)(*args++);
  double c = *(double*)(*args++);
  double d = *(double*)(*args++);
  int z = *(int*)(*args++);
  double r=a+b+c+d+z;
  FPRINTF(out,"double f(4*double,int):(%g,%g,%g,%g,%d)",a,b,c,d,z);
  fflush(out);
  *(double*)retp = r;
}}
void d_d7i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_d7i) { fprintf(out,"wrong data for d_d7i\n"); exit(1); }
 {double a = *(double*)(*args++);
  double b = *(double*)(*args++);
  double c = *(double*)(*args++);
  double d = *(double*)(*args++);
  double e = *(double*)(*args++);
  double f = *(double*)(*args++);
  double g = *(double*)(*args++);
  int z = *(int*)(*args++);
  double r=a+b+c+d+e+f+g+z;
  FPRINTF(out,"double f(7*double,int):(%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,z);
  fflush(out);
  *(double*)retp = r;
}}
void d_d8i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_d8i) { fprintf(out,"wrong data for d_d8i\n"); exit(1); }
 {double a = *(double*)(*args++);
  double b = *(double*)(*args++);
  double c = *(double*)(*args++);
  double d = *(double*)(*args++);
  double e = *(double*)(*args++);
  double f = *(double*)(*args++);
  double g = *(double*)(*args++);
  double h = *(double*)(*args++);
  int z = *(int*)(*args++);
  double r=a+b+c+d+e+f+g+h+z;
  FPRINTF(out,"double f(8*double,int):(%g,%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,h,z);
  fflush(out);
  *(double*)retp = r;
}}
void d_d12i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_d12i) { fprintf(out,"wrong data for d_d12i\n"); exit(1); }
 {double a = *(double*)(*args++);
  double b = *(double*)(*args++);
  double c = *(double*)(*args++);
  double d = *(double*)(*args++);
  double e = *(double*)(*args++);
  double f = *(double*)(*args++);
  double g = *(double*)(*args++);
  double h = *(double*)(*args++);
  double i = *(double*)(*args++);
  double j = *(double*)(*args++);
  double k = *(double*)(*args++);
  double l = *(double*)(*args++);
  int z = *(int*)(*args++);
  double r=a+b+c+d+e+f+g+h+i+j+k+l+z;
  FPRINTF(out,"double f(12*double,int):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,h,i,j,k,l,z);
  fflush(out);
  *(double*)retp = r;
}}
void d_d13i_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_d13i) { fprintf(out,"wrong data for d_d13i\n"); exit(1); }
 {double a = *(double*)(*args++);
  double b = *(double*)(*args++);
  double c = *(double*)(*args++);
  double d = *(double*)(*args++);
  double e = *(double*)(*args++);
  double f = *(double*)(*args++);
  double g = *(double*)(*args++);
  double h = *(double*)(*args++);
  double i = *(double*)(*args++);
  double j = *(double*)(*args++);
  double k = *(double*)(*args++);
  double l = *(double*)(*args++);
  double m = *(double*)(*args++);
  int z = *(int*)(*args++);
  double r=a+b+c+d+e+f+g+h+i+j+k+l+m+z;
  FPRINTF(out,"double f(13*double,int):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,h,i,j,k,l,m,z);
  fflush(out);
  *(double*)retp = r;
}}

/* small structure return tests */
void S1_v_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&S1_v) { fprintf(out,"wrong data for S1_v\n"); exit(1); }
 {Size1 r = Size1_1;
  FPRINTF(out,"Size1 f(void):");
  fflush(out);
  *(Size1*)retp = r;
}}
void S2_v_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&S2_v) { fprintf(out,"wrong data for S2_v\n"); exit(1); }
 {Size2 r = Size2_1;
  FPRINTF(out,"Size2 f(void):");
  fflush(out);
  *(Size2*)retp = r;
}}
void S3_v_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&S3_v) { fprintf(out,"wrong data for S3_v\n"); exit(1); }
 {Size3 r = Size3_1;
  FPRINTF(out,"Size3 f(void):");
  fflush(out);
  *(Size3*)retp = r;
}}
void S4_v_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&S4_v) { fprintf(out,"wrong data for S4_v\n"); exit(1); }
 {Size4 r = Size4_1;
  FPRINTF(out,"Size4 f(void):");
  fflush(out);
  *(Size4*)retp = r;
}}
void S7_v_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&S7_v) { fprintf(out,"wrong data for S7_v\n"); exit(1); }
 {Size7 r = Size7_1;
  FPRINTF(out,"Size7 f(void):");
  fflush(out);
  *(Size7*)retp = r;
}}
void S8_v_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&S8_v) { fprintf(out,"wrong data for S8_v\n"); exit(1); }
 {Size8 r = Size8_1;
  FPRINTF(out,"Size8 f(void):");
  fflush(out);
  *(Size8*)retp = r;
}}
void S12_v_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&S12_v) { fprintf(out,"wrong data for S12_v\n"); exit(1); }
 {Size12 r = Size12_1;
  FPRINTF(out,"Size12 f(void):");
  fflush(out);
  *(Size12*)retp = r;
}}
void S15_v_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&S15_v) { fprintf(out,"wrong data for S15_v\n"); exit(1); }
 {Size15 r = Size15_1;
  FPRINTF(out,"Size15 f(void):");
  fflush(out);
  *(Size15*)retp = r;
}}
void S16_v_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&S16_v) { fprintf(out,"wrong data for S16_v\n"); exit(1); }
 {Size16 r = Size16_1;
  FPRINTF(out,"Size16 f(void):");
  fflush(out);
  *(Size16*)retp = r;
}}

/* structure tests */
void I_III_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&I_III) { fprintf(out,"wrong data for I_III\n"); exit(1); }
  if(0){
    FPRINTF(out," a@0x%lx ",(long unsigned)args[0]);
    FPRINTF(out," b@0x%lx ",(long unsigned)args[1]);
    FPRINTF(out," c@0x%lx ",(long unsigned)args[2]);
    FPRINTF(out," ((Int*)c)->x=%d ",((Int*)args[2])->x);
  }
 {Int a = *(Int*)(*args++);
  Int b = *(Int*)(*args++);
  Int c = *(Int*)(*args++);
  Int r;
  r.x = a.x + b.x + c.x;
  FPRINTF(out,"Int f(Int,Int,Int):({%d},{%d},{%d})",a.x,b.x,c.x);
  fflush(out);
  *(Int*)retp = r;
}}
void C_CdC_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&C_CdC) { fprintf(out,"wrong data for C_CdC\n"); exit(1); }
 {Char a = *(Char*)(*args++);
  double b = *(double*)(*args++);
  Char c = *(Char*)(*args++);
  Char r;
  r.x = (a.x + c.x)/2;
  FPRINTF(out,"Char f(Char,double,Char):({'%c'},%g,{'%c'})",a.x,b,c.x);
  fflush(out);
  *(Char*)retp = r;
}}
void F_Ffd_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&F_Ffd) { fprintf(out,"wrong data for F_Ffd\n"); exit(1); }
 {Float a = *(Float*)(*args++);
  float b = *(float*)(*args++);
  double c = *(double*)(*args++);
  Float r;
  r.x = a.x + b + c;
  FPRINTF(out,"Float f(Float,float,double):({%g},%g,%g)",a.x,b,c);
  fflush(out);
  *(Float*)retp = r;
}}
void D_fDd_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&D_fDd) { fprintf(out,"wrong data for D_fDd\n"); exit(1); }
 {float a = *(float*)(*args++);
  Double b = *(Double*)(*args++);
  double c = *(double*)(*args++);
  Double r;
  r.x = a + b.x + c;
  FPRINTF(out,"Double f(float,Double,double):(%g,{%g},%g)",a,b.x,c);
  fflush(out);
  *(Double*)retp = r;
}}
void D_Dfd_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&D_Dfd) { fprintf(out,"wrong data for D_Dfd\n"); exit(1); }
 {Double a = *(Double*)(*args++);
  float b = *(float*)(*args++);
  double c = *(double*)(*args++);
  Double r;
  r.x = a.x + b + c;
  FPRINTF(out,"Double f(Double,float,double):({%g},%g,%g)",a.x,b,c);
  fflush(out);
  *(Double*)retp = r;
}}
void J_JiJ_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&J_JiJ) { fprintf(out,"wrong data for J_JiJ\n"); exit(1); }
 {J a = *(J*)(*args++);
  int b= *(int*)(*args++);
  J c = *(J*)(*args++);
  J r;
  r.l1 = a.l1+c.l1; r.l2 = a.l2+b+c.l2;
  FPRINTF(out,"J f(J,int,J):({%ld,%ld},%d,{%ld,%ld})",a.l1,a.l2,b,c.l1,c.l2);
  fflush(out);
  *(J*)retp = r;
}}
#ifndef SKIP_EXTRA_STRUCTS
void T_TcT_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&T_TcT) { fprintf(out,"wrong data for T_TcT\n"); exit(1); }
 {T a = *(T*)(*args++);
  char b = *(char*)(*args++);
  T c = *(T*)(*args++);
  T r;
  r.c[0]='b'; r.c[1]=c.c[1]; r.c[2]=c.c[2];
  FPRINTF(out,"T f(T,char,T):({\"%c%c%c\"},'%c',{\"%c%c%c\"})",a.c[0],a.c[1],a.c[2],b,c.c[0],c.c[1],c.c[2]);
  fflush(out);
  *(T*)retp = r;
}}
void X_BcdB_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&X_BcdB) { fprintf(out,"wrong data for X_BcdB\n"); exit(1); }
 {B a = *(B*)(*args++);
  char b = *(char*)(*args++);
  double c = *(double*)(*args++);
  B d = *(B*)(*args++);
  static X xr={"return val",'R'};
  X r;
  r = xr;
  r.c1 = b;
  FPRINTF(out,"X f(B,char,double,B):({%g,{%d,%d,%d}},'%c',%g,{%g,{%d,%d,%d}})",
          a.d,a.i[0],a.i[1],a.i[2],b,c,d.d,d.i[0],d.i[1],d.i[2]);
  fflush(out);
  *(X*)retp = r;
}}
#endif

/* gpargs boundary tests */
void l_l0K_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&l_l0K) { fprintf(out,"wrong data for l_l0K\n"); exit(1); }
 {K b = *(K*)(*args++);
  long c = *(long*)(*args++);
  long r = b.l1 + b.l2 + b.l3 + b.l4 + c;
  FPRINTF(out,"long f(K,long):(%ld,%ld,%ld,%ld,%ld)",b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  *(ffi_arg*)retp = r;
}}
void l_l1K_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&l_l1K) { fprintf(out,"wrong data for l_l1K\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  K b = *(K*)(*args++);
  long c = *(long*)(*args++);
  long r = a1 + b.l1 + b.l2 + b.l3 + b.l4 + c;
  FPRINTF(out,"long f(long,K,long):(%ld,%ld,%ld,%ld,%ld,%ld)",a1,b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  *(ffi_arg*)retp = r;
}}
void l_l2K_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&l_l2K) { fprintf(out,"wrong data for l_l2K\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  K b = *(K*)(*args++);
  long c = *(long*)(*args++);
  long r = a1 + a2 + b.l1 + b.l2 + b.l3 + b.l4 + c;
  FPRINTF(out,"long f(2*long,K,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a1,a2,b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  *(ffi_arg*)retp = r;
}}
void l_l3K_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&l_l3K) { fprintf(out,"wrong data for l_l3K\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  K b = *(K*)(*args++);
  long c = *(long*)(*args++);
  long r = a1 + a2 + a3 + b.l1 + b.l2 + b.l3 + b.l4 + c;
  FPRINTF(out,"long f(3*long,K,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a1,a2,a3,b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  *(ffi_arg*)retp = r;
}}
void l_l4K_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&l_l4K) { fprintf(out,"wrong data for l_l4K\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  long a4 = *(long*)(*args++);
  K b = *(K*)(*args++);
  long c = *(long*)(*args++);
  long r = a1 + a2 + a3 + a4 + b.l1 + b.l2 + b.l3 + b.l4 + c;
  FPRINTF(out,"long f(4*long,K,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a1,a2,a3,a4,b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  *(ffi_arg*)retp = r;
}}
void l_l5K_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&l_l5K) { fprintf(out,"wrong data for l_l5K\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  long a4 = *(long*)(*args++);
  long a5 = *(long*)(*args++);
  K b = *(K*)(*args++);
  long c = *(long*)(*args++);
  long r = a1 + a2 + a3 + a4 + a5 + b.l1 + b.l2 + b.l3 + b.l4 + c;
  FPRINTF(out,"long f(5*long,K,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a1,a2,a3,a4,a5,b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  *(ffi_arg*)retp = r;
}}
void l_l6K_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&l_l6K) { fprintf(out,"wrong data for l_l6K\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  long a4 = *(long*)(*args++);
  long a5 = *(long*)(*args++);
  long a6 = *(long*)(*args++);
  K b = *(K*)(*args++);
  long c = *(long*)(*args++);
  long r = a1 + a2 + a3 + a4 + a5 + a6 + b.l1 + b.l2 + b.l3 + b.l4 + c;
  FPRINTF(out,"long f(6*long,K,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a1,a2,a3,a4,a5,a6,b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  *(ffi_arg*)retp = r;
}}
void f_f17l3L_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&f_f17l3L) { fprintf(out,"wrong data for f_f17l3L\n"); exit(1); }
 {float a = *(float*)(*args++);
  float b = *(float*)(*args++);
  float c = *(float*)(*args++);
  float d = *(float*)(*args++);
  float e = *(float*)(*args++);
  float f = *(float*)(*args++);
  float g = *(float*)(*args++);
  float h = *(float*)(*args++);
  float i = *(float*)(*args++);
  float j = *(float*)(*args++);
  float k = *(float*)(*args++);
  float l = *(float*)(*args++);
  float m = *(float*)(*args++);
  float n = *(float*)(*args++);
  float o = *(float*)(*args++);
  float p = *(float*)(*args++);
  float q = *(float*)(*args++);
  long s = *(long*)(*args++);
  long t = *(long*)(*args++);
  long u = *(long*)(*args++);
  L z = *(L*)(*args++);
  float r = a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+s+t+u+z.l1+z.l2+z.l3+z.l4+z.l5+z.l6;
  FPRINTF(out,"float f(17*float,3*int,L):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,s,t,u,z.l1,z.l2,z.l3,z.l4,z.l5,z.l6);
  fflush(out);
  *(float*)retp = r;
}}
void d_d17l3L_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_d17l3L) { fprintf(out,"wrong data for d_d17l3L\n"); exit(1); }
 {double a = *(double*)(*args++);
  double b = *(double*)(*args++);
  double c = *(double*)(*args++);
  double d = *(double*)(*args++);
  double e = *(double*)(*args++);
  double f = *(double*)(*args++);
  double g = *(double*)(*args++);
  double h = *(double*)(*args++);
  double i = *(double*)(*args++);
  double j = *(double*)(*args++);
  double k = *(double*)(*args++);
  double l = *(double*)(*args++);
  double m = *(double*)(*args++);
  double n = *(double*)(*args++);
  double o = *(double*)(*args++);
  double p = *(double*)(*args++);
  double q = *(double*)(*args++);
  long s = *(long*)(*args++);
  long t = *(long*)(*args++);
  long u = *(long*)(*args++);
  L z = *(L*)(*args++);
  double r = a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+s+t+u+z.l1+z.l2+z.l3+z.l4+z.l5+z.l6;
  FPRINTF(out,"double f(17*double,3*int,L):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,s,t,u,z.l1,z.l2,z.l3,z.l4,z.l5,z.l6);
  fflush(out);
  *(double*)retp = r;
}}
void ll_l2ll_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ll_l2ll) { fprintf(out,"wrong data for ll_l2ll\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long long b = *(long long *)(*args++);
  long c = *(long*)(*args++);
  long long r = (long long) (a1 + a2) + b + c;
  FPRINTF(out,"long long f(2*long,long long,long):(%ld,%ld,0x%lx%08lx,%ld)",a1,a2,(long)(b>>32),(long)(b&0xffffffff),c);
  fflush(out);
  *(long long *)retp = r;
}}
void ll_l3ll_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ll_l3ll) { fprintf(out,"wrong data for ll_l3ll\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  long long b = *(long long *)(*args++);
  long c = *(long*)(*args++);
  long long r = (long long) (a1 + a2 + a3) + b + c;
  FPRINTF(out,"long long f(3*long,long long,long):(%ld,%ld,%ld,0x%lx%08lx,%ld)",a1,a2,a3,(long)(b>>32),(long)(b&0xffffffff),c);
  fflush(out);
  *(long long *)retp = r;
}}
void ll_l4ll_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ll_l4ll) { fprintf(out,"wrong data for ll_l4ll\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  long a4 = *(long*)(*args++);
  long long b = *(long long *)(*args++);
  long c = *(long*)(*args++);
  long long r = (long long) (a1 + a2 + a3 + a4) + b + c;
  FPRINTF(out,"long long f(4*long,long long,long):(%ld,%ld,%ld,%ld,0x%lx%08lx,%ld)",a1,a2,a3,a4,(long)(b>>32),(long)(b&0xffffffff),c);
  fflush(out);
  *(long long *)retp = r;
}}
void ll_l5ll_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ll_l5ll) { fprintf(out,"wrong data for ll_l5ll\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  long a4 = *(long*)(*args++);
  long a5 = *(long*)(*args++);
  long long b = *(long long *)(*args++);
  long c = *(long*)(*args++);
  long long r = (long long) (a1 + a2 + a3 + a4 + a5) + b + c;
  FPRINTF(out,"long long f(5*long,long long,long):(%ld,%ld,%ld,%ld,%ld,0x%lx%08lx,%ld)",a1,a2,a3,a4,a5,(long)(b>>32),(long)(b&0xffffffff),c);
  fflush(out);
  *(long long *)retp = r;
}}
void ll_l6ll_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ll_l6ll) { fprintf(out,"wrong data for ll_l6ll\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  long a4 = *(long*)(*args++);
  long a5 = *(long*)(*args++);
  long a6 = *(long*)(*args++);
  long long b = *(long long *)(*args++);
  long c = *(long*)(*args++);
  long long r = (long long) (a1 + a2 + a3 + a4 + a5 + a6) + b + c;
  FPRINTF(out,"long long f(6*long,long long,long):(%ld,%ld,%ld,%ld,%ld,%ld,0x%lx%08lx,%ld)",a1,a2,a3,a4,a5,a6,(long)(b>>32),(long)(b&0xffffffff),c);
  fflush(out);
  *(long long *)retp = r;
}}
void ll_l7ll_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ll_l7ll) { fprintf(out,"wrong data for ll_l7ll\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  long a4 = *(long*)(*args++);
  long a5 = *(long*)(*args++);
  long a6 = *(long*)(*args++);
  long a7 = *(long*)(*args++);
  long long b = *(long long *)(*args++);
  long c = *(long*)(*args++);
  long long r = (long long) (a1 + a2 + a3 + a4 + a5 + a6 + a7) + b + c;
  FPRINTF(out,"long long f(7*long,long long,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld,0x%lx%08lx,%ld)",a1,a2,a3,a4,a5,a6,a7,(long)(b>>32),(long)(b&0xffffffff),c);
  fflush(out);
  *(long long *)retp = r;
}}
void d_l2d_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_l2d) { fprintf(out,"wrong data for d_l2d\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  double b = *(double*)(*args++);
  long c = *(long*)(*args++);
  double r = (double) (a1 + a2) + b + c;
  FPRINTF(out,"double f(2*long,double,long):(%ld,%ld,%g,%ld)",a1,a2,b,c);
  fflush(out);
  *(double*)retp = r;
}}
void d_l3d_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_l3d) { fprintf(out,"wrong data for d_l3d\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  double b = *(double*)(*args++);
  long c = *(long*)(*args++);
  double r = (double) (a1 + a2 + a3) + b + c;
  FPRINTF(out,"double f(3*long,double,long):(%ld,%ld,%ld,%g,%ld)",a1,a2,a3,b,c);
  fflush(out);
  *(double*)retp = r;
}}
void d_l4d_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_l4d) { fprintf(out,"wrong data for d_l4d\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  long a4 = *(long*)(*args++);
  double b = *(double*)(*args++);
  long c = *(long*)(*args++);
  double r = (double) (a1 + a2 + a3 + a4) + b + c;
  FPRINTF(out,"double f(4*long,double,long):(%ld,%ld,%ld,%ld,%g,%ld)",a1,a2,a3,a4,b,c);
  fflush(out);
  *(double*)retp = r;
}}
void d_l5d_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_l5d) { fprintf(out,"wrong data for d_l5d\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  long a4 = *(long*)(*args++);
  long a5 = *(long*)(*args++);
  double b = *(double*)(*args++);
  long c = *(long*)(*args++);
  double r = (double) (a1 + a2 + a3 + a4 + a5) + b + c;
  FPRINTF(out,"double f(5*long,double,long):(%ld,%ld,%ld,%ld,%ld,%g,%ld)",a1,a2,a3,a4,a5,b,c);
  fflush(out);
  *(double*)retp = r;
}}
void d_l6d_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_l6d) { fprintf(out,"wrong data for d_l6d\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  long a4 = *(long*)(*args++);
  long a5 = *(long*)(*args++);
  long a6 = *(long*)(*args++);
  double b = *(double*)(*args++);
  long c = *(long*)(*args++);
  double r = (double) (a1 + a2 + a3 + a4 + a5 + a6) + b + c;
  FPRINTF(out,"double f(6*long,double,long):(%ld,%ld,%ld,%ld,%ld,%ld,%g,%ld)",a1,a2,a3,a4,a5,a6,b,c);
  fflush(out);
  *(double*)retp = r;
}}
void d_l7d_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&d_l7d) { fprintf(out,"wrong data for d_l7d\n"); exit(1); }
 {long a1 = *(long*)(*args++);
  long a2 = *(long*)(*args++);
  long a3 = *(long*)(*args++);
  long a4 = *(long*)(*args++);
  long a5 = *(long*)(*args++);
  long a6 = *(long*)(*args++);
  long a7 = *(long*)(*args++);
  double b = *(double*)(*args++);
  long c = *(long*)(*args++);
  double r = (double) (a1 + a2 + a3 + a4 + a5 + a6 + a7) + b + c;
  FPRINTF(out,"double f(7*long,double,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld,%g,%ld)",a1,a2,a3,a4,a5,a6,a7,b,c);
  fflush(out);
  *(double*)retp = r;
}}
void i_cpcp_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&i_cpcp) { fprintf(out,"wrong data for i_cpcp\n"); exit(1); }
 {char* a = *(char* *)(*args++);
  char* b = *(char* *)(*args++);
  const int maxlen = 80;
  char result[maxlen];
  fprintf(out,"int f(char*,char*):('%s','%s')",a,b);
  fflush(out);
  result[0] = '\0';
  strncat(result, a, maxlen-1);
  strncat(result, b, maxlen-1-strlen(a));
  *(int*)retp = strlen(result);
}}
void ul_cp3_simulator (ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data)
{
  if (data != (void*)&ul_cp3) { fprintf(out,"wrong data for ul_cp3\n"); exit(1); }
 {char* a = *(char* *)(*args++);
  char* b = *(char* *)(*args++);
  char* c = *(char* *)(*args++);
  ulong ret=0UL; /* sum hashes of all three strings */
  int i;
  char *x;
  char buf[80];
  i = snprintf(&buf[0],80,"\"%s\",\"%s\",\"%s\"",a,b,c);
  if( i<0 || i>=80 ) goto oops;
  fprintf(out,"ulong ul_cp3(%s)",&buf[0]);
  for(x=a,i=0; *x!='\0'; ++x){
    if(++i > 20) goto oops; else ret += i * (ulong)*x;
  }
  for(x=b,i=0; *x!='\0'; ++x){
    if(++i > 20) goto oops; else ret += i * (ulong)*x;
  }
  for(x=c,i=0; *x!='\0'; ++x){
    if(++i > 20) goto oops; else ret += i * (ulong)*x;
  }
  goto ok;
oops: ret=13UL;
ok: *(ulong*)retp = ret;
}}
void d_divar_simulator(ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data){
  if (data != (void*)&d_divar) { fprintf(out,"wrong data for d_divar\n"); exit(1); }
 {double d1 = *(double*)(*args++);
  unsigned cnt = *(unsigned*)(*args++);
  fprintf(out,"double d_divar(double,unsigned,...):(%g,%u",d1,cnt); fflush(out);
  for(unsigned i=2U; i<cif->nargs; ++i){
    double d = *(double*)(*args++);
    fprintf(out,",%g",d); fflush(out);
    d1 += d;
  }
  fprintf(out,")"); fflush(out);
  *(double*)retp = d1;
}}
void i_cpivar_simulator(ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data){
  if (data != (void*)&i_cpivar) { fprintf(out,"wrong data for i_cpivar\n"); exit(1); }
 {char *buf = *(char**)(*args++);
  char *fmt = *(char**)(*args++);
  fprintf(out,"int i_cpivar(char*,char*,...):(%p,\"%s\",...)",(void*)buf,fmt); fflush(out);
  int r=0;
  /* Want a way to for libffi to "construct" a va_list (memory blob)
   * and pass it around.   RFE milestone for libffi 4.0 (TBD).
   * For now ASSUME that all varargs here are actually char* (### UGLY ###) */
  switch(cif->nargs){
    case(2): r=sprintf(buf,fmt); break;
    case(3): r=sprintf(buf,fmt,*(char**)args[0]); break;
    case(4): r=sprintf(buf,fmt,*(char**)args[0],*(char**)args[1]); break;
    case(5): r=sprintf(buf,fmt,*(char**)args[0],*(char**)args[1],*(char**)args[2]); break;
    case(6): r=sprintf(buf,fmt,*(char**)args[0],*(char**)args[1],*(char**)args[2],*(char**)args[3]); break;
    default: fprintf(out,"variadic closures TBD for libffi 4.0\n"); exit(1);
  }
  fprintf(out,":%s",buf);
  /* Could also think of using the normal FFI_CALL mechanism, but this has same issues */
  *(int*)retp = r;
}}
//double d_pdvar(double* pd,...)
void d_pdvar_simulator(ffi_cif* cif, void* retp, /*const*/ void* /*const*/ *args, void* data){
  if (data != (void*)&d_pdvar) { fprintf(out,"wrong data for d_pdvar\n"); exit(1); }
 {double r=0.0;
  double *pd = *(double**)(*args++);
  fprintf(out,"double d_pdvar(double*,...):("); fflush(out);
  if(pd==NULL){
    fprintf(out,"NULL"); fflush(out);
  }else{
    r = *pd;
    fprintf(out,"%g",*pd); fflush(out);
#pragma _NEC novector
    for(unsigned i=1U; i<999U; ++i){
      double* d = *(double**)(*args++); /*va_arg(args,double*);*/
      /*fprintf(out,",d@%p",(void*)d);*/
      if((void*)d == NULL){
        fprintf(out,",NULL"); fflush(out);
        break;
      }
      fprintf(out,",%g",*d); fflush(out);
      r += *d;
    }
  }
  fprintf(out,")"); fflush(out);
  *(double*)retp = r;
}}


/*
 * The way we run these tests - first call the function directly, then
 * through vacall() - there is the danger that arguments or results seem
 * to be passed correctly, but what we are seeing are in fact the vestiges
 * (traces) or the previous call. This may seriously fake the test.
 * Avoid this by clearing the registers between the first and the second call.
 */
long clear_traces_i (long a, long b, long c, long d, long e, long f, long g, long h,
                     long i, long j, long k, long l, long m, long n, long o, long p)
{ return 0; }
float clear_traces_f (float a, float b, float c, float d, float e, float f, float g,
                      float h, float i, float j, float k, float l, float m, float n,
                      float o, float p)
{ return 0.0; }
double clear_traces_d (double a, double b, double c, double d, double e, double f, double g,
                       double h, double i, double j, double k, double l, double m, double n,
                       double o, double p)
{ return 0.0; }
J clear_traces_J (void)
{ J j; j.l1 = j.l2 = 0; return j; }
void clear_traces (void)
{ clear_traces_i(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  clear_traces_f(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
  clear_traces_d(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
  clear_traces_J();
}

static void* callback_code;
static void* callback_writable;

#define ALLOC_CALLBACK() \
  callback_writable = ffi_closure_alloc(sizeof(ffi_closure),&callback_code); \
if (!callback_writable) abort()
#define PREP_CALLBACK(cif,simulator,data) \
  if (ffi_prep_closure_loc(callback_writable,&(cif),simulator,data,callback_code) != FFI_OK) abort()
#define FREE_CALLBACK() \
  ffi_closure_free(callback_writable)

void variadic_tests(FILE* out){
  /* variadic tests */
  double dr;
#if (!defined(DGTEST)) || DGTEST == 82
  dr = d_divar(d1,3, d2,d3,d4);
  FPRINTF(out,"->%g\n",dr); fflush(out);
  dr = 0; clear_traces(); ALLOC_CALLBACK();
  {
    ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_uint, &ffi_type_double, &ffi_type_double, &ffi_type_double };
    ffi_cif cif;
    FFI_PREP_CIF_VAR(cif,2,argtypes,ffi_type_double); /* 2 fixed args */
    PREP_CALLBACK(cif,d_divar_simulator,(void*)d_divar);
    dr = ((double (*) (double,unsigned,...))callback_code) (d1,3U,d2,d3,d4);
  }
  FPRINTF(out,"->%g\n",dr); fflush(out);
  /* */
  {
    void *null=NULL;
    dr = d_pdvar(&d1,&d2,&d3,&d4,null);
    FPRINTF(out,"->%g\n",dr); fflush(out);
    dr = 0; clear_traces(); ALLOC_CALLBACK();
    {
      /* libffi does not provide a way to "construct" a va_list (milestone for libffi 4.0) */
      ffi_type* argtypes[] = { &ffi_type_pointer, &ffi_type_pointer, &ffi_type_pointer, &ffi_type_pointer, &ffi_type_pointer };
      ffi_cif cif;
      FFI_PREP_CIF_VAR(cif,1,argtypes,ffi_type_double); /* 1 fixed arg */
      PREP_CALLBACK(cif,d_pdvar_simulator,(void*)d_pdvar);
      dr = ((double (*)(double* pd,...))callback_code) (&d1,&d2,&d3,&d4,null);
    }
    FPRINTF(out,"->%g\n",dr); fflush(out);
  }
  /* */
  {
    char *fmt="%s AND %s AND %s";
    char buf[120];
    int ir = i_cpivar(buf,fmt,str1,str2,str3);
    FPRINTF(out,"->%d\n",ir); fflush(out);
    ir = 0; clear_traces(); ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_pointer, &ffi_type_pointer, &ffi_type_pointer, &ffi_type_pointer, &ffi_type_pointer };
      ffi_cif cif;
      FFI_PREP_CIF_VAR(cif,2,argtypes,ffi_type_double); /* 2 fixed args */
      PREP_CALLBACK(cif,i_cpivar_simulator,(void*)i_cpivar);
      ir = ((int (*)(char *,char *,...))callback_code) (&buf[0],fmt,str1,str2,str3);
    }
    FPRINTF(out,"->%d\n",ir); fflush(out);
  }
#endif
}
int main (void)
{
  void* callback_code;
  void* callback_writable;
#define ALLOC_CALLBACK() \
  callback_writable = ffi_closure_alloc(sizeof(ffi_closure),&callback_code); \
  if (!callback_writable) abort()
#define PREP_CALLBACK(cif,simulator,data) \
  if (ffi_prep_closure_loc(callback_writable,&(cif),simulator,data,callback_code) != FFI_OK) abort()
#define FREE_CALLBACK() \
  ffi_closure_free(callback_writable)

  ffi_type_char = (char)(-1) < 0 ? ffi_type_schar : ffi_type_uchar;
  out = stdout;

#if (!defined(DGTEST)) || DGTEST == 1  
  /* void tests */
  v_v();
  clear_traces();
  ALLOC_CALLBACK();
  {
    ffi_cif cif;
    FFI_PREP_CIF_NOARGS(cif,ffi_type_void);
    PREP_CALLBACK(cif,v_v_simulator,(void*)&v_v);
    ((void (*) (void)) callback_code) ();
  }
  FREE_CALLBACK();
#endif

  /* int tests */
  { int ir;

#if (!defined(DGTEST)) || DGTEST == 2
    ir = i_v();
    fprintf(out,"->%d\n",ir);
    fflush(out);
    ir = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_cif cif;
      FFI_PREP_CIF_NOARGS(cif,ffi_type_sint);
      PREP_CALLBACK(cif,i_v_simulator,(void*)&i_v);
      ir = ((int (*) (void)) callback_code) ();
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%d\n",ir);
    fflush(out);
#endif    

#if (!defined(DGTEST)) || DGTEST == 3
    ir = i_i(i1);
    fprintf(out,"->%d\n",ir);
    fflush(out);
    ir = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_sint);
      PREP_CALLBACK(cif,i_i_simulator,(void*)&i_i);
      ir = ((int (*) (int)) callback_code) (i1);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%d\n",ir);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 4    
    ir = i_i2(i1,i2);
    fprintf(out,"->%d\n",ir);
    fflush(out);
    ir = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_sint, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_sint);
      PREP_CALLBACK(cif,i_i2_simulator,(void*)&i_i2);
      ir = ((int (*) (int,int)) callback_code) (i1,i2);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%d\n",ir);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 5    
    ir = i_i4(i1,i2,i3,i4);
    fprintf(out,"->%d\n",ir);
    fflush(out);
    ir = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_sint);
      PREP_CALLBACK(cif,i_i4_simulator,(void*)&i_i4);
      ir = ((int (*) (int,int,int,int)) callback_code) (i1,i2,i3,i4);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%d\n",ir);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 6    
    ir = i_i8(i1,i2,i3,i4,i5,i6,i7,i8);
    fprintf(out,"->%d\n",ir);
    fflush(out);
    ir = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_sint);
      PREP_CALLBACK(cif,i_i8_simulator,(void*)&i_i8);
      ir = ((int (*) (int,int,int,int,int,int,int,int)) callback_code) (i1,i2,i3,i4,i5,i6,i7,i8);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%d\n",ir);
    fflush(out);
#endif
  
#if (!defined(DGTEST)) || DGTEST == 7
    ir = i_i16(i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,i15,i16);
    fprintf(out,"->%d\n",ir);
    fflush(out);
    ir = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_sint);
      PREP_CALLBACK(cif,i_i16_simulator,(void*)&i_i16);
      ir = ((int (*) (int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int)) callback_code) (i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,i15,i16);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%d\n",ir);
    fflush(out);
#endif
  }

  /* float tests */
  { float fr;

#if (!defined(DGTEST)) || DGTEST == 8  
    fr = f_f(f1);
    fprintf(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_f_simulator,(void*)&f_f);
      fr = ((float (*) (float)) callback_code) (f1);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 9    
    fr = f_f2(f1,f2);
    fprintf(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_float };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_f2_simulator,(void*)&f_f2);
      fr = ((float (*) (float,float)) callback_code) (f1,f2);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 10
    fr = f_f4(f1,f2,f3,f4);
    fprintf(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_f4_simulator,(void*)&f_f4);
      fr = ((float (*) (float,float,float,float)) callback_code) (f1,f2,f3,f4);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 11    
    fr = f_f8(f1,f2,f3,f4,f5,f6,f7,f8);
    fprintf(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_f8_simulator,(void*)&f_f8);
      fr = ((float (*) (float,float,float,float,float,float,float,float)) callback_code) (f1,f2,f3,f4,f5,f6,f7,f8);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 12    
    fr = f_f16(f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16);
    fprintf(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_f16_simulator,(void*)&f_f16);
      fr = ((float (*) (float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float)) callback_code) (f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 13    
    fr = f_f24(f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16,f17,f18,f19,f20,f21,f22,f23,f24);
    fprintf(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_f24_simulator,(void*)&f_f24);
      fr = ((float (*) (float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float)) callback_code) (f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16,f17,f18,f19,f20,f21,f22,f23,f24);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

  }

  /* double tests */
  { double dr;

#if (!defined(DGTEST)) || DGTEST == 14
    dr = d_d(d1);
    fprintf(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_d_simulator,(void*)&d_d);
      dr = ((double (*) (double)) callback_code) (d1);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 15    
    dr = d_d2(d1,d2);
    fprintf(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_double };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_d2_simulator,(void*)&d_d2);
      dr = ((double (*) (double,double)) callback_code) (d1,d2);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif
  
#if (!defined(DGTEST)) || DGTEST == 16    
    dr = d_d4(d1,d2,d3,d4);
    fprintf(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_d4_simulator,(void*)&d_d4);
      dr = ((double (*) (double,double,double,double)) callback_code) (d1,d2,d3,d4);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 17    
    dr = d_d8(d1,d2,d3,d4,d5,d6,d7,d8);
    fprintf(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_d8_simulator,(void*)&d_d8);
      dr = ((double (*) (double,double,double,double,double,double,double,double)) callback_code) (d1,d2,d3,d4,d5,d6,d7,d8);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 18    
    dr = d_d16(d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13,d14,d15,d16);
    fprintf(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_d16_simulator,(void*)&d_d16);
      dr = ((double (*) (double,double,double,double,double,double,double,double,double,double,double,double,double,double,double,double)) callback_code) (d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13,d14,d15,d16);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif
  }

#if HAVE_LONG_DOUBLE
  /* long double tests */
  { long double ldr;

#if (!defined(DGTEST)) || DGTEST == 14
    ldr = ld_ld(ld1);
    fprintf(out,"->%g\n",(double)ldr);
    fflush(out);
    ldr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_longdouble };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_longdouble);
      PREP_CALLBACK(cif,ld_ld_simulator,(void*)&ld_ld);
      ldr = ((long double (*) (long double)) callback_code) (ld1);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",(double)ldr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 15    
    ldr = ld_ld2(ld1,ld2);
    fprintf(out,"->%g\n",(double)ldr);
    fflush(out);
    ldr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_longdouble, &ffi_type_longdouble };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_longdouble);
      PREP_CALLBACK(cif,ld_ld2_simulator,(void*)&ld_ld2);
      ldr = ((long double (*) (long double,long double)) callback_code) (ld1,ld2);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",(double)ldr);
    fflush(out);
#endif
  
#if (!defined(DGTEST)) || DGTEST == 16    
    ldr = ld_ld4(ld1,ld2,ld3,ld4);
    fprintf(out,"->%g\n",(double)ldr);
    fflush(out);
    ldr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_longdouble);
      PREP_CALLBACK(cif,ld_ld4_simulator,(void*)&ld_ld4);
      ldr = ((long double (*) (long double,long double,long double,long double)) callback_code) (ld1,ld2,ld3,ld4);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",(double)ldr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 17    
    ldr = ld_ld8(ld1,ld2,ld3,ld4,ld5,ld6,ld7,ld8);
    fprintf(out,"->%g\n",(double)ldr);
    fflush(out);
    ldr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_longdouble);
      PREP_CALLBACK(cif,ld_ld8_simulator,(void*)&ld_ld8);
      ldr = ((long double (*) (long double,long double,long double,long double,long double,long double,long double,long double)) callback_code) (ld1,ld2,ld3,ld4,ld5,ld6,ld7,ld8);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",(double)ldr);
    fflush(out);
    /* */
    ldr = ld_fldx5(f1,ld1,f2,ld2,f3,ld3,f4,ld4,f5,ld5);
    fprintf(out,"->%g\n",(double)ldr);
    fflush(out);
    ldr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = {
	      &ffi_type_float, &ffi_type_longdouble,
	      &ffi_type_float, &ffi_type_longdouble,
	      &ffi_type_float, &ffi_type_longdouble,
	      &ffi_type_float, &ffi_type_longdouble,
	      &ffi_type_float, &ffi_type_longdouble };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_longdouble);
      PREP_CALLBACK(cif,ld_fldx5_simulator,(void*)&ld_fldx5);
      ldr = ((long double (*) (float,long double, float,long double, float,long double, float,long double, float,long double)) callback_code) (f1,ld1,f2,ld2,f3,ld3,f4,ld4,f5,ld5);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",(double)ldr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 18
    ldr = ld_ld16(ld1,ld2,ld3,ld4,ld5,ld6,ld7,ld8,ld9,ld10,ld11,ld12,ld13,ld14,ld15,ld16);
    fprintf(out,"->%g\n",(double)ldr);
    fflush(out);
    ldr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble, &ffi_type_longdouble };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_longdouble);
      PREP_CALLBACK(cif,ld_ld16_simulator,(void*)&ld_ld16);
      ldr = ((long double (*) (long double,long double,long double,long double,long double,long double,long double,long double,long double,long double,long double,long double,long double,long double,long double,long double)) callback_code) (ld1,ld2,ld3,ld4,ld5,ld6,ld7,ld8,ld9,ld10,ld11,ld12,ld13,ld14,ld15,ld16);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",(double)ldr);
    fflush(out);
#endif
  }
#endif // HAVE_LONG_DOUBLE

#if HAVE_COMPLEX
#if 1 /* float complex */
  /* float complex tests */
  { float complex fqr;

#if (!defined(DGTEST)) || DGTEST == 14
    fqr = fq_fq(fq1);
    fprintf(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
    fqr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_complex_float };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_complex_float);
      PREP_CALLBACK(cif,fq_fq_simulator,(void*)&fq_fq);
      fqr = ((float complex (*) (float complex)) callback_code) (fq1);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 15    
    fqr = fq_fq2(fq1,fq2);
    fprintf(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
    fqr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_complex_float, &ffi_type_complex_float };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_complex_float);
      PREP_CALLBACK(cif,fq_fq2_simulator,(void*)&fq_fq2);
      fqr = ((float complex (*) (float complex,float complex)) callback_code) (fq1,fq2);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
    /* */
    fqr = fq_ffq(f1,fq2);
    fprintf(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
    fqr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_complex_float };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_complex_float);
      PREP_CALLBACK(cif,fq_ffq_simulator,(void*)&fq_ffq);
      fqr = ((float complex (*) (float,float complex)) callback_code) (f1,fq2);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
#endif
  
#if (!defined(DGTEST)) || DGTEST == 16    
    fqr = fq_fq4(fq1,fq2,fq3,fq4);
    fprintf(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
    fqr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_complex_float);
      PREP_CALLBACK(cif,fq_fq4_simulator,(void*)&fq_fq4);
      fqr = ((float complex (*) (float complex,float complex,float complex,float complex)) callback_code) (fq1,fq2,fq3,fq4);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 17    
    fqr = fq_fq8(fq1,fq2,fq3,fq4,fq5,fq6,fq7,fq8);
    fprintf(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
    fqr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_complex_float);
      PREP_CALLBACK(cif,fq_fq8_simulator,(void*)&fq_fq8);
      fqr = ((float complex (*) (float complex,float complex,float complex,float complex,float complex,float complex,float complex,float complex)) callback_code) (fq1,fq2,fq3,fq4,fq5,fq6,fq7,fq8);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
    /* */
    fqr = fq_ffqx5(f1,fq1,f2,fq2,f3,fq3,f4,fq4,f5,fq5);
    fprintf(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
    fqr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = {
	      &ffi_type_float, &ffi_type_complex_float,
	      &ffi_type_float, &ffi_type_complex_float,
	      &ffi_type_float, &ffi_type_complex_float,
	      &ffi_type_float, &ffi_type_complex_float,
	      &ffi_type_float, &ffi_type_complex_float };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_complex_float);
      PREP_CALLBACK(cif,fq_ffqx5_simulator,(void*)&fq_ffqx5);
      fqr = ((float complex (*) (float,float complex, float,float complex, float,float complex, float,float complex, float,float complex)) callback_code) (f1,fq1,f2,fq2,f3,fq3,f4,fq4,f5,fq5);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 18
    fqr = fq_fq16(fq1,fq2,fq3,fq4,fq5,fq6,fq7,fq8,fq9,fq10,fq11,fq12,fq13,fq14,fq15,fq16);
    fprintf(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
    fqr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float, &ffi_type_complex_float };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_complex_float);
      PREP_CALLBACK(cif,fq_fq16_simulator,(void*)&fq_fq16);
      fqr = ((float complex (*) (float complex,float complex,float complex,float complex,float complex,float complex,float complex,float complex,float complex,float complex,float complex,float complex,float complex,float complex,float complex,float complex)) callback_code) (fq1,fq2,fq3,fq4,fq5,fq6,fq7,fq8,fq9,fq10,fq11,fq12,fq13,fq14,fq15,fq16);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g+%g*I\n",(double)crealf(fqr),(double)cimagf(fqr));
    fflush(out);
#endif
  }
#endif /* float complex */
#endif // HAVE_COMPLEX

  /* pointer tests */
  { void* vpr;

#if (!defined(DGTEST)) || DGTEST == 19 
    vpr = vp_vpdpcpsp(&uc1,&d2,str3,&I4);
    fprintf(out,"->0x%p\n",vpr);
    fflush(out);
    vpr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_pointer, &ffi_type_pointer, &ffi_type_pointer, &ffi_type_pointer };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_pointer);
      PREP_CALLBACK(cif,vp_vpdpcpsp_simulator,(void*)&vp_vpdpcpsp);
      vpr = ((void* (*) (void*,double*,char*,Int*)) callback_code) (&uc1,&d2,str3,&I4);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->0x%p\n",vpr);
    fflush(out);
#endif
  }

  /* mixed number tests */
  { uchar ucr;
    ushort usr;
    float fr;
    double dr;
    long long llr;

#if (!defined(DGTEST)) || DGTEST == 20
    ucr = uc_ucsil(uc1,us2,ui3,ul4);
    fprintf(out,"->%u\n",ucr);
    fflush(out);
    ucr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_uchar, &ffi_type_ushort, &ffi_type_uint, &ffi_type_ulong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_uchar);
      PREP_CALLBACK(cif,uc_ucsil_simulator,(void*)&uc_ucsil);
      ucr = ((uchar (*) (uchar,ushort,uint,ulong)) callback_code) (uc1,us2,ui3,ul4);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%u\n",ucr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 21    
    dr = d_iidd(i1,i2,d3,d4);
    fprintf(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_sint, &ffi_type_sint, &ffi_type_double, &ffi_type_double };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_iidd_simulator,(void*)&d_iidd);
      dr = ((double (*) (int,int,double,double)) callback_code) (i1,i2,d3,d4);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 22    
    dr = d_iiidi(i1,i2,i3,d4,i5);
    fprintf(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_double, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_iiidi_simulator,(void*)&d_iiidi);
      dr = ((double (*) (int,int,int,double,int)) callback_code) (i1,i2,i3,d4,i5);
    }
    FREE_CALLBACK();
    fprintf(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 23    
    dr = d_idid(i1,d2,i3,d4);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_sint, &ffi_type_double, &ffi_type_sint, &ffi_type_double };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_idid_simulator,(void*)&d_idid);
      dr = ((double (*) (int,double,int,double)) callback_code) (i1,d2,i3,d4);
    }
    FREE_CALLBACK();
    fprintf(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 24    
    dr = d_fdi(f1,d2,i3);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_double, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_fdi_simulator,(void*)&d_fdi);
      dr = ((double (*) (float,double,int)) callback_code) (f1,d2,i3);
    }
    FREE_CALLBACK();
    fprintf(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 25    
    usr = us_cdcd(c1,d2,c3,d4);
    FPRINTF(out,"->%u\n",usr);
    fflush(out);
    usr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_char, &ffi_type_double, &ffi_type_char, &ffi_type_double };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_ushort);
      PREP_CALLBACK(cif,us_cdcd_simulator,(void*)&us_cdcd);
      usr = ((ushort (*) (char,double,char,double)) callback_code) (c1,d2,c3,d4);
    }
    FREE_CALLBACK();
    fprintf(out,"->%u\n",usr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 26    
    llr = ll_iiilli(i1,i2,i3,ll1,i13);
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
    llr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_sint, &ffi_type_sint, &ffi_type_sint, &ffi_type_slonglong, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slonglong);
      PREP_CALLBACK(cif,ll_iiilli_simulator,(void*)&ll_iiilli);
      llr = ((long long (*) (int,int,int,long long,int)) callback_code) (i1,i2,i3,ll1,i13);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 27    
    llr = ll_flli(f13,ll1,i13);
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
    llr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_slonglong, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slonglong);
      PREP_CALLBACK(cif,ll_flli_simulator,(void*)&ll_flli);
      llr = ((long long (*) (float,long long,int)) callback_code) (f13,ll1,i13);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 28    
    fr = f_fi(f1,i9);
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_fi_simulator,(void*)&f_fi);
      fr = ((float (*) (float,int)) callback_code) (f1,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 29    
    fr = f_f2i(f1,f2,i9);
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_float, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_f2i_simulator,(void*)&f_f2i);
      fr = ((float (*) (float,float,int)) callback_code) (f1,f2,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 30    
    fr = f_f3i(f1,f2,f3,i9);
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_f3i_simulator,(void*)&f_f3i);
      fr = ((float (*) (float,float,float,int)) callback_code) (f1,f2,f3,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 31    
    fr = f_f4i(f1,f2,f3,f4,i9);
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_f4i_simulator,(void*)&f_f4i);
      fr = ((float (*) (float,float,float,float,int)) callback_code) (f1,f2,f3,f4,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 32    
    fr = f_f7i(f1,f2,f3,f4,f5,f6,f7,i9);
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_f7i_simulator,(void*)&f_f7i);
      fr = ((float (*) (float,float,float,float,float,float,float,int)) callback_code) (f1,f2,f3,f4,f5,f6,f7,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 33    
    fr = f_f8i(f1,f2,f3,f4,f5,f6,f7,f8,i9);
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_f8i_simulator,(void*)&f_f8i);
      fr = ((float (*) (float,float,float,float,float,float,float,float,int)) callback_code) (f1,f2,f3,f4,f5,f6,f7,f8,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 34    
    fr = f_f13i(f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,i9);
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_f13i_simulator,(void*)&f_f13i);
      fr = ((float (*) (float,float,float,float,float,float,float,float,float,float,float,float,float,int)) callback_code) (f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 35    
    dr = d_di(d1,i9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_di_simulator,(void*)&d_di);
      dr = ((double (*) (double,int)) callback_code) (d1,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 36    
    dr = d_d2i(d1,d2,i9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_double, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_d2i_simulator,(void*)&d_d2i);
      dr = ((double (*) (double,double,int)) callback_code) (d1,d2,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 37    
    dr = d_d3i(d1,d2,d3,i9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_d3i_simulator,(void*)&d_d3i);
      dr = ((double (*) (double,double,double,int)) callback_code) (d1,d2,d3,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 38    
    dr = d_d4i(d1,d2,d3,d4,i9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_d4i_simulator,(void*)&d_d4i);
      dr = ((double (*) (double,double,double,double,int)) callback_code) (d1,d2,d3,d4,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 39    
    dr = d_d7i(d1,d2,d3,d4,d5,d6,d7,i9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_d7i_simulator,(void*)&d_d7i);
      dr = ((double (*) (double,double,double,double,double,double,double,int)) callback_code) (d1,d2,d3,d4,d5,d6,d7,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 40    
    dr = d_d8i(d1,d2,d3,d4,d5,d6,d7,d8,i9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_d8i_simulator,(void*)&d_d8i);
      dr = ((double (*) (double,double,double,double,double,double,double,double,int)) callback_code) (d1,d2,d3,d4,d5,d6,d7,d8,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 41    
    dr = d_d12i(d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,i9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_d12i_simulator,(void*)&d_d12i);
      dr = ((double (*) (double,double,double,double,double,double,double,double,double,double,double,double,int)) callback_code) (d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 42    
    dr = d_d13i(d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13,i9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_sint };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_d13i_simulator,(void*)&d_d13i);
      dr = ((double (*) (double,double,double,double,double,double,double,double,double,double,double,double,double,int)) callback_code) (d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13,i9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif
  }

  /* small structure return tests */
#if (!defined(DGTEST)) || DGTEST == 43
  {
    Size1 r = S1_v();
    FPRINTF(out,"->{%c}\n",r.x1);
    fflush(out);
    memset(&r,0,sizeof(r)); clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Size1_elements[] = { &ffi_type_char, NULL };
      ffi_type ffi_type_Size1;
      ffi_type_Size1.type = FFI_TYPE_STRUCT;
      ffi_type_Size1.size = sizeof(Size1);
      ffi_type_Size1.alignment = alignof_slot(Size1);
      ffi_type_Size1.elements = ffi_type_Size1_elements;
      ffi_cif cif;
      FFI_PREP_CIF_NOARGS(cif,ffi_type_Size1);
      PREP_CALLBACK(cif,S1_v_simulator,(void*)&S1_v);
      r = ((Size1 (*) (void)) callback_code) ();
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%c}\n",r.x1);
    fflush(out);
  }
#endif

#if (!defined(DGTEST)) || DGTEST == 44
  {
    Size2 r = S2_v();
    FPRINTF(out,"->{%c%c}\n",r.x1,r.x2);
    fflush(out);
    memset(&r,0,sizeof(r)); clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Size2_elements[] = { &ffi_type_char, &ffi_type_char, NULL };
      ffi_type ffi_type_Size2;
      ffi_type_Size2.type = FFI_TYPE_STRUCT;
      ffi_type_Size2.size = sizeof(Size2);
      ffi_type_Size2.alignment = alignof_slot(Size2);
      ffi_type_Size2.elements = ffi_type_Size2_elements;
      ffi_cif cif;
      FFI_PREP_CIF_NOARGS(cif,ffi_type_Size2);
      PREP_CALLBACK(cif,S2_v_simulator,(void*)&S2_v);
      r = ((Size2 (*) (void)) callback_code) ();
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%c%c}\n",r.x1,r.x2);
    fflush(out);
  }
#endif    

#if (!defined(DGTEST)) || DGTEST == 45
  {
    Size3 r = S3_v();
    FPRINTF(out,"->{%c%c%c}\n",r.x1,r.x2,r.x3);
    fflush(out);
    memset(&r,0,sizeof(r)); clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Size3_elements[] = { &ffi_type_char, &ffi_type_char, &ffi_type_char, NULL };
      ffi_type ffi_type_Size3;
      ffi_type_Size3.type = FFI_TYPE_STRUCT;
      ffi_type_Size3.size = sizeof(Size3);
      ffi_type_Size3.alignment = alignof_slot(Size3);
      ffi_type_Size3.elements = ffi_type_Size3_elements;
      ffi_cif cif;
      FFI_PREP_CIF_NOARGS(cif,ffi_type_Size3);
      PREP_CALLBACK(cif,S3_v_simulator,(void*)&S3_v);
      r = ((Size3 (*) (void)) callback_code) ();
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%c%c%c}\n",r.x1,r.x2,r.x3);
    fflush(out);
  }
#endif    

#if (!defined(DGTEST)) || DGTEST == 46
  {
    Size4 r = S4_v();
    FPRINTF(out,"->{%c%c%c%c}\n",r.x1,r.x2,r.x3,r.x4);
    fflush(out);
    memset(&r,0,sizeof(r)); clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Size4_elements[] = { &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, NULL };
      ffi_type ffi_type_Size4;
      ffi_type_Size4.type = FFI_TYPE_STRUCT;
      ffi_type_Size4.size = sizeof(Size4);
      ffi_type_Size4.alignment = alignof_slot(Size4);
      ffi_type_Size4.elements = ffi_type_Size4_elements;
      ffi_cif cif;
      FFI_PREP_CIF_NOARGS(cif,ffi_type_Size4);
      PREP_CALLBACK(cif,S4_v_simulator,(void*)&S4_v);
      r = ((Size4 (*) (void)) callback_code) ();
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%c%c%c%c}\n",r.x1,r.x2,r.x3,r.x4);
    fflush(out);
  }
#endif

#if (!defined(DGTEST)) || DGTEST == 47  
  {
    Size7 r = S7_v();
    FPRINTF(out,"->{%c%c%c%c%c%c%c}\n",r.x1,r.x2,r.x3,r.x4,r.x5,r.x6,r.x7);
    fflush(out);
    memset(&r,0,sizeof(r)); clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Size7_elements[] = { &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, NULL };
      ffi_type ffi_type_Size7;
      ffi_type_Size7.type = FFI_TYPE_STRUCT;
      ffi_type_Size7.size = sizeof(Size7);
      ffi_type_Size7.alignment = alignof_slot(Size7);
      ffi_type_Size7.elements = ffi_type_Size7_elements;
      ffi_cif cif;
      FFI_PREP_CIF_NOARGS(cif,ffi_type_Size7);
      PREP_CALLBACK(cif,S7_v_simulator,(void*)&S7_v);
      r = ((Size7 (*) (void)) callback_code) ();
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%c%c%c%c%c%c%c}\n",r.x1,r.x2,r.x3,r.x4,r.x5,r.x6,r.x7);
    fflush(out);
  }
#endif

#if (!defined(DGTEST)) || DGTEST == 48  
  {
    Size8 r = S8_v();
    FPRINTF(out,"->{%c%c%c%c%c%c%c%c}\n",r.x1,r.x2,r.x3,r.x4,r.x5,r.x6,r.x7,r.x8);
    fflush(out);
    memset(&r,0,sizeof(r)); clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Size8_elements[] = { &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, NULL };
      ffi_type ffi_type_Size8;
      ffi_type_Size8.type = FFI_TYPE_STRUCT;
      ffi_type_Size8.size = sizeof(Size8);
      ffi_type_Size8.alignment = alignof_slot(Size8);
      ffi_type_Size8.elements = ffi_type_Size8_elements;
      ffi_cif cif;
      FFI_PREP_CIF_NOARGS(cif,ffi_type_Size8);
      PREP_CALLBACK(cif,S8_v_simulator,(void*)&S8_v);
      r = ((Size8 (*) (void)) callback_code) ();
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%c%c%c%c%c%c%c%c}\n",r.x1,r.x2,r.x3,r.x4,r.x5,r.x6,r.x7,r.x8);
    fflush(out);
  }
#endif

#if (!defined(DGTEST)) || DGTEST == 49
  {
    Size12 r = S12_v();
    FPRINTF(out,"->{%c%c%c%c%c%c%c%c%c%c%c%c}\n",r.x1,r.x2,r.x3,r.x4,r.x5,r.x6,r.x7,r.x8,r.x9,r.x10,r.x11,r.x12);
    fflush(out);
    memset(&r,0,sizeof(r)); clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Size12_elements[] = { &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, NULL };
      ffi_type ffi_type_Size12;
      ffi_type_Size12.type = FFI_TYPE_STRUCT;
      ffi_type_Size12.size = sizeof(Size12);
      ffi_type_Size12.alignment = alignof_slot(Size12);
      ffi_type_Size12.elements = ffi_type_Size12_elements;
      ffi_cif cif;
      FFI_PREP_CIF_NOARGS(cif,ffi_type_Size12);
      PREP_CALLBACK(cif,S12_v_simulator,(void*)&S12_v);
      r = ((Size12 (*) (void)) callback_code) ();
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%c%c%c%c%c%c%c%c%c%c%c%c}\n",r.x1,r.x2,r.x3,r.x4,r.x5,r.x6,r.x7,r.x8,r.x9,r.x10,r.x11,r.x12);
    fflush(out);
  }
#endif

#if (!defined(DGTEST)) || DGTEST == 50  
  {
    Size15 r = S15_v();
    FPRINTF(out,"->{%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c}\n",r.x1,r.x2,r.x3,r.x4,r.x5,r.x6,r.x7,r.x8,r.x9,r.x10,r.x11,r.x12,r.x13,r.x14,r.x15);
    fflush(out);
    memset(&r,0,sizeof(r)); clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Size15_elements[] = { &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, NULL };
      ffi_type ffi_type_Size15;
      ffi_type_Size15.type = FFI_TYPE_STRUCT;
      ffi_type_Size15.size = sizeof(Size15);
      ffi_type_Size15.alignment = alignof_slot(Size15);
      ffi_type_Size15.elements = ffi_type_Size15_elements;
      ffi_cif cif;
      FFI_PREP_CIF_NOARGS(cif,ffi_type_Size15);
      PREP_CALLBACK(cif,S15_v_simulator,(void*)&S15_v);
      r = ((Size15 (*) (void)) callback_code) ();
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c}\n",r.x1,r.x2,r.x3,r.x4,r.x5,r.x6,r.x7,r.x8,r.x9,r.x10,r.x11,r.x12,r.x13,r.x14,r.x15);
    fflush(out);
  }
#endif

#if (!defined(DGTEST)) || DGTEST == 51
  {
    Size16 r = S16_v();
    FPRINTF(out,"->{%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c}\n",r.x1,r.x2,r.x3,r.x4,r.x5,r.x6,r.x7,r.x8,r.x9,r.x10,r.x11,r.x12,r.x13,r.x14,r.x15,r.x16);
    fflush(out);
    memset(&r,0,sizeof(r)); clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Size16_elements[] = { &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, &ffi_type_char, NULL };
      ffi_type ffi_type_Size16;
      ffi_type_Size16.type = FFI_TYPE_STRUCT;
      ffi_type_Size16.size = sizeof(Size16);
      ffi_type_Size16.alignment = alignof_slot(Size16);
      ffi_type_Size16.elements = ffi_type_Size16_elements;
      ffi_cif cif;
      FFI_PREP_CIF_NOARGS(cif,ffi_type_Size16);
      PREP_CALLBACK(cif,S16_v_simulator,(void*)&S16_v);
      r = ((Size16 (*) (void)) callback_code) ();
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c}\n",r.x1,r.x2,r.x3,r.x4,r.x5,r.x6,r.x7,r.x8,r.x9,r.x10,r.x11,r.x12,r.x13,r.x14,r.x15,r.x16);
    fflush(out);
  }
#endif

  
  /* structure tests */
  { Int Ir;
    Char Cr;
    Float Fr;
    Double Dr;
    J Jr;
#ifndef SKIP_EXTRA_STRUCTS
    T Tr;
    X Xr;
#endif    

#if (!defined(DGTEST)) || DGTEST == 52
    Ir = I_III(I1,I2,I3);
    FPRINTF(out,"->{%d}\n",Ir.x);
    fflush(out);
    Ir.x = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Int_elements[] = { &ffi_type_sint, NULL };
      ffi_type ffi_type_Int;
      ffi_type_Int.type = FFI_TYPE_STRUCT;
      ffi_type_Int.size = sizeof(Int);
      ffi_type_Int.alignment = alignof_slot(Int);
      ffi_type_Int.elements = ffi_type_Int_elements;
      ffi_type* argtypes[] = { &ffi_type_Int, &ffi_type_Int, &ffi_type_Int };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_Int);
      PREP_CALLBACK(cif,I_III_simulator,(void*)&I_III);
      Ir = ((Int (*) (Int,Int,Int)) callback_code) (I1,I2,I3);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%d}\n",Ir.x);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 53
    Cr = C_CdC(C1,d2,C3);
    FPRINTF(out,"->{'%c'}\n",Cr.x);
    fflush(out);
    Cr.x = '\0'; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Char_elements[] = { &ffi_type_char, NULL };
      ffi_type ffi_type_Char;
      ffi_type_Char.type = FFI_TYPE_STRUCT;
      ffi_type_Char.size = sizeof(Char);
      ffi_type_Char.alignment = alignof_slot(Char);
      ffi_type_Char.elements = ffi_type_Char_elements;
      ffi_type* argtypes[] = { &ffi_type_Char, &ffi_type_double, &ffi_type_Char };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_Char);
      PREP_CALLBACK(cif,C_CdC_simulator,(void*)&C_CdC);
      Cr = ((Char (*) (Char,double,Char)) callback_code) (C1,d2,C3);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{'%c'}\n",Cr.x);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 54    
    Fr = F_Ffd(F1,f2,d3);
    FPRINTF(out,"->{%g}\n",Fr.x);
    fflush(out);
    Fr.x = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Float_elements[] = { &ffi_type_float, NULL };
      ffi_type ffi_type_Float;
      ffi_type_Float.type = FFI_TYPE_STRUCT;
      ffi_type_Float.size = sizeof(Float);
      ffi_type_Float.alignment = alignof_slot(Float);
      ffi_type_Float.elements = ffi_type_Float_elements;
      ffi_type* argtypes[] = { &ffi_type_Float, &ffi_type_float, &ffi_type_double };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_Float);
      PREP_CALLBACK(cif,F_Ffd_simulator,(void*)&F_Ffd);
      Fr = ((Float (*) (Float,float,double)) callback_code) (F1,f2,d3);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%g}\n",Fr.x);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 55    
    Dr = D_fDd(f1,D2,d3);
    FPRINTF(out,"->{%g}\n",Dr.x);
    fflush(out);
    Dr.x = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Double_elements[] = { &ffi_type_double, NULL };
      ffi_type ffi_type_Double;
      ffi_type_Double.type = FFI_TYPE_STRUCT;
      ffi_type_Double.size = sizeof(Double);
      ffi_type_Double.alignment = alignof_slot(Double);
      ffi_type_Double.elements = ffi_type_Double_elements;
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_Double, &ffi_type_double };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_Double);
      PREP_CALLBACK(cif,D_fDd_simulator,(void*)&D_fDd);
      Dr = ((Double (*) (float,Double,double)) callback_code) (f1,D2,d3);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%g}\n",Dr.x);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 56
    Dr = D_Dfd(D1,f2,d3);
    FPRINTF(out,"->{%g}\n",Dr.x);
    fflush(out);
    Dr.x = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_Double_elements[] = { &ffi_type_double, NULL };
      ffi_type ffi_type_Double;
      ffi_type_Double.type = FFI_TYPE_STRUCT;
      ffi_type_Double.size = sizeof(Double);
      ffi_type_Double.alignment = alignof_slot(Double);
      ffi_type_Double.elements = ffi_type_Double_elements;
      ffi_type* argtypes[] = { &ffi_type_Double, &ffi_type_float, &ffi_type_double };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_Double);
      PREP_CALLBACK(cif,D_Dfd_simulator,(void*)&D_Dfd);
      Dr = ((Double (*) (Double,float,double)) callback_code) (D1,f2,d3);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%g}\n",Dr.x);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 57
    Jr = J_JiJ(J1,i2,J2);
    FPRINTF(out,"->{%ld,%ld}\n",Jr.l1,Jr.l2);
    fflush(out);
    Jr.l1 = Jr.l2 = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_J_elements[] = { &ffi_type_slong, &ffi_type_slong, NULL };
      ffi_type ffi_type_J;
      ffi_type_J.type = FFI_TYPE_STRUCT;
      ffi_type_J.size = sizeof(J);
      ffi_type_J.alignment = alignof_slot(J);
      ffi_type_J.elements = ffi_type_J_elements;
      ffi_type* argtypes[] = { &ffi_type_J, &ffi_type_sint, &ffi_type_J };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_J);
      PREP_CALLBACK(cif,J_JiJ_simulator,(void*)&J_JiJ);
      Jr = ((J (*) (J,int,J)) callback_code) (J1,i2,J2);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{%ld,%ld}\n",Jr.l1,Jr.l2);
    fflush(out);
#endif

#ifndef SKIP_EXTRA_STRUCTS
#if (!defined(DGTEST)) || DGTEST == 58
    Tr = T_TcT(T1,' ',T2);
    FPRINTF(out,"->{\"%c%c%c\"}\n",Tr.c[0],Tr.c[1],Tr.c[2]);
    fflush(out);
    Tr.c[0] = Tr.c[1] = Tr.c[2] = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_T_elements[] = { /* ?? */ &ffi_type_char, &ffi_type_char, &ffi_type_char_, NULL };
      ffi_type ffi_type_T;
      ffi_type_T.type = FFI_TYPE_STRUCT;
      ffi_type_T.size = sizeof(T);
      ffi_type_T.alignment = alignof_slot(T);
      ffi_type_T.elements = ffi_type_T_elements;
      ffi_type* argtypes[] = { &ffi_type_T, &ffi_type_char, &ffi_type_T };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_T);
      PREP_CALLBACK(cif,T_TcT_simulator,(void*)&T_TcT);
      Tr = ((T (*) (T,char,T)) callback_code) (T1,' ',T2);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{\"%c%c%c\"}\n",Tr.c[0],Tr.c[1],Tr.c[2]);
    fflush(out);
#endif

#ifndef SKIP_X
#if (!defined(DGTEST)) || DGTEST == 59
    Xr = X_BcdB(B1,c2,d3,B2);
    FPRINTF(out,"->{\"%s\",'%c'}\n",Xr.c,Xr.c1);
    fflush(out);
    Xr.c[0]=Xr.c1='\0'; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* ffi_type_X_elements[] = { &ffi_type_pointer, /* ?? */ &ffi_type_char, NULL };
      ffi_type ffi_type_X;
      ffi_type_X.type = FFI_TYPE_STRUCT;
      ffi_type_X.size = sizeof(X);
      ffi_type_X.alignment = alignof_slot(X);
      ffi_type_X.elements = ffi_type_X_elements;
      ffi_type* argtypes[] = { &ffi_type_X, &ffi_type_char, &ffi_type_double, &ffi_type_X };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_X);
      PREP_CALLBACK(cif,X_BcdB_simulator,(void*)&X_BcdB);
      Xr = ((X (*) (B,char,double,B)) callback_code) (B1,c2,d3,B2);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->{\"%s\",'%c'}\n",Xr.c,Xr.c1);
    fflush(out);
#endif
#endif
#endif
  }

  
  /* gpargs boundary tests */
  {
    ffi_type* ffi_type_K_elements[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, NULL };
    ffi_type ffi_type_K;
    ffi_type* ffi_type_L_elements[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, NULL };
    ffi_type ffi_type_L;
    long lr;
    long long llr;
    float fr;
    double dr;

    ffi_type_K.type = FFI_TYPE_STRUCT;
    ffi_type_K.size = sizeof(K);
    ffi_type_K.alignment = alignof_slot(K);
    ffi_type_K.elements = ffi_type_K_elements;

    ffi_type_L.type = FFI_TYPE_STRUCT;
    ffi_type_L.size = sizeof(L);
    ffi_type_L.alignment = alignof_slot(L);
    ffi_type_L.elements = ffi_type_L_elements;

#if (!defined(DGTEST)) || DGTEST == 60
    lr = l_l0K(K1,l9);
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
    lr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_K, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slong);
      PREP_CALLBACK(cif,l_l0K_simulator,(void*)l_l0K);
      lr = ((long (*) (K,long)) callback_code) (K1,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 61
    lr = l_l1K(l1,K1,l9);
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
    lr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_K, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slong);
      PREP_CALLBACK(cif,l_l1K_simulator,(void*)l_l1K);
      lr = ((long (*) (long,K,long)) callback_code) (l1,K1,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 62
    lr = l_l2K(l1,l2,K1,l9);
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
    lr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_K, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slong);
      PREP_CALLBACK(cif,l_l2K_simulator,(void*)l_l2K);
      lr = ((long (*) (long,long,K,long)) callback_code) (l1,l2,K1,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 63
    lr = l_l3K(l1,l2,l3,K1,l9);
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
    lr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_K, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slong);
      PREP_CALLBACK(cif,l_l3K_simulator,(void*)l_l3K);
      lr = ((long (*) (long,long,long,K,long)) callback_code) (l1,l2,l3,K1,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 64
    lr = l_l4K(l1,l2,l3,l4,K1,l9);
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
    lr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_K, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slong);
      PREP_CALLBACK(cif,l_l4K_simulator,(void*)l_l4K);
      lr = ((long (*) (long,long,long,long,K,long)) callback_code) (l1,l2,l3,l4,K1,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 65  
    lr = l_l5K(l1,l2,l3,l4,l5,K1,l9);
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
    lr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_K, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slong);
      PREP_CALLBACK(cif,l_l5K_simulator,(void*)l_l5K);
      lr = ((long (*) (long,long,long,long,long,K,long)) callback_code) (l1,l2,l3,l4,l5,K1,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 66
    lr = l_l6K(l1,l2,l3,l4,l5,l6,K1,l9);
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
    lr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_K, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slong);
      PREP_CALLBACK(cif,l_l6K_simulator,(void*)l_l6K);
      lr = ((long (*) (long,long,long,long,long,long,K,long)) callback_code) (l1,l2,l3,l4,l5,l6,K1,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%ld\n",lr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 67    
    fr = f_f17l3L(f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16,f17,l6,l7,l8,L1);
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
    fr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_float, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_L };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_float);
      PREP_CALLBACK(cif,f_f17l3L_simulator,(void*)&f_f17l3L);
      fr = ((float (*) (float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,long,long,long,L)) callback_code) (f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16,f17,l6,l7,l8,L1);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",fr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 68    
    dr = d_d17l3L(d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13,d14,d15,d16,d17,l6,l7,l8,L1);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_double, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_L };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_d17l3L_simulator,(void*)&d_d17l3L);
      dr = ((double (*) (double,double,double,double,double,double,double,double,double,double,double,double,double,double,double,double,double,long,long,long,L)) callback_code) (d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13,d14,d15,d16,d17,l6,l7,l8,L1);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 69    
    llr = ll_l2ll(l1,l2,ll1,l9);
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
    llr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slonglong, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slonglong);
      PREP_CALLBACK(cif,ll_l2ll_simulator,(void*)ll_l2ll);
      llr = ((long long (*) (long,long,long long,long)) callback_code) (l1,l2,ll1,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 70
    llr = ll_l3ll(l1,l2,l3,ll1,l9);
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
    llr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slonglong, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slonglong);
      PREP_CALLBACK(cif,ll_l3ll_simulator,(void*)ll_l3ll);
      llr = ((long long (*) (long,long,long,long long,long)) callback_code) (l1,l2,l3,ll1,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 71    
    llr = ll_l4ll(l1,l2,l3,l4,ll1,l9);
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
    llr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slonglong, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slonglong);
      PREP_CALLBACK(cif,ll_l4ll_simulator,(void*)ll_l4ll);
      llr = ((long long (*) (long,long,long,long,long long,long)) callback_code) (l1,l2,l3,l4,ll1,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 72    
    llr = ll_l5ll(l1,l2,l3,l4,l5,ll1,l9);
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
    llr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slonglong, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slonglong);
      PREP_CALLBACK(cif,ll_l5ll_simulator,(void*)ll_l5ll);
      llr = ((long long (*) (long,long,long,long,long,long long,long)) callback_code) (l1,l2,l3,l4,l5,ll1,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 73    
    llr = ll_l6ll(l1,l2,l3,l4,l5,l6,ll1,l9);
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
    llr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slonglong, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slonglong);
      PREP_CALLBACK(cif,ll_l6ll_simulator,(void*)ll_l6ll);
      llr = ((long long (*) (long,long,long,long,long,long,long long,long)) callback_code) (l1,l2,l3,l4,l5,l6,ll1,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 74    
    llr = ll_l7ll(l1,l2,l3,l4,l5,l6,l7,ll1,l9);
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
    llr = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slonglong, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_slonglong);
      PREP_CALLBACK(cif,ll_l7ll_simulator,(void*)ll_l7ll);
      llr = ((long long (*) (long,long,long,long,long,long,long,long long,long)) callback_code) (l1,l2,l3,l4,l5,l6,l7,ll1,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->0x%lx%08lx\n",(long)(llr>>32),(long)(llr&0xffffffff));
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 75    
    dr = d_l2d(l1,l2,d2,l9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_double, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_l2d_simulator,(void*)d_l2d);
      dr = ((double (*) (long,long,double,long)) callback_code) (l1,l2,d2,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 76    
    dr = d_l3d(l1,l2,l3,d2,l9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_double, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_l3d_simulator,(void*)d_l3d);
      dr = ((double (*) (long,long,long,double,long)) callback_code) (l1,l2,l3,d2,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 77    
    dr = d_l4d(l1,l2,l3,l4,d2,l9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_double, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_l4d_simulator,(void*)d_l4d);
      dr = ((double (*) (long,long,long,long,double,long)) callback_code) (l1,l2,l3,l4,d2,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 78
    dr = d_l5d(l1,l2,l3,l4,l5,d2,l9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_double, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_l5d_simulator,(void*)d_l5d);
      dr = ((double (*) (long,long,long,long,long,double,long)) callback_code) (l1,l2,l3,l4,l5,d2,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 79
    dr = d_l6d(l1,l2,l3,l4,l5,l6,d2,l9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_double, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_l6d_simulator,(void*)d_l6d);
      dr = ((double (*) (long,long,long,long,long,long,double,long)) callback_code) (l1,l2,l3,l4,l5,l6,d2,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

#if (!defined(DGTEST)) || DGTEST == 80
    dr = d_l7d(l1,l2,l3,l4,l5,l6,l7,d2,l9);
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
    dr = 0.0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_slong, &ffi_type_double, &ffi_type_slong };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_double);
      PREP_CALLBACK(cif,d_l7d_simulator,(void*)d_l7d);
      dr = ((double (*) (long,long,long,long,long,long,long,double,long)) callback_code) (l1,l2,l3,l4,l5,l6,l7,d2,l9);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%g\n",dr);
    fflush(out);
#endif

  }

  /* string tests */
  {
    int ir;
    ulong ulr;
#if (!defined(DGTEST)) || DGTEST == 81
    /* standard libffi convention for FFI_TYPE_POINTER is to pass-by-address */
    ir = i_cpcp(str1, str2);
    FPRINTF(out,"->%d\n",ir);
    fflush(out);
    ir = 0; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_pointer, &ffi_type_pointer };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_sint);
      PREP_CALLBACK(cif,i_cpcp_simulator,(void*)i_cpcp);
      ir = ((int (*) (char*,char*)) callback_code) (str1,str2);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%d\n",ir);
    fflush(out);
    /* */
    ulr = ul_cp3(str1,str2,str3);
    FPRINTF(out,"->%lu\n",ulr);
    fflush(out);
    ulr = 0UL; clear_traces();
    ALLOC_CALLBACK();
    {
      ffi_type* argtypes[] = { &ffi_type_pointer, &ffi_type_pointer, &ffi_type_pointer };
      ffi_cif cif;
      FFI_PREP_CIF(cif,argtypes,ffi_type_pointer);
      PREP_CALLBACK(cif,ul_cp3_simulator,(void*)ul_cp3);
      ulr = ((ulong (*) (char*,char*,char*)) callback_code) (str1,str2,str3);
    }
    FREE_CALLBACK();
    FPRINTF(out,"->%lu\n",ulr);
    fflush(out);
#endif
  }

  variadic_tests(out);

  printf("test-callback: normal exit\n");
  printf("test-callback: normal exit\n"); /* twice so not flagged as failure */
  fflush(stdout);  
  exit(0);
}
/* vim: set sw=2 ts=2 et: */
