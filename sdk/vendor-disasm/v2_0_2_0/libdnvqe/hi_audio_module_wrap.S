
hi_audio_module_wrap.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l       .rodata.MODULE_HI_Audio_ModuleHandleInit.str1.1	00000000 .LC0
0000000c l       .rodata.MODULE_HI_Audio_ModuleHandleInit.str1.1	00000000 .LC1
00000000 l    d  .bss.s_audio_module_mutex	00000000 .bss.s_audio_module_mutex
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.MODULE_HI_Audio_ModuleHandleInit	00000000 .text.MODULE_HI_Audio_ModuleHandleInit
00000000 l    d  .text.MODULE_HI_Audio_ModuleHandleDeInit	00000000 .text.MODULE_HI_Audio_ModuleHandleDeInit
00000000 l    d  .rodata.MODULE_HI_Audio_ModuleHandleInit.str1.1	00000000 .rodata.MODULE_HI_Audio_ModuleHandleInit.str1.1
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.MODULE_HI_Audio_ModuleHandleInit	00000148 MODULE_HI_Audio_ModuleHandleInit
00000000         *UND*	00000000 pthread_mutex_lock
00000000         *UND*	00000000 memset
00000000         *UND*	00000000 snprintf_s
00000000         *UND*	00000000 DL_HI_Audio_Dlopen
00000000         *UND*	00000000 pthread_mutex_unlock
00000000         *UND*	00000000 __stack_chk_fail
00000000         *UND*	00000000 DL_HI_Audio_Dlsym
00000000         *UND*	00000000 DL_HI_Audio_Dlclose
00000000         *UND*	00000000 strlen
00000000         *UND*	00000000 memcpy_s
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 __stack_chk_guard
00000000 g     F .text.MODULE_HI_Audio_ModuleHandleDeInit	0000003c MODULE_HI_Audio_ModuleHandleDeInit



Disassembly of section .text.MODULE_HI_Audio_ModuleHandleInit:

00000000 <MODULE_HI_Audio_ModuleHandleInit>:
   0:	e59f312c 	ldr	r3, [pc, #300]	; 134 <MODULE_HI_Audio_ModuleHandleInit+0x134>
   4:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
   8:	e1a09002 	mov	r9, r2
   c:	e08f3003 	add	r3, pc, r3
  10:	e24ddf46 	sub	sp, sp, #280	; 0x118
  14:	e59f211c 	ldr	r2, [pc, #284]	; 138 <MODULE_HI_Audio_ModuleHandleInit+0x138>
  18:	e3a08000 	mov	r8, #0
  1c:	e59f6118 	ldr	r6, [pc, #280]	; 13c <MODULE_HI_Audio_ModuleHandleInit+0x13c>
  20:	e28d5010 	add	r5, sp, #16
  24:	e7937002 	ldr	r7, [r3, r2]
  28:	e1a04000 	mov	r4, r0
  2c:	e08f6006 	add	r6, pc, r6
  30:	e1a0a001 	mov	sl, r1
  34:	e5973000 	ldr	r3, [r7]
  38:	e1a00006 	mov	r0, r6
  3c:	e58d800c 	str	r8, [sp, #12]
  40:	e58d3114 	str	r3, [sp, #276]	; 0x114
  44:	ebfffffe 	bl	0 <pthread_mutex_lock>
			44: R_ARM_CALL	pthread_mutex_lock
  48:	e1a01008 	mov	r1, r8
  4c:	e3002101 	movw	r2, #257	; 0x101
  50:	e1a00005 	mov	r0, r5
  54:	ebfffffe 	bl	0 <memset>
			54: R_ARM_CALL	memset
  58:	e59f30e0 	ldr	r3, [pc, #224]	; 140 <MODULE_HI_Audio_ModuleHandleInit+0x140>
  5c:	e3a02c01 	mov	r2, #256	; 0x100
  60:	e58d9004 	str	r9, [sp, #4]
  64:	e3001101 	movw	r1, #257	; 0x101
  68:	e08f3003 	add	r3, pc, r3
  6c:	e1a00005 	mov	r0, r5
  70:	e58da000 	str	sl, [sp]
  74:	ebfffffe 	bl	0 <snprintf_s>
			74: R_ARM_CALL	snprintf_s
  78:	e1a01005 	mov	r1, r5
  7c:	e2840018 	add	r0, r4, #24
  80:	ebfffffe 	bl	0 <DL_HI_Audio_Dlopen>
			80: R_ARM_CALL	DL_HI_Audio_Dlopen
  84:	e2508000 	subs	r8, r0, #0
  88:	0a000007 	beq	ac <MODULE_HI_Audio_ModuleHandleInit+0xac>
  8c:	e1a00006 	mov	r0, r6
  90:	ebfffffe 	bl	0 <pthread_mutex_unlock>
			90: R_ARM_CALL	pthread_mutex_unlock
  94:	e59d2114 	ldr	r2, [sp, #276]	; 0x114
  98:	e1a00008 	mov	r0, r8
  9c:	e5973000 	ldr	r3, [r7]
  a0:	e1520003 	cmp	r2, r3
  a4:	0a000020 	beq	12c <MODULE_HI_Audio_ModuleHandleInit+0x12c>
  a8:	ebfffffe 	bl	0 <__stack_chk_fail>
			a8: R_ARM_CALL	__stack_chk_fail
  ac:	e59f3090 	ldr	r3, [pc, #144]	; 144 <MODULE_HI_Audio_ModuleHandleInit+0x144>
  b0:	e3a02c01 	mov	r2, #256	; 0x100
  b4:	e58d9000 	str	r9, [sp]
  b8:	e3001101 	movw	r1, #257	; 0x101
  bc:	e08f3003 	add	r3, pc, r3
  c0:	e1a00005 	mov	r0, r5
  c4:	ebfffffe 	bl	0 <snprintf_s>
			c4: R_ARM_CALL	snprintf_s
  c8:	e5941018 	ldr	r1, [r4, #24]
  cc:	e1a02005 	mov	r2, r5
  d0:	e28d000c 	add	r0, sp, #12
  d4:	ebfffffe 	bl	0 <DL_HI_Audio_Dlsym>
			d4: R_ARM_CALL	DL_HI_Audio_Dlsym
  d8:	e2508000 	subs	r8, r0, #0
  dc:	0a000002 	beq	ec <MODULE_HI_Audio_ModuleHandleInit+0xec>
  e0:	e5940018 	ldr	r0, [r4, #24]
  e4:	ebfffffe 	bl	0 <DL_HI_Audio_Dlclose>
			e4: R_ARM_CALL	DL_HI_Audio_Dlclose
  e8:	eaffffe7 	b	8c <MODULE_HI_Audio_ModuleHandleInit+0x8c>
  ec:	e59d500c 	ldr	r5, [sp, #12]
  f0:	e1a00005 	mov	r0, r5
  f4:	ebfffffe 	bl	0 <strlen>
			f4: R_ARM_CALL	strlen
  f8:	e2803001 	add	r3, r0, #1
  fc:	e1a02005 	mov	r2, r5
 100:	e3a01014 	mov	r1, #20
 104:	e1a00004 	mov	r0, r4
 108:	ebfffffe 	bl	0 <memcpy_s>
			108: R_ARM_CALL	memcpy_s
 10c:	e59d300c 	ldr	r3, [sp, #12]
 110:	e593201c 	ldr	r2, [r3, #28]
 114:	e584201c 	str	r2, [r4, #28]
 118:	e5932020 	ldr	r2, [r3, #32]
 11c:	e5933024 	ldr	r3, [r3, #36]	; 0x24
 120:	e5842020 	str	r2, [r4, #32]
 124:	e5843024 	str	r3, [r4, #36]	; 0x24
 128:	eaffffd7 	b	8c <MODULE_HI_Audio_ModuleHandleInit+0x8c>
 12c:	e28ddf46 	add	sp, sp, #280	; 0x118
 130:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
 134:	00000120 	.word	0x00000120
			134: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 138:	00000000 	.word	0x00000000
			138: R_ARM_GOT32	__stack_chk_guard
 13c:	00000108 	.word	0x00000108
			13c: R_ARM_REL32	.bss.s_audio_module_mutex
 140:	000000d0 	.word	0x000000d0
			140: R_ARM_REL32	.LC0
 144:	00000080 	.word	0x00000080
			144: R_ARM_REL32	.LC1

Disassembly of section .text.MODULE_HI_Audio_ModuleHandleDeInit:

00000000 <MODULE_HI_Audio_ModuleHandleDeInit>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e1a06000 	mov	r6, r0
   8:	e59f5028 	ldr	r5, [pc, #40]	; 38 <MODULE_HI_Audio_ModuleHandleDeInit+0x38>
   c:	e3a04000 	mov	r4, #0
  10:	e08f5005 	add	r5, pc, r5
  14:	e1a00005 	mov	r0, r5
  18:	ebfffffe 	bl	0 <pthread_mutex_lock>
			18: R_ARM_CALL	pthread_mutex_lock
  1c:	e5960018 	ldr	r0, [r6, #24]
  20:	ebfffffe 	bl	0 <DL_HI_Audio_Dlclose>
			20: R_ARM_CALL	DL_HI_Audio_Dlclose
  24:	e5864018 	str	r4, [r6, #24]
  28:	e1a00005 	mov	r0, r5
  2c:	ebfffffe 	bl	0 <pthread_mutex_unlock>
			2c: R_ARM_CALL	pthread_mutex_unlock
  30:	e1a00004 	mov	r0, r4
  34:	e8bd8070 	pop	{r4, r5, r6, pc}
  38:	00000020 	.word	0x00000020
			38: R_ARM_REL32	.bss.s_audio_module_mutex
