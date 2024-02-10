#ifndef BASE_MATH_H
#define BASE_MATH_H

#define Min(A,B) ((A>B)?(B):(A))
#define Max(A,B) ((A>B)?(A):(B))
#define ClampTop(A, B) Min(A,B)
#define ClampBottom(A, B) Max(A,B)

typedef union Rng1U64 Rng1U64;
union Rng1U64
{
  struct
  {
    U64 min;
    U64 max;
  };
  U64 v[2];
};

internal Rng1U64 rng_1u64(U64 min, U64 max);
 
#endif