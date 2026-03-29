
check_param.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l    df *ABS*	00000000 check_param.c
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.md_check_attr	00000000 .text.md_check_attr
00000220 l       .rodata.md_check_attr.str1.4	00000000 .LC7
00000000 l       .rodata.md_check_attr.str1.4	00000000 .LC0
00000284 l       .rodata.md_check_attr.str1.4	00000000 .LC8
00000314 l       .rodata.md_check_attr.str1.4	00000000 .LC10
00000048 l       .rodata.md_check_attr.str1.4	00000000 .LC1
0000008c l       .rodata.md_check_attr.str1.4	00000000 .LC2
000000d4 l       .rodata.md_check_attr.str1.4	00000000 .LC3
0000011c l       .rodata.md_check_attr.str1.4	00000000 .LC4
0000018c l       .rodata.md_check_attr.str1.4	00000000 .LC5
000001d4 l       .rodata.md_check_attr.str1.4	00000000 .LC6
000002d0 l       .rodata.md_check_attr.str1.4	00000000 .LC9
00000378 l       .rodata.md_check_attr.str1.4	00000000 .LC11
00000000 l    d  .rodata.__FUNCTION__.8990	00000000 .rodata.__FUNCTION__.8990
00000000 l     O .rodata.__FUNCTION__.8990	0000000e __FUNCTION__.8990
00000000 l    d  .rodata.md_check_attr.str1.4	00000000 .rodata.md_check_attr.str1.4
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.md_check_attr	000003f4 md_check_attr
00000000         *UND*	00000000 fprintf
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 stderr



Disassembly of section .text.md_check_attr:

00000000 <md_check_attr>:
   0:	e5901000 	ldr	r1, [r0]
   4:	e92d4070 	push	{r4, r5, r6, lr}
   8:	e3510001 	cmp	r1, #1
   c:	e59f5378 	ldr	r5, [pc, #888]	; 38c <md_check_attr+0x38c>
  10:	e24dd010 	sub	sp, sp, #16
  14:	e08f5005 	add	r5, pc, r5
  18:	8a00003b 	bhi	10c <md_check_attr+0x10c>
  1c:	e590600c 	ldr	r6, [r0, #12]
  20:	e2463040 	sub	r3, r6, #64	; 0x40
  24:	e3530d1d 	cmp	r3, #1856	; 0x740
  28:	8a000066 	bhi	1c8 <md_check_attr+0x1c8>
  2c:	e590c010 	ldr	ip, [r0, #16]
  30:	e24c3040 	sub	r3, ip, #64	; 0x40
  34:	e3530ffe 	cmp	r3, #1016	; 0x3f8
  38:	8a000072 	bhi	208 <md_check_attr+0x208>
  3c:	e5901004 	ldr	r1, [r0, #4]
  40:	e3510002 	cmp	r1, #2
  44:	8a00007f 	bhi	248 <md_check_attr+0x248>
  48:	e590e008 	ldr	lr, [r0, #8]
  4c:	e35e0004 	cmp	lr, #4
  50:	135e0001 	cmpne	lr, #1
  54:	8a00008b 	bhi	288 <md_check_attr+0x288>
  58:	e3a04004 	mov	r4, #4
  5c:	e1a04114 	lsl	r4, r4, r1
  60:	e2443001 	sub	r3, r4, #1
  64:	e1160003 	tst	r6, r3
  68:	1a000093 	bne	2bc <md_check_attr+0x2bc>
  6c:	e01c1003 	ands	r1, ip, r3
  70:	1a00009f 	bne	2f4 <md_check_attr+0x2f4>
  74:	e35e0001 	cmp	lr, #1
  78:	0a00000f 	beq	bc <md_check_attr+0xbc>
  7c:	e5901018 	ldr	r1, [r0, #24]
  80:	e3510001 	cmp	r1, #1
  84:	8a000030 	bhi	14c <md_check_attr+0x14c>
  88:	e1d031be 	ldrh	r3, [r0, #30]
  8c:	e3530000 	cmp	r3, #0
  90:	0a0000a5 	beq	32c <md_check_attr+0x32c>
  94:	e1d0c2b0 	ldrh	ip, [r0, #32]
  98:	e1d0e2b2 	ldrh	lr, [r0, #34]	; 0x22
  9c:	e08c300e 	add	r3, ip, lr
  a0:	e3530801 	cmp	r3, #65536	; 0x10000
  a4:	1a000038 	bne	18c <md_check_attr+0x18c>
  a8:	e35c0000 	cmp	ip, #0
  ac:	13a00000 	movne	r0, #0
  b0:	0a0000a9 	beq	35c <md_check_attr+0x35c>
  b4:	e28dd010 	add	sp, sp, #16
  b8:	e8bd8070 	pop	{r4, r5, r6, pc}
  bc:	e1d0c1b4 	ldrh	ip, [r0, #20]
  c0:	e35c00ff 	cmp	ip, #255	; 0xff
  c4:	9affffec 	bls	7c <md_check_attr+0x7c>
  c8:	e59f02c0 	ldr	r0, [pc, #704]	; 390 <md_check_attr+0x390>
  cc:	e3a040ff 	mov	r4, #255	; 0xff
  d0:	e59f22bc 	ldr	r2, [pc, #700]	; 394 <md_check_attr+0x394>
  d4:	e3a03035 	mov	r3, #53	; 0x35
  d8:	e7950000 	ldr	r0, [r5, r0]
  dc:	e58de00c 	str	lr, [sp, #12]
  e0:	e5900000 	ldr	r0, [r0]
  e4:	e58d1004 	str	r1, [sp, #4]
  e8:	e59f12a8 	ldr	r1, [pc, #680]	; 398 <md_check_attr+0x398>
  ec:	e08f2002 	add	r2, pc, r2
  f0:	e58dc000 	str	ip, [sp]
  f4:	e58d4008 	str	r4, [sp, #8]
  f8:	e08f1001 	add	r1, pc, r1
  fc:	ebfffffe 	bl	0 <fprintf>
			fc: R_ARM_CALL	fprintf
 100:	e3080003 	movw	r0, #32771	; 0x8003
 104:	e34a001d 	movt	r0, #40989	; 0xa01d
 108:	eaffffe9 	b	b4 <md_check_attr+0xb4>
 10c:	e59f327c 	ldr	r3, [pc, #636]	; 390 <md_check_attr+0x390>
 110:	e3a0e002 	mov	lr, #2
 114:	e59f2280 	ldr	r2, [pc, #640]	; 39c <md_check_attr+0x39c>
 118:	e3a0c000 	mov	ip, #0
 11c:	e7950003 	ldr	r0, [r5, r3]
 120:	e3a0301c 	mov	r3, #28
 124:	e58d1000 	str	r1, [sp]
 128:	e59f1270 	ldr	r1, [pc, #624]	; 3a0 <md_check_attr+0x3a0>
 12c:	e5900000 	ldr	r0, [r0]
 130:	e08f2002 	add	r2, pc, r2
 134:	e98d5000 	stmib	sp, {ip, lr}
 138:	e08f1001 	add	r1, pc, r1
 13c:	ebfffffe 	bl	0 <fprintf>
			13c: R_ARM_CALL	fprintf
 140:	e3080003 	movw	r0, #32771	; 0x8003
 144:	e34a001d 	movt	r0, #40989	; 0xa01d
 148:	eaffffd9 	b	b4 <md_check_attr+0xb4>
 14c:	e59f323c 	ldr	r3, [pc, #572]	; 390 <md_check_attr+0x390>
 150:	e3a0e002 	mov	lr, #2
 154:	e59f2248 	ldr	r2, [pc, #584]	; 3a4 <md_check_attr+0x3a4>
 158:	e3a0c000 	mov	ip, #0
 15c:	e7950003 	ldr	r0, [r5, r3]
 160:	e3a0303b 	mov	r3, #59	; 0x3b
 164:	e58d1000 	str	r1, [sp]
 168:	e59f1238 	ldr	r1, [pc, #568]	; 3a8 <md_check_attr+0x3a8>
 16c:	e5900000 	ldr	r0, [r0]
 170:	e08f2002 	add	r2, pc, r2
 174:	e98d5000 	stmib	sp, {ip, lr}
 178:	e08f1001 	add	r1, pc, r1
 17c:	ebfffffe 	bl	0 <fprintf>
			17c: R_ARM_CALL	fprintf
 180:	e3080003 	movw	r0, #32771	; 0x8003
 184:	e34a001d 	movt	r0, #40989	; 0xa01d
 188:	eaffffc9 	b	b4 <md_check_attr+0xb4>
 18c:	e59f11fc 	ldr	r1, [pc, #508]	; 390 <md_check_attr+0x390>
 190:	e3a04801 	mov	r4, #65536	; 0x10000
 194:	e59f2210 	ldr	r2, [pc, #528]	; 3ac <md_check_attr+0x3ac>
 198:	e3a03043 	mov	r3, #67	; 0x43
 19c:	e7950001 	ldr	r0, [r5, r1]
 1a0:	e59f1208 	ldr	r1, [pc, #520]	; 3b0 <md_check_attr+0x3b0>
 1a4:	e5900000 	ldr	r0, [r0]
 1a8:	e08f2002 	add	r2, pc, r2
 1ac:	e08f1001 	add	r1, pc, r1
 1b0:	e88d5000 	stm	sp, {ip, lr}
 1b4:	e58d4008 	str	r4, [sp, #8]
 1b8:	ebfffffe 	bl	0 <fprintf>
			1b8: R_ARM_CALL	fprintf
 1bc:	e3080003 	movw	r0, #32771	; 0x8003
 1c0:	e34a001d 	movt	r0, #40989	; 0xa01d
 1c4:	eaffffba 	b	b4 <md_check_attr+0xb4>
 1c8:	e59f31c0 	ldr	r3, [pc, #448]	; 390 <md_check_attr+0x390>
 1cc:	e3a0cd1e 	mov	ip, #1920	; 0x780
 1d0:	e59f21dc 	ldr	r2, [pc, #476]	; 3b4 <md_check_attr+0x3b4>
 1d4:	e3a01040 	mov	r1, #64	; 0x40
 1d8:	e7950003 	ldr	r0, [r5, r3]
 1dc:	e3a0301e 	mov	r3, #30
 1e0:	e58d6000 	str	r6, [sp]
 1e4:	e5900000 	ldr	r0, [r0]
 1e8:	e98d1002 	stmib	sp, {r1, ip}
 1ec:	e59f11c4 	ldr	r1, [pc, #452]	; 3b8 <md_check_attr+0x3b8>
 1f0:	e08f2002 	add	r2, pc, r2
 1f4:	e08f1001 	add	r1, pc, r1
 1f8:	ebfffffe 	bl	0 <fprintf>
			1f8: R_ARM_CALL	fprintf
 1fc:	e3080003 	movw	r0, #32771	; 0x8003
 200:	e34a001d 	movt	r0, #40989	; 0xa01d
 204:	eaffffaa 	b	b4 <md_check_attr+0xb4>
 208:	e59f3180 	ldr	r3, [pc, #384]	; 390 <md_check_attr+0x390>
 20c:	e300e438 	movw	lr, #1080	; 0x438
 210:	e59f21a4 	ldr	r2, [pc, #420]	; 3bc <md_check_attr+0x3bc>
 214:	e3a01040 	mov	r1, #64	; 0x40
 218:	e7950003 	ldr	r0, [r5, r3]
 21c:	e3a03020 	mov	r3, #32
 220:	e58dc000 	str	ip, [sp]
 224:	e5900000 	ldr	r0, [r0]
 228:	e98d4002 	stmib	sp, {r1, lr}
 22c:	e59f118c 	ldr	r1, [pc, #396]	; 3c0 <md_check_attr+0x3c0>
 230:	e08f2002 	add	r2, pc, r2
 234:	e08f1001 	add	r1, pc, r1
 238:	ebfffffe 	bl	0 <fprintf>
			238: R_ARM_CALL	fprintf
 23c:	e3080003 	movw	r0, #32771	; 0x8003
 240:	e34a001d 	movt	r0, #40989	; 0xa01d
 244:	eaffff9a 	b	b4 <md_check_attr+0xb4>
 248:	e59f3140 	ldr	r3, [pc, #320]	; 390 <md_check_attr+0x390>
 24c:	e3a0e003 	mov	lr, #3
 250:	e59f216c 	ldr	r2, [pc, #364]	; 3c4 <md_check_attr+0x3c4>
 254:	e3a0c000 	mov	ip, #0
 258:	e7950003 	ldr	r0, [r5, r3]
 25c:	e3a03024 	mov	r3, #36	; 0x24
 260:	e58d1000 	str	r1, [sp]
 264:	e59f115c 	ldr	r1, [pc, #348]	; 3c8 <md_check_attr+0x3c8>
 268:	e5900000 	ldr	r0, [r0]
 26c:	e08f2002 	add	r2, pc, r2
 270:	e98d5000 	stmib	sp, {ip, lr}
 274:	e08f1001 	add	r1, pc, r1
 278:	ebfffffe 	bl	0 <fprintf>
			278: R_ARM_CALL	fprintf
 27c:	e3080003 	movw	r0, #32771	; 0x8003
 280:	e34a001d 	movt	r0, #40989	; 0xa01d
 284:	eaffff8a 	b	b4 <md_check_attr+0xb4>
 288:	e59f1100 	ldr	r1, [pc, #256]	; 390 <md_check_attr+0x390>
 28c:	e3a0302a 	mov	r3, #42	; 0x2a
 290:	e59f2134 	ldr	r2, [pc, #308]	; 3cc <md_check_attr+0x3cc>
 294:	e7950001 	ldr	r0, [r5, r1]
 298:	e59f1130 	ldr	r1, [pc, #304]	; 3d0 <md_check_attr+0x3d0>
 29c:	e5900000 	ldr	r0, [r0]
 2a0:	e08f2002 	add	r2, pc, r2
 2a4:	e08f1001 	add	r1, pc, r1
 2a8:	e58de000 	str	lr, [sp]
 2ac:	ebfffffe 	bl	0 <fprintf>
			2ac: R_ARM_CALL	fprintf
 2b0:	e3080003 	movw	r0, #32771	; 0x8003
 2b4:	e34a001d 	movt	r0, #40989	; 0xa01d
 2b8:	eaffff7d 	b	b4 <md_check_attr+0xb4>
 2bc:	e59f10cc 	ldr	r1, [pc, #204]	; 390 <md_check_attr+0x390>
 2c0:	e3a0302e 	mov	r3, #46	; 0x2e
 2c4:	e59f2108 	ldr	r2, [pc, #264]	; 3d4 <md_check_attr+0x3d4>
 2c8:	e7950001 	ldr	r0, [r5, r1]
 2cc:	e59f1104 	ldr	r1, [pc, #260]	; 3d8 <md_check_attr+0x3d8>
 2d0:	e5900000 	ldr	r0, [r0]
 2d4:	e08f2002 	add	r2, pc, r2
 2d8:	e08f1001 	add	r1, pc, r1
 2dc:	e58d4004 	str	r4, [sp, #4]
 2e0:	e58d6000 	str	r6, [sp]
 2e4:	ebfffffe 	bl	0 <fprintf>
			2e4: R_ARM_CALL	fprintf
 2e8:	e3080003 	movw	r0, #32771	; 0x8003
 2ec:	e34a001d 	movt	r0, #40989	; 0xa01d
 2f0:	eaffff6f 	b	b4 <md_check_attr+0xb4>
 2f4:	e59f1094 	ldr	r1, [pc, #148]	; 390 <md_check_attr+0x390>
 2f8:	e3a03031 	mov	r3, #49	; 0x31
 2fc:	e59f20d8 	ldr	r2, [pc, #216]	; 3dc <md_check_attr+0x3dc>
 300:	e7950001 	ldr	r0, [r5, r1]
 304:	e59f10d4 	ldr	r1, [pc, #212]	; 3e0 <md_check_attr+0x3e0>
 308:	e5900000 	ldr	r0, [r0]
 30c:	e08f2002 	add	r2, pc, r2
 310:	e08f1001 	add	r1, pc, r1
 314:	e58d4004 	str	r4, [sp, #4]
 318:	e58dc000 	str	ip, [sp]
 31c:	ebfffffe 	bl	0 <fprintf>
			31c: R_ARM_CALL	fprintf
 320:	e3080003 	movw	r0, #32771	; 0x8003
 324:	e34a001d 	movt	r0, #40989	; 0xa01d
 328:	eaffff61 	b	b4 <md_check_attr+0xb4>
 32c:	e59f105c 	ldr	r1, [pc, #92]	; 390 <md_check_attr+0x390>
 330:	e3a0303e 	mov	r3, #62	; 0x3e
 334:	e59f20a8 	ldr	r2, [pc, #168]	; 3e4 <md_check_attr+0x3e4>
 338:	e7950001 	ldr	r0, [r5, r1]
 33c:	e59f10a4 	ldr	r1, [pc, #164]	; 3e8 <md_check_attr+0x3e8>
 340:	e08f2002 	add	r2, pc, r2
 344:	e08f1001 	add	r1, pc, r1
 348:	e5900000 	ldr	r0, [r0]
 34c:	ebfffffe 	bl	0 <fprintf>
			34c: R_ARM_CALL	fprintf
 350:	e3080003 	movw	r0, #32771	; 0x8003
 354:	e34a001d 	movt	r0, #40989	; 0xa01d
 358:	eaffff55 	b	b4 <md_check_attr+0xb4>
 35c:	e59f102c 	ldr	r1, [pc, #44]	; 390 <md_check_attr+0x390>
 360:	e3a03045 	mov	r3, #69	; 0x45
 364:	e59f2080 	ldr	r2, [pc, #128]	; 3ec <md_check_attr+0x3ec>
 368:	e7950001 	ldr	r0, [r5, r1]
 36c:	e59f107c 	ldr	r1, [pc, #124]	; 3f0 <md_check_attr+0x3f0>
 370:	e08f2002 	add	r2, pc, r2
 374:	e08f1001 	add	r1, pc, r1
 378:	e5900000 	ldr	r0, [r0]
 37c:	ebfffffe 	bl	0 <fprintf>
			37c: R_ARM_CALL	fprintf
 380:	e3080003 	movw	r0, #32771	; 0x8003
 384:	e34a001d 	movt	r0, #40989	; 0xa01d
 388:	eaffff49 	b	b4 <md_check_attr+0xb4>
 38c:	00000370 	.word	0x00000370
			38c: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 390:	00000000 	.word	0x00000000
			390: R_ARM_GOT32	stderr
 394:	000002a0 	.word	0x000002a0
			394: R_ARM_REL32	.rodata.__FUNCTION__.8990
 398:	00000298 	.word	0x00000298
			398: R_ARM_REL32	.LC7
 39c:	00000264 	.word	0x00000264
			39c: R_ARM_REL32	.rodata.__FUNCTION__.8990
 3a0:	00000260 	.word	0x00000260
			3a0: R_ARM_REL32	.LC0
 3a4:	0000022c 	.word	0x0000022c
			3a4: R_ARM_REL32	.rodata.__FUNCTION__.8990
 3a8:	00000228 	.word	0x00000228
			3a8: R_ARM_REL32	.LC8
 3ac:	000001fc 	.word	0x000001fc
			3ac: R_ARM_REL32	.rodata.__FUNCTION__.8990
 3b0:	000001fc 	.word	0x000001fc
			3b0: R_ARM_REL32	.LC10
 3b4:	000001bc 	.word	0x000001bc
			3b4: R_ARM_REL32	.rodata.__FUNCTION__.8990
 3b8:	000001bc 	.word	0x000001bc
			3b8: R_ARM_REL32	.LC1
 3bc:	00000184 	.word	0x00000184
			3bc: R_ARM_REL32	.rodata.__FUNCTION__.8990
 3c0:	00000184 	.word	0x00000184
			3c0: R_ARM_REL32	.LC2
 3c4:	00000150 	.word	0x00000150
			3c4: R_ARM_REL32	.rodata.__FUNCTION__.8990
 3c8:	0000014c 	.word	0x0000014c
			3c8: R_ARM_REL32	.LC3
 3cc:	00000124 	.word	0x00000124
			3cc: R_ARM_REL32	.rodata.__FUNCTION__.8990
 3d0:	00000124 	.word	0x00000124
			3d0: R_ARM_REL32	.LC4
 3d4:	000000f8 	.word	0x000000f8
			3d4: R_ARM_REL32	.rodata.__FUNCTION__.8990
 3d8:	000000f8 	.word	0x000000f8
			3d8: R_ARM_REL32	.LC5
 3dc:	000000c8 	.word	0x000000c8
			3dc: R_ARM_REL32	.rodata.__FUNCTION__.8990
 3e0:	000000c8 	.word	0x000000c8
			3e0: R_ARM_REL32	.LC6
 3e4:	0000009c 	.word	0x0000009c
			3e4: R_ARM_REL32	.rodata.__FUNCTION__.8990
 3e8:	0000009c 	.word	0x0000009c
			3e8: R_ARM_REL32	.LC9
 3ec:	00000074 	.word	0x00000074
			3ec: R_ARM_REL32	.rodata.__FUNCTION__.8990
 3f0:	00000074 	.word	0x00000074
			3f0: R_ARM_REL32	.LC11
