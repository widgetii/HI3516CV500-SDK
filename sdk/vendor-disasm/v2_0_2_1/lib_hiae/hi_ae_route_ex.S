
hi_ae_route_ex.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l    df *ABS*	00000000 hi_ae_route_ex.c
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.ae_route_ex_put_busy	00000000 .text.ae_route_ex_put_busy
00000000 l    d  .text.ae_route_ex_put_busy_by_pos	00000000 .text.ae_route_ex_put_busy_by_pos
00000000 l    d  .text.ae_route_ex_get_free	00000000 .text.ae_route_ex_get_free
00000000 l       .rodata.ae_route_ex_get_free.str1.4	00000000 .LC0
00000000 l    d  .text.ae_route_ex_put_free	00000000 .text.ae_route_ex_put_free
00000000 l    d  .text.ae_route_ex_node_multi_check	00000000 .text.ae_route_ex_node_multi_check
00000000 l    d  .text.ae_route_ex_node_recurit	00000000 .text.ae_route_ex_node_recurit
00000000 l    d  .text.ae_route_ex_node_stgy	00000000 .text.ae_route_ex_node_stgy
00000000 l    d  .text.ae_route_ex_init	00000000 .text.ae_route_ex_init
00000000 l    d  .text.ae_route_sf_ex_update	00000000 .text.ae_route_sf_ex_update
00000000 l    d  .text.ae_route_ex_update	00000000 .text.ae_route_ex_update
00000000 l    d  .text.ae_route_ex_limit_core	00000000 .text.ae_route_ex_limit_core
00000000 l    d  .text.ae_route_ex_get_joint	00000000 .text.ae_route_ex_get_joint
00000000 l    d  .text.ae_route_ex_get_up_node	00000000 .text.ae_route_ex_get_up_node
00000000 l    d  .text.ae_route_ex_get_dw_node	00000000 .text.ae_route_ex_get_dw_node
00000000 l    d  .text.ae_route_ex_get_first_node	00000000 .text.ae_route_ex_get_first_node
00000000 l    d  .text.ae_route_ex_print	00000000 .text.ae_route_ex_print
00000000 l       .rodata.ae_route_ex_print.str1.4	00000000 .LC1
00000000 l    d  .text.ae_route_ex_del_rdcy	00000000 .text.ae_route_ex_del_rdcy
00000000 l    d  .text.ae_route_ex_limit	00000000 .text.ae_route_ex_limit
00000000 l    d  .rodata.__FUNCTION__.29198	00000000 .rodata.__FUNCTION__.29198
00000000 l     O .rodata.__FUNCTION__.29198	00000015 __FUNCTION__.29198
00000000 l    d  .rodata.ae_route_ex_get_free.str1.4	00000000 .rodata.ae_route_ex_get_free.str1.4
00000000 l    d  .rodata.ae_route_ex_print.str1.4	00000000 .rodata.ae_route_ex_print.str1.4
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.ae_route_ex_put_busy	00000030 ae_route_ex_put_busy
00000000 g     F .text.ae_route_ex_put_busy_by_pos	00000030 ae_route_ex_put_busy_by_pos
00000000 g     F .text.ae_route_ex_get_free	00000080 ae_route_ex_get_free
00000000         *UND*	00000000 fprintf
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 stderr
00000000 g     F .text.ae_route_ex_put_free	00000030 ae_route_ex_put_free
00000000 g     F .text.ae_route_ex_node_multi_check	00000168 ae_route_ex_node_multi_check
00000000         *UND*	00000000 ae_ratio_calculate
00000000         *UND*	00000000 ae_boundaries_check
00000000 g     F .text.ae_route_ex_node_recurit	00000194 ae_route_ex_node_recurit
00000000 g     F .text.ae_route_ex_node_stgy	0000008c ae_route_ex_node_stgy
00000000 g     F .text.ae_route_ex_init	00000064 ae_route_ex_init
00000000 g     F .text.ae_route_sf_ex_update	0000017c ae_route_sf_ex_update
00000000         *UND*	00000000 isp_get_ae_ctx
00000000 g     F .text.ae_route_ex_update	00000170 ae_route_ex_update
00000000 g     F .text.ae_route_ex_limit_core	000001a4 ae_route_ex_limit_core
00000000         *UND*	00000000 __stack_chk_fail
00000000         *UND*	00000000 __stack_chk_guard
00000000 g     F .text.ae_route_ex_get_joint	0000008c ae_route_ex_get_joint
00000000 g     F .text.ae_route_ex_get_up_node	00000018 ae_route_ex_get_up_node
00000000 g     F .text.ae_route_ex_get_dw_node	00000018 ae_route_ex_get_dw_node
00000000 g     F .text.ae_route_ex_get_first_node	00000014 ae_route_ex_get_first_node
00000000 g     F .text.ae_route_ex_print	000000c4 ae_route_ex_print
00000000         *UND*	00000000 printf
00000000 g     F .text.ae_route_ex_del_rdcy	0000014c ae_route_ex_del_rdcy
00000000 g     F .text.ae_route_ex_limit	000000c8 ae_route_ex_limit



Disassembly of section .text.ae_route_ex_put_busy:

00000000 <ae_route_ex_put_busy>:
   0:	e5903300 	ldr	r3, [r0, #768]	; 0x300
   4:	e281c028 	add	ip, r1, #40	; 0x28
   8:	e5902314 	ldr	r2, [r0, #788]	; 0x314
   c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
  10:	e2833001 	add	r3, r3, #1
  14:	e580c314 	str	ip, [r0, #788]	; 0x314
  18:	e280ee31 	add	lr, r0, #784	; 0x310
  1c:	e581e028 	str	lr, [r1, #40]	; 0x28
  20:	e581202c 	str	r2, [r1, #44]	; 0x2c
  24:	e582c000 	str	ip, [r2]
  28:	e5803300 	str	r3, [r0, #768]	; 0x300
  2c:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

Disassembly of section .text.ae_route_ex_put_busy_by_pos:

00000000 <ae_route_ex_put_busy_by_pos>:
   0:	e5903300 	ldr	r3, [r0, #768]	; 0x300
   4:	e1a0c002 	mov	ip, r2
   8:	e92d4010 	push	{r4, lr}
   c:	e2833001 	add	r3, r3, #1
  10:	e5bc4028 	ldr	r4, [ip, #40]!	; 0x28
  14:	e281e028 	add	lr, r1, #40	; 0x28
  18:	e584e004 	str	lr, [r4, #4]
  1c:	e581c02c 	str	ip, [r1, #44]	; 0x2c
  20:	e5814028 	str	r4, [r1, #40]	; 0x28
  24:	e582e028 	str	lr, [r2, #40]	; 0x28
  28:	e5803300 	str	r3, [r0, #768]	; 0x300
  2c:	e8bd8010 	pop	{r4, pc}

Disassembly of section .text.ae_route_ex_get_free:

00000000 <ae_route_ex_get_free>:
   0:	e59fc068 	ldr	ip, [pc, #104]	; 70 <ae_route_ex_get_free+0x70>
   4:	e1a03000 	mov	r3, r0
   8:	e5b31308 	ldr	r1, [r3, #776]!	; 0x308
   c:	e08fc00c 	add	ip, pc, ip
  10:	e1510003 	cmp	r1, r3
  14:	0a000009 	beq	40 <ae_route_ex_get_free+0x40>
  18:	e5903304 	ldr	r3, [r0, #772]	; 0x304
  1c:	e1a02000 	mov	r2, r0
  20:	e1a00001 	mov	r0, r1
  24:	e5911004 	ldr	r1, [r1, #4]
  28:	e2433001 	sub	r3, r3, #1
  2c:	e410c028 	ldr	ip, [r0], #-40	; 0xffffffd8
  30:	e58c1004 	str	r1, [ip, #4]
  34:	e581c000 	str	ip, [r1]
  38:	e5823304 	str	r3, [r2, #772]	; 0x304
  3c:	e12fff1e 	bx	lr
  40:	e59f102c 	ldr	r1, [pc, #44]	; 74 <ae_route_ex_get_free+0x74>
  44:	e3a03021 	mov	r3, #33	; 0x21
  48:	e92d4010 	push	{r4, lr}
  4c:	e59f2024 	ldr	r2, [pc, #36]	; 78 <ae_route_ex_get_free+0x78>
  50:	e79c0001 	ldr	r0, [ip, r1]
  54:	e59f1020 	ldr	r1, [pc, #32]	; 7c <ae_route_ex_get_free+0x7c>
  58:	e08f2002 	add	r2, pc, r2
  5c:	e08f1001 	add	r1, pc, r1
  60:	e5900000 	ldr	r0, [r0]
  64:	ebfffffe 	bl	0 <fprintf>
			64: R_ARM_CALL	fprintf
  68:	e3a00000 	mov	r0, #0
  6c:	e8bd8010 	pop	{r4, pc}
  70:	0000005c 	.word	0x0000005c
			70: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  74:	00000000 	.word	0x00000000
			74: R_ARM_GOT32	stderr
  78:	00000018 	.word	0x00000018
			78: R_ARM_REL32	.rodata.__FUNCTION__.29198
  7c:	00000018 	.word	0x00000018
			7c: R_ARM_REL32	.LC0

Disassembly of section .text.ae_route_ex_put_free:

00000000 <ae_route_ex_put_free>:
   0:	e5903304 	ldr	r3, [r0, #772]	; 0x304
   4:	e281c028 	add	ip, r1, #40	; 0x28
   8:	e590230c 	ldr	r2, [r0, #780]	; 0x30c
   c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
  10:	e2833001 	add	r3, r3, #1
  14:	e580c30c 	str	ip, [r0, #780]	; 0x30c
  18:	e280efc2 	add	lr, r0, #776	; 0x308
  1c:	e581e028 	str	lr, [r1, #40]	; 0x28
  20:	e581202c 	str	r2, [r1, #44]	; 0x2c
  24:	e582c000 	str	ip, [r2]
  28:	e5803304 	str	r3, [r0, #772]	; 0x304
  2c:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

Disassembly of section .text.ae_route_ex_node_multi_check:

00000000 <ae_route_ex_node_multi_check>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e24dd00c 	sub	sp, sp, #12
   8:	e1c261d0 	ldrd	r6, [r2, #16]
   c:	e1c281d8 	ldrd	r8, [r2, #24]
  10:	e592e004 	ldr	lr, [r2, #4]
  14:	e1570009 	cmp	r7, r9
  18:	e590c020 	ldr	ip, [r0, #32]
  1c:	01560008 	cmpeq	r6, r8
  20:	e5923008 	ldr	r3, [r2, #8]
  24:	81a0300e 	movhi	r3, lr
  28:	e35c0005 	cmp	ip, #5
  2c:	0a000006 	beq	4c <ae_route_ex_node_multi_check+0x4c>
  30:	e5911024 	ldr	r1, [r1, #36]	; 0x24
  34:	e3510005 	cmp	r1, #5
  38:	0a000003 	beq	4c <ae_route_ex_node_multi_check+0x4c>
  3c:	e1c061f8 	strd	r6, [r0, #24]
  40:	e3a00000 	mov	r0, #0
  44:	e28dd00c 	add	sp, sp, #12
  48:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
  4c:	e592c000 	ldr	ip, [r2]
  50:	e35c0000 	cmp	ip, #0
  54:	0afffff8 	beq	3c <ae_route_ex_node_multi_check+0x3c>
  58:	e593e000 	ldr	lr, [r3]
  5c:	e5901000 	ldr	r1, [r0]
  60:	e151000e 	cmp	r1, lr
  64:	0280e004 	addeq	lr, r0, #4
  68:	03a0a000 	moveq	sl, #0
  6c:	0a000033 	beq	140 <ae_route_ex_node_multi_check+0x140>
  70:	e3a0b000 	mov	fp, #0
  74:	e1a0a00b 	mov	sl, fp
  78:	e1963007 	orrs	r3, r6, r7
  7c:	e1a04002 	mov	r4, r2
  80:	03a06001 	moveq	r6, #1
  84:	03a07000 	moveq	r7, #0
  88:	e1c261f0 	strd	r6, [r2, #16]
  8c:	e1a05000 	mov	r5, r0
  90:	e1a02001 	mov	r2, r1
  94:	e1a00006 	mov	r0, r6
  98:	e1a01007 	mov	r1, r7
  9c:	e1cd80f0 	strd	r8, [sp]
  a0:	ebfffffe 	bl	0 <ae_ratio_calculate>
			a0: R_ARM_CALL	ae_ratio_calculate
  a4:	e5942008 	ldr	r2, [r4, #8]
  a8:	e1a06000 	mov	r6, r0
  ac:	e5943004 	ldr	r3, [r4, #4]
  b0:	e785010a 	str	r0, [r5, sl, lsl #2]
  b4:	e792200b 	ldr	r2, [r2, fp]
  b8:	e793100b 	ldr	r1, [r3, fp]
  bc:	ebfffffe 	bl	0 <ae_boundaries_check>
			bc: R_ARM_CALL	ae_boundaries_check
  c0:	e1560000 	cmp	r6, r0
  c4:	e785010a 	str	r0, [r5, sl, lsl #2]
  c8:	0a000021 	beq	154 <ae_route_ex_node_multi_check+0x154>
  cc:	e9951009 	ldmib	r5, {r0, r3, ip}
  d0:	e594e000 	ldr	lr, [r4]
  d4:	e0810390 	umull	r0, r1, r0, r3
  d8:	e0832c90 	umull	r2, r3, r0, ip
  dc:	e023319c 	mla	r3, ip, r1, r3
  e0:	e2922702 	adds	r2, r2, #524288	; 0x80000
  e4:	e2a33000 	adc	r3, r3, #0
  e8:	e35e0004 	cmp	lr, #4
  ec:	e1a00a22 	lsr	r0, r2, #20
  f0:	e1800603 	orr	r0, r0, r3, lsl #12
  f4:	e1a01a23 	lsr	r1, r3, #20
  f8:	15952000 	ldrne	r2, [r5]
  fc:	15953010 	ldrne	r3, [r5, #16]
 100:	0595c000 	ldreq	ip, [r5]
 104:	10832392 	umullne	r2, r3, r2, r3
 108:	0083209c 	umulleq	r2, r3, ip, r0
 10c:	10010192 	mulne	r1, r2, r1
 110:	10211390 	mlane	r1, r0, r3, r1
 114:	10832092 	umullne	r2, r3, r2, r0
 118:	0023319c 	mlaeq	r3, ip, r1, r3
 11c:	10813003 	addne	r3, r1, r3
 120:	e3a00000 	mov	r0, #0
 124:	e1c521f8 	strd	r2, [r5, #24]
 128:	e28dd00c 	add	sp, sp, #12
 12c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 130:	e49e1004 	ldr	r1, [lr], #4
 134:	e5b34004 	ldr	r4, [r3, #4]!
 138:	e1510004 	cmp	r1, r4
 13c:	1affffcd 	bne	78 <ae_route_ex_node_multi_check+0x78>
 140:	e28aa001 	add	sl, sl, #1
 144:	e15a000c 	cmp	sl, ip
 148:	e1a0b10a 	lsl	fp, sl, #2
 14c:	1afffff7 	bne	130 <ae_route_ex_node_multi_check+0x130>
 150:	eaffffb9 	b	3c <ae_route_ex_node_multi_check+0x3c>
 154:	e1c421d8 	ldrd	r2, [r4, #24]
 158:	e3a00000 	mov	r0, #0
 15c:	e1c521f8 	strd	r2, [r5, #24]
 160:	e28dd00c 	add	sp, sp, #12
 164:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

Disassembly of section .text.ae_route_ex_node_recurit:

00000000 <ae_route_ex_node_recurit>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e1a08001 	mov	r8, r1
   8:	e24dd01c 	sub	sp, sp, #28
   c:	e1a05000 	mov	r5, r0
  10:	e1c001d8 	ldrd	r0, [r0, #24]
  14:	e58d2008 	str	r2, [sp, #8]
  18:	e58d3000 	str	r3, [sp]
  1c:	e1c821d8 	ldrd	r2, [r8, #24]
  20:	e1510003 	cmp	r1, r3
  24:	01500002 	cmpeq	r0, r2
  28:	0a000054 	beq	180 <ae_route_ex_node_recurit+0x180>
  2c:	e59de000 	ldr	lr, [sp]
  30:	e35e0000 	cmp	lr, #0
  34:	0a000051 	beq	180 <ae_route_ex_node_recurit+0x180>
  38:	e2483004 	sub	r3, r8, #4
  3c:	e3a06000 	mov	r6, #0
  40:	e58d3014 	str	r3, [sp, #20]
  44:	e1a02003 	mov	r2, r3
  48:	e1a0c006 	mov	ip, r6
  4c:	e2453004 	sub	r3, r5, #4
  50:	e5b30004 	ldr	r0, [r3, #4]!
  54:	e2866001 	add	r6, r6, #1
  58:	e5b21004 	ldr	r1, [r2, #4]!
  5c:	e1500001 	cmp	r0, r1
  60:	328cc001 	addcc	ip, ip, #1
  64:	e15e0006 	cmp	lr, r6
  68:	1afffff8 	bne	50 <ae_route_ex_node_recurit+0x50>
  6c:	e58dc00c 	str	ip, [sp, #12]
  70:	e35c0001 	cmp	ip, #1
  74:	9a000041 	bls	180 <ae_route_ex_node_recurit+0x180>
  78:	e3a03000 	mov	r3, #0
  7c:	e1a04003 	mov	r4, r3
  80:	ea000004 	b	98 <ae_route_ex_node_recurit+0x98>
  84:	e59d2000 	ldr	r2, [sp]
  88:	e2844001 	add	r4, r4, #1
  8c:	e1a03004 	mov	r3, r4
  90:	e1520003 	cmp	r2, r3
  94:	9a000039 	bls	180 <ae_route_ex_node_recurit+0x180>
  98:	e7951104 	ldr	r1, [r5, r4, lsl #2]
  9c:	e7982104 	ldr	r2, [r8, r4, lsl #2]
  a0:	e0880104 	add	r0, r8, r4, lsl #2
  a4:	e1510002 	cmp	r1, r2
  a8:	e58d0004 	str	r0, [sp, #4]
  ac:	0afffff4 	beq	84 <ae_route_ex_node_recurit+0x84>
  b0:	e5952020 	ldr	r2, [r5, #32]
  b4:	e1520003 	cmp	r2, r3
  b8:	1afffff1 	bne	84 <ae_route_ex_node_recurit+0x84>
  bc:	e59d0008 	ldr	r0, [sp, #8]
  c0:	ebfffffe 	bl	0 <ae_route_ex_node_recurit>
			c0: R_ARM_CALL	ae_route_ex_get_free
  c4:	e2503000 	subs	r3, r0, #0
  c8:	e58d3010 	str	r3, [sp, #16]
  cc:	0a00002e 	beq	18c <ae_route_ex_node_recurit+0x18c>
  d0:	e59d7014 	ldr	r7, [sp, #20]
  d4:	e3a02000 	mov	r2, #0
  d8:	e2439004 	sub	r9, r3, #4
  dc:	e3a00001 	mov	r0, #1
  e0:	e1a03002 	mov	r3, r2
  e4:	e3a01000 	mov	r1, #0
  e8:	e3a0e005 	mov	lr, #5
  ec:	e1540003 	cmp	r4, r3
  f0:	059dc004 	ldreq	ip, [sp, #4]
  f4:	1795c103 	ldrne	ip, [r5, r3, lsl #2]
  f8:	e2833001 	add	r3, r3, #1
  fc:	059cc000 	ldreq	ip, [ip]
 100:	e5a9c004 	str	ip, [r9, #4]!
 104:	e5b7b004 	ldr	fp, [r7, #4]!
 108:	e00a019c 	mul	sl, ip, r1
 10c:	e15c000b 	cmp	ip, fp
 110:	11a0e002 	movne	lr, r2
 114:	e1a02003 	mov	r2, r3
 118:	e0810c90 	umull	r0, r1, r0, ip
 11c:	e1560003 	cmp	r6, r3
 120:	e08a1001 	add	r1, sl, r1
 124:	1afffff0 	bne	ec <ae_route_ex_node_recurit+0xec>
 128:	e59d200c 	ldr	r2, [sp, #12]
 12c:	e5953020 	ldr	r3, [r5, #32]
 130:	e2422001 	sub	r2, r2, #1
 134:	e58d200c 	str	r2, [sp, #12]
 138:	e1a04002 	mov	r4, r2
 13c:	e59d2010 	ldr	r2, [sp, #16]
 140:	e1c201f8 	strd	r0, [r2, #24]
 144:	e5823024 	str	r3, [r2, #36]	; 0x24
 148:	e1a01002 	mov	r1, r2
 14c:	e588e024 	str	lr, [r8, #36]	; 0x24
 150:	e1a02005 	mov	r2, r5
 154:	e581e020 	str	lr, [r1, #32]
 158:	e1a05001 	mov	r5, r1
 15c:	e59d0008 	ldr	r0, [sp, #8]
 160:	ebfffffe 	bl	0 <ae_route_ex_node_recurit>
			160: R_ARM_CALL	ae_route_ex_put_busy_by_pos
 164:	e3540001 	cmp	r4, #1
 168:	0a000004 	beq	180 <ae_route_ex_node_recurit+0x180>
 16c:	e59d2000 	ldr	r2, [sp]
 170:	e3a04001 	mov	r4, #1
 174:	e1a03004 	mov	r3, r4
 178:	e1520003 	cmp	r2, r3
 17c:	8affffc5 	bhi	98 <ae_route_ex_node_recurit+0x98>
 180:	e3a00000 	mov	r0, #0
 184:	e28dd01c 	add	sp, sp, #28
 188:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 18c:	e3e00000 	mvn	r0, #0
 190:	eafffffb 	b	184 <ae_route_ex_node_recurit+0x184>

Disassembly of section .text.ae_route_ex_node_stgy:

00000000 <ae_route_ex_node_stgy>:
   0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   4:	e1c061d8 	ldrd	r6, [r0, #24]
   8:	e1c141d8 	ldrd	r4, [r1, #24]
   c:	e1570005 	cmp	r7, r5
  10:	01560004 	cmpeq	r6, r4
  14:	03a03005 	moveq	r3, #5
  18:	05803020 	streq	r3, [r0, #32]
  1c:	05813024 	streq	r3, [r1, #36]	; 0x24
  20:	0a000012 	beq	70 <ae_route_ex_node_stgy+0x70>
  24:	2a000011 	bcs	70 <ae_route_ex_node_stgy+0x70>
  28:	e3520000 	cmp	r2, #0
  2c:	0a00000f 	beq	70 <ae_route_ex_node_stgy+0x70>
  30:	e590c000 	ldr	ip, [r0]
  34:	e5913000 	ldr	r3, [r1]
  38:	e15c0003 	cmp	ip, r3
  3c:	0280e004 	addeq	lr, r0, #4
  40:	0281c004 	addeq	ip, r1, #4
  44:	03a03000 	moveq	r3, #0
  48:	0a000004 	beq	60 <ae_route_ex_node_stgy+0x60>
  4c:	ea000009 	b	78 <ae_route_ex_node_stgy+0x78>
  50:	e49e5004 	ldr	r5, [lr], #4
  54:	e49c4004 	ldr	r4, [ip], #4
  58:	e1550004 	cmp	r5, r4
  5c:	1a000006 	bne	7c <ae_route_ex_node_stgy+0x7c>
  60:	e2833001 	add	r3, r3, #1
  64:	e1530002 	cmp	r3, r2
  68:	e1a06003 	mov	r6, r3
  6c:	1afffff7 	bne	50 <ae_route_ex_node_stgy+0x50>
  70:	e3a00000 	mov	r0, #0
  74:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
  78:	e3a06000 	mov	r6, #0
  7c:	e5806020 	str	r6, [r0, #32]
  80:	e3a00000 	mov	r0, #0
  84:	e5816024 	str	r6, [r1, #36]	; 0x24
  88:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

Disassembly of section .text.ae_route_ex_init:

00000000 <ae_route_ex_init>:
   0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   4:	e3a02000 	mov	r2, #0
   8:	e5802300 	str	r2, [r0, #768]	; 0x300
   c:	e3a03010 	mov	r3, #16
  10:	e5803304 	str	r3, [r0, #772]	; 0x304
  14:	e1a06000 	mov	r6, r0
  18:	e3a05006 	mov	r5, #6
  1c:	e1a04000 	mov	r4, r0
  20:	e2807c03 	add	r7, r0, #768	; 0x300
  24:	e2802e31 	add	r2, r0, #784	; 0x310
  28:	e5802310 	str	r2, [r0, #784]	; 0x310
  2c:	e2803fc2 	add	r3, r0, #776	; 0x308
  30:	e5802314 	str	r2, [r0, #788]	; 0x314
  34:	e5803308 	str	r3, [r0, #776]	; 0x308
  38:	e580330c 	str	r3, [r0, #780]	; 0x30c
  3c:	e5845020 	str	r5, [r4, #32]
  40:	e1a01004 	mov	r1, r4
  44:	e5845024 	str	r5, [r4, #36]	; 0x24
  48:	e1a00006 	mov	r0, r6
  4c:	e2844030 	add	r4, r4, #48	; 0x30
  50:	ebfffffe 	bl	0 <ae_route_ex_init>
			50: R_ARM_CALL	ae_route_ex_put_free
  54:	e1540007 	cmp	r4, r7
  58:	1afffff7 	bne	3c <ae_route_ex_init+0x3c>
  5c:	e3a00000 	mov	r0, #0
  60:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

Disassembly of section .text.ae_route_sf_ex_update:

00000000 <ae_route_sf_ex_update>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e24dd014 	sub	sp, sp, #20
   8:	e58d1004 	str	r1, [sp, #4]
   c:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			c: R_ARM_CALL	isp_get_ae_ctx
  10:	e58d0008 	str	r0, [sp, #8]
  14:	e2809d4f 	add	r9, r0, #5056	; 0x13c0
  18:	e2899030 	add	r9, r9, #48	; 0x30
  1c:	e1a00009 	mov	r0, r9
  20:	ebfffffe 	bl	0 <ae_route_sf_ex_update>
			20: R_ARM_CALL	ae_route_ex_init
  24:	e2503000 	subs	r3, r0, #0
  28:	e58d300c 	str	r3, [sp, #12]
  2c:	1a00004d 	bne	168 <ae_route_sf_ex_update+0x168>
  30:	e59d3008 	ldr	r3, [sp, #8]
  34:	e2837a01 	add	r7, r3, #4096	; 0x1000
  38:	e5973268 	ldr	r3, [r7, #616]	; 0x268
  3c:	e3530000 	cmp	r3, #0
  40:	0a000034 	beq	118 <ae_route_sf_ex_update+0x118>
  44:	e59d3008 	ldr	r3, [sp, #8]
  48:	e59d600c 	ldr	r6, [sp, #12]
  4c:	e2838d49 	add	r8, r3, #4672	; 0x1240
  50:	e288802c 	add	r8, r8, #44	; 0x2c
  54:	ea00000d 	b	90 <ae_route_sf_ex_update+0x90>
  58:	e5912010 	ldr	r2, [r1, #16]
  5c:	e0832293 	umull	r2, r3, r3, r2
  60:	e0000592 	mul	r0, r2, r5
  64:	e0230394 	mla	r3, r4, r3, r0
  68:	e0854492 	umull	r4, r5, r2, r4
  6c:	e0835005 	add	r5, r3, r5
  70:	e1c141f8 	strd	r4, [r1, #24]
  74:	e1a00009 	mov	r0, r9
  78:	e2866001 	add	r6, r6, #1
  7c:	e2888018 	add	r8, r8, #24
  80:	ebfffffe 	bl	0 <ae_route_sf_ex_update>
			80: R_ARM_CALL	ae_route_ex_put_busy
  84:	e5973268 	ldr	r3, [r7, #616]	; 0x268
  88:	e1530006 	cmp	r3, r6
  8c:	9a000021 	bls	118 <ae_route_sf_ex_update+0x118>
  90:	e1a00009 	mov	r0, r9
  94:	ebfffffe 	bl	0 <ae_route_sf_ex_update>
			94: R_ARM_CALL	ae_route_ex_get_free
  98:	e2501000 	subs	r1, r0, #0
  9c:	0a00001d 	beq	118 <ae_route_sf_ex_update+0x118>
  a0:	e5983000 	ldr	r3, [r8]
  a4:	e5972738 	ldr	r2, [r7, #1848]	; 0x738
  a8:	e5813000 	str	r3, [r1]
  ac:	e598a004 	ldr	sl, [r8, #4]
  b0:	e3520000 	cmp	r2, #0
  b4:	03a0e001 	moveq	lr, #1
  b8:	e581a004 	str	sl, [r1, #4]
  bc:	e5980008 	ldr	r0, [r8, #8]
  c0:	0598c010 	ldreq	ip, [r8, #16]
  c4:	e5810008 	str	r0, [r1, #8]
  c8:	e08ba09a 	umull	sl, fp, sl, r0
  cc:	e598200c 	ldr	r2, [r8, #12]
  d0:	01a0cc1e 	lsleq	ip, lr, ip
  d4:	e085429a 	umull	r4, r5, sl, r2
  d8:	e581200c 	str	r2, [r1, #12]
  dc:	e0255b92 	mla	r5, r2, fp, r5
  e0:	e59d2004 	ldr	r2, [sp, #4]
  e4:	1598c014 	ldrne	ip, [r8, #20]
  e8:	e2944702 	adds	r4, r4, #524288	; 0x80000
  ec:	e2a55000 	adc	r5, r5, #0
  f0:	e3520004 	cmp	r2, #4
  f4:	e1a04a24 	lsr	r4, r4, #20
  f8:	e1844605 	orr	r4, r4, r5, lsl #12
  fc:	e581c010 	str	ip, [r1, #16]
 100:	e1a05a25 	lsr	r5, r5, #20
 104:	1affffd3 	bne	58 <ae_route_sf_ex_update+0x58>
 108:	e08ba493 	umull	sl, fp, r3, r4
 10c:	e02bb593 	mla	fp, r3, r5, fp
 110:	e1c1a1f8 	strd	sl, [r1, #24]
 114:	eaffffd6 	b	74 <ae_route_sf_ex_update+0x74>
 118:	e5970700 	ldr	r0, [r7, #1792]	; 0x700
 11c:	e59d3008 	ldr	r3, [sp, #8]
 120:	e5904000 	ldr	r4, [r0]
 124:	e2835c17 	add	r5, r3, #5888	; 0x1700
 128:	e1540005 	cmp	r4, r5
 12c:	11500005 	cmpne	r0, r5
 130:	1a000005 	bne	14c <ae_route_sf_ex_update+0x14c>
 134:	ea00000d 	b	170 <ae_route_sf_ex_update+0x170>
 138:	e5943000 	ldr	r3, [r4]
 13c:	e1550004 	cmp	r5, r4
 140:	11530005 	cmpne	r3, r5
 144:	e1a04003 	mov	r4, r3
 148:	0a000008 	beq	170 <ae_route_sf_ex_update+0x170>
 14c:	e59d2004 	ldr	r2, [sp, #4]
 150:	e2400028 	sub	r0, r0, #40	; 0x28
 154:	e2441028 	sub	r1, r4, #40	; 0x28
 158:	ebfffffe 	bl	0 <ae_route_sf_ex_update>
			158: R_ARM_CALL	ae_route_ex_node_stgy
 15c:	e3500000 	cmp	r0, #0
 160:	e1a00004 	mov	r0, r4
 164:	0afffff3 	beq	138 <ae_route_sf_ex_update+0x138>
 168:	e3e03000 	mvn	r3, #0
 16c:	e58d300c 	str	r3, [sp, #12]
 170:	e59d000c 	ldr	r0, [sp, #12]
 174:	e28dd014 	add	sp, sp, #20
 178:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

Disassembly of section .text.ae_route_ex_update:

00000000 <ae_route_ex_update>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e24dd014 	sub	sp, sp, #20
   8:	e58d1004 	str	r1, [sp, #4]
   c:	ebfffffe 	bl	0 <isp_get_ae_ctx>
			c: R_ARM_CALL	isp_get_ae_ctx
  10:	e1a07000 	mov	r7, r0
  14:	e2809d3d 	add	r9, r0, #3904	; 0xf40
  18:	e1a00009 	mov	r0, r9
  1c:	ebfffffe 	bl	0 <ae_route_ex_update>
			1c: R_ARM_CALL	ae_route_ex_init
  20:	e5973dbc 	ldr	r3, [r7, #3516]	; 0xdbc
  24:	e58d0008 	str	r0, [sp, #8]
  28:	e3530000 	cmp	r3, #0
  2c:	12873a01 	addne	r3, r7, #4096	; 0x1000
  30:	11a08007 	movne	r8, r7
  34:	158d300c 	strne	r3, [sp, #12]
  38:	13a06000 	movne	r6, #0
  3c:	1a00000e 	bne	7c <ae_route_ex_update+0x7c>
  40:	ea000030 	b	108 <ae_route_ex_update+0x108>
  44:	e5912010 	ldr	r2, [r1, #16]
  48:	e0832293 	umull	r2, r3, r3, r2
  4c:	e0000592 	mul	r0, r2, r5
  50:	e0230394 	mla	r3, r4, r3, r0
  54:	e0854492 	umull	r4, r5, r2, r4
  58:	e0835005 	add	r5, r3, r5
  5c:	e1c141f8 	strd	r4, [r1, #24]
  60:	e1a00009 	mov	r0, r9
  64:	e2866001 	add	r6, r6, #1
  68:	e2888018 	add	r8, r8, #24
  6c:	ebfffffe 	bl	0 <ae_route_ex_update>
			6c: R_ARM_CALL	ae_route_ex_put_busy
  70:	e5973dbc 	ldr	r3, [r7, #3516]	; 0xdbc
  74:	e1530006 	cmp	r3, r6
  78:	9a000024 	bls	110 <ae_route_ex_update+0x110>
  7c:	e1a00009 	mov	r0, r9
  80:	ebfffffe 	bl	0 <ae_route_ex_update>
			80: R_ARM_CALL	ae_route_ex_get_free
  84:	e2501000 	subs	r1, r0, #0
  88:	0a000020 	beq	110 <ae_route_ex_update+0x110>
  8c:	e5983dc0 	ldr	r3, [r8, #3520]	; 0xdc0
  90:	e59d200c 	ldr	r2, [sp, #12]
  94:	e5922738 	ldr	r2, [r2, #1848]	; 0x738
  98:	e5813000 	str	r3, [r1]
  9c:	e598adc4 	ldr	sl, [r8, #3524]	; 0xdc4
  a0:	e3520000 	cmp	r2, #0
  a4:	03a0e001 	moveq	lr, #1
  a8:	e581a004 	str	sl, [r1, #4]
  ac:	e5980dc8 	ldr	r0, [r8, #3528]	; 0xdc8
  b0:	0598cdd0 	ldreq	ip, [r8, #3536]	; 0xdd0
  b4:	e5810008 	str	r0, [r1, #8]
  b8:	e08ba09a 	umull	sl, fp, sl, r0
  bc:	e5982dcc 	ldr	r2, [r8, #3532]	; 0xdcc
  c0:	01a0cc1e 	lsleq	ip, lr, ip
  c4:	e085429a 	umull	r4, r5, sl, r2
  c8:	e581200c 	str	r2, [r1, #12]
  cc:	e0255b92 	mla	r5, r2, fp, r5
  d0:	e59d2004 	ldr	r2, [sp, #4]
  d4:	1598cdd4 	ldrne	ip, [r8, #3540]	; 0xdd4
  d8:	e2944702 	adds	r4, r4, #524288	; 0x80000
  dc:	e2a55000 	adc	r5, r5, #0
  e0:	e3520004 	cmp	r2, #4
  e4:	e1a04a24 	lsr	r4, r4, #20
  e8:	e1844605 	orr	r4, r4, r5, lsl #12
  ec:	e581c010 	str	ip, [r1, #16]
  f0:	e1a05a25 	lsr	r5, r5, #20
  f4:	1affffd2 	bne	44 <ae_route_ex_update+0x44>
  f8:	e08ba493 	umull	sl, fp, r3, r4
  fc:	e02bb593 	mla	fp, r3, r5, fp
 100:	e1c1a1f8 	strd	sl, [r1, #24]
 104:	eaffffd5 	b	60 <ae_route_ex_update+0x60>
 108:	e2873a01 	add	r3, r7, #4096	; 0x1000
 10c:	e58d300c 	str	r3, [sp, #12]
 110:	e59d300c 	ldr	r3, [sp, #12]
 114:	e2899e31 	add	r9, r9, #784	; 0x310
 118:	e5930250 	ldr	r0, [r3, #592]	; 0x250
 11c:	e5904000 	ldr	r4, [r0]
 120:	e1540009 	cmp	r4, r9
 124:	11500009 	cmpne	r0, r9
 128:	0a00000d 	beq	164 <ae_route_ex_update+0x164>
 12c:	e59d2004 	ldr	r2, [sp, #4]
 130:	e2441028 	sub	r1, r4, #40	; 0x28
 134:	e2400028 	sub	r0, r0, #40	; 0x28
 138:	ebfffffe 	bl	0 <ae_route_ex_update>
			138: R_ARM_CALL	ae_route_ex_node_stgy
 13c:	e5943000 	ldr	r3, [r4]
 140:	e58d0008 	str	r0, [sp, #8]
 144:	e1a00004 	mov	r0, r4
 148:	e1590004 	cmp	r9, r4
 14c:	11530009 	cmpne	r3, r9
 150:	e1a04003 	mov	r4, r3
 154:	03a02001 	moveq	r2, #1
 158:	13a02000 	movne	r2, #0
 15c:	e3520000 	cmp	r2, #0
 160:	0afffff1 	beq	12c <ae_route_ex_update+0x12c>
 164:	e59d0008 	ldr	r0, [sp, #8]
 168:	e28dd014 	add	sp, sp, #20
 16c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

Disassembly of section .text.ae_route_ex_limit_core:

00000000 <ae_route_ex_limit_core>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e24dd03c 	sub	sp, sp, #60	; 0x3c
   8:	e1a0a000 	mov	sl, r0
   c:	e58d2000 	str	r2, [sp]
  10:	e59f2180 	ldr	r2, [pc, #384]	; 198 <ae_route_ex_limit_core+0x198>
  14:	e58d3004 	str	r3, [sp, #4]
  18:	e5ba8310 	ldr	r8, [sl, #784]!	; 0x310
  1c:	e08f2002 	add	r2, pc, r2
  20:	e59f3174 	ldr	r3, [pc, #372]	; 19c <ae_route_ex_limit_core+0x19c>
  24:	e158000a 	cmp	r8, sl
  28:	e7923003 	ldr	r3, [r2, r3]
  2c:	e5989000 	ldr	r9, [r8]
  30:	e5933000 	ldr	r3, [r3]
  34:	e58d3034 	str	r3, [sp, #52]	; 0x34
  38:	e3a03000 	mov	r3, #0
  3c:	0a00003c 	beq	134 <ae_route_ex_limit_core+0x134>
  40:	e1a07001 	mov	r7, r1
  44:	e28d3010 	add	r3, sp, #16
  48:	e58d300c 	str	r3, [sp, #12]
  4c:	e159000a 	cmp	r9, sl
  50:	e2488028 	sub	r8, r8, #40	; 0x28
  54:	058d8008 	streq	r8, [sp, #8]
  58:	12493028 	subne	r3, r9, #40	; 0x28
  5c:	158d3008 	strne	r3, [sp, #8]
  60:	e3570000 	cmp	r7, #0
  64:	0a000040 	beq	16c <ae_route_ex_limit_core+0x16c>
  68:	e59d3000 	ldr	r3, [sp]
  6c:	e2485004 	sub	r5, r8, #4
  70:	e3a04000 	mov	r4, #0
  74:	e243b004 	sub	fp, r3, #4
  78:	e59d3004 	ldr	r3, [sp, #4]
  7c:	e2436004 	sub	r6, r3, #4
  80:	e5950004 	ldr	r0, [r5, #4]
  84:	e2844001 	add	r4, r4, #1
  88:	e5b62004 	ldr	r2, [r6, #4]!
  8c:	e5bb1004 	ldr	r1, [fp, #4]!
  90:	ebfffffe 	bl	0 <ae_boundaries_check>
			90: R_ARM_CALL	ae_boundaries_check
  94:	e1570004 	cmp	r7, r4
  98:	e5a50004 	str	r0, [r5, #4]!
  9c:	1afffff7 	bne	80 <ae_route_ex_limit_core+0x80>
  a0:	e9981009 	ldmib	r8, {r0, r3, ip}
  a4:	e0810390 	umull	r0, r1, r0, r3
  a8:	e0832c90 	umull	r2, r3, r0, ip
  ac:	e023319c 	mla	r3, ip, r1, r3
  b0:	e2922702 	adds	r2, r2, #524288	; 0x80000
  b4:	e2a33000 	adc	r3, r3, #0
  b8:	e3570004 	cmp	r7, #4
  bc:	e1a02a22 	lsr	r2, r2, #20
  c0:	e1822603 	orr	r2, r2, r3, lsl #12
  c4:	e1a03a23 	lsr	r3, r3, #20
  c8:	0a000023 	beq	15c <ae_route_ex_limit_core+0x15c>
  cc:	e5980000 	ldr	r0, [r8]
  d0:	e5981010 	ldr	r1, [r8, #16]
  d4:	e0810190 	umull	r0, r1, r0, r1
  d8:	e0030390 	mul	r3, r0, r3
  dc:	e0233192 	mla	r3, r2, r1, r3
  e0:	e0810290 	umull	r0, r1, r0, r2
  e4:	e0831001 	add	r1, r3, r1
  e8:	e1c821d8 	ldrd	r2, [r8, #24]
  ec:	e59dc000 	ldr	ip, [sp]
  f0:	e1510003 	cmp	r1, r3
  f4:	e58dc014 	str	ip, [sp, #20]
  f8:	01500002 	cmpeq	r0, r2
  fc:	e59dc004 	ldr	ip, [sp, #4]
 100:	e58d7010 	str	r7, [sp, #16]
 104:	e58dc018 	str	ip, [sp, #24]
 108:	e1cd02f0 	strd	r0, [sp, #32]
 10c:	e1cd22f8 	strd	r2, [sp, #40]	; 0x28
 110:	0a000003 	beq	124 <ae_route_ex_limit_core+0x124>
 114:	e59d1008 	ldr	r1, [sp, #8]
 118:	e1a00008 	mov	r0, r8
 11c:	e59d200c 	ldr	r2, [sp, #12]
 120:	ebfffffe 	bl	0 <ae_route_ex_limit_core>
			120: R_ARM_CALL	ae_route_ex_node_multi_check
 124:	e159000a 	cmp	r9, sl
 128:	e1a08009 	mov	r8, r9
 12c:	e5999000 	ldr	r9, [r9]
 130:	1affffc5 	bne	4c <ae_route_ex_limit_core+0x4c>
 134:	e59f2064 	ldr	r2, [pc, #100]	; 1a0 <ae_route_ex_limit_core+0x1a0>
 138:	e59f305c 	ldr	r3, [pc, #92]	; 19c <ae_route_ex_limit_core+0x19c>
 13c:	e08f2002 	add	r2, pc, r2
 140:	e7923003 	ldr	r3, [r2, r3]
 144:	e5932000 	ldr	r2, [r3]
 148:	e59d3034 	ldr	r3, [sp, #52]	; 0x34
 14c:	e0332002 	eors	r2, r3, r2
 150:	1a00000f 	bne	194 <ae_route_ex_limit_core+0x194>
 154:	e28dd03c 	add	sp, sp, #60	; 0x3c
 158:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 15c:	e598c000 	ldr	ip, [r8]
 160:	e081029c 	umull	r0, r1, ip, r2
 164:	e021139c 	mla	r1, ip, r3, r1
 168:	eaffffde 	b	e8 <ae_route_ex_limit_core+0xe8>
 16c:	e9981009 	ldmib	r8, {r0, r3, ip}
 170:	e0810390 	umull	r0, r1, r0, r3
 174:	e0832c90 	umull	r2, r3, r0, ip
 178:	e023319c 	mla	r3, ip, r1, r3
 17c:	e2922702 	adds	r2, r2, #524288	; 0x80000
 180:	e2a33000 	adc	r3, r3, #0
 184:	e1a02a22 	lsr	r2, r2, #20
 188:	e1822603 	orr	r2, r2, r3, lsl #12
 18c:	e1a03a23 	lsr	r3, r3, #20
 190:	eaffffcd 	b	cc <ae_route_ex_limit_core+0xcc>
 194:	ebfffffe 	bl	0 <__stack_chk_fail>
			194: R_ARM_CALL	__stack_chk_fail
 198:	00000174 	.word	0x00000174
			198: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 19c:	00000000 	.word	0x00000000
			19c: R_ARM_GOT32	__stack_chk_guard
 1a0:	0000005c 	.word	0x0000005c
			1a0: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_

Disassembly of section .text.ae_route_ex_get_joint:

00000000 <ae_route_ex_get_joint>:
   0:	e5b0c310 	ldr	ip, [r0, #784]!	; 0x310
   4:	e3a01000 	mov	r1, #0
   8:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   c:	e1a07003 	mov	r7, r3
  10:	e59c3000 	ldr	r3, [ip]
  14:	e59de018 	ldr	lr, [sp, #24]
  18:	e59d801c 	ldr	r8, [sp, #28]
  1c:	e15c0000 	cmp	ip, r0
  20:	11530000 	cmpne	r3, r0
  24:	e58e1000 	str	r1, [lr]
  28:	e5881000 	str	r1, [r8]
  2c:	0a000010 	beq	74 <ae_route_ex_get_joint+0x74>
  30:	e1a06002 	mov	r6, r2
  34:	e24c2028 	sub	r2, ip, #40	; 0x28
  38:	e2431028 	sub	r1, r3, #40	; 0x28
  3c:	e1c241d8 	ldrd	r4, [r2, #24]
  40:	e1a0c003 	mov	ip, r3
  44:	e1550007 	cmp	r5, r7
  48:	01540006 	cmpeq	r4, r6
  4c:	8a000003 	bhi	60 <ae_route_ex_get_joint+0x60>
  50:	e14341d0 	ldrd	r4, [r3, #-16]
  54:	e1570005 	cmp	r7, r5
  58:	01560004 	cmpeq	r6, r4
  5c:	9a000006 	bls	7c <ae_route_ex_get_joint+0x7c>
  60:	e5931000 	ldr	r1, [r3]
  64:	e1530000 	cmp	r3, r0
  68:	11510000 	cmpne	r1, r0
  6c:	e1a03001 	mov	r3, r1
  70:	1affffef 	bne	34 <ae_route_ex_get_joint+0x34>
  74:	e3a00000 	mov	r0, #0
  78:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
  7c:	e58e2000 	str	r2, [lr]
  80:	e3a00000 	mov	r0, #0
  84:	e5881000 	str	r1, [r8]
  88:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

Disassembly of section .text.ae_route_ex_get_up_node:

00000000 <ae_route_ex_get_up_node>:
   0:	e5913028 	ldr	r3, [r1, #40]	; 0x28
   4:	e2800e31 	add	r0, r0, #784	; 0x310
   8:	e1530000 	cmp	r3, r0
   c:	12430028 	subne	r0, r3, #40	; 0x28
  10:	03a00000 	moveq	r0, #0
  14:	e12fff1e 	bx	lr

Disassembly of section .text.ae_route_ex_get_dw_node:

00000000 <ae_route_ex_get_dw_node>:
   0:	e591302c 	ldr	r3, [r1, #44]	; 0x2c
   4:	e2800e31 	add	r0, r0, #784	; 0x310
   8:	e1530000 	cmp	r3, r0
   c:	12430028 	subne	r0, r3, #40	; 0x28
  10:	03a00000 	moveq	r0, #0
  14:	e12fff1e 	bx	lr

Disassembly of section .text.ae_route_ex_get_first_node:

00000000 <ae_route_ex_get_first_node>:
   0:	e5b03310 	ldr	r3, [r0, #784]!	; 0x310
   4:	e1530000 	cmp	r3, r0
   8:	12430028 	subne	r0, r3, #40	; 0x28
   c:	03a00000 	moveq	r0, #0
  10:	e12fff1e 	bx	lr

Disassembly of section .text.ae_route_ex_print:

00000000 <ae_route_ex_print>:
   0:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
   4:	e1a06000 	mov	r6, r0
   8:	e24dd024 	sub	sp, sp, #36	; 0x24
   c:	ebfffffe 	bl	0 <ae_route_ex_print>
			c: R_ARM_CALL	ae_route_ex_get_first_node
  10:	e5963300 	ldr	r3, [r6, #768]	; 0x300
  14:	e3530000 	cmp	r3, #0
  18:	0a000026 	beq	b8 <ae_route_ex_print+0xb8>
  1c:	e3500000 	cmp	r0, #0
  20:	e1a04000 	mov	r4, r0
  24:	01a05000 	moveq	r5, r0
  28:	0a00001e 	beq	a8 <ae_route_ex_print+0xa8>
  2c:	e59f708c 	ldr	r7, [pc, #140]	; c0 <ae_route_ex_print+0xc0>
  30:	e3a05000 	mov	r5, #0
  34:	e08f7007 	add	r7, pc, r7
  38:	e1c481d8 	ldrd	r8, [r4, #24]
  3c:	e5943004 	ldr	r3, [r4, #4]
  40:	e1a01005 	mov	r1, r5
  44:	e5942000 	ldr	r2, [r4]
  48:	e1a00007 	mov	r0, r7
  4c:	e1cd81f8 	strd	r8, [sp, #24]
  50:	e594c024 	ldr	ip, [r4, #36]	; 0x24
  54:	e2855001 	add	r5, r5, #1
  58:	e6ef5075 	uxtb	r5, r5
  5c:	e58dc010 	str	ip, [sp, #16]
  60:	e594c020 	ldr	ip, [r4, #32]
  64:	e58dc00c 	str	ip, [sp, #12]
  68:	e594c010 	ldr	ip, [r4, #16]
  6c:	e58dc008 	str	ip, [sp, #8]
  70:	e594c00c 	ldr	ip, [r4, #12]
  74:	e58dc004 	str	ip, [sp, #4]
  78:	e594c008 	ldr	ip, [r4, #8]
  7c:	e58dc000 	str	ip, [sp]
  80:	ebfffffe 	bl	0 <printf>
			80: R_ARM_CALL	printf
  84:	e1a01004 	mov	r1, r4
  88:	e1a00006 	mov	r0, r6
  8c:	ebfffffe 	bl	0 <ae_route_ex_print>
			8c: R_ARM_CALL	ae_route_ex_get_up_node
  90:	e5963300 	ldr	r3, [r6, #768]	; 0x300
  94:	e1a04000 	mov	r4, r0
  98:	e1550003 	cmp	r5, r3
  9c:	2a000005 	bcs	b8 <ae_route_ex_print+0xb8>
  a0:	e3540000 	cmp	r4, #0
  a4:	1affffe3 	bne	38 <ae_route_ex_print+0x38>
  a8:	e2855001 	add	r5, r5, #1
  ac:	e6ef5075 	uxtb	r5, r5
  b0:	e1530005 	cmp	r3, r5
  b4:	8afffffb 	bhi	a8 <ae_route_ex_print+0xa8>
  b8:	e28dd024 	add	sp, sp, #36	; 0x24
  bc:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
  c0:	00000084 	.word	0x00000084
			c0: R_ARM_REL32	.LC1

Disassembly of section .text.ae_route_ex_del_rdcy:

00000000 <ae_route_ex_del_rdcy>:
   0:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
   4:	e1a05000 	mov	r5, r0
   8:	e5b52310 	ldr	r2, [r5, #784]!	; 0x310
   c:	e5923000 	ldr	r3, [r2]
  10:	e1530005 	cmp	r3, r5
  14:	11520005 	cmpne	r2, r5
  18:	08bd83f0 	popeq	{r4, r5, r6, r7, r8, r9, pc}
  1c:	e2807fc2 	add	r7, r0, #776	; 0x308
  20:	ea00001c 	b	98 <ae_route_ex_del_rdcy+0x98>
  24:	e794410c 	ldr	r4, [r4, ip, lsl #2]
  28:	e79e810c 	ldr	r8, [lr, ip, lsl #2]
  2c:	e1580004 	cmp	r8, r4
  30:	0a000002 	beq	40 <ae_route_ex_del_rdcy+0x40>
  34:	e15c0006 	cmp	ip, r6
  38:	11a02003 	movne	r2, r3
  3c:	1a000011 	bne	88 <ae_route_ex_del_rdcy+0x88>
  40:	e8931010 	ldm	r3, {r4, ip}
  44:	e58e6020 	str	r6, [lr, #32]
  48:	e584c004 	str	ip, [r4, #4]
  4c:	e58c4000 	str	r4, [ip]
  50:	e5906308 	ldr	r6, [r0, #776]	; 0x308
  54:	e590e300 	ldr	lr, [r0, #768]	; 0x300
  58:	e590c304 	ldr	ip, [r0, #772]	; 0x304
  5c:	e5863004 	str	r3, [r6, #4]
  60:	e24ee001 	sub	lr, lr, #1
  64:	e5837004 	str	r7, [r3, #4]
  68:	e28cc001 	add	ip, ip, #1
  6c:	e5924004 	ldr	r4, [r2, #4]
  70:	e5836000 	str	r6, [r3]
  74:	e1540005 	cmp	r4, r5
  78:	e5803308 	str	r3, [r0, #776]	; 0x308
  7c:	11a02004 	movne	r2, r4
  80:	e580e300 	str	lr, [r0, #768]	; 0x300
  84:	e580c304 	str	ip, [r0, #772]	; 0x304
  88:	e5923000 	ldr	r3, [r2]
  8c:	e1520005 	cmp	r2, r5
  90:	11530005 	cmpne	r3, r5
  94:	0a00001c 	beq	10c <ae_route_ex_del_rdcy+0x10c>
  98:	e512c008 	ldr	ip, [r2, #-8]
  9c:	e242e028 	sub	lr, r2, #40	; 0x28
  a0:	e5136008 	ldr	r6, [r3, #-8]
  a4:	e2434028 	sub	r4, r3, #40	; 0x28
  a8:	e35c0005 	cmp	ip, #5
  ac:	1affffdc 	bne	24 <ae_route_ex_del_rdcy+0x24>
  b0:	e59e9000 	ldr	r9, [lr]
  b4:	e3510004 	cmp	r1, #4
  b8:	e5948000 	ldr	r8, [r4]
  bc:	0a000013 	beq	110 <ae_route_ex_del_rdcy+0x110>
  c0:	e1590008 	cmp	r9, r8
  c4:	1affffda 	bne	34 <ae_route_ex_del_rdcy+0x34>
  c8:	e59e9004 	ldr	r9, [lr, #4]
  cc:	e5948004 	ldr	r8, [r4, #4]
  d0:	e1590008 	cmp	r9, r8
  d4:	1affffd6 	bne	34 <ae_route_ex_del_rdcy+0x34>
  d8:	e59e9008 	ldr	r9, [lr, #8]
  dc:	e5948008 	ldr	r8, [r4, #8]
  e0:	e1590008 	cmp	r9, r8
  e4:	1affffd2 	bne	34 <ae_route_ex_del_rdcy+0x34>
  e8:	e59e900c 	ldr	r9, [lr, #12]
  ec:	e594800c 	ldr	r8, [r4, #12]
  f0:	e1590008 	cmp	r9, r8
  f4:	1affffce 	bne	34 <ae_route_ex_del_rdcy+0x34>
  f8:	e5944010 	ldr	r4, [r4, #16]
  fc:	e59e8010 	ldr	r8, [lr, #16]
 100:	e1580004 	cmp	r8, r4
 104:	1affffca 	bne	34 <ae_route_ex_del_rdcy+0x34>
 108:	eaffffcc 	b	40 <ae_route_ex_del_rdcy+0x40>
 10c:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
 110:	e1590008 	cmp	r9, r8
 114:	1affffc6 	bne	34 <ae_route_ex_del_rdcy+0x34>
 118:	e59e9004 	ldr	r9, [lr, #4]
 11c:	e5948004 	ldr	r8, [r4, #4]
 120:	e1590008 	cmp	r9, r8
 124:	1affffc2 	bne	34 <ae_route_ex_del_rdcy+0x34>
 128:	e59e9008 	ldr	r9, [lr, #8]
 12c:	e5948008 	ldr	r8, [r4, #8]
 130:	e1590008 	cmp	r9, r8
 134:	1affffbe 	bne	34 <ae_route_ex_del_rdcy+0x34>
 138:	e594400c 	ldr	r4, [r4, #12]
 13c:	e59e800c 	ldr	r8, [lr, #12]
 140:	e1580004 	cmp	r8, r4
 144:	1affffba 	bne	34 <ae_route_ex_del_rdcy+0x34>
 148:	eaffffbc 	b	40 <ae_route_ex_del_rdcy+0x40>

Disassembly of section .text.ae_route_ex_limit:

00000000 <ae_route_ex_limit>:
   0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   4:	e1a05000 	mov	r5, r0
   8:	e1a07000 	mov	r7, r0
   c:	e1a06001 	mov	r6, r1
  10:	ebfffffe 	bl	0 <ae_route_ex_limit>
			10: R_ARM_CALL	ae_route_ex_limit_core
  14:	e5b50310 	ldr	r0, [r5, #784]!	; 0x310
  18:	e5904000 	ldr	r4, [r0]
  1c:	e1500005 	cmp	r0, r5
  20:	11540005 	cmpne	r4, r5
  24:	1a000005 	bne	40 <ae_route_ex_limit+0x40>
  28:	ea000021 	b	b4 <ae_route_ex_limit+0xb4>
  2c:	e5943000 	ldr	r3, [r4]
  30:	e1550004 	cmp	r5, r4
  34:	11530005 	cmpne	r3, r5
  38:	e1a04003 	mov	r4, r3
  3c:	0a000009 	beq	68 <ae_route_ex_limit+0x68>
  40:	e2400028 	sub	r0, r0, #40	; 0x28
  44:	e1a03006 	mov	r3, r6
  48:	e1a02007 	mov	r2, r7
  4c:	e2441028 	sub	r1, r4, #40	; 0x28
  50:	ebfffffe 	bl	0 <ae_route_ex_limit>
			50: R_ARM_CALL	ae_route_ex_node_recurit
  54:	e3500000 	cmp	r0, #0
  58:	e1a00004 	mov	r0, r4
  5c:	0afffff2 	beq	2c <ae_route_ex_limit+0x2c>
  60:	e3e00000 	mvn	r0, #0
  64:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
  68:	e5970310 	ldr	r0, [r7, #784]	; 0x310
  6c:	e5904000 	ldr	r4, [r0]
  70:	e1500005 	cmp	r0, r5
  74:	11540005 	cmpne	r4, r5
  78:	1a000005 	bne	94 <ae_route_ex_limit+0x94>
  7c:	ea00000c 	b	b4 <ae_route_ex_limit+0xb4>
  80:	e5943000 	ldr	r3, [r4]
  84:	e1530005 	cmp	r3, r5
  88:	11550004 	cmpne	r5, r4
  8c:	e1a04003 	mov	r4, r3
  90:	0a000007 	beq	b4 <ae_route_ex_limit+0xb4>
  94:	e2400028 	sub	r0, r0, #40	; 0x28
  98:	e1a02006 	mov	r2, r6
  9c:	e2441028 	sub	r1, r4, #40	; 0x28
  a0:	ebfffffe 	bl	0 <ae_route_ex_limit>
			a0: R_ARM_CALL	ae_route_ex_node_stgy
  a4:	e3500000 	cmp	r0, #0
  a8:	e1a00004 	mov	r0, r4
  ac:	0afffff3 	beq	80 <ae_route_ex_limit+0x80>
  b0:	eaffffea 	b	60 <ae_route_ex_limit+0x60>
  b4:	e1a00007 	mov	r0, r7
  b8:	e1a01006 	mov	r1, r6
  bc:	ebfffffe 	bl	0 <ae_route_ex_limit>
			bc: R_ARM_CALL	ae_route_ex_del_rdcy
  c0:	e3a00000 	mov	r0, #0
  c4:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
