internal Arena *
arena_allocate__sized(U64 init_res, U64 init_cmt)
{ 
  init_res = ClampBottom(ARENA_HEADER_SIZE,  init_res);
  init_cmt = ClampBottom(ARENA_HEADER_SIZE,  init_cmt);
  void *memory = os_reserve(init_res);
  if(!os_commit(memory, init_cmt)) {
    memory = 0;
    os_release(memory, init_cmt);
  }  
  AssertAlways(memory);
  Arena *arena = (Arena*)memory;
  if(arena) {
    arena->prev     = 0;
    arena->current  = arena;
    arena->base_pos = 0;
    arena->pos      = ARENA_HEADER_SIZE;
    arena->cmt      = init_cmt;
    arena->res      = init_res;
  }
  return arena;
}

internal Arena *
arena_allocate()
{
  U64 init_cmt = ARENA_COMMIT_SIZE;
  U64 init_res = ARENA_RESERVE_SIZE;
  Arena *arena = arena_allocate__sized(init_res, init_cmt);
  return arena;
}

internal U64
arena_pos_addr(Arena *arena)
{
  Arena *current = arena->current;
  AssertAlways(current != 0);
  U64 pos = current->base_pos + current->pos;
  return pos;
}

internal void *
arena_push(Arena *arena, U64 size)
{  
  Arena *current = arena->current;
  AssertAlways(current != 0);
  if((current->pos + size) > arena->res) {
    // CHECK: does it works ?
    /*
      TODO(Axel): is memory space granurality useful here? 
      afaik win32 does it by itself. TEST my assumption
    */
    Arena *new_block = arena_allocate();
    if(new_block) {
      SLLStackPush_N(arena->current, new_block, prev);
      new_block->base_pos = current->base_pos + current->res;
      current = new_block;
    }
  }  
  void *memory = (U8*)current + current->pos;
  current->pos = current->pos + size;
  return memory;
}

internal void  
arena_pop_to(Arena *arena, U64 big_pos_unclamped)
{
  U64 big_pos = ClampBottom(big_pos_unclamped, ARENA_HEADER_SIZE);
  Arena *current = arena->current;
  for(; current->base_pos >= big_pos; current = current->prev) {
    os_release((void *)current, current->res);
  }
  AssertAlways(current);
  U64 new_pos = big_pos - current->base_pos;
  AssertAlways(new_pos <= current->pos);
  current->pos = new_pos;
}

internal void  
arena_put_back(Arena *arena, U64 size)
{ 
  U64 old_pos = arena_pos_addr(arena);
  U64 new_pos = old_pos - size;
  arena_pop_to(arena, new_pos);
}

internal void
arena_clear(Arena *arena)
{
  arena_pop_to(arena, 0);
}

internal Temp
temp_begin(Arena *arena)
{ 
  Temp result = { arena, arena->pos };
  return result;
}

internal void
temp_end(Temp temp)
{ 
  arena_pop_to(temp.arena, temp.pos);
}