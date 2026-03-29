
dnvqe_work.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l       .rodata.DNVQE_Create.str1.1	00000000 .LC0
00000022 l       .rodata.DNVQE_Create.str1.1	00000000 .LC1
00000047 l       .rodata.DNVQE_Create.str1.1	00000000 .LC2
0000004b l       .rodata.DNVQE_Create.str1.1	00000000 .LC3
00000050 l       .rodata.DNVQE_Create.str1.1	00000000 .LC4
00000054 l       .rodata.DNVQE_Create.str1.1	00000000 .LC5
00000058 l       .rodata.DNVQE_Create.str1.1	00000000 .LC6
0000005b l       .rodata.DNVQE_Create.str1.1	00000000 .LC7
0000005f l       .rodata.DNVQE_Create.str1.1	00000000 .LC8
00000000 l       .rodata.DNVQE_ProcessFrame.str1.1	00000000 .LC9
00000016 l       .rodata.DNVQE_ProcessFrame.str1.1	00000000 .LC10
00000043 l       .rodata.DNVQE_ProcessFrame.str1.1	00000000 .LC11
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.DNVQE_Destroy	00000000 .text.DNVQE_Destroy
00000000 l    d  .text.DNVQE_Create	00000000 .text.DNVQE_Create
00000000 l    d  .text.DNVQE_ProcessFrame	00000000 .text.DNVQE_ProcessFrame
00000000 l    d  .rodata.DNVQE_Create.str1.1	00000000 .rodata.DNVQE_Create.str1.1
00000000 l    d  .rodata.DNVQE_ProcessFrame.str1.1	00000000 .rodata.DNVQE_ProcessFrame.str1.1
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.DNVQE_Destroy	0000004c DNVQE_Destroy
00000000         *UND*	00000000 MODULE_HI_Audio_ModuleHandleDeInit
00000000         *UND*	00000000 free
00000000 g     F .text.DNVQE_Create	000003c0 DNVQE_Create
00000000         *UND*	00000000 fprintf
00000000         *UND*	00000000 malloc
00000000         *UND*	00000000 memset_s
00000000         *UND*	00000000 MODULE_HI_Audio_ModuleHandleInit
00000000         *UND*	00000000 memcpy_s
00000000         *UND*	00000000 memcpy
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 stderr
00000000 g     F .text.DNVQE_ProcessFrame	000001f4 DNVQE_ProcessFrame
00000000         *UND*	00000000 fputs



Disassembly of section .text.DNVQE_Destroy:

00000000 <DNVQE_Destroy>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e2504000 	subs	r4, r0, #0
   8:	12845e46 	addne	r5, r4, #1120	; 0x460
   c:	13a06000 	movne	r6, #0
  10:	1a000007 	bne	34 <DNVQE_Destroy+0x34>
  14:	e8bd8070 	pop	{r4, r5, r6, pc}
  18:	e5953024 	ldr	r3, [r5, #36]	; 0x24
  1c:	e2866001 	add	r6, r6, #1
  20:	e5150018 	ldr	r0, [r5, #-24]	; 0xffffffe8
  24:	e12fff33 	blx	r3
  28:	e1a00005 	mov	r0, r5
  2c:	e2855040 	add	r5, r5, #64	; 0x40
  30:	ebfffffe 	bl	0 <MODULE_HI_Audio_ModuleHandleDeInit>
			30: R_ARM_CALL	MODULE_HI_Audio_ModuleHandleDeInit
  34:	e59436c8 	ldr	r3, [r4, #1736]	; 0x6c8
  38:	e1560003 	cmp	r6, r3
  3c:	bafffff5 	blt	18 <DNVQE_Destroy+0x18>
  40:	e1a00004 	mov	r0, r4
  44:	e8bd4070 	pop	{r4, r5, r6, lr}
  48:	eafffffe 	b	0 <free>
			48: R_ARM_JUMP24	free

Disassembly of section .text.DNVQE_Create:

00000000 <DNVQE_Create>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e1a05003 	mov	r5, r3
   8:	e5937020 	ldr	r7, [r3, #32]
   c:	e24dd014 	sub	sp, sp, #20
  10:	e59fa360 	ldr	sl, [pc, #864]	; 378 <DNVQE_Create+0x378>
  14:	e2473050 	sub	r3, r7, #80	; 0x50
  18:	e08fa00a 	add	sl, pc, sl
  1c:	e3530efb 	cmp	r3, #4016	; 0xfb0
  20:	e98d0006 	stmib	sp, {r1, r2}
  24:	9a00000a 	bls	54 <DNVQE_Create+0x54>
  28:	e59f334c 	ldr	r3, [pc, #844]	; 37c <DNVQE_Create+0x37c>
  2c:	e1a02007 	mov	r2, r7
  30:	e59f1348 	ldr	r1, [pc, #840]	; 380 <DNVQE_Create+0x380>
  34:	e79a3003 	ldr	r3, [sl, r3]
  38:	e08f1001 	add	r1, pc, r1
  3c:	e5930000 	ldr	r0, [r3]
  40:	e59f833c 	ldr	r8, [pc, #828]	; 384 <DNVQE_Create+0x384>
  44:	ebfffffe 	bl	0 <fprintf>
			44: R_ARM_CALL	fprintf
  48:	e1a00008 	mov	r0, r8
  4c:	e28dd014 	add	sp, sp, #20
  50:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
  54:	e995000c 	ldmib	r5, {r2, r3}
  58:	e0823003 	add	r3, r2, r3
  5c:	e1a0b000 	mov	fp, r0
  60:	e5952000 	ldr	r2, [r5]
  64:	e0833002 	add	r3, r3, r2
  68:	e595200c 	ldr	r2, [r5, #12]
  6c:	e0833002 	add	r3, r3, r2
  70:	e3530000 	cmp	r3, #0
  74:	da000013 	ble	c8 <DNVQE_Create+0xc8>
  78:	e5952018 	ldr	r2, [r5, #24]
  7c:	e3520d7d 	cmp	r2, #8000	; 0x1f40
  80:	13520dfa 	cmpne	r2, #16000	; 0x3e80
  84:	0a000008 	beq	ac <DNVQE_Create+0xac>
  88:	e30b3b80 	movw	r3, #48000	; 0xbb80
  8c:	e1520003 	cmp	r2, r3
  90:	03a07e1e 	moveq	r7, #480	; 0x1e0
  94:	0a00000b 	beq	c8 <DNVQE_Create+0xc8>
  98:	e59f32dc 	ldr	r3, [pc, #732]	; 37c <DNVQE_Create+0x37c>
  9c:	e59f12e4 	ldr	r1, [pc, #740]	; 388 <DNVQE_Create+0x388>
  a0:	e79a3003 	ldr	r3, [sl, r3]
  a4:	e08f1001 	add	r1, pc, r1
  a8:	eaffffe3 	b	3c <DNVQE_Create+0x3c>
  ac:	e3520d7d 	cmp	r2, #8000	; 0x1f40
  b0:	03a07050 	moveq	r7, #80	; 0x50
  b4:	0a000003 	beq	c8 <DNVQE_Create+0xc8>
  b8:	e30b7b80 	movw	r7, #48000	; 0xbb80
  bc:	e1520007 	cmp	r2, r7
  c0:	03a07e1e 	moveq	r7, #480	; 0x1e0
  c4:	13a070a0 	movne	r7, #160	; 0xa0
  c8:	e30406cc 	movw	r0, #18124	; 0x46cc
  cc:	ebfffffe 	bl	0 <malloc>
			cc: R_ARM_CALL	malloc
  d0:	e2504000 	subs	r4, r0, #0
  d4:	059f82b0 	ldreq	r8, [pc, #688]	; 38c <DNVQE_Create+0x38c>
  d8:	0affffda 	beq	48 <DNVQE_Create+0x48>
  dc:	e30436cc 	movw	r3, #18124	; 0x46cc
  e0:	e3a02000 	mov	r2, #0
  e4:	e1a01003 	mov	r1, r3
  e8:	e3e08000 	mvn	r8, #0
  ec:	ebfffffe 	bl	0 <memset_s>
			ec: R_ARM_CALL	memset_s
  f0:	e5956000 	ldr	r6, [r5]
  f4:	e5848440 	str	r8, [r4, #1088]	; 0x440
  f8:	e5848444 	str	r8, [r4, #1092]	; 0x444
  fc:	e3560000 	cmp	r6, #0
 100:	0a00000f 	beq	144 <DNVQE_Create+0x144>
 104:	e59f2284 	ldr	r2, [pc, #644]	; 390 <DNVQE_Create+0x390>
 108:	e2840e46 	add	r0, r4, #1120	; 0x460
 10c:	e59f1280 	ldr	r1, [pc, #640]	; 394 <DNVQE_Create+0x394>
 110:	e08f2002 	add	r2, pc, r2
 114:	e08f1001 	add	r1, pc, r1
 118:	ebfffffe 	bl	0 <MODULE_HI_Audio_ModuleHandleInit>
			118: R_ARM_CALL	MODULE_HI_Audio_ModuleHandleInit
 11c:	e3500000 	cmp	r0, #0
 120:	13a03000 	movne	r3, #0
 124:	158436c8 	strne	r3, [r4, #1736]	; 0x6c8
 128:	1a000013 	bne	17c <DNVQE_Create+0x17c>
 12c:	e5953024 	ldr	r3, [r5, #36]	; 0x24
 130:	e3a06001 	mov	r6, #1
 134:	e584845c 	str	r8, [r4, #1116]	; 0x45c
 138:	e5843454 	str	r3, [r4, #1108]	; 0x454
 13c:	e2853028 	add	r3, r5, #40	; 0x28
 140:	e5843458 	str	r3, [r4, #1112]	; 0x458
 144:	e5953004 	ldr	r3, [r5, #4]
 148:	e3530000 	cmp	r3, #0
 14c:	0a000016 	beq	1ac <DNVQE_Create+0x1ac>
 150:	e1a08306 	lsl	r8, r6, #6
 154:	e59f223c 	ldr	r2, [pc, #572]	; 398 <DNVQE_Create+0x398>
 158:	e2880e46 	add	r0, r8, #1120	; 0x460
 15c:	e59f1238 	ldr	r1, [pc, #568]	; 39c <DNVQE_Create+0x39c>
 160:	e08f2002 	add	r2, pc, r2
 164:	e08f1001 	add	r1, pc, r1
 168:	e0840000 	add	r0, r4, r0
 16c:	ebfffffe 	bl	0 <MODULE_HI_Audio_ModuleHandleInit>
			16c: R_ARM_CALL	MODULE_HI_Audio_ModuleHandleInit
 170:	e3500000 	cmp	r0, #0
 174:	0a000004 	beq	18c <DNVQE_Create+0x18c>
 178:	e58466c8 	str	r6, [r4, #1736]	; 0x6c8
 17c:	e59f821c 	ldr	r8, [pc, #540]	; 3a0 <DNVQE_Create+0x3a0>
 180:	e1a00004 	mov	r0, r4
 184:	ebfffffe 	bl	0 <DNVQE_Create>
			184: R_ARM_CALL	DNVQE_Destroy
 188:	eaffffae 	b	48 <DNVQE_Create+0x48>
 18c:	e0843008 	add	r3, r4, r8
 190:	e3e02000 	mvn	r2, #0
 194:	e2866001 	add	r6, r6, #1
 198:	e583245c 	str	r2, [r3, #1116]	; 0x45c
 19c:	e5952024 	ldr	r2, [r5, #36]	; 0x24
 1a0:	e5832454 	str	r2, [r3, #1108]	; 0x454
 1a4:	e2852030 	add	r2, r5, #48	; 0x30
 1a8:	e5832458 	str	r2, [r3, #1112]	; 0x458
 1ac:	e5953008 	ldr	r3, [r5, #8]
 1b0:	e3530000 	cmp	r3, #0
 1b4:	0a000017 	beq	218 <DNVQE_Create+0x218>
 1b8:	e1a08306 	lsl	r8, r6, #6
 1bc:	e59f21e0 	ldr	r2, [pc, #480]	; 3a4 <DNVQE_Create+0x3a4>
 1c0:	e2880e46 	add	r0, r8, #1120	; 0x460
 1c4:	e59f11dc 	ldr	r1, [pc, #476]	; 3a8 <DNVQE_Create+0x3a8>
 1c8:	e08f2002 	add	r2, pc, r2
 1cc:	e08f1001 	add	r1, pc, r1
 1d0:	e0840000 	add	r0, r4, r0
 1d4:	ebfffffe 	bl	0 <MODULE_HI_Audio_ModuleHandleInit>
			1d4: R_ARM_CALL	MODULE_HI_Audio_ModuleHandleInit
 1d8:	e3500000 	cmp	r0, #0
 1dc:	1affffe5 	bne	178 <DNVQE_Create+0x178>
 1e0:	e595200c 	ldr	r2, [r5, #12]
 1e4:	e2866001 	add	r6, r6, #1
 1e8:	e0843008 	add	r3, r4, r8
 1ec:	e0848008 	add	r8, r4, r8
 1f0:	e3520000 	cmp	r2, #0
 1f4:	e583044c 	str	r0, [r3, #1100]	; 0x44c
 1f8:	13a02001 	movne	r2, #1
 1fc:	e5832450 	str	r2, [r3, #1104]	; 0x450
 200:	e3e03000 	mvn	r3, #0
 204:	e588345c 	str	r3, [r8, #1116]	; 0x45c
 208:	e5953024 	ldr	r3, [r5, #36]	; 0x24
 20c:	e5883454 	str	r3, [r8, #1108]	; 0x454
 210:	e2853040 	add	r3, r5, #64	; 0x40
 214:	e5883458 	str	r3, [r8, #1112]	; 0x458
 218:	e595300c 	ldr	r3, [r5, #12]
 21c:	e3530000 	cmp	r3, #0
 220:	0a000014 	beq	278 <DNVQE_Create+0x278>
 224:	e1a08306 	lsl	r8, r6, #6
 228:	e59f217c 	ldr	r2, [pc, #380]	; 3ac <DNVQE_Create+0x3ac>
 22c:	e2880e46 	add	r0, r8, #1120	; 0x460
 230:	e59f1178 	ldr	r1, [pc, #376]	; 3b0 <DNVQE_Create+0x3b0>
 234:	e08f2002 	add	r2, pc, r2
 238:	e08f1001 	add	r1, pc, r1
 23c:	e0840000 	add	r0, r4, r0
 240:	ebfffffe 	bl	0 <MODULE_HI_Audio_ModuleHandleInit>
			240: R_ARM_CALL	MODULE_HI_Audio_ModuleHandleInit
 244:	e3500000 	cmp	r0, #0
 248:	1affffca 	bne	178 <DNVQE_Create+0x178>
 24c:	e0843008 	add	r3, r4, r8
 250:	e3a02001 	mov	r2, #1
 254:	e2866001 	add	r6, r6, #1
 258:	e583244c 	str	r2, [r3, #1100]	; 0x44c
 25c:	e3e02000 	mvn	r2, #0
 260:	e583245c 	str	r2, [r3, #1116]	; 0x45c
 264:	e5952024 	ldr	r2, [r5, #36]	; 0x24
 268:	e5830450 	str	r0, [r3, #1104]	; 0x450
 26c:	e5832454 	str	r2, [r3, #1108]	; 0x454
 270:	e2852054 	add	r2, r5, #84	; 0x54
 274:	e5832458 	str	r2, [r3, #1112]	; 0x458
 278:	e5953010 	ldr	r3, [r5, #16]
 27c:	e3530000 	cmp	r3, #0
 280:	0a00000f 	beq	2c4 <DNVQE_Create+0x2c4>
 284:	e1a08306 	lsl	r8, r6, #6
 288:	e59f2124 	ldr	r2, [pc, #292]	; 3b4 <DNVQE_Create+0x3b4>
 28c:	e2880e46 	add	r0, r8, #1120	; 0x460
 290:	e59f1120 	ldr	r1, [pc, #288]	; 3b8 <DNVQE_Create+0x3b8>
 294:	e08f2002 	add	r2, pc, r2
 298:	e08f1001 	add	r1, pc, r1
 29c:	e0840000 	add	r0, r4, r0
 2a0:	ebfffffe 	bl	0 <MODULE_HI_Audio_ModuleHandleInit>
			2a0: R_ARM_CALL	MODULE_HI_Audio_ModuleHandleInit
 2a4:	e3500000 	cmp	r0, #0
 2a8:	1affffb2 	bne	178 <DNVQE_Create+0x178>
 2ac:	e0848008 	add	r8, r4, r8
 2b0:	e3e03000 	mvn	r3, #0
 2b4:	e2866001 	add	r6, r6, #1
 2b8:	e588345c 	str	r3, [r8, #1116]	; 0x45c
 2bc:	e2853064 	add	r3, r5, #100	; 0x64
 2c0:	e5883458 	str	r3, [r8, #1112]	; 0x458
 2c4:	e58466c8 	str	r6, [r4, #1736]	; 0x6c8
 2c8:	e2849d11 	add	r9, r4, #1088	; 0x440
 2cc:	e2899008 	add	r9, r9, #8
 2d0:	e3a03000 	mov	r3, #0
 2d4:	e1560003 	cmp	r6, r3
 2d8:	1a00000e 	bne	318 <DNVQE_Create+0x318>
 2dc:	e300343c 	movw	r3, #1084	; 0x43c
 2e0:	e1a02005 	mov	r2, r5
 2e4:	e1a01003 	mov	r1, r3
 2e8:	e1a00004 	mov	r0, r4
 2ec:	e3a08000 	mov	r8, #0
 2f0:	ebfffffe 	bl	0 <memcpy_s>
			2f0: R_ARM_CALL	memcpy_s
 2f4:	e59d3008 	ldr	r3, [sp, #8]
 2f8:	e300243c 	movw	r2, #1084	; 0x43c
 2fc:	e58b4000 	str	r4, [fp]
 300:	e1a01004 	mov	r1, r4
 304:	e584743c 	str	r7, [r4, #1084]	; 0x43c
 308:	e5837000 	str	r7, [r3]
 30c:	e59d0004 	ldr	r0, [sp, #4]
 310:	ebfffffe 	bl	0 <memcpy>
			310: R_ARM_CALL	memcpy
 314:	eaffff4b 	b	48 <DNVQE_Create+0x48>
 318:	e1a02303 	lsl	r2, r3, #6
 31c:	e1a00009 	mov	r0, r9
 320:	e5998034 	ldr	r8, [r9, #52]	; 0x34
 324:	e58d2000 	str	r2, [sp]
 328:	e2892004 	add	r2, r9, #4
 32c:	e5951018 	ldr	r1, [r5, #24]
 330:	e2899040 	add	r9, r9, #64	; 0x40
 334:	e58d300c 	str	r3, [sp, #12]
 338:	e12fff38 	blx	r8
 33c:	e59d300c 	ldr	r3, [sp, #12]
 340:	e2508000 	subs	r8, r0, #0
 344:	0a000009 	beq	370 <DNVQE_Create+0x370>
 348:	e59d3000 	ldr	r3, [sp]
 34c:	e59f1068 	ldr	r1, [pc, #104]	; 3bc <DNVQE_Create+0x3bc>
 350:	e2832e46 	add	r2, r3, #1120	; 0x460
 354:	e59f3020 	ldr	r3, [pc, #32]	; 37c <DNVQE_Create+0x37c>
 358:	e0842002 	add	r2, r4, r2
 35c:	e79a3003 	ldr	r3, [sl, r3]
 360:	e08f1001 	add	r1, pc, r1
 364:	e5930000 	ldr	r0, [r3]
 368:	ebfffffe 	bl	0 <fprintf>
			368: R_ARM_CALL	fprintf
 36c:	eaffff83 	b	180 <DNVQE_Create+0x180>
 370:	e2833001 	add	r3, r3, #1
 374:	eaffffd6 	b	2d4 <DNVQE_Create+0x2d4>
 378:	00000358 	.word	0x00000358
			378: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 37c:	00000000 	.word	0x00000000
			37c: R_ARM_GOT32	stderr
 380:	00000340 	.word	0x00000340
			380: R_ARM_REL32	.LC0
 384:	a1360002 	.word	0xa1360002
 388:	000002dc 	.word	0x000002dc
			388: R_ARM_REL32	.LC1
 38c:	a1360003 	.word	0xa1360003
 390:	00000278 	.word	0x00000278
			390: R_ARM_REL32	.LC2
 394:	00000278 	.word	0x00000278
			394: R_ARM_REL32	.LC3
 398:	00000230 	.word	0x00000230
			398: R_ARM_REL32	.LC4
 39c:	00000230 	.word	0x00000230
			39c: R_ARM_REL32	.LC3
 3a0:	a1360009 	.word	0xa1360009
 3a4:	000001d4 	.word	0x000001d4
			3a4: R_ARM_REL32	.LC5
 3a8:	000001d4 	.word	0x000001d4
			3a8: R_ARM_REL32	.LC3
 3ac:	00000170 	.word	0x00000170
			3ac: R_ARM_REL32	.LC6
 3b0:	00000170 	.word	0x00000170
			3b0: R_ARM_REL32	.LC3
 3b4:	00000118 	.word	0x00000118
			3b4: R_ARM_REL32	.LC7
 3b8:	00000118 	.word	0x00000118
			3b8: R_ARM_REL32	.LC3
 3bc:	00000054 	.word	0x00000054
			3bc: R_ARM_REL32	.LC8

Disassembly of section .text.DNVQE_ProcessFrame:

00000000 <DNVQE_ProcessFrame>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e1a07001 	mov	r7, r1
   8:	e24dd044 	sub	sp, sp, #68	; 0x44
   c:	e1a08002 	mov	r8, r2
  10:	e2501000 	subs	r1, r0, #0
  14:	e1a02003 	mov	r2, r3
  18:	e59f31b8 	ldr	r3, [pc, #440]	; 1d8 <DNVQE_ProcessFrame+0x1d8>
  1c:	e58d1004 	str	r1, [sp, #4]
  20:	e08f3003 	add	r3, pc, r3
  24:	1a000009 	bne	50 <DNVQE_ProcessFrame+0x50>
  28:	e59f21ac 	ldr	r2, [pc, #428]	; 1dc <DNVQE_ProcessFrame+0x1dc>
  2c:	e59f01ac 	ldr	r0, [pc, #428]	; 1e0 <DNVQE_ProcessFrame+0x1e0>
  30:	e7933002 	ldr	r3, [r3, r2]
  34:	e08f0000 	add	r0, pc, r0
  38:	e5931000 	ldr	r1, [r3]
  3c:	ebfffffe 	bl	0 <fputs>
			3c: R_ARM_CALL	fputs
  40:	e59f419c 	ldr	r4, [pc, #412]	; 1e4 <DNVQE_ProcessFrame+0x1e4>
  44:	e1a00004 	mov	r0, r4
  48:	e28dd044 	add	sp, sp, #68	; 0x44
  4c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
  50:	e59d1004 	ldr	r1, [sp, #4]
  54:	e3580000 	cmp	r8, #0
  58:	13570000 	cmpne	r7, #0
  5c:	e591943c 	ldr	r9, [r1, #1084]	; 0x43c
  60:	1a000008 	bne	88 <DNVQE_ProcessFrame+0x88>
  64:	e59f2170 	ldr	r2, [pc, #368]	; 1dc <DNVQE_ProcessFrame+0x1dc>
  68:	e59f1178 	ldr	r1, [pc, #376]	; 1e8 <DNVQE_ProcessFrame+0x1e8>
  6c:	e7930002 	ldr	r0, [r3, r2]
  70:	e1a03008 	mov	r3, r8
  74:	e08f1001 	add	r1, pc, r1
  78:	e1a02007 	mov	r2, r7
  7c:	e5900000 	ldr	r0, [r0]
  80:	ebfffffe 	bl	0 <fprintf>
			80: R_ARM_CALL	fprintf
  84:	eaffffed 	b	40 <DNVQE_ProcessFrame+0x40>
  88:	e711f912 	sdiv	r1, r2, r9
  8c:	e0612199 	mls	r1, r9, r1, r2
  90:	e3510000 	cmp	r1, #0
  94:	0a000005 	beq	b0 <DNVQE_ProcessFrame+0xb0>
  98:	e59f113c 	ldr	r1, [pc, #316]	; 1dc <DNVQE_ProcessFrame+0x1dc>
  9c:	e7930001 	ldr	r0, [r3, r1]
  a0:	e1a03009 	mov	r3, r9
  a4:	e59f1140 	ldr	r1, [pc, #320]	; 1ec <DNVQE_ProcessFrame+0x1ec>
  a8:	e08f1001 	add	r1, pc, r1
  ac:	eafffff2 	b	7c <DNVQE_ProcessFrame+0x7c>
  b0:	e59d3004 	ldr	r3, [sp, #4]
  b4:	e59336c8 	ldr	r3, [r3, #1736]	; 0x6c8
  b8:	e58d3008 	str	r3, [sp, #8]
  bc:	e3530000 	cmp	r3, #0
  c0:	0a000028 	beq	168 <DNVQE_ProcessFrame+0x168>
  c4:	e1a03089 	lsl	r3, r9, #1
  c8:	e1a04002 	mov	r4, r2
  cc:	e58d300c 	str	r3, [sp, #12]
  d0:	e59d3004 	ldr	r3, [sp, #4]
  d4:	e2833d9b 	add	r3, r3, #9920	; 0x26c0
  d8:	e283300c 	add	r3, r3, #12
  dc:	e58d3010 	str	r3, [sp, #16]
  e0:	e3540000 	cmp	r4, #0
  e4:	0affffd6 	beq	44 <DNVQE_ProcessFrame+0x44>
  e8:	e3a03014 	mov	r3, #20
  ec:	e28d5018 	add	r5, sp, #24
  f0:	e1a01003 	mov	r1, r3
  f4:	e3a02000 	mov	r2, #0
  f8:	e1a00005 	mov	r0, r5
  fc:	e28d602c 	add	r6, sp, #44	; 0x2c
 100:	e3a0a000 	mov	sl, #0
 104:	ebfffffe 	bl	0 <memset_s>
			104: R_ARM_CALL	memset_s
 108:	e3a03014 	mov	r3, #20
 10c:	e3a02000 	mov	r2, #0
 110:	e1a01003 	mov	r1, r3
 114:	e1a00006 	mov	r0, r6
 118:	ebfffffe 	bl	0 <memset_s>
			118: R_ARM_CALL	memset_s
 11c:	e59d300c 	ldr	r3, [sp, #12]
 120:	e59db004 	ldr	fp, [sp, #4]
 124:	e58d3028 	str	r3, [sp, #40]	; 0x28
 128:	e59d3004 	ldr	r3, [sp, #4]
 12c:	e58d7018 	str	r7, [sp, #24]
 130:	e2833d1b 	add	r3, r3, #1728	; 0x6c0
 134:	e283300c 	add	r3, r3, #12
 138:	e58d302c 	str	r3, [sp, #44]	; 0x2c
 13c:	e59d3008 	ldr	r3, [sp, #8]
 140:	e2433001 	sub	r3, r3, #1
 144:	e58d3014 	str	r3, [sp, #20]
 148:	e59d3008 	ldr	r3, [sp, #8]
 14c:	e15a0003 	cmp	sl, r3
 150:	ba00000b 	blt	184 <DNVQE_ProcessFrame+0x184>
 154:	e59d300c 	ldr	r3, [sp, #12]
 158:	e0444009 	sub	r4, r4, r9
 15c:	e0877003 	add	r7, r7, r3
 160:	e0888003 	add	r8, r8, r3
 164:	eaffffdd 	b	e0 <DNVQE_ProcessFrame+0xe0>
 168:	e1a03082 	lsl	r3, r2, #1
 16c:	e1a00008 	mov	r0, r8
 170:	e1a02007 	mov	r2, r7
 174:	e1a01003 	mov	r1, r3
 178:	ebfffffe 	bl	0 <memcpy_s>
			178: R_ARM_CALL	memcpy_s
 17c:	e59d4008 	ldr	r4, [sp, #8]
 180:	eaffffaf 	b	44 <DNVQE_ProcessFrame+0x44>
 184:	e59d3014 	ldr	r3, [sp, #20]
 188:	e1a02006 	mov	r2, r6
 18c:	e59b0448 	ldr	r0, [fp, #1096]	; 0x448
 190:	e1a01005 	mov	r1, r5
 194:	e15a0003 	cmp	sl, r3
 198:	05868000 	streq	r8, [r6]
 19c:	e59b3480 	ldr	r3, [fp, #1152]	; 0x480
 1a0:	e12fff33 	blx	r3
 1a4:	e3500000 	cmp	r0, #0
 1a8:	1a000008 	bne	1d0 <DNVQE_ProcessFrame+0x1d0>
 1ac:	e35a0000 	cmp	sl, #0
 1b0:	e28bb040 	add	fp, fp, #64	; 0x40
 1b4:	e28aa001 	add	sl, sl, #1
 1b8:	059d3010 	ldreq	r3, [sp, #16]
 1bc:	05853000 	streq	r3, [r5]
 1c0:	e1a03005 	mov	r3, r5
 1c4:	e1a05006 	mov	r5, r6
 1c8:	e1a06003 	mov	r6, r3
 1cc:	eaffffdd 	b	148 <DNVQE_ProcessFrame+0x148>
 1d0:	e59f4018 	ldr	r4, [pc, #24]	; 1f0 <DNVQE_ProcessFrame+0x1f0>
 1d4:	eaffff9a 	b	44 <DNVQE_ProcessFrame+0x44>
 1d8:	000001b0 	.word	0x000001b0
			1d8: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 1dc:	00000000 	.word	0x00000000
			1dc: R_ARM_GOT32	stderr
 1e0:	000001a4 	.word	0x000001a4
			1e0: R_ARM_REL32	.LC9
 1e4:	a1360002 	.word	0xa1360002
 1e8:	0000016c 	.word	0x0000016c
			1e8: R_ARM_REL32	.LC10
 1ec:	0000013c 	.word	0x0000013c
			1ec: R_ARM_REL32	.LC11
 1f0:	a1360004 	.word	0xa1360004
