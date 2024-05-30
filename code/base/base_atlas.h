/* date = August 1st 2022 10:28 am */

#ifndef BASE_ATLAS_H
#define BASE_ATLAS_H

////////////////////////////////
//~ rjf: Atlas Allocator Types

typedef U32 AtlasRegionNodeFlags;
enum
{
 AtlasRegionNodeFlag_Taken = (1<<0),
};

typedef struct AtlasRegionNode AtlasRegionNode;
struct AtlasRegionNode
{
 AtlasRegionNode *parent;
 AtlasRegionNode *children[Corner_COUNT];
 Vec2S64 max_free_size[Corner_COUNT];
 AtlasRegionNodeFlags flags;
 U64 num_allocated_descendants;
};

typedef struct Atlas Atlas;
struct Atlas
{
 Vec2S64 root_dim;
 AtlasRegionNode *root;
};

////////////////////////////////
//~ rjf: Atlas Allocator Functions

internal Atlas *AtlasMake(Arena *arena, Vec2S64 dim);
internal Rng2S64 AtlasRegionAlloc(Arena *arena, Atlas *atlas, Vec2S64 needed_size);
internal void AtlasRegionRelease(Atlas *atlas, Rng2S64 region);

#endif // BASE_ATLAS_H
