//- rjf: main thread entry point

internal void
BaseMainThreadEntry(void (*entry)(CmdLine *cmdln), U64 argument_count, char **arguments)
{
#if BUILD_TELEMETRY
 local_persist U8 tm_data[Megabytes(64)];
 tmLoadLibrary(TM_RELEASE);
 tmSetMaxThreadCount(256);
 tmInitialize(sizeof(tm_data), (char *)tm_data);
#endif
 ProfThreadName("[main thread]");
 ThreadCtx tctx = ThreadCtxAlloc();
 tctx.is_main_thread = 1;
 SetThreadCtx(&tctx);
 String8List args_list = {0};
 for(U64 argument_idx = 1; argument_idx < argument_count; argument_idx += 1)
 {
  Str8ListPush(tctx.scratch_arenas[0], &args_list, Str8C(arguments[argument_idx]));
 }
 CmdLine cmdline = CmdLineFromStringList(tctx.scratch_arenas[0], args_list);
 B32 capture = CmdLineOptB32(&cmdline, Str8Lit("capture"));
 if(capture)
 {
  ProfBeginCapture(arguments[0]);
 }
 SetThreadName(Str8Lit("Main Thread"));
#if defined(OS_CORE_H)
 OS_InitReceipt    os_init     = OS_Init();
#endif
#if defined(CONTENT_CORE_H)
 C_InitReceipt     c_init      = C_Init(os_init);
#endif
#if defined(FILE_STREAM_H)
 FS_InitReceipt    fs_init     = FS_Init(os_init, c_init);
#endif
#if defined(BITMAP_CACHE_H)
 BC_InitReceipt    bc_init     = BC_Init(os_init, c_init);
#endif
#if defined(OS_GFX_H)
 OS_InitGfxReceipt os_init_gfx = OS_InitGfx(os_init);
#endif
#if defined(OS_NET_H)
 OS_InitNetReceipt os_init_net = OS_InitNet(os_init);
#endif
#if defined(RENDER_CORE_H)
 R_InitReceipt     r_init      = R_Init(os_init, os_init_gfx);
#endif
#if defined(TEXTURE_CACHE_H)
 T_InitReceipt     t_init      = T_Init(c_init, r_init);
#endif
#if defined(FONT_PROVIDER_CORE_H)
 FP_InitReceipt    fp_init     = FP_Init(c_init);
#endif
#if defined(FONT_CACHE_H)
 F_InitReceipt     f_init      = F_Init(fp_init, r_init, V2S64(2048, 2048));
#endif
#if defined(DRAW_H)
 D_InitReceipt     d_init      = D_Init(r_init, f_init);
#endif
#if defined(ANIMATION_CURVE_H)
 AC_InitThread();
#endif
 entry(&cmdline);
 ThreadCtxRelease(&tctx);
 if(capture)
 {
  ProfEndCapture();
 }
}

//- rjf: non-main-thread entry point

internal void
BaseThreadEntry(void (*entry)(void *p), void *params)
{
 ThreadCtx tctx = ThreadCtxAlloc();
 SetThreadCtx(&tctx);
#if defined(ANIMATION_CURVE_H)
 AC_InitThread();
#endif
 entry(params);
 ThreadCtxRelease(&tctx);
}
