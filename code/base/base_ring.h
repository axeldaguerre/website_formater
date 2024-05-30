/* date = September 8th 2022 11:54 pm */

#ifndef BASE_RING_H
#define BASE_RING_H

////////////////////////////////
//~ rjf: Ring Buffer Functions

internal U64 RingWrite(U8 *ring_base, U64 ring_size, U64 pos, void *src, U64 write_size);
internal U64 RingRead(U8 *ring_base, U64 ring_size, U64 pos, void *dst, U64 read_size);
#define RingWriteStruct(base, size, pos, ptr) RingWrite((base), (size), (pos), (ptr), sizeof(*(ptr)))
#define RingReadStruct(base, size, pos, ptr) RingRead((base), (size), (pos), (ptr), sizeof(*(ptr)))

#endif // BASE_RING_H
