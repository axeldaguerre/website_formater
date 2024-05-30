/* date = September 13th 2023 10:41 pm */

#ifndef BASE_META_H
#define BASE_META_H

typedef struct TweakB32Info TweakB32Info;
struct TweakB32Info
{
 String8 name;
 B32 default_value;
 B32 *value_ptr;
};

typedef struct TweakF32Info TweakF32Info;
struct TweakF32Info
{
 String8 name;
 F32 default_value;
 Rng1F32 value_range;
 F32 *value_ptr;
};

#endif //BASE_META_H
