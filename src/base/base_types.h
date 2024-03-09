#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include<stdint.h>

#define internal static 
#define global   static 

typedef uint8_t    U8;
typedef uint16_t   U16;
typedef uint32_t   U32;
typedef uint64_t   U64;
typedef int32_t    S32;
typedef S32        B32;

#define KB(n)  (((U64)(n)) << 10)
#define MB(n)  (((U64)(n)) << 20)
#define GB(n)  (((U64)(n)) << 30)
#define TB(n)  (((U64)(n)) << 40)

#define Thousand(n)   ((n)*1000)
#define Million(n)    ((n)*1000000)
#define Billion(n)    ((n)*1000000000)

#define Stmnt(S) do{ S }while(0)

global U64 max_U64 = 0xffffffffffffffffull;
global U32 max_U32 = 0xffffffff;
global U16 max_U16 = 0xffff;
global U8  max_U8  = 0xff;

global const U32 bitmask3  = 0x00000007;
global const U32 bitmask4  = 0x0000000F;
global const U32 bitmask5  = 0x0000001F;
global const U32 bitmask6  = 0x0000003F;
global const U32 bitmask10 = 0x0000003FF;

#define MemoryCopy(dst, src, size)    memmove((dst), (src), (size))
#define MemoryZero(s,z) memset((s),0,(z))

#define MAX_PATH_SIZE 260

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define AssertAlways(Expression) if(!(Expression)) {*(int *)0 = 0;}

#define Swap(T, a, b) Stmnt(T t__ = a; a = b; b = t__;)

internal U32 u32_from_u64(U64 x);
internal U64 u64_from_high_low_u32(U32 high, U32 low);

#if COMPILER_CL || (COMPILER_CLANG && OS_WINDOWS)
# pragma section(".rdata$", read)
# define read_only __declspec(allocate(".rdata$"))
#elif (COMPILER_CLANG && OS_LINUX)
# define read_only __attribute__((section(".rodata")))
#else
// NOTE(rjf): I don't know of a useful way to do this in GCC land.
// __attribute__((section(".rodata"))) looked promising, but it introduces a
// strange warning about malformed section attributes, and it doesn't look
// like writing to that section reliably produces access violations, strangely
// enough. (It does on Clang)
# define read_only
#endif

#endif