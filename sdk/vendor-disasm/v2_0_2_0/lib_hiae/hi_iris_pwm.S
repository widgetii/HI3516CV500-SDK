
hi_iris_pwm.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l    df *ABS*	00000000 hi_iris_pwm.c
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.ae_dciris_pwm_init	00000000 .text.ae_dciris_pwm_init
00000078 l       .rodata.ae_dciris_pwm_init.str1.4	00000000 .LC3
00000000 l       .rodata.ae_dciris_pwm_init.str1.4	00000000 .LC0
00000044 l       .rodata.ae_dciris_pwm_init.str1.4	00000000 .LC2
00000010 l       .rodata.ae_dciris_pwm_init.str1.4	00000000 .LC1
00000000 l    d  .text.ae_dciris_pwm_exit	00000000 .text.ae_dciris_pwm_exit
00000000 l    d  .text.ae_dciris_pwm_update	00000000 .text.ae_dciris_pwm_update
00000000 l    d  .text.AeDCiris_register_callback	00000000 .text.AeDCiris_register_callback
00000000 l       .rodata.AeDCiris_register_callback.str1.4	00000000 .LC4
0000000c l       .rodata.AeDCiris_register_callback.str1.4	00000000 .LC5
00000000 l    d  .bss.g_au32PreIrisValue	00000000 .bss.g_au32PreIrisValue
00000000 l     O .bss.g_au32PreIrisValue	00000010 g_au32PreIrisValue
00000000 l    d  .data.g_Ispfd	00000000 .data.g_Ispfd
00000000 l     O .data.g_Ispfd	00000010 g_Ispfd
00000000 l    d  .data.g_Pwmfd	00000000 .data.g_Pwmfd
00000000 l     O .data.g_Pwmfd	00000010 g_Pwmfd
00000000 l    d  .data.g_au32PwmNum	00000000 .data.g_au32PwmNum
00000000 l     O .data.g_au32PwmNum	00000010 g_au32PwmNum
00000000 l    d  .data.g_au8PWMPrintIndex	00000000 .data.g_au8PWMPrintIndex
00000000 l     O .data.g_au8PWMPrintIndex	00000004 g_au8PWMPrintIndex
00000000 l    d  .rodata.AeDCiris_register_callback.str1.4	00000000 .rodata.AeDCiris_register_callback.str1.4
00000000 l    d  .rodata.__FUNCTION__.29260	00000000 .rodata.__FUNCTION__.29260
00000000 l     O .rodata.__FUNCTION__.29260	00000013 __FUNCTION__.29260
00000000 l    d  .rodata.__FUNCTION__.29277	00000000 .rodata.__FUNCTION__.29277
00000000 l     O .rodata.__FUNCTION__.29277	0000001b __FUNCTION__.29277
00000000 l    d  .rodata.ae_dciris_pwm_init.str1.4	00000000 .rodata.ae_dciris_pwm_init.str1.4
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.ae_dciris_pwm_init	000001b0 ae_dciris_pwm_init
00000000         *UND*	00000000 open
00000000         *UND*	00000000 ioctl
00000000         *UND*	00000000 close
00000000         *UND*	00000000 fprintf
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 stderr
00000000 g     F .text.ae_dciris_pwm_exit	00000040 ae_dciris_pwm_exit
00000000 g     F .text.ae_dciris_pwm_update	000000f8 ae_dciris_pwm_update
00000000         *UND*	00000000 __stack_chk_fail
00000000         *UND*	00000000 __stack_chk_guard
00000000 g     F .text.AeDCiris_register_callback	0000010c AeDCiris_register_callback
00000000         *UND*	00000000 strncpy_s
00000000         *UND*	00000000 HI_MPI_AE_IrisRegisterCallBack



Disassembly of section .text.ae_dciris_pwm_init:

00000000 <ae_dciris_pwm_init>:
   0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   4:	e24dd008 	sub	sp, sp, #8
   8:	e59f5170 	ldr	r5, [pc, #368]	; 180 <ae_dciris_pwm_init+0x180>
   c:	e1a04000 	mov	r4, r0
  10:	e59f716c 	ldr	r7, [pc, #364]	; 184 <ae_dciris_pwm_init+0x184>
  14:	e08f5005 	add	r5, pc, r5
  18:	e08f7007 	add	r7, pc, r7
  1c:	e7956100 	ldr	r6, [r5, r0, lsl #2]
  20:	e58d0004 	str	r0, [sp, #4]
  24:	e3760001 	cmn	r6, #1
  28:	0a000016 	beq	88 <ae_dciris_pwm_init+0x88>
  2c:	e59f0154 	ldr	r0, [pc, #340]	; 188 <ae_dciris_pwm_init+0x188>
  30:	e3a02c01 	mov	r2, #256	; 0x100
  34:	e3a01000 	mov	r1, #0
  38:	e08f0000 	add	r0, pc, r0
  3c:	ebfffffe 	bl	0 <open>
			3c: R_ARM_CALL	open
  40:	e59f3144 	ldr	r3, [pc, #324]	; 18c <ae_dciris_pwm_init+0x18c>
  44:	e59d2004 	ldr	r2, [sp, #4]
  48:	e08f3003 	add	r3, pc, r3
  4c:	e7830104 	str	r0, [r3, r4, lsl #2]
  50:	e7933102 	ldr	r3, [r3, r2, lsl #2]
  54:	e3530000 	cmp	r3, #0
  58:	a3a00000 	movge	r0, #0
  5c:	aa000007 	bge	80 <ae_dciris_pwm_init+0x80>
  60:	e59f3128 	ldr	r3, [pc, #296]	; 190 <ae_dciris_pwm_init+0x190>
  64:	e08f3003 	add	r3, pc, r3
  68:	e7d31002 	ldrb	r1, [r3, r2]
  6c:	e3510001 	cmp	r1, #1
  70:	03e00000 	mvneq	r0, #0
  74:	13e00000 	mvnne	r0, #0
  78:	03a01000 	moveq	r1, #0
  7c:	07c31002 	strbeq	r1, [r3, r2]
  80:	e28dd008 	add	sp, sp, #8
  84:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
  88:	e59f0104 	ldr	r0, [pc, #260]	; 194 <ae_dciris_pwm_init+0x194>
  8c:	e3a02c01 	mov	r2, #256	; 0x100
  90:	e59f8100 	ldr	r8, [pc, #256]	; 198 <ae_dciris_pwm_init+0x198>
  94:	e3a01000 	mov	r1, #0
  98:	e08f0000 	add	r0, pc, r0
  9c:	e08f8008 	add	r8, pc, r8
  a0:	ebfffffe 	bl	0 <open>
			a0: R_ARM_CALL	open
  a4:	e59d3004 	ldr	r3, [sp, #4]
  a8:	e7880104 	str	r0, [r8, r4, lsl #2]
  ac:	e7980103 	ldr	r0, [r8, r3, lsl #2]
  b0:	e3500000 	cmp	r0, #0
  b4:	ba000026 	blt	154 <ae_dciris_pwm_init+0x154>
  b8:	e3a01c49 	mov	r1, #18688	; 0x4900
  bc:	e28d2004 	add	r2, sp, #4
  c0:	e3441004 	movt	r1, #16388	; 0x4004
  c4:	ebfffffe 	bl	0 <ioctl>
			c4: R_ARM_CALL	ioctl
  c8:	e59d3004 	ldr	r3, [sp, #4]
  cc:	e3500000 	cmp	r0, #0
  d0:	1a00000b 	bne	104 <ae_dciris_pwm_init+0x104>
  d4:	e7980103 	ldr	r0, [r8, r3, lsl #2]
  d8:	e304192f 	movw	r1, #18735	; 0x492f
  dc:	e3481004 	movt	r1, #32772	; 0x8004
  e0:	e0852103 	add	r2, r5, r3, lsl #2
  e4:	ebfffffe 	bl	0 <ioctl>
			e4: R_ARM_CALL	ioctl
  e8:	e3500000 	cmp	r0, #0
  ec:	1a00000a 	bne	11c <ae_dciris_pwm_init+0x11c>
  f0:	e59d3004 	ldr	r3, [sp, #4]
  f4:	e7980103 	ldr	r0, [r8, r3, lsl #2]
  f8:	ebfffffe 	bl	0 <close>
			f8: R_ARM_CALL	close
  fc:	e59d4004 	ldr	r4, [sp, #4]
 100:	eaffffc9 	b	2c <ae_dciris_pwm_init+0x2c>
 104:	e7980103 	ldr	r0, [r8, r3, lsl #2]
 108:	ebfffffe 	bl	0 <close>
			108: R_ARM_CALL	close
 10c:	e59d3004 	ldr	r3, [sp, #4]
 110:	e1a00006 	mov	r0, r6
 114:	e7886103 	str	r6, [r8, r3, lsl #2]
 118:	eaffffd8 	b	80 <ae_dciris_pwm_init+0x80>
 11c:	e59f1078 	ldr	r1, [pc, #120]	; 19c <ae_dciris_pwm_init+0x19c>
 120:	e3a0303b 	mov	r3, #59	; 0x3b
 124:	e59f2074 	ldr	r2, [pc, #116]	; 1a0 <ae_dciris_pwm_init+0x1a0>
 128:	e7970001 	ldr	r0, [r7, r1]
 12c:	e59f1070 	ldr	r1, [pc, #112]	; 1a4 <ae_dciris_pwm_init+0x1a4>
 130:	e08f2002 	add	r2, pc, r2
 134:	e08f1001 	add	r1, pc, r1
 138:	e5900000 	ldr	r0, [r0]
 13c:	ebfffffe 	bl	0 <fprintf>
			13c: R_ARM_CALL	fprintf
 140:	e59d3004 	ldr	r3, [sp, #4]
 144:	e7980103 	ldr	r0, [r8, r3, lsl #2]
 148:	ebfffffe 	bl	0 <close>
			148: R_ARM_CALL	close
 14c:	e1a00006 	mov	r0, r6
 150:	eaffffca 	b	80 <ae_dciris_pwm_init+0x80>
 154:	e59f1040 	ldr	r1, [pc, #64]	; 19c <ae_dciris_pwm_init+0x19c>
 158:	e3a0302f 	mov	r3, #47	; 0x2f
 15c:	e59f2044 	ldr	r2, [pc, #68]	; 1a8 <ae_dciris_pwm_init+0x1a8>
 160:	e7970001 	ldr	r0, [r7, r1]
 164:	e59f1040 	ldr	r1, [pc, #64]	; 1ac <ae_dciris_pwm_init+0x1ac>
 168:	e08f2002 	add	r2, pc, r2
 16c:	e08f1001 	add	r1, pc, r1
 170:	e5900000 	ldr	r0, [r0]
 174:	ebfffffe 	bl	0 <fprintf>
			174: R_ARM_CALL	fprintf
 178:	e1a00006 	mov	r0, r6
 17c:	eaffffbf 	b	80 <ae_dciris_pwm_init+0x80>
 180:	00000164 	.word	0x00000164
			180: R_ARM_REL32	.data.g_au32PwmNum
 184:	00000164 	.word	0x00000164
			184: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 188:	00000148 	.word	0x00000148
			188: R_ARM_REL32	.LC3
 18c:	0000013c 	.word	0x0000013c
			18c: R_ARM_REL32	.data.g_Pwmfd
 190:	00000124 	.word	0x00000124
			190: R_ARM_REL32	.data.g_au8PWMPrintIndex
 194:	000000f4 	.word	0x000000f4
			194: R_ARM_REL32	.LC0
 198:	000000f4 	.word	0x000000f4
			198: R_ARM_REL32	.data.g_Ispfd
 19c:	00000000 	.word	0x00000000
			19c: R_ARM_GOT32	stderr
 1a0:	00000068 	.word	0x00000068
			1a0: R_ARM_REL32	.rodata.__FUNCTION__.29260
 1a4:	00000068 	.word	0x00000068
			1a4: R_ARM_REL32	.LC2
 1a8:	00000038 	.word	0x00000038
			1a8: R_ARM_REL32	.rodata.__FUNCTION__.29260
 1ac:	00000038 	.word	0x00000038
			1ac: R_ARM_REL32	.LC1

Disassembly of section .text.ae_dciris_pwm_exit:

00000000 <ae_dciris_pwm_exit>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e59f5030 	ldr	r5, [pc, #48]	; 3c <ae_dciris_pwm_exit+0x3c>
   8:	e08f5005 	add	r5, pc, r5
   c:	e7953100 	ldr	r3, [r5, r0, lsl #2]
  10:	e3530000 	cmp	r3, #0
  14:	ba000006 	blt	34 <ae_dciris_pwm_exit+0x34>
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
			3c: R_ARM_REL32	.data.g_Pwmfd

Disassembly of section .text.ae_dciris_pwm_update:

00000000 <ae_dciris_pwm_update>:
   0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   4:	e1a03001 	mov	r3, r1
   8:	e59f60d0 	ldr	r6, [pc, #208]	; e0 <ae_dciris_pwm_update+0xe0>
   c:	e1a05000 	mov	r5, r0
  10:	e59f20cc 	ldr	r2, [pc, #204]	; e4 <ae_dciris_pwm_update+0xe4>
  14:	e24dd018 	sub	sp, sp, #24
  18:	e08f6006 	add	r6, pc, r6
  1c:	e3a01001 	mov	r1, #1
  20:	e59fe0c0 	ldr	lr, [pc, #192]	; e8 <ae_dciris_pwm_update+0xe8>
  24:	e796c100 	ldr	ip, [r6, r0, lsl #2]
  28:	e3a00ffa 	mov	r0, #1000	; 0x3e8
  2c:	e08fe00e 	add	lr, pc, lr
  30:	e153000c 	cmp	r3, ip
  34:	e08f2002 	add	r2, pc, r2
  38:	e59fc0ac 	ldr	ip, [pc, #172]	; ec <ae_dciris_pwm_update+0xec>
  3c:	e7922105 	ldr	r2, [r2, r5, lsl #2]
  40:	e79ec00c 	ldr	ip, [lr, ip]
  44:	e59cc000 	ldr	ip, [ip]
  48:	e58dc014 	str	ip, [sp, #20]
  4c:	e3a0c000 	mov	ip, #0
  50:	e58d000c 	str	r0, [sp, #12]
  54:	03a00000 	moveq	r0, #0
  58:	e58d3008 	str	r3, [sp, #8]
  5c:	e5cd2004 	strb	r2, [sp, #4]
  60:	e5cd1010 	strb	r1, [sp, #16]
  64:	0a00000d 	beq	a0 <ae_dciris_pwm_update+0xa0>
  68:	e59f7080 	ldr	r7, [pc, #128]	; f0 <ae_dciris_pwm_update+0xf0>
  6c:	e3530064 	cmp	r3, #100	; 0x64
  70:	33a03064 	movcc	r3, #100	; 0x64
  74:	e28d2004 	add	r2, sp, #4
  78:	e08f7007 	add	r7, pc, r7
  7c:	e1530000 	cmp	r3, r0
  80:	e7978105 	ldr	r8, [r7, r5, lsl #2]
  84:	31a04003 	movcc	r4, r3
  88:	23a04ffa 	movcs	r4, #1000	; 0x3e8
  8c:	e1a00008 	mov	r0, r8
  90:	ebfffffe 	bl	0 <ioctl>
			90: R_ARM_CALL	ioctl
  94:	e3500000 	cmp	r0, #0
  98:	07864105 	streq	r4, [r6, r5, lsl #2]
  9c:	1a000009 	bne	c8 <ae_dciris_pwm_update+0xc8>
  a0:	e59f204c 	ldr	r2, [pc, #76]	; f4 <ae_dciris_pwm_update+0xf4>
  a4:	e59f3040 	ldr	r3, [pc, #64]	; ec <ae_dciris_pwm_update+0xec>
  a8:	e08f2002 	add	r2, pc, r2
  ac:	e7923003 	ldr	r3, [r2, r3]
  b0:	e5932000 	ldr	r2, [r3]
  b4:	e59d3014 	ldr	r3, [sp, #20]
  b8:	e0332002 	eors	r2, r3, r2
  bc:	1a000006 	bne	dc <ae_dciris_pwm_update+0xdc>
  c0:	e28dd018 	add	sp, sp, #24
  c4:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
  c8:	e1a00008 	mov	r0, r8
  cc:	ebfffffe 	bl	0 <close>
			cc: R_ARM_CALL	close
  d0:	e3e00000 	mvn	r0, #0
  d4:	e7870105 	str	r0, [r7, r5, lsl #2]
  d8:	eafffff0 	b	a0 <ae_dciris_pwm_update+0xa0>
  dc:	ebfffffe 	bl	0 <__stack_chk_fail>
			dc: R_ARM_CALL	__stack_chk_fail
  e0:	000000c0 	.word	0x000000c0
			e0: R_ARM_REL32	.bss.g_au32PreIrisValue
  e4:	000000a8 	.word	0x000000a8
			e4: R_ARM_REL32	.data.g_au32PwmNum
  e8:	000000b4 	.word	0x000000b4
			e8: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  ec:	00000000 	.word	0x00000000
			ec: R_ARM_GOT32	__stack_chk_guard
  f0:	00000070 	.word	0x00000070
			f0: R_ARM_REL32	.data.g_Pwmfd
  f4:	00000044 	.word	0x00000044
			f4: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_

Disassembly of section .text.AeDCiris_register_callback:

00000000 <AeDCiris_register_callback>:
   0:	e92d4030 	push	{r4, r5, lr}
   4:	e24dd034 	sub	sp, sp, #52	; 0x34
   8:	e59fe0d0 	ldr	lr, [pc, #208]	; e0 <AeDCiris_register_callback+0xe0>
   c:	e28d5030 	add	r5, sp, #48	; 0x30
  10:	e1c00fc0 	bic	r0, r0, r0, asr #31
  14:	e3a0300c 	mov	r3, #12
  18:	e08fe00e 	add	lr, pc, lr
  1c:	e6ef0070 	uxtb	r0, r0
  20:	e59fc0bc 	ldr	ip, [pc, #188]	; e4 <AeDCiris_register_callback+0xe4>
  24:	e1a01003 	mov	r1, r3
  28:	e59f20b8 	ldr	r2, [pc, #184]	; e8 <AeDCiris_register_callback+0xe8>
  2c:	e525001c 	str	r0, [r5, #-28]!	; 0xffffffe4
  30:	e28d0018 	add	r0, sp, #24
  34:	e79ec00c 	ldr	ip, [lr, ip]
  38:	e59f40ac 	ldr	r4, [pc, #172]	; ec <AeDCiris_register_callback+0xec>
  3c:	e08f2002 	add	r2, pc, r2
  40:	e08f4004 	add	r4, pc, r4
  44:	e59cc000 	ldr	ip, [ip]
  48:	e58dc02c 	str	ip, [sp, #44]	; 0x2c
  4c:	e3a0c000 	mov	ip, #0
  50:	ebfffffe 	bl	0 <strncpy_s>
			50: R_ARM_CALL	strncpy_s
  54:	e59f3094 	ldr	r3, [pc, #148]	; f0 <AeDCiris_register_callback+0xf0>
  58:	e1a00005 	mov	r0, r5
  5c:	e59f2090 	ldr	r2, [pc, #144]	; f4 <AeDCiris_register_callback+0xf4>
  60:	e1a0100d 	mov	r1, sp
  64:	e794c003 	ldr	ip, [r4, r3]
  68:	e59f3088 	ldr	r3, [pc, #136]	; f8 <AeDCiris_register_callback+0xf8>
  6c:	e58dc000 	str	ip, [sp]
  70:	e7942002 	ldr	r2, [r4, r2]
  74:	e58d2004 	str	r2, [sp, #4]
  78:	e7943003 	ldr	r3, [r4, r3]
  7c:	e58d3008 	str	r3, [sp, #8]
  80:	ebfffffe 	bl	0 <HI_MPI_AE_IrisRegisterCallBack>
			80: R_ARM_CALL	HI_MPI_AE_IrisRegisterCallBack
  84:	e2505000 	subs	r5, r0, #0
  88:	0a000008 	beq	b0 <AeDCiris_register_callback+0xb0>
  8c:	e59f1068 	ldr	r1, [pc, #104]	; fc <AeDCiris_register_callback+0xfc>
  90:	e3a0308c 	mov	r3, #140	; 0x8c
  94:	e59f2064 	ldr	r2, [pc, #100]	; 100 <AeDCiris_register_callback+0x100>
  98:	e7940001 	ldr	r0, [r4, r1]
  9c:	e59f1060 	ldr	r1, [pc, #96]	; 104 <AeDCiris_register_callback+0x104>
  a0:	e08f2002 	add	r2, pc, r2
  a4:	e08f1001 	add	r1, pc, r1
  a8:	e5900000 	ldr	r0, [r0]
  ac:	ebfffffe 	bl	0 <fprintf>
			ac: R_ARM_CALL	fprintf
  b0:	e59f2050 	ldr	r2, [pc, #80]	; 108 <AeDCiris_register_callback+0x108>
  b4:	e1a00005 	mov	r0, r5
  b8:	e59f3024 	ldr	r3, [pc, #36]	; e4 <AeDCiris_register_callback+0xe4>
  bc:	e08f2002 	add	r2, pc, r2
  c0:	e7923003 	ldr	r3, [r2, r3]
  c4:	e5932000 	ldr	r2, [r3]
  c8:	e59d302c 	ldr	r3, [sp, #44]	; 0x2c
  cc:	e0332002 	eors	r2, r3, r2
  d0:	1a000001 	bne	dc <AeDCiris_register_callback+0xdc>
  d4:	e28dd034 	add	sp, sp, #52	; 0x34
  d8:	e8bd8030 	pop	{r4, r5, pc}
  dc:	ebfffffe 	bl	0 <__stack_chk_fail>
			dc: R_ARM_CALL	__stack_chk_fail
  e0:	000000c0 	.word	0x000000c0
			e0: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  e4:	00000000 	.word	0x00000000
			e4: R_ARM_GOT32	__stack_chk_guard
  e8:	000000a4 	.word	0x000000a4
			e8: R_ARM_REL32	.LC4
  ec:	000000a4 	.word	0x000000a4
			ec: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
	...
			f0: R_ARM_GOT32	ae_dciris_pwm_init
			f4: R_ARM_GOT32	ae_dciris_pwm_update
			f8: R_ARM_GOT32	ae_dciris_pwm_exit
			fc: R_ARM_GOT32	stderr
 100:	00000058 	.word	0x00000058
			100: R_ARM_REL32	.rodata.__FUNCTION__.29277
 104:	00000058 	.word	0x00000058
			104: R_ARM_REL32	.LC5
 108:	00000044 	.word	0x00000044
			108: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
