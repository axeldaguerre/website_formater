////////////////////////////////
//~ rjf: Atlas Allocator Functions

internal Atlas *
AtlasMake(Arena *arena, Vec2S64 dim)
{
 Atlas *atlas = PushArray(arena, Atlas, 1);
 atlas->root_dim = dim;
 atlas->root = PushArray(arena, AtlasRegionNode, 1);
 atlas->root->max_free_size[Corner_00] = 
  atlas->root->max_free_size[Corner_01] = 
  atlas->root->max_free_size[Corner_10] = 
  atlas->root->max_free_size[Corner_11] = V2S64(dim.x/2, dim.y/2);
 return atlas;
}

internal Rng2S64
AtlasRegionAlloc(Arena *arena, Atlas *atlas, Vec2S64 needed_size)
{
 //- rjf: find node with best-fit size
 Vec2S64 region_p0 = {0};
 Vec2S64 region_sz = {0};
 Corner node_corner = Corner_Invalid;
 AtlasRegionNode *node = 0;
 {
  Vec2S64 n_supported_size = atlas->root_dim;
  for(AtlasRegionNode *n = atlas->root, *next = 0; n != 0; n = next, next = 0)
  {
   // rjf: we've traversed to a taken node.
   if(n->flags & AtlasRegionNodeFlag_Taken)
   {
    break;
   }
   
   // rjf: calculate if this node can be allocated (all children are non-allocated)
   B32 n_can_be_allocated = (n->num_allocated_descendants == 0);
   
   // rjf: fill size
   if(n_can_be_allocated)
   {
    region_sz = n_supported_size;
   }
   
   // rjf: calculate size of this node's children
   Vec2S64 child_size = {n_supported_size.x/2, n_supported_size.y/2};
   
   // rjf: find best next child
   AtlasRegionNode *best_child = 0;
   if(child_size.x >= needed_size.x && child_size.y >= needed_size.y)
   {
    for(Corner corner = (Corner)0; corner < Corner_COUNT; corner = (Corner)(corner+1))
    {
     if(n->children[corner] == 0)
     {
      n->children[corner] = PushArray(arena, AtlasRegionNode, 1);
      n->children[corner]->parent = n;
      n->children[corner]->max_free_size[Corner_00] = 
       n->children[corner]->max_free_size[Corner_01] = 
       n->children[corner]->max_free_size[Corner_10] = 
       n->children[corner]->max_free_size[Corner_11] = V2S64(child_size.x/2, child_size.y/2);
     }
     if(n->max_free_size[corner].x >= needed_size.x && 
        n->max_free_size[corner].y >= needed_size.y)
     {
      best_child = n->children[corner];
      node_corner = corner;
      Vec2S64 side_vertex = SideVertexFromCorner(corner);
      region_p0.x += side_vertex.x*child_size.x;
      region_p0.y += side_vertex.y*child_size.y;
      break;
     }
    }
   }
   
   // rjf: resolve node to this node if it can be allocated and children
   // don't fit, or keep going to the next best child
   if(n_can_be_allocated && best_child == 0)
   {
    node = n;
   }
   else
   {
    next = best_child;
    n_supported_size = child_size;
   }
  }
 }
 
 //- rjf: we're taking the subtree rooted by `node`. mark up all parents
 if(node != 0 && node_corner != Corner_Invalid)
 {
  node->flags |= AtlasRegionNodeFlag_Taken;
  if(node->parent != 0)
  {
   MemoryZeroStruct(&node->parent->max_free_size[node_corner]);
  }
  for(AtlasRegionNode *p = node->parent; p != 0; p = p->parent)
  {
   p->num_allocated_descendants += 1;
   AtlasRegionNode *parent = p->parent;
   if(parent != 0)
   {
    Corner p_corner = (p == parent->children[Corner_00] ? Corner_00 :
                       p == parent->children[Corner_01] ? Corner_01 :
                       p == parent->children[Corner_10] ? Corner_10 :
                       p == parent->children[Corner_11] ? Corner_11 :
                       Corner_Invalid);
    if(p_corner == Corner_Invalid)
    {
     InvalidPath;
    }
    parent->max_free_size[p_corner].x = Max(Max(p->max_free_size[Corner_00].x,
                                                p->max_free_size[Corner_01].x),
                                            Max(p->max_free_size[Corner_10].x,
                                                p->max_free_size[Corner_11].x));
    parent->max_free_size[p_corner].y = Max(Max(p->max_free_size[Corner_00].y,
                                                p->max_free_size[Corner_01].y),
                                            Max(p->max_free_size[Corner_10].y,
                                                p->max_free_size[Corner_11].y));
   }
  }
 }
 
 //- rjf: fill rectangular region & return
 Rng2S64 result = R2S64(region_p0, Add2S64(region_p0, region_sz));
 return result;
}

internal void
AtlasRegionRelease(Atlas *atlas, Rng2S64 region)
{
 //- rjf: extract region size
 Vec2S64 region_size = Dim2S64(region);
 
 //- rjf: map region to associated node
 Vec2S64 calc_region_size = {0};
 AtlasRegionNode *node = 0;
 Corner node_corner = Corner_Invalid;
 {
  Vec2S64 n_p0 = V2S64(0, 0);
  Vec2S64 n_sz = atlas->root_dim;
  for(AtlasRegionNode *n = atlas->root, *next = 0; n != 0; n = next)
  {
   // rjf: is the region within this node's boundaries? (either this node, or a descendant)
   if(n_p0.x <= region.p0.x && region.p0.x < n_p0.x+n_sz.x &&
      n_p0.y <= region.p0.y && region.p0.y < n_p0.y+n_sz.y)
   {
    // rjf: check the region against this node
    if(region.p0.x == n_p0.x && region.p0.y == n_p0.y && 
       region_size.x == n_sz.x && region_size.y == n_sz.y)
    {
     node = n;
     calc_region_size = n_sz;
     break;
    }
    // rjf: check the region against children & iterate
    else
    {
     Vec2S64 r_midpoint =
     {
      region.p0.x + region_size.x/2,
      region.p0.y + region_size.y/2,
     };
     Vec2S64 n_midpoint =
     {
      n_p0.x + n_sz.x/2,
      n_p0.y + n_sz.y/2,
     };
     Corner next_corner = Corner_Invalid;
     if(r_midpoint.x <= n_midpoint.x && r_midpoint.y <= n_midpoint.y)
     {
      next_corner = Corner_00;
     }
     else if(r_midpoint.x <= n_midpoint.x && n_midpoint.y <= r_midpoint.y)
     {
      next_corner = Corner_01;
     }
     else if(n_midpoint.x <= r_midpoint.x && r_midpoint.y <= n_midpoint.y)
     {
      next_corner = Corner_10;
     }
     else if(n_midpoint.x <= r_midpoint.x && n_midpoint.y <= r_midpoint.y)
     {
      next_corner = Corner_11;
     }
     next = n->children[next_corner];
     node_corner = next_corner;
     n_sz.x /= 2;
     n_sz.y /= 2;
     Vec2S64 side_vertex = SideVertexFromCorner(node_corner);
     n_p0.x += side_vertex.x*n_sz.x;
     n_p0.y += side_vertex.y*n_sz.y;
    }
   }
   else
   {
    break; 
   }
  }
 }
 
 //- rjf: free node
 if(node != 0 && node_corner != Corner_Invalid)
 {
  node->flags &= ~AtlasRegionNodeFlag_Taken;
  if(node->parent != 0)
  {
   node->parent->max_free_size[node_corner] = calc_region_size;
  }
  for(AtlasRegionNode *p = node->parent; p != 0; p = p->parent)
  {
   p->num_allocated_descendants -= 1;
   AtlasRegionNode *parent = p->parent;
   if(parent != 0)
   {
    Corner p_corner = (p == parent->children[Corner_00] ? Corner_00 :
                       p == parent->children[Corner_01] ? Corner_01 :
                       p == parent->children[Corner_10] ? Corner_10 :
                       p == parent->children[Corner_11] ? Corner_11 :
                       Corner_Invalid);
    if(p_corner == Corner_Invalid)
    {
     InvalidPath;
    }
    parent->max_free_size[p_corner].x = Max(Max(p->max_free_size[Corner_00].x,
                                                p->max_free_size[Corner_01].x),
                                            Max(p->max_free_size[Corner_10].x,
                                                p->max_free_size[Corner_11].x));
    parent->max_free_size[p_corner].y = Max(Max(p->max_free_size[Corner_00].y,
                                                p->max_free_size[Corner_01].y),
                                            Max(p->max_free_size[Corner_10].y,
                                                p->max_free_size[Corner_11].y));
   }
  }
 }
}
