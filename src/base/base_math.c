internal Rng1U64 rng_1u64(U64 min, U64 max) {Rng1U64 r = {min, max}; if(r.max < r.min) { Swap(U64, r.min, r.max); } return r;}



