////////////////////////////////
//~ rjf: List/Array Building Functions

//- rjf: s32s

internal void
S32ChunkListPush(Arena *arena, S32ChunkList *list, U64 cap, S32 v)
{
 S32ChunkNode *node = list->last;
 if(node == 0 || node->count >= node->cap)
 {
  node = PushArray(arena, S32ChunkNode, 1);
  QueuePush(list->first, list->last, node);
  list->chunk_count += 1;
  node->v = PushArrayNoZero(arena, S32, cap);
  node->cap = cap;
 }
 node->v[node->count] = v;
 node->count += 1;
 list->total_count += 1;
}

internal S32Array
S32ArrayFromChunkList(Arena *arena, S32ChunkList *list)
{
 S32Array array = {0};
 array.count = list->total_count;
 array.v = PushArrayNoZero(arena, S32, array.count);
 U64 idx = 0;
 for(S32ChunkNode *n = list->first; n != 0; n = n->next)
 {
  MemoryCopy(array.v+idx, n->v, n->count*sizeof(n->v[0]));
  idx += n->count;
 }
 return array;
}

//- rjf: f32s

internal void
F32ChunkListPush(Arena *arena, F32ChunkList *list, U64 cap, F32 v)
{
 F32ChunkNode *node = list->last;
 if(node == 0 || node->count >= node->cap)
 {
  node = PushArray(arena, F32ChunkNode, 1);
  QueuePush(list->first, list->last, node);
  list->chunk_count += 1;
  node->v = PushArrayNoZero(arena, F32, cap);
  node->cap = cap;
 }
 node->v[node->count] = v;
 node->count += 1;
 list->total_count += 1;
}

internal F32Array
F32ArrayFromChunkList(Arena *arena, F32ChunkList *list)
{
 F32Array array = {0};
 array.count = list->total_count;
 array.v = PushArrayNoZero(arena, F32, array.count);
 U64 idx = 0;
 for(F32ChunkNode *n = list->first; n != 0; n = n->next)
 {
  MemoryCopy(array.v+idx, n->v, n->count*sizeof(n->v[0]));
  idx += n->count;
 }
 return array;
}

//- rjf: vec3f32s

internal void
Vec3F32ChunkListPush(Arena *arena, Vec3F32ChunkList *list, U64 cap, Vec3F32 v)
{
 Vec3F32ChunkNode *node = list->last;
 if(node == 0 || node->count >= node->cap)
 {
  node = PushArray(arena, Vec3F32ChunkNode, 1);
  QueuePush(list->first, list->last, node);
  list->chunk_count += 1;
  node->v = PushArrayNoZero(arena, Vec3F32, cap);
  node->cap = cap;
 }
 node->v[node->count] = v;
 node->count += 1;
 list->total_count += 1;
}

internal Vec3F32Array
Vec3F32ArrayFromChunkList(Arena *arena, Vec3F32ChunkList *list)
{
 Vec3F32Array array = {0};
 array.count = list->total_count;
 array.v = PushArrayNoZero(arena, Vec3F32, array.count);
 U64 idx = 0;
 for(Vec3F32ChunkNode *n = list->first; n != 0; n = n->next)
 {
  MemoryCopy(array.v+idx, n->v, n->count*sizeof(n->v[0]));
  idx += n->count;
 }
 return array;
}
