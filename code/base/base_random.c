////////////////////////////////
//~ rjf: PCG

#define PCG_HAS_128BIT_OPS 0
#if COMPILER_MSVC && COMPILER_MSVC_YEAR < 2015
#define inline
#include "third_party/pcg/include/pcg_variants.h"
#undef inline
#else
#include "third_party/pcg/include/pcg_variants.h"
#endif

StaticAssert(sizeof(Dist) >= sizeof(pcg32_random_t), dist_size_check);

////////////////////////////////
//~ rjf: Implementations

internal Dist
DistMakeUniform(U64 seed)
{
 Dist result = {0};
 {
  pcg32_random_t *pcg_state = (pcg32_random_t *)result.u64;
  pcg32_srandom_r(pcg_state, seed, 0);
 }
 return result;
}

internal U32
SampleFromDist1U32(Dist *dist, Rng1U32 range)
{
 pcg32_random_t *pcg_state = (pcg32_random_t *)dist->u64;
 U32 value = pcg32_random_r(pcg_state);
 value = value%Dim1U32(range);
 value += range.min;
 return value;
}
