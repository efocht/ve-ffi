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

/* This file defines test functions of selected signatures, that exercise
   dark corners of the various ABIs. */

#include <stdio.h>
#if defined(HAVE_COMPLEX)
#include <complex.h>
#endif
#include <string.h>

FILE* out;

#define uchar unsigned char
#define ushort unsigned short
#define uint unsigned int
#define ulong unsigned long

typedef struct { char x; } Char;
typedef struct { short x; } Short;
typedef struct { int x; } Int;
typedef struct { long x; } Long;
typedef struct { float x; } Float;
typedef struct { double x; } Double;
typedef struct { char c; float f; } A;
typedef struct { double d; int i[3]; } B;
typedef struct { long l1; long l2; } J;
typedef struct { long l1; long l2; long l3; long l4; } K;
typedef struct { long l1; long l2; long l3; long l4; long l5; long l6; } L;
typedef struct { char x1; } Size1;
typedef struct { char x1; char x2; } Size2;
typedef struct { char x1; char x2; char x3; } Size3;
typedef struct { char x1; char x2; char x3; char x4; } Size4;
typedef struct {
  char x1; char x2; char x3; char x4; char x5; char x6; char x7;
} Size7;
typedef struct {
  char x1; char x2; char x3; char x4; char x5; char x6; char x7; char x8;
} Size8;
typedef struct {
  char x1; char x2; char x3; char x4; char x5; char x6; char x7; char x8;
  char x9; char x10; char x11; char x12;
} Size12;
typedef struct {
  char x1; char x2; char x3; char x4; char x5; char x6; char x7; char x8;
  char x9; char x10; char x11; char x12; char x13; char x14; char x15;
} Size15;
typedef struct {
  char x1; char x2; char x3; char x4; char x5; char x6; char x7; char x8;
  char x9; char x10; char x11; char x12; char x13; char x14; char x15; char x16;
} Size16;
typedef struct { char c[3]; } T;
typedef struct { char c[33],c1; } X;

char c1='a', c2=127, c3=(char)128, c4=(char)255, c5=-1;
short s1=32767, s2=(short)32768, s3=3, s4=4, s5=5, s6=6, s7=7, s8=8, s9=9;
int i1=1, i2=2, i3=3, i4=4, i5=5, i6=6, i7=7, i8=8, i9=9,
    i10=11, i11=12, i12=13, i13=14, i14=15, i15=16, i16=17;
long l1=1, l2=2, l3=3, l4=4, l5=5, l6=6, l7=7, l8=8, l9=9;
long long ll1 = 3875056143130689530LL;
float f1=0.1, f2=0.2, f3=0.3, f4=0.4, f5=0.5, f6=0.6, f7=0.7, f8=0.8, f9=0.9,
      f10=1.1, f11=1.2, f12=1.3, f13=1.4, f14=1.5, f15=1.6, f16=1.7, f17=1.8,
      f18=1.9, f19=2.1, f20=2.2, f21=2.3, f22=2.4, f23=2.5, f24=2.6;
double d1=0.1, d2=0.2, d3=0.3, d4=0.4, d5=0.5, d6=0.6, d7=0.7, d8=0.8, d9=0.9,
       d10=1.1, d11=1.2, d12=1.3, d13=1.4, d14=1.5, d15=1.6, d16=1.7, d17=1.8;
#if defined HAVE_LONG_DOUBLE
long double ld1=0.1, ld2=0.2, ld3=0.3, ld4=0.4, ld5=0.5, ld6=0.6, ld7=0.7, ld8=0.8, ld9=0.9,
       ld10=1.1, ld11=1.2, ld12=1.3, ld13=1.4, ld14=1.5, ld15=1.6, ld16=1.7, ld17=1.8;
#endif
#if defined HAVE_COMPLEX
float complex fq1=0.1-0.1*I, fq2=0.2-0.2*I, fq3=0.3-0.3*I,
      fq4=0.4-0.4*I, fq5=0.5-0.5*I, fq6=0.6-0.6*I, fq7=0.7-0.7*I,
      fq8=0.8-0.8*I, fq9=0.9-0.9*I, fq10=1.1-1.1*I, fq11=1.2-1.2*I,
      fq12=1.3-1.3*I, fq13=1.4-1.4*I, fq14=1.5-1.5*I, fq15=1.6-1.6*I,
      fq16=1.7-1.7*I, fq17=1.8-1.8*I;
double complex dq1=0.1-0.1*I, dq2=0.2-0.2*I, dq3=0.3-0.3*I,
      dq4=0.4-0.4*I, dq5=0.5-0.5*I, dq6=0.6-0.6*I, dq7=0.7-0.7*I,
      dq8=0.8-0.8*I, dq9=0.9-0.9*I, dq10=1.1-1.1*I, dq11=1.2-1.2*I,
      dq12=1.3-1.3*I, dq13=1.4-1.4*I, dq14=1.5-1.5*I, dq15=1.6-1.6*I,
      dq16=1.7-1.7*I, dq17=1.8-1.8*I;
long double complex ldq1=0.1-0.1*I, ldq2=0.2-0.2*I, ldq3=0.3-0.3*I,
      ldq4=0.4-0.4*I, ldq5=0.5-0.5*I, ldq6=0.6-0.6*I, ldq7=0.7-0.7*I,
      ldq8=0.8-0.8*I, ldq9=0.9-0.9*I, ldq10=1.1-1.1*I, ldq11=1.2-1.2*I,
      ldq12=1.3-1.3*I, ldq13=1.4-1.4*I, ldq14=1.5-1.5*I, ldq15=1.6-1.6*I,
      ldq16=1.7-1.7*I, ldq17=1.8-1.8*I;
#endif

uchar uc1='a', uc2=127, uc3=128, uc4=255, uc5=(uchar)-1;
ushort us1=1, us2=2, us3=3, us4=4, us5=5, us6=6, us7=7, us8=8, us9=9;
uint ui1=1, ui2=2, ui3=3, ui4=4, ui5=5, ui6=6, ui7=7, ui8=8, ui9=9;
ulong ul1=1, ul2=2, ul3=3, ul4=4, ul5=5, ul6=6, ul7=7, ul8=8, ul9=9;

char *str1="hello",str2[]="goodbye",*str3="still here?";
Char C1={'A'}, C2={'B'}, C3={'C'}, C4={'\377'}, C5={(char)(-1)};
Short S1={1}, S2={2}, S3={3}, S4={4}, S5={5}, S6={6}, S7={7}, S8={8}, S9={9};
Int I1={1}, I2={2}, I3={3}, I4={4}, I5={5}, I6={6}, I7={7}, I8={8}, I9={9};
Float F1={0.1}, F2={0.2}, F3={0.3}, F4={0.4}, F5={0.5}, F6={0.6}, F7={0.7}, F8={0.8}, F9={0.9};
Double D1={0.1}, D2={0.2}, D3={0.3}, D4={0.4}, D5={0.5}, D6={0.6}, D7={0.7}, D8={0.8}, D9={0.9};

A A1={'a',0.1},A2={'b',0.2},A3={'\377',0.3};
B B1={0.1,{1,2,3}},B2={0.2,{5,4,3}};
J J1={47,11},J2={73,55};
K K1={19,69,12,28};
L L1={561,1105,1729,2465,2821,6601}; /* A002997 */
Size1 Size1_1={'a'};
Size2 Size2_1={'a','b'};
Size3 Size3_1={'a','b','c'};
Size4 Size4_1={'a','b','c','d'};
Size7 Size7_1={'a','b','c','d','e','f','g'};
Size8 Size8_1={'a','b','c','d','e','f','g','h'};
Size12 Size12_1={'a','b','c','d','e','f','g','h','i','j','k','l'};
Size15 Size15_1={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o'};
Size16 Size16_1={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'};
T T1={{'t','h','e'}},T2={{'f','o','x'}};
X X1={"abcdefghijklmnopqrstuvwxyzABCDEF",'G'}, X2={"123",'9'}, X3={"return-return-return",'R'};

/* void tests */
void v_v (void)
{
  fprintf(out,"void f(void):\n");
  fflush(out);
}

/* int tests */
int i_v (void)
{
  int r=99;
  fprintf(out,"int f(void):");
  fflush(out);
  return r;
}
int i_i (int a)
{
  int r=a+1;
  fprintf(out,"int f(int):(%d)",a);
  fflush(out);
  return r;
}
int i_i2 (int a, int b)
{
  int r=a+b;
  fprintf(out,"int f(2*int):(%d,%d)",a,b);
  fflush(out);
  return r;
}
int i_i4 (int a, int b, int c, int d)
{
  int r=a+b+c+d;
  fprintf(out,"int f(4*int):(%d,%d,%d,%d)",a,b,c,d);
  fflush(out);
  return r;
}
int i_i8 (int a, int b, int c, int d, int e, int f, int g, int h)
{
  int r=a+b+c+d+e+f+g+h;
  fprintf(out,"int f(8*int):(%d,%d,%d,%d,%d,%d,%d,%d)",a,b,c,d,e,f,g,h);
  fflush(out);
  return r;
}
int i_i16 (int a, int b, int c, int d, int e, int f, int g, int h,
           int i, int j, int k, int l, int m, int n, int o, int p)
{
  int r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  fprintf(out,"int f(16*int):(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
          a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p);
  fflush(out);
  return r;
}

/* float tests */
float f_f (float a)
{
  float r=a+1.0;
  fprintf(out,"float f(float):(%g)",a);
  fflush(out);
  return r;
}
float f_f2 (float a, float b)
{
  float r=a+b;
  fprintf(out,"float f(2*float):(%g,%g)",a,b);
  fflush(out);
  return r;
}
float f_f4 (float a, float b, float c, float d)
{
  float r=a+b+c+d;
  fprintf(out,"float f(4*float):(%g,%g,%g,%g)",a,b,c,d);
  fflush(out);
  return r;
}
float f_f8 (float a, float b, float c, float d, float e, float f,
            float g, float h)
{
  float r=a+b+c+d+e+f+g+h;
  fprintf(out,"float f(8*float):(%g,%g,%g,%g,%g,%g,%g,%g)",a,b,c,d,e,f,g,h);
  fflush(out);
  return r;
}
float f_f16 (float a, float b, float c, float d, float e, float f, float g, float h,
             float i, float j, float k, float l, float m, float n, float o, float p)
{
  float r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  fprintf(out,"float f(16*float):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g)",a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p);
  fflush(out);
  return r;
}
float f_f24 (float a, float b, float c, float d, float e, float f, float g, float h,
             float i, float j, float k, float l, float m, float n, float o, float p,
             float q, float s, float t, float u, float v, float w, float x, float y)
{
  float r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+s+t+u+v+w+x+y;
  fprintf(out,"float f(24*float):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g)",a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,s,t,u,v,w,x,y);
  fflush(out);
  return r;
}

/* double tests */
double d_d (double a)
{
  double r=a+1.0;
  fprintf(out,"double f(double):(%g)",a);
  fflush(out);
  return r;
}
double d_d2 (double a, double b)
{
  double r=a+b;
  fprintf(out,"double f(2*double):(%g,%g)",a,b);
  fflush(out);
  return r;
}
double d_d4 (double a, double b, double c, double d)
{
  double r=a+b+c+d;
  fprintf(out,"double f(4*double):(%g,%g,%g,%g)",a,b,c,d);
  fflush(out);
  return r;
}
double d_d8 (double a, double b, double c, double d, double e, double f,
             double g, double h)
{
  double r=a+b+c+d+e+f+g+h;
  fprintf(out,"double f(8*double):(%g,%g,%g,%g,%g,%g,%g,%g)",a,b,c,d,e,f,g,h);
  fflush(out);
  return r;
}
double d_d16 (double a, double b, double c, double d, double e, double f,
              double g, double h, double i, double j, double k, double l,
              double m, double n, double o, double p)
{
  double r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  fprintf(out,"double f(16*double):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g)",a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p);
  fflush(out);
  return r;
}

#if HAVE_LONG_DOUBLE
/* long double tests */
long double ld_ld (long double a)
{
  long double r=a+1.0;
  fprintf(out,"long double f(long double):(%g)",(double)a);
  fflush(out);
  return r;
}
long double ld_ld2 (long double a, long double b)
{
  long double r=a+b;
  fprintf(out,"long double f(2*long double):(%g,%g)",(double)a,(double)b);
  fflush(out);
  return r;
}
long double ld_ld4 (long double a, long double b, long double c, long double d)
{
  long double r=a+b+c+d;
  fprintf(out,"long double f(4*long double):(%g,%g,%g,%g)",(double)a,(double)b,(double)c,(double)d);
  fflush(out);
  return r;
}
long double ld_ld8 (long double a, long double b, long double c, long double d, long double e, long double f,
             long double g, long double h)
{
  long double r=a+b+c+d+e+f+g+h;
  fprintf(out,"long double f(8*long double):(%g,%g,%g,%g,%g,%g,%g,%g)",(double)a,(double)b,(double)c,(double)d,(double)e,(double)f,(double)g,(double)h);
  fflush(out);
  return r;
}
long double ld_ld16 (long double a, long double b, long double c, long double d, long double e, long double f,
              long double g, long double h, long double i, long double j, long double k, long double l,
              long double m, long double n, long double o, long double p)
{
  long double r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  fprintf(out,"long double f(16*long double):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g)",(double)a,(double)b,(double)c,(double)d,(double)e,(double)f,(double)g,(double)h,(double)i,(double)j,(double)k,(double)l,(double)m,(double)n,(double)o,(double)p);
  fflush(out);
  return r;
}
long double ld_fldx5 (float a, long double b,
		float c, long double d,
		float e, long double f,
		float g, long double h,
		float i, long double j)
{
  long double r=a+b+c+d+e+f+g+h+i+j;
  fprintf(out,"long double f(5*(float,long double)):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g)",(double)a,(double)b,(double)c,(double)d,(double)e,(double)f,(double)g,(double)h,(double)i,(double)j);
  fflush(out);
  return r;
}
#endif
#if HAVE_COMPLEX
#if 1 /* float complex tests */
float complex fq_fq (float complex a)
{
  float complex r=a+1.0;
  fprintf(out,"float complex f(float complex):(%g+%g*I)",
		  (double)crealf(a), (double)cimagf(a));
  fflush(out);
  return r;
}
float complex fq_fq2 (float complex a, float complex b)
{
  float complex r=a+b;
  fprintf(out,"float complex f(2*float complex):(%g+%g*I,%g+%g*I)",
		  (double)crealf(a), (double)cimagf(a),
		  (double)crealf(b), (double)cimagf(b));
  fflush(out);
  return r;
}
float complex fq_ffq (float a, float complex b)
{
  float complex r=a+b;
  fprintf(out,"float complex f(float, float complex):(%g,%g+%g*I)",
		  (double)a, (double)crealf(b), (double)cimagf(b));
  fflush(out);
  return r;
}
float complex fq_fq4 (float complex a, float complex b, float complex c, float complex d)
{
  float complex r=a+b+c+d;
  fprintf(out,"float complex f(4*float complex):(%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I)",
		  (double)crealf(a), (double)cimagf(a),
		  (double)crealf(b), (double)cimagf(b),
		  (double)crealf(c), (double)cimagf(c),
		  (double)crealf(d), (double)cimagf(d));
  fflush(out);
  return r;
}
float complex fq_fq8 (float complex a, float complex b, float complex c, float complex d, float complex e, float complex f,
             float complex g, float complex h)
{
  fprintf(out,"float complex f(8*float complex):(%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I)",
		  (double)crealf(a), (double)cimagf(a),
		  (double)crealf(b), (double)cimagf(b),
		  (double)crealf(c), (double)cimagf(c),
		  (double)crealf(d), (double)cimagf(d),
		  (double)crealf(e), (double)cimagf(e),
		  (double)crealf(f), (double)cimagf(f),
		  (double)crealf(g), (double)cimagf(g),
		  (double)crealf(h), (double)cimagf(h));
  fflush(out);
  float complex r=a+b+c+d+e+f+g+h;
  return r;
}
float complex fq_fq16 (float complex a, float complex b, float complex c, float complex d, float complex e, float complex f,
              float complex g, float complex h, float complex i, float complex j, float complex k, float complex l,
              float complex m, float complex n, float complex o, float complex p)
{
  float complex r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  fprintf(out,"float complex f(16*float complex):(%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I)",
		  (double)crealf(a), (double)cimagf(a),
		  (double)crealf(b), (double)cimagf(b),
		  (double)crealf(c), (double)cimagf(c),
		  (double)crealf(d), (double)cimagf(d),
		  (double)crealf(e), (double)cimagf(e),
		  (double)crealf(f), (double)cimagf(f),
		  (double)crealf(g), (double)cimagf(g),
		  (double)crealf(h), (double)cimagf(h),
		  (double)crealf(i), (double)cimagf(i),
		  (double)crealf(j), (double)cimagf(j),
		  (double)crealf(k), (double)cimagf(k),
		  (double)crealf(l), (double)cimagf(l),
		  (double)crealf(m), (double)cimagf(m),
		  (double)crealf(n), (double)cimagf(n),
		  (double)crealf(o), (double)cimagf(o),
		  (double)crealf(p), (double)cimagf(p));
  fflush(out);
  return r;
}
float complex fq_ffqx5 (float a, float complex b,
		float c, float complex d,
		float e, float complex f,
		float g, float complex h,
		float i, float complex j)
{
  float complex r=a+b+c+d+e+f+g+h+i+j;
  fprintf(out,"float complex f(5*(float,float complex)):(%g,%g+%g*I,%g,%g+%g*I,%g,%g+%g*I,%g,%g+%g*I,%g,%g+%g*I)",
		  (double)a,(double)crealf(b),(double)cimagf(b),
		  (double)c,(double)crealf(d),(double)cimagf(d),
		  (double)e,(double)crealf(f),(double)cimagf(f),
		  (double)g,(double)crealf(h),(double)cimagf(h),
		  (double)i,(double)crealf(j),(double)cimagf(j));
  fflush(out);
  return r;
}
#endif /* float complex */
#if 1 /* double complex tests */
double complex dq_dq (double complex a)
{
  double complex r=a+1.0;
  fprintf(out,"double complex f(double complex):(%g+%g*I)",
		  (double)creal(a), (double)cimag(a));
  fflush(out);
  return r;
}
double complex dq_dq2 (double complex a, double complex b)
{
  double complex r=a+b;
  fprintf(out,"double complex f(2*double complex):(%g+%g*I,%g+%g*I)",
		  (double)creal(a), (double)cimag(a),
		  (double)creal(b), (double)cimag(b));
  fflush(out);
  return r;
}
double complex dq_fdq (float a, double complex b)
{
  double complex r=a+b;
  fprintf(out,"double complex f(float, double complex):(%g,%g+%g*I)",
		  (double)a, (double)creal(b), (double)cimag(b));
  fflush(out);
  return r;
}
double complex dq_dq4 (double complex a, double complex b, double complex c, double complex d)
{
  double complex r=a+b+c+d;
  fprintf(out,"double complex f(4*double complex):(%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I)",
		  (double)creal(a), (double)cimag(a),
		  (double)creal(b), (double)cimag(b),
		  (double)creal(c), (double)cimag(c),
		  (double)creal(d), (double)cimag(d));
  fflush(out);
  return r;
}
double complex dq_dq8 (double complex a, double complex b, double complex c, double complex d, double complex e, double complex f,
             double complex g, double complex h)
{
  double complex r=a+b+c+d+e+f+g+h;
  fprintf(out,"double complex f(8*double complex):(%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I)",
		  (double)creal(a), (double)cimag(a),
		  (double)creal(b), (double)cimag(b),
		  (double)creal(c), (double)cimag(c),
		  (double)creal(d), (double)cimag(d),
		  (double)creal(e), (double)cimag(e),
		  (double)creal(f), (double)cimag(f),
		  (double)creal(g), (double)cimag(g),
		  (double)creal(h), (double)cimag(h));
  fflush(out);
  return r;
}
double complex dq_dq16 (double complex a, double complex b, double complex c, double complex d, double complex e, double complex f,
              double complex g, double complex h, double complex i, double complex j, double complex k, double complex l,
              double complex m, double complex n, double complex o, double complex p)
{
  double complex r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  fprintf(out,"double complex f(16*double complex):(%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I)",
		  (double)creal(a), (double)cimag(a),
		  (double)creal(b), (double)cimag(b),
		  (double)creal(c), (double)cimag(c),
		  (double)creal(d), (double)cimag(d),
		  (double)creal(e), (double)cimag(e),
		  (double)creal(f), (double)cimag(f),
		  (double)creal(g), (double)cimag(g),
		  (double)creal(h), (double)cimag(h),
		  (double)creal(i), (double)cimag(i),
		  (double)creal(j), (double)cimag(j),
		  (double)creal(k), (double)cimag(k),
		  (double)creal(l), (double)cimag(l),
		  (double)creal(m), (double)cimag(m),
		  (double)creal(n), (double)cimag(n),
		  (double)creal(o), (double)cimag(o),
		  (double)creal(p), (double)cimag(p));
  fflush(out);
  return r;
}
double complex dq_fdqx5 (float a, double complex b,
		float c, double complex d,
		float e, double complex f,
		float g, double complex h,
		float i, double complex j)
{
  double complex r=a+b+c+d+e+f+g+h+i+j;
  fprintf(out,"double complex f(5*(float,double complex)):(%g,%g+%g*I,%g,%g+%g*I,%g,%g+%g*I,%g,%g+%g*I,%g,%g+%g*I)",
		  (double)a,(double)creal(b),(double)creal(b),
		  (double)c,(double)creal(d),(double)creal(d),
		  (double)e,(double)creal(f),(double)creal(f),
		  (double)g,(double)creal(h),(double)creal(h),
		  (double)i,(double)creal(j),(double)creal(j));
  fflush(out);
  return r;
}
#endif /* double complex */
#if 1 /* long long double complex tests */
long double complex ldq_ldq (long double complex a)
{
  long double complex r=a+1.0;
  fprintf(out,"long double complex f(long double complex):(%g+%g*I)",
		  (double)creall(a), (double)cimagl(a));
  fflush(out);
  return r;
}
long double complex ldq_ldq2 (long double complex a, long double complex b)
{
  long double complex r=a+b;
  fprintf(out,"long double complex f(2*long double complex):(%g+%g*I,%g+%g*I)",
		  (double)creall(a), (double)cimagl(a),
		  (double)creall(b), (double)cimagl(b));
  fflush(out);
  return r;
}
long double complex ldq_fldq (float a, long double complex b)
{
  long double complex r=a+b;
  fprintf(out,"long double complex f(float, long double complex):(%g,%g+%g*I)",
		  (double)a, (double)creall(b), (double)cimagl(b));
  fflush(out);
  return r;
}
long double complex ldq_ldq4 (long double complex a, long double complex b, long double complex c, long double complex d)
{
  long double complex r=a+b+c+d;
  fprintf(out,"long double complex f(4*long double complex):(%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I)",
		  (double)creall(a), (double)cimagl(a),
		  (double)creall(b), (double)cimagl(b),
		  (double)creall(c), (double)cimagl(c),
		  (double)creall(d), (double)cimagl(d));
  fflush(out);
  return r;
}
long double complex ldq_ldq8 (long double complex a, long double complex b, long double complex c, long double complex d, long double complex e, long double complex f,
             long double complex g, long double complex h)
{
  long double complex r=a+b+c+d+e+f+g+h;
  fprintf(out,"long double complex f(8*long double complex):(%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I)",
		  (double)creall(a), (double)cimagl(a),
		  (double)creall(b), (double)cimagl(b),
		  (double)creall(c), (double)cimagl(c),
		  (double)creall(d), (double)cimagl(d),
		  (double)creall(e), (double)cimagl(e),
		  (double)creall(f), (double)cimagl(f),
		  (double)creall(g), (double)cimagl(g),
		  (double)creall(h), (double)cimagl(h));
  fflush(out);
  return r;
}
long double complex ldq_ldq16 (long double complex a, long double complex b, long double complex c, long double complex d, long double complex e, long double complex f,
              long double complex g, long double complex h, long double complex i, long double complex j, long double complex k, long double complex l,
              long double complex m, long double complex n, long double complex o, long double complex p)
{
  long double complex r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  fprintf(out,"long double complex f(16*long double complex):(%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I,%g+%g*I)",
		  (double)creall(a), (double)cimagl(a),
		  (double)creall(b), (double)cimagl(b),
		  (double)creall(c), (double)cimagl(c),
		  (double)creall(d), (double)cimagl(d),
		  (double)creall(e), (double)cimagl(e),
		  (double)creall(f), (double)cimagl(f),
		  (double)creall(g), (double)cimagl(g),
		  (double)creall(h), (double)cimagl(h),
		  (double)creall(i), (double)cimagl(i),
		  (double)creall(j), (double)cimagl(j),
		  (double)creall(k), (double)cimagl(k),
		  (double)creall(l), (double)cimagl(l),
		  (double)creall(m), (double)cimagl(m),
		  (double)creall(n), (double)cimagl(n),
		  (double)creall(o), (double)cimagl(o),
		  (double)creall(p), (double)cimagl(p));
  fflush(out);
  return r;
}
long double complex ldq_fldqx5 (float a, long double complex b,
		float c, long double complex d,
		float e, long double complex f,
		float g, long double complex h,
		float i, long double complex j)
{
  long double complex r=a+b+c+d+e+f+g+h+i+j;
  fprintf(out,"long double complex f(5*(float,long double complex)):(%g,%g+%g*I,%g,%g+%g*I,%g,%g+%g*I,%g,%g+%g*I,%g,%g+%g*I)",
		  (double)a,(double)creall(b),(double)cimagl(b),
		  (double)c,(double)creall(d),(double)cimagl(d),
		  (double)e,(double)creall(f),(double)cimagl(f),
		  (double)g,(double)creall(h),(double)cimagl(h),
		  (double)i,(double)creall(j),(double)cimagl(j));
  fflush(out);
  return r;
}
#endif /* long long double complex */
#endif

/* pointer tests */
double d_ifpsp (int a, float* b, Int* c)
{
  double ret = a + *b + c->x;
  fprintf(out,"double f(int,(double)float*,Int*):(%d,0x%p[%f],0x%p[%d])",
		  a, b,*b, c,c->x);
  fflush(out);
  return ret;
}
void* vp_vpdpcpsp (void* a, double* b, char* c, Int* d)
{
  void* ret = (char*)b + 1;
  fprintf(out,"void* f(void*,double*,char*,Int*):(0x%p,0x%p,0x%p,0x%p)",a,b,c,d);
  fflush(out);
  return ret;
}

/* string tests */
int i_cpcp (char *a, char *b)
{
  const int maxlen = 80;
  char result[maxlen];
  fprintf(out,"int f(char*,char*):('%s','%s')",a,b);
  fflush(out);
  result[0] = '\0';
  strncat(result, a, maxlen-1);
  strncat(result, b, maxlen-1-strlen(a));
  return strlen(result);
}
ulong ul_cp3 (char /*const*/ * a, char /*const*/ * b, char /*const*/ * c){
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
ok: return ret;
}

/* mixed number tests */
uchar uc_ucsil (uchar a, ushort b, uint c, ulong d)
{
  uchar r = (uchar)-1;
  fprintf(out,"uchar f(uchar,ushort,uint,ulong):(%u,%u,%u,%lu)",a,b,c,d);
  fflush(out);
  return r;
}
double d_iidd (int a, int b, double c, double d)
{
  double r = a+b+c+d;
  fprintf(out,"double f(int,int,double,double):(%d,%d,%g,%g)",a,b,c,d);
  fflush(out);
  return r;
}
double d_iiidi (int a, int b, int c, double d, int e)
{
  double r = a+b+c+d+e;
  fprintf(out,"double f(int,int,int,double,int):(%d,%d,%d,%g,%d)",a,b,c,d,e);
  fflush(out);
  return r;
}
double d_idid (int a, double b, int c, double d)
{
  double r = a+b+c+d;
  fprintf(out,"double f(int,double,int,double):(%d,%g,%d,%g)",a,b,c,d);
  fflush(out);
  return r;
}
double d_fdi (float a, double b, int c)
{
  double r = a+b+c;
  fprintf(out,"double f(float,double,int):(%g,%g,%d)",a,b,c);
  fflush(out);
  return r;
}
ushort us_cdcd (char a, double b, char c, double d)
{
  ushort r = (ushort)(a + b + c + d);
  fprintf(out,"ushort f(char,double,char,double):('%c',%g,'%c',%g)",a,b,c,d);
  fflush(out);
  return r;
}

long long ll_iiilli (int a, int b, int c, long long d, int e)
{
  long long r = (long long)(int)a+(long long)(int)b+(long long)(int)c+d+(long long)(int)e;
  fprintf(out,"long long f(int,int,int,long long,int):(%d,%d,%d,0x%lx%08lx,%d)",a,b,c,(long)(d>>32),(long)(d&0xffffffff),e);
  fflush(out);
  return r;
}
long long ll_flli (float a, long long b, int c)
{
  long long r = (long long)(int)a + b + (long long)c;
  fprintf(out,"long long f(float,long long,int):(%g,0x%lx%08lx,0x%lx)",a,(long)(b>>32),(long)(b&0xffffffff),(long)c);
  fflush(out);
  return r;
}

float f_fi (float a, int z)
{
  float r = a+z;
  fprintf(out,"float f(float,int):(%g,%d)",a,z);
  fflush(out);
  return r;
}
float f_f2i (float a, float b, int z)
{
  float r = a+b+z;
  fprintf(out,"float f(2*float,int):(%g,%g,%d)",a,b,z);
  fflush(out);
  return r;
}
float f_f3i (float a, float b, float c, int z)
{
  float r = a+b+c+z;
  fprintf(out,"float f(3*float,int):(%g,%g,%g,%d)",a,b,c,z);
  fflush(out);
  return r;
}
float f_f4i (float a, float b, float c, float d, int z)
{
  float r = a+b+c+d+z;
  fprintf(out,"float f(4*float,int):(%g,%g,%g,%g,%d)",a,b,c,d,z);
  fflush(out);
  return r;
}
float f_f7i (float a, float b, float c, float d, float e, float f, float g,
             int z)
{
  float r = a+b+c+d+e+f+g+z;
  fprintf(out,"float f(7*float,int):(%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,z);
  fflush(out);
  return r;
}
float f_f8i (float a, float b, float c, float d, float e, float f, float g,
             float h, int z)
{
  float r = a+b+c+d+e+f+g+h+z;
  fprintf(out,"float f(8*float,int):(%g,%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,h,z);
  fflush(out);
  return r;
}
float f_f12i (float a, float b, float c, float d, float e, float f, float g,
              float h, float i, float j, float k, float l, int z)
{
  float r = a+b+c+d+e+f+g+h+i+j+k+l+z;
  fprintf(out,"float f(12*float,int):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,h,i,j,k,l,z);
  fflush(out);
  return r;
}
float f_f13i (float a, float b, float c, float d, float e, float f, float g,
              float h, float i, float j, float k, float l, float m, int z)
{
  float r = a+b+c+d+e+f+g+h+i+j+k+l+m+z;
  fprintf(out,"float f(13*float,int):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,h,i,j,k,l,m,z);
  fflush(out);
  return r;
}

double d_di (double a, int z)
{
  double r = a+z;
  fprintf(out,"double f(double,int):(%g,%d)",a,z);
  fflush(out);
  return r;
}
double d_d2i (double a, double b, int z)
{
  double r = a+b+z;
  fprintf(out,"double f(2*double,int):(%g,%g,%d)",a,b,z);
  fflush(out);
  return r;
}
double d_d3i (double a, double b, double c, int z)
{
  double r = a+b+c+z;
  fprintf(out,"double f(3*double,int):(%g,%g,%g,%d)",a,b,c,z);
  fflush(out);
  return r;
}
double d_d4i (double a, double b, double c, double d, int z)
{
  double r = a+b+c+d+z;
  fprintf(out,"double f(4*double,int):(%g,%g,%g,%g,%d)",a,b,c,d,z);
  fflush(out);
  return r;
}
double d_d7i (double a, double b, double c, double d, double e, double f,
              double g, int z)
{
  double r = a+b+c+d+e+f+g+z;
  fprintf(out,"double f(7*double,int):(%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,z);
  fflush(out);
  return r;
}
double d_d8i (double a, double b, double c, double d, double e, double f,
              double g, double h, int z)
{
  double r = a+b+c+d+e+f+g+h+z;
  fprintf(out,"double f(8*double,int):(%g,%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,h,z);
  fflush(out);
  return r;
}
double d_d12i (double a, double b, double c, double d, double e, double f,
               double g, double h, double i, double j, double k, double l,
               int z)
{
  double r = a+b+c+d+e+f+g+h+i+j+k+l+z;
  fprintf(out,"double f(12*double,int):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,h,i,j,k,l,z);
  fflush(out);
  return r;
}
double d_d13i (double a, double b, double c, double d, double e, double f,
               double g, double h, double i, double j, double k, double l,
               double m, int z)
{
  double r = a+b+c+d+e+f+g+h+i+j+k+l+m+z;
  fprintf(out,"double f(13*double,int):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%d)",a,b,c,d,e,f,g,h,i,j,k,l,m,z);
  fflush(out);
  return r;
}

/* small structure return tests */
Size1 S1_v (void)
{
  fprintf(out,"Size1 f(void):");
  fflush(out);
  return Size1_1;
}
Size2 S2_v (void)
{
  fprintf(out,"Size2 f(void):");
  fflush(out);
  return Size2_1;
}
Size3 S3_v (void)
{
  fprintf(out,"Size3 f(void):");
  fflush(out);
  return Size3_1;
}
Size4 S4_v (void)
{
  fprintf(out,"Size4 f(void):");
  fflush(out);
  return Size4_1;
}
Size7 S7_v (void)
{
  fprintf(out,"Size7 f(void):");
  fflush(out);
  return Size7_1;
}
Size8 S8_v (void)
{
  fprintf(out,"Size8 f(void):");
  fflush(out);
  return Size8_1;
}
Size12 S12_v (void)
{
  fprintf(out,"Size12 f(void):");
  fflush(out);
  return Size12_1;
}
Size15 S15_v (void)
{
  fprintf(out,"Size15 f(void):");
  fflush(out);
  return Size15_1;
}
Size16 S16_v (void)
{
  fprintf(out,"Size16 f(void):");
  fflush(out);
  return Size16_1;
}

/* structure tests */
Int I_III (Int a, Int b, Int c)
{
  Int r;
  r.x = a.x + b.x + c.x;
  fprintf(out,"Int f(Int,Int,Int):({%d},{%d},{%d})",a.x,b.x,c.x);
  fflush(out);
  return r;
}
Char C_CdC (Char a, double b, Char c)
{
  Char r;
  r.x = (a.x + c.x)/2;
  fprintf(out,"Char f(Char,double,Char):({'%c'},%g,{'%c'})",a.x,b,c.x);
  fflush(out);
  return r;
}
Float F_Ffd (Float a, float b, double c)
{
  Float r;
  r.x = a.x + b + c;
  fprintf(out,"Float f(Float,float,double):({%g},%g,%g)",a.x,b,c);
  fflush(out);
  return r;
}
Double D_fDd (float a, Double b, double c)
{
  Double r;
  r.x = a + b.x + c;
  fprintf(out,"Double f(float,Double,double):(%g,{%g},%g)",a,b.x,c);
  fflush(out);
  return r;
}
Double D_Dfd (Double a, float b, double c)
{
  Double r;
  r.x = a.x + b + c;
  fprintf(out,"Double f(Double,float,double):({%g},%g,%g)",a.x,b,c);
  fflush(out);
  return r;
}
J J_JiJ (J a, int b, J c)
{
  J r;
  r.l1 = a.l1+c.l1; r.l2 = a.l2+b+c.l2;
  fprintf(out,"J f(J,int,J):({%ld,%ld},%d,{%ld,%ld})",a.l1,a.l2,b,c.l1,c.l2);
  fflush(out);
  return r;
}
T T_TcT (T a, char b, T c)
{
  T r;
  r.c[0]='b'; r.c[1]=c.c[1]; r.c[2]=c.c[2];
  fprintf(out,"T f(T,char,T):({\"%c%c%c\"},'%c',{\"%c%c%c\"})",a.c[0],a.c[1],a.c[2],b,c.c[0],c.c[1],c.c[2]);
  fflush(out);
  return r;
}
X X_BcdB (B a, char b, double c, B d)
{
  static X xr={"return val",'R'};
  X r;
  r = xr;
  r.c1 = b;
  fprintf(out,"X f(B,char,double,B):({%g,{%d,%d,%d}},'%c',%g,{%g,{%d,%d,%d}})",
          a.d,a.i[0],a.i[1],a.i[2],b,c,d.d,d.i[0],d.i[1],d.i[2]);
  fflush(out);
  return r;
}

/* Test for cases where some argument (especially structure, 'long long', or
   'double') may be passed partially in general-purpose argument registers
   and partially on the stack. Different ABIs pass between 4 and 8 arguments
   (or none) in general-purpose argument registers. */

long l_l0K (K b, long c)
{
  long r = b.l1 + b.l2 + b.l3 + b.l4 + c;
  fprintf(out,"long f(K,long):(%ld,%ld,%ld,%ld,%ld)",b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  return r;
}
long l_l1K (long a1, K b, long c)
{
  long r = a1 + b.l1 + b.l2 + b.l3 + b.l4 + c;
  fprintf(out,"long f(long,K,long):(%ld,%ld,%ld,%ld,%ld,%ld)",a1,b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  return r;
}
long l_l2K (long a1, long a2, K b, long c)
{
  long r = a1 + a2 + b.l1 + b.l2 + b.l3 + b.l4 + c;
  fprintf(out,"long f(2*long,K,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a1,a2,b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  return r;
}
long l_l3K (long a1, long a2, long a3, K b, long c)
{
  long r = a1 + a2 + a3 + b.l1 + b.l2 + b.l3 + b.l4 + c;
  fprintf(out,"long f(3*long,K,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a1,a2,a3,b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  return r;
}
long l_l4K (long a1, long a2, long a3, long a4, K b, long c)
{
  long r = a1 + a2 + a3 + a4 + b.l1 + b.l2 + b.l3 + b.l4 + c;
  fprintf(out,"long f(4*long,K,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a1,a2,a3,a4,b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  return r;
}
long l_l5K (long a1, long a2, long a3, long a4, long a5, K b, long c)
{
  long r = a1 + a2 + a3 + a4 + a5 + b.l1 + b.l2 + b.l3 + b.l4 + c;
  fprintf(out,"long f(5*long,K,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a1,a2,a3,a4,a5,b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  return r;
}
long l_l6K (long a1, long a2, long a3, long a4, long a5, long a6, K b, long c)
{
  long r = a1 + a2 + a3 + a4 + a5 + a6 + b.l1 + b.l2 + b.l3 + b.l4 + c;
  fprintf(out,"long f(6*long,K,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a1,a2,a3,a4,a5,a6,b.l1,b.l2,b.l3,b.l4,c);
  fflush(out);
  return r;
}
/* These tests is crafted on the knowledge that for all known ABIs:
   * 17 > number of floating-point argument registers,
   * 3 < number of general-purpose argument registers < 3 + 6. */
float f_f17l3L (float a, float b, float c, float d, float e, float f, float g,
                float h, float i, float j, float k, float l, float m, float n,
                float o, float p, float q,
                long s, long t, long u, L z)
{
  float r = a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+s+t+u+z.l1+z.l2+z.l3+z.l4+z.l5+z.l6;
  fprintf(out,"float f(17*float,3*int,L):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,s,t,u,z.l1,z.l2,z.l3,z.l4,z.l5,z.l6);
  fflush(out);
  return r;
}
double d_d17l3L (double a, double b, double c, double d, double e, double f,
                 double g, double h, double i, double j, double k, double l,
                 double m, double n, double o, double p, double q,
                 long s, long t, long u, L z)
{
  double r = a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+s+t+u+z.l1+z.l2+z.l3+z.l4+z.l5+z.l6;
  fprintf(out,"double f(17*double,3*int,L):(%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld)",a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,s,t,u,z.l1,z.l2,z.l3,z.l4,z.l5,z.l6);
  fflush(out);
  return r;
}

long long ll_l2ll (long a1, long a2, long long b, long c)
{
  long long r = (long long) (a1 + a2) + b + c;
  fprintf(out,"long long f(2*long,long long,long):(%ld,%ld,0x%lx%08lx,%ld)",a1,a2,(long)(b>>32),(long)(b&0xffffffff),c);
  fflush(out);
  return r;
}
long long ll_l3ll (long a1, long a2, long a3, long long b, long c)
{
  long long r = (long long) (a1 + a2 + a3) + b + c;
  fprintf(out,"long long f(3*long,long long,long):(%ld,%ld,%ld,0x%lx%08lx,%ld)",a1,a2,a3,(long)(b>>32),(long)(b&0xffffffff),c);
  fflush(out);
  return r;
}
long long ll_l4ll (long a1, long a2, long a3, long a4, long long b, long c)
{
  long long r = (long long) (a1 + a2 + a3 + a4) + b + c;
  fprintf(out,"long long f(4*long,long long,long):(%ld,%ld,%ld,%ld,0x%lx%08lx,%ld)",a1,a2,a3,a4,(long)(b>>32),(long)(b&0xffffffff),c);
  fflush(out);
  return r;
}
long long ll_l5ll (long a1, long a2, long a3, long a4, long a5, long long b, long c)
{
  long long r = (long long) (a1 + a2 + a3 + a4 + a5) + b + c;
  fprintf(out,"long long f(5*long,long long,long):(%ld,%ld,%ld,%ld,%ld,0x%lx%08lx,%ld)",a1,a2,a3,a4,a5,(long)(b>>32),(long)(b&0xffffffff),c);
  fflush(out);
  return r;
}
long long ll_l6ll (long a1, long a2, long a3, long a4, long a5, long a6, long long b, long c)
{
  long long r = (long long) (a1 + a2 + a3 + a4 + a5 + a6) + b + c;
  fprintf(out,"long long f(6*long,long long,long):(%ld,%ld,%ld,%ld,%ld,%ld,0x%lx%08lx,%ld)",a1,a2,a3,a4,a5,a6,(long)(b>>32),(long)(b&0xffffffff),c);
  fflush(out);
  return r;
}
long long ll_l7ll (long a1, long a2, long a3, long a4, long a5, long a6, long a7, long long b, long c)
{
  long long r = (long long) (a1 + a2 + a3 + a4 + a5 + a6 + a7) + b + c;
  fprintf(out,"long long f(7*long,long long,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld,0x%lx%08lx,%ld)",a1,a2,a3,a4,a5,a6,a7,(long)(b>>32),(long)(b&0xffffffff),c);
  fflush(out);
  return r;
}

double d_l2d (long a1, long a2, double b, long c)
{
  double r = (double) (a1 + a2) + b + c;
  fprintf(out,"double f(2*long,double,long):(%ld,%ld,%g,%ld)",a1,a2,b,c);
  fflush(out);
  return r;
}
double d_l3d (long a1, long a2, long a3, double b, long c)
{
  double r = (double) (a1 + a2 + a3) + b + c;
  fprintf(out,"double f(3*long,double,long):(%ld,%ld,%ld,%g,%ld)",a1,a2,a3,b,c);
  fflush(out);
  return r;
}
double d_l4d (long a1, long a2, long a3, long a4, double b, long c)
{
  double r = (double) (a1 + a2 + a3 + a4) + b + c;
  fprintf(out,"double f(4*long,double,long):(%ld,%ld,%ld,%ld,%g,%ld)",a1,a2,a3,a4,b,c);
  fflush(out);
  return r;
}
double d_l5d (long a1, long a2, long a3, long a4, long a5, double b, long c)
{
  double r = (double) (a1 + a2 + a3 + a4 + a5) + b + c;
  fprintf(out,"double f(5*long,double,long):(%ld,%ld,%ld,%ld,%ld,%g,%ld)",a1,a2,a3,a4,a5,b,c);
  fflush(out);
  return r;
}
double d_l6d (long a1, long a2, long a3, long a4, long a5, long a6, double b, long c)
{
  double r = (double) (a1 + a2 + a3 + a4 + a5 + a6) + b + c;
  fprintf(out,"double f(6*long,double,long):(%ld,%ld,%ld,%ld,%ld,%ld,%g,%ld)",a1,a2,a3,a4,a5,a6,b,c);
  fflush(out);
  return r;
}
double d_l7d (long a1, long a2, long a3, long a4, long a5, long a6, long a7, double b, long c)
{
  double r = (double) (a1 + a2 + a3 + a4 + a5 + a6 + a7) + b + c;
  fprintf(out,"double f(7*long,double,long):(%ld,%ld,%ld,%ld,%ld,%ld,%ld,%g,%ld)",a1,a2,a3,a4,a5,a6,a7,b,c);
  fflush(out);
  return r;
}
double d_divar(double d1, unsigned cnt, ...){
  double r = d1;
  fprintf(out,"double d_divar(double,unsigned,...):(%g,%u",d1,cnt); fflush(out);
  va_list args;
  va_start(args, cnt);
  for(unsigned i=0U; i<cnt; ++i){
    double d = va_arg(args,double);
    fprintf(out,",%g",d); fflush(out);
    r += d;
  }
  va_end(args);
  fprintf(out,")"); fflush(out);
  return r;
}
/* illegal C : double d_pdvar(...) */
double d_pdvar(double* pd,...){
  double r=0.0;
  fprintf(out,"double d_pdvar(double*,...):("); fflush(out);
  if(pd==NULL){
    fprintf(out,"NULL"); fflush(out);
  }else{
    r = *pd;
    fprintf(out,"%g",*pd); fflush(out);
    va_list args;
    va_start(args, pd);
    for(unsigned i=1U; i<999U; ++i){
      double* d = va_arg(args,double*);
      if((void*)d == NULL){
        fprintf(out,",NULL"); fflush(out);
        break;
      }
      fprintf(out,",%g",*d); fflush(out);
      r += *d;
    }
    va_end(args);
  }
  fprintf(out,")"); fflush(out);
  return r;
}
int i_cpivar(char *buf, char *fmt, ...){
  int r;
  va_list args;
  fprintf(out,"int i_cpivar(char*,char*,...):(%p,\"%s\",...)",(void*)buf,fmt); fflush(out);
  va_start(args,fmt);
  r = vsprintf(buf,fmt,args);
  fprintf(out,":%s",buf);
  va_end(args);
  return r;
}
int i_i5uvari(int s1,int s2,int s3,int s4,int s5,unsigned cnt,...){
  int r = 0;
  va_list args;
  va_start(args,cnt);
  fprintf(out,"i_i5uvari(int,int,int,int,int,unsigned,int...):(%d,%d,%d,%d,%d,%u,...",s1,s2,s3,s4,s5,cnt); fflush(out);
  for(unsigned i=0U; i<cnt; ++i){
    int j = va_arg(args,int);
    fprintf(out,"%c%d",(i==0?'{':','),j); fflush(out);
    if(j==s1||j==s2||j==s3||j==s4||j==s5) r+= j;
  }
  va_end(args);
  fprintf(out,"})"); fflush(out);
  return r;
}
/* vim: set sw=2 ts=2 et: */
