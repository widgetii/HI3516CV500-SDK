
dnvqe_work.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l       .rodata.dnvqe_create.str1.1	00000000 .LC0
0000003b l       .rodata.dnvqe_create.str1.1	00000000 .LC1
0000007a l       .rodata.dnvqe_create.str1.1	00000000 .LC2
0000007e l       .rodata.dnvqe_create.str1.1	00000000 .LC3
00000083 l       .rodata.dnvqe_create.str1.1	00000000 .LC4
00000087 l       .rodata.dnvqe_create.str1.1	00000000 .LC5
0000008b l       .rodata.dnvqe_create.str1.1	00000000 .LC6
0000008e l       .rodata.dnvqe_create.str1.1	00000000 .LC7
00000092 l       .rodata.dnvqe_create.str1.1	00000000 .LC8
000000bc l       .rodata.dnvqe_create.str1.1	00000000 .LC9
00000000 l       .rodata.dnvqe_process_frame.str1.1	00000000 .LC10
00000030 l       .rodata.dnvqe_process_frame.str1.1	00000000 .LC11
00000072 l       .rodata.dnvqe_process_frame.str1.1	00000000 .LC12
000000bc l       .rodata.dnvqe_process_frame.str1.1	00000000 .LC13
00000000 l    d  .rodata.__FUNCTION__.8246	00000000 .rodata.__FUNCTION__.8246
00000000 l    d  .rodata.__FUNCTION__.8263	00000000 .rodata.__FUNCTION__.8263
00000000 l    d  .rodata.__FUNCTION__.8303	00000000 .rodata.__FUNCTION__.8303
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.dnvqe_destroy	00000000 .text.dnvqe_destroy
00000000 l    d  .text.dnvqe_create	00000000 .text.dnvqe_create
00000000 l    d  .text.dnvqe_process_frame	00000000 .text.dnvqe_process_frame
00000000 l    d  .rodata.dnvqe_create.str1.1	00000000 .rodata.dnvqe_create.str1.1
00000000 l    d  .rodata.dnvqe_process_frame.str1.1	00000000 .rodata.dnvqe_process_frame.str1.1
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.dnvqe_destroy	0000004c dnvqe_destroy
00000000         *UND*	00000000 MODULE_hi_audio_module_handle_deinit
00000000         *UND*	00000000 free
00000000 g     F .text.dnvqe_create	00000448 dnvqe_create
00000000         *UND*	00000000 fprintf
00000000         *UND*	00000000 malloc
00000000         *UND*	00000000 memset_s
00000000         *UND*	00000000 MODULE_hi_audio_module_handle_init
00000000         *UND*	00000000 memcpy_s
00000000         *UND*	00000000 memcpy
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 stderr
00000000 g     F .text.dnvqe_process_frame	00000274 dnvqe_process_frame
00000000         *UND*	00000000 __stack_chk_fail
00000000         *UND*	00000000 __stack_chk_guard



Disassembly of section .text.dnvqe_destroy:

00000000 <dnvqe_destroy>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e2504000 	subs	r4, r0, #0
   8:	12845e46 	addne	r5, r4, #1120	; 0x460
   c:	13a06000 	movne	r6, #0
  10:	1a000007 	bne	34 <dnvqe_destroy+0x34>
  14:	e8bd8070 	pop	{r4, r5, r6, pc}
  18:	e5953024 	ldr	r3, [r5, #36]	; 0x24
  1c:	e2866001 	add	r6, r6, #1
  20:	e5150018 	ldr	r0, [r5, #-24]	; 0xffffffe8
  24:	e12fff33 	blx	r3
  28:	e1a00005 	mov	r0, r5
  2c:	e2855044 	add	r5, r5, #68	; 0x44
  30:	ebfffffe 	bl	0 <MODULE_hi_audio_module_handle_deinit>
			30: R_ARM_CALL	MODULE_hi_audio_module_handle_deinit
  34:	e59436f0 	ldr	r3, [r4, #1776]	; 0x6f0
  38:	e1560003 	cmp	r6, r3
  3c:	bafffff5 	blt	18 <dnvqe_destroy+0x18>
  40:	e1a00004 	mov	r0, r4
  44:	e8bd4070 	pop	{r4, r5, r6, lr}
  48:	eafffffe 	b	0 <free>
			48: R_ARM_JUMP24	free

Disassembly of section .text.dnvqe_create:

00000000 <dnvqe_create>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e1a06003 	mov	r6, r3
   8:	e5938020 	ldr	r8, [r3, #32]
   c:	e24dd01c 	sub	sp, sp, #28
  10:	e59f93d4 	ldr	r9, [pc, #980]	; 3ec <dnvqe_create+0x3ec>
  14:	e58d000c 	str	r0, [sp, #12]
  18:	e2483050 	sub	r3, r8, #80	; 0x50
  1c:	e08f9009 	add	r9, pc, r9
  20:	e3530efb 	cmp	r3, #4016	; 0xfb0
  24:	e58d1010 	str	r1, [sp, #16]
  28:	e58d2014 	str	r2, [sp, #20]
  2c:	9a00000d 	bls	68 <dnvqe_create+0x68>
  30:	e59f33b8 	ldr	r3, [pc, #952]	; 3f0 <dnvqe_create+0x3f0>
  34:	e59f23b8 	ldr	r2, [pc, #952]	; 3f4 <dnvqe_create+0x3f4>
  38:	e59f13b8 	ldr	r1, [pc, #952]	; 3f8 <dnvqe_create+0x3f8>
  3c:	e7990003 	ldr	r0, [r9, r3]
  40:	e3003181 	movw	r3, #385	; 0x181
  44:	e08f2002 	add	r2, pc, r2
  48:	e08f1001 	add	r1, pc, r1
  4c:	e58d8000 	str	r8, [sp]
  50:	e5900000 	ldr	r0, [r0]
  54:	e59f53a0 	ldr	r5, [pc, #928]	; 3fc <dnvqe_create+0x3fc>
  58:	ebfffffe 	bl	0 <fprintf>
			58: R_ARM_CALL	fprintf
  5c:	e1a00005 	mov	r0, r5
  60:	e28dd01c 	add	sp, sp, #28
  64:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
  68:	e996000c 	ldmib	r6, {r2, r3}
  6c:	e0823003 	add	r3, r2, r3
  70:	e5962000 	ldr	r2, [r6]
  74:	e0833002 	add	r3, r3, r2
  78:	e596200c 	ldr	r2, [r6, #12]
  7c:	e0833002 	add	r3, r3, r2
  80:	e3530000 	cmp	r3, #0
  84:	da000016 	ble	e4 <dnvqe_create+0xe4>
  88:	e5963018 	ldr	r3, [r6, #24]
  8c:	e3530d7d 	cmp	r3, #8000	; 0x1f40
  90:	13530dfa 	cmpne	r3, #16000	; 0x3e80
  94:	0a00000c 	beq	cc <dnvqe_create+0xcc>
  98:	e30b2b80 	movw	r2, #48000	; 0xbb80
  9c:	e1530002 	cmp	r3, r2
  a0:	03a08e1e 	moveq	r8, #480	; 0x1e0
  a4:	0a00000e 	beq	e4 <dnvqe_create+0xe4>
  a8:	e59f2340 	ldr	r2, [pc, #832]	; 3f0 <dnvqe_create+0x3f0>
  ac:	e59f134c 	ldr	r1, [pc, #844]	; 400 <dnvqe_create+0x400>
  b0:	e7990002 	ldr	r0, [r9, r2]
  b4:	e59f2348 	ldr	r2, [pc, #840]	; 404 <dnvqe_create+0x404>
  b8:	e58d3000 	str	r3, [sp]
  bc:	e300318b 	movw	r3, #395	; 0x18b
  c0:	e08f2002 	add	r2, pc, r2
  c4:	e08f1001 	add	r1, pc, r1
  c8:	eaffffe0 	b	50 <dnvqe_create+0x50>
  cc:	e3530d7d 	cmp	r3, #8000	; 0x1f40
  d0:	03a08050 	moveq	r8, #80	; 0x50
  d4:	0a000002 	beq	e4 <dnvqe_create+0xe4>
  d8:	e3530dfa 	cmp	r3, #16000	; 0x3e80
  dc:	13a08e1e 	movne	r8, #480	; 0x1e0
  e0:	03a080a0 	moveq	r8, #160	; 0xa0
  e4:	e30406f4 	movw	r0, #18164	; 0x46f4
  e8:	ebfffffe 	bl	0 <malloc>
			e8: R_ARM_CALL	malloc
  ec:	e2504000 	subs	r4, r0, #0
  f0:	059f5310 	ldreq	r5, [pc, #784]	; 408 <dnvqe_create+0x408>
  f4:	0affffd8 	beq	5c <dnvqe_create+0x5c>
  f8:	e30436f4 	movw	r3, #18164	; 0x46f4
  fc:	e3a02000 	mov	r2, #0
 100:	e1a01003 	mov	r1, r3
 104:	e3e07000 	mvn	r7, #0
 108:	ebfffffe 	bl	0 <memset_s>
			108: R_ARM_CALL	memset_s
 10c:	e5965000 	ldr	r5, [r6]
 110:	e5847440 	str	r7, [r4, #1088]	; 0x440
 114:	e5847444 	str	r7, [r4, #1092]	; 0x444
 118:	e3550000 	cmp	r5, #0
 11c:	0a00000f 	beq	160 <dnvqe_create+0x160>
 120:	e59f22e4 	ldr	r2, [pc, #740]	; 40c <dnvqe_create+0x40c>
 124:	e2840e46 	add	r0, r4, #1120	; 0x460
 128:	e59f12e0 	ldr	r1, [pc, #736]	; 410 <dnvqe_create+0x410>
 12c:	e08f2002 	add	r2, pc, r2
 130:	e08f1001 	add	r1, pc, r1
 134:	ebfffffe 	bl	0 <MODULE_hi_audio_module_handle_init>
			134: R_ARM_CALL	MODULE_hi_audio_module_handle_init
 138:	e3500000 	cmp	r0, #0
 13c:	13a03000 	movne	r3, #0
 140:	158436f0 	strne	r3, [r4, #1776]	; 0x6f0
 144:	1a000014 	bne	19c <dnvqe_create+0x19c>
 148:	e5963024 	ldr	r3, [r6, #36]	; 0x24
 14c:	e3a05001 	mov	r5, #1
 150:	e584745c 	str	r7, [r4, #1116]	; 0x45c
 154:	e5843454 	str	r3, [r4, #1108]	; 0x454
 158:	e2863028 	add	r3, r6, #40	; 0x28
 15c:	e5843458 	str	r3, [r4, #1112]	; 0x458
 160:	e5963004 	ldr	r3, [r6, #4]
 164:	e3530000 	cmp	r3, #0
 168:	0a000017 	beq	1cc <dnvqe_create+0x1cc>
 16c:	e59f22a0 	ldr	r2, [pc, #672]	; 414 <dnvqe_create+0x414>
 170:	e3a03044 	mov	r3, #68	; 0x44
 174:	e0070593 	mul	r7, r3, r5
 178:	e59f1298 	ldr	r1, [pc, #664]	; 418 <dnvqe_create+0x418>
 17c:	e08f2002 	add	r2, pc, r2
 180:	e2870e46 	add	r0, r7, #1120	; 0x460
 184:	e08f1001 	add	r1, pc, r1
 188:	e0840000 	add	r0, r4, r0
 18c:	ebfffffe 	bl	0 <MODULE_hi_audio_module_handle_init>
			18c: R_ARM_CALL	MODULE_hi_audio_module_handle_init
 190:	e3500000 	cmp	r0, #0
 194:	0a000004 	beq	1ac <dnvqe_create+0x1ac>
 198:	e58456f0 	str	r5, [r4, #1776]	; 0x6f0
 19c:	e59f5278 	ldr	r5, [pc, #632]	; 41c <dnvqe_create+0x41c>
 1a0:	e1a00004 	mov	r0, r4
 1a4:	ebfffffe 	bl	0 <dnvqe_create>
			1a4: R_ARM_CALL	dnvqe_destroy
 1a8:	eaffffab 	b	5c <dnvqe_create+0x5c>
 1ac:	e0843007 	add	r3, r4, r7
 1b0:	e3e02000 	mvn	r2, #0
 1b4:	e2855001 	add	r5, r5, #1
 1b8:	e583245c 	str	r2, [r3, #1116]	; 0x45c
 1bc:	e5962024 	ldr	r2, [r6, #36]	; 0x24
 1c0:	e5832454 	str	r2, [r3, #1108]	; 0x454
 1c4:	e2862030 	add	r2, r6, #48	; 0x30
 1c8:	e5832458 	str	r2, [r3, #1112]	; 0x458
 1cc:	e5963008 	ldr	r3, [r6, #8]
 1d0:	e3530000 	cmp	r3, #0
 1d4:	0a000019 	beq	240 <dnvqe_create+0x240>
 1d8:	e59f2240 	ldr	r2, [pc, #576]	; 420 <dnvqe_create+0x420>
 1dc:	e3a03044 	mov	r3, #68	; 0x44
 1e0:	e0070593 	mul	r7, r3, r5
 1e4:	e59f1238 	ldr	r1, [pc, #568]	; 424 <dnvqe_create+0x424>
 1e8:	e08f2002 	add	r2, pc, r2
 1ec:	e2870e46 	add	r0, r7, #1120	; 0x460
 1f0:	e08f1001 	add	r1, pc, r1
 1f4:	e0840000 	add	r0, r4, r0
 1f8:	ebfffffe 	bl	0 <MODULE_hi_audio_module_handle_init>
			1f8: R_ARM_CALL	MODULE_hi_audio_module_handle_init
 1fc:	e3500000 	cmp	r0, #0
 200:	1affffe4 	bne	198 <dnvqe_create+0x198>
 204:	e596200c 	ldr	r2, [r6, #12]
 208:	e0843007 	add	r3, r4, r7
 20c:	e3520000 	cmp	r2, #0
 210:	e583044c 	str	r0, [r3, #1100]	; 0x44c
 214:	13a02001 	movne	r2, #1
 218:	e5832450 	str	r2, [r3, #1104]	; 0x450
 21c:	e3a03044 	mov	r3, #68	; 0x44
 220:	e0234593 	mla	r3, r3, r5, r4
 224:	e3e02000 	mvn	r2, #0
 228:	e2855001 	add	r5, r5, #1
 22c:	e583245c 	str	r2, [r3, #1116]	; 0x45c
 230:	e5962024 	ldr	r2, [r6, #36]	; 0x24
 234:	e5832454 	str	r2, [r3, #1108]	; 0x454
 238:	e2862040 	add	r2, r6, #64	; 0x40
 23c:	e5832458 	str	r2, [r3, #1112]	; 0x458
 240:	e596300c 	ldr	r3, [r6, #12]
 244:	e3530000 	cmp	r3, #0
 248:	0a000015 	beq	2a4 <dnvqe_create+0x2a4>
 24c:	e59f21d4 	ldr	r2, [pc, #468]	; 428 <dnvqe_create+0x428>
 250:	e3a03044 	mov	r3, #68	; 0x44
 254:	e0070593 	mul	r7, r3, r5
 258:	e59f11cc 	ldr	r1, [pc, #460]	; 42c <dnvqe_create+0x42c>
 25c:	e08f2002 	add	r2, pc, r2
 260:	e2870e46 	add	r0, r7, #1120	; 0x460
 264:	e08f1001 	add	r1, pc, r1
 268:	e0840000 	add	r0, r4, r0
 26c:	ebfffffe 	bl	0 <MODULE_hi_audio_module_handle_init>
			26c: R_ARM_CALL	MODULE_hi_audio_module_handle_init
 270:	e3500000 	cmp	r0, #0
 274:	1affffc7 	bne	198 <dnvqe_create+0x198>
 278:	e0843007 	add	r3, r4, r7
 27c:	e3a02001 	mov	r2, #1
 280:	e2855001 	add	r5, r5, #1
 284:	e583244c 	str	r2, [r3, #1100]	; 0x44c
 288:	e3e02000 	mvn	r2, #0
 28c:	e583245c 	str	r2, [r3, #1116]	; 0x45c
 290:	e5962024 	ldr	r2, [r6, #36]	; 0x24
 294:	e5830450 	str	r0, [r3, #1104]	; 0x450
 298:	e5832454 	str	r2, [r3, #1108]	; 0x454
 29c:	e2862054 	add	r2, r6, #84	; 0x54
 2a0:	e5832458 	str	r2, [r3, #1112]	; 0x458
 2a4:	e5963010 	ldr	r3, [r6, #16]
 2a8:	e3530000 	cmp	r3, #0
 2ac:	0a000010 	beq	2f4 <dnvqe_create+0x2f4>
 2b0:	e59f2178 	ldr	r2, [pc, #376]	; 430 <dnvqe_create+0x430>
 2b4:	e3a07044 	mov	r7, #68	; 0x44
 2b8:	e0070597 	mul	r7, r7, r5
 2bc:	e59f1170 	ldr	r1, [pc, #368]	; 434 <dnvqe_create+0x434>
 2c0:	e08f2002 	add	r2, pc, r2
 2c4:	e2870e46 	add	r0, r7, #1120	; 0x460
 2c8:	e08f1001 	add	r1, pc, r1
 2cc:	e0840000 	add	r0, r4, r0
 2d0:	ebfffffe 	bl	0 <MODULE_hi_audio_module_handle_init>
			2d0: R_ARM_CALL	MODULE_hi_audio_module_handle_init
 2d4:	e3500000 	cmp	r0, #0
 2d8:	1affffae 	bne	198 <dnvqe_create+0x198>
 2dc:	e0847007 	add	r7, r4, r7
 2e0:	e3e03000 	mvn	r3, #0
 2e4:	e2855001 	add	r5, r5, #1
 2e8:	e587345c 	str	r3, [r7, #1116]	; 0x45c
 2ec:	e2863064 	add	r3, r6, #100	; 0x64
 2f0:	e5873458 	str	r3, [r7, #1112]	; 0x458
 2f4:	e58456f0 	str	r5, [r4, #1776]	; 0x6f0
 2f8:	e2847d11 	add	r7, r4, #1088	; 0x440
 2fc:	e2877008 	add	r7, r7, #8
 300:	e3a0a000 	mov	sl, #0
 304:	e3a0b044 	mov	fp, #68	; 0x44
 308:	ea000018 	b	370 <dnvqe_create+0x370>
 30c:	e0030a9b 	mul	r3, fp, sl
 310:	e5961018 	ldr	r1, [r6, #24]
 314:	e2872004 	add	r2, r7, #4
 318:	e58d3008 	str	r3, [sp, #8]
 31c:	e1a00007 	mov	r0, r7
 320:	e5973034 	ldr	r3, [r7, #52]	; 0x34
 324:	e2877044 	add	r7, r7, #68	; 0x44
 328:	e12fff33 	blx	r3
 32c:	e3500000 	cmp	r0, #0
 330:	0a00000d 	beq	36c <dnvqe_create+0x36c>
 334:	e59f30b4 	ldr	r3, [pc, #180]	; 3f0 <dnvqe_create+0x3f0>
 338:	e59f20f8 	ldr	r2, [pc, #248]	; 438 <dnvqe_create+0x438>
 33c:	e7990003 	ldr	r0, [r9, r3]
 340:	e59d3008 	ldr	r3, [sp, #8]
 344:	e59f10f0 	ldr	r1, [pc, #240]	; 43c <dnvqe_create+0x43c>
 348:	e08f2002 	add	r2, pc, r2
 34c:	e2833e46 	add	r3, r3, #1120	; 0x460
 350:	e0843003 	add	r3, r4, r3
 354:	e58d3000 	str	r3, [sp]
 358:	e3003163 	movw	r3, #355	; 0x163
 35c:	e08f1001 	add	r1, pc, r1
 360:	e5900000 	ldr	r0, [r0]
 364:	ebfffffe 	bl	0 <fprintf>
			364: R_ARM_CALL	fprintf
 368:	eaffff8b 	b	19c <dnvqe_create+0x19c>
 36c:	e28aa001 	add	sl, sl, #1
 370:	e15a0005 	cmp	sl, r5
 374:	1affffe4 	bne	30c <dnvqe_create+0x30c>
 378:	e300343c 	movw	r3, #1084	; 0x43c
 37c:	e1a02006 	mov	r2, r6
 380:	e1a01003 	mov	r1, r3
 384:	e1a00004 	mov	r0, r4
 388:	ebfffffe 	bl	0 <memcpy_s>
			388: R_ARM_CALL	memcpy_s
 38c:	e2505000 	subs	r5, r0, #0
 390:	0a00000b 	beq	3c4 <dnvqe_create+0x3c4>
 394:	e59f3054 	ldr	r3, [pc, #84]	; 3f0 <dnvqe_create+0x3f0>
 398:	e59f20a0 	ldr	r2, [pc, #160]	; 440 <dnvqe_create+0x440>
 39c:	e7990003 	ldr	r0, [r9, r3]
 3a0:	e30031a5 	movw	r3, #421	; 0x1a5
 3a4:	e59f1098 	ldr	r1, [pc, #152]	; 444 <dnvqe_create+0x444>
 3a8:	e58d5000 	str	r5, [sp]
 3ac:	e08f2002 	add	r2, pc, r2
 3b0:	e08f1001 	add	r1, pc, r1
 3b4:	e5900000 	ldr	r0, [r0]
 3b8:	e59f503c 	ldr	r5, [pc, #60]	; 3fc <dnvqe_create+0x3fc>
 3bc:	ebfffffe 	bl	0 <fprintf>
			3bc: R_ARM_CALL	fprintf
 3c0:	eaffff76 	b	1a0 <dnvqe_create+0x1a0>
 3c4:	e59d300c 	ldr	r3, [sp, #12]
 3c8:	e300243c 	movw	r2, #1084	; 0x43c
 3cc:	e584843c 	str	r8, [r4, #1084]	; 0x43c
 3d0:	e1a01004 	mov	r1, r4
 3d4:	e5834000 	str	r4, [r3]
 3d8:	e59d3014 	ldr	r3, [sp, #20]
 3dc:	e59d0010 	ldr	r0, [sp, #16]
 3e0:	e5838000 	str	r8, [r3]
 3e4:	ebfffffe 	bl	0 <memcpy>
			3e4: R_ARM_CALL	memcpy
 3e8:	eaffff1b 	b	5c <dnvqe_create+0x5c>
 3ec:	000003c8 	.word	0x000003c8
			3ec: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 3f0:	00000000 	.word	0x00000000
			3f0: R_ARM_GOT32	stderr
 3f4:	000003a8 	.word	0x000003a8
			3f4: R_ARM_REL32	.rodata.__FUNCTION__.8263
 3f8:	000003a8 	.word	0x000003a8
			3f8: R_ARM_REL32	.LC0
 3fc:	a1360002 	.word	0xa1360002
 400:	00000334 	.word	0x00000334
			400: R_ARM_REL32	.LC1
 404:	0000033c 	.word	0x0000033c
			404: R_ARM_REL32	.rodata.__FUNCTION__.8263
 408:	a1360003 	.word	0xa1360003
 40c:	000002d8 	.word	0x000002d8
			40c: R_ARM_REL32	.LC2
 410:	000002d8 	.word	0x000002d8
			410: R_ARM_REL32	.LC3
 414:	00000290 	.word	0x00000290
			414: R_ARM_REL32	.LC4
 418:	0000028c 	.word	0x0000028c
			418: R_ARM_REL32	.LC3
 41c:	a1360009 	.word	0xa1360009
 420:	00000230 	.word	0x00000230
			420: R_ARM_REL32	.LC5
 424:	0000022c 	.word	0x0000022c
			424: R_ARM_REL32	.LC3
 428:	000001c4 	.word	0x000001c4
			428: R_ARM_REL32	.LC6
 42c:	000001c0 	.word	0x000001c0
			42c: R_ARM_REL32	.LC3
 430:	00000168 	.word	0x00000168
			430: R_ARM_REL32	.LC7
 434:	00000164 	.word	0x00000164
			434: R_ARM_REL32	.LC3
 438:	000000e8 	.word	0x000000e8
			438: R_ARM_REL32	.rodata.__FUNCTION__.8246
 43c:	000000d8 	.word	0x000000d8
			43c: R_ARM_REL32	.LC8
 440:	0000008c 	.word	0x0000008c
			440: R_ARM_REL32	.rodata.__FUNCTION__.8263
 444:	0000008c 	.word	0x0000008c
			444: R_ARM_REL32	.LC9

Disassembly of section .text.dnvqe_process_frame:

00000000 <dnvqe_process_frame>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e1a09002 	mov	r9, r2
   8:	e59f2228 	ldr	r2, [pc, #552]	; 238 <dnvqe_process_frame+0x238>
   c:	e1a04003 	mov	r4, r3
  10:	e59f3224 	ldr	r3, [pc, #548]	; 23c <dnvqe_process_frame+0x23c>
  14:	e24dd044 	sub	sp, sp, #68	; 0x44
  18:	e08f2002 	add	r2, pc, r2
  1c:	e2507000 	subs	r7, r0, #0
  20:	e59f5218 	ldr	r5, [pc, #536]	; 240 <dnvqe_process_frame+0x240>
  24:	e7923003 	ldr	r3, [r2, r3]
  28:	e08f5005 	add	r5, pc, r5
  2c:	e5933000 	ldr	r3, [r3]
  30:	e58d303c 	str	r3, [sp, #60]	; 0x3c
  34:	e3a03000 	mov	r3, #0
  38:	1a000013 	bne	8c <dnvqe_process_frame+0x8c>
  3c:	e59f3200 	ldr	r3, [pc, #512]	; 244 <dnvqe_process_frame+0x244>
  40:	e59f2200 	ldr	r2, [pc, #512]	; 248 <dnvqe_process_frame+0x248>
  44:	e7950003 	ldr	r0, [r5, r3]
  48:	e3003207 	movw	r3, #519	; 0x207
  4c:	e59f11f8 	ldr	r1, [pc, #504]	; 24c <dnvqe_process_frame+0x24c>
  50:	e08f2002 	add	r2, pc, r2
  54:	e08f1001 	add	r1, pc, r1
  58:	e5900000 	ldr	r0, [r0]
  5c:	ebfffffe 	bl	0 <fprintf>
			5c: R_ARM_CALL	fprintf
  60:	e59f41e8 	ldr	r4, [pc, #488]	; 250 <dnvqe_process_frame+0x250>
  64:	e59f21e8 	ldr	r2, [pc, #488]	; 254 <dnvqe_process_frame+0x254>
  68:	e1a00004 	mov	r0, r4
  6c:	e59f31c8 	ldr	r3, [pc, #456]	; 23c <dnvqe_process_frame+0x23c>
  70:	e08f2002 	add	r2, pc, r2
  74:	e7923003 	ldr	r3, [r2, r3]
  78:	e5932000 	ldr	r2, [r3]
  7c:	e59d303c 	ldr	r3, [sp, #60]	; 0x3c
  80:	e0332002 	eors	r2, r3, r2
  84:	0a000069 	beq	230 <dnvqe_process_frame+0x230>
  88:	ebfffffe 	bl	0 <__stack_chk_fail>
			88: R_ARM_CALL	__stack_chk_fail
  8c:	e3590000 	cmp	r9, #0
  90:	13510000 	cmpne	r1, #0
  94:	e1a08001 	mov	r8, r1
  98:	e597b43c 	ldr	fp, [r7, #1084]	; 0x43c
  9c:	1a00000a 	bne	cc <dnvqe_process_frame+0xcc>
  a0:	e59f319c 	ldr	r3, [pc, #412]	; 244 <dnvqe_process_frame+0x244>
  a4:	e59f21ac 	ldr	r2, [pc, #428]	; 258 <dnvqe_process_frame+0x258>
  a8:	e59f11ac 	ldr	r1, [pc, #428]	; 25c <dnvqe_process_frame+0x25c>
  ac:	e7950003 	ldr	r0, [r5, r3]
  b0:	e3a03e21 	mov	r3, #528	; 0x210
  b4:	e08f2002 	add	r2, pc, r2
  b8:	e08f1001 	add	r1, pc, r1
  bc:	e88d0300 	stm	sp, {r8, r9}
  c0:	e5900000 	ldr	r0, [r0]
  c4:	ebfffffe 	bl	0 <fprintf>
			c4: R_ARM_CALL	fprintf
  c8:	eaffffe4 	b	60 <dnvqe_process_frame+0x60>
  cc:	e713fb14 	sdiv	r3, r4, fp
  d0:	e063439b 	mls	r3, fp, r3, r4
  d4:	e3530000 	cmp	r3, #0
  d8:	0a000008 	beq	100 <dnvqe_process_frame+0x100>
  dc:	e59f3160 	ldr	r3, [pc, #352]	; 244 <dnvqe_process_frame+0x244>
  e0:	e59f2178 	ldr	r2, [pc, #376]	; 260 <dnvqe_process_frame+0x260>
  e4:	e59f1178 	ldr	r1, [pc, #376]	; 264 <dnvqe_process_frame+0x264>
  e8:	e7950003 	ldr	r0, [r5, r3]
  ec:	e3003215 	movw	r3, #533	; 0x215
  f0:	e88d0810 	stm	sp, {r4, fp}
  f4:	e08f2002 	add	r2, pc, r2
  f8:	e08f1001 	add	r1, pc, r1
  fc:	eaffffef 	b	c0 <dnvqe_process_frame+0xc0>
 100:	e59736f0 	ldr	r3, [r7, #1776]	; 0x6f0
 104:	e3530000 	cmp	r3, #0
 108:	0a000021 	beq	194 <dnvqe_process_frame+0x194>
 10c:	e1a0308b 	lsl	r3, fp, #1
 110:	e58d3008 	str	r3, [sp, #8]
 114:	e2873d9b 	add	r3, r7, #9920	; 0x26c0
 118:	e2833034 	add	r3, r3, #52	; 0x34
 11c:	e58d300c 	str	r3, [sp, #12]
 120:	e3540000 	cmp	r4, #0
 124:	0affffce 	beq	64 <dnvqe_process_frame+0x64>
 128:	e3a03014 	mov	r3, #20
 12c:	e3a02000 	mov	r2, #0
 130:	e08d5003 	add	r5, sp, r3
 134:	e1a01003 	mov	r1, r3
 138:	e1a00005 	mov	r0, r5
 13c:	e28d6028 	add	r6, sp, #40	; 0x28
 140:	e3a0a000 	mov	sl, #0
 144:	ebfffffe 	bl	0 <memset_s>
			144: R_ARM_CALL	memset_s
 148:	e3a03014 	mov	r3, #20
 14c:	e3a02000 	mov	r2, #0
 150:	e1a01003 	mov	r1, r3
 154:	e1a00006 	mov	r0, r6
 158:	ebfffffe 	bl	0 <memset_s>
			158: R_ARM_CALL	memset_s
 15c:	e59d3008 	ldr	r3, [sp, #8]
 160:	e58d8014 	str	r8, [sp, #20]
 164:	e58d3024 	str	r3, [sp, #36]	; 0x24
 168:	e2873e6f 	add	r3, r7, #1776	; 0x6f0
 16c:	e2833004 	add	r3, r3, #4
 170:	e58d3028 	str	r3, [sp, #40]	; 0x28
 174:	e59736f0 	ldr	r3, [r7, #1776]	; 0x6f0
 178:	e153000a 	cmp	r3, sl
 17c:	ca000017 	bgt	1e0 <dnvqe_process_frame+0x1e0>
 180:	e59d3008 	ldr	r3, [sp, #8]
 184:	e044400b 	sub	r4, r4, fp
 188:	e0888003 	add	r8, r8, r3
 18c:	e0899003 	add	r9, r9, r3
 190:	eaffffe2 	b	120 <dnvqe_process_frame+0x120>
 194:	e1a03084 	lsl	r3, r4, #1
 198:	e1a02001 	mov	r2, r1
 19c:	e1a00009 	mov	r0, r9
 1a0:	e1a01003 	mov	r1, r3
 1a4:	ebfffffe 	bl	0 <memcpy_s>
			1a4: R_ARM_CALL	memcpy_s
 1a8:	e2504000 	subs	r4, r0, #0
 1ac:	0affffac 	beq	64 <dnvqe_process_frame+0x64>
 1b0:	e59f308c 	ldr	r3, [pc, #140]	; 244 <dnvqe_process_frame+0x244>
 1b4:	e59f20ac 	ldr	r2, [pc, #172]	; 268 <dnvqe_process_frame+0x268>
 1b8:	e7950003 	ldr	r0, [r5, r3]
 1bc:	e300321d 	movw	r3, #541	; 0x21d
 1c0:	e59f10a4 	ldr	r1, [pc, #164]	; 26c <dnvqe_process_frame+0x26c>
 1c4:	e58d4000 	str	r4, [sp]
 1c8:	e08f2002 	add	r2, pc, r2
 1cc:	e08f1001 	add	r1, pc, r1
 1d0:	e5900000 	ldr	r0, [r0]
 1d4:	ebfffffe 	bl	0 <fprintf>
			1d4: R_ARM_CALL	fprintf
 1d8:	e59f4090 	ldr	r4, [pc, #144]	; 270 <dnvqe_process_frame+0x270>
 1dc:	eaffffa0 	b	64 <dnvqe_process_frame+0x64>
 1e0:	e2433001 	sub	r3, r3, #1
 1e4:	e1a02006 	mov	r2, r6
 1e8:	e153000a 	cmp	r3, sl
 1ec:	e3a03044 	mov	r3, #68	; 0x44
 1f0:	e0207a93 	mla	r0, r3, sl, r7
 1f4:	05869000 	streq	r9, [r6]
 1f8:	e1a01005 	mov	r1, r5
 1fc:	e5903480 	ldr	r3, [r0, #1152]	; 0x480
 200:	e5900448 	ldr	r0, [r0, #1096]	; 0x448
 204:	e12fff33 	blx	r3
 208:	e3500000 	cmp	r0, #0
 20c:	1afffff1 	bne	1d8 <dnvqe_process_frame+0x1d8>
 210:	e35a0000 	cmp	sl, #0
 214:	e28aa001 	add	sl, sl, #1
 218:	059d300c 	ldreq	r3, [sp, #12]
 21c:	05853000 	streq	r3, [r5]
 220:	e1a03005 	mov	r3, r5
 224:	e1a05006 	mov	r5, r6
 228:	e1a06003 	mov	r6, r3
 22c:	eaffffd0 	b	174 <dnvqe_process_frame+0x174>
 230:	e28dd044 	add	sp, sp, #68	; 0x44
 234:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 238:	00000218 	.word	0x00000218
			238: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 23c:	00000000 	.word	0x00000000
			23c: R_ARM_GOT32	__stack_chk_guard
 240:	00000210 	.word	0x00000210
			240: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 244:	00000000 	.word	0x00000000
			244: R_ARM_GOT32	stderr
 248:	000001f0 	.word	0x000001f0
			248: R_ARM_REL32	.rodata.__FUNCTION__.8303
 24c:	000001f0 	.word	0x000001f0
			24c: R_ARM_REL32	.LC10
 250:	a1360002 	.word	0xa1360002
 254:	000001dc 	.word	0x000001dc
			254: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 258:	0000019c 	.word	0x0000019c
			258: R_ARM_REL32	.rodata.__FUNCTION__.8303
 25c:	0000019c 	.word	0x0000019c
			25c: R_ARM_REL32	.LC11
 260:	00000164 	.word	0x00000164
			260: R_ARM_REL32	.rodata.__FUNCTION__.8303
 264:	00000164 	.word	0x00000164
			264: R_ARM_REL32	.LC12
 268:	00000098 	.word	0x00000098
			268: R_ARM_REL32	.rodata.__FUNCTION__.8303
 26c:	00000098 	.word	0x00000098
			26c: R_ARM_REL32	.LC13
 270:	a1360004 	.word	0xa1360004
