#ifndef BASE_CORE_H
#define BASE_CORE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#if COMPILER_MSVC
#include <intrin.h>
#endif

////////////////////////////////
//~ rjf: Meow Hash

#include "third_party/meow_hash_x64_aesni.h"

////////////////////////////////
//~ rjf: Macros

//- rjf: linking keywords

#if LANG_CPP
# define no_name_mangle extern "C"
#else
# define no_name_mangle
#endif

#if BUILD_ROOT_LINK_MODE == BUILD_LINK_MODE_SOURCE
# define root_global no_name_mangle
# define internal function
#elif BUILD_ROOT_LINK_MODE == BUILD_LINK_MODE_STATIC
# define root_global no_name_mangle
# define internal no_name_mangle
#elif BUILD_ROOT_LINK_MODE == BUILD_LINK_MODE_DYNAMIC
# if BUILD_CORE
#  define root_global exported extern
#  define internal exported
# else
#  define root_global imported
#  define internal imported
# endif
#endif


#if OS_WINDOWS
# define exported no_name_mangle __declspec(dllexport)
#else
# define exported no_name_mangle
#endif

#if OS_WINDOWS
# define imported no_name_mangle __declspec(dllimport)
#else
# define imported no_name_mangle
#endif

//- rjf: keywords

#define global         static
#define function       static
#define local_persist  static
#define main_thread_only
#define fallthrough

#if ASAN_ENABLED && COMPILER_MSVC
# define no_asan __declspec(no_sanitize_address)
#elif ASAN_ENABLED && (COMPILER_CLANG || COMPILER_GCC)
# define no_asan __attribute__((no_sanitize("address")))
#endif
#if !defined(no_asan)
# define no_asan
#endif

#if COMPILER_MSVC
# define per_thread __declspec(thread)
#elif COMPILER_CLANG
# define per_thread __thread
#elif COMPILER_GCC
# define per_thread __thread
#endif

#if COMPILER_MSVC && COMPILER_MSVC_YEAR < 2015
# define inline_function static
#else
# define inline_function inline static
#endif

#if COMPILER_MSVC && COMPILER_MSVC_YEAR < 2015
# define this_function_name "unknown"
#else
# define this_function_name __func__
#endif

#if COMPILER_MSVC && COMPILER_MSVC_YEAR < 2015
# define snprintf _snprintf
#endif

#if OS_WINDOWS
# pragma section(".roglob", read)
# define read_only no_asan __declspec(allocate(".roglob"))
#else
# define read_only
#endif

#if LANG_CPP
# define zero_struct {}
#else
# define zero_struct {0}
#endif

//- rjf: atomics

#if OS_WINDOWS
# define AtomicIncEval64(ptr)                       InterlockedIncrement64(ptr)
# define AtomicDecEval64(ptr)                       InterlockedDecrement64(ptr)
# define AtomicAddEval64(ptr, val)                  InterlockedAdd64((ptr), (val))
# define AtomicEvalSet64(ptr, val)                  InterlockedExchange64((ptr), (val))
# define AtomicEval64(ptr)                          InterlockedAdd64((ptr), 0)
# define AtomicEvalCompareExchange64(ptr, val, cmp) InterlockedCompareExchange64((ptr), (val), (cmp))
#else
# error Atomics not implemented for this operating system.
#endif

//- rjf: intrinsics

#define ReadTimestampCounter() __rdtsc() 

//- rjf: memory copy/set operations

#define MemoryCopy memcpy
#define MemoryMove memmove
#define MemorySet  memset

#define MemoryCopyStruct(dst, src) do { Assert(sizeof(*(dst)) == sizeof(*(src))); MemoryCopy((dst), (src), sizeof(*(dst))); } while(0)
#define MemoryCopyArray(dst, src)  do { Assert(sizeof(dst) == sizeof(src)); MemoryCopy((dst), (src), sizeof(src)); }while(0)

#define MemoryZero(ptr, size) MemorySet((ptr), 0, (size))
#define MemoryZeroStruct(ptr) MemoryZero((ptr), sizeof(*(ptr)))
#define MemoryZeroArray(arr)  MemoryZero((arr), sizeof(arr))

//- rjf: quick sort wrapper

#define QuickSort(base, type, count, compare_function) qsort((base), (count), sizeof(type), (int(*)(const void *, const void *))(compare_function))

//- rjf: compiler, shut up! helpers

#define UnusedVariable(name) (void)name

//- rjf: integer/pointer/array/type manipulations

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))
#define IntFromPtr(p) (U64)(((U8*)p) - 0)
#define PtrFromInt(i) (void*)(((U8*)0) + i)
#define Member(type, member_name) ((type *)0)->member_name
#define OffsetOf(type, member_name) IntFromPtr(&Member(type, member_name))
#define BaseFromMember(type, member_name, ptr) (type *)((U8 *)(ptr) - OffsetOf(type, member_name))

#define Bytes(n)      (n)
#define Kilobytes(n)  (n << 10)
#define Megabytes(n)  (n << 20)
#define Gigabytes(n)  (((U64)n) << 30)
#define Terabytes(n)  (((U64)n) << 40)

#define Thousand(n) ((n)*1000)
#define Million(n)  ((n)*1000000)
#define Billion(n)  ((n)*1000000000LL)

#define Swap(type, a, b) do{ type _swapper_ = a; a = b; b = _swapper_; }while(0)

#define AbsoluteValueS32(x) (S32)abs((x))
#define AbsoluteValueS64(x) (S64)llabs((S64)(x))
#define AbsoluteValueU64(x) (U64)llabs((U64)(x))

//- rjf: linked list helpers

#define CheckNull(p) ((p)==0)
#define SetNull(p) ((p)=0)

#define QueuePush_NZ(f,l,n,next,zchk,zset) (zchk(f)?\
(((f)=(l)=(n)), zset((n)->next)):\
((l)->next=(n),(l)=(n),zset((n)->next)))
#define QueuePushFront_NZ(f,l,n,next,zchk,zset) (zchk(f) ? (((f) = (l) = (n)), zset((n)->next)) :\
((n)->next = (f)), ((f) = (n)))
#define QueuePop_NZ(f,l,next,zset) ((f)==(l)?\
(zset(f),zset(l)):\
((f)=(f)->next))
#define StackPush_N(f,n,next) ((n)->next=(f),(f)=(n))
#define StackPop_NZ(f,next,zchk) (zchk(f)?0:((f)=(f)->next))

#define DLLInsert_NPZ(f,l,p,n,next,prev,zchk,zset) \
(zchk(f) ? (((f) = (l) = (n)), zset((n)->next), zset((n)->prev)) :\
zchk(p) ? (zset((n)->prev), (n)->next = (f), (zchk(f) ? (0) : ((f)->prev = (n))), (f) = (n)) :\
((zchk((p)->next) ? (0) : (((p)->next->prev) = (n))), (n)->next = (p)->next, (n)->prev = (p), (p)->next = (n),\
((p) == (l) ? (l) = (n) : (0))))
#define DLLPushBack_NPZ(f,l,n,next,prev,zchk,zset) DLLInsert_NPZ(f,l,l,n,next,prev,zchk,zset)
#define DLLRemove_NPZ(f,l,n,next,prev,zchk,zset) (((f)==(n))?\
((f)=(f)->next, (zchk(f) ? (zset(l)) : zset((f)->prev))):\
((l)==(n))?\
((l)=(l)->prev, (zchk(l) ? (zset(f)) : zset((l)->next))):\
((zchk((n)->next) ? (0) : ((n)->next->prev=(n)->prev)),\
(zchk((n)->prev) ? (0) : ((n)->prev->next=(n)->next))))

#define QueuePush(f,l,n)         QueuePush_NZ(f,l,n,next,CheckNull,SetNull)
#define QueuePushFront(f,l,n)    QueuePushFront_NZ(f,l,n,next,CheckNull,SetNull)
#define QueuePop(f,l)            QueuePop_NZ(f,l,next,SetNull)
#define StackPush(f,n)           StackPush_N(f,n,next)
#define StackPop(f)              StackPop_NZ(f,next,CheckNull)
#define DLLPushBack(f,l,n)       DLLPushBack_NPZ(f,l,n,next,prev,CheckNull,SetNull)
#define DLLPushFront(f,l,n)      DLLPushBack_NPZ(l,f,n,prev,next,CheckNull,SetNull)
#define DLLInsert(f,l,p,n)       DLLInsert_NPZ(f,l,p,n,next,prev,CheckNull,SetNull)
#define DLLRemove(f,l,n)         DLLRemove_NPZ(f,l,n,next,prev,CheckNull,SetNull)

//- rjf: clamps, mins, maxes

#define Min(a, b) (((a)<(b)) ? (a) : (b))
#define Max(a, b) (((a)>(b)) ? (a) : (b))
#define ClampTop(x, a) Min(x,a)
#define ClampBot(a, x) Max(a,x)
#define Clamp(a, x, b) (((a)>(x))?(a):((b)<(x))?(b):(x))

//- rjf: loop helpers

#define EachElement(arr, it) U64 it = 0; it < ArrayCount(arr); it += 1
#define EachEnumVal(type, it) type it = (type)0; it < type##_COUNT; it = (type)(it+1)
#define EachNonzeroEnumVal(type, it) type it = (type)1; it < type##_COUNT; it = (type)(it+1)

//- rjf: defer-loop

#define DeferLoop(start, end) for(int _i_ = ((start), 0); _i_ == 0; _i_ += 1, (end))
#define DeferLoopChecked(begin, end) for(int _i_ = 2 * !(begin); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))

//- rjf: property array helpers

#define PropArrayAdd(arr, prop)       ((arr)[(prop) / (sizeof((arr)[0])*8)] |=  (((U64)1) << ((prop) % (sizeof((arr)[0])*8))))
#define PropArrayRemove(arr, prop)    ((arr)[(prop) / (sizeof((arr)[0])*8)] &= ~(((U64)1) << ((prop) % (sizeof((arr)[0])*8))))
#define PropArrayGet(arr, prop)    (!!((arr)[(prop) / (sizeof((arr)[0])*8)] &   (((U64)1) << ((prop) % (sizeof((arr)[0])*8)))))

////////////////////////////////
//~ rjf: Base-Types

typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;
typedef float    F32;
typedef double   F64;
typedef void VoidFunction(void);
typedef struct U128 U128;
struct U128 {U64 u64[2];};

inline_function U128 U128Zero(void) {U128 v = {0}; return v;}
inline_function B32 U128Match(U128 a, U128 b) {return a.u64[0] == b.u64[0] && a.u64[1] == b.u64[1];}

////////////////////////////////
//~ rjf: Limits

read_only root_global U8 U8Max = 0xFF;
read_only root_global U8 U8Min = 0;

read_only root_global U16 U16Max = 0xFFFF;
read_only root_global U16 U16Min = 0;

read_only root_global U32 U32Max = 0xFFFFFFFF;
read_only root_global U32 U32Min = 0;

read_only root_global U64 U64Max = 0xFFFFFFFFFFFFFFFF;
read_only root_global U64 U64Min = 0;

read_only root_global S8 S8Max = 0x7F;
read_only root_global S8 S8Min = -1 - 0x7F;

read_only root_global S16 S16Max = 0x7FFF;
read_only root_global S16 S16Min = -1 - 0x7FFF;

read_only root_global S32 S32Max = 0x7FFFFFFF;
read_only root_global S32 S32Min = -1 - 0x7FFFFFFF;

read_only root_global S64 S64Max = 0x7FFFFFFFFFFFFFFF;
read_only root_global S64 S64Min = -1 - 0x7FFFFFFFFFFFFFFF;

////////////////////////////////
//~ rjf: Constants

read_only root_global U32 SignF32 = 0x80000000;
read_only root_global U32 ExponentF32 = 0x7F800000;
read_only root_global U32 MantissaF32 = 0x7FFFFF;

read_only root_global F32 F32Max = 3.402823e+38f;
read_only root_global F32 F32Min = -3.402823e+38f;
read_only root_global F32 F32SmallestPositive = 1.1754943508e-38;
read_only root_global F32 F32Epsilon = 5.96046448e-8;

read_only root_global U64 SignF64 = 0x8000000000000000ull;
read_only root_global U64 ExponentF64 = 0x7FF0000000000000ull;
read_only root_global U64 MantissaF64 = 0xFFFFFFFFFFFFFull;

// TODO(rjf): // TODO(rjf): // TODO(rjf)
// read_only root_global F64 F64Max = ???;
// read_only root_global F64 F64Min = ???;
// read_only root_global F64 F64SmallestPositive = ???;
// read_only root_global F64 F64Epsilon = ???;

////////////////////////////////
//~ rjf: Base Enums

typedef enum Side
{
 Side_Invalid = -1,
 Side_Min,
 Side_Max,
 Side_COUNT
}
Side;

typedef enum Axis2
{
 Axis2_Invalid = -1,
 Axis2_X,
 Axis2_Y,
 Axis2_COUNT
}
Axis2;
#define Axis2_Flip(a) ((Axis2)(!(a)))

typedef enum Axis3
{
 Axis3_Invalid = -1,
 Axis3_X,
 Axis3_Y,
 Axis3_Z,
 Axis3_COUNT
}
Axis3;

typedef enum Axis4
{
 Axis4_Invalid = -1,
 Axis4_X,
 Axis4_Y,
 Axis4_Z,
 Axis4_W,
 Axis4_COUNT
}
Axis4;

typedef enum Corner
{
 Corner_Invalid = -1,
 Corner_00,
 Corner_01,
 Corner_10,
 Corner_11,
 Corner_COUNT
}
Corner;

typedef enum Dir2
{
 Dir2_Left,
 Dir2_Up,
 Dir2_Right,
 Dir2_Down,
 Dir2_COUNT
}
Dir2;

typedef enum Comparison
{
 Comparison_Null,
 Comparison_EqualTo,
 Comparison_NotEqualTo,
 Comparison_LessThan,
 Comparison_LessThanOrEqualTo,
 Comparison_GreaterThan,
 Comparison_GreaterThanOrEqualTo,
 Comparison_COUNT
}
Comparison;

////////////////////////////////
//~ rjf: Member Offset Helper

typedef struct MemberOffset MemberOffset;
struct MemberOffset
{
 U64 v;
};

#define MemberOff(S, member) (MemberOffset){OffsetOf(S, member)}
#define MemberOffLit(S, member) {OffsetOf(S, member)}
#define MemberFromOff(ptr, type, memoff) (*(type *)((U8 *)ptr + memoff.v))

////////////////////////////////
//~ rjf: Text Coordinates

typedef struct TxtPt TxtPt;
struct TxtPt
{
 S64 line;
 S64 column;
};

typedef struct TxtRng TxtRng;
struct TxtRng
{
 TxtPt min;
 TxtPt max;
};

inline_function TxtPt
TxtPtMake(S64 line, S64 column)
{
 TxtPt result = {line, column};
 return result;
}

inline_function B32
TxtPtMatch(TxtPt a, TxtPt b)
{
 return a.line == b.line && a.column == b.column;
}

inline_function B32
TxtPtLessThan(TxtPt a, TxtPt b)
{
 B32 result = (a.line < b.line ? 1 :
               b.line < a.line ? 0 :
               (a.column < b.column));
 return result;
}

inline_function TxtPt
TxtPtMin(TxtPt a, TxtPt b)
{
 TxtPt c = a;
 if(TxtPtLessThan(b, a))
 {
  c = b;
 }
 return c;
}

inline_function TxtPt
TxtPtMax(TxtPt a, TxtPt b)
{
 TxtPt c = a;
 if(TxtPtLessThan(a, b))
 {
  c = b;
 }
 return c;
}

inline_function TxtRng
TxtRngMake(TxtPt min, TxtPt max)
{
 TxtRng rng = {0};
 rng.min = min;
 rng.max = max;
 if(TxtPtLessThan(rng.max, rng.min))
 {
  Swap(TxtPt, rng.min, rng.max);
 }
 return rng;
}

////////////////////////////////
//~ rjf: Date/Time

typedef struct DateTime DateTime;
struct DateTime
{
 U16 year;
 U8 month;           // [1,  12]
 U8 day_of_week;     // [0,   6]
 U8 day;             // [1,  31]
 U8 hour;            // [0,  23]
 U8 minute;          // [0,  59]
 U8 second;          // [0,  59]
 U16 milliseconds;   // [0, 999]
};

inline_function B32
DateTimeMatch(DateTime a, DateTime b)
{
 return (a.year == b.year &&
         a.month == b.month &&
         a.day_of_week == b.day_of_week &&
         a.day == b.day &&
         a.hour == b.hour &&
         a.minute == b.minute &&
         a.second == b.second &&
         a.milliseconds == b.milliseconds);
}

inline_function B32
DateTimeLessThan(DateTime a, DateTime b)
{
 B32 result = 0;
 if(0){}
 else if(a.year < b.year) { result = 1; }
 else if(a.year > b.year) { result = 0; }
 else if(a.month < b.month) { result = 1; }
 else if(a.month > b.month) { result = 0; }
 else if(a.day < b.day) { result = 1; }
 else if(a.day > b.day) { result = 0; }
 else if(a.hour < b.hour) { result = 1; }
 else if(a.hour > b.hour) { result = 0; }
 else if(a.minute < b.minute) { result = 1; }
 else if(a.minute > b.minute) { result = 0; }
 else if(a.second < b.second) { result = 1; }
 else if(a.second > b.second) { result = 0; }
 else if(a.milliseconds < b.milliseconds) { result = 1; }
 else if(a.milliseconds > b.milliseconds) { result = 0; }
 return result;
}

////////////////////////////////
//~ rjf: Assertions

#if OS_WINDOWS
#define BreakDebugger() __debugbreak()
#else
#define BreakDebugger() (*(volatile int *)0 = 0)
#endif

#undef Assert
#define Assert(b) do { if(!(b)) { BreakDebugger(); } } while(0)
#if BUILD_CORE
#define StaticAssert(c, label) U8 static_assert_##label[(c)?(1):(-1)]
#else
#define StaticAssert(c, label)
#endif
#define NotImplemented Assert(!"Not Implemented")
#define InvalidPath Assert(!"Invalid Path")

////////////////////////////////
//~ rjf: Bit Patterns

internal U32 UpToPow2_32(U32 x);
internal U64 UpToPow2_64(U64 x);
internal U32 SearchFirstOneBit_32_BinarySearch(U32 x);
internal U32 SearchFirstOneBit_64_BinarySearch(U64 x);

inline_function F32
F32Inf(void)
{
 union { U32 u; F32 f; } x;
 x.u = 0x7F800000;
 return x.f;
}

inline_function F32
F32NegInf(void)
{
 union { U32 u; F32 f; } x;
 x.u = 0xFF800000;
 return x.f;
}

inline_function B32
F32IsNan(F32 f)
{
 union { U32 u; F32 f; } x;
 x.f = f;
 return ((x.u & ExponentF32) == ExponentF32) && ((x.u & MantissaF32) != 0);
}

inline_function B32
F32IsDenorm(F32 f)
{
 union { U32 u; F32 f; } x;
 x.f = f;
 return ((x.u & ExponentF32) == 0) && ((x.u & MantissaF32) != 0);
}

inline_function F32
AbsoluteValueF32(F32 f)
{
 union { U32 u; F32 f; } x;
 x.f = f;
 x.u = x.u & ~SignF32;
 return x.f;
}

inline_function F64
AbsoluteValueF64(F64 f)
{
 union { U64 u; F64 f; } x;
 x.f = f;
 x.u = x.u & ~SignF64;
 return x.f;
}

inline_function F32
SignFromF32(F32 f)
{
 return f < 0.f ? -1.f : +1.f;
}

inline_function F64
SignFromF64(F64 f)
{
 return f < 0.0 ? -1.0 : +1.0;
}

////////////////////////////////
//~ rjf: Comparisons

inline_function B32
Compare_U64(U64 a, U64 b, Comparison comparison)
{
 B32 result = 0;
 switch(comparison)
 {
  default: break;
  case Comparison_EqualTo:               result = (a == b); break;
  case Comparison_NotEqualTo:            result = (a != b); break;
  case Comparison_LessThan:              result = (a <  b); break;
  case Comparison_LessThanOrEqualTo:     result = (a <= b); break;
  case Comparison_GreaterThan:           result = (a >  b); break;
  case Comparison_GreaterThanOrEqualTo:  result = (a >= b); break;
 }
 return result;
}

////////////////////////////////
//~ rjf: Byte Swaps

inline_function U64
ByteSwapU64(U64 v)
{
 U64 result = 0;
 result |= (v&0xff00000000000000ull)>>56;
 result |= (v&0x00ff000000000000ull)>>40;
 result |= (v&0x0000ff0000000000ull)>>24;
 result |= (v&0x000000ff00000000ull)>>8;
 result |= (v&0x00000000ff000000ull)<<8;
 result |= (v&0x0000000000ff0000ull)<<24;
 result |= (v&0x000000000000ff00ull)<<40;
 result |= (v&0x00000000000000ffull)<<56;
 return result;
}

inline_function U32
ByteSwapU32(U32 v)
{
 U32 result = 0;
 result |= (v&0xff000000)>>24;
 result |= (v&0x00ff0000)>>8;
 result |= (v&0x0000ff00)<<8;
 result |= (v&0x000000ff)<<24;
 return result;
}

inline_function U16
ByteSwapU16(U16 v)
{
 U16 result = 0;
 result |= (v&0xff00)>>8;
 result |= (v&0x00ff)<<8;
 return result;
}

#endif // BASE_CORE_H
