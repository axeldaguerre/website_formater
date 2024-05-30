#if !defined(ArenaImpl_Reserve)
#error ArenaImpl_Reserve must be defined to use base memory.
#endif
#if !defined(ArenaImpl_Release)
#error ArenaImpl_Release must be defined to use base memory.
#endif
#if !defined(ArenaImpl_Commit)
#error ArenaImpl_Commit must be defined to use base memory.
#endif
#if !defined(ArenaImpl_Decommit)
#error ArenaImpl_Decommit must be defined to use base memory.
#endif

////////////////////////////////
//~ rjf: Arena Functions

internal Arena *
ArenaAlloc(U64 size)
{
 U64 size_roundup_granularity = Megabytes(64);
 size += size_roundup_granularity-1;
 size -= size%size_roundup_granularity;
 void *block = ArenaImpl_Reserve(size);
 U64 initial_commit_size = ARENA_COMMIT_GRANULARITY;
 Assert(initial_commit_size >= sizeof(Arena));
 ArenaImpl_Commit(block, initial_commit_size);
 Arena *arena = (Arena *)block;
 arena->pos = sizeof(Arena);
 arena->commit_pos = initial_commit_size;
 arena->align = 8;
 arena->size = size;
 return arena;
}

internal Arena *
ArenaAllocDefault(void)
{
 Arena *arena = ArenaAlloc(Gigabytes(8));
 return arena;
}

internal void
ArenaRelease(Arena *arena)
{
 ArenaImpl_Release(arena, arena->size);
}

internal void *
ArenaPushNoZero(Arena *arena, U64 size)
{
 void *result = 0;
 if(arena->pos + size <= arena->size)
 {
  U8 *base = (U8 *)arena;
  U64 post_align_pos = (arena->pos + (arena->align-1));
  post_align_pos -= post_align_pos%arena->align;
  U64 align = post_align_pos - arena->pos;
  result = base + arena->pos + align;
  arena->pos += size + align;
  if(arena->commit_pos < arena->pos)
  {
   U64 size_to_commit = arena->pos - arena->commit_pos;
   size_to_commit += ARENA_COMMIT_GRANULARITY - 1;
   size_to_commit -= size_to_commit%ARENA_COMMIT_GRANULARITY;
   ArenaImpl_Commit(base + arena->commit_pos, size_to_commit);
   arena->commit_pos += size_to_commit;
  }
 }
 else
 {
  // NOTE(rjf): fallback strategy. right now, just fail.
 }
 return result;
}

internal void *
ArenaPushAligner(Arena *arena, U64 alignment)
{
 U64 pos = arena->pos;
 U64 pos_rounded_up = pos + alignment-1;
 pos_rounded_up -= pos_rounded_up%alignment;
 U64 size_to_alloc = pos_rounded_up - pos;
 void *result = 0;
 if(size_to_alloc != 0)
 {
  result = ArenaPushNoZero(arena, size_to_alloc);
 }
 return result;
}

internal void *
ArenaPush(Arena *arena, U64 size)
{
 void *result = ArenaPushNoZero(arena, size);
 // TODO(rjf): we can avoid the explicit MemoryZero by relying on commit
 // boundaries
 MemoryZero(result, size);
 return result;
}

internal void
ArenaPopTo(Arena *arena, U64 pos)
{
 U64 min_pos = sizeof(Arena);
 U64 new_pos = Max(min_pos, pos);
 arena->pos = new_pos;
 U64 pos_aligned_to_commit_chunks = arena->pos + ARENA_COMMIT_GRANULARITY-1;
 pos_aligned_to_commit_chunks -= pos_aligned_to_commit_chunks%ARENA_COMMIT_GRANULARITY;
 if(pos_aligned_to_commit_chunks + ARENA_DECOMMIT_THRESHOLD <= arena->commit_pos)
 {
  U8 *base = (U8 *)arena;
  U64 size_to_decommit = arena->commit_pos-pos_aligned_to_commit_chunks;
  ArenaImpl_Decommit(base + pos_aligned_to_commit_chunks, size_to_decommit);
  arena->commit_pos -= size_to_decommit;
 }
}

internal void
ArenaSetAutoAlign(Arena *arena, U64 align)
{
 arena->align = align;
}

internal void
ArenaPop(Arena *arena, U64 size)
{
 U64 min_pos = sizeof(Arena);
 U64 size_to_pop = Min(size, arena->pos);
 U64 new_pos = arena->pos - size_to_pop;
 new_pos = Max(new_pos, min_pos);
 ArenaPopTo(arena, new_pos);
}

internal void
ArenaClear(Arena *arena)
{
 ArenaPopTo(arena, sizeof(Arena));
}

internal U64
ArenaPos(Arena *arena)
{
 return arena->pos;
}

////////////////////////////////
//~ rjf: Arena Temp Functions

internal Temp
TempBegin(Arena *arena)
{
 Temp temp = {0};
 temp.arena = arena;
 temp.pos = arena->pos;
 return temp;
}

internal void
TempEnd(Temp temp)
{
 ArenaPopTo(temp.arena, temp.pos);
}
