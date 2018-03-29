/* -----------------------------------------------------------------------
   ffi.c - Copyright (c) 2003, 2004, 2006, 2007, 2012 Kaz Kojima
           Copyright (c) 2008 Anthony Green
   
   NEC VE Aurora (c) 2018 NEC Labs America, LLC
   VE ABI support


   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   ``Software''), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED ``AS IS'', WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------- */

#include <ffi.h>
#include <ffi_common.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> /* intptr_t */

#include <math.h>       /* fabs */
#include <complex.h>

#define DEBUG_LEVEL   3
#define FIRST_ARG_BYTE  176 /* 22*8: fp, ret | %tp %got %plt %s17 16*%s18..33 */
#define ROUND_UP(v, a)  (((size_t)(v) + (a) - 1) & ~((a) - 1))

#define debug(lvl, x...) do { if (lvl <= DEBUG_LEVEL) { printf(x); fflush(stdout); } } while (0)

#define NGREGARG 8
//#define NFREGARG 12

void print_s0(UINT64 s0){
    debug(2, " print_s0: %lu = %ld = 0x%lx ", (long unsigned)s0, 
            (long int)s0, (long unsigned)s0);
}
void print_s0_example(UINT64 s0){
    print_s0(s0);
    if( s0 != 2 ) goto done;
    s0 = s0 + s0;
done:
    print_s0(s0);
}
void print_msg_u64(void const* const s0_msg, UINT64 const s1_u64){
    debug(2, " %s:0x%lx,%lu,%ld ", (char const*)s0_msg, (long unsigned)s1_u64,
            (long int)s1_u64, (long unsigned)s1_u64);
    //debug(2, " %s:0x%lx,%lu,%ld,%f ", (char const*)s0_msg, (long unsigned)s1_u64,
    //        (long int)s1_u64, (long unsigned)s1_u64, *(float*)(void*)&s1_u64);
}
void print_msg_u64_example(void const* const s0_msg, UINT64 const s1_u64){
    print_msg_u64((char const*)s0_msg, s1_u64);
}
void print_msg_float(void const* const s0_msg, float const s1_f){
    debug(2, " %s:%f ", (char const*)s0_msg, s1_f);
}
void print_msg_double(void const* const s0_msg, double const s1_f){
    debug(2, " %s:%f ", (char const*)s0_msg, s1_f);
}

ffi_cif ffi_cif_example;
const int ffi_cif_offset_rtype = (char*)&ffi_cif_example.rtype - (char*)&ffi_cif_example;
ffi_type ffi_type_example;
const int ffi_type_offset_type = (char*)&ffi_type_example.type - (char*)&ffi_type_example;
void prt_ecif_cif_rtype_type( extended_cif *ecif ){
    printf("ecif->rtype_type=%lu\n", (long unsigned)ecif->cif->rtype->type);
    union { uint64_t u64; float f[2]; } val;
    val.u64 = 0UL;
    val.f[0] = 1.499f;
    float x = val.f[0];
    printf(" val.f[0]=%f", (double)x);
}

typedef enum ve_argclass {
    VE_REGISTER = 0,
    VE_REFERENCE = 1,
    VE_BOTH
} Argclass;

static Argclass argclass (ffi_type const *arg)
{
    Argclass ret = VE_REGISTER; /* most things */

    if (arg->type == FFI_TYPE_STRUCT) /* or union or array */
        ret = VE_REFERENCE;

#if defined FFI_TARGET_SPECIFIC_VARIADIC
    else if( arg->type == FFI_TARGET_SPECIFIC_VARIADIC /* where defined? */
            /* || prototype-less ?? */
           ) ret = VE_BOTH;
#endif

    return ret;
}
static char const* argclass_names[3] = {"REG","REF","BOTH"};

/* ffi_type is {size_t size; u16 alignment; u16 type; _ffi_type **elements;} */
static char const* ffi_type_str( ffi_type const* const t ){
    char * ret = "??";
    switch(t->type){
        case( FFI_TYPE_VOID       ): ret = "void"; /*0*/ break;
        case( FFI_TYPE_INT        ): ret = "int"; /*1*/ break;
        case( FFI_TYPE_FLOAT      ): ret = "float"; /*2*/ break;
        case( FFI_TYPE_DOUBLE     ): ret = "double"; /*3*/ break;
        case( FFI_TYPE_LONGDOUBLE ): ret = "Ldouble"; /*4*/ break;
        case( FFI_TYPE_UINT8      ): ret = "u8"; /*5*/ break;
        case( FFI_TYPE_SINT8      ): ret = "s8"; /*6*/ break;
        case( FFI_TYPE_UINT16     ): ret = "u16"; /*7*/ break;
        case( FFI_TYPE_SINT16     ): ret = "s16"; /*8*/ break;
        case( FFI_TYPE_UINT32     ): ret = "u32"; /*9*/ break;
        case( FFI_TYPE_SINT32     ): ret = "s32"; /*10*/ break;
        case( FFI_TYPE_UINT64     ): ret = "u64"; /*11*/ break;
        case( FFI_TYPE_SINT64     ): ret = "s64"; /*12*/ break;
        case( FFI_TYPE_STRUCT     ): ret = "struct"; /*13*/ break;
        case( FFI_TYPE_POINTER    ): ret = "ptr"; /*14*/ break;
        case( FFI_TYPE_COMPLEX    ): ret = "complex"; /*15*/ break;
        default: ret = "Huh"; break;
    }
    return ret;
}

static char buf_avalue[1024];
static char const* ffi_avalue_str( ffi_type const* const t, void* avalue ){
    char *buf = &buf_avalue[0];
    int rem_len = 1024;
#define DPRINT(...) do \
    { \
        int n = snprintf(buf, rem_len, __VA_ARGS__); \
        if( n > rem_len ){ rem_len = 0; } \
        else { buf+=n; rem_len-=n; } \
    } while(0)
    switch(t->type){
        case( FFI_TYPE_VOID       ): DPRINT("void"); break;
        case( FFI_TYPE_INT        ): DPRINT("%lu",*(int*)avalue); break;
        case( FFI_TYPE_FLOAT      ): DPRINT("%f",*(float*)avalue); break;
        case( FFI_TYPE_DOUBLE     ): DPRINT("%f",*(double*)avalue); break;
        case( FFI_TYPE_LONGDOUBLE ): DPRINT("%lf",*(long double*)avalue); break;
        case( FFI_TYPE_UINT8      ): DPRINT("%lu",(long unsigned) *(UINT8*)avalue); break;
        case( FFI_TYPE_SINT8      ): DPRINT("%ld",(long   signed) *(SINT8*)avalue); break;
        case( FFI_TYPE_UINT16     ): DPRINT("%lu",(long unsigned) *(UINT16*)avalue); break;
        case( FFI_TYPE_SINT16     ): DPRINT("%ld",(long   signed) *(SINT16*)avalue); break;
        case( FFI_TYPE_UINT32     ): DPRINT("%lu",(long unsigned) *(UINT32*)avalue); break;
        case( FFI_TYPE_SINT32     ): DPRINT("%ld",(long   signed) *(SINT32*)avalue); break;
        case( FFI_TYPE_UINT64     ): DPRINT("%lu",(long unsigned) *(UINT64*)avalue); break;
        case( FFI_TYPE_SINT64     ): DPRINT("%ld",(long   signed) *(SINT64*)avalue); break;
        case( FFI_TYPE_STRUCT     ): DPRINT("struct@%p",*(void**)avalue); break;
        case( FFI_TYPE_POINTER    ): DPRINT("ptr@%p",*(void**)avalue); break;
        case( FFI_TYPE_COMPLEX    ):
                                     {
                                         complex c=*(complex*)avalue;
                                         double r=creal(c), i=cimag(c);
                                         DPRINT("%f%s%f",r,(i<0?"-i*":"+i*"),fabs(i));
                                     } break;
        default: DPRINT("?%p",avalue); break;
    }
    return &buf_avalue[0];
}

static char buf_avalues[1024];
static char const* ffi_avalues_str( ffi_cif const* cif, void **avalue ){
    char *buf = &buf_avalues[0];
    int rem_len = 1024;
#define DPRINT(...) do \
    { \
        int n = snprintf(buf, rem_len, __VA_ARGS__); \
        if( n > rem_len ){ rem_len = 0; } \
        else { buf+=n; rem_len-=n; } \
    } while(0)
    if(cif == NULL) DPRINT("cif==NULL,avalue=%p???",avalue);
    else if(avalue == NULL) DPRINT("avalue=NULL???");
    else if(cif->nargs <= 0 ) DPRINT("avalue={}");
    else if(cif->arg_types==NULL) DPRINT("arg_types==NULL,avalue=%p???",avalue);
    else{
        DPRINT("avalues=");
        for(int i=0; i<cif->nargs; ++i)
            DPRINT("%s%s", (i==0?"{":","), ffi_avalue_str(cif->arg_types[i], avalue[i]));
        DPRINT("}");
    }
    //DPRINT("\n");
    return &buf_avalues[0];
}

static char buf_type[1024];
static char const* ffi_type_detail( ffi_type const* const t ){
    char *buf = &buf_type[0];
    int rem_len = 1024;
#define DPRINT(...) do \
    { \
        int n = snprintf(buf, rem_len, __VA_ARGS__); \
        if( n > rem_len ){ rem_len = 0; } \
        else { buf+=n; rem_len-=n; } \
    } while(0)
    DPRINT("%s(s%llu,a%u)%s", ffi_type_str(t), (unsigned long long)t->size
            , (unsigned)t->alignment, argclass_names[argclass(t)]);
    return &buf_type[0];
#undef DPRINT
}

static char buf_cif[1024];
static char const* ffi_cif_str( ffi_cif const* cif){
    char *buf = &buf_cif[0];
    int rem_len = 1024;
#define DPRINT(...) do \
    { \
        int n = snprintf(buf, rem_len, __VA_ARGS__); \
        if( n > rem_len ){ rem_len = 0; } \
        else { buf+=n; rem_len-=n; } \
    } while(0)
    DPRINT("ffi_cif={abi:%d, nargs:%u, arg_types:", cif->abi, cif->nargs);
    if(cif->arg_types==NULL) DPRINT("NULL");
    else if(cif->nargs <= 0) DPRINT("{}");
    else{
        /* ffi_type **arg_types */
        for(int i=0; i<cif->nargs; ++i)
            DPRINT("%s%s", (i==0?"{":","), ffi_type_detail(cif->arg_types[i]));
        DPRINT("}");
    }
    DPRINT("\n\t\t, rtype=");
    if(cif->rtype==NULL) DPRINT("NULL");
    else DPRINT("%p[%s]", (void*)cif->rtype, ffi_type_detail(cif->rtype));
    DPRINT(", bytes:%u, flags:%u"
#if defined(FFI_EXTRA_CIF_FIELDS)
            //" [,extra]"
            ", flags2:%lx"
#endif /*FFI_EXTRA_CIF_FIELDS*/
            ") ",
            cif->bytes, cif->flags
#if defined(FFI_EXTRA_CIF_FIELDS)
            , (long unsigned)cif->flags2
#endif
            );
#if defined(FFI_EXTRA_CIF_FIELDS)
    if(cif->flags2 == 0){
        DPRINT("flags2:{no reg args}");
    }else{
        UINT64 flg = cif->flags2;
        int greg;
        DPRINT("flags2:");
        for( greg=0; greg<NGREGARG; ++greg ){
            SINT8 reginfo = flg&0xff;
            flg >>= 8;
            if( reginfo == 0xff ) DPRINT("R");
            else if( reginfo == 0x80 ) DPRINT("x");
            else if( reginfo < 0 ) DPRINT("?");
            else DPRINT("%u",(unsigned)(UINT8)reginfo); /* reginfo>0 holds an arg# in [0,127] */
        }
    }
#endif
    return &buf_cif[0];
#undef DPRINT
}

/* NEC VE has a downward growing stack, which looks like this:

###### caller's stack frame
  Locals and Temporaries
.............
            | (Arg Stack, if needed for the call)
   176(%fp) | Parameter Area for callee
.............
R S A..|
e a r  | 168(%fp)   %s33Callee-saved register
g v e  |  ...        ...
i e a  | 48(%fp)    %s18 Callee-saved register
s      | 40(%fp)    %s17 Linkage Area Register 
t      | 32(%fp)    %plt Procedure Linkage Table Register
e      | 24(%fp)    %got Global Offset Table Register
r......| 16(%fp)    %tp Thread Pointer Register
   8(%sp                return address
   0(%sp)               %sp (or caller's $fp)
............ Caller Stack Frame (16-byte alignment)

   ### same, but wrt %sp for callee

............ Callee Stack Frame (16-byte alignment)

   Scalar types:
     Memory:
       bool, int types up to 64-bit occupy 1,2,4,8 bytes in memory
       ptr is 8byte unsigned
       float, double, long double are 4,8,16 bytes
       memory byte alignment == size of base type
       undefined-fill structs to alignment of largest type
       long double stores top bits in higher then lower mem address
   Scalars are promoted to 64-bit registers %s0..%s7 AND/OR memory by:
     sign-extend/zero-fill signed/unsigned integers toward MSB
     LSBs of float are set to zero
     %s7..7 are filled left-to-right order

   Arguments are passed in registers AND/OR memory Parameter Area.
   The first parameters are stuck into %s0..%s7, consecutively
   Exception long double, long double _Imaginary, long double _Complex
   will skip beginning on an odd-numbered register.

   Arguments are of 3 classes:
     REGISTER  (fit in 64-bit %s0..7)
       - all scalar types 8 bytes or less (incl float, double _Imaginary)
       - long double, complex etc. are split across register
         realHI,realLO, imagHI,imagLO in order %s0..%s7 and mem hi-->low addr
     REFERENCE passed and returned by memory
       - struct, union, array
       - %s0..7 store address of memory image in Parameter Area
     BOTH      passed in registers and memory
       - variadic or no prototype

   Arguments passed via memory are promoted in the same manner as they
   are for register storage (i.e. typically 8-byte alignment)
   structs/unions/arrays ??? satisfy the alignmen of most-aligned member
   (i.e. could be 1,2,4,8,16 alignment)
   */

/* VE ret type can use argclass and deal with VE_REGISTER or VE_REFERENCE.
VE_REFERENCE: %s0 holds [lowest] addr of ret value (usually 176(%fp)
>=176(%fp) in callee-frame holds the struct data.
VE_REGISTER returns use %s0..%s7, stored same way as for calling.

NEC VE ABI 0.10 does not support small-struct optimizations. */


/* ffi_prep_args is called by the assembly routine once stack space
   has been allocated for the function's arguments.
   
   \p stack points to parameter area in caller's stack frame.

   We organize the stack as follows, for simplicity:

   low addr   --------------------> high addr
   stack args area   | 64 bytes reg args area (for %s0..%s7)

   ecif->cif->bytes must be size of non-reg stack args + 64,
   as prepared by ffi_prep_cif_machdep(ffi_cif *cif)
*/

void ffi_prep_args(char *stack, extended_cif const* const ecif)
{
    register unsigned int i;
    register unsigned int avn;
    register void **p_argv;
    register char *stkarg;
#if ! defined(FFI_EXTRA_CIF_FIELDS)
#error "NEC VE relies on machdep having deduce which args go in registers"
#endif
    register char *stkreg;      /* for now, last 64 bytes of ecif->cif->bytes are register arg mirrors */
    char* stkreg_beg;
    char* stkreg_end;
    register unsigned int regn;
    register ffi_type **p_arg;


    /* ecif = { ffi_cif*, void*rvalue, void**avalue } is used in callback from assembly */
    debug(1, "%s %s: stack = %p, ecif = %p, bytes = %u, rvalue=%p, avalue=%p\n      ecif->cif=%s\n",
            "BEGIN", __FUNCTION__, stack, ecif, ecif->cif->bytes, ecif->rvalue, ecif->avalue,
            ffi_cif_str(ecif->cif));
    if(ecif->avalue != NULL) debug(1, "      %s\n", ffi_avalues_str(ecif->cif,ecif->avalue));

    stkarg = stack;                           /* non-register args area */
    stkreg = stack + ecif->cif->bytes - 64;   /* register mirror area %s0..%s7 */
    stkreg_beg = stkreg;        /* overrun check:  stkarg <= stkreg_beg */
    stkreg_end = stkreg + 64;   /* overrun check:  stkreg <= stkreg_end */
    /* above shows that -64 as "adds" of M value (58)1 */
    debug(3,"stkreg=%p + %lu + 0x%x = %p\n",
            (void*)stack,(long unsigned)ecif->cif->bytes,
            -64, (void*)stkreg);
    regn = 0;
    UINT64 regflags = ecif->cif->flags2;
    SINT8  reginfo = regflags&0xFF;
#define STKREG_NEXT do { \
    if(regn < NGREGARG){ \
        stkreg += sizeof(UINT64); ++regn; \
        regflags >>= 8; reginfo = regflags&0xFF; \
        for( ; regn<NGREGARG && reginfo<0; ++regn, \
                regflags>>=8, reginfo=regflags&0xFF ){ \
            *(UINT64*)stkreg = 0UL; \
            stkreg+=sizeof(UINT64); \
        } \
    }else{ \
        reginfo = 0xff; /*reginfo < 0 : arg not in register */ \
    } \
}while(0)


#if !defined(FFI_NO_STRUCTS)
    /* When callee returns an aggregate (VE_REFERENCE), the caller:
       - povides return memory on its stack and set %s0 to caller-176%(sp).
       and callee also set %s0 to the address of the returned struct.

       In effect, a returned struct becomes a hidden "first argument".
       
       Now does libffi stick this "hidden arg" into arg_types?
       If not, how is ffi_cif.rtype used? */
    if (ecif->cif->rtype->type == FFI_TYPE_STRUCT)
    {
        FFI_ASSERT( regn == 0 && reginfo < 0 );
        FFI_ASSERT( ecif->rvalue != NULL );
        *(UINT64*)stkreg = (UINT64)ecif->rvalue;    /* %s0 points at return-struct mem (alloca) */
        STKREG_NEXT;
    }
#endif

    avn = ecif->cif->nargs;
    p_argv = ecif->avalue;

    debug(3," prep_args: ");
    for (i = 0, p_arg = ecif->cif->arg_types; i < avn; i++, p_arg++, p_argv++)
    {
        size_t z;
        int align;

        int type = (*p_arg)->type;
        //debug(3,"\n   a%u[%s]%s",(unsigned)i,ffi_type_detail(*p_arg),ffi_avalue_str(*p_arg,*p_argv));

        Argclass cls = argclass(*p_arg); /* VE_REGISTER/REFERENCE/BOTH */
#if !defined(FFI_NO_STRUCTS)
        if( type == FFI_TYPE_STRUCT ){
            FFI_ASSERT( type == FFI_TYPE_STRUCT ); /* VE has no special small-struct handling */
            FFI_ASSERT( cls == VE_REFERENCE )
            FFI_ASSERT("struct args TBD" == NULL);
            continue;
        }
#endif
        FFI_ASSERT( cls == VE_REGISTER || cls == VE_BOTH);

        z = (*p_arg)->size;
        align = (*p_arg)->alignment;

        if (z <= sizeof (UINT64))
        {
            /* these types are passed as single-register (if possible) */
            union uarg_t {
                UINT64 r1;
                float f[2];
                double d;
            } val;
            val.r1 = 0UL;

            switch (type)
            {

                case FFI_TYPE_SINT8:
                    *(SINT64*) &val.r1 = (SINT64) *(SINT8 *)(*p_argv);
                    break;

                case FFI_TYPE_UINT8:
                    *(UINT64 *) &val.r1 = (UINT64) *(UINT8 *)(*p_argv);
                    break;

                case FFI_TYPE_SINT16:
                    *(SINT64 *) &val.r1 = (SINT64) *(SINT16 *)(*p_argv);
                    break;

                case FFI_TYPE_UINT16:
                    *(UINT64 *) &val.r1 = (UINT64) *(UINT16 *)(*p_argv);
                    break;

                case FFI_TYPE_INT:
                case FFI_TYPE_SINT32:
                    *(SINT64*) &val.r1 = (SINT64) *(SINT32 *)(*p_argv);
                    break;

                case FFI_TYPE_UINT32:
                    *(UINT64*) &val.r1 = (UINT64) *(UINT32 *)(*p_argv);
                    break;

                case FFI_TYPE_SINT64:
                    *(SINT64*) &val.r1 = *(SINT64 *)(*p_argv);
                    break;

                case FFI_TYPE_UINT64:
                    *(UINT64*) &val.r1 = *(UINT64 *)(*p_argv);
                    break;

                case FFI_TYPE_FLOAT: /* zeroes in 'other' half */
                    /*val.r1   = 0UL;*/
                    val.f[1] = *(float*)(*p_argv);
                    break;

                case FFI_TYPE_DOUBLE:
                    val.d = *(double *)(*p_argv);
                    break;

                case FFI_TYPE_POINTER:
                    debug(3," *p_argv=%p ", *(void**)p_argv);
                    *(SINT64*) &val.r1 = (SINT64) *(void**)p_argv;
                    debug(3," ptr 0x%lx ", (unsigned long)val.r1);
                    break;

                default:
                    FFI_ASSERT("unhandled small type in ffi_prep_args" == NULL);
            }
            if( reginfo == i ){ /* is this a register value? */
                *(UINT64*)stkreg = val.r1;
                debug(3," r%lu", (long unsigned)val.r1);
                //debug(3," r%s", ffi_avalue_str(*p_arg,(void*)&val));
                STKREG_NEXT;
                //if( cls == VE_REGISTER )
                //    continue; /* ONLY in register */
                FFI_ASSERT( stkreg <= stkreg_end );
            }
            /* store in arg space ... */
            *(UINT64*)stkarg = val.r1;
            debug(3," stk%lu", (unsigned long)val.r1);
            //debug(3," stk%s", ffi_avalue_str(*p_arg,(void*)&val));
            stkarg += 8;
            FFI_ASSERT( stkarg <= stkreg_beg );
        }else if( type ==  FFI_TYPE_LONGDOUBLE ){
            FFI_ASSERT("prep_args FFI_TYPE_LONGDOUBLE TBD" == NULL);
        }else if( type == FFI_TYPE_COMPLEX ){
            FFI_ASSERT("prep_args FFI_TYPE_COMPLEX TBD" == NULL);
        }
#if 0
        {
            int n = (z + sizeof (UINT64) - 1) / sizeof (UINT64);
            memcpy (stkarg, *p_argv, z);
            stkarg += n * sizeof (UINT64);
        }
#endif
    }
    debug(3,"\n");

    debug(1, "%s %s: stack = %p, ecif = %p, bytes = %u, rvalue=%p, avalue=%p\n      ecif->cif=%s\n",
            "END", __FUNCTION__, stack, ecif, ecif->cif->bytes, ecif->rvalue, ecif->avalue,
            ffi_cif_str(ecif->cif));
    if(ecif->avalue != NULL) debug(1, "      %s\n", ffi_avalues_str(ecif->cif,ecif->avalue));
    return;
}

/* Perform machine dependent cif processing.
 * ffi_prep_args        is called for every invocation, while
 * ffi_prep_cif_machdep is called just once.
 * So here we set flags that simplify setting up the stack in ffi_prep_args.
 * 1. flags2 is used to remember which args get passed in registers
 * 2. TODO (flags3?   For a register arg in flags2, is it a BOTH arg?)
 * */
ffi_status ffi_prep_cif_machdep(ffi_cif *cif)
{
    UINT8 i, cif_nargs=(UINT8)cif->nargs;
    int size, type;
    int n;
    UINT8 greg=0;
    UINT64 flags2 = 0UL;
    //int fpair = -1;
#if defined(FFI_EXTRA_CIF_FIELDS)
    int j=0;
    int m;
    cif->flags2 = 0UL;
#endif

    debug(2,"ve: ffi_prep_cif_machdep(ffi_cif*) %s (cif->bytes=%lu)\n      %s\n",
            "BEGINS", (unsigned long)cif->bytes, ffi_cif_str(cif));

    /* VE_REGISTER --> 0
       return value in %s0.., no regs implicated for the call */
    /* VE_REFERENCE --> 1
       caller provides space on stack and passes address in %s0 as if
       this were a "hidden" first argument to the callee */
    /* greg = argclass(cif->rtype); */  /* too tricky, and want to set flags2! */
    if( argclass(cif->rtype) == VE_REFERENCE ){
        FFI_ASSERT( cif->rtype->type == FFI_TYPE_STRUCT );
        /* use %s0 as a pointer register for return value */
        flags2 = 0x80;  /* first arg 0x80 ==> return value pointer */
        greg=1;
        debug(2," %s:%lx:%u","ret_REF",flags2,(unsigned)greg);
    }

    /* ? which args get passed in register */
    /* C limits # argumnents to a function to 127 */
    /* So for 8 register-args, we can have a 1-bit flag + 7-bit arg# */
    /* The 7-bit arg#s must be increasing. (if not treat as "skip" reg) */
    /* Multi-register args repeat the flags value */
    /* If the flag bit is set, this could mean "skip" (or for %s0 "retval ptr") */
    /* If two consecutive "skip" flags occur, there will be no more register args */
    for (i = 0; i < cif_nargs; ++i)
    {
        int regs = 1;
        ffi_type* ffi_type_i = (cif->arg_types)[i];
        debug(3,"i=%d[%s],greg=%d ",(int)i,ffi_type_detail(ffi_type_i), (int)greg);
        int klas = argclass(ffi_type_i);

        if(greg >= NGREGARG) break;
        type = ffi_type_i->type;
        size = ffi_type_i->size;
        if( klas == VE_REGISTER ){
            switch(type){
                case FFI_TYPE_STRUCT:
                    FFI_ASSERT(klas == VE_REFERENCE);
                    if (size < sizeof (UINT64))
                        cif->bytes += sizeof (UINT64) - size;
                    n = (size + sizeof (UINT64) - 1) / sizeof (UINT64);
                    if (greg >= NGREGARG){
                        debug(3," %s:%lx:%u","REF",flags2,(unsigned)greg);
                        continue;
                    }else if (greg < NGREGARG){ /* structs use 1 reg, as a pointer */
                        flags2 += (UINT64)i<<(greg++*8); /* this greg holds arg 'i' (ptr-to-value) */
                        debug(3," %s:%lx:%u","REF",flags2,(unsigned)greg);
                    }
                    break;
                case FFI_TYPE_LONGDOUBLE: /* also for long double _Imaginary */
                    regs=2; /* and start on even reg */
                    if( greg + (greg&1) + regs <= NGREGARG ){ /* have enough regs! */
                        if( greg&1 ){
                            flags2 += 0xffLU<<(greg++*8); /* "skip" register (zero?) */
                            debug(3," %s:%lx:%u","ldSKIP",flags2,(unsigned)greg);
                        }
                        flags2 += (UINT64)i<<(greg++*8); /* upper part */
                        flags2 += (UINT64)i<<(greg++*8); /* lower part */
                        debug(3," %s:%lx:%u","ldREG",flags2,(unsigned)greg);
                    }else{
                        /* "If there are no registers available for arguments,
                         * the **remaining** arguments are passed by the
                         * parameter area on the stack */
                        for( ; greg<NGREGARG; ++greg ) flags2 += 0xffLU<<(greg*8); /* "skip" */
                        greg = NGREGARG;
                        debug(3," %s:%lx:%u","Done",flags2,(unsigned)greg);
                    }
                    break;
#if 0
                case FFI_TYPE_LONGDOUBLE_COMPLEX:
                    regs=4;  /* and start on even reg */
                    if( greg + (greg&1) + regs <= NGREGARG ){ /* registers available! */
                        greg += (greg&1); /* maybe skip a reg */
                        ; //reg_narg[reginf] = (klas<<14) + (greg<<8) & ((UINT8)i<<0);
                        greg += regs;
                    }else{
                        greg = NGREGARG;
                    }
                    break;
#endif
                default:
                    FFI_ASSERT(klas != VE_REFERENCE);
                    FFI_ASSERT(klas == VE_REGISTER); /* size must be <=8, or 16, or 32 */
                    if (size < sizeof (UINT64)){
                        /* The default cif->bytes seems to be OK for VE (no fixup required ?) */
                        ;//cif->bytes += sizeof (UINT64) - size;
                    }else{
                        FFI_ASSERT( size == 8 ); /* treat larger ones case-by-case */
                    }
                    n = (size + sizeof (UINT64) - 1) / sizeof (UINT64); /* 1 except for long specials types */
                    if (greg >= NGREGARG){
                        debug(3," cont:n=%u ",(unsigned)n);
                        continue;
                    }else if (greg + n > NGREGARG){ /* overflow, do not pass in reg */
                        for( ; greg<NGREGARG; ++greg ) flags2 += 0xffLU<<(greg*8); /* "skip" */
                        greg = NGREGARG;
                        debug(3," %s:%lx:%u","DONE",flags2,(unsigned)greg);
                    }else{
                        n += greg;  /* end register# for this arg */
                        do{
                            flags2 += (UINT64)i<<(greg++*8); /* this greg holds arg 'i' */
                            debug(3," %s:%lx:%u","REG",flags2,(unsigned)greg);
                        }while( greg < n );
                    }
#if 0 && defined(FFI_EXTRA_CIF_FIELDS) /* this was for some other chip */
                    for (m = 0; m < n; m++)
                        cif->flags2 += FFI_TYPE_INT << (2 * j++);
#endif /*FFI_EXTRA_CIF_FIELDS*/
                    break;
            }
        }
#if 0 && !defined(FFI_NO_STRUCTS)
        case FFI_TYPE_STRUCT: /* argclass is REFERENCE */
        FFI_ASSERT(0);
        break;
#endif
    }

    if( greg < NGREGARG ){
        for( ; greg<NGREGARG; ++greg ){
            flags2 += (0xffLU<<(greg*8)); /* "skip" */
        }
        debug(3," %s:%lx:%u","DoNe",flags2,(unsigned)greg);
    }
    debug(3,"\n");
#if defined(FFI_EXTRA_CIF_FIELDS)
    cif->flags2 = flags2;
#endif /*FFI_EXTRA_CIF_FIELDS*/


    /* Set the return type flag XXX CHECKME */
    switch (cif->rtype->type)
    {
#if !defined(FFI_NO_STRUCTS)
        case FFI_TYPE_STRUCT: /* there is no special handling for small-structs*/
            /*cif->bytes += cif->type->size;*/
            /* are structs on stack, or really on "temporary space"? */
            /* I'll try alloca for the space, and the ptr-->%s0 */
#endif
        case FFI_TYPE_VOID:
        case FFI_TYPE_FLOAT:
        case FFI_TYPE_DOUBLE:
        case FFI_TYPE_SINT64:
        case FFI_TYPE_UINT64:
            cif->flags = cif->rtype->type;
            break;

        default:
            cif->flags = cif->rtype->type; /* ?? */
            break;
    }

    cif->bytes += 64; /* ALWAYS provide a register area (fixed size, for now) */

    /* VE ABI 0.32 requires stack frame size & alignment of 16 */
    cif->bytes = FFI_ALIGN( cif->bytes, 16 );

    debug(2,"ve: ffi_prep_cif_machdep(ffi_cif*) %s (cif->bytes=%lu)\n      %s\n",
            "ENDS", (unsigned long)cif->bytes, ffi_cif_str(cif));
    return FFI_OK;
}

/*@-declundef@*/
/*@-exportheader@*/
extern void ffi_call_SYSV(void (*)(char *, extended_cif *), 
			  /*@out@*/ extended_cif *, 
			  unsigned, unsigned,
#if defined(FFI_EXTRA_CIF_FIELDS)
              long unsigned, /*flags2*/
#endif /*FFI_EXTRA_CIF_FIELDS*/
			  /*@out@*/ unsigned *, 
			  void (*fn)(void));
/*@=declundef@*/
/*@=exportheader@*/

void ffi_call_SYSV_example(void (*prep_args)(char *, extended_cif *), 
			  /*@out@*/ extended_cif *ecif, 
			  unsigned bytes, unsigned flags,
#if defined(FFI_EXTRA_CIF_FIELDS)
              long unsigned flags2,
#endif /*FFI_EXTRA_CIF_FIELDS*/
			  /*@out@*/ unsigned *rvalue, 
			  void (*fn)(void))
{
    debug(2,"ve: ffi_call_SYSV_example ('C' stub)\n");
    char* fn_stack = (char*)alloca(bytes);
    prep_args( fn_stack, ecif );
    /* asm copy mem reg area into registers */
    /* invoke fn in current stack frame ? (This is why we need assembler) */
}
void ffi_call_example(/*@dependent@*/ ffi_cif *cif, 
	      void (*fn)(void), 
	      /*@out@*/ void *rvalue, 
	      /*@dependent@*/ void **avalue)
{
    extended_cif ecif;
    UINT64 trvalue;     /* temporary rvalue */

    /* argument values */
    ecif.cif = cif;
    if( cif->nargs == 0 ) ecif.avalue = NULL; /* avalue is ignored */
    else ecif.avalue = avalue;
    /* return value */
    ecif.rvalue = rvalue;
    ffi_call_SYSV_example(ffi_prep_args, &ecif, cif->bytes, cif->flags,
#if defined(FFI_EXTRA_CIF_FIELDS)
            cif->flags2,
#endif /*FFI_EXTRA_CIF_FIELDS*/
            ecif.rvalue, fn);

    debug(2,"ve: ffi_call_example(ffi_cif*, void(*fn)(), void*rvalue, void**avalue)) ENDS %s\n      cif = %s\n      fn = %p, rvalue = %p, avalue=%p\n      %s",
            "(ffi_call_example)",
            ffi_cif_str(cif), fn, rvalue, avalue, ffi_avalues_str(cif,avalue));
}


void ffi_call(/*@dependent@*/ ffi_cif *cif, 
	      void (*fn)(void), 
	      /*@out@*/ void *rvalue, 
	      /*@dependent@*/ void **avalue)
{
#define FFI_USE_ASSEMBLER 1
    /* We might not need an extended_cif at all */
    debug(2,"ve: %s ffi_call(ffi_cif*, void(*fn)(), void*rvalue, void**avalue)) %s\n      cif = %s\n      fn = %p, rvalue = %p, avalue=%p\n      %s\n",
            "BEG",(FFI_USE_ASSEMBLER? "(sysv.S)":"(ffi_call_SYSV_example)"),
            ffi_cif_str(cif), fn, rvalue, avalue, ffi_avalues_str(cif,avalue));
    extended_cif ecif;
    UINT64 trvalue;     /* temporary rvalue */

    /* argument values */
    ecif.cif = cif;
    if( cif->nargs == 0 ) ecif.avalue = NULL; /* avalue is ignored */
    else ecif.avalue = avalue;

    /* return value */
    if( cif->rtype->type == FFI_TYPE_VOID )
        ecif.rvalue = &trvalue; /* rvalue is ignored */
#if !FFI_NOSTRUCTS
    /* If the return value is a struct and we don't have a */
    /* return value address then we need to make one	   */
    /* VE: this adress could be muxed into the arg passing area ? */
    else if (rvalue == NULL
            && (cif->rtype->type == FFI_TYPE_STRUCT)
            && cif->rtype->size > 8)
        ecif.rvalue = alloca(cif->rtype->size);
#endif
    else{
        ecif.rvalue = rvalue; /* pointer to a chunk of memory that will hold the result
                                 of the function call (min 8==register size bytes).
                                 Caller must ensure correct alignment. */
    }

    switch (cif->abi) 
    {
        case FFI_SYSV:
#if FFI_USE_ASSEMBLER /* try out the sysv.S assembler code */
            ffi_call_SYSV(ffi_prep_args, (extended_cif const* const)&ecif, cif->bytes, cif->flags,
#if defined(FFI_EXTRA_CIF_FIELDS)
                    cif->flags2,
#endif /*FFI_EXTRA_CIF_FIELDS*/
                    ecif.rvalue, fn);
#else
            ffi_call_SYSV_example(ffi_prep_args, &ecif, cif->bytes, cif->flags,
#if defined(FFI_EXTRA_CIF_FIELDS)
                    cif->flags2,
#endif /*FFI_EXTRA_CIF_FIELDS*/
                    ecif.rvalue, fn);
#endif
            debug(3,"\n  rvalue[%s] : %p --> %s",
                    ffi_type_detail(cif->rtype),
                    (void*)ecif.rvalue,
                    ffi_avalue_str(cif->rtype,ecif.rvalue));
            break;
        default:
            FFI_ASSERT(0);
            break;
    }

#if 0
    if (rvalue
            && cif->rtype->type == FFI_TYPE_STRUCT
            && return_type (cif->rtype) != FFI_TYPE_STRUCT)
        memcpy (rvalue, &trvalue, cif->rtype->size);
#endif
    debug(2,"ve: %s ffi_call(ffi_cif*, void(*fn)(), void*rvalue, void**avalue)) %s\n      cif = %s\n      fn = %p, rvalue = %p, avalue=%p\n      %s\n\n",
            "ENDS",(FFI_USE_ASSEMBLER? "(sysv.S)":"(ffi_call_SYSV_example)"),
            ffi_cif_str(cif), fn, rvalue, avalue, ffi_avalues_str(cif,avalue));
}

#if FFI_CLOSURES
#error "NEC VE closures TBD"
extern void ffi_closure_SYSV (void);
extern void __ic_invalidate (void *line);

ffi_status
ffi_prep_closure_loc (ffi_closure *closure,
		      ffi_cif *cif,
		      void (*fun)(ffi_cif*, void*, void**, void*),
		      void *user_data,
		      void *codeloc)
{
  unsigned int *tramp;

  if (cif->abi != FFI_SYSV)
    return FFI_BAD_ABI;

  tramp = (unsigned int *) &closure->tramp[0];
  /* Since ffi_closure is an aligned object, the ffi trampoline is
     called as an SHcompact code.  Sigh.
     SHcompact part:
     mova @(1,pc),r0; add #1,r0; jmp @r0; nop;
     SHmedia part:
     movi fnaddr >> 16,r1; shori fnaddr,r1; ptabs/l r1,tr0
     movi cxt >> 16,r1; shori cxt,r1; blink tr0,r63  */
#ifdef __LITTLE_ENDIAN__
  tramp[0] = 0x7001c701;
  tramp[1] = 0x0009402b;
#else
  tramp[0] = 0xc7017001;
  tramp[1] = 0x402b0009;
#endif
  tramp[2] = 0xcc000010 | (((UINT32) ffi_closure_SYSV) >> 16) << 10;
  tramp[3] = 0xc8000010 | (((UINT32) ffi_closure_SYSV) & 0xffff) << 10;
  tramp[4] = 0x6bf10600;
  tramp[5] = 0xcc000010 | (((UINT32) codeloc) >> 16) << 10;
  tramp[6] = 0xc8000010 | (((UINT32) codeloc) & 0xffff) << 10;
  tramp[7] = 0x4401fff0;

  closure->cif = cif;
  closure->fun = fun;
  closure->user_data = user_data;

  /* Flush the icache.  */
  asm volatile ("ocbwb %0,0; synco; icbi %1,0; synci" : : "r" (tramp),
		"r"(codeloc));

  return FFI_OK;
}

/* Basically the trampoline invokes ffi_closure_SYSV, and on 
 * entry, r3 holds the address of the closure.
 * After storing the registers that could possibly contain
 * parameters to be passed into the stack frame and setting
 * up space for a return value, ffi_closure_SYSV invokes the 
 * following helper function to do most of the work.
 */

int
ffi_closure_helper_SYSV (ffi_closure *closure, UINT64 *rvalue, 
			 UINT64 *pgr, UINT64 *pfr, UINT64 *pst)
{
  void **avalue;
  ffi_type **p_arg;
  int i, avn;
  int greg, freg;
  ffi_cif *cif;
  //int fpair = -1;

  cif = closure->cif;
  avalue = alloca (cif->nargs * sizeof (void *));

  /* Copy the caller's structure return value address so that the closure
     returns the data directly to the caller.  */
  if (return_type (cif->rtype) == FFI_TYPE_STRUCT)
    {
      rvalue = (UINT64 *) *pgr;
      greg = 1;
    }
  else
    greg = 0;

  freg = 0;
  cif = closure->cif;
  avn = cif->nargs;

  /* Grab the addresses of the arguments from the stack frame.  */
  for (i = 0, p_arg = cif->arg_types; i < avn; i++, p_arg++)
    {
      size_t z;
      void *p;

      z = (*p_arg)->size;
      if (z < sizeof (UINT32))
	{
	  p = pgr + greg++;

	  switch ((*p_arg)->type)
	    {
	    case FFI_TYPE_SINT8:
	    case FFI_TYPE_UINT8:
	    case FFI_TYPE_SINT16:
	    case FFI_TYPE_UINT16:
	    case FFI_TYPE_STRUCT:
#ifdef __LITTLE_ENDIAN__
	      avalue[i] = p;
#else
	      avalue[i] = ((char *) p) + sizeof (UINT32) - z;
#endif
	      break;

	    default:
	      FFI_ASSERT(0);
	    }
	}
      else if (z == sizeof (UINT32))
	{
	  if ((*p_arg)->type == FFI_TYPE_FLOAT)
	    {
	      if (freg < NFREGARG - 1)
		{
		  if (fpair >= 0)
		    {
		      avalue[i] = (UINT32 *) pfr + fpair;
		      fpair = -1;
		    }
		  else
		    {
#ifdef __LITTLE_ENDIAN__
		      fpair = freg;
		      avalue[i] = (UINT32 *) pfr + (1 ^ freg);
#else
		      fpair = 1 ^ freg;
		      avalue[i] = (UINT32 *) pfr + freg;
#endif
		      freg += 2;
		    }
		}
	      else
#ifdef __LITTLE_ENDIAN__
		avalue[i] = pgr + greg;
#else
		avalue[i] = (UINT32 *) (pgr + greg) + 1;
#endif
	    }
	  else
#ifdef __LITTLE_ENDIAN__
	    avalue[i] = pgr + greg;
#else
	    avalue[i] = (UINT32 *) (pgr + greg) + 1;
#endif
	  greg++;
	}
      else if ((*p_arg)->type == FFI_TYPE_DOUBLE)
	{
	  if (freg + 1 >= NFREGARG)
	    avalue[i] = pgr + greg;
	  else
	    {
	      avalue[i] = pfr + (freg >> 1);
	      freg += 2;
	    }
	  greg++;
	}
      else
	{
	  int n = (z + sizeof (UINT64) - 1) / sizeof (UINT64);

	  avalue[i] = pgr + greg;
	  greg += n;
	}
    }

  (closure->fun) (cif, rvalue, avalue, closure->user_data);

  /* Tell ffi_closure_SYSV how to perform return type promotions.  */
  return return_type (cif->rtype);
}
#endif /* FFI_CLOSURES */
/* vim: set ts=4 sw=4 et ai: */
