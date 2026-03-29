
hi_audio_dl_work.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l       .rodata.DL_hi_audio_dlopen.str1.1	00000000 .LC0
0000000f l       .rodata.DL_hi_audio_dlopen.str1.1	00000000 .LC1
0000001e l       .rodata.DL_hi_audio_dlopen.str1.1	00000000 .LC2
0000002d l       .rodata.DL_hi_audio_dlopen.str1.1	00000000 .LC3
0000003b l       .rodata.DL_hi_audio_dlopen.str1.1	00000000 .LC4
0000008b l       .rodata.DL_hi_audio_dlopen.str1.1	00000000 .LC5
00000000 l       .rodata.DL_hi_audio_dlsym.str1.1	00000000 .LC6
00000030 l       .rodata.DL_hi_audio_dlsym.str1.1	00000000 .LC7
0000005c l       .rodata.DL_hi_audio_dlsym.str1.1	00000000 .LC8
00000000 l    d  .rodata.__FUNCTION__.6323	00000000 .rodata.__FUNCTION__.6323
00000000 l    d  .rodata.__FUNCTION__.6330	00000000 .rodata.__FUNCTION__.6330
00000000 l    d  .rodata.__FUNCTION__.6334	00000000 .rodata.__FUNCTION__.6334
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.DL_hi_audio_dlopen	00000000 .text.DL_hi_audio_dlopen
00000000 l    d  .text.DL_hi_audio_dlsym	00000000 .text.DL_hi_audio_dlsym
00000000 l    d  .text.DL_hi_audio_dlclose	00000000 .text.DL_hi_audio_dlclose
00000000 l    d  .rodata.DL_hi_audio_dlopen.str1.1	00000000 .rodata.DL_hi_audio_dlopen.str1.1
00000000 l    d  .rodata.DL_hi_audio_dlsym.str1.1	00000000 .rodata.DL_hi_audio_dlsym.str1.1
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.DL_hi_audio_dlopen	00000120 DL_hi_audio_dlopen
00000000         *UND*	00000000 dlopen
00000000         *UND*	00000000 strcmp
00000000         *UND*	00000000 fprintf
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 stderr
00000000 g     F .text.DL_hi_audio_dlsym	000000dc DL_hi_audio_dlsym
00000000         *UND*	00000000 dlsym
00000000         *UND*	00000000 dlerror
00000000 g     F .text.DL_hi_audio_dlclose	0000005c DL_hi_audio_dlclose
00000000         *UND*	00000000 dlclose



Disassembly of section .text.DL_hi_audio_dlopen:

00000000 <DL_hi_audio_dlopen>:
   0:	e92d40f7 	push	{r0, r1, r2, r4, r5, r6, r7, lr}
   4:	e3510000 	cmp	r1, #0
   8:	13500000 	cmpne	r0, #0
   c:	e59f70e4 	ldr	r7, [pc, #228]	; f8 <DL_hi_audio_dlopen+0xf8>
  10:	03a05001 	moveq	r5, #1
  14:	13a05000 	movne	r5, #0
  18:	e08f7007 	add	r7, pc, r7
  1c:	0a00002b 	beq	d0 <DL_hi_audio_dlopen+0xd0>
  20:	e5805000 	str	r5, [r0]
  24:	e1a04001 	mov	r4, r1
  28:	e1a06000 	mov	r6, r0
  2c:	e3a01001 	mov	r1, #1
  30:	e1a00004 	mov	r0, r4
  34:	ebfffffe 	bl	0 <dlopen>
			34: R_ARM_CALL	dlopen
  38:	e5860000 	str	r0, [r6]
  3c:	e3500000 	cmp	r0, #0
  40:	11a00005 	movne	r0, r5
  44:	1a000022 	bne	d4 <DL_hi_audio_dlopen+0xd4>
  48:	e59f10ac 	ldr	r1, [pc, #172]	; fc <DL_hi_audio_dlopen+0xfc>
  4c:	e1a00004 	mov	r0, r4
  50:	e08f1001 	add	r1, pc, r1
  54:	ebfffffe 	bl	0 <strcmp>
			54: R_ARM_CALL	strcmp
  58:	e59f30a0 	ldr	r3, [pc, #160]	; 100 <DL_hi_audio_dlopen+0x100>
  5c:	e3500000 	cmp	r0, #0
  60:	e7975003 	ldr	r5, [r7, r3]
  64:	0a000011 	beq	b0 <DL_hi_audio_dlopen+0xb0>
  68:	e59f1094 	ldr	r1, [pc, #148]	; 104 <DL_hi_audio_dlopen+0x104>
  6c:	e1a00004 	mov	r0, r4
  70:	e08f1001 	add	r1, pc, r1
  74:	ebfffffe 	bl	0 <strcmp>
			74: R_ARM_CALL	strcmp
  78:	e3500000 	cmp	r0, #0
  7c:	0a00000b 	beq	b0 <DL_hi_audio_dlopen+0xb0>
  80:	e59f1080 	ldr	r1, [pc, #128]	; 108 <DL_hi_audio_dlopen+0x108>
  84:	e1a00004 	mov	r0, r4
  88:	e08f1001 	add	r1, pc, r1
  8c:	ebfffffe 	bl	0 <strcmp>
			8c: R_ARM_CALL	strcmp
  90:	e3500000 	cmp	r0, #0
  94:	0a000005 	beq	b0 <DL_hi_audio_dlopen+0xb0>
  98:	e59f106c 	ldr	r1, [pc, #108]	; 10c <DL_hi_audio_dlopen+0x10c>
  9c:	e1a00004 	mov	r0, r4
  a0:	e08f1001 	add	r1, pc, r1
  a4:	ebfffffe 	bl	0 <strcmp>
			a4: R_ARM_CALL	strcmp
  a8:	e3500000 	cmp	r0, #0
  ac:	1a00000a 	bne	dc <DL_hi_audio_dlopen+0xdc>
  b0:	e59f2058 	ldr	r2, [pc, #88]	; 110 <DL_hi_audio_dlopen+0x110>
  b4:	e3a03027 	mov	r3, #39	; 0x27
  b8:	e59f1054 	ldr	r1, [pc, #84]	; 114 <DL_hi_audio_dlopen+0x114>
  bc:	e08f2002 	add	r2, pc, r2
  c0:	e08f1001 	add	r1, pc, r1
  c4:	e58d4000 	str	r4, [sp]
  c8:	e5950000 	ldr	r0, [r5]
  cc:	ebfffffe 	bl	0 <fprintf>
			cc: R_ARM_CALL	fprintf
  d0:	e3e00000 	mvn	r0, #0
  d4:	e28dd00c 	add	sp, sp, #12
  d8:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
  dc:	e59f2034 	ldr	r2, [pc, #52]	; 118 <DL_hi_audio_dlopen+0x118>
  e0:	e3a0302d 	mov	r3, #45	; 0x2d
  e4:	e59f1030 	ldr	r1, [pc, #48]	; 11c <DL_hi_audio_dlopen+0x11c>
  e8:	e58d4000 	str	r4, [sp]
  ec:	e08f2002 	add	r2, pc, r2
  f0:	e08f1001 	add	r1, pc, r1
  f4:	eafffff3 	b	c8 <DL_hi_audio_dlopen+0xc8>
  f8:	000000d8 	.word	0x000000d8
			f8: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  fc:	000000a4 	.word	0x000000a4
			fc: R_ARM_REL32	.LC0
 100:	00000000 	.word	0x00000000
			100: R_ARM_GOT32	stderr
 104:	0000008c 	.word	0x0000008c
			104: R_ARM_REL32	.LC1
 108:	00000078 	.word	0x00000078
			108: R_ARM_REL32	.LC2
 10c:	00000064 	.word	0x00000064
			10c: R_ARM_REL32	.LC3
 110:	0000004c 	.word	0x0000004c
			110: R_ARM_REL32	.rodata.__FUNCTION__.6323
 114:	0000004c 	.word	0x0000004c
			114: R_ARM_REL32	.LC4
 118:	00000024 	.word	0x00000024
			118: R_ARM_REL32	.rodata.__FUNCTION__.6323
 11c:	00000024 	.word	0x00000024
			11c: R_ARM_REL32	.LC5

Disassembly of section .text.DL_hi_audio_dlsym:

00000000 <DL_hi_audio_dlsym>:
   0:	e92d40f7 	push	{r0, r1, r2, r4, r5, r6, r7, lr}
   4:	e1a07000 	mov	r7, r0
   8:	e59f50b0 	ldr	r5, [pc, #176]	; c0 <DL_hi_audio_dlsym+0xc0>
   c:	e2510000 	subs	r0, r1, #0
  10:	e08f5005 	add	r5, pc, r5
  14:	1a00000b 	bne	48 <DL_hi_audio_dlsym+0x48>
  18:	e59f30a4 	ldr	r3, [pc, #164]	; c4 <DL_hi_audio_dlsym+0xc4>
  1c:	e59f20a4 	ldr	r2, [pc, #164]	; c8 <DL_hi_audio_dlsym+0xc8>
  20:	e7950003 	ldr	r0, [r5, r3]
  24:	e3a0303c 	mov	r3, #60	; 0x3c
  28:	e59f109c 	ldr	r1, [pc, #156]	; cc <DL_hi_audio_dlsym+0xcc>
  2c:	e08f2002 	add	r2, pc, r2
  30:	e08f1001 	add	r1, pc, r1
  34:	e5900000 	ldr	r0, [r0]
  38:	ebfffffe 	bl	0 <fprintf>
			38: R_ARM_CALL	fprintf
  3c:	e3e00000 	mvn	r0, #0
  40:	e28dd00c 	add	sp, sp, #12
  44:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
  48:	e3a04000 	mov	r4, #0
  4c:	e1a01002 	mov	r1, r2
  50:	e5874000 	str	r4, [r7]
  54:	e1a06002 	mov	r6, r2
  58:	ebfffffe 	bl	0 <dlsym>
			58: R_ARM_CALL	dlsym
  5c:	e1500004 	cmp	r0, r4
  60:	e5870000 	str	r0, [r7]
  64:	11a00004 	movne	r0, r4
  68:	1afffff4 	bne	40 <DL_hi_audio_dlsym+0x40>
  6c:	e59f3050 	ldr	r3, [pc, #80]	; c4 <DL_hi_audio_dlsym+0xc4>
  70:	e59f4058 	ldr	r4, [pc, #88]	; d0 <DL_hi_audio_dlsym+0xd0>
  74:	e7955003 	ldr	r5, [r5, r3]
  78:	e3a03049 	mov	r3, #73	; 0x49
  7c:	e59f1050 	ldr	r1, [pc, #80]	; d4 <DL_hi_audio_dlsym+0xd4>
  80:	e08f4004 	add	r4, pc, r4
  84:	e58d6000 	str	r6, [sp]
  88:	e1a02004 	mov	r2, r4
  8c:	e08f1001 	add	r1, pc, r1
  90:	e5950000 	ldr	r0, [r5]
  94:	ebfffffe 	bl	0 <fprintf>
			94: R_ARM_CALL	fprintf
  98:	e5955000 	ldr	r5, [r5]
  9c:	ebfffffe 	bl	0 <dlerror>
			9c: R_ARM_CALL	dlerror
  a0:	e59f1030 	ldr	r1, [pc, #48]	; d8 <DL_hi_audio_dlsym+0xd8>
  a4:	e3a0304b 	mov	r3, #75	; 0x4b
  a8:	e58d0000 	str	r0, [sp]
  ac:	e1a02004 	mov	r2, r4
  b0:	e08f1001 	add	r1, pc, r1
  b4:	e1a00005 	mov	r0, r5
  b8:	ebfffffe 	bl	0 <fprintf>
			b8: R_ARM_CALL	fprintf
  bc:	eaffffde 	b	3c <DL_hi_audio_dlsym+0x3c>
  c0:	000000a8 	.word	0x000000a8
			c0: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  c4:	00000000 	.word	0x00000000
			c4: R_ARM_GOT32	stderr
  c8:	00000094 	.word	0x00000094
			c8: R_ARM_REL32	.rodata.__FUNCTION__.6330
  cc:	00000094 	.word	0x00000094
			cc: R_ARM_REL32	.LC6
  d0:	00000048 	.word	0x00000048
			d0: R_ARM_REL32	.rodata.__FUNCTION__.6330
  d4:	00000040 	.word	0x00000040
			d4: R_ARM_REL32	.LC7
  d8:	00000020 	.word	0x00000020
			d8: R_ARM_REL32	.LC8

Disassembly of section .text.DL_hi_audio_dlclose:

00000000 <DL_hi_audio_dlclose>:
   0:	e59f3044 	ldr	r3, [pc, #68]	; 4c <DL_hi_audio_dlclose+0x4c>
   4:	e3500000 	cmp	r0, #0
   8:	e92d4010 	push	{r4, lr}
   c:	e08f3003 	add	r3, pc, r3
  10:	1a00000a 	bne	40 <DL_hi_audio_dlclose+0x40>
  14:	e59f2034 	ldr	r2, [pc, #52]	; 50 <DL_hi_audio_dlclose+0x50>
  18:	e59f1034 	ldr	r1, [pc, #52]	; 54 <DL_hi_audio_dlclose+0x54>
  1c:	e7930002 	ldr	r0, [r3, r2]
  20:	e3a03058 	mov	r3, #88	; 0x58
  24:	e59f202c 	ldr	r2, [pc, #44]	; 58 <DL_hi_audio_dlclose+0x58>
  28:	e08f1001 	add	r1, pc, r1
  2c:	e08f2002 	add	r2, pc, r2
  30:	e5900000 	ldr	r0, [r0]
  34:	ebfffffe 	bl	0 <fprintf>
			34: R_ARM_CALL	fprintf
  38:	e3e00000 	mvn	r0, #0
  3c:	e8bd8010 	pop	{r4, pc}
  40:	ebfffffe 	bl	0 <dlclose>
			40: R_ARM_CALL	dlclose
  44:	e3a00000 	mov	r0, #0
  48:	e8bd8010 	pop	{r4, pc}
  4c:	00000038 	.word	0x00000038
			4c: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  50:	00000000 	.word	0x00000000
			50: R_ARM_GOT32	stderr
  54:	00000024 	.word	0x00000024
			54: R_ARM_REL32	.LC6
  58:	00000024 	.word	0x00000024
			58: R_ARM_REL32	.rodata.__FUNCTION__.6334
