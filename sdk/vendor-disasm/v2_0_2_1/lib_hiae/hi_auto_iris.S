
hi_auto_iris.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l    df *ABS*	00000000 hi_auto_iris.c
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.ae_dc_iris_ctrl_init	00000000 .text.ae_dc_iris_ctrl_init
00000000 l    d  .text.ae_dc_iris_debug	00000000 .text.ae_dc_iris_debug
00000000 l    d  .text.dc_iris_ctrler_process	00000000 .text.dc_iris_ctrler_process
00000000 l       .rodata.dc_iris_ctrler_process.str1.4	00000000 .LC3
00000000 l    d  .text.dc_iris_ctrler_do_cmd	00000000 .text.dc_iris_ctrler_do_cmd
00000000 l    d  .text.ae_dc_iris_auto	00000000 .text.ae_dc_iris_auto
00000000 l    d  .text.ae_dc_iris_manu	00000000 .text.ae_dc_iris_manu
00000000 l    d  .text.ae_dc_iris_exit	00000000 .text.ae_dc_iris_exit
00000000 l    d  .text.ae_iris_ctrl_init	00000000 .text.ae_iris_ctrl_init
00000000 l    d  .text.ae_iris_exit	00000000 .text.ae_iris_exit
00000000 l    d  .text.hi_mpi_ae_iris_register_callback	00000000 .text.hi_mpi_ae_iris_register_callback
00000038 l       .rodata.hi_mpi_ae_iris_register_callback.str1.4	00000000 .LC5
00000044 l       .rodata.hi_mpi_ae_iris_register_callback.str1.4	00000000 .LC6
00000000 l       .rodata.hi_mpi_ae_iris_register_callback.str1.4	00000000 .LC4
00000000 l    d  .text.hi_mpi_ae_iris_unregister_callback	00000000 .text.hi_mpi_ae_iris_unregister_callback
00000000 l    d  .rodata	00000000 .rodata
00000000 l    d  .rodata.__FUNCTION__.30458	00000000 .rodata.__FUNCTION__.30458
00000000 l     O .rodata.__FUNCTION__.30458	00000017 __FUNCTION__.30458
00000000 l    d  .rodata.__FUNCTION__.30468	00000000 .rodata.__FUNCTION__.30468
00000000 l     O .rodata.__FUNCTION__.30468	00000016 __FUNCTION__.30468
00000000 l    d  .rodata.__FUNCTION__.30503	00000000 .rodata.__FUNCTION__.30503
00000000 l     O .rodata.__FUNCTION__.30503	00000021 __FUNCTION__.30503
00000000 l    d  .rodata.__FUNCTION__.30518	00000000 .rodata.__FUNCTION__.30518
00000000 l     O .rodata.__FUNCTION__.30518	00000023 __FUNCTION__.30518
00000000 l    d  .rodata.dc_iris_ctrler_process.str1.4	00000000 .rodata.dc_iris_ctrler_process.str1.4
00000000 l    d  .rodata.hi_mpi_ae_iris_register_callback.str1.4	00000000 .rodata.hi_mpi_ae_iris_register_callback.str1.4
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.ae_dc_iris_ctrl_init	00000108 ae_dc_iris_ctrl_init
00000000         *UND*	00000000 isp_get_ae_ctx
00000000         *UND*	00000000 memcpy
00000000         *UND*	00000000 memcpy_s
00000000         *UND*	00000000 __stack_chk_fail
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 __stack_chk_guard
00000000 g     F .text.ae_dc_iris_debug	00000084 ae_dc_iris_debug
00000000 g     F .text.dc_iris_ctrler_process	0000028c dc_iris_ctrler_process
00000000         *UND*	00000000 fprintf
00000000         *UND*	00000000 stderr
00000000 g     F .text.dc_iris_ctrler_do_cmd	000000ec dc_iris_ctrler_do_cmd
00000000 g     F .text.ae_dc_iris_auto	00000268 ae_dc_iris_auto
00000000 g     F .text.ae_dc_iris_manu	00000044 ae_dc_iris_manu
00000000 g     F .text.ae_dc_iris_exit	00000050 ae_dc_iris_exit
00000000 g     F .text.ae_iris_ctrl_init	00000064 ae_iris_ctrl_init
00000000 g     F .text.ae_iris_exit	00000024 ae_iris_exit
00000000 g     F .text.hi_mpi_ae_iris_register_callback	0000016c hi_mpi_ae_iris_register_callback
00000000         *UND*	00000000 strcmp
00000000 g     F .text.hi_mpi_ae_iris_unregister_callback	00000138 hi_mpi_ae_iris_unregister_callback
00000000         *UND*	00000000 memset_s



Disassembly of section .text.ae_dc_iris_ctrl_init:

00000000 <ae_dc_iris_ctrl_init>:
   0:	e59f20f0 	ldr	r2, [pc, #240]	; f8 <ae_dc_iris_ctrl_init+0xf8>
   4:	e59f30f0 	ldr	r3, [pc, #240]	; fc <ae_dc_iris_ctrl_init+0xfc>
   8:	e08f2002 	add	r2, pc, r2
   c:	e92d4070 	push	{r4, r5, r6, lr}
  10:	e24dd050 	sub	sp, sp, #80	; 0x50
  14:	e7923003 	ldr	r3, [r2, r3]
  18:	e28d6004 	add	r6, sp, #4
  1c:	e5933000 	ldr	r3, [r3]
  20:	e58d304c 	str	r3, [sp, #76]	; 0x4c
  24:	e3a03000 	mov	r3, #0
  28:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			28: R_ARM_CALL	isp_get_ae_ctx
  2c:	e59f10cc 	ldr	r1, [pc, #204]	; 100 <ae_dc_iris_ctrl_init+0x100>
  30:	e1a05000 	mov	r5, r0
  34:	e3a02048 	mov	r2, #72	; 0x48
  38:	e1a00006 	mov	r0, r6
  3c:	e08f1001 	add	r1, pc, r1
  40:	e2854a02 	add	r4, r5, #8192	; 0x2000
  44:	ebfffffe 	bl	0 <memcpy>
			44: R_ARM_CALL	memcpy
  48:	e3043240 	movw	r3, #16960	; 0x4240
  4c:	e300c3b6 	movw	ip, #950	; 0x3b6
  50:	e340300f 	movt	r3, #15
  54:	e2850d82 	add	r0, r5, #8320	; 0x2080
  58:	e5843094 	str	r3, [r4, #148]	; 0x94
  5c:	e3a030fa 	mov	r3, #250	; 0xfa
  60:	e58430f0 	str	r3, [r4, #240]	; 0xf0
  64:	e3a03e32 	mov	r3, #800	; 0x320
  68:	e58430f4 	str	r3, [r4, #244]	; 0xf4
  6c:	e3013b58 	movw	r3, #7000	; 0x1b58
  70:	e5843084 	str	r3, [r4, #132]	; 0x84
  74:	e3003bb8 	movw	r3, #3000	; 0xbb8
  78:	e584308c 	str	r3, [r4, #140]	; 0x8c
  7c:	e3a03001 	mov	r3, #1
  80:	e584307c 	str	r3, [r4, #124]	; 0x7c
  84:	e3a05000 	mov	r5, #0
  88:	e5843088 	str	r3, [r4, #136]	; 0x88
  8c:	e3a03048 	mov	r3, #72	; 0x48
  90:	e5845080 	str	r5, [r4, #128]	; 0x80
  94:	e1a02006 	mov	r2, r6
  98:	e58450f8 	str	r5, [r4, #248]	; 0xf8
  9c:	e1a01003 	mov	r1, r3
  a0:	e58450fc 	str	r5, [r4, #252]	; 0xfc
  a4:	e2800024 	add	r0, r0, #36	; 0x24
  a8:	e5845090 	str	r5, [r4, #144]	; 0x90
  ac:	e5845098 	str	r5, [r4, #152]	; 0x98
  b0:	e5845118 	str	r5, [r4, #280]	; 0x118
  b4:	e5845120 	str	r5, [r4, #288]	; 0x120
  b8:	e584c0ec 	str	ip, [r4, #236]	; 0xec
  bc:	e584c11c 	str	ip, [r4, #284]	; 0x11c
  c0:	ebfffffe 	bl	0 <memcpy_s>
			c0: R_ARM_CALL	memcpy_s
  c4:	e59f2038 	ldr	r2, [pc, #56]	; 104 <ae_dc_iris_ctrl_init+0x104>
  c8:	e59f302c 	ldr	r3, [pc, #44]	; fc <ae_dc_iris_ctrl_init+0xfc>
  cc:	e08f2002 	add	r2, pc, r2
  d0:	e584509c 	str	r5, [r4, #156]	; 0x9c
  d4:	e58450a0 	str	r5, [r4, #160]	; 0xa0
  d8:	e7923003 	ldr	r3, [r2, r3]
  dc:	e5932000 	ldr	r2, [r3]
  e0:	e59d304c 	ldr	r3, [sp, #76]	; 0x4c
  e4:	e0332002 	eors	r2, r3, r2
  e8:	1a000001 	bne	f4 <ae_dc_iris_ctrl_init+0xf4>
  ec:	e28dd050 	add	sp, sp, #80	; 0x50
  f0:	e8bd8070 	pop	{r4, r5, r6, pc}
  f4:	ebfffffe 	bl	0 <__stack_chk_fail>
			f4: R_ARM_CALL	__stack_chk_fail
  f8:	000000e8 	.word	0x000000e8
			f8: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  fc:	00000000 	.word	0x00000000
			fc: R_ARM_GOT32	__stack_chk_guard
 100:	000000bc 	.word	0x000000bc
			100: R_ARM_REL32	.rodata
 104:	00000030 	.word	0x00000030
			104: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_

Disassembly of section .text.ae_dc_iris_debug:

00000000 <ae_dc_iris_debug>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e1a04000 	mov	r4, r0
   8:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			8: R_ARM_CALL	isp_get_ae_ctx
   c:	e2800a02 	add	r0, r0, #8192	; 0x2000
  10:	e5903110 	ldr	r3, [r0, #272]	; 0x110
  14:	e590566c 	ldr	r5, [r0, #1644]	; 0x66c
  18:	e3530002 	cmp	r3, #2
  1c:	0a00000e 	beq	5c <ae_dc_iris_debug+0x5c>
  20:	e3530001 	cmp	r3, #1
  24:	0a000001 	beq	30 <ae_dc_iris_debug+0x30>
  28:	e3a00000 	mov	r0, #0
  2c:	e8bd8070 	pop	{r4, r5, r6, pc}
  30:	e1a00004 	mov	r0, r4
  34:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			34: R_ARM_CALL	isp_get_ae_ctx
  38:	e2800a02 	add	r0, r0, #8192	; 0x2000
  3c:	e590306c 	ldr	r3, [r0, #108]	; 0x6c
  40:	e3530000 	cmp	r3, #0
  44:	0afffff7 	beq	28 <ae_dc_iris_debug+0x28>
  48:	e1a00005 	mov	r0, r5
  4c:	e3a01ffa 	mov	r1, #1000	; 0x3e8
  50:	e12fff33 	blx	r3
  54:	e3a00000 	mov	r0, #0
  58:	e8bd8070 	pop	{r4, r5, r6, pc}
  5c:	e1a00004 	mov	r0, r4
  60:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			60: R_ARM_CALL	isp_get_ae_ctx
  64:	e2800a02 	add	r0, r0, #8192	; 0x2000
  68:	e590306c 	ldr	r3, [r0, #108]	; 0x6c
  6c:	e3530000 	cmp	r3, #0
  70:	0affffec 	beq	28 <ae_dc_iris_debug+0x28>
  74:	e1a00005 	mov	r0, r5
  78:	e3a01064 	mov	r1, #100	; 0x64
  7c:	e12fff33 	blx	r3
  80:	eaffffe8 	b	28 <ae_dc_iris_debug+0x28>

Disassembly of section .text.dc_iris_ctrler_process:

00000000 <dc_iris_ctrler_process>:
   0:	e59fc26c 	ldr	ip, [pc, #620]	; 274 <dc_iris_ctrler_process+0x274>
   4:	e3500000 	cmp	r0, #0
   8:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
   c:	e24dd008 	sub	sp, sp, #8
  10:	e08fc00c 	add	ip, pc, ip
  14:	0a000081 	beq	220 <dc_iris_ctrler_process+0x220>
  18:	e3510000 	cmp	r1, #0
  1c:	0a00008c 	beq	254 <dc_iris_ctrler_process+0x254>
  20:	e5906000 	ldr	r6, [r0]
  24:	e3a03000 	mov	r3, #0
  28:	e5927020 	ldr	r7, [r2, #32]
  2c:	e026efc6 	eor	lr, r6, r6, asr #31
  30:	e5827024 	str	r7, [r2, #36]	; 0x24
  34:	e04eefc6 	sub	lr, lr, r6, asr #31
  38:	e5826020 	str	r6, [r2, #32]
  3c:	e0467007 	sub	r7, r6, r7
  40:	ea000000 	b	48 <dc_iris_ctrler_process+0x48>
  44:	e1a03000 	mov	r3, r0
  48:	e2830001 	add	r0, r3, #1
  4c:	e00c0390 	mul	ip, r0, r3
  50:	e15c000e 	cmp	ip, lr
  54:	3afffffa 	bcc	44 <dc_iris_ctrler_process+0x44>
  58:	e027afc7 	eor	sl, r7, r7, asr #31
  5c:	e356000f 	cmp	r6, #15
  60:	e04aafc7 	sub	sl, sl, r7, asr #31
  64:	d3a0c020 	movle	ip, #32
  68:	da000003 	ble	7c <dc_iris_ctrler_process+0x7c>
  6c:	e356002f 	cmp	r6, #47	; 0x2f
  70:	ca000062 	bgt	200 <dc_iris_ctrler_process+0x200>
  74:	e286c010 	add	ip, r6, #16
  78:	e6ffc07c 	uxth	ip, ip
  7c:	e5920008 	ldr	r0, [r2, #8]
  80:	e28640ff 	add	r4, r6, #255	; 0xff
  84:	e5925010 	ldr	r5, [r2, #16]
  88:	e0000c90 	mul	r0, r0, ip
  8c:	e00c0c95 	mul	ip, r5, ip
  90:	e592800c 	ldr	r8, [r2, #12]
  94:	e280501f 	add	r5, r0, #31
  98:	e3500000 	cmp	r0, #0
  9c:	e28c901f 	add	r9, ip, #31
  a0:	b1a00005 	movlt	r0, r5
  a4:	e35c0000 	cmp	ip, #0
  a8:	e30051fe 	movw	r5, #510	; 0x1fe
  ac:	b1a0c009 	movlt	ip, r9
  b0:	e1540005 	cmp	r4, r5
  b4:	e1a002c0 	asr	r0, r0, #5
  b8:	83a0e000 	movhi	lr, #0
  bc:	e1a0c2cc 	asr	ip, ip, #5
  c0:	9a00003e 	bls	1c0 <dc_iris_ctrler_process+0x1c0>
  c4:	e5925018 	ldr	r5, [r2, #24]
  c8:	e30b4589 	movw	r4, #46473	; 0xb589
  cc:	e34144f8 	movt	r4, #5368	; 0x14f8
  d0:	e3049dd3 	movw	r9, #19923	; 0x4dd3
  d4:	e08ee005 	add	lr, lr, r5
  d8:	e008089e 	mul	r8, lr, r8
  dc:	e0000690 	mul	r0, r0, r6
  e0:	e0c54498 	smull	r4, r5, r8, r4
  e4:	e3419062 	movt	r9, #4194	; 0x1062
  e8:	e1a08fc8 	asr	r8, r8, #31
  ec:	e00c079c 	mul	ip, ip, r7
  f0:	e06886c5 	rsb	r8, r8, r5, asr #13
  f4:	e0c54990 	smull	r4, r5, r0, r9
  f8:	e1a00fc0 	asr	r0, r0, #31
  fc:	e0600345 	rsb	r0, r0, r5, asr #6
 100:	e0c5499c 	smull	r4, r5, ip, r9
 104:	e1a0cfcc 	asr	ip, ip, #31
 108:	e0808008 	add	r8, r0, r8
 10c:	e06cc345 	rsb	ip, ip, r5, asr #6
 110:	e5920074 	ldr	r0, [r2, #116]	; 0x74
 114:	e088800c 	add	r8, r8, ip
 118:	e1580000 	cmp	r8, r0
 11c:	e5826014 	str	r6, [r2, #20]
 120:	e582701c 	str	r7, [r2, #28]
 124:	aa000021 	bge	1b0 <dc_iris_ctrler_process+0x1b0>
 128:	e3044240 	movw	r4, #16960	; 0x4240
 12c:	e308c6a0 	movw	ip, #34464	; 0x86a0
 130:	e340400f 	movt	r4, #15
 134:	e15e0004 	cmp	lr, r4
 138:	e340c001 	movt	ip, #1
 13c:	a1a0e004 	movge	lr, r4
 140:	e15e000c 	cmp	lr, ip
 144:	b1a0e00c 	movlt	lr, ip
 148:	e153000a 	cmp	r3, sl
 14c:	e582e018 	str	lr, [r2, #24]
 150:	8a000005 	bhi	16c <dc_iris_ctrler_process+0x16c>
 154:	e3a03000 	mov	r3, #0
 158:	e58230a4 	str	r3, [r2, #164]	; 0xa4
 15c:	e5810000 	str	r0, [r1]
 160:	e3a00000 	mov	r0, #0
 164:	e28dd008 	add	sp, sp, #8
 168:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
 16c:	e5923078 	ldr	r3, [r2, #120]	; 0x78
 170:	e1530000 	cmp	r3, r0
 174:	8afffff6 	bhi	154 <dc_iris_ctrler_process+0x154>
 178:	eef37a0e 	vmov.f32	s15, #62	; 0x41f00000  30.0
 17c:	ed927a26 	vldr	s14, [r2, #152]	; 0x98
 180:	e59230a4 	ldr	r3, [r2, #164]	; 0xa4
 184:	eeb47ae7 	vcmpe.f32	s14, s15
 188:	e283c001 	add	ip, r3, #1
 18c:	e582c0a4 	str	ip, [r2, #164]	; 0xa4
 190:	eef1fa10 	vmrs	APSR_nzcv, fpscr
 194:	a3a0c019 	movge	ip, #25
 198:	b3a0c032 	movlt	ip, #50	; 0x32
 19c:	e15c0003 	cmp	ip, r3
 1a0:	33a03000 	movcc	r3, #0
 1a4:	358230a4 	strcc	r3, [r2, #164]	; 0xa4
 1a8:	3582307c 	strcc	r3, [r2, #124]	; 0x7c
 1ac:	eaffffea 	b	15c <dc_iris_ctrler_process+0x15c>
 1b0:	e5920070 	ldr	r0, [r2, #112]	; 0x70
 1b4:	e1580000 	cmp	r8, r0
 1b8:	b1a00008 	movlt	r0, r8
 1bc:	eaffffd9 	b	128 <dc_iris_ctrler_process+0x128>
 1c0:	e3560000 	cmp	r6, #0
 1c4:	e20e401f 	and	r4, lr, #31
 1c8:	a3a05009 	movge	r5, #9
 1cc:	b3a05000 	movlt	r5, #0
 1d0:	e085e2ce 	add	lr, r5, lr, asr #5
 1d4:	e082510e 	add	r5, r2, lr, lsl #2
 1d8:	e595e028 	ldr	lr, [r5, #40]	; 0x28
 1dc:	e595502c 	ldr	r5, [r5, #44]	; 0x2c
 1e0:	e045500e 	sub	r5, r5, lr
 1e4:	e0040594 	mul	r4, r4, r5
 1e8:	e284501f 	add	r5, r4, #31
 1ec:	e3540000 	cmp	r4, #0
 1f0:	b1a04005 	movlt	r4, r5
 1f4:	e08ee2c4 	add	lr, lr, r4, asr #5
 1f8:	e00e0e96 	mul	lr, r6, lr
 1fc:	eaffffb0 	b	c4 <dc_iris_ctrler_process+0xc4>
 200:	e356003f 	cmp	r6, #63	; 0x3f
 204:	d246c030 	suble	ip, r6, #48	; 0x30
 208:	c3a0c060 	movgt	ip, #96	; 0x60
 20c:	d1a0c30c 	lslle	ip, ip, #6
 210:	d1a0c2cc 	asrle	ip, ip, #5
 214:	d28cc040 	addle	ip, ip, #64	; 0x40
 218:	d6ffc07c 	uxthle	ip, ip
 21c:	eaffff96 	b	7c <dc_iris_ctrler_process+0x7c>
 220:	e59f1050 	ldr	r1, [pc, #80]	; 278 <dc_iris_ctrler_process+0x278>
 224:	e300310e 	movw	r3, #270	; 0x10e
 228:	e59f204c 	ldr	r2, [pc, #76]	; 27c <dc_iris_ctrler_process+0x27c>
 22c:	e79c0001 	ldr	r0, [ip, r1]
 230:	e59f1048 	ldr	r1, [pc, #72]	; 280 <dc_iris_ctrler_process+0x280>
 234:	e08f2002 	add	r2, pc, r2
 238:	e08f1001 	add	r1, pc, r1
 23c:	e5900000 	ldr	r0, [r0]
 240:	e58d2000 	str	r2, [sp]
 244:	ebfffffe 	bl	0 <fprintf>
			244: R_ARM_CALL	fprintf
 248:	e3080006 	movw	r0, #32774	; 0x8006
 24c:	e34a001c 	movt	r0, #40988	; 0xa01c
 250:	eaffffc3 	b	164 <dc_iris_ctrler_process+0x164>
 254:	e59f101c 	ldr	r1, [pc, #28]	; 278 <dc_iris_ctrler_process+0x278>
 258:	e300310f 	movw	r3, #271	; 0x10f
 25c:	e59f2020 	ldr	r2, [pc, #32]	; 284 <dc_iris_ctrler_process+0x284>
 260:	e79c0001 	ldr	r0, [ip, r1]
 264:	e59f101c 	ldr	r1, [pc, #28]	; 288 <dc_iris_ctrler_process+0x288>
 268:	e08f2002 	add	r2, pc, r2
 26c:	e08f1001 	add	r1, pc, r1
 270:	eafffff1 	b	23c <dc_iris_ctrler_process+0x23c>
 274:	0000025c 	.word	0x0000025c
			274: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 278:	00000000 	.word	0x00000000
			278: R_ARM_GOT32	stderr
 27c:	00000040 	.word	0x00000040
			27c: R_ARM_REL32	.rodata.__FUNCTION__.30458
 280:	00000040 	.word	0x00000040
			280: R_ARM_REL32	.LC3
 284:	00000014 	.word	0x00000014
			284: R_ARM_REL32	.rodata.__FUNCTION__.30458
 288:	00000014 	.word	0x00000014
			288: R_ARM_REL32	.LC3

Disassembly of section .text.dc_iris_ctrler_do_cmd:

00000000 <dc_iris_ctrler_do_cmd>:
   0:	e59fc0d4 	ldr	ip, [pc, #212]	; dc <dc_iris_ctrler_do_cmd+0xdc>
   4:	e3510000 	cmp	r1, #0
   8:	e08fc00c 	add	ip, pc, ip
   c:	0a000022 	beq	9c <dc_iris_ctrler_do_cmd+0x9c>
  10:	e3500001 	cmp	r0, #1
  14:	0a00001c 	beq	8c <dc_iris_ctrler_do_cmd+0x8c>
  18:	e3500002 	cmp	r0, #2
  1c:	1a000018 	bne	84 <dc_iris_ctrler_do_cmd+0x84>
  20:	e5921078 	ldr	r1, [r2, #120]	; 0x78
  24:	e3a03001 	mov	r3, #1
  28:	e582307c 	str	r3, [r2, #124]	; 0x7c
  2c:	e3a03000 	mov	r3, #0
  30:	e5823014 	str	r3, [r2, #20]
  34:	e24130c8 	sub	r3, r1, #200	; 0xc8
  38:	e3530063 	cmp	r3, #99	; 0x63
  3c:	930836a0 	movwls	r3, #34464	; 0x86a0
  40:	93403001 	movtls	r3, #1
  44:	9a000007 	bls	68 <dc_iris_ctrler_do_cmd+0x68>
  48:	e3a03ffa 	mov	r3, #1000	; 0x3e8
  4c:	e3040240 	movw	r0, #16960	; 0x4240
  50:	e0030193 	mul	r3, r3, r1
  54:	e340000f 	movt	r0, #15
  58:	e2433bc3 	sub	r3, r3, #199680	; 0x30c00
  5c:	e2433d05 	sub	r3, r3, #320	; 0x140
  60:	e1530000 	cmp	r3, r0
  64:	21a03000 	movcs	r3, r0
  68:	e5823018 	str	r3, [r2, #24]
  6c:	e3a03000 	mov	r3, #0
  70:	e582301c 	str	r3, [r2, #28]
  74:	e1a00003 	mov	r0, r3
  78:	e5823020 	str	r3, [r2, #32]
  7c:	e5823024 	str	r3, [r2, #36]	; 0x24
  80:	e12fff1e 	bx	lr
  84:	e3a00000 	mov	r0, #0
  88:	e12fff1e 	bx	lr
  8c:	e592307c 	ldr	r3, [r2, #124]	; 0x7c
  90:	e3a00000 	mov	r0, #0
  94:	e5813000 	str	r3, [r1]
  98:	e12fff1e 	bx	lr
  9c:	e59f103c 	ldr	r1, [pc, #60]	; e0 <dc_iris_ctrler_do_cmd+0xe0>
  a0:	e3003149 	movw	r3, #329	; 0x149
  a4:	e59f2038 	ldr	r2, [pc, #56]	; e4 <dc_iris_ctrler_do_cmd+0xe4>
  a8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
  ac:	e24dd00c 	sub	sp, sp, #12
  b0:	e79c0001 	ldr	r0, [ip, r1]
  b4:	e59f102c 	ldr	r1, [pc, #44]	; e8 <dc_iris_ctrler_do_cmd+0xe8>
  b8:	e08f2002 	add	r2, pc, r2
  bc:	e08f1001 	add	r1, pc, r1
  c0:	e5900000 	ldr	r0, [r0]
  c4:	e58d2000 	str	r2, [sp]
  c8:	ebfffffe 	bl	0 <fprintf>
			c8: R_ARM_CALL	fprintf
  cc:	e3080006 	movw	r0, #32774	; 0x8006
  d0:	e34a001c 	movt	r0, #40988	; 0xa01c
  d4:	e28dd00c 	add	sp, sp, #12
  d8:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)
  dc:	000000cc 	.word	0x000000cc
			dc: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  e0:	00000000 	.word	0x00000000
			e0: R_ARM_GOT32	stderr
  e4:	00000024 	.word	0x00000024
			e4: R_ARM_REL32	.rodata.__FUNCTION__.30468
  e8:	00000024 	.word	0x00000024
			e8: R_ARM_REL32	.LC3

Disassembly of section .text.ae_dc_iris_auto:

00000000 <ae_dc_iris_auto>:
   0:	e59f2254 	ldr	r2, [pc, #596]	; 25c <ae_dc_iris_auto+0x25c>
   4:	e59f3254 	ldr	r3, [pc, #596]	; 260 <ae_dc_iris_auto+0x260>
   8:	e08f2002 	add	r2, pc, r2
   c:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
  10:	e24dd014 	sub	sp, sp, #20
  14:	e7923003 	ldr	r3, [r2, r3]
  18:	e1a06000 	mov	r6, r0
  1c:	e5933000 	ldr	r3, [r3]
  20:	e58d300c 	str	r3, [sp, #12]
  24:	e3a03000 	mov	r3, #0
  28:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			28: R_ARM_CALL	isp_get_ae_ctx
  2c:	e1d01ef4 	ldrsh	r1, [r0, #228]	; 0xe4
  30:	e1a04000 	mov	r4, r0
  34:	e5d020cc 	ldrb	r2, [r0, #204]	; 0xcc
  38:	e2805a02 	add	r5, r0, #8192	; 0x2000
  3c:	e1a03181 	lsl	r3, r1, #3
  40:	e0433001 	sub	r3, r3, r1
  44:	e3520000 	cmp	r2, #0
  48:	e59010c4 	ldr	r1, [r0, #196]	; 0xc4
  4c:	03a02001 	moveq	r2, #1
  50:	e1a03183 	lsl	r3, r3, #3
  54:	e5900574 	ldr	r0, [r0, #1396]	; 0x574
  58:	e713f213 	sdiv	r3, r3, r2
  5c:	e5942558 	ldr	r2, [r4, #1368]	; 0x558
  60:	e58d3004 	str	r3, [sp, #4]
  64:	e1500002 	cmp	r0, r2
  68:	e5851114 	str	r1, [r5, #276]	; 0x114
  6c:	e595766c 	ldr	r7, [r5, #1644]	; 0x66c
  70:	8a000003 	bhi	84 <ae_dc_iris_auto+0x84>
  74:	e59425b4 	ldr	r2, [r4, #1460]	; 0x5b4
  78:	e5943598 	ldr	r3, [r4, #1432]	; 0x598
  7c:	e1520003 	cmp	r2, r3
  80:	9a000059 	bls	1ec <ae_dc_iris_auto+0x1ec>
  84:	e3a01000 	mov	r1, #0
  88:	e5943008 	ldr	r3, [r4, #8]
  8c:	e3530000 	cmp	r3, #0
  90:	1a000002 	bne	a0 <ae_dc_iris_auto+0xa0>
  94:	e594300c 	ldr	r3, [r4, #12]
  98:	e3530000 	cmp	r3, #0
  9c:	0a00004c 	beq	1d4 <ae_dc_iris_auto+0x1d4>
  a0:	e5943514 	ldr	r3, [r4, #1300]	; 0x514
  a4:	e5942584 	ldr	r2, [r4, #1412]	; 0x584
  a8:	e1530002 	cmp	r3, r2
  ac:	83a03000 	movhi	r3, #0
  b0:	93a03001 	movls	r3, #1
  b4:	e595207c 	ldr	r2, [r5, #124]	; 0x7c
  b8:	e3520000 	cmp	r2, #0
  bc:	0a000022 	beq	14c <ae_dc_iris_auto+0x14c>
  c0:	e0118003 	ands	r8, r1, r3
  c4:	1a000029 	bne	170 <ae_dc_iris_auto+0x170>
  c8:	e58580fc 	str	r8, [r5, #252]	; 0xfc
  cc:	e1a00006 	mov	r0, r6
  d0:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			d0: R_ARM_CALL	isp_get_ae_ctx
  d4:	e59530fc 	ldr	r3, [r5, #252]	; 0xfc
  d8:	e2800a02 	add	r0, r0, #8192	; 0x2000
  dc:	e30023b6 	movw	r2, #950	; 0x3b6
  e0:	e5808118 	str	r8, [r0, #280]	; 0x118
  e4:	e580211c 	str	r2, [r0, #284]	; 0x11c
  e8:	e2530001 	subs	r0, r3, #1
  ec:	13a00001 	movne	r0, #1
  f0:	e5840474 	str	r0, [r4, #1140]	; 0x474
  f4:	e1a00006 	mov	r0, r6
  f8:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			f8: R_ARM_CALL	isp_get_ae_ctx
  fc:	e2800a02 	add	r0, r0, #8192	; 0x2000
 100:	e590306c 	ldr	r3, [r0, #108]	; 0x6c
 104:	e3530000 	cmp	r3, #0
 108:	0a000002 	beq	118 <ae_dc_iris_auto+0x118>
 10c:	e595111c 	ldr	r1, [r5, #284]	; 0x11c
 110:	e1a00007 	mov	r0, r7
 114:	e12fff33 	blx	r3
 118:	e595207c 	ldr	r2, [r5, #124]	; 0x7c
 11c:	e5852080 	str	r2, [r5, #128]	; 0x80
 120:	e3a00000 	mov	r0, #0
 124:	e59f2138 	ldr	r2, [pc, #312]	; 264 <ae_dc_iris_auto+0x264>
 128:	e59f3130 	ldr	r3, [pc, #304]	; 260 <ae_dc_iris_auto+0x260>
 12c:	e08f2002 	add	r2, pc, r2
 130:	e7923003 	ldr	r3, [r2, r3]
 134:	e5932000 	ldr	r2, [r3]
 138:	e59d300c 	ldr	r3, [sp, #12]
 13c:	e0332002 	eors	r2, r3, r2
 140:	1a000044 	bne	258 <ae_dc_iris_auto+0x258>
 144:	e28dd014 	add	sp, sp, #20
 148:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
 14c:	e5953080 	ldr	r3, [r5, #128]	; 0x80
 150:	e3530001 	cmp	r3, #1
 154:	1afffff0 	bne	11c <ae_dc_iris_auto+0x11c>
 158:	e58520fc 	str	r2, [r5, #252]	; 0xfc
 15c:	e3a02d0f 	mov	r2, #960	; 0x3c0
 160:	e585211c 	str	r2, [r5, #284]	; 0x11c
 164:	e1a00006 	mov	r0, r6
 168:	e5843474 	str	r3, [r4, #1140]	; 0x474
 16c:	eaffffe1 	b	f8 <ae_dc_iris_auto+0xf8>
 170:	e59580fc 	ldr	r8, [r5, #252]	; 0xfc
 174:	e3580000 	cmp	r8, #0
 178:	0a000021 	beq	204 <ae_dc_iris_auto+0x204>
 17c:	e3580001 	cmp	r8, #1
 180:	13a00001 	movne	r0, #1
 184:	1affffd9 	bne	f0 <ae_dc_iris_auto+0xf0>
 188:	e2849d81 	add	r9, r4, #8256	; 0x2040
 18c:	e28d0004 	add	r0, sp, #4
 190:	e289903c 	add	r9, r9, #60	; 0x3c
 194:	e1a02009 	mov	r2, r9
 198:	e28910a0 	add	r1, r9, #160	; 0xa0
 19c:	ebfffffe 	bl	0 <ae_dc_iris_auto>
			19c: R_ARM_CALL	dc_iris_ctrler_process
 1a0:	e1a02009 	mov	r2, r9
 1a4:	e28d1008 	add	r1, sp, #8
 1a8:	e1a00008 	mov	r0, r8
 1ac:	ebfffffe 	bl	0 <ae_dc_iris_auto>
			1ac: R_ARM_CALL	dc_iris_ctrler_do_cmd
 1b0:	e59d3008 	ldr	r3, [sp, #8]
 1b4:	e3530000 	cmp	r3, #0
 1b8:	01a00008 	moveq	r0, r8
 1bc:	0a000002 	beq	1cc <ae_dc_iris_auto+0x1cc>
 1c0:	e59530fc 	ldr	r3, [r5, #252]	; 0xfc
 1c4:	e2530001 	subs	r0, r3, #1
 1c8:	13a00001 	movne	r0, #1
 1cc:	e58530fc 	str	r3, [r5, #252]	; 0xfc
 1d0:	eaffffc6 	b	f0 <ae_dc_iris_auto+0xf0>
 1d4:	e5943510 	ldr	r3, [r4, #1296]	; 0x510
 1d8:	e5942584 	ldr	r2, [r4, #1412]	; 0x584
 1dc:	e1530002 	cmp	r3, r2
 1e0:	83a03000 	movhi	r3, #0
 1e4:	93a03001 	movls	r3, #1
 1e8:	eaffffb1 	b	b4 <ae_dc_iris_auto+0xb4>
 1ec:	e59425cc 	ldr	r2, [r4, #1484]	; 0x5cc
 1f0:	e59435d8 	ldr	r3, [r4, #1496]	; 0x5d8
 1f4:	e1530082 	cmp	r3, r2, lsl #1
 1f8:	93a01001 	movls	r1, #1
 1fc:	83a01000 	movhi	r1, #0
 200:	eaffffa0 	b	88 <ae_dc_iris_auto+0x88>
 204:	e1a00006 	mov	r0, r6
 208:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			208: R_ARM_CALL	isp_get_ae_ctx
 20c:	e2801a02 	add	r1, r0, #8192	; 0x2000
 210:	e5913118 	ldr	r3, [r1, #280]	; 0x118
 214:	e3530005 	cmp	r3, #5
 218:	e2833001 	add	r3, r3, #1
 21c:	e5813118 	str	r3, [r1, #280]	; 0x118
 220:	8a000003 	bhi	234 <ae_dc_iris_auto+0x234>
 224:	e59500fc 	ldr	r0, [r5, #252]	; 0xfc
 228:	e2500001 	subs	r0, r0, #1
 22c:	13a00001 	movne	r0, #1
 230:	eaffffae 	b	f0 <ae_dc_iris_auto+0xf0>
 234:	e5818118 	str	r8, [r1, #280]	; 0x118
 238:	e2802d81 	add	r2, r0, #8256	; 0x2040
 23c:	e3a03001 	mov	r3, #1
 240:	e282203c 	add	r2, r2, #60	; 0x3c
 244:	e58130fc 	str	r3, [r1, #252]	; 0xfc
 248:	e3a00002 	mov	r0, #2
 24c:	e28d1008 	add	r1, sp, #8
 250:	ebfffffe 	bl	0 <ae_dc_iris_auto>
			250: R_ARM_CALL	dc_iris_ctrler_do_cmd
 254:	eafffff2 	b	224 <ae_dc_iris_auto+0x224>
 258:	ebfffffe 	bl	0 <__stack_chk_fail>
			258: R_ARM_CALL	__stack_chk_fail
 25c:	0000024c 	.word	0x0000024c
			25c: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 260:	00000000 	.word	0x00000000
			260: R_ARM_GOT32	__stack_chk_guard
 264:	00000130 	.word	0x00000130
			264: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_

Disassembly of section .text.ae_dc_iris_manu:

00000000 <ae_dc_iris_manu>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e1a05000 	mov	r5, r0
   8:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			8: R_ARM_CALL	isp_get_ae_ctx
   c:	e2804a02 	add	r4, r0, #8192	; 0x2000
  10:	e1a00005 	mov	r0, r5
  14:	e594566c 	ldr	r5, [r4, #1644]	; 0x66c
  18:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			18: R_ARM_CALL	isp_get_ae_ctx
  1c:	e2800a02 	add	r0, r0, #8192	; 0x2000
  20:	e590306c 	ldr	r3, [r0, #108]	; 0x6c
  24:	e3530000 	cmp	r3, #0
  28:	0a000003 	beq	3c <ae_dc_iris_manu+0x3c>
  2c:	e2844f43 	add	r4, r4, #268	; 0x10c
  30:	e1a00005 	mov	r0, r5
  34:	e1d410b0 	ldrh	r1, [r4]
  38:	e12fff33 	blx	r3
  3c:	e3a00000 	mov	r0, #0
  40:	e8bd8070 	pop	{r4, r5, r6, pc}

Disassembly of section .text.ae_dc_iris_exit:

00000000 <ae_dc_iris_exit>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e1a05000 	mov	r5, r0
   8:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			8: R_ARM_CALL	isp_get_ae_ctx
   c:	e2804a02 	add	r4, r0, #8192	; 0x2000
  10:	e1a00005 	mov	r0, r5
  14:	e594566c 	ldr	r5, [r4, #1644]	; 0x66c
  18:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			18: R_ARM_CALL	isp_get_ae_ctx
  1c:	e2800a02 	add	r0, r0, #8192	; 0x2000
  20:	e590306c 	ldr	r3, [r0, #108]	; 0x6c
  24:	e3530000 	cmp	r3, #0
  28:	0a000002 	beq	38 <ae_dc_iris_exit+0x38>
  2c:	e3a01000 	mov	r1, #0
  30:	e1a00005 	mov	r0, r5
  34:	e12fff33 	blx	r3
  38:	e5943070 	ldr	r3, [r4, #112]	; 0x70
  3c:	e3530000 	cmp	r3, #0
  40:	08bd8070 	popeq	{r4, r5, r6, pc}
  44:	e1a00005 	mov	r0, r5
  48:	e8bd4070 	pop	{r4, r5, r6, lr}
  4c:	e12fff13 	bx	r3

Disassembly of section .text.ae_iris_ctrl_init:

00000000 <ae_iris_ctrl_init>:
   0:	e92d4010 	push	{r4, lr}
   4:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			4: R_ARM_CALL	isp_get_ae_ctx
   8:	e2802a01 	add	r2, r0, #4096	; 0x1000
   c:	e2803b05 	add	r3, r0, #5120	; 0x1400
  10:	e300c336 	movw	ip, #822	; 0x336
  14:	e2800a02 	add	r0, r0, #8192	; 0x2000
  18:	e5921754 	ldr	r1, [r2, #1876]	; 0x754
  1c:	e592e758 	ldr	lr, [r2, #1880]	; 0x758
  20:	e2832fd7 	add	r2, r3, #860	; 0x35c
  24:	e2833028 	add	r3, r3, #40	; 0x28
  28:	e1d220b0 	ldrh	r2, [r2]
  2c:	e193c0bc 	ldrh	ip, [r3, ip]
  30:	e2803f49 	add	r3, r0, #292	; 0x124
  34:	e5801128 	str	r1, [r0, #296]	; 0x128
  38:	e3a01001 	mov	r1, #1
  3c:	e580e124 	str	lr, [r0, #292]	; 0x124
  40:	e1c320b8 	strh	r2, [r3, #8]
  44:	e3a0200a 	mov	r2, #10
  48:	e1c3c0ba 	strh	ip, [r3, #10]
  4c:	e3a03000 	mov	r3, #0
  50:	e5801130 	str	r1, [r0, #304]	; 0x130
  54:	e5802138 	str	r2, [r0, #312]	; 0x138
  58:	e5803134 	str	r3, [r0, #308]	; 0x134
  5c:	e5803144 	str	r3, [r0, #324]	; 0x144
  60:	e8bd8010 	pop	{r4, pc}

Disassembly of section .text.ae_iris_exit:

00000000 <ae_iris_exit>:
   0:	e92d4010 	push	{r4, lr}
   4:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			4: R_ARM_CALL	isp_get_ae_ctx
   8:	e2800a02 	add	r0, r0, #8192	; 0x2000
   c:	e5903078 	ldr	r3, [r0, #120]	; 0x78
  10:	e3530000 	cmp	r3, #0
  14:	08bd8010 	popeq	{r4, pc}
  18:	e590066c 	ldr	r0, [r0, #1644]	; 0x66c
  1c:	e8bd4010 	pop	{r4, lr}
  20:	e12fff13 	bx	r3

Disassembly of section .text.hi_mpi_ae_iris_register_callback:

00000000 <hi_mpi_ae_iris_register_callback>:
   0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   4:	e3500000 	cmp	r0, #0
   8:	e59f8130 	ldr	r8, [pc, #304]	; 140 <hi_mpi_ae_iris_register_callback+0x140>
   c:	e24dd008 	sub	sp, sp, #8
  10:	e08f8008 	add	r8, pc, r8
  14:	0a000041 	beq	120 <hi_mpi_ae_iris_register_callback+0x120>
  18:	e3510000 	cmp	r1, #0
  1c:	e1a07001 	mov	r7, r1
  20:	0a000030 	beq	e8 <hi_mpi_ae_iris_register_callback+0xe8>
  24:	e5906000 	ldr	r6, [r0]
  28:	e3560003 	cmp	r6, #3
  2c:	8a00001f 	bhi	b0 <hi_mpi_ae_iris_register_callback+0xb0>
  30:	e59f110c 	ldr	r1, [pc, #268]	; 144 <hi_mpi_ae_iris_register_callback+0x144>
  34:	e2805004 	add	r5, r0, #4
  38:	e1a00005 	mov	r0, r5
  3c:	e08f1001 	add	r1, pc, r1
  40:	ebfffffe 	bl	0 <strcmp>
			40: R_ARM_CALL	strcmp
  44:	e2504000 	subs	r4, r0, #0
  48:	1a00000a 	bne	78 <hi_mpi_ae_iris_register_callback+0x78>
  4c:	e1a00006 	mov	r0, r6
  50:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			50: R_ARM_CALL	isp_get_ae_ctx
  54:	e2800d81 	add	r0, r0, #8256	; 0x2040
  58:	e3a03014 	mov	r3, #20
  5c:	e2800028 	add	r0, r0, #40	; 0x28
  60:	e1a02007 	mov	r2, r7
  64:	e1a01003 	mov	r1, r3
  68:	ebfffffe 	bl	0 <memcpy_s>
			68: R_ARM_CALL	memcpy_s
  6c:	e1a00004 	mov	r0, r4
  70:	e28dd008 	add	sp, sp, #8
  74:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
  78:	e59f20c8 	ldr	r2, [pc, #200]	; 148 <hi_mpi_ae_iris_register_callback+0x148>
  7c:	e30031a6 	movw	r3, #422	; 0x1a6
  80:	e59fc0c4 	ldr	ip, [pc, #196]	; 14c <hi_mpi_ae_iris_register_callback+0x14c>
  84:	e7980002 	ldr	r0, [r8, r2]
  88:	e59f10c0 	ldr	r1, [pc, #192]	; 150 <hi_mpi_ae_iris_register_callback+0x150>
  8c:	e08fc00c 	add	ip, pc, ip
  90:	e5900000 	ldr	r0, [r0]
  94:	e1a0200c 	mov	r2, ip
  98:	e08f1001 	add	r1, pc, r1
  9c:	e88d1020 	stm	sp, {r5, ip}
  a0:	ebfffffe 	bl	0 <fprintf>
			a0: R_ARM_CALL	fprintf
  a4:	e3080003 	movw	r0, #32771	; 0x8003
  a8:	e34a001c 	movt	r0, #40988	; 0xa01c
  ac:	eaffffef 	b	70 <hi_mpi_ae_iris_register_callback+0x70>
  b0:	e59f2090 	ldr	r2, [pc, #144]	; 148 <hi_mpi_ae_iris_register_callback+0x148>
  b4:	e3a03f69 	mov	r3, #420	; 0x1a4
  b8:	e59fc094 	ldr	ip, [pc, #148]	; 154 <hi_mpi_ae_iris_register_callback+0x154>
  bc:	e7980002 	ldr	r0, [r8, r2]
  c0:	e59f1090 	ldr	r1, [pc, #144]	; 158 <hi_mpi_ae_iris_register_callback+0x158>
  c4:	e08fc00c 	add	ip, pc, ip
  c8:	e5900000 	ldr	r0, [r0]
  cc:	e1a0200c 	mov	r2, ip
  d0:	e08f1001 	add	r1, pc, r1
  d4:	e88d1040 	stm	sp, {r6, ip}
  d8:	ebfffffe 	bl	0 <fprintf>
			d8: R_ARM_CALL	fprintf
  dc:	e3080003 	movw	r0, #32771	; 0x8003
  e0:	e34a001c 	movt	r0, #40988	; 0xa01c
  e4:	eaffffe1 	b	70 <hi_mpi_ae_iris_register_callback+0x70>
  e8:	e59f2058 	ldr	r2, [pc, #88]	; 148 <hi_mpi_ae_iris_register_callback+0x148>
  ec:	e30031a1 	movw	r3, #417	; 0x1a1
  f0:	e59fc064 	ldr	ip, [pc, #100]	; 15c <hi_mpi_ae_iris_register_callback+0x15c>
  f4:	e59f1064 	ldr	r1, [pc, #100]	; 160 <hi_mpi_ae_iris_register_callback+0x160>
  f8:	e08fc00c 	add	ip, pc, ip
  fc:	e7980002 	ldr	r0, [r8, r2]
 100:	e08f1001 	add	r1, pc, r1
 104:	e5900000 	ldr	r0, [r0]
 108:	e1a0200c 	mov	r2, ip
 10c:	e58dc000 	str	ip, [sp]
 110:	ebfffffe 	bl	0 <fprintf>
			110: R_ARM_CALL	fprintf
 114:	e3080006 	movw	r0, #32774	; 0x8006
 118:	e34a001c 	movt	r0, #40988	; 0xa01c
 11c:	eaffffd3 	b	70 <hi_mpi_ae_iris_register_callback+0x70>
 120:	e59f2020 	ldr	r2, [pc, #32]	; 148 <hi_mpi_ae_iris_register_callback+0x148>
 124:	e3a03e1a 	mov	r3, #416	; 0x1a0
 128:	e59fc034 	ldr	ip, [pc, #52]	; 164 <hi_mpi_ae_iris_register_callback+0x164>
 12c:	e59f1034 	ldr	r1, [pc, #52]	; 168 <hi_mpi_ae_iris_register_callback+0x168>
 130:	e08fc00c 	add	ip, pc, ip
 134:	e7980002 	ldr	r0, [r8, r2]
 138:	e08f1001 	add	r1, pc, r1
 13c:	eafffff0 	b	104 <hi_mpi_ae_iris_register_callback+0x104>
 140:	00000128 	.word	0x00000128
			140: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 144:	00000100 	.word	0x00000100
			144: R_ARM_REL32	.LC5
 148:	00000000 	.word	0x00000000
			148: R_ARM_GOT32	stderr
 14c:	000000b8 	.word	0x000000b8
			14c: R_ARM_REL32	.rodata.__FUNCTION__.30503
 150:	000000b0 	.word	0x000000b0
			150: R_ARM_REL32	.LC6
 154:	00000088 	.word	0x00000088
			154: R_ARM_REL32	.rodata.__FUNCTION__.30503
 158:	00000080 	.word	0x00000080
			158: R_ARM_REL32	.LC4
 15c:	0000005c 	.word	0x0000005c
			15c: R_ARM_REL32	.rodata.__FUNCTION__.30503
 160:	00000058 	.word	0x00000058
			160: R_ARM_REL32	.LC3
 164:	0000002c 	.word	0x0000002c
			164: R_ARM_REL32	.rodata.__FUNCTION__.30503
 168:	00000028 	.word	0x00000028
			168: R_ARM_REL32	.LC3

Disassembly of section .text.hi_mpi_ae_iris_unregister_callback:

00000000 <hi_mpi_ae_iris_unregister_callback>:
   0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   4:	e3500000 	cmp	r0, #0
   8:	e59f7104 	ldr	r7, [pc, #260]	; 114 <hi_mpi_ae_iris_unregister_callback+0x114>
   c:	e24dd00c 	sub	sp, sp, #12
  10:	e08f7007 	add	r7, pc, r7
  14:	0a000030 	beq	dc <hi_mpi_ae_iris_unregister_callback+0xdc>
  18:	e5906000 	ldr	r6, [r0]
  1c:	e3560003 	cmp	r6, #3
  20:	8a00001f 	bhi	a4 <hi_mpi_ae_iris_unregister_callback+0xa4>
  24:	e59f10ec 	ldr	r1, [pc, #236]	; 118 <hi_mpi_ae_iris_unregister_callback+0x118>
  28:	e2805004 	add	r5, r0, #4
  2c:	e1a00005 	mov	r0, r5
  30:	e08f1001 	add	r1, pc, r1
  34:	ebfffffe 	bl	0 <strcmp>
			34: R_ARM_CALL	strcmp
  38:	e2504000 	subs	r4, r0, #0
  3c:	1a00000a 	bne	6c <hi_mpi_ae_iris_unregister_callback+0x6c>
  40:	e1a00006 	mov	r0, r6
  44:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			44: R_ARM_CALL	isp_get_ae_ctx
  48:	e2800d81 	add	r0, r0, #8256	; 0x2040
  4c:	e3a03014 	mov	r3, #20
  50:	e2800028 	add	r0, r0, #40	; 0x28
  54:	e1a01003 	mov	r1, r3
  58:	e1a02004 	mov	r2, r4
  5c:	ebfffffe 	bl	0 <memset_s>
			5c: R_ARM_CALL	memset_s
  60:	e1a00004 	mov	r0, r4
  64:	e28dd00c 	add	sp, sp, #12
  68:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
  6c:	e59f20a8 	ldr	r2, [pc, #168]	; 11c <hi_mpi_ae_iris_unregister_callback+0x11c>
  70:	e30031b9 	movw	r3, #441	; 0x1b9
  74:	e59fc0a4 	ldr	ip, [pc, #164]	; 120 <hi_mpi_ae_iris_unregister_callback+0x120>
  78:	e7970002 	ldr	r0, [r7, r2]
  7c:	e59f10a0 	ldr	r1, [pc, #160]	; 124 <hi_mpi_ae_iris_unregister_callback+0x124>
  80:	e08fc00c 	add	ip, pc, ip
  84:	e5900000 	ldr	r0, [r0]
  88:	e1a0200c 	mov	r2, ip
  8c:	e08f1001 	add	r1, pc, r1
  90:	e88d1020 	stm	sp, {r5, ip}
  94:	ebfffffe 	bl	0 <fprintf>
			94: R_ARM_CALL	fprintf
  98:	e3080003 	movw	r0, #32771	; 0x8003
  9c:	e34a001c 	movt	r0, #40988	; 0xa01c
  a0:	eaffffef 	b	64 <hi_mpi_ae_iris_unregister_callback+0x64>
  a4:	e59f2070 	ldr	r2, [pc, #112]	; 11c <hi_mpi_ae_iris_unregister_callback+0x11c>
  a8:	e3a03f6e 	mov	r3, #440	; 0x1b8
  ac:	e59fc074 	ldr	ip, [pc, #116]	; 128 <hi_mpi_ae_iris_unregister_callback+0x128>
  b0:	e7970002 	ldr	r0, [r7, r2]
  b4:	e59f1070 	ldr	r1, [pc, #112]	; 12c <hi_mpi_ae_iris_unregister_callback+0x12c>
  b8:	e08fc00c 	add	ip, pc, ip
  bc:	e5900000 	ldr	r0, [r0]
  c0:	e1a0200c 	mov	r2, ip
  c4:	e08f1001 	add	r1, pc, r1
  c8:	e88d1040 	stm	sp, {r6, ip}
  cc:	ebfffffe 	bl	0 <fprintf>
			cc: R_ARM_CALL	fprintf
  d0:	e3080003 	movw	r0, #32771	; 0x8003
  d4:	e34a001c 	movt	r0, #40988	; 0xa01c
  d8:	eaffffe1 	b	64 <hi_mpi_ae_iris_unregister_callback+0x64>
  dc:	e59f2038 	ldr	r2, [pc, #56]	; 11c <hi_mpi_ae_iris_unregister_callback+0x11c>
  e0:	e30031b5 	movw	r3, #437	; 0x1b5
  e4:	e59fc044 	ldr	ip, [pc, #68]	; 130 <hi_mpi_ae_iris_unregister_callback+0x130>
  e8:	e7970002 	ldr	r0, [r7, r2]
  ec:	e59f1040 	ldr	r1, [pc, #64]	; 134 <hi_mpi_ae_iris_unregister_callback+0x134>
  f0:	e08fc00c 	add	ip, pc, ip
  f4:	e5900000 	ldr	r0, [r0]
  f8:	e1a0200c 	mov	r2, ip
  fc:	e08f1001 	add	r1, pc, r1
 100:	e58dc000 	str	ip, [sp]
 104:	ebfffffe 	bl	0 <fprintf>
			104: R_ARM_CALL	fprintf
 108:	e3080006 	movw	r0, #32774	; 0x8006
 10c:	e34a001c 	movt	r0, #40988	; 0xa01c
 110:	eaffffd3 	b	64 <hi_mpi_ae_iris_unregister_callback+0x64>
 114:	000000fc 	.word	0x000000fc
			114: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 118:	000000e0 	.word	0x000000e0
			118: R_ARM_REL32	.LC5
 11c:	00000000 	.word	0x00000000
			11c: R_ARM_GOT32	stderr
 120:	00000098 	.word	0x00000098
			120: R_ARM_REL32	.rodata.__FUNCTION__.30518
 124:	00000090 	.word	0x00000090
			124: R_ARM_REL32	.LC6
 128:	00000068 	.word	0x00000068
			128: R_ARM_REL32	.rodata.__FUNCTION__.30518
 12c:	00000060 	.word	0x00000060
			12c: R_ARM_REL32	.LC4
 130:	00000038 	.word	0x00000038
			130: R_ARM_REL32	.rodata.__FUNCTION__.30518
 134:	00000030 	.word	0x00000030
			134: R_ARM_REL32	.LC3
