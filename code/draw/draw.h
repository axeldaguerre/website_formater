#ifndef DRAW_H
#define DRAW_H

////////////////////////////////
//~ rjf: Basics

typedef struct D_InitReceipt D_InitReceipt;
struct D_InitReceipt
{
 U64 u64[1];
};

////////////////////////////////
//~ rjf: Generated Code

#include "draw/generated/draw.meta.h"

////////////////////////////////
//~ rjf: Command Types

typedef struct D_Bucket D_Bucket;
struct D_Bucket
{
 // rjf: pass list
 R_PassList passes;
 
 // rjf: pass parameter stacks
 U64 last_cmd_stack_gen;
 U64 current_stack_gen;
 D_DeclBucketStacks;
};

////////////////////////////////
//~ rjf: Main & Per-Thread State Types

typedef struct D_State D_State;
struct D_State
{
 Arena *arena;
 R_Handle white_texture;
};

typedef struct D_BucketSelectionNode D_BucketSelectionNode;
struct D_BucketSelectionNode
{
 D_BucketSelectionNode *next;
 D_Bucket *bucket;
 Arena *arena;
};

typedef struct D_ThreadCtx D_ThreadCtx;
struct D_ThreadCtx
{
 Arena *arena;
 Arena *fallback_arena;
 D_Bucket fallback_bucket;
 D_BucketSelectionNode bucket_selection_fallback;
 D_BucketSelectionNode *bucket_selection_top;
 D_BucketSelectionNode *bucket_selection_free;
 D_DeclThreadStackTops;
};

////////////////////////////////
//~ rjf: Drawing Param Types

typedef struct D_RectParams D_RectParams;
struct D_RectParams
{
 Vec4F32 color;
 union
 {
  F32 corner_radius;
  F32 corner;
  F32 radius;
  F32 r;
 };
 union
 {
  F32 border_thickness;
  F32 border;
  F32 thickness;
  F32 t;
 };
 union
 {
  F32 softness;
  F32 soft;
  F32 s;
 };
 union
 {
  R_Slice2F32 slice;
  struct
  {
   union
   {
    R_Handle albedo_texture;
    R_Handle albedo;
    R_Handle texture;
    R_Handle tex;
   };
   union
   {
    Rng2F32 src_rect;
    Rng2F32 src;
   };
  };
 };
};

typedef struct D_SpriteParams D_SpriteParams;
struct D_SpriteParams
{
 Vec4F32 color;
 Vec2F32 scale;
 F32 shear;
 union
 {
  R_Slice2F32 slice;
  struct
  {
   union
   {
    R_Handle albedo_texture;
    R_Handle albedo;
    R_Handle texture;
    R_Handle tex;
   };
   union
   {
    Rng2F32 src_rect;
    Rng2F32 src;
   };
  };
 };
};

////////////////////////////////
//~ rjf: root_globals

root_global D_State *d_state;
extern per_thread D_ThreadCtx *d_thread_ctx;

////////////////////////////////
//~ rjf: Initialization

internal D_InitReceipt D_Init(R_InitReceipt r_init_receipt, F_InitReceipt f_init_receipt);
internal void D_EnsureThreadInited(void);

////////////////////////////////
//~ rjf: Frame Boundaries

internal void D_BeginFrame(void);
internal void D_EndFrame(void);

////////////////////////////////
//~ rjf: Basic Helpers

internal U64 D_HashFromString(U64 seed, String8 string);

////////////////////////////////
//~ rjf: Bucket Creation, Selection, & Submission

internal D_Bucket *D_BucketMake(Arena *arena);
internal void D_BucketConcatInPlace(D_Bucket *to_push);
internal void D_PushBucket(Arena *arena, D_Bucket *bucket);
internal void D_PopBucket(void);
#define D_BucketScope(arena, bucket) DeferLoop(D_PushBucket((arena), (bucket)), D_PopBucket())
internal Arena *D_ActiveArena(void);
internal D_Bucket *D_ActiveBucket(void);
internal void D_Submit(R_Handle window_r, D_Bucket *bucket);

////////////////////////////////
//~ rjf: Pass Building Helpers

internal R_Pass *D_PassFromBucket(Arena *arena, D_Bucket *bucket, R_PassKind kind);

////////////////////////////////
//~ rjf: UI Pass Commands

//- rjf: rectangles (2d)
#define D_Rect2D(r, ...) D_Rect2D_((r), &(D_RectParams){.color = {1, 1, 1, 1}, __VA_ARGS__})
internal R_Rect2DInst *D_Rect2D_(Rng2F32 rect, D_RectParams *p);

//- rjf: text (2d)
internal F32 D_Text2D(Vec2F32 position, F_Tag font, F32 size, Vec4F32 color, String8 string);
internal F32 D_Text2DF(Vec2F32 position, F_Tag font, F32 size, Vec4F32 color, char *fmt, ...);

////////////////////////////////
//~ rjf: G0 Pass Commands

//- rjf: sprites (3d)
#define D_Sprite3D(pos, xform, ...) D_Sprite3D_((pos), (xform), &(D_SpriteParams){.color = {1, 1, 1, 1}, .scale = {1, 1}, __VA_ARGS__})
internal R_Sprite3DInst *D_Sprite3D_(Vec3F32 pos, Mat4x4F32 xform, D_SpriteParams *p);

//- rjf: point lights (3d)
internal R_PointLight3DInst *D_PointLight3D(Vec3F32 pos, F32 radius, Vec4F32 color, F32 intensity);

//- rjf: debug lines (3d)
internal R_DebugLine3DInst *D_DebugLine3D(Vec3F32 p0, Vec3F32 p1, Vec4F32 color0, Vec4F32 color1);
internal void D_DebugCuboid3D(Rng3F32 rng, Vec4F32 color);
internal void D_DebugSphere3D(Vec3F32 center, F32 radius, Vec4F32 color);

////////////////////////////////
//~ rjf: Stacks

internal R_Tex2DSampleKind          D_PushTex2DSampleKind(R_Tex2DSampleKind v);
internal R_BlendMode                D_PushBlendMode(R_BlendMode v);
internal Mat3x3F32                  D_PushTransform2D(Mat3x3F32 v);
internal Rng2F32                    D_PushClip(Rng2F32 v);
internal F32                        D_PushTransparency(F32 v);

internal R_Tex2DSampleKind          D_PopTex2DSampleKind(void);
internal R_BlendMode                D_PopBlendMode(void);
internal Mat3x3F32                  D_PopTransform2D(void);
internal Rng2F32                    D_PopClip(void);
internal F32                        D_PopTransparency(void);

internal R_Tex2DSampleKind          D_TopTex2DSampleKind(void);
internal R_BlendMode                D_TopBlendMode(void);
internal Mat3x3F32                  D_TopTransform2D(void);
internal Rng2F32                    D_TopClip(void);
internal F32                        D_TopTransparency(void);

#define D_Tex2DSampleKind(v)             DeferLoop(D_PushTex2DSampleKind(v), D_PopTex2DSampleKind())
#define D_BlendMode(v)                   DeferLoop(D_PushBlendMode(v), D_PopBlendMode())
#define D_Transform2D(v)                 DeferLoop(D_PushTransform2D(v), D_PopTransform2D())
#define D_Clip(v)                        DeferLoop(D_PushClip(v), D_PopClip())
#define D_Transparency(v)                DeferLoop(D_PushTransparency(v), D_PopTransparency())

#endif // DRAW_H
