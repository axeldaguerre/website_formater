#ifndef BASE_MATH_H
#define BASE_MATH_H

////////////////////////////////
//~ rjf: Macros

//- rjf: floating point operations

#define FloorF32(f)        floorf(f)
#define CeilF32(f)         ceilf(f)
#define RoundF32(f)        roundf(f)
#define ModF32(x, y)       fmodf(x, y)
#define DegFromRadF32(v)   ((180.f/PiF32) * (v))
#define RadFromDegF32(v)   ((PiF32/180.f) * (v))
#define TurnsFromDegF32(v) ((v) / 360.f)
#define TurnsFromRadF32(v) ((v) / (2*PiF32))
#define DegFromTurnsF32(v) ((v) * 360.f)
#define RadFromTurnsF32(v) ((v) * (2*PiF32))
#define SquareRootF32(x)   sqrtf(x)
#define SinF32(v)          sinf(RadFromTurnsF32(v))
#define CosF32(v)          cosf(RadFromTurnsF32(v))
#define TanF32(v)          tanf(RadFromTurnsF32(v))
#define Sin2F32(v)         PowF32(Sin(v), 2)
#define Cos2F32(v)         PowF32(Cos(v), 2)
#define PowF32(b, exp)     powf(b, exp)
#define Log10F32(v)        log10f(v)
#define LogEF32(v)         logf(v)

#define FloorF64(f)        floor(f)
#define CeilF64(f)         ceil(f)
#define RoundF64(f)        round(f)
#define ModF64(x, y)       fmod(x, y)
#define DegFromRadF64(v)   ((180.0/PiF64) * (v))
#define RadFromDegF64(v)   ((PiF64/180.0) * (v))
#define TurnsFromDegF64(v) ((v) / 360.0)
#define TurnsFromRadF64(v) ((v) / (2*PiF64))
#define DegFromTurnsF64(v) ((v) * 360.0)
#define RadFromTurnsF64(v) ((v) * (2*PiF64))
#define SquareRootF64(x)   sqrt(x)
#define SinF64(v)          sin(RadFromTurnsF64(v))
#define CosF64(v)          cos(RadFromTurnsF64(v))
#define TanF64(v)          tan(RadFromTurnsF64(v))
#define Sin2F64(v)         PowF64(Sin(v), 2)
#define Cos2F64(v)         PowF64(Cos(v), 2)
#define PowF64(b, exp)     pow(b, exp)
#define Log10F64(v)        log10(v)
#define LogEF64(v)         log(v)

//- rjf: omitted base type aliases

#define Floor(f)           FloorF32(f)
#define Ceil(f)            CeilF32(f)
#define Mod(x, y)          ModF32(x, y)
#define DegFromRad(v)      DegFromRadF32(v)
#define RadFromDeg(v)      RadFromDegF32(v)
#define TurnsFromDeg(v)    TurnsFromDegF32(v)
#define TurnsFromRad(v)    TurnsFromRadF32(v)
#define DegFromTurns(v)    ((v) * 360.0)
#define RadFromTurns(v)    ((v) * (2*PiF64))
#define SquareRoot(x)      SquareRootF32(x)
#define Sin(v)             SinF32(v)
#define Cos(v)             CosF32(v)
#define Tan(v)             TanF32(v)
#define Sin2(v)            Sin2F32(v)
#define Cos2(v)            Cos2F32(v)
#define Pow(b, exp)        PowF32(b, exp)
#define Log10(v)           Log10F32(v)
#define LogE(v)            LogEF32(v)

////////////////////////////////
//~ rjf: Constants

#define InfinityF32                 ((F32)INFINITY)

#define PiF32                       (3.1415926535897f)
#define OneOverSquareRootOfTwoPiF32 (0.3989422804f)
#define EulersNumberF32             (2.7182818284590452353602874713527f)

#define PiF64                       (3.1415926535897)
#define OneOverSquareRootOfTwoPiF64 (0.3989422804)
#define EulersNumberF64             (2.7182818284590452353602874713527)

////////////////////////////////
//~ rjf: Vector Types

//- rjf: 2-vectors

typedef union Vec2F32 Vec2F32;
union Vec2F32
{
 struct
 {
  F32 x;
  F32 y;
 };
 F32 elements[2];
 F32 v[2];
};

typedef union Vec2F64 Vec2F64;
union Vec2F64
{
 struct
 {
  F64 x;
  F64 y;
 };
 F64 elements[2];
 F64 v[2];
};

typedef union Vec2S32 Vec2S32;
union Vec2S32
{
 struct
 {
  S32 x;
  S32 y;
 };
 S32 elements[2];
 S32 v[2];
};

typedef union Vec2S64 Vec2S64;
union Vec2S64
{
 struct
 {
  S64 x;
  S64 y;
 };
 S64 elements[2];
 S64 v[2];
};

//- rjf: 3-vectors

typedef union Vec3F32 Vec3F32;
union Vec3F32
{
 struct
 {
  F32 x;
  F32 y;
  F32 z;
 };
 
 struct
 {
  F32 r;
  F32 g;
  F32 b;
 };
 
 struct
 {
  F32 red;
  F32 green;
  F32 blue;
 };
 
 struct
 {
  Vec2F32 xy;
  F32 _z1;
 };
 
 struct
 {
  F32 _x1;
  Vec2F32 yz;
 };
 
 F32 elements[3];
 F32 v[3];
};

typedef union Vec3F64 Vec3F64;
union Vec3F64
{
 struct
 {
  F64 x;
  F64 y;
  F64 z;
 };
 
 struct
 {
  F64 r;
  F64 g;
  F64 b;
 };
 
 struct
 {
  F64 red;
  F64 green;
  F64 blue;
 };
 
 struct
 {
  Vec2F64 xy;
  F64 _z1;
 };
 
 struct
 {
  F64 _x1;
  Vec2F64 yz;
 };
 
 F64 elements[3];
 F64 v[3];
};

typedef union Vec3S32 Vec3S32;
union Vec3S32
{
 struct
 {
  S32 x;
  S32 y;
  S32 z;
 };
 
 struct
 {
  S32 r;
  S32 g;
  S32 b;
 };
 
 S32 elements[3];
 S32 v[3];
};

typedef union Vec3S64 Vec3S64;
union Vec3S64
{
 struct
 {
  S64 x;
  S64 y;
  S64 z;
 };
 
 struct
 {
  S64 r;
  S64 g;
  S64 b;
 };
 
 S64 elements[3];
 S64 v[3];
};

//- rjf: 4-vectors

typedef union Vec4F32 Vec4F32;
union Vec4F32
{
 struct
 {
  F32 x;
  F32 y;
  F32 z;
  F32 w;
 };
 
 struct
 {
  Vec2F32 xy;
  Vec2F32 zw;
 };
 
 struct
 {
  Vec3F32 xyz;
  F32 _w1;
 };
 
 struct
 {
  F32 _x1;
  Vec3F32 yzw;
 };
 
 struct
 {
  Vec3F32 rgb;
  F32 _w2;
 };
 
 struct
 {
  Vec3F32 gba;
  F32 _w3;
 };
 
 struct
 {
  F32 r;
  F32 g;
  F32 b;
  F32 a;
 };
 
 struct
 {
  F32 red;
  F32 green;
  F32 blue;
  F32 alpha;
 };
 
 struct
 {
  F32 left;
  F32 up;
  F32 right;
  F32 down;
 };
 
 F32 elements[4];
 F32 v[4];
 struct
 {
  F32 padding_[2];
  F32 dim[2];
 };
};

typedef union Vec4F64 Vec4F64;
union Vec4F64
{
 struct
 {
  F64 x;
  F64 y;
  F64 z;
  F64 w;
 };
 
 struct
 {
  Vec2F64 xy;
  Vec2F64 zw;
 };
 
 struct
 {
  Vec3F64 xyz;
  F64 _w1;
 };
 
 struct
 {
  F64 _x1;
  Vec3F64 yzw;
 };
 
 struct
 {
  Vec3F64 rgb;
  F64 _w2;
 };
 
 struct
 {
  Vec3F64 gba;
  F64 _w3;
 };
 
 struct
 {
  F64 r;
  F64 g;
  F64 b;
  F64 a;
 };
 
 struct
 {
  F64 red;
  F64 green;
  F64 blue;
  F64 alpha;
 };
 
 struct
 {
  F64 left;
  F64 up;
  F64 right;
  F64 down;
 };
 
 F64 elements[4];
 F64 v[4];
 struct
 {
  F64 padding_[2];
  F64 dim[2];
 };
};

typedef union Vec4S32 Vec4S32;
union Vec4S32
{
 struct
 {
  S32 x;
  S32 y;
  S32 z;
  S32 w;
 };
 
 struct
 {
  Vec2S32 xy;
  Vec2S32 zw;
 };
 
 struct
 {
  Vec3S32 xyz;
  S32 _w1;
 };
 
 struct
 {
  S32 _x1;
  Vec3S32 yzw;
 };
 
 struct
 {
  Vec3S32 rgb;
  S32 _w2;
 };
 
 struct
 {
  Vec3S32 gba;
  S32 _w3;
 };
 
 struct
 {
  S32 r;
  S32 g;
  S32 b;
  S32 a;
 };
 
 struct
 {
  S32 red;
  S32 green;
  S32 blue;
  S32 alpha;
 };
 
 struct
 {
  S32 left;
  S32 up;
  S32 right;
  S32 down;
 };
 
 struct
 {
  S32 padding_[2];
  S32 dim[2];
 };
 
 S32 elements[4];
 S32 v[4];
};

typedef union Vec4S64 Vec4S64;
union Vec4S64
{
 struct
 {
  S64 x;
  S64 y;
  S64 z;
  S64 w;
 };
 
 struct
 {
  Vec2S64 xy;
  Vec2S64 zw;
 };
 
 struct
 {
  Vec3S64 xyz;
  S64 _w1;
 };
 
 struct
 {
  S64 _x1;
  Vec3S64 yzw;
 };
 
 struct
 {
  Vec3S64 rgb;
  S64 _w2;
 };
 
 struct
 {
  Vec3S64 gba;
  S64 _w3;
 };
 
 struct
 {
  S64 r;
  S64 g;
  S64 b;
  S64 a;
 };
 
 struct
 {
  S64 red;
  S64 green;
  S64 blue;
  S64 alpha;
 };
 
 struct
 {
  S64 left;
  S64 up;
  S64 right;
  S64 down;
 };
 
 struct
 {
  S64 padding_[2];
  S64 dim[2];
 };
 
 S64 elements[4];
 S64 v[4];
};

#define Vec2F32FromVec(v) V2F32((F32)(v).x, (F32)(v).y)
#define Vec2F64FromVec(v) V2F64((F64)(v).x, (F64)(v).y)
#define Vec2S32FromVec(v) V2S32((S32)(v).x, (S32)(v).y)
#define Vec2S64FromVec(v) V2S64((S64)(v).x, (S64)(v).y)
#define Vec3F32FromVec(v) V3F32((F32)(v).x, (F32)(v).y, (F32)(v).z)
#define Vec3F64FromVec(v) V3F64((F64)(v).x, (F64)(v).y, (F64)(v).z)
#define Vec3S32FromVec(v) V3S32((S32)(v).x, (S32)(v).y, (S32)(v).z)
#define Vec3S64FromVec(v) V3S64((S64)(v).x, (S64)(v).y, (S64)(v).z)
#define Vec4F32FromVec(v) V4F32((F32)(v).x, (F32)(v).y, (F32)(v).z, (F32)(v).w)
#define Vec4F64FromVec(v) V4F64((F64)(v).x, (F64)(v).y, (F64)(v).z, (F64)(v).w)
#define Vec4S32FromVec(v) V4S32((S32)(v).x, (S32)(v).y, (S32)(v).z, (S32)(v).w)
#define Vec4S64FromVec(v) V4S64((S64)(v).x, (S64)(v).y, (S64)(v).z, (S64)(v).w)

////////////////////////////////
//~ rjf: Matrix Types

typedef struct Mat3x3F32 Mat3x3F32;
struct Mat3x3F32
{
 F32 elements[3][3];
};

typedef struct Mat4x4F32 Mat4x4F32;
struct Mat4x4F32
{
 F32 elements[4][4];
};

typedef struct Mat3x3F64 Mat3x3F64;
struct Mat3x3F64
{
 F64 elements[3][3];
};

typedef struct Mat4x4F64 Mat4x4F64;
struct Mat4x4F64
{
 F64 elements[4][4];
};

////////////////////////////////
//~ rjf: Quaternion Types

typedef union QuatF32 QuatF32;
union QuatF32
{
 Vec4F32 xyzw;
 struct
 {
  Vec3F32 xyz;
  F32 w;
 };
 struct
 {
  Vec2F32 xy;
  Vec2F32 zw;
 };
 struct
 {
  F32 x;
  Vec3F32 yzw;
 };
 struct
 {
  F32 _x;
  F32 y;
  F32 z;
  F32 _w;
 };
 F32 elements[4];
};

////////////////////////////////
//~ rjf: Interval Types

//- rjf: 1D intervals

typedef union Rng1F32 Rng1F32;
union Rng1F32
{
 struct
 {
  F32 min;
  F32 max;
 };
 F32 v[2];
};

typedef union Rng1F64 Rng1F64;
union Rng1F64
{
 struct
 {
  F64 min;
  F64 max;
 };
 F64 v[2];
};

typedef union Rng1U32 Rng1U32;
union Rng1U32
{
 struct
 {
  U32 min;
  U32 max;
 };
 U32 v[2];
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

typedef union Rng1S32 Rng1S32;
union Rng1S32
{
 struct
 {
  S32 min;
  S32 max;
 };
 S32 v[2];
};

typedef union Rng1S64 Rng1S64;
union Rng1S64
{
 struct
 {
  S64 min;
  S64 max;
 };
 S64 v[2];
};

//- rjf: 2D intervals

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

typedef union Rng2F64 Rng2F64;
union Rng2F64
{
 struct
 {
  Vec2F64 min;
  Vec2F64 max;
 };
 struct
 {
  Vec2F64 p0;
  Vec2F64 p1;
 };
 struct
 {
  F64 x0;
  F64 y0;
  F64 x1;
  F64 y1;
 };
 Vec2F64 v[2];
};

typedef union Rng2S32 Rng2S32;
union Rng2S32
{
 struct
 {
  Vec2S32 min;
  Vec2S32 max;
 };
 struct
 {
  Vec2S32 p0;
  Vec2S32 p1;
 };
 struct
 {
  S32 x0;
  S32 y0;
  S32 x1;
  S32 y1;
 };
 Vec2S32 v[2];
};

typedef union Rng2S64 Rng2S64;
union Rng2S64
{
 struct
 {
  Vec2S64 min;
  Vec2S64 max;
 };
 struct
 {
  Vec2S64 p0;
  Vec2S64 p1;
 };
 struct
 {
  S64 x0;
  S64 y0;
  S64 x1;
  S64 y1;
 };
 Vec2S64 v[2];
};

//- rjf: 3D intervals

typedef union Rng3F32 Rng3F32;
union Rng3F32
{
 struct
 {
  Vec3F32 min;
  Vec3F32 max;
 };
 struct
 {
  Vec3F32 p0;
  Vec3F32 p1;
 };
 struct
 {
  F32 x0;
  F32 y0;
  F32 z0;
  F32 x1;
  F32 y1;
  F32 z1;
 };
 Vec3F32 v[2];
};

//- rjf: 1D interval list types

typedef struct Rng1U64Node Rng1U64Node;
struct Rng1U64Node
{
 Rng1U64Node *next;
 Rng1U64 v;
};

typedef struct Rng1U64List Rng1U64List;
struct Rng1U64List
{
 Rng1U64Node *first;
 Rng1U64Node *last;
 U64 count;
 U64 total_count;
};

////////////////////////////////
//~ rjf: Scalar Ops

internal F32 Mix1F32(F32 a, F32 b, F32 t);

////////////////////////////////
//~ rjf: Vector Ops

internal Vec2F32 V2F32(F32 x, F32 y);
internal Vec2F32 Add2F32(Vec2F32 a, Vec2F32 b);
internal Vec2F32 Sub2F32(Vec2F32 a, Vec2F32 b);
internal Vec2F32 Mul2F32(Vec2F32 a, Vec2F32 b);
internal Vec2F32 Div2F32(Vec2F32 a, Vec2F32 b);
internal Vec2F32 Scale2F32(Vec2F32 a, F32 scale);
internal F32 Dot2F32(Vec2F32 a, Vec2F32 b);
internal F32 LengthSquared2F32(Vec2F32 v);
internal F32 Length2F32(Vec2F32 v);
internal Vec2F32 Normalize2F32(Vec2F32 v);
internal Vec2F32 Mix2F32(Vec2F32 a, Vec2F32 b, F32 t);

internal Vec2F64 V2F64(F64 x, F64 y);
internal Vec2F64 Add2F64(Vec2F64 a, Vec2F64 b);
internal Vec2F64 Sub2F64(Vec2F64 a, Vec2F64 b);
internal Vec2F64 Mul2F64(Vec2F64 a, Vec2F64 b);
internal Vec2F64 Div2F64(Vec2F64 a, Vec2F64 b);
internal Vec2F64 Scale2F64(Vec2F64 a, F64 scale);
internal F64 Dot2F64(Vec2F64 a, Vec2F64 b);
internal F64 LengthSquared2F64(Vec2F64 v);
internal F64 Length2F64(Vec2F64 v);
internal Vec2F64 Normalize2F64(Vec2F64 v);
internal Vec2F64 Mix2F64(Vec2F64 a, Vec2F64 b, F64 t);

internal Vec2S32 V2S32(S32 x, S32 y);
internal Vec2S32 Add2S32(Vec2S32 a, Vec2S32 b);
internal Vec2S32 Sub2S32(Vec2S32 a, Vec2S32 b);

internal Vec2S64 V2S64(S64 x, S64 y);
internal Vec2S64 Add2S64(Vec2S64 a, Vec2S64 b);
internal Vec2S64 Sub2S64(Vec2S64 a, Vec2S64 b);

internal Vec3F32 V3F32(F32 x, F32 y, F32 z);
internal Vec3F32 Add3F32(Vec3F32 a, Vec3F32 b);
internal Vec3F32 Sub3F32(Vec3F32 a, Vec3F32 b);
internal Vec3F32 Mul3F32(Vec3F32 a, Vec3F32 b);
internal Vec3F32 Div3F32(Vec3F32 a, Vec3F32 b);
internal Vec3F32 Scale3F32(Vec3F32 a, F32 scale);
internal F32 Dot3F32(Vec3F32 a, Vec3F32 b);
internal F32 LengthSquared3F32(Vec3F32 v);
internal F32 Length3F32(Vec3F32 v);
internal Vec3F32 Normalize3F32(Vec3F32 v);
internal Vec3F32 Mix3F32(Vec3F32 a, Vec3F32 b, F32 t);
internal Vec3F32 Cross3F32(Vec3F32 a, Vec3F32 b);
internal Vec3F32 Transform3F32(Vec3F32 v, Mat3x3F32 m);

internal Vec3F64 V3F64(F64 x, F64 y, F64 z);
internal Vec3F64 Add3F64(Vec3F64 a, Vec3F64 b);
internal Vec3F64 Sub3F64(Vec3F64 a, Vec3F64 b);
internal Vec3F64 Mul3F64(Vec3F64 a, Vec3F64 b);
internal Vec3F64 Div3F64(Vec3F64 a, Vec3F64 b);
internal Vec3F64 Scale3F64(Vec3F64 a, F64 scale);
internal F64 Dot3F64(Vec3F64 a, Vec3F64 b);
internal F64 LengthSquared3F64(Vec3F64 v);
internal F64 Length3F64(Vec3F64 v);
internal Vec3F64 Normalize3F64(Vec3F64 v);
internal Vec3F64 Mix3F64(Vec3F64 a, Vec3F64 b, F64 t);
internal Vec3F64 Cross3F64(Vec3F64 a, Vec3F64 b);
internal Vec3F64 Transform3F64(Vec3F64 v, Mat3x3F64 m);

internal Vec3S32 V3S32(S32 x, S32 y, S32 z);

internal Vec3S64 V3S64(S64 x, S64 y, S64 z);

internal Vec4F32 V4F32(F32 x, F32 y, F32 z, F32 w);
internal Vec4F32 Add4F32(Vec4F32 a, Vec4F32 b);
internal Vec4F32 Sub4F32(Vec4F32 a, Vec4F32 b);
internal Vec4F32 Mul4F32(Vec4F32 a, Vec4F32 b);
internal Vec4F32 Div4F32(Vec4F32 a, Vec4F32 b);
internal Vec4F32 Scale4F32(Vec4F32 a, F32 scale);
internal F32 Dot4F32(Vec4F32 a, Vec4F32 b);
internal F32 LengthSquared4F32(Vec4F32 v);
internal F32 Length4F32(Vec4F32 v);
internal Vec4F32 Normalize4F32(Vec4F32 v);
internal Vec4F32 Mix4F32(Vec4F32 a, Vec4F32 b, F32 t);
internal Vec4F32 Transform4F32(Vec4F32 v, Mat4x4F32 m);

internal Vec4F64 V4F64(F64 x, F64 y, F64 z, F64 w);
internal Vec4F64 Add4F64(Vec4F64 a, Vec4F64 b);
internal Vec4F64 Sub4F64(Vec4F64 a, Vec4F64 b);
internal Vec4F64 Mul4F64(Vec4F64 a, Vec4F64 b);
internal Vec4F64 Div4F64(Vec4F64 a, Vec4F64 b);
internal Vec4F64 Scale4F64(Vec4F64 a, F64 scale);
internal F64 Dot4F64(Vec4F64 a, Vec4F64 b);
internal F64 LengthSquared4F64(Vec4F64 v);
internal F64 Length4F64(Vec4F64 v);
internal Vec4F64 Normalize4F64(Vec4F64 v);
internal Vec4F64 Mix4F64(Vec4F64 a, Vec4F64 b, F64 t);
internal Vec4F64 Transform4F64(Vec4F64 v, Mat4x4F64 m);

internal Vec4S32 V4S32(S32 x, S32 y, S32 z, S32 w);

internal Vec4S64 V4S64(S64 x, S64 y, S64 z, S64 w);

////////////////////////////////
//~ rjf: Matrix Constructors

internal Mat3x3F32 MakeMat3x3F32(F32 d);
internal Mat3x3F32 MakeTranslate3x3F32(Vec2F32 translation);
internal Mat3x3F32 MakeScale3x3F32(Vec2F32 scale);
internal Mat3x3F32 MakeRotate3x3F32(F32 turns);

internal Mat4x4F32 MakeMat4x4F32(F32 d);
internal Mat4x4F32 MakeTranslate4x4F32(Vec3F32 translation);
internal Mat4x4F32 MakeScale4x4F32(Vec3F32 scale);
internal Mat4x4F32 MakePerspective4x4F32(F32 fov, F32 aspect_ratio, F32 near_z, F32 far_z);
internal Mat4x4F32 MakeOrthographic4x4F32(F32 left, F32 right, F32 bottom, F32 top, F32 near_z, F32 far_z);
internal Mat4x4F32 MakeLookAt4x4F32(Vec3F32 eye, Vec3F32 center, Vec3F32 up);
internal Mat4x4F32 MakeRotate4x4F32(Vec3F32 axis, F32 turns);

////////////////////////////////
//~ rjf: Matrix Ops

internal Mat3x3F32 Mul3x3F32(Mat3x3F32 a, Mat3x3F32 b);
internal Mat3x3F32 Scale3x3F32(Mat3x3F32 m, F32 scale);
internal Mat4x4F32 Mul4x4F32(Mat4x4F32 a, Mat4x4F32 b);
internal Mat4x4F32 Scale4x4F32(Mat4x4F32 m, F32 scale);
internal Mat4x4F32 Inverse4x4F32(Mat4x4F32 m);
internal Mat4x4F32 RemoveRotation4x4F32(Mat4x4F32 mat);

////////////////////////////////
//~ rjf: Quaternion Constructors

internal QuatF32 MakeQuatF32(F32 x, F32 y, F32 z, F32 w);
internal QuatF32 QuatFromAxisAngleF32(Vec3F32 axis, F32 turns);

////////////////////////////////
//~ rjf: Quaternion Ops

internal QuatF32 AddQF32(QuatF32 a, QuatF32 b);
internal QuatF32 SubQF32(QuatF32 a, QuatF32 b);
internal QuatF32 MulQF32(QuatF32 a, QuatF32 b);
internal QuatF32 ScaleQF32(QuatF32 a, F32 scale);
internal QuatF32 NormalizeQF32(QuatF32 q);
internal QuatF32 MixQF32(QuatF32 a, QuatF32 b, F32 t);
internal F32 DotQF32(QuatF32 a, QuatF32 b);
internal Mat4x4F32 Mat4x4FromQuatF32(QuatF32 q);

////////////////////////////////
//~ rjf: Range Ops

internal Rng1F32 R1F32(F32 min, F32 max);
internal Rng1F32 Shift1F32(Rng1F32 r, F32 v);
internal Rng1F32 Pad1F32(Rng1F32 r, F32 x);
internal F32 Center1F32(Rng1F32 r);
internal B32 Contains1F32(Rng1F32 r, F32 v);
internal F32 Dim1F32(Rng1F32 r);
internal Rng1F32 Union1F32(Rng1F32 a, Rng1F32 b);
internal Rng1F32 Intersection1F32(Rng1F32 a, Rng1F32 b);
internal F32 Clamp1F32(Rng1F32 r, F32 v);

internal Rng1F64 R1F64(F64 min, F64 max);
internal Rng1F64 Shift1F64(Rng1F64 r, F64 v);
internal Rng1F64 Pad1F64(Rng1F64 r, F64 x);
internal F64 Center1F64(Rng1F64 r);
internal B32 Contains1F64(Rng1F64 r, F64 v);
internal F64 Dim1F64(Rng1F64 r);
internal Rng1F64 Union1F64(Rng1F64 a, Rng1F64 b);
internal Rng1F64 Intersection1F64(Rng1F64 a, Rng1F64 b);
internal F64 Clamp1F64(Rng1F64 r, F64 v);

internal Rng1S32 R1S32(S32 min, S32 max);
internal Rng1S32 Shift1S32(Rng1S32 r, S32 v);
internal Rng1S32 Pad1S32(Rng1S32 r, S32 x);
internal S32 Center1S32(Rng1S32 r);
internal B32 Contains1S32(Rng1S32 r, S32 v);
internal S32 Dim1S32(Rng1S32 r);
internal Rng1S32 Union1S32(Rng1S32 a, Rng1S32 b);
internal Rng1S32 Intersection1S32(Rng1S32 a, Rng1S32 b);
internal S32 Clamp1S32(Rng1S32 r, S32 v);

internal Rng1S64 R1S64(S64 min, S64 max);
internal Rng1S64 Shift1S64(Rng1S64 r, S64 v);
internal Rng1S64 Pad1S64(Rng1S64 r, S64 x);
internal S64 Center1S64(Rng1S64 r);
internal B32 Contains1S64(Rng1S64 r, S64 v);
internal S64 Dim1S64(Rng1S64 r);
internal Rng1S64 Union1S64(Rng1S64 a, Rng1S64 b);
internal Rng1S64 Intersection1S64(Rng1S64 a, Rng1S64 b);
internal S64 Clamp1S64(Rng1S64 r, S64 v);

internal Rng1U32 R1U32(U32 min, U32 max);
internal Rng1U32 Shift1U32(Rng1U32 r, U32 v);
internal Rng1U32 Pad1U32(Rng1U32 r, U32 x);
internal U32 Center1U32(Rng1U32 r);
internal B32 Contains1U32(Rng1U32 r, U32 v);
internal U32 Dim1U32(Rng1U32 r);
internal Rng1U32 Union1U32(Rng1U32 a, Rng1U32 b);
internal Rng1U32 Intersection1U32(Rng1U32 a, Rng1U32 b);
internal U32 Clamp1U32(Rng1U32 r, U32 v);

internal Rng1U64 R1U64(U64 min, U64 max);
internal Rng1U64 Shift1U64(Rng1U64 r, U64 v);
internal Rng1U64 Pad1U64(Rng1U64 r, U64 x);
internal U64 Center1U64(Rng1U64 r);
internal B32 Contains1U64(Rng1U64 r, U64 v);
internal U64 Dim1U64(Rng1U64 r);
internal Rng1U64 Union1U64(Rng1U64 a, Rng1U64 b);
internal Rng1U64 Intersection1U64(Rng1U64 a, Rng1U64 b);
internal U64 Clamp1U64(Rng1U64 r, U64 v);

internal Rng2F32 R2F32(Vec2F32 min, Vec2F32 max);
internal Rng2F32 Shift2F32(Rng2F32 r, Vec2F32 v);
internal Rng2F32 Pad2F32(Rng2F32 r, F32 x);
internal Vec2F32 Center2F32(Rng2F32 r);
internal B32 Contains2F32(Rng2F32 r, Vec2F32 v);
internal Vec2F32 Dim2F32(Rng2F32 r);
internal Rng2F32 Union2F32(Rng2F32 a, Rng2F32 b);
internal Rng2F32 Intersection2F32(Rng2F32 a, Rng2F32 b);
internal Vec2F32 Clamp2F32(Rng2F32 r, Vec2F32 v);

internal Rng2F64 R2F64(Vec2F64 min, Vec2F64 max);
internal Rng2F64 Shift2F64(Rng2F64 r, Vec2F64 v);
internal Rng2F64 Pad2F64(Rng2F64 r, F64 x);
internal Vec2F64 Center2F64(Rng2F64 r);
internal B32 Contains2F64(Rng2F64 r, Vec2F64 v);
internal Vec2F64 Dim2F64(Rng2F64 r);
internal Rng2F64 Union2F64(Rng2F64 a, Rng2F64 b);
internal Rng2F64 Intersection2F64(Rng2F64 a, Rng2F64 b);
internal Vec2F64 Clamp2F64(Rng2F64 r, Vec2F64 v);

internal Rng2S32 R2S32(Vec2S32 min, Vec2S32 max);
internal Rng2S32 Shift2S32(Rng2F32 r, Vec2S32 v);
internal Rng2S32 Pad2S32(Rng2S32 r, S32 x);
internal Vec2S32 Center2S32(Rng2S32 r);
internal B32 Contains2S32(Rng2S32 r, Vec2S32 v);
internal Vec2S32 Dim2S32(Rng2S32 r);
internal Rng2S32 Union2S32(Rng2S32 a, Rng2S32 b);
internal Rng2S32 Intersection2S32(Rng2S32 a, Rng2S32 b);
internal Vec2S32 Clamp2S32(Rng2S32 r, Vec2S32 v);

internal Rng2S64 R2S64(Vec2S64 min, Vec2S64 max);
internal Rng2S64 Shift2S64(Rng2F32 r, Vec2S64 v);
internal Rng2S64 Pad2S64(Rng2S64 r, S64 x);
internal Vec2S64 Center2S64(Rng2S64 r);
internal B32 Contains2S64(Rng2S64 r, Vec2S64 v);
internal Vec2S64 Dim2S64(Rng2S64 r);
internal Rng2S64 Union2S64(Rng2S64 a, Rng2S64 b);
internal Rng2S64 Intersection2S64(Rng2S64 a, Rng2S64 b);
internal Vec2S64 Clamp2S64(Rng2S64 r, Vec2S64 v);

internal Rng3F32 R3F32(Vec3F32 min, Vec3F32 max);
internal Rng3F32 Shift3F32(Rng3F32 r, Vec3F32 v);
internal Rng3F32 Pad3F32(Rng3F32 r, F32 x);
internal Vec3F32 Center3F32(Rng3F32 r);
internal B32 Contains3F32(Rng3F32 r, Vec3F32 v);
internal Vec3F32 Dim3F32(Rng3F32 r);
internal Rng3F32 Union3F32(Rng3F32 a, Rng3F32 b);
internal Rng3F32 Intersection3F32(Rng3F32 a, Rng3F32 b);
internal Vec3F32 Clamp3F32(Rng3F32 r, Vec3F32 v);

////////////////////////////////
//~ rjf: Lists

internal void Rng1U64ListPushNode(Rng1U64List *list, Rng1U64Node *node);
internal void Rng1U64ListPushNodeFront(Rng1U64List *list, Rng1U64Node *node);
internal void Rng1U64ListPush(Arena *arena, Rng1U64List *list, Rng1U64 rng);
internal void Rng1U64ListPushFront(Arena *arena, Rng1U64List *list, Rng1U64 rng);

////////////////////////////////
//~ rjf: Miscellaneous Ops

internal Vec3F32 HSVFromRGB(Vec3F32 rgb);
internal Vec3F32 RGBFromHSV(Vec3F32 hsv);
internal Vec4F32 Vec4F32FromHexRGBA(U32 hex);
internal F32 MillisecondsFromMicroseconds(U64 microseconds);
internal U64 MicrosecondsFromMilliseconds(F32 milliseconds);
internal Vec2S64 SideVertexFromCorner(Corner corner);

////////////////////////////////
//~ rjf: Omitted Base Type Wrappers (Assume F32)

#if !BASE_DISABLE_OMITTED_BASE_TYPES

//- rjf: types
typedef Vec2F32 Vec2;
typedef Vec3F32 Vec3;
typedef Vec4F32 Vec4;
typedef Rng1F32 Rng1;
typedef Rng2F32 Rng2;
typedef Rng3F32 Rng3;
typedef Mat3x3F32 Mat3x3;
typedef Mat4x4F32 Mat4x4;

//- rjf: constructors
#define V2(x, y)       V2F32((x), (y))
#define V3(x, y, z)    V3F32((x), (y), (z))
#define V4(x, y, z, w) V4F32((x), (y), (z), (w))
#define Vec2FromVec(v) Vec2F32FromVec(v)
#define Vec3FromVec(v) Vec3F32FromVec(v)
#define Vec4FromVec(v) Vec4F32FromVec(v)
#define R1(p0, p1) R1F32((p0), (p1))
#define R2(p0, p1) R2F32((p0), (p1))
#define R3(p0, p1) R3F32((p0), (p1))
#define MakeMat3x3(d) MakeMat3x3F32(d)
#define MakeTranslate3x3(translation) MakeTranslate3x3F32(translation)
#define MakeScale3x3(scale) MakeScale3x3F32(scale)
#define MakeRotate3x3(angle) MakeRotate3x3F32(angle)
#define MakeMat4x4(d) MakeMat4x4F32(d)
#define MakeTranslate4x4(translation) MakeTranslate4x4F32(translation)
#define MakeScale4x4(scale) MakeScale4x4F32(scale)
#define MakePerspective4x4(fov, aspect_ratio, near_z, far_z) MakePerspective4x4F32((fov), (aspect_ratio), (near_z), (far_z))
#define MakeOrthographic4x4(left, right, bottom, top, near_z, far_z) MakeOrthographic4x4F32((left), (right), (bottom), (top), (near_z), (far_z))
#define MakeLookAt4x4(eye, center, up) MakeLookAt4x4F32((eye), (center), (up))

//- rjf: scalar ops
#define Mix1(a, b, t) Mix1F32((a), (b), (t))

//- rjf: 2d vector ops
#define Add2(a, b) Add2F32((a), (b))
#define Sub2(a, b) Sub2F32((a), (b))
#define Mul2(a, b) Mul2F32((a), (b))
#define Div2(a, b) Div2F32((a), (b))
#define Scale2(v, scale) Scale2F32((v), (scale))
#define Dot2(a, b) Dot2F32((a), (b))
#define LengthSquared2(v) LengthSquared2F32(v)
#define Length2(v) Length2F32(v)
#define Normalize2(v) Normalize2F32(v)
#define Mix2(a, b, t) Mix2F32((a), (b), (t))

//- rjf: 3d vector ops
#define Add3(a, b) Add3F32((a), (b))
#define Sub3(a, b) Sub3F32((a), (b))
#define Mul3(a, b) Mul3F32((a), (b))
#define Div3(a, b) Div3F32((a), (b))
#define Scale3(v, scale) Scale3F32((v), (scale))
#define Dot3(a, b) Dot3F32((a), (b))
#define LengthSquared3(v) LengthSquared3F32(v)
#define Length3(v) Length3F32(v)
#define Normalize3(v) Normalize3F32(v)
#define Mix3(a, b, t) Mix3F32((a), (b), (t))
#define Cross3(a, b) Cross3F32((a), (b))
#define Transform3(v, mat3x3) Transform3F32((v), (mat3x3))

//- rjf: 4d vector ops
#define Add4(a, b) Add4F32((a), (b))
#define Sub4(a, b) Sub4F32((a), (b))
#define Mul4(a, b) Mul4F32((a), (b))
#define Div4(a, b) Div4F32((a), (b))
#define Scale4(v, scale) Scale4F32((v), (scale))
#define Dot4(a, b) Dot4F32((a), (b))
#define LengthSquared4(v) LengthSquared4F32(v)
#define Length4(v) Length4F32(v)
#define Normalize4(v) Normalize4F32(v)
#define Mix4(a, b, t) Mix4F32((a), (b), (t))
#define Transform4(v, mat4x4) Transform4F32((v), (mat4x4))

//- rjf: r1 ops
#define Shift1(rng, v) Shift1F32((rng), (v))
#define Pad1(rng, v) Pad1F32((rng), (v))
#define Center1(rng) Center1F32(rng)
#define Contains1(rng, v) Contains1F32((rng), (v))
#define Dim1(rng) Dim1F32(rng)
#define Union1(a, b) Union1F32((a), (b))
#define Intersection1(a, b) Intersection1F32((a), (b))

//- rjf: r2 ops
#define Shift2(rng, v) Shift2F32((rng), (v))
#define Pad2(rng, v) Pad2F32((rng), (v))
#define Center2(rng) Center2F32(rng)
#define Contains2(rng, v) Contains2F32((rng), (v))
#define Dim2(rng) Dim2F32(rng)
#define Union2(a, b) Union2F32((a), (b))
#define Intersection2(a, b) Intersection2F32((a), (b))

//- rjf: r3 ops
#define Shift3(rng, v) Shift3F32((rng), (v))
#define Pad3(rng, v) Pad3F32((rng), (v))
#define Center3(rng) Center3F32(rng)
#define Contains3(rng, v) Contains3F32((rng), (v))
#define Dim3(rng) Dim3F32(rng)
#define Union3(a, b) Union3F32((a), (b))
#define Intersection3(a, b) Intersection3F32((a), (b))

//- rjf: 3x3 ops
#define Mul3x3(a, b) Mul3x3F32((a), (b))
#define Scale3x3(m, scalar) Scale3x3F32((m), (scalar))

//- rjf: 4x4 ops
#define Mul4x4(a, b) Mul4x4F32((a), (b))
#define Scale4x4(m, scalar) Scale4x4F32((m), (scalar))
#define Inverse4x4(m) Inverse4x4F32(m)
#define RemoveRotation4x4(m) RemoveRotation4x4F32(m)

#endif // !BASE_DISABLE_OMITTED_BASE_TYPES

#endif // BASE_MATH_H
