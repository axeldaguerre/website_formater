////////////////////////////////
//~ rjf: Ring Functions

internal U64
RingWrite(U8 *ring_base, U64 ring_size, U64 pos, void *src, U64 write_size)
{
 write_size = Min(write_size, ring_size);
 U64 first_part_write_off = pos%ring_size;
 U64 second_part_write_off = 0;
 String8 first_part = Str8((U8 *)src, write_size);
 String8 second_part = Str8Lit("");
 if(first_part_write_off + write_size > ring_size)
 {
  first_part.size = ring_size - first_part_write_off;
  second_part = Str8((U8 *)src + first_part.size, write_size-first_part.size);
 }
 if(first_part.size != 0)
 {
  MemoryCopy(ring_base + first_part_write_off, first_part.str, first_part.size);
 }
 if(second_part.size != 0)
 {
  MemoryCopy(ring_base + second_part_write_off, second_part.str, second_part.size);
 }
 return write_size;
}

internal U64
RingRead(U8 *ring_base, U64 ring_size, U64 pos, void *dst, U64 read_size)
{
 read_size = Min(read_size, ring_size);
 U64 first_part_read_off = pos%ring_size;
 U64 second_part_read_off = 0;
 U64 first_part_read_size = read_size;
 U64 second_part_read_size = 0;
 if(first_part_read_off + read_size > ring_size)
 {
  first_part_read_size = ring_size - first_part_read_off;
  second_part_read_size = read_size - first_part_read_size;
 }
 if(first_part_read_size != 0)
 {
  MemoryCopy(dst, ring_base + first_part_read_off, first_part_read_size);
 }
 if(second_part_read_size != 0)
 {
  MemoryCopy((U8 *)dst + first_part_read_size, ring_base + second_part_read_off, second_part_read_size);
 }
 return read_size;
}
