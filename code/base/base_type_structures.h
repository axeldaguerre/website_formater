/* date = December 16th 2023 2:39 pm */

#ifndef BASE_TYPE_STRUCTURES_H
#define BASE_TYPE_STRUCTURES_H

////////////////////////////////
//~ rjf: Base Type Lists/Arrays

//- rjf: s32s

typedef struct S32ChunkNode S32ChunkNode;
struct S32ChunkNode
{
 S32ChunkNode *next;
 S32 *v;
 U64 count;
 U64 cap;
};

typedef struct S32ChunkList S32ChunkList;
struct S32ChunkList
{
 S32ChunkNode *first;
 S32ChunkNode *last;
 U64 chunk_count;
 U64 total_count;
};

typedef struct S32Array S32Array;
struct S32Array
{
 S32 *v;
 U64 count;
};

//- rjf: f32s

typedef struct F32ChunkNode F32ChunkNode;
struct F32ChunkNode
{
 F32ChunkNode *next;
 F32 *v;
 U64 count;
 U64 cap;
};

typedef struct F32ChunkList F32ChunkList;
struct F32ChunkList
{
 F32ChunkNode *first;
 F32ChunkNode *last;
 U64 chunk_count;
 U64 total_count;
};

typedef struct F32Array F32Array;
struct F32Array
{
 F32 *v;
 U64 count;
};

//- rjf: vec3f32s

typedef struct Vec3F32ChunkNode Vec3F32ChunkNode;
struct Vec3F32ChunkNode
{
 Vec3F32ChunkNode *next;
 Vec3F32 *v;
 U64 count;
 U64 cap;
};

typedef struct Vec3F32ChunkList Vec3F32ChunkList;
struct Vec3F32ChunkList
{
 Vec3F32ChunkNode *first;
 Vec3F32ChunkNode *last;
 U64 chunk_count;
 U64 total_count;
};

typedef struct Vec3F32Array Vec3F32Array;
struct Vec3F32Array
{
 Vec3F32 *v;
 U64 count;
};

////////////////////////////////
//~ rjf: List/Array Building Functions

//- rjf: s32s
internal void S32ChunkListPush(Arena *arena, S32ChunkList *list, U64 cap, S32 v);
internal S32Array S32ArrayFromChunkList(Arena *arena, S32ChunkList *list);

//- rjf: f32s
internal void F32ChunkListPush(Arena *arena, F32ChunkList *list, U64 cap, F32 v);
internal F32Array F32ArrayFromChunkList(Arena *arena, F32ChunkList *list);

//- rjf: vec3f32s
internal void Vec3F32ChunkListPush(Arena *arena, Vec3F32ChunkList *list, U64 cap, Vec3F32 v);
internal Vec3F32Array Vec3F32ArrayFromChunkList(Arena *arena, Vec3F32ChunkList *list);

#endif //BASE_TYPE_STRUCTURES_H
