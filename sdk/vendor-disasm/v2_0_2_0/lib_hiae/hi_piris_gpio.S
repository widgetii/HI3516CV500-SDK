
hi_piris_gpio.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l    df *ABS*	00000000 hi_piris_gpio.c
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.ae_piris_exit	00000000 .text.ae_piris_exit
00000000 l    d  .text.ae_piris_init	00000000 .text.ae_piris_init
00000000 l       .rodata.ae_piris_init.str1.4	00000000 .LC0
00000000 l    d  .text.ae_piris_status_get	00000000 .text.ae_piris_status_get
00000000 l    d  .text.ae_piris_origin_set	00000000 .text.ae_piris_origin_set
00000000 l    d  .text.ae_piris_close_set	00000000 .text.ae_piris_close_set
00000000 l    d  .text.AePiris_register_callback	00000000 .text.AePiris_register_callback
00000000 l       .rodata.AePiris_register_callback.str1.4	00000000 .LC1
0000000c l       .rodata.AePiris_register_callback.str1.4	00000000 .LC2
00000000 l    d  .data.g_Pirisfd	00000000 .data.g_Pirisfd
00000000 l     O .data.g_Pirisfd	00000010 g_Pirisfd
00000000 l    d  .data.g_au8PIRISPrintIndex	00000000 .data.g_au8PIRISPrintIndex
00000000 l     O .data.g_au8PIRISPrintIndex	00000004 g_au8PIRISPrintIndex
00000000 l    d  .rodata.AePiris_register_callback.str1.4	00000000 .rodata.AePiris_register_callback.str1.4
00000000 l    d  .rodata.__FUNCTION__.29286	00000000 .rodata.__FUNCTION__.29286
00000000 l     O .rodata.__FUNCTION__.29286	0000001a __FUNCTION__.29286
00000000 l    d  .rodata.ae_piris_init.str1.4	00000000 .rodata.ae_piris_init.str1.4
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.ae_piris_exit	00000040 ae_piris_exit
00000000         *UND*	00000000 close
00000000 g     F .text.ae_piris_init	000000ec ae_piris_init
00000000         *UND*	00000000 open
00000000         *UND*	00000000 ioctl
00000000         *UND*	00000000 __stack_chk_fail
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 __stack_chk_guard
00000000 g     F .text.ae_piris_status_get	000000a0 ae_piris_status_get
00000000 g     F .text.ae_piris_origin_set	000000c0 ae_piris_origin_set
00000000 g     F .text.ae_piris_close_set	000000c0 ae_piris_close_set
00000000 g     F .text.AePiris_register_callback	0000012c AePiris_register_callback
00000000         *UND*	00000000 strncpy_s
00000000         *UND*	00000000 HI_MPI_AE_IrisRegisterCallBack
00000000         *UND*	00000000 fprintf
00000000         *UND*	00000000 stderr



Disassembly of section .text.ae_piris_exit:

00000000 <ae_piris_exit>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e59f5030 	ldr	r5, [pc, #48]	; 3c <ae_piris_exit+0x3c>
   8:	e08f5005 	add	r5, pc, r5
   c:	e7953100 	ldr	r3, [r5, r0, lsl #2]
  10:	e3530000 	cmp	r3, #0
  14:	ba000006 	blt	34 <ae_piris_exit+0x34>
  18:	e1a04000 	mov	r4, r0
  1c:	e1a00003 	mov	r0, r3
  20:	ebfffffe 	bl	0 <close>
			20: R_ARM_CALL	close
  24:	e3e03000 	mvn	r3, #0
  28:	e3a00000 	mov	r0, #0
  2c:	e7853104 	str	r3, [r5, r4, lsl #2]
  30:	e8bd8070 	pop	{r4, r5, r6, pc}
  34:	e3e00000 	mvn	r0, #0
  38:	e8bd8070 	pop	{r4, r5, r6, pc}
  3c:	0000002c 	.word	0x0000002c
			3c: R_ARM_REL32	.data.g_Pirisfd

Disassembly of section .text.ae_piris_init:

00000000 <ae_piris_init>:
   0:	e59fc0cc 	ldr	ip, [pc, #204]	; d4 <ae_piris_init+0xd4>
   4:	e3a02c01 	mov	r2, #256	; 0x100
   8:	e92d4070 	push	{r4, r5, r6, lr}
   c:	e24dd008 	sub	sp, sp, #8
  10:	e08fc00c 	add	ip, pc, ip
  14:	e1a04000 	mov	r4, r0
  18:	e59f30b8 	ldr	r3, [pc, #184]	; d8 <ae_piris_init+0xd8>
  1c:	e3a01000 	mov	r1, #0
  20:	e59f00b4 	ldr	r0, [pc, #180]	; dc <ae_piris_init+0xdc>
  24:	e58d4000 	str	r4, [sp]
  28:	e79c3003 	ldr	r3, [ip, r3]
  2c:	e59f60ac 	ldr	r6, [pc, #172]	; e0 <ae_piris_init+0xe0>
  30:	e08f0000 	add	r0, pc, r0
  34:	e5933000 	ldr	r3, [r3]
  38:	e58d3004 	str	r3, [sp, #4]
  3c:	e3a03000 	mov	r3, #0
  40:	ebfffffe 	bl	0 <open>
			40: R_ARM_CALL	open
  44:	e08f6006 	add	r6, pc, r6
  48:	e3500000 	cmp	r0, #0
  4c:	e7860104 	str	r0, [r6, r4, lsl #2]
  50:	ba000010 	blt	98 <ae_piris_init+0x98>
  54:	e3071004 	movw	r1, #28676	; 0x7004
  58:	e1a0200d 	mov	r2, sp
  5c:	e3441004 	movt	r1, #16388	; 0x4004
  60:	e1a05000 	mov	r5, r0
  64:	ebfffffe 	bl	0 <ioctl>
			64: R_ARM_CALL	ioctl
  68:	e3500000 	cmp	r0, #0
  6c:	1a000012 	bne	bc <ae_piris_init+0xbc>
  70:	e59f206c 	ldr	r2, [pc, #108]	; e4 <ae_piris_init+0xe4>
  74:	e59f305c 	ldr	r3, [pc, #92]	; d8 <ae_piris_init+0xd8>
  78:	e08f2002 	add	r2, pc, r2
  7c:	e7923003 	ldr	r3, [r2, r3]
  80:	e5932000 	ldr	r2, [r3]
  84:	e59d3004 	ldr	r3, [sp, #4]
  88:	e0332002 	eors	r2, r3, r2
  8c:	1a00000f 	bne	d0 <ae_piris_init+0xd0>
  90:	e28dd008 	add	sp, sp, #8
  94:	e8bd8070 	pop	{r4, r5, r6, pc}
  98:	e59f3048 	ldr	r3, [pc, #72]	; e8 <ae_piris_init+0xe8>
  9c:	e08f3003 	add	r3, pc, r3
  a0:	e7d32004 	ldrb	r2, [r3, r4]
  a4:	e3520001 	cmp	r2, #1
  a8:	03a02000 	moveq	r2, #0
  ac:	03e00000 	mvneq	r0, #0
  b0:	07c32004 	strbeq	r2, [r3, r4]
  b4:	13e00000 	mvnne	r0, #0
  b8:	eaffffec 	b	70 <ae_piris_init+0x70>
  bc:	e1a00005 	mov	r0, r5
  c0:	ebfffffe 	bl	0 <close>
			c0: R_ARM_CALL	close
  c4:	e3e00000 	mvn	r0, #0
  c8:	e7860104 	str	r0, [r6, r4, lsl #2]
  cc:	eaffffe7 	b	70 <ae_piris_init+0x70>
  d0:	ebfffffe 	bl	0 <__stack_chk_fail>
			d0: R_ARM_CALL	__stack_chk_fail
  d4:	000000bc 	.word	0x000000bc
			d4: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  d8:	00000000 	.word	0x00000000
			d8: R_ARM_GOT32	__stack_chk_guard
  dc:	000000a4 	.word	0x000000a4
			dc: R_ARM_REL32	.LC0
  e0:	00000094 	.word	0x00000094
			e0: R_ARM_REL32	.data.g_Pirisfd
  e4:	00000064 	.word	0x00000064
			e4: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  e8:	00000044 	.word	0x00000044
			e8: R_ARM_REL32	.data.g_au8PIRISPrintIndex

Disassembly of section .text.ae_piris_status_get:

00000000 <ae_piris_status_get>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e3071005 	movw	r1, #28677	; 0x7005
   8:	e59f4080 	ldr	r4, [pc, #128]	; 90 <ae_piris_status_get+0x90>
   c:	e24dd008 	sub	sp, sp, #8
  10:	e59fc07c 	ldr	ip, [pc, #124]	; 94 <ae_piris_status_get+0x94>
  14:	e1a05000 	mov	r5, r0
  18:	e08f4004 	add	r4, pc, r4
  1c:	e1a0200d 	mov	r2, sp
  20:	e08fc00c 	add	ip, pc, ip
  24:	e59f306c 	ldr	r3, [pc, #108]	; 98 <ae_piris_status_get+0x98>
  28:	e7946100 	ldr	r6, [r4, r0, lsl #2]
  2c:	e79c3003 	ldr	r3, [ip, r3]
  30:	e3481004 	movt	r1, #32772	; 0x8004
  34:	e1a00006 	mov	r0, r6
  38:	e5933000 	ldr	r3, [r3]
  3c:	e58d3004 	str	r3, [sp, #4]
  40:	e3a03000 	mov	r3, #0
  44:	ebfffffe 	bl	0 <ioctl>
			44: R_ARM_CALL	ioctl
  48:	e3500000 	cmp	r0, #0
  4c:	059d0000 	ldreq	r0, [sp]
  50:	0a000003 	beq	64 <ae_piris_status_get+0x64>
  54:	e1a00006 	mov	r0, r6
  58:	ebfffffe 	bl	0 <close>
			58: R_ARM_CALL	close
  5c:	e3e00000 	mvn	r0, #0
  60:	e7840105 	str	r0, [r4, r5, lsl #2]
  64:	e59f2030 	ldr	r2, [pc, #48]	; 9c <ae_piris_status_get+0x9c>
  68:	e59f3028 	ldr	r3, [pc, #40]	; 98 <ae_piris_status_get+0x98>
  6c:	e08f2002 	add	r2, pc, r2
  70:	e7923003 	ldr	r3, [r2, r3]
  74:	e5932000 	ldr	r2, [r3]
  78:	e59d3004 	ldr	r3, [sp, #4]
  7c:	e0332002 	eors	r2, r3, r2
  80:	1a000001 	bne	8c <ae_piris_status_get+0x8c>
  84:	e28dd008 	add	sp, sp, #8
  88:	e8bd8070 	pop	{r4, r5, r6, pc}
  8c:	ebfffffe 	bl	0 <__stack_chk_fail>
			8c: R_ARM_CALL	__stack_chk_fail
  90:	00000070 	.word	0x00000070
			90: R_ARM_REL32	.data.g_Pirisfd
  94:	0000006c 	.word	0x0000006c
			94: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  98:	00000000 	.word	0x00000000
			98: R_ARM_GOT32	__stack_chk_guard
  9c:	00000028 	.word	0x00000028
			9c: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_

Disassembly of section .text.ae_piris_origin_set:

00000000 <ae_piris_origin_set>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e24dd010 	sub	sp, sp, #16
   8:	e59f30a0 	ldr	r3, [pc, #160]	; b0 <ae_piris_origin_set+0xb0>
   c:	e6ef1071 	uxtb	r1, r1
  10:	e5cd1000 	strb	r1, [sp]
  14:	e3510001 	cmp	r1, #1
  18:	e59f1094 	ldr	r1, [pc, #148]	; b4 <ae_piris_origin_set+0xb4>
  1c:	e1a05000 	mov	r5, r0
  20:	e59f4090 	ldr	r4, [pc, #144]	; b8 <ae_piris_origin_set+0xb8>
  24:	e08f1001 	add	r1, pc, r1
  28:	e58d2004 	str	r2, [sp, #4]
  2c:	1282200a 	addne	r2, r2, #10
  30:	e7913003 	ldr	r3, [r1, r3]
  34:	e3071002 	movw	r1, #28674	; 0x7002
  38:	e08f4004 	add	r4, pc, r4
  3c:	e5933000 	ldr	r3, [r3]
  40:	e58d300c 	str	r3, [sp, #12]
  44:	e3a03000 	mov	r3, #0
  48:	03e03009 	mvneq	r3, #9
  4c:	e7946100 	ldr	r6, [r4, r0, lsl #2]
  50:	00432002 	subeq	r2, r3, r2
  54:	e58d2008 	str	r2, [sp, #8]
  58:	e1a00006 	mov	r0, r6
  5c:	e3441004 	movt	r1, #16388	; 0x4004
  60:	e1a0200d 	mov	r2, sp
  64:	ebfffffe 	bl	0 <ioctl>
			64: R_ARM_CALL	ioctl
  68:	e3500000 	cmp	r0, #0
  6c:	1a000009 	bne	98 <ae_piris_origin_set+0x98>
  70:	e59f2044 	ldr	r2, [pc, #68]	; bc <ae_piris_origin_set+0xbc>
  74:	e59f3034 	ldr	r3, [pc, #52]	; b0 <ae_piris_origin_set+0xb0>
  78:	e08f2002 	add	r2, pc, r2
  7c:	e7923003 	ldr	r3, [r2, r3]
  80:	e5932000 	ldr	r2, [r3]
  84:	e59d300c 	ldr	r3, [sp, #12]
  88:	e0332002 	eors	r2, r3, r2
  8c:	1a000006 	bne	ac <ae_piris_origin_set+0xac>
  90:	e28dd010 	add	sp, sp, #16
  94:	e8bd8070 	pop	{r4, r5, r6, pc}
  98:	e1a00006 	mov	r0, r6
  9c:	ebfffffe 	bl	0 <close>
			9c: R_ARM_CALL	close
  a0:	e3e00000 	mvn	r0, #0
  a4:	e7840105 	str	r0, [r4, r5, lsl #2]
  a8:	eafffff0 	b	70 <ae_piris_origin_set+0x70>
  ac:	ebfffffe 	bl	0 <__stack_chk_fail>
			ac: R_ARM_CALL	__stack_chk_fail
  b0:	00000000 	.word	0x00000000
			b0: R_ARM_GOT32	__stack_chk_guard
  b4:	00000088 	.word	0x00000088
			b4: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  b8:	00000078 	.word	0x00000078
			b8: R_ARM_REL32	.data.g_Pirisfd
  bc:	0000003c 	.word	0x0000003c
			bc: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_

Disassembly of section .text.ae_piris_close_set:

00000000 <ae_piris_close_set>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e24dd010 	sub	sp, sp, #16
   8:	e59f40a0 	ldr	r4, [pc, #160]	; b0 <ae_piris_close_set+0xb0>
   c:	e6ef1071 	uxtb	r1, r1
  10:	e5cd1000 	strb	r1, [sp]
  14:	e3510001 	cmp	r1, #1
  18:	e59f1094 	ldr	r1, [pc, #148]	; b4 <ae_piris_close_set+0xb4>
  1c:	e1a05000 	mov	r5, r0
  20:	e59f3090 	ldr	r3, [pc, #144]	; b8 <ae_piris_close_set+0xb8>
  24:	e08f1001 	add	r1, pc, r1
  28:	e08f4004 	add	r4, pc, r4
  2c:	e58d2004 	str	r2, [sp, #4]
  30:	02422001 	subeq	r2, r2, #1
  34:	e7913003 	ldr	r3, [r1, r3]
  38:	e3071003 	movw	r1, #28675	; 0x7003
  3c:	e7946100 	ldr	r6, [r4, r0, lsl #2]
  40:	e5933000 	ldr	r3, [r3]
  44:	e58d300c 	str	r3, [sp, #12]
  48:	e3a03000 	mov	r3, #0
  4c:	13a03000 	movne	r3, #0
  50:	058d2008 	streq	r2, [sp, #8]
  54:	e1a00006 	mov	r0, r6
  58:	e3441004 	movt	r1, #16388	; 0x4004
  5c:	e1a0200d 	mov	r2, sp
  60:	158d3008 	strne	r3, [sp, #8]
  64:	ebfffffe 	bl	0 <ioctl>
			64: R_ARM_CALL	ioctl
  68:	e3500000 	cmp	r0, #0
  6c:	1a000009 	bne	98 <ae_piris_close_set+0x98>
  70:	e59f2044 	ldr	r2, [pc, #68]	; bc <ae_piris_close_set+0xbc>
  74:	e59f303c 	ldr	r3, [pc, #60]	; b8 <ae_piris_close_set+0xb8>
  78:	e08f2002 	add	r2, pc, r2
  7c:	e7923003 	ldr	r3, [r2, r3]
  80:	e5932000 	ldr	r2, [r3]
  84:	e59d300c 	ldr	r3, [sp, #12]
  88:	e0332002 	eors	r2, r3, r2
  8c:	1a000006 	bne	ac <ae_piris_close_set+0xac>
  90:	e28dd010 	add	sp, sp, #16
  94:	e8bd8070 	pop	{r4, r5, r6, pc}
  98:	e1a00006 	mov	r0, r6
  9c:	ebfffffe 	bl	0 <close>
			9c: R_ARM_CALL	close
  a0:	e3e00000 	mvn	r0, #0
  a4:	e7840105 	str	r0, [r4, r5, lsl #2]
  a8:	eafffff0 	b	70 <ae_piris_close_set+0x70>
  ac:	ebfffffe 	bl	0 <__stack_chk_fail>
			ac: R_ARM_CALL	__stack_chk_fail
  b0:	00000080 	.word	0x00000080
			b0: R_ARM_REL32	.data.g_Pirisfd
  b4:	00000088 	.word	0x00000088
			b4: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  b8:	00000000 	.word	0x00000000
			b8: R_ARM_GOT32	__stack_chk_guard
  bc:	0000003c 	.word	0x0000003c
			bc: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_

Disassembly of section .text.AePiris_register_callback:

00000000 <AePiris_register_callback>:
   0:	e92d4030 	push	{r4, r5, lr}
   4:	e24dd034 	sub	sp, sp, #52	; 0x34
   8:	e59fe0e8 	ldr	lr, [pc, #232]	; f8 <AePiris_register_callback+0xf8>
   c:	e28d5030 	add	r5, sp, #48	; 0x30
  10:	e1c00fc0 	bic	r0, r0, r0, asr #31
  14:	e3a0300c 	mov	r3, #12
  18:	e08fe00e 	add	lr, pc, lr
  1c:	e6ef0070 	uxtb	r0, r0
  20:	e59fc0d4 	ldr	ip, [pc, #212]	; fc <AePiris_register_callback+0xfc>
  24:	e1a01003 	mov	r1, r3
  28:	e59f20d0 	ldr	r2, [pc, #208]	; 100 <AePiris_register_callback+0x100>
  2c:	e525001c 	str	r0, [r5, #-28]!	; 0xffffffe4
  30:	e28d0018 	add	r0, sp, #24
  34:	e79ec00c 	ldr	ip, [lr, ip]
  38:	e59f40c4 	ldr	r4, [pc, #196]	; 104 <AePiris_register_callback+0x104>
  3c:	e08f2002 	add	r2, pc, r2
  40:	e08f4004 	add	r4, pc, r4
  44:	e59cc000 	ldr	ip, [ip]
  48:	e58dc02c 	str	ip, [sp, #44]	; 0x2c
  4c:	e3a0c000 	mov	ip, #0
  50:	ebfffffe 	bl	0 <strncpy_s>
			50: R_ARM_CALL	strncpy_s
  54:	e59f30ac 	ldr	r3, [pc, #172]	; 108 <AePiris_register_callback+0x108>
  58:	e1a00005 	mov	r0, r5
  5c:	e59f20a8 	ldr	r2, [pc, #168]	; 10c <AePiris_register_callback+0x10c>
  60:	e1a0100d 	mov	r1, sp
  64:	e794c003 	ldr	ip, [r4, r3]
  68:	e59f30a0 	ldr	r3, [pc, #160]	; 110 <AePiris_register_callback+0x110>
  6c:	e58dc000 	str	ip, [sp]
  70:	e794c002 	ldr	ip, [r4, r2]
  74:	e59f2098 	ldr	r2, [pc, #152]	; 114 <AePiris_register_callback+0x114>
  78:	e58dc004 	str	ip, [sp, #4]
  7c:	e794c003 	ldr	ip, [r4, r3]
  80:	e59f3090 	ldr	r3, [pc, #144]	; 118 <AePiris_register_callback+0x118>
  84:	e58dc008 	str	ip, [sp, #8]
  88:	e7942002 	ldr	r2, [r4, r2]
  8c:	e58d200c 	str	r2, [sp, #12]
  90:	e7943003 	ldr	r3, [r4, r3]
  94:	e58d3010 	str	r3, [sp, #16]
  98:	ebfffffe 	bl	0 <HI_MPI_AE_IrisRegisterCallBack>
			98: R_ARM_CALL	HI_MPI_AE_IrisRegisterCallBack
  9c:	e2505000 	subs	r5, r0, #0
  a0:	0a000008 	beq	c8 <AePiris_register_callback+0xc8>
  a4:	e59f1070 	ldr	r1, [pc, #112]	; 11c <AePiris_register_callback+0x11c>
  a8:	e3a03097 	mov	r3, #151	; 0x97
  ac:	e59f206c 	ldr	r2, [pc, #108]	; 120 <AePiris_register_callback+0x120>
  b0:	e7940001 	ldr	r0, [r4, r1]
  b4:	e59f1068 	ldr	r1, [pc, #104]	; 124 <AePiris_register_callback+0x124>
  b8:	e08f2002 	add	r2, pc, r2
  bc:	e08f1001 	add	r1, pc, r1
  c0:	e5900000 	ldr	r0, [r0]
  c4:	ebfffffe 	bl	0 <fprintf>
			c4: R_ARM_CALL	fprintf
  c8:	e59f2058 	ldr	r2, [pc, #88]	; 128 <AePiris_register_callback+0x128>
  cc:	e1a00005 	mov	r0, r5
  d0:	e59f3024 	ldr	r3, [pc, #36]	; fc <AePiris_register_callback+0xfc>
  d4:	e08f2002 	add	r2, pc, r2
  d8:	e7923003 	ldr	r3, [r2, r3]
  dc:	e5932000 	ldr	r2, [r3]
  e0:	e59d302c 	ldr	r3, [sp, #44]	; 0x2c
  e4:	e0332002 	eors	r2, r3, r2
  e8:	1a000001 	bne	f4 <AePiris_register_callback+0xf4>
  ec:	e28dd034 	add	sp, sp, #52	; 0x34
  f0:	e8bd8030 	pop	{r4, r5, pc}
  f4:	ebfffffe 	bl	0 <__stack_chk_fail>
			f4: R_ARM_CALL	__stack_chk_fail
  f8:	000000d8 	.word	0x000000d8
			f8: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  fc:	00000000 	.word	0x00000000
			fc: R_ARM_GOT32	__stack_chk_guard
 100:	000000bc 	.word	0x000000bc
			100: R_ARM_REL32	.LC1
 104:	000000bc 	.word	0x000000bc
			104: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
	...
			108: R_ARM_GOT32	ae_piris_init
			10c: R_ARM_GOT32	ae_piris_status_get
			110: R_ARM_GOT32	ae_piris_origin_set
			114: R_ARM_GOT32	ae_piris_close_set
			118: R_ARM_GOT32	ae_piris_exit
			11c: R_ARM_GOT32	stderr
 120:	00000060 	.word	0x00000060
			120: R_ARM_REL32	.rodata.__FUNCTION__.29286
 124:	00000060 	.word	0x00000060
			124: R_ARM_REL32	.LC2
 128:	0000004c 	.word	0x0000004c
			128: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
