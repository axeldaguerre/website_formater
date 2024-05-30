////////////////////////////////
//~ rjf: Geometry Generators

internal VertexIndexArrayPair
IcoSphereMake(Arena *arena, U64 num_subdivide_passes)
{
 VertexIndexArrayPair result = {0};
 {
  //- rjf: built-in 20-triangle icosahedron vertices/indices
  local_persist read_only Vec3F32 icosahedron_vertices[] =
  {
#define face_size 1.618034f // (1 + sqrt(5)) / 2
   {-1.f, +face_size, +0.f},
   {+1.f, +face_size, +0.f},
   {-1.f, -face_size, +0.f},
   {+1.f, -face_size, +0.f},
   {+0.f, -1.f, +face_size},
   {+0.f, +1.f, +face_size},
   {+0.f, -1.f, -face_size},
   {+0.f, +1.f, -face_size},
   {+face_size, +0.f, -1.f},
   {+face_size, +0.f, +1.f},
   {-face_size, +0.f, -1.f},
   {-face_size, +0.f, +1.f},
#undef face_size
  };
  local_persist read_only S32 icosahedron_indices[] =
  {
   0, 11, 5, 0, 5, 1, 0, 1, 7,
   0, 7, 10, 0, 10, 11, 11, 10, 2,
   5, 11, 4, 1, 5, 9, 7, 1, 8,
   10, 7, 6, 3, 9, 4, 3, 4, 2,
   3, 2, 6, 3, 6, 8, 3, 8, 9,
   9, 8, 1, 4, 9, 5, 2, 4, 11,
   6, 2, 10, 8, 6, 7
  };
  
  //- rjf: determine vertex/index counts
  U64 vertex_count = ArrayCount(icosahedron_vertices);
  U64 index_count  = ArrayCount(icosahedron_indices);
  
  //- rjf: set up buffers
  result.vertices.v = PushArrayNoZero(arena, Vec3F32, vertex_count);
  result.vertices.count = vertex_count;
  result.indices.v = PushArrayNoZero(arena, S32, index_count);
  result.indices.count = index_count;
  Vec3F32 *vtx = result.vertices.v;
  U64 vtx_write_idx = 0;
  S32 *idx = result.indices.v;
  U64 idx_write_idx = 0;
  
  //- rjf: write initial icosahedron vertices/indices
  MemoryCopy(vtx, icosahedron_vertices, sizeof(icosahedron_vertices));
  MemoryCopy(idx, icosahedron_indices, sizeof(icosahedron_indices));
  vtx_write_idx += ArrayCount(icosahedron_vertices);
  idx_write_idx += ArrayCount(icosahedron_indices);
  
  //- rjf: subdivide
  // TODO(rjf): subdivisions
 }
 return result;
}
