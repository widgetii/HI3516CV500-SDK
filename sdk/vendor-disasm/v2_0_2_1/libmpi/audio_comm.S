
audio_comm.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l    df *ABS*	00000000 audio_comm.c
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.audio_alloc	00000000 .text.audio_alloc
0000002c l       .rodata.audio_alloc.str1.4	00000000 .LC2
00000000 l       .rodata.audio_alloc.str1.4	00000000 .LC0
00000014 l       .rodata.audio_alloc.str1.4	00000000 .LC1
00000000 l    d  .text.audio_free	00000000 .text.audio_free
00000000 l    d  .data.g_mmz_fd	00000000 .data.g_mmz_fd
00000000 l     O .data.g_mmz_fd	00000004 g_mmz_fd
00000000 l    d  .rodata.audio_alloc.str1.4	00000000 .rodata.audio_alloc.str1.4
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.audio_alloc	00000178 audio_alloc
00000000         *UND*	00000000 memset
00000000         *UND*	00000000 snprintf_s
00000000         *UND*	00000000 ioctl
00000000         *UND*	00000000 open
00000000         *UND*	00000000 perror
00000000         *UND*	00000000 __stack_chk_fail
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 __stack_chk_guard
00000000 g     F .text.audio_free	00000110 audio_free



Disassembly of section .text.audio_alloc:

00000000 <audio_alloc>:
   0:	e59fc150 	ldr	ip, [pc, #336]	; 158 <audio_alloc+0x158>
   4:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
   8:	e1a08003 	mov	r8, r3
   c:	e08fc00c 	add	ip, pc, ip
  10:	e24dd064 	sub	sp, sp, #100	; 0x64
  14:	e59f5140 	ldr	r5, [pc, #320]	; 15c <audio_alloc+0x15c>
  18:	e28d4008 	add	r4, sp, #8
  1c:	e59f313c 	ldr	r3, [pc, #316]	; 160 <audio_alloc+0x160>
  20:	e1a09000 	mov	r9, r0
  24:	e08f5005 	add	r5, pc, r5
  28:	e1a06001 	mov	r6, r1
  2c:	e79c3003 	ldr	r3, [ip, r3]
  30:	e1a07002 	mov	r7, r2
  34:	e1a00004 	mov	r0, r4
  38:	e3a02050 	mov	r2, #80	; 0x50
  3c:	e5933000 	ldr	r3, [r3]
  40:	e58d305c 	str	r3, [sp, #92]	; 0x5c
  44:	e3a03000 	mov	r3, #0
  48:	e3a01000 	mov	r1, #0
  4c:	ebfffffe 	bl	0 <memset>
			4c: R_ARM_CALL	memset
  50:	e5953000 	ldr	r3, [r5]
  54:	e3530000 	cmp	r3, #0
  58:	ba00002f 	blt	11c <audio_alloc+0x11c>
  5c:	e58d9000 	str	r9, [sp]
  60:	e3a03003 	mov	r3, #3
  64:	e5cd301c 	strb	r3, [sp, #28]
  68:	e3a01001 	mov	r1, #1
  6c:	e59dc01c 	ldr	ip, [sp, #28]
  70:	e3a0200f 	mov	r2, #15
  74:	e59f30e8 	ldr	r3, [pc, #232]	; 164 <audio_alloc+0x164>
  78:	e28d0020 	add	r0, sp, #32
  7c:	e7d3c411 	bfi	ip, r1, #8, #12
  80:	e3a01010 	mov	r1, #16
  84:	e08f3003 	add	r3, pc, r3
  88:	e58d8010 	str	r8, [sp, #16]
  8c:	e58dc01c 	str	ip, [sp, #28]
  90:	ebfffffe 	bl	0 <snprintf_s>
			90: R_ARM_CALL	snprintf_s
  94:	e3500000 	cmp	r0, #0
  98:	da00002b 	ble	14c <audio_alloc+0x14c>
  9c:	e59f30c4 	ldr	r3, [pc, #196]	; 168 <audio_alloc+0x168>
  a0:	e3061d0a 	movw	r1, #27914	; 0x6d0a
  a4:	e34c1050 	movt	r1, #49232	; 0xc050
  a8:	e1a02004 	mov	r2, r4
  ac:	e08f3003 	add	r3, pc, r3
  b0:	e5935000 	ldr	r5, [r3]
  b4:	e1a00005 	mov	r0, r5
  b8:	ebfffffe 	bl	0 <ioctl>
			b8: R_ARM_CALL	ioctl
  bc:	e3500000 	cmp	r0, #0
  c0:	1a000021 	bne	14c <audio_alloc+0x14c>
  c4:	e3061d14 	movw	r1, #27924	; 0x6d14
  c8:	e1a02004 	mov	r2, r4
  cc:	e34c1050 	movt	r1, #49232	; 0xc050
  d0:	e1a00005 	mov	r0, r5
  d4:	ebfffffe 	bl	0 <ioctl>
			d4: R_ARM_CALL	ioctl
  d8:	e2503000 	subs	r3, r0, #0
  dc:	1a00001a 	bne	14c <audio_alloc+0x14c>
  e0:	e59d1008 	ldr	r1, [sp, #8]
  e4:	e1a00003 	mov	r0, r3
  e8:	e59d2018 	ldr	r2, [sp, #24]
  ec:	e886000a 	stm	r6, {r1, r3}
  f0:	e5872000 	str	r2, [r7]
  f4:	e59f2070 	ldr	r2, [pc, #112]	; 16c <audio_alloc+0x16c>
  f8:	e59f3060 	ldr	r3, [pc, #96]	; 160 <audio_alloc+0x160>
  fc:	e08f2002 	add	r2, pc, r2
 100:	e7923003 	ldr	r3, [r2, r3]
 104:	e5932000 	ldr	r2, [r3]
 108:	e59d305c 	ldr	r3, [sp, #92]	; 0x5c
 10c:	e0332002 	eors	r2, r3, r2
 110:	1a00000f 	bne	154 <audio_alloc+0x154>
 114:	e28dd064 	add	sp, sp, #100	; 0x64
 118:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
 11c:	e59f004c 	ldr	r0, [pc, #76]	; 170 <audio_alloc+0x170>
 120:	e3a01002 	mov	r1, #2
 124:	e08f0000 	add	r0, pc, r0
 128:	ebfffffe 	bl	0 <open>
			128: R_ARM_CALL	open
 12c:	e5850000 	str	r0, [r5]
 130:	e3500000 	cmp	r0, #0
 134:	aaffffc8 	bge	5c <audio_alloc+0x5c>
 138:	e59f0034 	ldr	r0, [pc, #52]	; 174 <audio_alloc+0x174>
 13c:	e08f0000 	add	r0, pc, r0
 140:	ebfffffe 	bl	0 <perror>
			140: R_ARM_CALL	perror
 144:	e3e00000 	mvn	r0, #0
 148:	eaffffe9 	b	f4 <audio_alloc+0xf4>
 14c:	e3e00000 	mvn	r0, #0
 150:	eaffffe7 	b	f4 <audio_alloc+0xf4>
 154:	ebfffffe 	bl	0 <__stack_chk_fail>
			154: R_ARM_CALL	__stack_chk_fail
 158:	00000144 	.word	0x00000144
			158: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 15c:	00000130 	.word	0x00000130
			15c: R_ARM_REL32	.data.g_mmz_fd
 160:	00000000 	.word	0x00000000
			160: R_ARM_GOT32	__stack_chk_guard
 164:	000000d8 	.word	0x000000d8
			164: R_ARM_REL32	.LC2
 168:	000000b4 	.word	0x000000b4
			168: R_ARM_REL32	.data.g_mmz_fd
 16c:	00000068 	.word	0x00000068
			16c: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 170:	00000044 	.word	0x00000044
			170: R_ARM_REL32	.LC0
 174:	00000030 	.word	0x00000030
			174: R_ARM_REL32	.LC1

Disassembly of section .text.audio_free:

00000000 <audio_free>:
   0:	e59fc0f0 	ldr	ip, [pc, #240]	; f8 <audio_free+0xf8>
   4:	e3a02050 	mov	r2, #80	; 0x50
   8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   c:	e24dd05c 	sub	sp, sp, #92	; 0x5c
  10:	e08fc00c 	add	ip, pc, ip
  14:	e1a07000 	mov	r7, r0
  18:	e59f60dc 	ldr	r6, [pc, #220]	; fc <audio_free+0xfc>
  1c:	e1a0000d 	mov	r0, sp
  20:	e59f30d8 	ldr	r3, [pc, #216]	; 100 <audio_free+0x100>
  24:	e3a01000 	mov	r1, #0
  28:	e08f6006 	add	r6, pc, r6
  2c:	e1a0500d 	mov	r5, sp
  30:	e79c3003 	ldr	r3, [ip, r3]
  34:	e5933000 	ldr	r3, [r3]
  38:	e58d3054 	str	r3, [sp, #84]	; 0x54
  3c:	e3a03000 	mov	r3, #0
  40:	ebfffffe 	bl	0 <memset>
			40: R_ARM_CALL	memset
  44:	e5964000 	ldr	r4, [r6]
  48:	e3540000 	cmp	r4, #0
  4c:	ba000019 	blt	b8 <audio_free+0xb8>
  50:	e58d7000 	str	r7, [sp]
  54:	e3061d16 	movw	r1, #27926	; 0x6d16
  58:	e34c1050 	movt	r1, #49232	; 0xc050
  5c:	e1a02005 	mov	r2, r5
  60:	e1a00004 	mov	r0, r4
  64:	ebfffffe 	bl	0 <ioctl>
			64: R_ARM_CALL	ioctl
  68:	e3500000 	cmp	r0, #0
  6c:	1a00001e 	bne	ec <audio_free+0xec>
  70:	e3061d0c 	movw	r1, #27916	; 0x6d0c
  74:	e1a02005 	mov	r2, r5
  78:	e3441050 	movt	r1, #16464	; 0x4050
  7c:	e1a00004 	mov	r0, r4
  80:	ebfffffe 	bl	0 <ioctl>
			80: R_ARM_CALL	ioctl
  84:	e2900000 	adds	r0, r0, #0
  88:	13a00001 	movne	r0, #1
  8c:	e2600000 	rsb	r0, r0, #0
  90:	e59f206c 	ldr	r2, [pc, #108]	; 104 <audio_free+0x104>
  94:	e59f3064 	ldr	r3, [pc, #100]	; 100 <audio_free+0x100>
  98:	e08f2002 	add	r2, pc, r2
  9c:	e7923003 	ldr	r3, [r2, r3]
  a0:	e5932000 	ldr	r2, [r3]
  a4:	e59d3054 	ldr	r3, [sp, #84]	; 0x54
  a8:	e0332002 	eors	r2, r3, r2
  ac:	1a000010 	bne	f4 <audio_free+0xf4>
  b0:	e28dd05c 	add	sp, sp, #92	; 0x5c
  b4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
  b8:	e59f0048 	ldr	r0, [pc, #72]	; 108 <audio_free+0x108>
  bc:	e3a01002 	mov	r1, #2
  c0:	e08f0000 	add	r0, pc, r0
  c4:	ebfffffe 	bl	0 <open>
			c4: R_ARM_CALL	open
  c8:	e5860000 	str	r0, [r6]
  cc:	e3500000 	cmp	r0, #0
  d0:	e1a04000 	mov	r4, r0
  d4:	aaffffdd 	bge	50 <audio_free+0x50>
  d8:	e59f002c 	ldr	r0, [pc, #44]	; 10c <audio_free+0x10c>
  dc:	e08f0000 	add	r0, pc, r0
  e0:	ebfffffe 	bl	0 <perror>
			e0: R_ARM_CALL	perror
  e4:	e3e00000 	mvn	r0, #0
  e8:	eaffffe8 	b	90 <audio_free+0x90>
  ec:	e3e00000 	mvn	r0, #0
  f0:	eaffffe6 	b	90 <audio_free+0x90>
  f4:	ebfffffe 	bl	0 <__stack_chk_fail>
			f4: R_ARM_CALL	__stack_chk_fail
  f8:	000000e0 	.word	0x000000e0
			f8: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  fc:	000000cc 	.word	0x000000cc
			fc: R_ARM_REL32	.data.g_mmz_fd
 100:	00000000 	.word	0x00000000
			100: R_ARM_GOT32	__stack_chk_guard
 104:	00000064 	.word	0x00000064
			104: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 108:	00000040 	.word	0x00000040
			108: R_ARM_REL32	.LC0
 10c:	00000028 	.word	0x00000028
			10c: R_ARM_REL32	.LC1
