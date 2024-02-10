#ifndef BASE_LINKED_LIST_H
#define BASE_LINKED_LIST_H

#define SetNil(nil, p) ((p)=nil)

#define SLLPush(f,l,n) (f == 0) ?\
  (f)=(l)=(n):\
  ((l)->next=(n), (l)=(n), SetNil(0,(n)->next))

#define SLLQueuePop_NZ(nil,f,l,next) ((f)==(l)?\
(SetNil(nil,f),SetNil(nil,l)):\
((f)=(f)->next))

#define SLLStackPush_N(f,n,field) ((n)->field=(f), (f)=(n))
#define SLLStackPop_N(f,field) ((f)=(f)->field)

#define SLLConvert(f,n,field) ((n)->field=(f), (f)=(n))

#endif