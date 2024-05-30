/* date = April 2nd 2023 0:12 pm */

#ifndef BASE_RANDOM_H
#define BASE_RANDOM_H

typedef struct Dist Dist;
struct Dist
{
 U64 u64[8];
};

internal Dist DistMakeUniform(U64 seed);

internal U32 SampleFromDist1U32(Dist *dist, Rng1U32 range);
#define SampleFromDist1S32(dist, range) (S32)SampleFromDist1U32((dist), (range))
#define SampleFromDist1F32(dist, range) (F32)(((F64)SampleFromDist1U32((dist), R1U32(0, U32Max))/U32Max)*Dim1F32(range)+(range).min)

#endif // BASE_RANDOM_H
