//- GENERATED CODE

#ifndef DRAW_META_H
#define DRAW_META_H

typedef struct D_Tex2DSampleKindNode D_Tex2DSampleKindNode; struct D_Tex2DSampleKindNode {D_Tex2DSampleKindNode *next; R_Tex2DSampleKind v;};
typedef struct D_BlendModeNode D_BlendModeNode; struct D_BlendModeNode {D_BlendModeNode *next; R_BlendMode v;};
typedef struct D_Transform2DNode D_Transform2DNode; struct D_Transform2DNode {D_Transform2DNode *next; Mat3x3F32 v;};
typedef struct D_ClipNode D_ClipNode; struct D_ClipNode {D_ClipNode *next; Rng2F32 v;};
typedef struct D_TransparencyNode D_TransparencyNode; struct D_TransparencyNode {D_TransparencyNode *next; F32 v;};
#define D_DeclThreadStackTops \
struct\
{\
D_Tex2DSampleKindNode tex2d_sample_kind_nil_stack_top;\
D_BlendModeNode blend_mode_nil_stack_top;\
D_Transform2DNode xform2d_nil_stack_top;\
D_ClipNode clip_nil_stack_top;\
D_TransparencyNode transparency_nil_stack_top;\
}
#define D_InitThreadStackTops \
d_thread_ctx->tex2d_sample_kind_nil_stack_top.v = R_Tex2DSampleKind_Nearest;\
d_thread_ctx->blend_mode_nil_stack_top.v = R_BlendMode_Normal;\
d_thread_ctx->xform2d_nil_stack_top.v = MakeMat3x3F32(1.f);\
d_thread_ctx->clip_nil_stack_top.v = R2F32(V2F32(0, 0), V2F32(0, 0));\
d_thread_ctx->transparency_nil_stack_top.v = 0.f;\

#define D_DeclBucketStacks \
struct\
{\
D_Tex2DSampleKindNode *tex2d_sample_kind_stack_top; D_Tex2DSampleKindNode *tex2d_sample_kind_free;\
D_BlendModeNode *blend_mode_stack_top; D_BlendModeNode *blend_mode_free;\
D_Transform2DNode *xform2d_stack_top; D_Transform2DNode *xform2d_free;\
D_ClipNode *clip_stack_top; D_ClipNode *clip_free;\
D_TransparencyNode *transparency_stack_top; D_TransparencyNode *transparency_free;\
}
#define D_InitBucketStacks(b) \
(b)->tex2d_sample_kind_stack_top = &d_thread_ctx->tex2d_sample_kind_nil_stack_top;\
(b)->blend_mode_stack_top = &d_thread_ctx->blend_mode_nil_stack_top;\
(b)->xform2d_stack_top = &d_thread_ctx->xform2d_nil_stack_top;\
(b)->clip_stack_top = &d_thread_ctx->clip_nil_stack_top;\
(b)->transparency_stack_top = &d_thread_ctx->transparency_nil_stack_top;\

#if 0
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
#endif

#endif // DRAW_META_H
