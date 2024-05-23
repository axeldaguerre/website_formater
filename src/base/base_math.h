#ifndef BASE_MATH_H
#define BASE_MATH_H

#define Min(A,B) ((A>B)?(B):(A))
#define Max(A,B) ((A>B)?(A):(B))
#define ClampTop(A, B) Min(A,B)
#define ClampBottom(A, B) Max(A,B)


typedef union Vec2F32 Vec2F32;
union Vec2F32
{
  struct
  {
    F32 x;
    F32 y;
  };
  F32 v[2];
};

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

typedef union Rng2F32 Rng2F32;
union Rng2F32
{
  struct
  {
    Vec2F32 min;
    Vec2F32 max;
  };
  struct
  {
    Vec2F32 p0;
    Vec2F32 p1;
  };
  struct
  {
    F32 x0;
    F32 y0;
    F32 x1;
    F32 y1;
  };
  Vec2F32 v[2];
};

internal U64 dim_1u64(Rng1U64 r)           {U64 c = r.max-r.min; return c;}
internal Rng1U64 rng_1u64(U64 min, U64 max){Rng1U64 r = {min, max}; if(r.min > r.max) { Swap(U64, r.min, r.max); } return r;}

#define r1u64(min, max) rng_1u64((min), (max))

#endif