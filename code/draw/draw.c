////////////////////////////////
//~ rjf: root_globals

#if BUILD_CORE
D_State *d_state = 0;
per_thread D_ThreadCtx *d_thread_ctx = 0;
#endif

////////////////////////////////
//~ rjf: Stack Implementation Helper Macros

#define D_StackPush(node_type, val_type, name, new_val) \
{\
Arena *arena = D_ActiveArena();\
D_Bucket *bucket = D_ActiveBucket();\
node_type *node_v = bucket->name##_free;\
if(node_v != 0) {StackPop(bucket->name##_free);}\
else {node_v = PushArray(arena, node_type, 1);}\
node_v->v = new_val;\
new_val = bucket->name##_stack_top->v;\
StackPush(bucket->name##_stack_top, node_v);\
bucket->current_stack_gen += 1;\
return new_val;\
}

#define D_StackPop(node_type, val_type, name) \
{\
val_type result = d_thread_ctx->name##_nil_stack_top.v;\
D_Bucket *bucket = D_ActiveBucket();\
if(bucket->name##_stack_top != &d_thread_ctx->name##_nil_stack_top)\
{\
node_type *node = bucket->name##_stack_top;\
result = node->v;\
StackPop(bucket->name##_stack_top);\
StackPush(bucket->name##_free, node);\
bucket->current_stack_gen += 1;\
}\
return result;\
}

#define D_StackTop(node_type, val_type, name) \
{\
D_Bucket *bucket = D_ActiveBucket();\
val_type result = bucket->name##_stack_top->v;\
return result;\
}

////////////////////////////////
//~ rjf: Generated Code

#include "draw/generated/draw.meta.c"

////////////////////////////////
//~ rjf: Layer Initialization

internal D_InitReceipt
D_Init(R_InitReceipt r_init_receipt, F_InitReceipt f_init_receipt)
{
 if(IsMainThread() && d_state == 0)
 {
  Arena *arena = ArenaAlloc(Gigabytes(8));
  d_state = PushArray(arena, D_State, 1);
  d_state->arena = arena;
  U32 white_data[] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
  d_state->white_texture = R_Tex2DAlloc(V2S64(2, 2), R_Tex2DFormat_RGBA8, R_Tex2DKind_Static, &white_data[0]);
  D_EnsureThreadInited();
 }
 D_InitReceipt result = {0};
 return result;
}

internal void
D_EnsureThreadInited(void)
{
 if(d_thread_ctx == 0)
 {
  Arena *arena = ArenaAlloc(Megabytes(1));
  d_thread_ctx = PushArray(arena, D_ThreadCtx, 1);
  d_thread_ctx->arena = arena;
  d_thread_ctx->fallback_arena = ArenaAlloc(Megabytes(256));
  D_InitBucketStacks(&d_thread_ctx->fallback_bucket);
  d_thread_ctx->bucket_selection_fallback.arena = d_thread_ctx->fallback_arena;
  d_thread_ctx->bucket_selection_fallback.bucket = &d_thread_ctx->fallback_bucket;
  d_thread_ctx->bucket_selection_top = &d_thread_ctx->bucket_selection_fallback;
  D_InitThreadStackTops;
 }
}

////////////////////////////////
//~ rjf: Frame Boundaries

internal void
D_BeginFrame(void)
{
 D_EnsureThreadInited();
}

internal void
D_EndFrame(void)
{
}

////////////////////////////////
//~ rjf: Basic Helpers

internal U64
D_HashFromString(U64 seed, String8 string)
{
 U64 result = seed;
 for(U64 i = 0; i < string.size; i += 1)
 {
  result = ((result << 5) + result) + string.str[i];
 }
 return result;
}

////////////////////////////////
//~ rjf: Bucket Creation, Selection, & Submission

internal D_Bucket *
D_BucketMake(Arena *arena)
{
 D_Bucket *bucket = PushArray(arena, D_Bucket, 1);
 D_InitBucketStacks(bucket);
 return bucket;
}

internal void
D_BucketConcatInPlace(D_Bucket *to_push)
{
 D_Bucket *bucket = D_ActiveBucket();
 R_PassListConcatInPlace(&bucket->passes, &to_push->passes);
}

internal void
D_PushBucket(Arena *arena, D_Bucket *bucket)
{
 D_BucketSelectionNode *node = d_thread_ctx->bucket_selection_free;
 if(node != 0)
 {
  StackPop(d_thread_ctx->bucket_selection_free);
 }
 else
 {
  node = PushArray(d_thread_ctx->arena, D_BucketSelectionNode, 1);
 }
 node->arena = arena;
 node->bucket = bucket;
 StackPush(d_thread_ctx->bucket_selection_top, node);
}

internal void
D_PopBucket(void)
{
 D_BucketSelectionNode *node = d_thread_ctx->bucket_selection_top;
 if(node != &d_thread_ctx->bucket_selection_fallback)
 {
  StackPop(d_thread_ctx->bucket_selection_top);
  StackPush(d_thread_ctx->bucket_selection_free, node);
 }
 if(d_thread_ctx->bucket_selection_top == &d_thread_ctx->bucket_selection_fallback)
 {
  ArenaClear(d_thread_ctx->fallback_arena);
  MemoryZeroStruct(&d_thread_ctx->fallback_bucket);
  D_InitBucketStacks(&d_thread_ctx->fallback_bucket);
 }
}

internal Arena *
D_ActiveArena(void)
{
 return d_thread_ctx->bucket_selection_top->arena;
}

internal D_Bucket *
D_ActiveBucket(void)
{
 return d_thread_ctx->bucket_selection_top->bucket;
}

internal void
D_Submit(R_Handle window_r, D_Bucket *bucket)
{
 R_WindowSubmit(window_r, &bucket->passes);
}

#if 0
internal R_Cmd *
D_PushCmd(R_CmdKind kind, R_Handle albedo_texture)
{
 Arena *arena = D_ActiveArena();
 D_Bucket *bucket = D_ActiveBucket();
 R_CmdList *cmds = &bucket->cmds;
 R_Cmd *cmd = cmds->last ? &cmds->last->cmd : 0;
 if(cmd == 0 || cmd->kind != kind ||
    bucket->last_cmd_stack_gen != bucket->current_stack_gen ||
    (!R_HandleMatch(cmd->albedo_texture, albedo_texture) &&
     !R_HandleMatch(albedo_texture, d_state->white_texture)))
 {
  R_Cmd new_cmd = {0};
  new_cmd.kind = kind;
  new_cmd.albedo_texture = albedo_texture;
  new_cmd.albedo_texture_sample_kind = bucket->texture2d_sample_kind_stack_top->v;
  new_cmd.xform2d        = bucket->xform2d_stack_top->v;
  new_cmd.clip           = bucket->clip_stack_top->v;
  new_cmd.opacity        = bucket->opacity_stack_top->v;
  R_CmdNode *new_cmd_node = R_CmdListPush(arena, cmds, &new_cmd);
  cmd = &new_cmd_node->cmd;
 }
 bucket->last_cmd_stack_gen = bucket->current_stack_gen;
 return cmd;
}
#endif

////////////////////////////////
//~ rjf: Pass Building Helpers

internal R_Pass *
D_PassFromBucket(Arena *arena, D_Bucket *bucket, R_PassKind kind)
{
 R_PassNode *node = bucket->passes.last;
 R_Pass *pass = 0;
 if(node == 0 || node->v.kind != kind)
 {
  pass = R_PassListPush(arena, &bucket->passes, kind);
 }
 else
 {
  pass = &node->v;
 }
 return pass;
}

////////////////////////////////
//~ rjf: UI Pass Commands

//- rjf: rectangles (2d)

internal R_Rect2DInst *
D_Rect2D_(Rng2F32 rect, D_RectParams *p)
{
 Arena *arena = D_ActiveArena();
 D_Bucket *bucket = D_ActiveBucket();
 R_Pass *pass = D_PassFromBucket(arena, bucket, R_PassKind_UI);
 R_PassParams_UI *pass_params = pass->params_ui;
 R_BatchGroup2DNode *batch_group = pass_params->rects.last;
 R_Handle tex = R_HandleMatch(p->tex, R_HandleZero()) ? d_state->white_texture : p->tex;
 if(batch_group == 0 ||
    (!R_HandleMatch(batch_group->params.albedo_tex, tex) &&
     !R_HandleMatch(tex, d_state->white_texture)) ||
    bucket->last_cmd_stack_gen != bucket->current_stack_gen)
 {
  batch_group = PushArray(arena, R_BatchGroup2DNode, 1);
  QueuePush(pass_params->rects.first, pass_params->rects.last, batch_group);
  pass_params->rects.count += 1;
  batch_group->params.albedo_tex = tex;
  batch_group->params.albedo_tex_sample_kind = bucket->tex2d_sample_kind_stack_top->v;
  batch_group->params.xform2d = bucket->xform2d_stack_top->v;
  batch_group->params.clip = bucket->clip_stack_top->v;
  batch_group->params.transparency = bucket->transparency_stack_top->v;
 }
 R_Rect2DInst *inst = R_BatchListPushStruct(arena, &batch_group->batches, 256, R_Rect2DInst);
 inst->dst_rect = rect;
 inst->src_rect = p->src_rect;
 inst->colors[Corner_00] = inst->colors[Corner_01] = inst->colors[Corner_10] = inst->colors[Corner_11] = p->color;
 inst->corner_radii[Corner_00] = inst->corner_radii[Corner_01] = inst->corner_radii[Corner_10] = inst->corner_radii[Corner_11] = p->radius;
 inst->border_thickness = p->border_thickness;
 inst->softness = p->soft;
 inst->omit_texture = (F32)!!R_HandleIsZero(p->albedo_texture);
 return inst;
}

//- rjf: text (2d)

internal F32
D_Text2D(Vec2F32 position, F_Tag font, F32 size, Vec4F32 color, String8 string)
{
 Arena *arena = D_ActiveArena();
 D_Bucket *bucket = D_ActiveBucket();
 Temp scratch = ScratchBegin(&arena, 1);
 F_Run run = F_RunFromFontSizeString(scratch.arena, font, size, string);
 Vec2F32 p = position;
 for(F_Piece *piece = run.first_piece; piece != 0; piece = piece->next)
 {
  Vec2F32 size = Dim2F32(piece->src_rect);
  Vec2F32 p0 = Add2F32(p, piece->offset);
  Vec2F32 p1 = Add2F32(p0, size);
  Rng2F32 rect = R2F32(p0, p1);
  D_Rect2D(rect, .texture = piece->texture, .src_rect = piece->src_rect, .color = color);
  p.x += piece->advance;
 }
 ScratchEnd(scratch);
 F32 result = p.x - position.x;
 return result;
}

internal F32
D_Text2DF(Vec2F32 position, F_Tag font, F32 size, Vec4F32 color, char *fmt, ...)
{
 Arena *arena = D_ActiveArena();
 D_Bucket *bucket = D_ActiveBucket();
 Temp scratch = ScratchBegin(&arena, 1);
 va_list args;
 va_start(args, fmt);
 String8 string = PushStr8FV(scratch.arena, fmt, args);
 F32 result = D_Text2D(position, font, size, color, string);
 va_end(args);
 ScratchEnd(scratch);
 return result;
}

////////////////////////////////
//~ rjf: G0 Pass Commands

//- rjf: sprites (3d)

internal R_Sprite3DInst *
D_Sprite3D_(Vec3F32 pos, Mat4x4F32 xform, D_SpriteParams *p)
{
 Arena *arena = D_ActiveArena();
 D_Bucket *bucket = D_ActiveBucket();
 R_Pass *pass = D_PassFromBucket(arena, bucket, R_PassKind_G0);
 R_PassParams_G0 *pass_params = pass->params_g0;
 R_BatchGroup3DMap *map = &pass_params->sprites;
 if(map->slots_count == 0)
 {
  map->slots_count = 64;
  map->slots = PushArray(arena, R_BatchGroup3DSlot, map->slots_count);
 }
 R_Tex2DSampleKind sample_kind = D_TopTex2DSampleKind();
 R_BlendMode blend_mode = D_TopBlendMode();
 U64 tex_hash = D_HashFromString(5381, Str8Struct(&p->tex));
 U64 smp_hash = D_HashFromString(tex_hash, Str8Struct(&sample_kind));
 U64 bln_hash = D_HashFromString(smp_hash, Str8Struct(&blend_mode));
 U64 slot_idx = bln_hash%map->slots_count;
 R_BatchGroup3DSlot *slot = &map->slots[slot_idx];
 R_BatchGroup3DNode *node = 0;
 for(R_BatchGroup3DNode *n = slot->first; n != 0; n = n->next)
 {
  if(R_HandleMatch(n->params.albedo_tex, p->tex) &&
     n->params.albedo_tex_sample_kind == sample_kind &&
     n->params.blend_mode == blend_mode)
  {
   node = n;
   break;
  }
 }
 if(node == 0)
 {
  node = PushArray(arena, R_BatchGroup3DNode, 1);
  QueuePush(slot->first, slot->last, node);
  node->params.albedo_tex = p->tex;
  node->params.albedo_tex_sample_kind = sample_kind;
  node->params.blend_mode = blend_mode;
 }
 R_Sprite3DInst *inst = R_BatchListPushStruct(arena, &node->batches, 1024, R_Sprite3DInst);
 inst->pos = V4F32(pos.x, pos.y, pos.z, 1);
 inst->xform = xform;
 inst->src_rect = p->src_rect;
 inst->colors[Corner_00] = inst->colors[Corner_01] = inst->colors[Corner_10] = inst->colors[Corner_11] = p->color;
 inst->omit_texture = (F32)!!R_HandleIsZero(p->albedo_texture);
 inst->shear = p->shear;
 return inst;
}

//- rjf: point lights (3d)

internal R_PointLight3DInst *
D_PointLight3D(Vec3F32 pos, F32 radius, Vec4F32 color, F32 intensity)
{
 Arena *arena = D_ActiveArena();
 D_Bucket *bucket = D_ActiveBucket();
 R_Pass *pass = D_PassFromBucket(arena, bucket, R_PassKind_G0);
 R_PassParams_G0 *pass_params = pass->params_g0;
 R_PointLight3DInst *inst = R_BatchListPushStruct(arena, &pass_params->point_lights, 2048, R_PointLight3DInst);
 inst->pos.xyz   = pos;
 inst->pos.w     = radius;
 inst->color     = color;
 inst->intensity = intensity;
 return inst;
}

//- rjf: debug lines (3d)

internal R_DebugLine3DInst *
D_DebugLine3D(Vec3F32 p0, Vec3F32 p1, Vec4F32 color0, Vec4F32 color1)
{
 Arena *arena = D_ActiveArena();
 D_Bucket *bucket = D_ActiveBucket();
 R_Pass *pass = D_PassFromBucket(arena, bucket, R_PassKind_G0);
 R_PassParams_G0 *pass_params = pass->params_g0;
 R_DebugLine3DInst *inst = R_BatchListPushStruct(arena, &pass_params->debug_lines, 2048, R_DebugLine3DInst);
 inst->p0 = V4F32(p0.x, p0.y, p0.z, 1.f);
 inst->p1 = V4F32(p1.x, p1.y, p1.z, 1.f);
 inst->color0 = color0;
 inst->color1 = color1;
 return inst;
}

internal void
D_DebugCuboid3D(Rng3F32 rng, Vec4F32 color)
{
 D_DebugLine3D(V3F32(rng.min.x, rng.min.y, rng.min.z), V3F32(rng.min.x, rng.max.y, rng.min.z), color, color);
 D_DebugLine3D(V3F32(rng.min.x, rng.min.y, rng.min.z), V3F32(rng.max.x, rng.min.y, rng.min.z), color, color);
 D_DebugLine3D(V3F32(rng.max.x, rng.min.y, rng.min.z), V3F32(rng.max.x, rng.max.y, rng.min.z), color, color);
 D_DebugLine3D(V3F32(rng.min.x, rng.max.y, rng.min.z), V3F32(rng.max.x, rng.max.y, rng.min.z), color, color);
 D_DebugLine3D(V3F32(rng.min.x, rng.min.y, rng.max.z), V3F32(rng.min.x, rng.max.y, rng.max.z), color, color);
 D_DebugLine3D(V3F32(rng.min.x, rng.min.y, rng.max.z), V3F32(rng.max.x, rng.min.y, rng.max.z), color, color);
 D_DebugLine3D(V3F32(rng.max.x, rng.min.y, rng.max.z), V3F32(rng.max.x, rng.max.y, rng.max.z), color, color);
 D_DebugLine3D(V3F32(rng.min.x, rng.max.y, rng.max.z), V3F32(rng.max.x, rng.max.y, rng.max.z), color, color);
 D_DebugLine3D(V3F32(rng.min.x, rng.min.y, rng.min.z), V3F32(rng.min.x, rng.min.y, rng.max.z), color, color);
 D_DebugLine3D(V3F32(rng.min.x, rng.max.y, rng.min.z), V3F32(rng.min.x, rng.max.y, rng.max.z), color, color);
 D_DebugLine3D(V3F32(rng.max.x, rng.min.y, rng.min.z), V3F32(rng.max.x, rng.min.y, rng.max.z), color, color);
 D_DebugLine3D(V3F32(rng.max.x, rng.max.y, rng.min.z), V3F32(rng.max.x, rng.max.y, rng.max.z), color, color);
}

internal void
D_DebugSphere3D(Vec3F32 center, F32 radius, Vec4F32 color)
{
 for(F32 p = 0; p <= 1.1f; p += 0.1f)
 {
  // rjf: x/y plane
  {
   Vec3F32 circle_center = center;
   circle_center.z += radius*CosF32(p*0.5f);
   Vec3F32 last_vertex = {0};
   F32 p_radius = SinF32(p*0.5f) * radius;
   for(F32 angle_p = 0; angle_p <= 1.1f; angle_p += 0.05f)
   {
    F32 angle = angle_p;
    Vec3F32 vertex = Add3F32(circle_center, V3F32(p_radius*CosF32(angle), p_radius*SinF32(angle), 0));
    if(angle_p > 0)
    {
     D_DebugLine3D(vertex, last_vertex, color, color);
    }
    last_vertex = vertex;
   }
  }
  
  // rjf: x/z plane
  {
   Vec3F32 circle_center = center;
   circle_center.y += radius*CosF32(p*0.5f);
   Vec3F32 last_vertex = {0};
   F32 p_radius = SinF32(p*0.5f) * radius;
   for(F32 angle_p = 0; angle_p <= 1.1f; angle_p += 0.05f)
   {
    F32 angle = angle_p;
    Vec3F32 vertex = Add3F32(circle_center, V3F32(p_radius*CosF32(angle), 0, p_radius*SinF32(angle)));
    if(angle_p > 0)
    {
     D_DebugLine3D(vertex, last_vertex, color, color);
    }
    last_vertex = vertex;
   }
  }
 }
}
