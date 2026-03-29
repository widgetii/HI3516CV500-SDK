
hi_resample_work.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l       .rodata.RES_ReSampler_Create.str1.1	00000000 .LC0
00000018 l       .rodata.RES_ReSampler_Create.str1.1	00000000 .LC1
0000001c l       .rodata.RES_ReSampler_Create.str1.1	00000000 .LC2
00000021 l       .rodata.RES_ReSampler_Create.str1.1	00000000 .LC3
0000003d l       .rodata.RES_ReSampler_Create.str1.1	00000000 .LC4
00000057 l       .rodata.RES_ReSampler_Create.str1.1	00000000 .LC5
00000076 l       .rodata.RES_ReSampler_Create.str1.1	00000000 .LC6
00000096 l       .rodata.RES_ReSampler_Create.str1.1	00000000 .LC7
00000000 l       .rodata.RES_ReSampler_ProcessFrame.str1.1	00000000 .LC8
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.RES_ReSampler_Create	00000000 .text.RES_ReSampler_Create
00000000 l    d  .text.RES_ReSampler_Destory	00000000 .text.RES_ReSampler_Destory
00000000 l    d  .text.RES_ReSampler_GetInputNum	00000000 .text.RES_ReSampler_GetInputNum
00000000 l    d  .text.RES_ReSampler_ProcessFrame	00000000 .text.RES_ReSampler_ProcessFrame
00000000 l    d  .rodata.RES_ReSampler_Create.str1.1	00000000 .rodata.RES_ReSampler_Create.str1.1
00000000 l    d  .rodata.RES_ReSampler_ProcessFrame.str1.1	00000000 .rodata.RES_ReSampler_ProcessFrame.str1.1
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.RES_ReSampler_Create	000002e4 RES_ReSampler_Create
00000000         *UND*	00000000 malloc
00000000         *UND*	00000000 fputs
00000000         *UND*	00000000 memset_s
00000000         *UND*	00000000 MODULE_HI_Audio_ModuleHandleInit
00000000         *UND*	00000000 free
00000000         *UND*	00000000 fprintf
00000000         *UND*	00000000 MODULE_HI_Audio_ModuleHandleDeInit
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 stderr
00000000 g     F .text.RES_ReSampler_Destory	00000090 RES_ReSampler_Destory
00000000 g     F .text.RES_ReSampler_GetInputNum	00000048 RES_ReSampler_GetInputNum
00000000 g     F .text.RES_ReSampler_ProcessFrame	000004d4 RES_ReSampler_ProcessFrame
00000000         *UND*	00000000 memcpy_s
00000000         *UND*	00000000 memmove_s



Disassembly of section .text.RES_ReSampler_Create:

00000000 <RES_ReSampler_Create>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e24dd01c 	sub	sp, sp, #28
   8:	e59f92a0 	ldr	r9, [pc, #672]	; 2b0 <RES_ReSampler_Create+0x2b0>
   c:	e1510002 	cmp	r1, r2
  10:	e08f9009 	add	r9, pc, r9
  14:	1a000004 	bne	2c <RES_ReSampler_Create+0x2c>
  18:	e3a05000 	mov	r5, #0
  1c:	e5805000 	str	r5, [r0]
  20:	e1a00005 	mov	r0, r5
  24:	e28dd01c 	add	sp, sp, #28
  28:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
  2c:	e241ed7d 	sub	lr, r1, #8000	; 0x1f40
  30:	e30dcac0 	movw	ip, #56000	; 0xdac0
  34:	e15e000c 	cmp	lr, ip
  38:	8afffff6 	bhi	18 <RES_ReSampler_Create+0x18>
  3c:	e242ed7d 	sub	lr, r2, #8000	; 0x1f40
  40:	e309cc40 	movw	ip, #40000	; 0x9c40
  44:	e15e000c 	cmp	lr, ip
  48:	8afffff2 	bhi	18 <RES_ReSampler_Create+0x18>
  4c:	e1a0a000 	mov	sl, r0
  50:	e3060044 	movw	r0, #24644	; 0x6044
  54:	e1a0b003 	mov	fp, r3
  58:	e1a06002 	mov	r6, r2
  5c:	e1a07001 	mov	r7, r1
  60:	ebfffffe 	bl	0 <malloc>
			60: R_ARM_CALL	malloc
  64:	e2504000 	subs	r4, r0, #0
  68:	1a000007 	bne	8c <RES_ReSampler_Create+0x8c>
  6c:	e59f3240 	ldr	r3, [pc, #576]	; 2b4 <RES_ReSampler_Create+0x2b4>
  70:	e59f0240 	ldr	r0, [pc, #576]	; 2b8 <RES_ReSampler_Create+0x2b8>
  74:	e7993003 	ldr	r3, [r9, r3]
  78:	e08f0000 	add	r0, pc, r0
  7c:	e5931000 	ldr	r1, [r3]
  80:	ebfffffe 	bl	0 <fputs>
			80: R_ARM_CALL	fputs
  84:	e59f5230 	ldr	r5, [pc, #560]	; 2bc <RES_ReSampler_Create+0x2bc>
  88:	eaffffe4 	b	20 <RES_ReSampler_Create+0x20>
  8c:	e3063044 	movw	r3, #24644	; 0x6044
  90:	e3a02000 	mov	r2, #0
  94:	e1a01003 	mov	r1, r3
  98:	e2848a06 	add	r8, r4, #24576	; 0x6000
  9c:	e288801c 	add	r8, r8, #28
  a0:	ebfffffe 	bl	0 <memset_s>
			a0: R_ARM_CALL	memset_s
  a4:	e59f2214 	ldr	r2, [pc, #532]	; 2c0 <RES_ReSampler_Create+0x2c0>
  a8:	e3a03001 	mov	r3, #1
  ac:	e59f1210 	ldr	r1, [pc, #528]	; 2c4 <RES_ReSampler_Create+0x2c4>
  b0:	e1a00008 	mov	r0, r8
  b4:	e58d3014 	str	r3, [sp, #20]
  b8:	e3a03000 	mov	r3, #0
  bc:	e08f2002 	add	r2, pc, r2
  c0:	e08f1001 	add	r1, pc, r1
  c4:	e58d3010 	str	r3, [sp, #16]
  c8:	ebfffffe 	bl	0 <MODULE_HI_Audio_ModuleHandleInit>
			c8: R_ARM_CALL	MODULE_HI_Audio_ModuleHandleInit
  cc:	e2505000 	subs	r5, r0, #0
  d0:	0a000008 	beq	f8 <RES_ReSampler_Create+0xf8>
  d4:	e59f31d8 	ldr	r3, [pc, #472]	; 2b4 <RES_ReSampler_Create+0x2b4>
  d8:	e59f01e8 	ldr	r0, [pc, #488]	; 2c8 <RES_ReSampler_Create+0x2c8>
  dc:	e7993003 	ldr	r3, [r9, r3]
  e0:	e08f0000 	add	r0, pc, r0
  e4:	e5931000 	ldr	r1, [r3]
  e8:	ebfffffe 	bl	0 <fputs>
			e8: R_ARM_CALL	fputs
  ec:	e1a00004 	mov	r0, r4
  f0:	ebfffffe 	bl	0 <free>
			f0: R_ARM_CALL	free
  f4:	eaffffc9 	b	20 <RES_ReSampler_Create+0x20>
  f8:	e713f617 	sdiv	r3, r7, r6
  fc:	e0627396 	mls	r2, r6, r3, r7
 100:	e3520000 	cmp	r2, #0
 104:	11a03005 	movne	r3, r5
 108:	e35b0000 	cmp	fp, #0
 10c:	e5843000 	str	r3, [r4]
 110:	1a000030 	bne	1d8 <RES_ReSampler_Create+0x1d8>
 114:	e28d2018 	add	r2, sp, #24
 118:	e284ba06 	add	fp, r4, #24576	; 0x6000
 11c:	e1a01007 	mov	r1, r7
 120:	e284000c 	add	r0, r4, #12
 124:	e59b3038 	ldr	r3, [fp, #56]	; 0x38
 128:	e522600c 	str	r6, [r2, #-12]!
 12c:	e12fff33 	blx	r3
 130:	e594300c 	ldr	r3, [r4, #12]
 134:	e3530000 	cmp	r3, #0
 138:	1a00000c 	bne	170 <RES_ReSampler_Create+0x170>
 13c:	e59f3170 	ldr	r3, [pc, #368]	; 2b4 <RES_ReSampler_Create+0x2b4>
 140:	e3a02061 	mov	r2, #97	; 0x61
 144:	e59f1180 	ldr	r1, [pc, #384]	; 2cc <RES_ReSampler_Create+0x2cc>
 148:	e7993003 	ldr	r3, [r9, r3]
 14c:	e08f1001 	add	r1, pc, r1
 150:	e5930000 	ldr	r0, [r3]
 154:	ebfffffe 	bl	0 <fprintf>
			154: R_ARM_CALL	fprintf
 158:	e59f5170 	ldr	r5, [pc, #368]	; 2d0 <RES_ReSampler_Create+0x2d0>
 15c:	e1a00008 	mov	r0, r8
 160:	ebfffffe 	bl	0 <MODULE_HI_Audio_ModuleHandleDeInit>
			160: R_ARM_CALL	MODULE_HI_Audio_ModuleHandleDeInit
 164:	e1a00004 	mov	r0, r4
 168:	ebfffffe 	bl	0 <free>
			168: R_ARM_CALL	free
 16c:	eaffffab 	b	20 <RES_ReSampler_Create+0x20>
 170:	e5943000 	ldr	r3, [r4]
 174:	e3530000 	cmp	r3, #0
 178:	0a000039 	beq	264 <RES_ReSampler_Create+0x264>
 17c:	e2833001 	add	r3, r3, #1
 180:	e1a03083 	lsl	r3, r3, #1
 184:	e58d3004 	str	r3, [sp, #4]
 188:	e1a00003 	mov	r0, r3
 18c:	ebfffffe 	bl	0 <malloc>
			18c: R_ARM_CALL	malloc
 190:	e5840018 	str	r0, [r4, #24]
 194:	e3500000 	cmp	r0, #0
 198:	e59d3004 	ldr	r3, [sp, #4]
 19c:	1a00002d 	bne	258 <RES_ReSampler_Create+0x258>
 1a0:	e59f310c 	ldr	r3, [pc, #268]	; 2b4 <RES_ReSampler_Create+0x2b4>
 1a4:	e59f0128 	ldr	r0, [pc, #296]	; 2d4 <RES_ReSampler_Create+0x2d4>
 1a8:	e7993003 	ldr	r3, [r9, r3]
 1ac:	e08f0000 	add	r0, pc, r0
 1b0:	e5931000 	ldr	r1, [r3]
 1b4:	ebfffffe 	bl	0 <fputs>
			1b4: R_ARM_CALL	fputs
 1b8:	e59b3040 	ldr	r3, [fp, #64]	; 0x40
 1bc:	e594000c 	ldr	r0, [r4, #12]
 1c0:	e12fff33 	blx	r3
 1c4:	e1a00008 	mov	r0, r8
 1c8:	ebfffffe 	bl	0 <MODULE_HI_Audio_ModuleHandleDeInit>
			1c8: R_ARM_CALL	MODULE_HI_Audio_ModuleHandleDeInit
 1cc:	e1a00004 	mov	r0, r4
 1d0:	ebfffffe 	bl	0 <free>
			1d0: R_ARM_CALL	free
 1d4:	eaffffaa 	b	84 <RES_ReSampler_Create+0x84>
 1d8:	e35b0001 	cmp	fp, #1
 1dc:	1a000026 	bne	27c <RES_ReSampler_Create+0x27c>
 1e0:	e28d2018 	add	r2, sp, #24
 1e4:	e284ba06 	add	fp, r4, #24576	; 0x6000
 1e8:	e1a01007 	mov	r1, r7
 1ec:	e2840010 	add	r0, r4, #16
 1f0:	e59b3038 	ldr	r3, [fp, #56]	; 0x38
 1f4:	e522600c 	str	r6, [r2, #-12]!
 1f8:	e12fff33 	blx	r3
 1fc:	e5943010 	ldr	r3, [r4, #16]
 200:	e3530000 	cmp	r3, #0
 204:	0affffd3 	beq	158 <RES_ReSampler_Create+0x158>
 208:	e3a03064 	mov	r3, #100	; 0x64
 20c:	e713f316 	sdiv	r3, r6, r3
 210:	e2833001 	add	r3, r3, #1
 214:	e1a03083 	lsl	r3, r3, #1
 218:	e58d3004 	str	r3, [sp, #4]
 21c:	e1a00003 	mov	r0, r3
 220:	ebfffffe 	bl	0 <malloc>
			220: R_ARM_CALL	malloc
 224:	e5840018 	str	r0, [r4, #24]
 228:	e3500000 	cmp	r0, #0
 22c:	e59d3004 	ldr	r3, [sp, #4]
 230:	1a000008 	bne	258 <RES_ReSampler_Create+0x258>
 234:	e59f3078 	ldr	r3, [pc, #120]	; 2b4 <RES_ReSampler_Create+0x2b4>
 238:	e59f0098 	ldr	r0, [pc, #152]	; 2d8 <RES_ReSampler_Create+0x2d8>
 23c:	e7993003 	ldr	r3, [r9, r3]
 240:	e08f0000 	add	r0, pc, r0
 244:	e5931000 	ldr	r1, [r3]
 248:	ebfffffe 	bl	0 <fputs>
			248: R_ARM_CALL	fputs
 24c:	e59b3040 	ldr	r3, [fp, #64]	; 0x40
 250:	e5940010 	ldr	r0, [r4, #16]
 254:	eaffffd9 	b	1c0 <RES_ReSampler_Create+0x1c0>
 258:	e1a02005 	mov	r2, r5
 25c:	e1a01003 	mov	r1, r3
 260:	ebfffffe 	bl	0 <memset_s>
			260: R_ARM_CALL	memset_s
 264:	e5847004 	str	r7, [r4, #4]
 268:	e3a03000 	mov	r3, #0
 26c:	e5843014 	str	r3, [r4, #20]
 270:	e5846008 	str	r6, [r4, #8]
 274:	e58a4000 	str	r4, [sl]
 278:	eaffff68 	b	20 <RES_ReSampler_Create+0x20>
 27c:	e59f3030 	ldr	r3, [pc, #48]	; 2b4 <RES_ReSampler_Create+0x2b4>
 280:	e1a0200b 	mov	r2, fp
 284:	e59f1050 	ldr	r1, [pc, #80]	; 2dc <RES_ReSampler_Create+0x2dc>
 288:	e7993003 	ldr	r3, [r9, r3]
 28c:	e08f1001 	add	r1, pc, r1
 290:	e5930000 	ldr	r0, [r3]
 294:	e59f5044 	ldr	r5, [pc, #68]	; 2e0 <RES_ReSampler_Create+0x2e0>
 298:	ebfffffe 	bl	0 <fprintf>
			298: R_ARM_CALL	fprintf
 29c:	e1a00008 	mov	r0, r8
 2a0:	ebfffffe 	bl	0 <MODULE_HI_Audio_ModuleHandleDeInit>
			2a0: R_ARM_CALL	MODULE_HI_Audio_ModuleHandleDeInit
 2a4:	e1a00004 	mov	r0, r4
 2a8:	ebfffffe 	bl	0 <free>
			2a8: R_ARM_CALL	free
 2ac:	eaffff5b 	b	20 <RES_ReSampler_Create+0x20>
 2b0:	00000298 	.word	0x00000298
			2b0: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 2b4:	00000000 	.word	0x00000000
			2b4: R_ARM_GOT32	stderr
 2b8:	00000238 	.word	0x00000238
			2b8: R_ARM_REL32	.LC0
 2bc:	a3340003 	.word	0xa3340003
 2c0:	000001fc 	.word	0x000001fc
			2c0: R_ARM_REL32	.LC1
 2c4:	000001fc 	.word	0x000001fc
			2c4: R_ARM_REL32	.LC2
 2c8:	000001e0 	.word	0x000001e0
			2c8: R_ARM_REL32	.LC3
 2cc:	00000178 	.word	0x00000178
			2cc: R_ARM_REL32	.LC4
 2d0:	a3340004 	.word	0xa3340004
 2d4:	00000120 	.word	0x00000120
			2d4: R_ARM_REL32	.LC5
 2d8:	00000090 	.word	0x00000090
			2d8: R_ARM_REL32	.LC6
 2dc:	00000048 	.word	0x00000048
			2dc: R_ARM_REL32	.LC7
 2e0:	a3340002 	.word	0xa3340002

Disassembly of section .text.RES_ReSampler_Destory:

00000000 <RES_ReSampler_Destory>:
   0:	e92d4010 	push	{r4, lr}
   4:	e2504000 	subs	r4, r0, #0
   8:	0a00001e 	beq	88 <RES_ReSampler_Destory+0x88>
   c:	e594000c 	ldr	r0, [r4, #12]
  10:	e3500000 	cmp	r0, #0
  14:	0a000008 	beq	3c <RES_ReSampler_Destory+0x3c>
  18:	e2843a06 	add	r3, r4, #24576	; 0x6000
  1c:	e5933040 	ldr	r3, [r3, #64]	; 0x40
  20:	e12fff33 	blx	r3
  24:	e5940018 	ldr	r0, [r4, #24]
  28:	e3500000 	cmp	r0, #0
  2c:	0a000000 	beq	34 <RES_ReSampler_Destory+0x34>
  30:	ebfffffe 	bl	0 <free>
			30: R_ARM_CALL	free
  34:	e3a03000 	mov	r3, #0
  38:	e5843018 	str	r3, [r4, #24]
  3c:	e5940010 	ldr	r0, [r4, #16]
  40:	e3500000 	cmp	r0, #0
  44:	0a000008 	beq	6c <RES_ReSampler_Destory+0x6c>
  48:	e2843a06 	add	r3, r4, #24576	; 0x6000
  4c:	e5933040 	ldr	r3, [r3, #64]	; 0x40
  50:	e12fff33 	blx	r3
  54:	e5940018 	ldr	r0, [r4, #24]
  58:	e3500000 	cmp	r0, #0
  5c:	0a000000 	beq	64 <RES_ReSampler_Destory+0x64>
  60:	ebfffffe 	bl	0 <free>
			60: R_ARM_CALL	free
  64:	e3a03000 	mov	r3, #0
  68:	e5843018 	str	r3, [r4, #24]
  6c:	e2840a06 	add	r0, r4, #24576	; 0x6000
  70:	e280001c 	add	r0, r0, #28
  74:	ebfffffe 	bl	0 <MODULE_HI_Audio_ModuleHandleDeInit>
			74: R_ARM_CALL	MODULE_HI_Audio_ModuleHandleDeInit
  78:	e1a00004 	mov	r0, r4
  7c:	ebfffffe 	bl	0 <free>
			7c: R_ARM_CALL	free
  80:	e3a00000 	mov	r0, #0
  84:	e8bd8010 	pop	{r4, pc}
  88:	e3e00000 	mvn	r0, #0
  8c:	e8bd8010 	pop	{r4, pc}

Disassembly of section .text.RES_ReSampler_GetInputNum:

00000000 <RES_ReSampler_GetInputNum>:
   0:	e2503000 	subs	r3, r0, #0
   4:	0a00000b 	beq	38 <RES_ReSampler_GetInputNum+0x38>
   8:	e3520001 	cmp	r2, #1
   c:	1a00000b 	bne	40 <RES_ReSampler_GetInputNum+0x40>
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

Disassembly of section .text.RES_ReSampler_ProcessFrame:

00000000 <RES_ReSampler_ProcessFrame>:
   0:	e3520000 	cmp	r2, #0
   4:	13510000 	cmpne	r1, #0
   8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   c:	03a08001 	moveq	r8, #1
  10:	e59fa49c 	ldr	sl, [pc, #1180]	; 4b4 <RES_ReSampler_ProcessFrame+0x4b4>
  14:	13a08000 	movne	r8, #0
  18:	e24dd04c 	sub	sp, sp, #76	; 0x4c
  1c:	e3500000 	cmp	r0, #0
  20:	03888001 	orreq	r8, r8, #1
  24:	e08fa00a 	add	sl, pc, sl
  28:	e3580000 	cmp	r8, #0
  2c:	e59db070 	ldr	fp, [sp, #112]	; 0x70
  30:	13e06000 	mvnne	r6, #0
  34:	e59d9074 	ldr	r9, [sp, #116]	; 0x74
  38:	1a00007e 	bne	238 <RES_ReSampler_ProcessFrame+0x238>
  3c:	e1a06003 	mov	r6, r3
  40:	e28d3020 	add	r3, sp, #32
  44:	e58d300c 	str	r3, [sp, #12]
  48:	e3a03014 	mov	r3, #20
  4c:	e1a07001 	mov	r7, r1
  50:	e1a05002 	mov	r5, r2
  54:	e1a01003 	mov	r1, r3
  58:	e1a02008 	mov	r2, r8
  5c:	e1a04000 	mov	r4, r0
  60:	e59d000c 	ldr	r0, [sp, #12]
  64:	ebfffffe 	bl	0 <memset_s>
			64: R_ARM_CALL	memset_s
  68:	e28d3034 	add	r3, sp, #52	; 0x34
  6c:	e1a02008 	mov	r2, r8
  70:	e58d3010 	str	r3, [sp, #16]
  74:	e3a03014 	mov	r3, #20
  78:	e59d0010 	ldr	r0, [sp, #16]
  7c:	e1a01003 	mov	r1, r3
  80:	ebfffffe 	bl	0 <memset_s>
			80: R_ARM_CALL	memset_s
  84:	e3590000 	cmp	r9, #0
  88:	1a000036 	bne	168 <RES_ReSampler_ProcessFrame+0x168>
  8c:	e5942000 	ldr	r2, [r4]
  90:	e3520000 	cmp	r2, #0
  94:	0a000026 	beq	134 <RES_ReSampler_ProcessFrame+0x134>
  98:	e713f216 	sdiv	r3, r6, r2
  9c:	e0636392 	mls	r3, r2, r3, r6
  a0:	e3530000 	cmp	r3, #0
  a4:	e5943014 	ldr	r3, [r4, #20]
  a8:	1a000001 	bne	b4 <RES_ReSampler_ProcessFrame+0xb4>
  ac:	e3530000 	cmp	r3, #0
  b0:	da00001f 	ble	134 <RES_ReSampler_ProcessFrame+0x134>
  b4:	e3530000 	cmp	r3, #0
  b8:	da000004 	ble	d0 <RES_ReSampler_ProcessFrame+0xd0>
  bc:	e1a03083 	lsl	r3, r3, #1
  c0:	e284001c 	add	r0, r4, #28
  c4:	e5942018 	ldr	r2, [r4, #24]
  c8:	e1a01003 	mov	r1, r3
  cc:	ebfffffe 	bl	0 <memcpy_s>
			cc: R_ARM_CALL	memcpy_s
  d0:	e5940014 	ldr	r0, [r4, #20]
  d4:	e1a0a004 	mov	sl, r4
  d8:	e0862000 	add	r2, r6, r0
  dc:	e49a301c 	ldr	r3, [sl], #28
  e0:	e718f312 	sdiv	r8, r2, r3
  e4:	e0682893 	mls	r8, r3, r8, r2
  e8:	e08a0080 	add	r0, sl, r0, lsl #1
  ec:	e1a02007 	mov	r2, r7
  f0:	e0469008 	sub	r9, r6, r8
  f4:	e1a09089 	lsl	r9, r9, #1
  f8:	e1a03009 	mov	r3, r9
  fc:	e1a01009 	mov	r1, r9
 100:	ebfffffe 	bl	0 <memcpy_s>
			100: R_ARM_CALL	memcpy_s
 104:	e3580000 	cmp	r8, #0
 108:	0a000004 	beq	120 <RES_ReSampler_ProcessFrame+0x120>
 10c:	e1a03088 	lsl	r3, r8, #1
 110:	e0872009 	add	r2, r7, r9
 114:	e1a01003 	mov	r1, r3
 118:	e5940018 	ldr	r0, [r4, #24]
 11c:	ebfffffe 	bl	0 <memcpy_s>
			11c: R_ARM_CALL	memcpy_s
 120:	e5947014 	ldr	r7, [r4, #20]
 124:	e5848014 	str	r8, [r4, #20]
 128:	e0866007 	add	r6, r6, r7
 12c:	e1a0700a 	mov	r7, sl
 130:	e0466008 	sub	r6, r6, r8
 134:	e58d7020 	str	r7, [sp, #32]
 138:	e2843a06 	add	r3, r4, #24576	; 0x6000
 13c:	e1a06086 	lsl	r6, r6, #1
 140:	e58d6030 	str	r6, [sp, #48]	; 0x30
 144:	e58d5034 	str	r5, [sp, #52]	; 0x34
 148:	e593303c 	ldr	r3, [r3, #60]	; 0x3c
 14c:	e59d2010 	ldr	r2, [sp, #16]
 150:	e59d100c 	ldr	r1, [sp, #12]
 154:	e594000c 	ldr	r0, [r4, #12]
 158:	e12fff33 	blx	r3
 15c:	e58b0000 	str	r0, [fp]
 160:	e3a06000 	mov	r6, #0
 164:	ea000033 	b	238 <RES_ReSampler_ProcessFrame+0x238>
 168:	e3590001 	cmp	r9, #1
 16c:	1a00006a 	bne	31c <RES_ReSampler_ProcessFrame+0x31c>
 170:	e5943000 	ldr	r3, [r4]
 174:	e59b8000 	ldr	r8, [fp]
 178:	e594b010 	ldr	fp, [r4, #16]
 17c:	e3530000 	cmp	r3, #0
 180:	1a000001 	bne	18c <RES_ReSampler_ProcessFrame+0x18c>
 184:	e3560000 	cmp	r6, #0
 188:	12466001 	subne	r6, r6, #1
 18c:	e1a03086 	lsl	r3, r6, #1
 190:	e58d3014 	str	r3, [sp, #20]
 194:	e58d3030 	str	r3, [sp, #48]	; 0x30
 198:	e5943014 	ldr	r3, [r4, #20]
 19c:	e58d7020 	str	r7, [sp, #32]
 1a0:	e58d5034 	str	r5, [sp, #52]	; 0x34
 1a4:	e3530000 	cmp	r3, #0
 1a8:	da00000d 	ble	1e4 <RES_ReSampler_ProcessFrame+0x1e4>
 1ac:	e1a03083 	lsl	r3, r3, #1
 1b0:	e1a00005 	mov	r0, r5
 1b4:	e5942018 	ldr	r2, [r4, #24]
 1b8:	e1a01003 	mov	r1, r3
 1bc:	ebfffffe 	bl	0 <memcpy_s>
			1bc: R_ARM_CALL	memcpy_s
 1c0:	e5943014 	ldr	r3, [r4, #20]
 1c4:	e59d2034 	ldr	r2, [sp, #52]	; 0x34
 1c8:	e0488003 	sub	r8, r8, r3
 1cc:	e1a03083 	lsl	r3, r3, #1
 1d0:	e0822003 	add	r2, r2, r3
 1d4:	e0855003 	add	r5, r5, r3
 1d8:	e3a03000 	mov	r3, #0
 1dc:	e58d2034 	str	r2, [sp, #52]	; 0x34
 1e0:	e5843014 	str	r3, [r4, #20]
 1e4:	e59d2010 	ldr	r2, [sp, #16]
 1e8:	e2843a06 	add	r3, r4, #24576	; 0x6000
 1ec:	e59d100c 	ldr	r1, [sp, #12]
 1f0:	e1a0000b 	mov	r0, fp
 1f4:	e593603c 	ldr	r6, [r3, #60]	; 0x3c
 1f8:	e58d3018 	str	r3, [sp, #24]
 1fc:	e12fff36 	blx	r6
 200:	e1580000 	cmp	r8, r0
 204:	e1a09000 	mov	r9, r0
 208:	e59d3018 	ldr	r3, [sp, #24]
 20c:	aa00000c 	bge	244 <RES_ReSampler_ProcessFrame+0x244>
 210:	e59f32a0 	ldr	r3, [pc, #672]	; 4b8 <RES_ReSampler_ProcessFrame+0x4b8>
 214:	e300213e 	movw	r2, #318	; 0x13e
 218:	e59f129c 	ldr	r1, [pc, #668]	; 4bc <RES_ReSampler_ProcessFrame+0x4bc>
 21c:	e79a0003 	ldr	r0, [sl, r3]
 220:	e1a03009 	mov	r3, r9
 224:	e08f1001 	add	r1, pc, r1
 228:	e58d8000 	str	r8, [sp]
 22c:	e5900000 	ldr	r0, [r0]
 230:	e59f6288 	ldr	r6, [pc, #648]	; 4c0 <RES_ReSampler_ProcessFrame+0x4c0>
 234:	ebfffffe 	bl	0 <fprintf>
			234: R_ARM_CALL	fprintf
 238:	e1a00006 	mov	r0, r6
 23c:	e28dd04c 	add	sp, sp, #76	; 0x4c
 240:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 244:	e5946000 	ldr	r6, [r4]
 248:	e3560000 	cmp	r6, #0
 24c:	0a000009 	beq	278 <RES_ReSampler_ProcessFrame+0x278>
 250:	e1580000 	cmp	r8, r0
 254:	daffffc1 	ble	160 <RES_ReSampler_ProcessFrame+0x160>
 258:	e59f3258 	ldr	r3, [pc, #600]	; 4b8 <RES_ReSampler_ProcessFrame+0x4b8>
 25c:	e3002149 	movw	r2, #329	; 0x149
 260:	e59f125c 	ldr	r1, [pc, #604]	; 4c4 <RES_ReSampler_ProcessFrame+0x4c4>
 264:	e79a0003 	ldr	r0, [sl, r3]
 268:	e1a03009 	mov	r3, r9
 26c:	e58d8000 	str	r8, [sp]
 270:	e08f1001 	add	r1, pc, r1
 274:	eaffffec 	b	22c <RES_ReSampler_ProcessFrame+0x22c>
 278:	e59d2014 	ldr	r2, [sp, #20]
 27c:	e1a0000b 	mov	r0, fp
 280:	e593303c 	ldr	r3, [r3, #60]	; 0x3c
 284:	e0877002 	add	r7, r7, r2
 288:	e3a02002 	mov	r2, #2
 28c:	e58d2030 	str	r2, [sp, #48]	; 0x30
 290:	e5942018 	ldr	r2, [r4, #24]
 294:	e58d7020 	str	r7, [sp, #32]
 298:	e58d2034 	str	r2, [sp, #52]	; 0x34
 29c:	e59d100c 	ldr	r1, [sp, #12]
 2a0:	e59d2010 	ldr	r2, [sp, #16]
 2a4:	e12fff33 	blx	r3
 2a8:	e1580009 	cmp	r8, r9
 2ac:	d5840014 	strle	r0, [r4, #20]
 2b0:	daffffaa 	ble	160 <RES_ReSampler_ProcessFrame+0x160>
 2b4:	e0487009 	sub	r7, r8, r9
 2b8:	e0400007 	sub	r0, r0, r7
 2bc:	e5840014 	str	r0, [r4, #20]
 2c0:	e3500000 	cmp	r0, #0
 2c4:	aa000007 	bge	2e8 <RES_ReSampler_ProcessFrame+0x2e8>
 2c8:	e59f31e8 	ldr	r3, [pc, #488]	; 4b8 <RES_ReSampler_ProcessFrame+0x4b8>
 2cc:	e300215a 	movw	r2, #346	; 0x15a
 2d0:	e59f11f0 	ldr	r1, [pc, #496]	; 4c8 <RES_ReSampler_ProcessFrame+0x4c8>
 2d4:	e79a0003 	ldr	r0, [sl, r3]
 2d8:	e1a03009 	mov	r3, r9
 2dc:	e58d8000 	str	r8, [sp]
 2e0:	e08f1001 	add	r1, pc, r1
 2e4:	eaffffd0 	b	22c <RES_ReSampler_ProcessFrame+0x22c>
 2e8:	e1a07087 	lsl	r7, r7, #1
 2ec:	e5942018 	ldr	r2, [r4, #24]
 2f0:	e1a03007 	mov	r3, r7
 2f4:	e0850089 	add	r0, r5, r9, lsl #1
 2f8:	e1a01007 	mov	r1, r7
 2fc:	ebfffffe 	bl	0 <memcpy_s>
			2fc: R_ARM_CALL	memcpy_s
 300:	e5940018 	ldr	r0, [r4, #24]
 304:	e5943014 	ldr	r3, [r4, #20]
 308:	e0802007 	add	r2, r0, r7
 30c:	e1a03083 	lsl	r3, r3, #1
 310:	e1a01003 	mov	r1, r3
 314:	ebfffffe 	bl	0 <memmove_s>
			314: R_ARM_CALL	memmove_s
 318:	eaffffc6 	b	238 <RES_ReSampler_ProcessFrame+0x238>
 31c:	e3590002 	cmp	r9, #2
 320:	159f61a4 	ldrne	r6, [pc, #420]	; 4cc <RES_ReSampler_ProcessFrame+0x4cc>
 324:	1affffc3 	bne	238 <RES_ReSampler_ProcessFrame+0x238>
 328:	e5943010 	ldr	r3, [r4, #16]
 32c:	e59b8000 	ldr	r8, [fp]
 330:	e58d3014 	str	r3, [sp, #20]
 334:	e5943000 	ldr	r3, [r4]
 338:	e3530000 	cmp	r3, #0
 33c:	1a000001 	bne	348 <RES_ReSampler_ProcessFrame+0x348>
 340:	e3560000 	cmp	r6, #0
 344:	12466001 	subne	r6, r6, #1
 348:	e1a03086 	lsl	r3, r6, #1
 34c:	e58d3018 	str	r3, [sp, #24]
 350:	e58d3030 	str	r3, [sp, #48]	; 0x30
 354:	e5943014 	ldr	r3, [r4, #20]
 358:	e58d7020 	str	r7, [sp, #32]
 35c:	e58d5034 	str	r5, [sp, #52]	; 0x34
 360:	e3530000 	cmp	r3, #0
 364:	da00000d 	ble	3a0 <RES_ReSampler_ProcessFrame+0x3a0>
 368:	e1a03083 	lsl	r3, r3, #1
 36c:	e1a00005 	mov	r0, r5
 370:	e5942018 	ldr	r2, [r4, #24]
 374:	e1a01003 	mov	r1, r3
 378:	ebfffffe 	bl	0 <memcpy_s>
			378: R_ARM_CALL	memcpy_s
 37c:	e5943014 	ldr	r3, [r4, #20]
 380:	e59d2034 	ldr	r2, [sp, #52]	; 0x34
 384:	e0488003 	sub	r8, r8, r3
 388:	e1a03083 	lsl	r3, r3, #1
 38c:	e0822003 	add	r2, r2, r3
 390:	e0855003 	add	r5, r5, r3
 394:	e3a03000 	mov	r3, #0
 398:	e58d2034 	str	r2, [sp, #52]	; 0x34
 39c:	e5843014 	str	r3, [r4, #20]
 3a0:	e59d2010 	ldr	r2, [sp, #16]
 3a4:	e2843a06 	add	r3, r4, #24576	; 0x6000
 3a8:	e58d301c 	str	r3, [sp, #28]
 3ac:	e59d100c 	ldr	r1, [sp, #12]
 3b0:	e593303c 	ldr	r3, [r3, #60]	; 0x3c
 3b4:	e59d0014 	ldr	r0, [sp, #20]
 3b8:	e12fff33 	blx	r3
 3bc:	e1580000 	cmp	r8, r0
 3c0:	e1a09000 	mov	r9, r0
 3c4:	aa000007 	bge	3e8 <RES_ReSampler_ProcessFrame+0x3e8>
 3c8:	e59f30e8 	ldr	r3, [pc, #232]	; 4b8 <RES_ReSampler_ProcessFrame+0x4b8>
 3cc:	e3002185 	movw	r2, #389	; 0x185
 3d0:	e59f10f8 	ldr	r1, [pc, #248]	; 4d0 <RES_ReSampler_ProcessFrame+0x4d0>
 3d4:	e79a0003 	ldr	r0, [sl, r3]
 3d8:	e1a03009 	mov	r3, r9
 3dc:	e58d8000 	str	r8, [sp]
 3e0:	e08f1001 	add	r1, pc, r1
 3e4:	eaffff90 	b	22c <RES_ReSampler_ProcessFrame+0x22c>
 3e8:	e5946000 	ldr	r6, [r4]
 3ec:	e3560000 	cmp	r6, #0
 3f0:	0a000005 	beq	40c <RES_ReSampler_ProcessFrame+0x40c>
 3f4:	e1580000 	cmp	r8, r0
 3f8:	c0489000 	subgt	r9, r8, r0
 3fc:	c59b8000 	ldrgt	r8, [fp]
 400:	c0488009 	subgt	r8, r8, r9
 404:	c58b8000 	strgt	r8, [fp]
 408:	eaffff54 	b	160 <RES_ReSampler_ProcessFrame+0x160>
 40c:	e59d3018 	ldr	r3, [sp, #24]
 410:	e59d2010 	ldr	r2, [sp, #16]
 414:	e0877003 	add	r7, r7, r3
 418:	e3a03002 	mov	r3, #2
 41c:	e58d3030 	str	r3, [sp, #48]	; 0x30
 420:	e5943018 	ldr	r3, [r4, #24]
 424:	e58d7020 	str	r7, [sp, #32]
 428:	e58d3034 	str	r3, [sp, #52]	; 0x34
 42c:	e2843a06 	add	r3, r4, #24576	; 0x6000
 430:	e59d100c 	ldr	r1, [sp, #12]
 434:	e593303c 	ldr	r3, [r3, #60]	; 0x3c
 438:	e59d0014 	ldr	r0, [sp, #20]
 43c:	e12fff33 	blx	r3
 440:	e1580009 	cmp	r8, r9
 444:	e1a03000 	mov	r3, r0
 448:	e5840014 	str	r0, [r4, #20]
 44c:	daffff43 	ble	160 <RES_ReSampler_ProcessFrame+0x160>
 450:	e0488009 	sub	r8, r8, r9
 454:	e1530008 	cmp	r3, r8
 458:	e0850089 	add	r0, r5, r9, lsl #1
 45c:	aa000008 	bge	484 <RES_ReSampler_ProcessFrame+0x484>
 460:	e1a03083 	lsl	r3, r3, #1
 464:	e5942018 	ldr	r2, [r4, #24]
 468:	e1a01003 	mov	r1, r3
 46c:	ebfffffe 	bl	0 <memcpy_s>
			46c: R_ARM_CALL	memcpy_s
 470:	e5948014 	ldr	r8, [r4, #20]
 474:	e0889009 	add	r9, r8, r9
 478:	e58b9000 	str	r9, [fp]
 47c:	e5846014 	str	r6, [r4, #20]
 480:	eaffff6c 	b	238 <RES_ReSampler_ProcessFrame+0x238>
 484:	e0433008 	sub	r3, r3, r8
 488:	e1a08088 	lsl	r8, r8, #1
 48c:	e5843014 	str	r3, [r4, #20]
 490:	e1a01008 	mov	r1, r8
 494:	e5942018 	ldr	r2, [r4, #24]
 498:	e1a03008 	mov	r3, r8
 49c:	ebfffffe 	bl	0 <memcpy_s>
			49c: R_ARM_CALL	memcpy_s
 4a0:	e5940018 	ldr	r0, [r4, #24]
 4a4:	e5943014 	ldr	r3, [r4, #20]
 4a8:	e0802008 	add	r2, r0, r8
 4ac:	e1a03083 	lsl	r3, r3, #1
 4b0:	eaffff96 	b	310 <RES_ReSampler_ProcessFrame+0x310>
 4b4:	00000488 	.word	0x00000488
			4b4: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 4b8:	00000000 	.word	0x00000000
			4b8: R_ARM_GOT32	stderr
 4bc:	00000290 	.word	0x00000290
			4bc: R_ARM_REL32	.LC8
 4c0:	a3340004 	.word	0xa3340004
 4c4:	0000024c 	.word	0x0000024c
			4c4: R_ARM_REL32	.LC8
 4c8:	000001e0 	.word	0x000001e0
			4c8: R_ARM_REL32	.LC8
 4cc:	a3340002 	.word	0xa3340002
 4d0:	000000e8 	.word	0x000000e8
			4d0: R_ARM_REL32	.LC8
