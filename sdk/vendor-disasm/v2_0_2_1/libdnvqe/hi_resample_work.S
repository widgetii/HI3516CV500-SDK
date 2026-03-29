
hi_resample_work.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l     F .text.update_cache.isra.0	000000bc update_cache.isra.0
00000000 l       .rodata.update_cache.isra.0.str1.1	00000000 .LC0
00000000 l       .rodata.RES_resampler_create.str1.1	00000000 .LC1
00000033 l       .rodata.RES_resampler_create.str1.1	00000000 .LC2
00000037 l       .rodata.RES_resampler_create.str1.1	00000000 .LC3
0000003c l       .rodata.RES_resampler_create.str1.1	00000000 .LC4
00000073 l       .rodata.RES_resampler_create.str1.1	00000000 .LC5
000000a8 l       .rodata.RES_resampler_create.str1.1	00000000 .LC6
000000e0 l       .rodata.RES_resampler_create.str1.1	00000000 .LC7
0000011c l       .rodata.RES_resampler_create.str1.1	00000000 .LC8
00000000 l       .rodata.RES_resampler_process_frame.str1.1	00000000 .LC9
00000045 l       .rodata.RES_resampler_process_frame.str1.1	00000000 .LC10
00000085 l       .rodata.RES_resampler_process_frame.str1.1	00000000 .LC11
000000bd l       .rodata.RES_resampler_process_frame.str1.1	00000000 .LC12
00000000 l    d  .rodata.__FUNCTION__.8166	00000000 .rodata.__FUNCTION__.8166
00000000 l    d  .rodata.__FUNCTION__.8176	00000000 .rodata.__FUNCTION__.8176
00000000 l    d  .rodata.__FUNCTION__.8194	00000000 .rodata.__FUNCTION__.8194
00000000 l    d  .rodata.__FUNCTION__.8210	00000000 .rodata.__FUNCTION__.8210
00000000 l    d  .rodata.__FUNCTION__.8220	00000000 .rodata.__FUNCTION__.8220
00000000 l    d  .rodata.__FUNCTION__.8232	00000000 .rodata.__FUNCTION__.8232
00000000 l    d  .rodata.__FUNCTION__.8242	00000000 .rodata.__FUNCTION__.8242
00000000 l    d  .rodata.__FUNCTION__.8254	00000000 .rodata.__FUNCTION__.8254
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.update_cache.isra.0	00000000 .text.update_cache.isra.0
00000000 l    d  .text.RES_resampler_create	00000000 .text.RES_resampler_create
00000000 l    d  .text.RES_resampler_destory	00000000 .text.RES_resampler_destory
00000000 l    d  .text.RES_resampler_get_input_num	00000000 .text.RES_resampler_get_input_num
00000000 l    d  .text.RES_resampler_process_frame	00000000 .text.RES_resampler_process_frame
00000000 l    d  .rodata.RES_resampler_create.str1.1	00000000 .rodata.RES_resampler_create.str1.1
00000000 l    d  .rodata.RES_resampler_process_frame.str1.1	00000000 .rodata.RES_resampler_process_frame.str1.1
00000000 l    d  .rodata.update_cache.isra.0.str1.1	00000000 .rodata.update_cache.isra.0.str1.1
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000         *UND*	00000000 memcpy_s
00000000         *UND*	00000000 fprintf
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 stderr
00000000 g     F .text.RES_resampler_create	00000360 RES_resampler_create
00000000         *UND*	00000000 __stack_chk_fail
00000000         *UND*	00000000 malloc
00000000         *UND*	00000000 memset_s
00000000         *UND*	00000000 MODULE_hi_audio_module_handle_init
00000000         *UND*	00000000 free
00000000         *UND*	00000000 MODULE_hi_audio_module_handle_deinit
00000000         *UND*	00000000 __stack_chk_guard
00000000 g     F .text.RES_resampler_destory	00000090 RES_resampler_destory
00000000 g     F .text.RES_resampler_get_input_num	00000048 RES_resampler_get_input_num
00000000 g     F .text.RES_resampler_process_frame	00000738 RES_resampler_process_frame
00000000         *UND*	00000000 memmove_s



Disassembly of section .text.update_cache.isra.0:

00000000 <update_cache.isra.0>:
   0:	e92d41f3 	push	{r0, r1, r4, r5, r6, r7, r8, lr}
   4:	e1a07003 	mov	r7, r3
   8:	e5903014 	ldr	r3, [r0, #20]
   c:	e1a05002 	mov	r5, r2
  10:	e59f8094 	ldr	r8, [pc, #148]	; ac <update_cache.isra.0+0xac>
  14:	e3530000 	cmp	r3, #0
  18:	e08f8008 	add	r8, pc, r8
  1c:	da000013 	ble	70 <update_cache.isra.0+0x70>
  20:	e5902018 	ldr	r2, [r0, #24]
  24:	e1a06001 	mov	r6, r1
  28:	e1a03083 	lsl	r3, r3, #1
  2c:	e1a04000 	mov	r4, r0
  30:	e5960000 	ldr	r0, [r6]
  34:	e1a01003 	mov	r1, r3
  38:	ebfffffe 	bl	0 <memcpy_s>
			38: R_ARM_CALL	memcpy_s
  3c:	e3500000 	cmp	r0, #0
  40:	0a00000d 	beq	7c <update_cache.isra.0+0x7c>
  44:	e59f3064 	ldr	r3, [pc, #100]	; b0 <update_cache.isra.0+0xb0>
  48:	e3a05000 	mov	r5, #0
  4c:	e59f2060 	ldr	r2, [pc, #96]	; b4 <update_cache.isra.0+0xb4>
  50:	e798c003 	ldr	ip, [r8, r3]
  54:	e3003113 	movw	r3, #275	; 0x113
  58:	e59f1058 	ldr	r1, [pc, #88]	; b8 <update_cache.isra.0+0xb8>
  5c:	e58d0000 	str	r0, [sp]
  60:	e08f2002 	add	r2, pc, r2
  64:	e08f1001 	add	r1, pc, r1
  68:	e59c0000 	ldr	r0, [ip]
  6c:	ebfffffe 	bl	0 <fprintf>
			6c: R_ARM_CALL	fprintf
  70:	e1a00005 	mov	r0, r5
  74:	e28dd008 	add	sp, sp, #8
  78:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
  7c:	e5942014 	ldr	r2, [r4, #20]
  80:	e5973000 	ldr	r3, [r7]
  84:	e0433002 	sub	r3, r3, r2
  88:	e5873000 	str	r3, [r7]
  8c:	e5943014 	ldr	r3, [r4, #20]
  90:	e5962000 	ldr	r2, [r6]
  94:	e1a03083 	lsl	r3, r3, #1
  98:	e0822003 	add	r2, r2, r3
  9c:	e5862000 	str	r2, [r6]
  a0:	e0855003 	add	r5, r5, r3
  a4:	e5840014 	str	r0, [r4, #20]
  a8:	eafffff0 	b	70 <update_cache.isra.0+0x70>
  ac:	0000008c 	.word	0x0000008c
			ac: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  b0:	00000000 	.word	0x00000000
			b0: R_ARM_GOT32	stderr
  b4:	0000004c 	.word	0x0000004c
			b4: R_ARM_REL32	.rodata.__FUNCTION__.8210
  b8:	0000004c 	.word	0x0000004c
			b8: R_ARM_REL32	.LC0

Disassembly of section .text.RES_resampler_create:

00000000 <RES_resampler_create>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e1a06002 	mov	r6, r2
   8:	e59f22fc 	ldr	r2, [pc, #764]	; 30c <RES_resampler_create+0x30c>
   c:	e1a0b003 	mov	fp, r3
  10:	e59f32f8 	ldr	r3, [pc, #760]	; 310 <RES_resampler_create+0x310>
  14:	e24dd024 	sub	sp, sp, #36	; 0x24
  18:	e08f2002 	add	r2, pc, r2
  1c:	e1a0a000 	mov	sl, r0
  20:	e59f82ec 	ldr	r8, [pc, #748]	; 314 <RES_resampler_create+0x314>
  24:	e7923003 	ldr	r3, [r2, r3]
  28:	e1510006 	cmp	r1, r6
  2c:	e08f8008 	add	r8, pc, r8
  30:	e5933000 	ldr	r3, [r3]
  34:	e58d301c 	str	r3, [sp, #28]
  38:	e3a03000 	mov	r3, #0
  3c:	1a00000b 	bne	70 <RES_resampler_create+0x70>
  40:	e3a05000 	mov	r5, #0
  44:	e58a5000 	str	r5, [sl]
  48:	e59f22c8 	ldr	r2, [pc, #712]	; 318 <RES_resampler_create+0x318>
  4c:	e1a00005 	mov	r0, r5
  50:	e59f32b8 	ldr	r3, [pc, #696]	; 310 <RES_resampler_create+0x310>
  54:	e08f2002 	add	r2, pc, r2
  58:	e7923003 	ldr	r3, [r2, r3]
  5c:	e5932000 	ldr	r2, [r3]
  60:	e59d301c 	ldr	r3, [sp, #28]
  64:	e0332002 	eors	r2, r3, r2
  68:	0a0000a5 	beq	304 <RES_resampler_create+0x304>
  6c:	ebfffffe 	bl	0 <__stack_chk_fail>
			6c: R_ARM_CALL	__stack_chk_fail
  70:	e2412d7d 	sub	r2, r1, #8000	; 0x1f40
  74:	e30d3ac0 	movw	r3, #56000	; 0xdac0
  78:	e1520003 	cmp	r2, r3
  7c:	e1a07001 	mov	r7, r1
  80:	8affffee 	bhi	40 <RES_resampler_create+0x40>
  84:	e2462d7d 	sub	r2, r6, #8000	; 0x1f40
  88:	e3093c40 	movw	r3, #40000	; 0x9c40
  8c:	e1520003 	cmp	r2, r3
  90:	8affffea 	bhi	40 <RES_resampler_create+0x40>
  94:	e3060048 	movw	r0, #24648	; 0x6048
  98:	ebfffffe 	bl	0 <malloc>
			98: R_ARM_CALL	malloc
  9c:	e2504000 	subs	r4, r0, #0
  a0:	1a00000a 	bne	d0 <RES_resampler_create+0xd0>
  a4:	e59f3270 	ldr	r3, [pc, #624]	; 31c <RES_resampler_create+0x31c>
  a8:	e59f2270 	ldr	r2, [pc, #624]	; 320 <RES_resampler_create+0x320>
  ac:	e7980003 	ldr	r0, [r8, r3]
  b0:	e3a03073 	mov	r3, #115	; 0x73
  b4:	e59f1268 	ldr	r1, [pc, #616]	; 324 <RES_resampler_create+0x324>
  b8:	e08f2002 	add	r2, pc, r2
  bc:	e08f1001 	add	r1, pc, r1
  c0:	e5900000 	ldr	r0, [r0]
  c4:	e59f525c 	ldr	r5, [pc, #604]	; 328 <RES_resampler_create+0x328>
  c8:	ebfffffe 	bl	0 <fprintf>
			c8: R_ARM_CALL	fprintf
  cc:	eaffffdd 	b	48 <RES_resampler_create+0x48>
  d0:	e3063048 	movw	r3, #24648	; 0x6048
  d4:	e3a02000 	mov	r2, #0
  d8:	e1a01003 	mov	r1, r3
  dc:	e2849a06 	add	r9, r4, #24576	; 0x6000
  e0:	e289901c 	add	r9, r9, #28
  e4:	ebfffffe 	bl	0 <memset_s>
			e4: R_ARM_CALL	memset_s
  e8:	e59f223c 	ldr	r2, [pc, #572]	; 32c <RES_resampler_create+0x32c>
  ec:	e3a03001 	mov	r3, #1
  f0:	e59f1238 	ldr	r1, [pc, #568]	; 330 <RES_resampler_create+0x330>
  f4:	e1a00009 	mov	r0, r9
  f8:	e58d3018 	str	r3, [sp, #24]
  fc:	e3a03000 	mov	r3, #0
 100:	e08f2002 	add	r2, pc, r2
 104:	e08f1001 	add	r1, pc, r1
 108:	e58d3014 	str	r3, [sp, #20]
 10c:	ebfffffe 	bl	0 <MODULE_hi_audio_module_handle_init>
			10c: R_ARM_CALL	MODULE_hi_audio_module_handle_init
 110:	e2505000 	subs	r5, r0, #0
 114:	0a00000b 	beq	148 <RES_resampler_create+0x148>
 118:	e59f31fc 	ldr	r3, [pc, #508]	; 31c <RES_resampler_create+0x31c>
 11c:	e59f2210 	ldr	r2, [pc, #528]	; 334 <RES_resampler_create+0x334>
 120:	e7980003 	ldr	r0, [r8, r3]
 124:	e3a0307e 	mov	r3, #126	; 0x7e
 128:	e59f1208 	ldr	r1, [pc, #520]	; 338 <RES_resampler_create+0x338>
 12c:	e08f2002 	add	r2, pc, r2
 130:	e08f1001 	add	r1, pc, r1
 134:	e5900000 	ldr	r0, [r0]
 138:	ebfffffe 	bl	0 <fprintf>
			138: R_ARM_CALL	fprintf
 13c:	e1a00004 	mov	r0, r4
 140:	ebfffffe 	bl	0 <free>
			140: R_ARM_CALL	free
 144:	eaffffbf 	b	48 <RES_resampler_create+0x48>
 148:	e713f617 	sdiv	r3, r7, r6
 14c:	e0627396 	mls	r2, r6, r3, r7
 150:	e3520000 	cmp	r2, #0
 154:	11a03005 	movne	r3, r5
 158:	e35b0000 	cmp	fp, #0
 15c:	e5843000 	str	r3, [r4]
 160:	1a000032 	bne	230 <RES_resampler_create+0x230>
 164:	e28d2020 	add	r2, sp, #32
 168:	e284ba06 	add	fp, r4, #24576	; 0x6000
 16c:	e1a01007 	mov	r1, r7
 170:	e284000c 	add	r0, r4, #12
 174:	e59b3038 	ldr	r3, [fp, #56]	; 0x38
 178:	e5226010 	str	r6, [r2, #-16]!
 17c:	e12fff33 	blx	r3
 180:	e594300c 	ldr	r3, [r4, #12]
 184:	e3530000 	cmp	r3, #0
 188:	1a00000a 	bne	1b8 <RES_resampler_create+0x1b8>
 18c:	e59f21a8 	ldr	r2, [pc, #424]	; 33c <RES_resampler_create+0x33c>
 190:	e59f11a8 	ldr	r1, [pc, #424]	; 340 <RES_resampler_create+0x340>
 194:	e59f3180 	ldr	r3, [pc, #384]	; 31c <RES_resampler_create+0x31c>
 198:	e08f2002 	add	r2, pc, r2
 19c:	e08f1001 	add	r1, pc, r1
 1a0:	e7980003 	ldr	r0, [r8, r3]
 1a4:	e3a03037 	mov	r3, #55	; 0x37
 1a8:	e58d3000 	str	r3, [sp]
 1ac:	e5900000 	ldr	r0, [r0]
 1b0:	ebfffffe 	bl	0 <fprintf>
			1b0: R_ARM_CALL	fprintf
 1b4:	ea000017 	b	218 <RES_resampler_create+0x218>
 1b8:	e5943000 	ldr	r3, [r4]
 1bc:	e3530000 	cmp	r3, #0
 1c0:	0a000040 	beq	2c8 <RES_resampler_create+0x2c8>
 1c4:	e2833001 	add	r3, r3, #1
 1c8:	e1a03083 	lsl	r3, r3, #1
 1cc:	e58d300c 	str	r3, [sp, #12]
 1d0:	e1a00003 	mov	r0, r3
 1d4:	ebfffffe 	bl	0 <malloc>
			1d4: R_ARM_CALL	malloc
 1d8:	e5840018 	str	r0, [r4, #24]
 1dc:	e3500000 	cmp	r0, #0
 1e0:	e59d300c 	ldr	r3, [sp, #12]
 1e4:	1a000034 	bne	2bc <RES_resampler_create+0x2bc>
 1e8:	e59f312c 	ldr	r3, [pc, #300]	; 31c <RES_resampler_create+0x31c>
 1ec:	e59f2150 	ldr	r2, [pc, #336]	; 344 <RES_resampler_create+0x344>
 1f0:	e7980003 	ldr	r0, [r8, r3]
 1f4:	e3a0303e 	mov	r3, #62	; 0x3e
 1f8:	e59f1148 	ldr	r1, [pc, #328]	; 348 <RES_resampler_create+0x348>
 1fc:	e08f2002 	add	r2, pc, r2
 200:	e08f1001 	add	r1, pc, r1
 204:	e5900000 	ldr	r0, [r0]
 208:	ebfffffe 	bl	0 <fprintf>
			208: R_ARM_CALL	fprintf
 20c:	e59b3040 	ldr	r3, [fp, #64]	; 0x40
 210:	e594000c 	ldr	r0, [r4, #12]
 214:	e12fff33 	blx	r3
 218:	e59f512c 	ldr	r5, [pc, #300]	; 34c <RES_resampler_create+0x34c>
 21c:	e1a00009 	mov	r0, r9
 220:	ebfffffe 	bl	0 <MODULE_hi_audio_module_handle_deinit>
			220: R_ARM_CALL	MODULE_hi_audio_module_handle_deinit
 224:	e1a00004 	mov	r0, r4
 228:	ebfffffe 	bl	0 <free>
			228: R_ARM_CALL	free
 22c:	eaffff85 	b	48 <RES_resampler_create+0x48>
 230:	e35b0001 	cmp	fp, #1
 234:	1a000029 	bne	2e0 <RES_resampler_create+0x2e0>
 238:	e28d2020 	add	r2, sp, #32
 23c:	e284ba06 	add	fp, r4, #24576	; 0x6000
 240:	e1a01007 	mov	r1, r7
 244:	e2840010 	add	r0, r4, #16
 248:	e59b3038 	ldr	r3, [fp, #56]	; 0x38
 24c:	e5226010 	str	r6, [r2, #-16]!
 250:	e12fff33 	blx	r3
 254:	e5943010 	ldr	r3, [r4, #16]
 258:	e3530000 	cmp	r3, #0
 25c:	0affffed 	beq	218 <RES_resampler_create+0x218>
 260:	e3a03064 	mov	r3, #100	; 0x64
 264:	e713f316 	sdiv	r3, r6, r3
 268:	e2833001 	add	r3, r3, #1
 26c:	e1a03083 	lsl	r3, r3, #1
 270:	e58d300c 	str	r3, [sp, #12]
 274:	e1a00003 	mov	r0, r3
 278:	ebfffffe 	bl	0 <malloc>
			278: R_ARM_CALL	malloc
 27c:	e5840018 	str	r0, [r4, #24]
 280:	e3500000 	cmp	r0, #0
 284:	e59d300c 	ldr	r3, [sp, #12]
 288:	1a00000b 	bne	2bc <RES_resampler_create+0x2bc>
 28c:	e59f3088 	ldr	r3, [pc, #136]	; 31c <RES_resampler_create+0x31c>
 290:	e59f20b8 	ldr	r2, [pc, #184]	; 350 <RES_resampler_create+0x350>
 294:	e7980003 	ldr	r0, [r8, r3]
 298:	e3a0304e 	mov	r3, #78	; 0x4e
 29c:	e59f10b0 	ldr	r1, [pc, #176]	; 354 <RES_resampler_create+0x354>
 2a0:	e08f2002 	add	r2, pc, r2
 2a4:	e08f1001 	add	r1, pc, r1
 2a8:	e5900000 	ldr	r0, [r0]
 2ac:	ebfffffe 	bl	0 <fprintf>
			2ac: R_ARM_CALL	fprintf
 2b0:	e59b3040 	ldr	r3, [fp, #64]	; 0x40
 2b4:	e5940010 	ldr	r0, [r4, #16]
 2b8:	eaffffd5 	b	214 <RES_resampler_create+0x214>
 2bc:	e1a02005 	mov	r2, r5
 2c0:	e1a01003 	mov	r1, r3
 2c4:	ebfffffe 	bl	0 <memset_s>
			2c4: R_ARM_CALL	memset_s
 2c8:	e5847004 	str	r7, [r4, #4]
 2cc:	e3a03000 	mov	r3, #0
 2d0:	e5843014 	str	r3, [r4, #20]
 2d4:	e5846008 	str	r6, [r4, #8]
 2d8:	e58a4000 	str	r4, [sl]
 2dc:	eaffff59 	b	48 <RES_resampler_create+0x48>
 2e0:	e59f3034 	ldr	r3, [pc, #52]	; 31c <RES_resampler_create+0x31c>
 2e4:	e59f206c 	ldr	r2, [pc, #108]	; 358 <RES_resampler_create+0x358>
 2e8:	e59f106c 	ldr	r1, [pc, #108]	; 35c <RES_resampler_create+0x35c>
 2ec:	e7980003 	ldr	r0, [r8, r3]
 2f0:	e3a03055 	mov	r3, #85	; 0x55
 2f4:	e58db000 	str	fp, [sp]
 2f8:	e08f2002 	add	r2, pc, r2
 2fc:	e08f1001 	add	r1, pc, r1
 300:	eaffffa9 	b	1ac <RES_resampler_create+0x1ac>
 304:	e28dd024 	add	sp, sp, #36	; 0x24
 308:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 30c:	000002ec 	.word	0x000002ec
			30c: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 310:	00000000 	.word	0x00000000
			310: R_ARM_GOT32	__stack_chk_guard
 314:	000002e0 	.word	0x000002e0
			314: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 318:	000002bc 	.word	0x000002bc
			318: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 31c:	00000000 	.word	0x00000000
			31c: R_ARM_GOT32	stderr
 320:	00000260 	.word	0x00000260
			320: R_ARM_REL32	.rodata.__FUNCTION__.8176
 324:	00000260 	.word	0x00000260
			324: R_ARM_REL32	.LC1
 328:	a3340003 	.word	0xa3340003
 32c:	00000224 	.word	0x00000224
			32c: R_ARM_REL32	.LC2
 330:	00000224 	.word	0x00000224
			330: R_ARM_REL32	.LC3
 334:	00000200 	.word	0x00000200
			334: R_ARM_REL32	.rodata.__FUNCTION__.8176
 338:	00000200 	.word	0x00000200
			338: R_ARM_REL32	.LC4
 33c:	0000019c 	.word	0x0000019c
			33c: R_ARM_REL32	.rodata.__FUNCTION__.8166
 340:	0000019c 	.word	0x0000019c
			340: R_ARM_REL32	.LC5
 344:	00000140 	.word	0x00000140
			344: R_ARM_REL32	.rodata.__FUNCTION__.8166
 348:	00000140 	.word	0x00000140
			348: R_ARM_REL32	.LC6
 34c:	a3340002 	.word	0xa3340002
 350:	000000a8 	.word	0x000000a8
			350: R_ARM_REL32	.rodata.__FUNCTION__.8166
 354:	000000a8 	.word	0x000000a8
			354: R_ARM_REL32	.LC7
 358:	00000058 	.word	0x00000058
			358: R_ARM_REL32	.rodata.__FUNCTION__.8166
 35c:	00000058 	.word	0x00000058
			35c: R_ARM_REL32	.LC8

Disassembly of section .text.RES_resampler_destory:

00000000 <RES_resampler_destory>:
   0:	e92d4010 	push	{r4, lr}
   4:	e2504000 	subs	r4, r0, #0
   8:	0a00001e 	beq	88 <RES_resampler_destory+0x88>
   c:	e594000c 	ldr	r0, [r4, #12]
  10:	e3500000 	cmp	r0, #0
  14:	0a000008 	beq	3c <RES_resampler_destory+0x3c>
  18:	e2843a06 	add	r3, r4, #24576	; 0x6000
  1c:	e5933040 	ldr	r3, [r3, #64]	; 0x40
  20:	e12fff33 	blx	r3
  24:	e5940018 	ldr	r0, [r4, #24]
  28:	e3500000 	cmp	r0, #0
  2c:	0a000002 	beq	3c <RES_resampler_destory+0x3c>
  30:	ebfffffe 	bl	0 <free>
			30: R_ARM_CALL	free
  34:	e3a03000 	mov	r3, #0
  38:	e5843018 	str	r3, [r4, #24]
  3c:	e5940010 	ldr	r0, [r4, #16]
  40:	e3500000 	cmp	r0, #0
  44:	0a000008 	beq	6c <RES_resampler_destory+0x6c>
  48:	e2843a06 	add	r3, r4, #24576	; 0x6000
  4c:	e5933040 	ldr	r3, [r3, #64]	; 0x40
  50:	e12fff33 	blx	r3
  54:	e5940018 	ldr	r0, [r4, #24]
  58:	e3500000 	cmp	r0, #0
  5c:	0a000002 	beq	6c <RES_resampler_destory+0x6c>
  60:	ebfffffe 	bl	0 <free>
			60: R_ARM_CALL	free
  64:	e3a03000 	mov	r3, #0
  68:	e5843018 	str	r3, [r4, #24]
  6c:	e2840a06 	add	r0, r4, #24576	; 0x6000
  70:	e280001c 	add	r0, r0, #28
  74:	ebfffffe 	bl	0 <MODULE_hi_audio_module_handle_deinit>
			74: R_ARM_CALL	MODULE_hi_audio_module_handle_deinit
  78:	e1a00004 	mov	r0, r4
  7c:	ebfffffe 	bl	0 <free>
			7c: R_ARM_CALL	free
  80:	e3a00000 	mov	r0, #0
  84:	e8bd8010 	pop	{r4, pc}
  88:	e3e00000 	mvn	r0, #0
  8c:	e8bd8010 	pop	{r4, pc}

Disassembly of section .text.RES_resampler_get_input_num:

00000000 <RES_resampler_get_input_num>:
   0:	e2503000 	subs	r3, r0, #0
   4:	0a00000b 	beq	38 <RES_resampler_get_input_num+0x38>
   8:	e3520001 	cmp	r2, #1
   c:	1a00000b 	bne	40 <RES_resampler_get_input_num+0x40>
  10:	e5930014 	ldr	r0, [r3, #20]
  14:	e0410000 	sub	r0, r1, r0
  18:	e5931004 	ldr	r1, [r3, #4]
  1c:	e0010091 	mul	r1, r1, r0
  20:	e5930008 	ldr	r0, [r3, #8]
  24:	e5933000 	ldr	r3, [r3]
  28:	e710f011 	sdiv	r0, r1, r0
  2c:	e3530000 	cmp	r3, #0
  30:	02800001 	addeq	r0, r0, #1
  34:	e12fff1e 	bx	lr
  38:	e3e00000 	mvn	r0, #0
  3c:	e12fff1e 	bx	lr
  40:	e3a00000 	mov	r0, #0
  44:	e12fff1e 	bx	lr

Disassembly of section .text.RES_resampler_process_frame:

00000000 <RES_resampler_process_frame>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e1a06002 	mov	r6, r2
   8:	e59f26ac 	ldr	r2, [pc, #1708]	; 6bc <RES_resampler_process_frame+0x6bc>
   c:	e24dd04c 	sub	sp, sp, #76	; 0x4c
  10:	e59f36a8 	ldr	r3, [pc, #1704]	; 6c0 <RES_resampler_process_frame+0x6c0>
  14:	e2504000 	subs	r4, r0, #0
  18:	e08f2002 	add	r2, pc, r2
  1c:	e59f76a0 	ldr	r7, [pc, #1696]	; 6c4 <RES_resampler_process_frame+0x6c4>
  20:	e7923003 	ldr	r3, [r2, r3]
  24:	e08f7007 	add	r7, pc, r7
  28:	e5933000 	ldr	r3, [r3]
  2c:	e58d3044 	str	r3, [sp, #68]	; 0x44
  30:	e3a03000 	mov	r3, #0
  34:	0a00019c 	beq	6ac <RES_resampler_process_frame+0x6ac>
  38:	e5913000 	ldr	r3, [r1]
  3c:	e1a05001 	mov	r5, r1
  40:	e3530000 	cmp	r3, #0
  44:	0a000198 	beq	6ac <RES_resampler_process_frame+0x6ac>
  48:	e5913004 	ldr	r3, [r1, #4]
  4c:	e3530000 	cmp	r3, #0
  50:	0a000195 	beq	6ac <RES_resampler_process_frame+0x6ac>
  54:	e3a03014 	mov	r3, #20
  58:	e28da01c 	add	sl, sp, #28
  5c:	e1a01003 	mov	r1, r3
  60:	e3a02000 	mov	r2, #0
  64:	e1a0000a 	mov	r0, sl
  68:	e28d9030 	add	r9, sp, #48	; 0x30
  6c:	ebfffffe 	bl	0 <memset_s>
			6c: R_ARM_CALL	memset_s
  70:	e3a03014 	mov	r3, #20
  74:	e3a02000 	mov	r2, #0
  78:	e1a01003 	mov	r1, r3
  7c:	e1a00009 	mov	r0, r9
  80:	ebfffffe 	bl	0 <memset_s>
			80: R_ARM_CALL	memset_s
  84:	e3560000 	cmp	r6, #0
  88:	1a00006d 	bne	244 <RES_resampler_process_frame+0x244>
  8c:	e5942000 	ldr	r2, [r4]
  90:	e3520000 	cmp	r2, #0
  94:	0a000059 	beq	200 <RES_resampler_process_frame+0x200>
  98:	e5951008 	ldr	r1, [r5, #8]
  9c:	e713f211 	sdiv	r3, r1, r2
  a0:	e0631392 	mls	r3, r2, r3, r1
  a4:	e3530000 	cmp	r3, #0
  a8:	e5943014 	ldr	r3, [r4, #20]
  ac:	1a000001 	bne	b8 <RES_resampler_process_frame+0xb8>
  b0:	e3530000 	cmp	r3, #0
  b4:	da000051 	ble	200 <RES_resampler_process_frame+0x200>
  b8:	e3530000 	cmp	r3, #0
  bc:	e284801c 	add	r8, r4, #28
  c0:	da00001b 	ble	134 <RES_resampler_process_frame+0x134>
  c4:	e1a03083 	lsl	r3, r3, #1
  c8:	e3a01a06 	mov	r1, #24576	; 0x6000
  cc:	e5942018 	ldr	r2, [r4, #24]
  d0:	e1a00008 	mov	r0, r8
  d4:	ebfffffe 	bl	0 <memcpy_s>
			d4: R_ARM_CALL	memcpy_s
  d8:	e3500000 	cmp	r0, #0
  dc:	0a000014 	beq	134 <RES_resampler_process_frame+0x134>
  e0:	e59f35e0 	ldr	r3, [pc, #1504]	; 6c8 <RES_resampler_process_frame+0x6c8>
  e4:	e59f25e0 	ldr	r2, [pc, #1504]	; 6cc <RES_resampler_process_frame+0x6cc>
  e8:	e59f15e0 	ldr	r1, [pc, #1504]	; 6d0 <RES_resampler_process_frame+0x6d0>
  ec:	e797c003 	ldr	ip, [r7, r3]
  f0:	e3a030d3 	mov	r3, #211	; 0xd3
  f4:	e08f2002 	add	r2, pc, r2
  f8:	e08f1001 	add	r1, pc, r1
  fc:	e58d0000 	str	r0, [sp]
 100:	e59c0000 	ldr	r0, [ip]
 104:	ebfffffe 	bl	0 <fprintf>
			104: R_ARM_CALL	fprintf
 108:	e59f65c4 	ldr	r6, [pc, #1476]	; 6d4 <RES_resampler_process_frame+0x6d4>
 10c:	e59f25c4 	ldr	r2, [pc, #1476]	; 6d8 <RES_resampler_process_frame+0x6d8>
 110:	e1a00006 	mov	r0, r6
 114:	e59f35a4 	ldr	r3, [pc, #1444]	; 6c0 <RES_resampler_process_frame+0x6c0>
 118:	e08f2002 	add	r2, pc, r2
 11c:	e7923003 	ldr	r3, [r2, r3]
 120:	e5932000 	ldr	r2, [r3]
 124:	e59d3044 	ldr	r3, [sp, #68]	; 0x44
 128:	e0332002 	eors	r2, r3, r2
 12c:	0a000160 	beq	6b4 <RES_resampler_process_frame+0x6b4>
 130:	ebfffffe 	bl	0 <__stack_chk_fail>
			130: R_ARM_CALL	__stack_chk_fail
 134:	e5953008 	ldr	r3, [r5, #8]
 138:	e5940014 	ldr	r0, [r4, #20]
 13c:	e5942000 	ldr	r2, [r4]
 140:	e0831000 	add	r1, r3, r0
 144:	e0880080 	add	r0, r8, r0, lsl #1
 148:	e716f211 	sdiv	r6, r1, r2
 14c:	e0661692 	mls	r6, r2, r6, r1
 150:	e5952000 	ldr	r2, [r5]
 154:	e0433006 	sub	r3, r3, r6
 158:	e1a03083 	lsl	r3, r3, #1
 15c:	e1a01003 	mov	r1, r3
 160:	ebfffffe 	bl	0 <memcpy_s>
			160: R_ARM_CALL	memcpy_s
 164:	e3500000 	cmp	r0, #0
 168:	0a000008 	beq	190 <RES_resampler_process_frame+0x190>
 16c:	e59f3554 	ldr	r3, [pc, #1364]	; 6c8 <RES_resampler_process_frame+0x6c8>
 170:	e59f2564 	ldr	r2, [pc, #1380]	; 6dc <RES_resampler_process_frame+0x6dc>
 174:	e59f1564 	ldr	r1, [pc, #1380]	; 6e0 <RES_resampler_process_frame+0x6e0>
 178:	e797c003 	ldr	ip, [r7, r3]
 17c:	e3a030dd 	mov	r3, #221	; 0xdd
 180:	e58d0000 	str	r0, [sp]
 184:	e08f2002 	add	r2, pc, r2
 188:	e08f1001 	add	r1, pc, r1
 18c:	eaffffdb 	b	100 <RES_resampler_process_frame+0x100>
 190:	e3560000 	cmp	r6, #0
 194:	0a000012 	beq	1e4 <RES_resampler_process_frame+0x1e4>
 198:	e5952008 	ldr	r2, [r5, #8]
 19c:	e5951000 	ldr	r1, [r5]
 1a0:	e0422006 	sub	r2, r2, r6
 1a4:	e1a03086 	lsl	r3, r6, #1
 1a8:	e0812082 	add	r2, r1, r2, lsl #1
 1ac:	e1a01003 	mov	r1, r3
 1b0:	e5940018 	ldr	r0, [r4, #24]
 1b4:	ebfffffe 	bl	0 <memcpy_s>
			1b4: R_ARM_CALL	memcpy_s
 1b8:	e3500000 	cmp	r0, #0
 1bc:	0a000008 	beq	1e4 <RES_resampler_process_frame+0x1e4>
 1c0:	e59f3500 	ldr	r3, [pc, #1280]	; 6c8 <RES_resampler_process_frame+0x6c8>
 1c4:	e59f2518 	ldr	r2, [pc, #1304]	; 6e4 <RES_resampler_process_frame+0x6e4>
 1c8:	e59f1518 	ldr	r1, [pc, #1304]	; 6e8 <RES_resampler_process_frame+0x6e8>
 1cc:	e797c003 	ldr	ip, [r7, r3]
 1d0:	e3a030e5 	mov	r3, #229	; 0xe5
 1d4:	e58d0000 	str	r0, [sp]
 1d8:	e08f2002 	add	r2, pc, r2
 1dc:	e08f1001 	add	r1, pc, r1
 1e0:	eaffffc6 	b	100 <RES_resampler_process_frame+0x100>
 1e4:	e5953008 	ldr	r3, [r5, #8]
 1e8:	e5942014 	ldr	r2, [r4, #20]
 1ec:	e5858000 	str	r8, [r5]
 1f0:	e0833002 	add	r3, r3, r2
 1f4:	e0433006 	sub	r3, r3, r6
 1f8:	e5853008 	str	r3, [r5, #8]
 1fc:	e5846014 	str	r6, [r4, #20]
 200:	e5953000 	ldr	r3, [r5]
 204:	e1a02009 	mov	r2, r9
 208:	e594000c 	ldr	r0, [r4, #12]
 20c:	e1a0100a 	mov	r1, sl
 210:	e58d301c 	str	r3, [sp, #28]
 214:	e5953008 	ldr	r3, [r5, #8]
 218:	e1a03083 	lsl	r3, r3, #1
 21c:	e58d302c 	str	r3, [sp, #44]	; 0x2c
 220:	e5953004 	ldr	r3, [r5, #4]
 224:	e595500c 	ldr	r5, [r5, #12]
 228:	e58d3030 	str	r3, [sp, #48]	; 0x30
 22c:	e2843a06 	add	r3, r4, #24576	; 0x6000
 230:	e593303c 	ldr	r3, [r3, #60]	; 0x3c
 234:	e12fff33 	blx	r3
 238:	e5850000 	str	r0, [r5]
 23c:	e3a06000 	mov	r6, #0
 240:	eaffffb1 	b	10c <RES_resampler_process_frame+0x10c>
 244:	e3560001 	cmp	r6, #1
 248:	1a000085 	bne	464 <RES_resampler_process_frame+0x464>
 24c:	e595300c 	ldr	r3, [r5, #12]
 250:	e594b010 	ldr	fp, [r4, #16]
 254:	e5933000 	ldr	r3, [r3]
 258:	e5952004 	ldr	r2, [r5, #4]
 25c:	e58d3018 	str	r3, [sp, #24]
 260:	e5943000 	ldr	r3, [r4]
 264:	e3530000 	cmp	r3, #0
 268:	1a000003 	bne	27c <RES_resampler_process_frame+0x27c>
 26c:	e5953008 	ldr	r3, [r5, #8]
 270:	e3530000 	cmp	r3, #0
 274:	12433001 	subne	r3, r3, #1
 278:	15853008 	strne	r3, [r5, #8]
 27c:	e5953000 	ldr	r3, [r5]
 280:	e1a01009 	mov	r1, r9
 284:	e58d2030 	str	r2, [sp, #48]	; 0x30
 288:	e1a00004 	mov	r0, r4
 28c:	e58d301c 	str	r3, [sp, #28]
 290:	e5953008 	ldr	r3, [r5, #8]
 294:	e1a03083 	lsl	r3, r3, #1
 298:	e58d302c 	str	r3, [sp, #44]	; 0x2c
 29c:	e28d3018 	add	r3, sp, #24
 2a0:	ebfffffe 	bl	0 <RES_resampler_process_frame>
			2a0: R_ARM_CALL	update_cache.isra.0
 2a4:	e2508000 	subs	r8, r0, #0
 2a8:	0affff96 	beq	108 <RES_resampler_process_frame+0x108>
 2ac:	e2843a06 	add	r3, r4, #24576	; 0x6000
 2b0:	e1a02009 	mov	r2, r9
 2b4:	e58d3010 	str	r3, [sp, #16]
 2b8:	e1a0100a 	mov	r1, sl
 2bc:	e593603c 	ldr	r6, [r3, #60]	; 0x3c
 2c0:	e1a0000b 	mov	r0, fp
 2c4:	e12fff36 	blx	r6
 2c8:	e59d2018 	ldr	r2, [sp, #24]
 2cc:	e1a06000 	mov	r6, r0
 2d0:	e59d3010 	ldr	r3, [sp, #16]
 2d4:	e1500002 	cmp	r0, r2
 2d8:	da00000c 	ble	310 <RES_resampler_process_frame+0x310>
 2dc:	e59f33e4 	ldr	r3, [pc, #996]	; 6c8 <RES_resampler_process_frame+0x6c8>
 2e0:	e59f1404 	ldr	r1, [pc, #1028]	; 6ec <RES_resampler_process_frame+0x6ec>
 2e4:	e7970003 	ldr	r0, [r7, r3]
 2e8:	e3003159 	movw	r3, #345	; 0x159
 2ec:	e58d2008 	str	r2, [sp, #8]
 2f0:	e59f23f8 	ldr	r2, [pc, #1016]	; 6f0 <RES_resampler_process_frame+0x6f0>
 2f4:	e08f1001 	add	r1, pc, r1
 2f8:	e08f2002 	add	r2, pc, r2
 2fc:	e58d6004 	str	r6, [sp, #4]
 300:	e58d3000 	str	r3, [sp]
 304:	e5900000 	ldr	r0, [r0]
 308:	ebfffffe 	bl	0 <fprintf>
			308: R_ARM_CALL	fprintf
 30c:	eaffff7d 	b	108 <RES_resampler_process_frame+0x108>
 310:	e5941000 	ldr	r1, [r4]
 314:	e3510000 	cmp	r1, #0
 318:	0a00000c 	beq	350 <RES_resampler_process_frame+0x350>
 31c:	e1500002 	cmp	r0, r2
 320:	aaffffc5 	bge	23c <RES_resampler_process_frame+0x23c>
 324:	e59f339c 	ldr	r3, [pc, #924]	; 6c8 <RES_resampler_process_frame+0x6c8>
 328:	e59f13c4 	ldr	r1, [pc, #964]	; 6f4 <RES_resampler_process_frame+0x6f4>
 32c:	e7970003 	ldr	r0, [r7, r3]
 330:	e3003161 	movw	r3, #353	; 0x161
 334:	e58d2008 	str	r2, [sp, #8]
 338:	e59f23b8 	ldr	r2, [pc, #952]	; 6f8 <RES_resampler_process_frame+0x6f8>
 33c:	e58d6004 	str	r6, [sp, #4]
 340:	e58d3000 	str	r3, [sp]
 344:	e08f2002 	add	r2, pc, r2
 348:	e08f1001 	add	r1, pc, r1
 34c:	eaffffec 	b	304 <RES_resampler_process_frame+0x304>
 350:	e5951008 	ldr	r1, [r5, #8]
 354:	e1a0000b 	mov	r0, fp
 358:	e5952000 	ldr	r2, [r5]
 35c:	e593303c 	ldr	r3, [r3, #60]	; 0x3c
 360:	e0822081 	add	r2, r2, r1, lsl #1
 364:	e1a0100a 	mov	r1, sl
 368:	e58d201c 	str	r2, [sp, #28]
 36c:	e3a02002 	mov	r2, #2
 370:	e58d202c 	str	r2, [sp, #44]	; 0x2c
 374:	e5942018 	ldr	r2, [r4, #24]
 378:	e58d2030 	str	r2, [sp, #48]	; 0x30
 37c:	e1a02009 	mov	r2, r9
 380:	e12fff33 	blx	r3
 384:	e59d3018 	ldr	r3, [sp, #24]
 388:	e5840014 	str	r0, [r4, #20]
 38c:	e1560003 	cmp	r6, r3
 390:	aaffffa9 	bge	23c <RES_resampler_process_frame+0x23c>
 394:	e0435006 	sub	r5, r3, r6
 398:	e0400005 	sub	r0, r0, r5
 39c:	e5840014 	str	r0, [r4, #20]
 3a0:	e3500000 	cmp	r0, #0
 3a4:	aa00000a 	bge	3d4 <RES_resampler_process_frame+0x3d4>
 3a8:	e59f2318 	ldr	r2, [pc, #792]	; 6c8 <RES_resampler_process_frame+0x6c8>
 3ac:	e59f1348 	ldr	r1, [pc, #840]	; 6fc <RES_resampler_process_frame+0x6fc>
 3b0:	e7970002 	ldr	r0, [r7, r2]
 3b4:	e59f2344 	ldr	r2, [pc, #836]	; 700 <RES_resampler_process_frame+0x700>
 3b8:	e58d3008 	str	r3, [sp, #8]
 3bc:	e300312a 	movw	r3, #298	; 0x12a
 3c0:	e58d6004 	str	r6, [sp, #4]
 3c4:	e58d3000 	str	r3, [sp]
 3c8:	e08f2002 	add	r2, pc, r2
 3cc:	e08f1001 	add	r1, pc, r1
 3d0:	eaffffcb 	b	304 <RES_resampler_process_frame+0x304>
 3d4:	e1a05085 	lsl	r5, r5, #1
 3d8:	e5942018 	ldr	r2, [r4, #24]
 3dc:	e1a03005 	mov	r3, r5
 3e0:	e0880086 	add	r0, r8, r6, lsl #1
 3e4:	e1a01005 	mov	r1, r5
 3e8:	ebfffffe 	bl	0 <memcpy_s>
			3e8: R_ARM_CALL	memcpy_s
 3ec:	e3500000 	cmp	r0, #0
 3f0:	0a000008 	beq	418 <RES_resampler_process_frame+0x418>
 3f4:	e59f32cc 	ldr	r3, [pc, #716]	; 6c8 <RES_resampler_process_frame+0x6c8>
 3f8:	e59f2304 	ldr	r2, [pc, #772]	; 704 <RES_resampler_process_frame+0x704>
 3fc:	e59f1304 	ldr	r1, [pc, #772]	; 708 <RES_resampler_process_frame+0x708>
 400:	e797c003 	ldr	ip, [r7, r3]
 404:	e3003131 	movw	r3, #305	; 0x131
 408:	e58d0000 	str	r0, [sp]
 40c:	e08f2002 	add	r2, pc, r2
 410:	e08f1001 	add	r1, pc, r1
 414:	eaffff39 	b	100 <RES_resampler_process_frame+0x100>
 418:	e5943014 	ldr	r3, [r4, #20]
 41c:	e3530000 	cmp	r3, #0
 420:	0affff85 	beq	23c <RES_resampler_process_frame+0x23c>
 424:	e5940018 	ldr	r0, [r4, #24]
 428:	e1a03083 	lsl	r3, r3, #1
 42c:	e0802005 	add	r2, r0, r5
 430:	e1a01003 	mov	r1, r3
 434:	ebfffffe 	bl	0 <memmove_s>
			434: R_ARM_CALL	memmove_s
 438:	e3500000 	cmp	r0, #0
 43c:	0affff7e 	beq	23c <RES_resampler_process_frame+0x23c>
 440:	e59f3280 	ldr	r3, [pc, #640]	; 6c8 <RES_resampler_process_frame+0x6c8>
 444:	e59f22c0 	ldr	r2, [pc, #704]	; 70c <RES_resampler_process_frame+0x70c>
 448:	e59f12c0 	ldr	r1, [pc, #704]	; 710 <RES_resampler_process_frame+0x710>
 44c:	e797c003 	ldr	ip, [r7, r3]
 450:	e3003139 	movw	r3, #313	; 0x139
 454:	e58d0000 	str	r0, [sp]
 458:	e08f2002 	add	r2, pc, r2
 45c:	e08f1001 	add	r1, pc, r1
 460:	eaffff26 	b	100 <RES_resampler_process_frame+0x100>
 464:	e3560002 	cmp	r6, #2
 468:	159f62a4 	ldrne	r6, [pc, #676]	; 714 <RES_resampler_process_frame+0x714>
 46c:	1affff26 	bne	10c <RES_resampler_process_frame+0x10c>
 470:	e595300c 	ldr	r3, [r5, #12]
 474:	e594b010 	ldr	fp, [r4, #16]
 478:	e5933000 	ldr	r3, [r3]
 47c:	e5952004 	ldr	r2, [r5, #4]
 480:	e58d3018 	str	r3, [sp, #24]
 484:	e5943000 	ldr	r3, [r4]
 488:	e3530000 	cmp	r3, #0
 48c:	1a000003 	bne	4a0 <RES_resampler_process_frame+0x4a0>
 490:	e5953008 	ldr	r3, [r5, #8]
 494:	e3530000 	cmp	r3, #0
 498:	12433001 	subne	r3, r3, #1
 49c:	15853008 	strne	r3, [r5, #8]
 4a0:	e5953000 	ldr	r3, [r5]
 4a4:	e1a01009 	mov	r1, r9
 4a8:	e58d2030 	str	r2, [sp, #48]	; 0x30
 4ac:	e1a00004 	mov	r0, r4
 4b0:	e58d301c 	str	r3, [sp, #28]
 4b4:	e5953008 	ldr	r3, [r5, #8]
 4b8:	e1a03083 	lsl	r3, r3, #1
 4bc:	e58d302c 	str	r3, [sp, #44]	; 0x2c
 4c0:	e28d3018 	add	r3, sp, #24
 4c4:	ebfffffe 	bl	0 <RES_resampler_process_frame>
			4c4: R_ARM_CALL	update_cache.isra.0
 4c8:	e2503000 	subs	r3, r0, #0
 4cc:	e58d3010 	str	r3, [sp, #16]
 4d0:	0affff0c 	beq	108 <RES_resampler_process_frame+0x108>
 4d4:	e2843a06 	add	r3, r4, #24576	; 0x6000
 4d8:	e1a02009 	mov	r2, r9
 4dc:	e58d3014 	str	r3, [sp, #20]
 4e0:	e1a0100a 	mov	r1, sl
 4e4:	e593603c 	ldr	r6, [r3, #60]	; 0x3c
 4e8:	e1a0000b 	mov	r0, fp
 4ec:	e12fff36 	blx	r6
 4f0:	e59d2018 	ldr	r2, [sp, #24]
 4f4:	e1a08000 	mov	r8, r0
 4f8:	e59d3014 	ldr	r3, [sp, #20]
 4fc:	e1500002 	cmp	r0, r2
 500:	da00000a 	ble	530 <RES_resampler_process_frame+0x530>
 504:	e59f31bc 	ldr	r3, [pc, #444]	; 6c8 <RES_resampler_process_frame+0x6c8>
 508:	e59f1208 	ldr	r1, [pc, #520]	; 718 <RES_resampler_process_frame+0x718>
 50c:	e7970003 	ldr	r0, [r7, r3]
 510:	e30031b6 	movw	r3, #438	; 0x1b6
 514:	e58d2008 	str	r2, [sp, #8]
 518:	e59f21fc 	ldr	r2, [pc, #508]	; 71c <RES_resampler_process_frame+0x71c>
 51c:	e58d8004 	str	r8, [sp, #4]
 520:	e58d3000 	str	r3, [sp]
 524:	e08f2002 	add	r2, pc, r2
 528:	e08f1001 	add	r1, pc, r1
 52c:	eaffff74 	b	304 <RES_resampler_process_frame+0x304>
 530:	e5946000 	ldr	r6, [r4]
 534:	e3560000 	cmp	r6, #0
 538:	0a000007 	beq	55c <RES_resampler_process_frame+0x55c>
 53c:	e1500002 	cmp	r0, r2
 540:	b0428000 	sublt	r8, r2, r0
 544:	b595300c 	ldrlt	r3, [r5, #12]
 548:	b58d8018 	strlt	r8, [sp, #24]
 54c:	b5932000 	ldrlt	r2, [r3]
 550:	b0428008 	sublt	r8, r2, r8
 554:	b5838000 	strlt	r8, [r3]
 558:	eaffff37 	b	23c <RES_resampler_process_frame+0x23c>
 55c:	e5951008 	ldr	r1, [r5, #8]
 560:	e1a0000b 	mov	r0, fp
 564:	e5952000 	ldr	r2, [r5]
 568:	e593303c 	ldr	r3, [r3, #60]	; 0x3c
 56c:	e0822081 	add	r2, r2, r1, lsl #1
 570:	e1a0100a 	mov	r1, sl
 574:	e58d201c 	str	r2, [sp, #28]
 578:	e3a02002 	mov	r2, #2
 57c:	e58d202c 	str	r2, [sp, #44]	; 0x2c
 580:	e5942018 	ldr	r2, [r4, #24]
 584:	e58d2030 	str	r2, [sp, #48]	; 0x30
 588:	e1a02009 	mov	r2, r9
 58c:	e12fff33 	blx	r3
 590:	e59d2018 	ldr	r2, [sp, #24]
 594:	e1a03000 	mov	r3, r0
 598:	e5840014 	str	r0, [r4, #20]
 59c:	e1580002 	cmp	r8, r2
 5a0:	aaffff25 	bge	23c <RES_resampler_process_frame+0x23c>
 5a4:	e0422008 	sub	r2, r2, r8
 5a8:	e59d1010 	ldr	r1, [sp, #16]
 5ac:	e1530002 	cmp	r3, r2
 5b0:	e0810088 	add	r0, r1, r8, lsl #1
 5b4:	aa000017 	bge	618 <RES_resampler_process_frame+0x618>
 5b8:	e3530000 	cmp	r3, #0
 5bc:	0a00000e 	beq	5fc <RES_resampler_process_frame+0x5fc>
 5c0:	e1a03083 	lsl	r3, r3, #1
 5c4:	e5942018 	ldr	r2, [r4, #24]
 5c8:	e1a01003 	mov	r1, r3
 5cc:	ebfffffe 	bl	0 <memcpy_s>
			5cc: R_ARM_CALL	memcpy_s
 5d0:	e3500000 	cmp	r0, #0
 5d4:	0a000008 	beq	5fc <RES_resampler_process_frame+0x5fc>
 5d8:	e59f30e8 	ldr	r3, [pc, #232]	; 6c8 <RES_resampler_process_frame+0x6c8>
 5dc:	e59f213c 	ldr	r2, [pc, #316]	; 720 <RES_resampler_process_frame+0x720>
 5e0:	e59f113c 	ldr	r1, [pc, #316]	; 724 <RES_resampler_process_frame+0x724>
 5e4:	e797c003 	ldr	ip, [r7, r3]
 5e8:	e3a03d06 	mov	r3, #384	; 0x180
 5ec:	e58d0000 	str	r0, [sp]
 5f0:	e08f2002 	add	r2, pc, r2
 5f4:	e08f1001 	add	r1, pc, r1
 5f8:	eafffec0 	b	100 <RES_resampler_process_frame+0x100>
 5fc:	e5943014 	ldr	r3, [r4, #20]
 600:	e595200c 	ldr	r2, [r5, #12]
 604:	e0838008 	add	r8, r3, r8
 608:	e3a03000 	mov	r3, #0
 60c:	e5828000 	str	r8, [r2]
 610:	e5843014 	str	r3, [r4, #20]
 614:	eafffebc 	b	10c <RES_resampler_process_frame+0x10c>
 618:	e0433002 	sub	r3, r3, r2
 61c:	e1a05082 	lsl	r5, r2, #1
 620:	e5843014 	str	r3, [r4, #20]
 624:	e1a01005 	mov	r1, r5
 628:	e5942018 	ldr	r2, [r4, #24]
 62c:	e1a03005 	mov	r3, r5
 630:	ebfffffe 	bl	0 <memcpy_s>
			630: R_ARM_CALL	memcpy_s
 634:	e3500000 	cmp	r0, #0
 638:	0a000008 	beq	660 <RES_resampler_process_frame+0x660>
 63c:	e59f3084 	ldr	r3, [pc, #132]	; 6c8 <RES_resampler_process_frame+0x6c8>
 640:	e59f20e0 	ldr	r2, [pc, #224]	; 728 <RES_resampler_process_frame+0x728>
 644:	e59f10e0 	ldr	r1, [pc, #224]	; 72c <RES_resampler_process_frame+0x72c>
 648:	e797c003 	ldr	ip, [r7, r3]
 64c:	e300318d 	movw	r3, #397	; 0x18d
 650:	e58d0000 	str	r0, [sp]
 654:	e08f2002 	add	r2, pc, r2
 658:	e08f1001 	add	r1, pc, r1
 65c:	eafffea7 	b	100 <RES_resampler_process_frame+0x100>
 660:	e5943014 	ldr	r3, [r4, #20]
 664:	e3530000 	cmp	r3, #0
 668:	0afffef3 	beq	23c <RES_resampler_process_frame+0x23c>
 66c:	e5940018 	ldr	r0, [r4, #24]
 670:	e1a03083 	lsl	r3, r3, #1
 674:	e0802005 	add	r2, r0, r5
 678:	e1a01003 	mov	r1, r3
 67c:	ebfffffe 	bl	0 <memmove_s>
			67c: R_ARM_CALL	memmove_s
 680:	e3500000 	cmp	r0, #0
 684:	0afffeec 	beq	23c <RES_resampler_process_frame+0x23c>
 688:	e59f3038 	ldr	r3, [pc, #56]	; 6c8 <RES_resampler_process_frame+0x6c8>
 68c:	e59f209c 	ldr	r2, [pc, #156]	; 730 <RES_resampler_process_frame+0x730>
 690:	e59f109c 	ldr	r1, [pc, #156]	; 734 <RES_resampler_process_frame+0x734>
 694:	e797c003 	ldr	ip, [r7, r3]
 698:	e3003195 	movw	r3, #405	; 0x195
 69c:	e58d0000 	str	r0, [sp]
 6a0:	e08f2002 	add	r2, pc, r2
 6a4:	e08f1001 	add	r1, pc, r1
 6a8:	eafffe94 	b	100 <RES_resampler_process_frame+0x100>
 6ac:	e3e06000 	mvn	r6, #0
 6b0:	eafffe95 	b	10c <RES_resampler_process_frame+0x10c>
 6b4:	e28dd04c 	add	sp, sp, #76	; 0x4c
 6b8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 6bc:	0000069c 	.word	0x0000069c
			6bc: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 6c0:	00000000 	.word	0x00000000
			6c0: R_ARM_GOT32	__stack_chk_guard
 6c4:	00000698 	.word	0x00000698
			6c4: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 6c8:	00000000 	.word	0x00000000
			6c8: R_ARM_GOT32	stderr
 6cc:	000005d0 	.word	0x000005d0
			6cc: R_ARM_REL32	.rodata.__FUNCTION__.8194
 6d0:	000005d0 	.word	0x000005d0
			6d0: R_ARM_REL32	.LC0
 6d4:	a3340004 	.word	0xa3340004
 6d8:	000005b8 	.word	0x000005b8
			6d8: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 6dc:	00000550 	.word	0x00000550
			6dc: R_ARM_REL32	.rodata.__FUNCTION__.8194
 6e0:	00000550 	.word	0x00000550
			6e0: R_ARM_REL32	.LC0
 6e4:	00000504 	.word	0x00000504
			6e4: R_ARM_REL32	.rodata.__FUNCTION__.8194
 6e8:	00000504 	.word	0x00000504
			6e8: R_ARM_REL32	.LC0
 6ec:	000003f0 	.word	0x000003f0
			6ec: R_ARM_REL32	.LC9
 6f0:	000003f0 	.word	0x000003f0
			6f0: R_ARM_REL32	.rodata.__FUNCTION__.8232
 6f4:	000003a4 	.word	0x000003a4
			6f4: R_ARM_REL32	.LC9
 6f8:	000003ac 	.word	0x000003ac
			6f8: R_ARM_REL32	.rodata.__FUNCTION__.8232
 6fc:	00000328 	.word	0x00000328
			6fc: R_ARM_REL32	.LC10
 700:	00000330 	.word	0x00000330
			700: R_ARM_REL32	.rodata.__FUNCTION__.8220
 704:	000002f0 	.word	0x000002f0
			704: R_ARM_REL32	.rodata.__FUNCTION__.8220
 708:	000002f0 	.word	0x000002f0
			708: R_ARM_REL32	.LC0
 70c:	000002ac 	.word	0x000002ac
			70c: R_ARM_REL32	.rodata.__FUNCTION__.8220
 710:	000002ac 	.word	0x000002ac
			710: R_ARM_REL32	.LC11
 714:	a3340002 	.word	0xa3340002
 718:	000001e8 	.word	0x000001e8
			718: R_ARM_REL32	.LC12
 71c:	000001f0 	.word	0x000001f0
			71c: R_ARM_REL32	.rodata.__FUNCTION__.8254
 720:	00000128 	.word	0x00000128
			720: R_ARM_REL32	.rodata.__FUNCTION__.8242
 724:	00000128 	.word	0x00000128
			724: R_ARM_REL32	.LC0
 728:	000000cc 	.word	0x000000cc
			728: R_ARM_REL32	.rodata.__FUNCTION__.8242
 72c:	000000cc 	.word	0x000000cc
			72c: R_ARM_REL32	.LC0
 730:	00000088 	.word	0x00000088
			730: R_ARM_REL32	.rodata.__FUNCTION__.8242
 734:	00000088 	.word	0x00000088
			734: R_ARM_REL32	.LC11
