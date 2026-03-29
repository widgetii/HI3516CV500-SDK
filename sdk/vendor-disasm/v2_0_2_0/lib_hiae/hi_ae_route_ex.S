
hi_ae_route_ex.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l    df *ABS*	00000000 hi_ae_route_ex.c
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.AeRouteExPutBusy	00000000 .text.AeRouteExPutBusy
00000000 l    d  .text.AeRouteExPutBusyByPos	00000000 .text.AeRouteExPutBusyByPos
00000000 l    d  .text.AeRouteExGetFree	00000000 .text.AeRouteExGetFree
00000000 l       .rodata.AeRouteExGetFree.str1.4	00000000 .LC0
00000000 l    d  .text.AeRouteExPutFree	00000000 .text.AeRouteExPutFree
00000000 l    d  .text.AeRouteExNodeMultiCheck	00000000 .text.AeRouteExNodeMultiCheck
00000000 l    d  .text.AeRouteExNodeRecurit	00000000 .text.AeRouteExNodeRecurit
00000000 l    d  .text.AeRouteExNodeStgy	00000000 .text.AeRouteExNodeStgy
00000000 l    d  .text.AeRouteExInit	00000000 .text.AeRouteExInit
00000000 l    d  .text.AeRouteSFExUpdate	00000000 .text.AeRouteSFExUpdate
00000000 l    d  .text.AeRouteExUpdate	00000000 .text.AeRouteExUpdate
00000000 l    d  .text.AeRouteExGetJoint	00000000 .text.AeRouteExGetJoint
00000000 l    d  .text.AeRouteExGetUpNode	00000000 .text.AeRouteExGetUpNode
00000000 l    d  .text.AeRouteExGetDwNode	00000000 .text.AeRouteExGetDwNode
00000000 l    d  .text.AeRouteExGetFirstNode	00000000 .text.AeRouteExGetFirstNode
00000000 l    d  .text.AeRouteExPrint	00000000 .text.AeRouteExPrint
00000000 l       .rodata.AeRouteExPrint.str1.4	00000000 .LC1
00000000 l    d  .text.AeRouteExDelRdcy	00000000 .text.AeRouteExDelRdcy
00000000 l    d  .text.AeRouteExLimit	00000000 .text.AeRouteExLimit
00000000 l    d  .rodata.AeRouteExGetFree.str1.4	00000000 .rodata.AeRouteExGetFree.str1.4
00000000 l    d  .rodata.AeRouteExPrint.str1.4	00000000 .rodata.AeRouteExPrint.str1.4
00000000 l    d  .rodata.__FUNCTION__.28141	00000000 .rodata.__FUNCTION__.28141
00000000 l     O .rodata.__FUNCTION__.28141	00000011 __FUNCTION__.28141
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.AeRouteExPutBusy	00000030 AeRouteExPutBusy
00000000 g     F .text.AeRouteExPutBusyByPos	00000030 AeRouteExPutBusyByPos
00000000 g     F .text.AeRouteExGetFree	00000080 AeRouteExGetFree
00000000         *UND*	00000000 fprintf
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 stderr
00000000 g     F .text.AeRouteExPutFree	00000030 AeRouteExPutFree
00000000 g     F .text.AeRouteExNodeMultiCheck	0000015c AeRouteExNodeMultiCheck
00000000         *UND*	00000000 AeRatioCalculate
00000000         *UND*	00000000 AeBoundariesCheck
00000000 g     F .text.AeRouteExNodeRecurit	00000194 AeRouteExNodeRecurit
00000000 g     F .text.AeRouteExNodeStgy	0000008c AeRouteExNodeStgy
00000000 g     F .text.AeRouteExInit	00000064 AeRouteExInit
00000000 g     F .text.AeRouteSFExUpdate	000001a4 AeRouteSFExUpdate
00000000         *UND*	00000000 g_astAeCtx
00000000 g     F .text.AeRouteExUpdate	0000018c AeRouteExUpdate
00000000 g     F .text.AeRouteExGetJoint	0000008c AeRouteExGetJoint
00000000 g     F .text.AeRouteExGetUpNode	00000018 AeRouteExGetUpNode
00000000 g     F .text.AeRouteExGetDwNode	00000018 AeRouteExGetDwNode
00000000 g     F .text.AeRouteExGetFirstNode	00000014 AeRouteExGetFirstNode
00000000 g     F .text.AeRouteExPrint	000000c4 AeRouteExPrint
00000000         *UND*	00000000 printf
00000000 g     F .text.AeRouteExDelRdcy	00000164 AeRouteExDelRdcy
00000000 g     F .text.AeRouteExLimit	00000204 AeRouteExLimit



Disassembly of section .text.AeRouteExPutBusy:

00000000 <AeRouteExPutBusy>:
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

Disassembly of section .text.AeRouteExPutBusyByPos:

00000000 <AeRouteExPutBusyByPos>:
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

Disassembly of section .text.AeRouteExGetFree:

00000000 <AeRouteExGetFree>:
   0:	e59fc068 	ldr	ip, [pc, #104]	; 70 <AeRouteExGetFree+0x70>
   4:	e1a03000 	mov	r3, r0
   8:	e5b31308 	ldr	r1, [r3, #776]!	; 0x308
   c:	e08fc00c 	add	ip, pc, ip
  10:	e1510003 	cmp	r1, r3
  14:	0a000009 	beq	40 <AeRouteExGetFree+0x40>
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
  40:	e59f102c 	ldr	r1, [pc, #44]	; 74 <AeRouteExGetFree+0x74>
  44:	e3a03029 	mov	r3, #41	; 0x29
  48:	e92d4010 	push	{r4, lr}
  4c:	e59f2024 	ldr	r2, [pc, #36]	; 78 <AeRouteExGetFree+0x78>
  50:	e79c0001 	ldr	r0, [ip, r1]
  54:	e59f1020 	ldr	r1, [pc, #32]	; 7c <AeRouteExGetFree+0x7c>
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
			78: R_ARM_REL32	.rodata.__FUNCTION__.28141
  7c:	00000018 	.word	0x00000018
			7c: R_ARM_REL32	.LC0

Disassembly of section .text.AeRouteExPutFree:

00000000 <AeRouteExPutFree>:
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

Disassembly of section .text.AeRouteExNodeMultiCheck:

00000000 <AeRouteExNodeMultiCheck>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e24dd00c 	sub	sp, sp, #12
   8:	e590e020 	ldr	lr, [r0, #32]
   c:	e1cd83d8 	ldrd	r8, [sp, #56]	; 0x38
  10:	e1cd44d0 	ldrd	r4, [sp, #64]	; 0x40
  14:	e59d7030 	ldr	r7, [sp, #48]	; 0x30
  18:	e1590005 	cmp	r9, r5
  1c:	01580004 	cmpeq	r8, r4
  20:	81a0c003 	movhi	ip, r3
  24:	91a0c007 	movls	ip, r7
  28:	e35e0005 	cmp	lr, #5
  2c:	0a000006 	beq	4c <AeRouteExNodeMultiCheck+0x4c>
  30:	e5911024 	ldr	r1, [r1, #36]	; 0x24
  34:	e3510005 	cmp	r1, #5
  38:	0a000003 	beq	4c <AeRouteExNodeMultiCheck+0x4c>
  3c:	e1c081f8 	strd	r8, [r0, #24]
  40:	e3a00000 	mov	r0, #0
  44:	e28dd00c 	add	sp, sp, #12
  48:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
  4c:	e3520000 	cmp	r2, #0
  50:	0afffff9 	beq	3c <AeRouteExNodeMultiCheck+0x3c>
  54:	e59ce000 	ldr	lr, [ip]
  58:	e5901000 	ldr	r1, [r0]
  5c:	e151000e 	cmp	r1, lr
  60:	0280e004 	addeq	lr, r0, #4
  64:	03a0a000 	moveq	sl, #0
  68:	0a000031 	beq	134 <AeRouteExNodeMultiCheck+0x134>
  6c:	e3a0b000 	mov	fp, #0
  70:	e1a0a00b 	mov	sl, fp
  74:	e198c009 	orrs	ip, r8, r9
  78:	e1a06003 	mov	r6, r3
  7c:	e1a05002 	mov	r5, r2
  80:	03a08001 	moveq	r8, #1
  84:	e1cd24d0 	ldrd	r2, [sp, #64]	; 0x40
  88:	03a09000 	moveq	r9, #0
  8c:	e1a04000 	mov	r4, r0
  90:	e1cd20f0 	strd	r2, [sp]
  94:	e1a00008 	mov	r0, r8
  98:	e1a02001 	mov	r2, r1
  9c:	e1a01009 	mov	r1, r9
  a0:	ebfffffe 	bl	0 <AeRatioCalculate>
			a0: R_ARM_CALL	AeRatioCalculate
  a4:	e784010a 	str	r0, [r4, sl, lsl #2]
  a8:	e1a08000 	mov	r8, r0
  ac:	e797200b 	ldr	r2, [r7, fp]
  b0:	e796100b 	ldr	r1, [r6, fp]
  b4:	ebfffffe 	bl	0 <AeBoundariesCheck>
			b4: R_ARM_CALL	AeBoundariesCheck
  b8:	e1580000 	cmp	r8, r0
  bc:	e784010a 	str	r0, [r4, sl, lsl #2]
  c0:	0a000020 	beq	148 <AeRouteExNodeMultiCheck+0x148>
  c4:	e994100c 	ldmib	r4, {r2, r3, ip}
  c8:	e0832392 	umull	r2, r3, r2, r3
  cc:	e0810c92 	umull	r0, r1, r2, ip
  d0:	e021139c 	mla	r1, ip, r3, r1
  d4:	e2900702 	adds	r0, r0, #524288	; 0x80000
  d8:	e2a11000 	adc	r1, r1, #0
  dc:	e3550004 	cmp	r5, #4
  e0:	e1a00a20 	lsr	r0, r0, #20
  e4:	15943010 	ldrne	r3, [r4, #16]
  e8:	15942000 	ldrne	r2, [r4]
  ec:	e1800601 	orr	r0, r0, r1, lsl #12
  f0:	10832392 	umullne	r2, r3, r2, r3
  f4:	e1a01a21 	lsr	r1, r1, #20
  f8:	0594c000 	ldreq	ip, [r4]
  fc:	10010192 	mulne	r1, r2, r1
 100:	0083209c 	umulleq	r2, r3, ip, r0
 104:	10211390 	mlane	r1, r0, r3, r1
 108:	10832092 	umullne	r2, r3, r2, r0
 10c:	0023319c 	mlaeq	r3, ip, r1, r3
 110:	10813003 	addne	r3, r1, r3
 114:	e3a00000 	mov	r0, #0
 118:	e1c421f8 	strd	r2, [r4, #24]
 11c:	e28dd00c 	add	sp, sp, #12
 120:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 124:	e49e1004 	ldr	r1, [lr], #4
 128:	e5bc4004 	ldr	r4, [ip, #4]!
 12c:	e1510004 	cmp	r1, r4
 130:	1affffcf 	bne	74 <AeRouteExNodeMultiCheck+0x74>
 134:	e28aa001 	add	sl, sl, #1
 138:	e15a0002 	cmp	sl, r2
 13c:	e1a0b10a 	lsl	fp, sl, #2
 140:	1afffff7 	bne	124 <AeRouteExNodeMultiCheck+0x124>
 144:	eaffffbc 	b	3c <AeRouteExNodeMultiCheck+0x3c>
 148:	e1cd24d0 	ldrd	r2, [sp, #64]	; 0x40
 14c:	e3a00000 	mov	r0, #0
 150:	e1c421f8 	strd	r2, [r4, #24]
 154:	e28dd00c 	add	sp, sp, #12
 158:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

Disassembly of section .text.AeRouteExNodeRecurit:

00000000 <AeRouteExNodeRecurit>:
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
  28:	0a000054 	beq	180 <AeRouteExNodeRecurit+0x180>
  2c:	e59de000 	ldr	lr, [sp]
  30:	e35e0000 	cmp	lr, #0
  34:	0a000051 	beq	180 <AeRouteExNodeRecurit+0x180>
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
  68:	1afffff8 	bne	50 <AeRouteExNodeRecurit+0x50>
  6c:	e58dc00c 	str	ip, [sp, #12]
  70:	e35c0001 	cmp	ip, #1
  74:	9a000041 	bls	180 <AeRouteExNodeRecurit+0x180>
  78:	e3a03000 	mov	r3, #0
  7c:	e1a04003 	mov	r4, r3
  80:	ea000004 	b	98 <AeRouteExNodeRecurit+0x98>
  84:	e59d2000 	ldr	r2, [sp]
  88:	e2844001 	add	r4, r4, #1
  8c:	e1a03004 	mov	r3, r4
  90:	e1520003 	cmp	r2, r3
  94:	9a000039 	bls	180 <AeRouteExNodeRecurit+0x180>
  98:	e7951104 	ldr	r1, [r5, r4, lsl #2]
  9c:	e7982104 	ldr	r2, [r8, r4, lsl #2]
  a0:	e0880104 	add	r0, r8, r4, lsl #2
  a4:	e1510002 	cmp	r1, r2
  a8:	e58d0004 	str	r0, [sp, #4]
  ac:	0afffff4 	beq	84 <AeRouteExNodeRecurit+0x84>
  b0:	e5952020 	ldr	r2, [r5, #32]
  b4:	e1520003 	cmp	r2, r3
  b8:	1afffff1 	bne	84 <AeRouteExNodeRecurit+0x84>
  bc:	e59d0008 	ldr	r0, [sp, #8]
  c0:	ebfffffe 	bl	0 <AeRouteExNodeRecurit>
			c0: R_ARM_CALL	AeRouteExGetFree
  c4:	e2503000 	subs	r3, r0, #0
  c8:	e58d3010 	str	r3, [sp, #16]
  cc:	0a00002e 	beq	18c <AeRouteExNodeRecurit+0x18c>
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
 124:	1afffff0 	bne	ec <AeRouteExNodeRecurit+0xec>
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
 160:	ebfffffe 	bl	0 <AeRouteExNodeRecurit>
			160: R_ARM_CALL	AeRouteExPutBusyByPos
 164:	e3540001 	cmp	r4, #1
 168:	0a000004 	beq	180 <AeRouteExNodeRecurit+0x180>
 16c:	e59d2000 	ldr	r2, [sp]
 170:	e3a04001 	mov	r4, #1
 174:	e1a03004 	mov	r3, r4
 178:	e1520003 	cmp	r2, r3
 17c:	8affffc5 	bhi	98 <AeRouteExNodeRecurit+0x98>
 180:	e3a00000 	mov	r0, #0
 184:	e28dd01c 	add	sp, sp, #28
 188:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 18c:	e3e00000 	mvn	r0, #0
 190:	eafffffb 	b	184 <AeRouteExNodeRecurit+0x184>

Disassembly of section .text.AeRouteExNodeStgy:

00000000 <AeRouteExNodeStgy>:
   0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   4:	e1c061d8 	ldrd	r6, [r0, #24]
   8:	e1c141d8 	ldrd	r4, [r1, #24]
   c:	e1570005 	cmp	r7, r5
  10:	01560004 	cmpeq	r6, r4
  14:	03a03005 	moveq	r3, #5
  18:	05803020 	streq	r3, [r0, #32]
  1c:	05813024 	streq	r3, [r1, #36]	; 0x24
  20:	0a000012 	beq	70 <AeRouteExNodeStgy+0x70>
  24:	2a000011 	bcs	70 <AeRouteExNodeStgy+0x70>
  28:	e3520000 	cmp	r2, #0
  2c:	0a00000f 	beq	70 <AeRouteExNodeStgy+0x70>
  30:	e590c000 	ldr	ip, [r0]
  34:	e5913000 	ldr	r3, [r1]
  38:	e15c0003 	cmp	ip, r3
  3c:	0280e004 	addeq	lr, r0, #4
  40:	0281c004 	addeq	ip, r1, #4
  44:	03a03000 	moveq	r3, #0
  48:	0a000004 	beq	60 <AeRouteExNodeStgy+0x60>
  4c:	ea000009 	b	78 <AeRouteExNodeStgy+0x78>
  50:	e49e5004 	ldr	r5, [lr], #4
  54:	e49c4004 	ldr	r4, [ip], #4
  58:	e1550004 	cmp	r5, r4
  5c:	1a000006 	bne	7c <AeRouteExNodeStgy+0x7c>
  60:	e2833001 	add	r3, r3, #1
  64:	e1530002 	cmp	r3, r2
  68:	e1a06003 	mov	r6, r3
  6c:	1afffff7 	bne	50 <AeRouteExNodeStgy+0x50>
  70:	e3a00000 	mov	r0, #0
  74:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
  78:	e3a06000 	mov	r6, #0
  7c:	e5806020 	str	r6, [r0, #32]
  80:	e3a00000 	mov	r0, #0
  84:	e5816024 	str	r6, [r1, #36]	; 0x24
  88:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

Disassembly of section .text.AeRouteExInit:

00000000 <AeRouteExInit>:
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
  50:	ebfffffe 	bl	0 <AeRouteExInit>
			50: R_ARM_CALL	AeRouteExPutFree
  54:	e1540007 	cmp	r4, r7
  58:	1afffff7 	bne	3c <AeRouteExInit+0x3c>
  5c:	e3a00000 	mov	r0, #0
  60:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

Disassembly of section .text.AeRouteSFExUpdate:

00000000 <AeRouteSFExUpdate>:
   0:	e59f3194 	ldr	r3, [pc, #404]	; 19c <AeRouteSFExUpdate+0x19c>
   4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   8:	e303b4b8 	movw	fp, #13496	; 0x34b8
   c:	e08f3003 	add	r3, pc, r3
  10:	e24dd00c 	sub	sp, sp, #12
  14:	e00b009b 	mul	fp, fp, r0
  18:	e59f2180 	ldr	r2, [pc, #384]	; 1a0 <AeRouteSFExUpdate+0x1a0>
  1c:	e1a09001 	mov	r9, r1
  20:	e58d0004 	str	r0, [sp, #4]
  24:	e28b8d4f 	add	r8, fp, #5056	; 0x13c0
  28:	e7933002 	ldr	r3, [r3, r2]
  2c:	e2888008 	add	r8, r8, #8
  30:	e0838008 	add	r8, r3, r8
  34:	e1a04003 	mov	r4, r3
  38:	e084a00b 	add	sl, r4, fp
  3c:	e1a00008 	mov	r0, r8
  40:	e58d3000 	str	r3, [sp]
  44:	ebfffffe 	bl	0 <AeRouteSFExUpdate>
			44: R_ARM_CALL	AeRouteExInit
  48:	e28a3d49 	add	r3, sl, #4672	; 0x1240
  4c:	e5933000 	ldr	r3, [r3]
  50:	e3530000 	cmp	r3, #0
  54:	0a000034 	beq	12c <AeRouteSFExUpdate+0x12c>
  58:	e59d3000 	ldr	r3, [sp]
  5c:	e28b7d49 	add	r7, fp, #4672	; 0x1240
  60:	e2877004 	add	r7, r7, #4
  64:	e3a06000 	mov	r6, #0
  68:	e0837007 	add	r7, r3, r7
  6c:	ea00000a 	b	9c <AeRouteSFExUpdate+0x9c>
  70:	e0832094 	umull	r2, r3, r4, r0
  74:	e0233590 	mla	r3, r0, r5, r3
  78:	e1c121f8 	strd	r2, [r1, #24]
  7c:	e1a00008 	mov	r0, r8
  80:	e2866001 	add	r6, r6, #1
  84:	e2877018 	add	r7, r7, #24
  88:	ebfffffe 	bl	0 <AeRouteSFExUpdate>
			88: R_ARM_CALL	AeRouteExPutBusy
  8c:	e28a3d49 	add	r3, sl, #4672	; 0x1240
  90:	e5933000 	ldr	r3, [r3]
  94:	e1530006 	cmp	r3, r6
  98:	9a000023 	bls	12c <AeRouteSFExUpdate+0x12c>
  9c:	e1a00008 	mov	r0, r8
  a0:	e3a05001 	mov	r5, #1
  a4:	ebfffffe 	bl	0 <AeRouteSFExUpdate>
			a4: R_ARM_CALL	AeRouteExGetFree
  a8:	e2501000 	subs	r1, r0, #0
  ac:	e301270c 	movw	r2, #5900	; 0x170c
  b0:	0a00001d 	beq	12c <AeRouteSFExUpdate+0x12c>
  b4:	e597c000 	ldr	ip, [r7]
  b8:	e5973008 	ldr	r3, [r7, #8]
  bc:	e597e00c 	ldr	lr, [r7, #12]
  c0:	e5974004 	ldr	r4, [r7, #4]
  c4:	e5813008 	str	r3, [r1, #8]
  c8:	e5814004 	str	r4, [r1, #4]
  cc:	e581c000 	str	ip, [r1]
  d0:	e581e00c 	str	lr, [r1, #12]
  d4:	e79a2002 	ldr	r2, [sl, r2]
  d8:	e3520000 	cmp	r2, #0
  dc:	05970010 	ldreq	r0, [r7, #16]
  e0:	15970014 	ldrne	r0, [r7, #20]
  e4:	01a00015 	lsleq	r0, r5, r0
  e8:	e0854394 	umull	r4, r5, r4, r3
  ec:	e5810010 	str	r0, [r1, #16]
  f0:	e0832e94 	umull	r2, r3, r4, lr
  f4:	e023359e 	mla	r3, lr, r5, r3
  f8:	e2922702 	adds	r2, r2, #524288	; 0x80000
  fc:	e2a33000 	adc	r3, r3, #0
 100:	e3590004 	cmp	r9, #4
 104:	e1a02a22 	lsr	r2, r2, #20
 108:	e1822603 	orr	r2, r2, r3, lsl #12
 10c:	e085429c 	umull	r4, r5, ip, r2
 110:	e1a03a23 	lsr	r3, r3, #20
 114:	e025539c 	mla	r5, ip, r3, r5
 118:	1affffd4 	bne	70 <AeRouteSFExUpdate+0x70>
 11c:	e085429c 	umull	r4, r5, ip, r2
 120:	e025539c 	mla	r5, ip, r3, r5
 124:	e1c141f8 	strd	r4, [r1, #24]
 128:	eaffffd3 	b	7c <AeRouteSFExUpdate+0x7c>
 12c:	e59d0004 	ldr	r0, [sp, #4]
 130:	e30334b8 	movw	r3, #13496	; 0x34b8
 134:	e59d1000 	ldr	r1, [sp]
 138:	e30126d8 	movw	r2, #5848	; 0x16d8
 13c:	e08bb002 	add	fp, fp, r2
 140:	e0231093 	mla	r3, r3, r0, r1
 144:	e081b00b 	add	fp, r1, fp
 148:	e7930002 	ldr	r0, [r3, r2]
 14c:	e5904000 	ldr	r4, [r0]
 150:	e150000b 	cmp	r0, fp
 154:	1154000b 	cmpne	r4, fp
 158:	0a00000c 	beq	190 <AeRouteSFExUpdate+0x190>
 15c:	e1a02009 	mov	r2, r9
 160:	e2441028 	sub	r1, r4, #40	; 0x28
 164:	e2400028 	sub	r0, r0, #40	; 0x28
 168:	ebfffffe 	bl	0 <AeRouteSFExUpdate>
			168: R_ARM_CALL	AeRouteExNodeStgy
 16c:	e5943000 	ldr	r3, [r4]
 170:	e1a00004 	mov	r0, r4
 174:	e15b0004 	cmp	fp, r4
 178:	1153000b 	cmpne	r3, fp
 17c:	e1a04003 	mov	r4, r3
 180:	03a02001 	moveq	r2, #1
 184:	13a02000 	movne	r2, #0
 188:	e3520000 	cmp	r2, #0
 18c:	0afffff2 	beq	15c <AeRouteSFExUpdate+0x15c>
 190:	e3a00000 	mov	r0, #0
 194:	e28dd00c 	add	sp, sp, #12
 198:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 19c:	00000188 	.word	0x00000188
			19c: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 1a0:	00000000 	.word	0x00000000
			1a0: R_ARM_GOT32	g_astAeCtx

Disassembly of section .text.AeRouteExUpdate:

00000000 <AeRouteExUpdate>:
   0:	e59f317c 	ldr	r3, [pc, #380]	; 184 <AeRouteExUpdate+0x184>
   4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   8:	e303b4b8 	movw	fp, #13496	; 0x34b8
   c:	e08f3003 	add	r3, pc, r3
  10:	e24dd00c 	sub	sp, sp, #12
  14:	e59f216c 	ldr	r2, [pc, #364]	; 188 <AeRouteExUpdate+0x188>
  18:	e1a0a001 	mov	sl, r1
  1c:	e00b009b 	mul	fp, fp, r0
  20:	e58d0000 	str	r0, [sp]
  24:	e7933002 	ldr	r3, [r3, r2]
  28:	e28b9ef1 	add	r9, fp, #3856	; 0xf10
  2c:	e2899008 	add	r9, r9, #8
  30:	e0839009 	add	r9, r3, r9
  34:	e1a04003 	mov	r4, r3
  38:	e084700b 	add	r7, r4, fp
  3c:	e1a00009 	mov	r0, r9
  40:	e58d3004 	str	r3, [sp, #4]
  44:	ebfffffe 	bl	0 <AeRouteExUpdate>
			44: R_ARM_CALL	AeRouteExInit
  48:	e5973d94 	ldr	r3, [r7, #3476]	; 0xd94
  4c:	e3530000 	cmp	r3, #0
  50:	13a06000 	movne	r6, #0
  54:	11a08007 	movne	r8, r7
  58:	1a00000a 	bne	88 <AeRouteExUpdate+0x88>
  5c:	ea00002d 	b	118 <AeRouteExUpdate+0x118>
  60:	e0832094 	umull	r2, r3, r4, r0
  64:	e0233590 	mla	r3, r0, r5, r3
  68:	e1c121f8 	strd	r2, [r1, #24]
  6c:	e1a00009 	mov	r0, r9
  70:	e2866001 	add	r6, r6, #1
  74:	e2877018 	add	r7, r7, #24
  78:	ebfffffe 	bl	0 <AeRouteExUpdate>
			78: R_ARM_CALL	AeRouteExPutBusy
  7c:	e5983d94 	ldr	r3, [r8, #3476]	; 0xd94
  80:	e1530006 	cmp	r3, r6
  84:	9a000023 	bls	118 <AeRouteExUpdate+0x118>
  88:	e1a00009 	mov	r0, r9
  8c:	e3a05001 	mov	r5, #1
  90:	ebfffffe 	bl	0 <AeRouteExUpdate>
			90: R_ARM_CALL	AeRouteExGetFree
  94:	e2501000 	subs	r1, r0, #0
  98:	e301270c 	movw	r2, #5900	; 0x170c
  9c:	0a00001d 	beq	118 <AeRouteExUpdate+0x118>
  a0:	e597cd98 	ldr	ip, [r7, #3480]	; 0xd98
  a4:	e5973da0 	ldr	r3, [r7, #3488]	; 0xda0
  a8:	e597eda4 	ldr	lr, [r7, #3492]	; 0xda4
  ac:	e5974d9c 	ldr	r4, [r7, #3484]	; 0xd9c
  b0:	e5813008 	str	r3, [r1, #8]
  b4:	e5814004 	str	r4, [r1, #4]
  b8:	e581c000 	str	ip, [r1]
  bc:	e581e00c 	str	lr, [r1, #12]
  c0:	e7982002 	ldr	r2, [r8, r2]
  c4:	e3520000 	cmp	r2, #0
  c8:	05970da8 	ldreq	r0, [r7, #3496]	; 0xda8
  cc:	15970dac 	ldrne	r0, [r7, #3500]	; 0xdac
  d0:	01a00015 	lsleq	r0, r5, r0
  d4:	e0854394 	umull	r4, r5, r4, r3
  d8:	e5810010 	str	r0, [r1, #16]
  dc:	e0832e94 	umull	r2, r3, r4, lr
  e0:	e023359e 	mla	r3, lr, r5, r3
  e4:	e2922702 	adds	r2, r2, #524288	; 0x80000
  e8:	e2a33000 	adc	r3, r3, #0
  ec:	e35a0004 	cmp	sl, #4
  f0:	e1a02a22 	lsr	r2, r2, #20
  f4:	e1822603 	orr	r2, r2, r3, lsl #12
  f8:	e085429c 	umull	r4, r5, ip, r2
  fc:	e1a03a23 	lsr	r3, r3, #20
 100:	e025539c 	mla	r5, ip, r3, r5
 104:	1affffd5 	bne	60 <AeRouteExUpdate+0x60>
 108:	e085429c 	umull	r4, r5, ip, r2
 10c:	e025539c 	mla	r5, ip, r3, r5
 110:	e1c141f8 	strd	r4, [r1, #24]
 114:	eaffffd4 	b	6c <AeRouteExUpdate+0x6c>
 118:	e89d0003 	ldm	sp, {r0, r1}
 11c:	e30334b8 	movw	r3, #13496	; 0x34b8
 120:	e3012228 	movw	r2, #4648	; 0x1228
 124:	e0231093 	mla	r3, r3, r0, r1
 128:	e08bb002 	add	fp, fp, r2
 12c:	e7930002 	ldr	r0, [r3, r2]
 130:	e081b00b 	add	fp, r1, fp
 134:	e5904000 	ldr	r4, [r0]
 138:	e150000b 	cmp	r0, fp
 13c:	1154000b 	cmpne	r4, fp
 140:	0a00000c 	beq	178 <AeRouteExUpdate+0x178>
 144:	e1a0200a 	mov	r2, sl
 148:	e2441028 	sub	r1, r4, #40	; 0x28
 14c:	e2400028 	sub	r0, r0, #40	; 0x28
 150:	ebfffffe 	bl	0 <AeRouteExUpdate>
			150: R_ARM_CALL	AeRouteExNodeStgy
 154:	e5943000 	ldr	r3, [r4]
 158:	e1a00004 	mov	r0, r4
 15c:	e15b0004 	cmp	fp, r4
 160:	1153000b 	cmpne	r3, fp
 164:	e1a04003 	mov	r4, r3
 168:	03a02001 	moveq	r2, #1
 16c:	13a02000 	movne	r2, #0
 170:	e3520000 	cmp	r2, #0
 174:	0afffff2 	beq	144 <AeRouteExUpdate+0x144>
 178:	e3a00000 	mov	r0, #0
 17c:	e28dd00c 	add	sp, sp, #12
 180:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 184:	00000170 	.word	0x00000170
			184: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 188:	00000000 	.word	0x00000000
			188: R_ARM_GOT32	g_astAeCtx

Disassembly of section .text.AeRouteExGetJoint:

00000000 <AeRouteExGetJoint>:
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
  2c:	0a000010 	beq	74 <AeRouteExGetJoint+0x74>
  30:	e1a06002 	mov	r6, r2
  34:	e24c2028 	sub	r2, ip, #40	; 0x28
  38:	e2431028 	sub	r1, r3, #40	; 0x28
  3c:	e1c241d8 	ldrd	r4, [r2, #24]
  40:	e1a0c003 	mov	ip, r3
  44:	e1550007 	cmp	r5, r7
  48:	01540006 	cmpeq	r4, r6
  4c:	8a000003 	bhi	60 <AeRouteExGetJoint+0x60>
  50:	e14341d0 	ldrd	r4, [r3, #-16]
  54:	e1570005 	cmp	r7, r5
  58:	01560004 	cmpeq	r6, r4
  5c:	9a000006 	bls	7c <AeRouteExGetJoint+0x7c>
  60:	e5931000 	ldr	r1, [r3]
  64:	e1530000 	cmp	r3, r0
  68:	11510000 	cmpne	r1, r0
  6c:	e1a03001 	mov	r3, r1
  70:	1affffef 	bne	34 <AeRouteExGetJoint+0x34>
  74:	e3a00000 	mov	r0, #0
  78:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
  7c:	e58e2000 	str	r2, [lr]
  80:	e3a00000 	mov	r0, #0
  84:	e5881000 	str	r1, [r8]
  88:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

Disassembly of section .text.AeRouteExGetUpNode:

00000000 <AeRouteExGetUpNode>:
   0:	e5913028 	ldr	r3, [r1, #40]	; 0x28
   4:	e2800e31 	add	r0, r0, #784	; 0x310
   8:	e1530000 	cmp	r3, r0
   c:	12430028 	subne	r0, r3, #40	; 0x28
  10:	03a00000 	moveq	r0, #0
  14:	e12fff1e 	bx	lr

Disassembly of section .text.AeRouteExGetDwNode:

00000000 <AeRouteExGetDwNode>:
   0:	e591302c 	ldr	r3, [r1, #44]	; 0x2c
   4:	e2800e31 	add	r0, r0, #784	; 0x310
   8:	e1530000 	cmp	r3, r0
   c:	12430028 	subne	r0, r3, #40	; 0x28
  10:	03a00000 	moveq	r0, #0
  14:	e12fff1e 	bx	lr

Disassembly of section .text.AeRouteExGetFirstNode:

00000000 <AeRouteExGetFirstNode>:
   0:	e5b03310 	ldr	r3, [r0, #784]!	; 0x310
   4:	e1530000 	cmp	r3, r0
   8:	12430028 	subne	r0, r3, #40	; 0x28
   c:	03a00000 	moveq	r0, #0
  10:	e12fff1e 	bx	lr

Disassembly of section .text.AeRouteExPrint:

00000000 <AeRouteExPrint>:
   0:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
   4:	e1a06000 	mov	r6, r0
   8:	e24dd024 	sub	sp, sp, #36	; 0x24
   c:	ebfffffe 	bl	0 <AeRouteExPrint>
			c: R_ARM_CALL	AeRouteExGetFirstNode
  10:	e5963300 	ldr	r3, [r6, #768]	; 0x300
  14:	e3530000 	cmp	r3, #0
  18:	0a000026 	beq	b8 <AeRouteExPrint+0xb8>
  1c:	e3500000 	cmp	r0, #0
  20:	e1a04000 	mov	r4, r0
  24:	01a05000 	moveq	r5, r0
  28:	0a00001e 	beq	a8 <AeRouteExPrint+0xa8>
  2c:	e59f708c 	ldr	r7, [pc, #140]	; c0 <AeRouteExPrint+0xc0>
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
  8c:	ebfffffe 	bl	0 <AeRouteExPrint>
			8c: R_ARM_CALL	AeRouteExGetUpNode
  90:	e5963300 	ldr	r3, [r6, #768]	; 0x300
  94:	e1a04000 	mov	r4, r0
  98:	e1550003 	cmp	r5, r3
  9c:	2a000005 	bcs	b8 <AeRouteExPrint+0xb8>
  a0:	e3540000 	cmp	r4, #0
  a4:	1affffe3 	bne	38 <AeRouteExPrint+0x38>
  a8:	e2855001 	add	r5, r5, #1
  ac:	e6ef5075 	uxtb	r5, r5
  b0:	e1530005 	cmp	r3, r5
  b4:	8afffffb 	bhi	a8 <AeRouteExPrint+0xa8>
  b8:	e28dd024 	add	sp, sp, #36	; 0x24
  bc:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
  c0:	00000084 	.word	0x00000084
			c0: R_ARM_REL32	.LC1

Disassembly of section .text.AeRouteExDelRdcy:

00000000 <AeRouteExDelRdcy>:
   0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   4:	e1a04000 	mov	r4, r0
   8:	e5b42310 	ldr	r2, [r4, #784]!	; 0x310
   c:	e5923000 	ldr	r3, [r2]
  10:	e1530004 	cmp	r3, r4
  14:	11520004 	cmpne	r2, r4
  18:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
  1c:	e2807fc2 	add	r7, r0, #776	; 0x308
  20:	ea00000a 	b	50 <AeRouteExDelRdcy+0x50>
  24:	e35c0005 	cmp	ip, #5
  28:	0a000025 	beq	c4 <AeRouteExDelRdcy+0xc4>
  2c:	e795510c 	ldr	r5, [r5, ip, lsl #2]
  30:	e79ec10c 	ldr	ip, [lr, ip, lsl #2]
  34:	e15c0005 	cmp	ip, r5
  38:	0a00000a 	beq	68 <AeRouteExDelRdcy+0x68>
  3c:	e1a02003 	mov	r2, r3
  40:	e5923000 	ldr	r3, [r2]
  44:	e1530004 	cmp	r3, r4
  48:	11520004 	cmpne	r2, r4
  4c:	0a00001b 	beq	c0 <AeRouteExDelRdcy+0xc0>
  50:	e512c008 	ldr	ip, [r2, #-8]
  54:	e242e028 	sub	lr, r2, #40	; 0x28
  58:	e5136008 	ldr	r6, [r3, #-8]
  5c:	e2435028 	sub	r5, r3, #40	; 0x28
  60:	e15c0006 	cmp	ip, r6
  64:	1affffee 	bne	24 <AeRouteExDelRdcy+0x24>
  68:	e8931020 	ldm	r3, {r5, ip}
  6c:	e58e6020 	str	r6, [lr, #32]
  70:	e585c004 	str	ip, [r5, #4]
  74:	e58c5000 	str	r5, [ip]
  78:	e5906308 	ldr	r6, [r0, #776]	; 0x308
  7c:	e590e300 	ldr	lr, [r0, #768]	; 0x300
  80:	e590c304 	ldr	ip, [r0, #772]	; 0x304
  84:	e5863004 	str	r3, [r6, #4]
  88:	e24ee001 	sub	lr, lr, #1
  8c:	e5837004 	str	r7, [r3, #4]
  90:	e28cc001 	add	ip, ip, #1
  94:	e5925004 	ldr	r5, [r2, #4]
  98:	e5836000 	str	r6, [r3]
  9c:	e1550004 	cmp	r5, r4
  a0:	e5803308 	str	r3, [r0, #776]	; 0x308
  a4:	11a02005 	movne	r2, r5
  a8:	e580e300 	str	lr, [r0, #768]	; 0x300
  ac:	e5923000 	ldr	r3, [r2]
  b0:	e580c304 	str	ip, [r0, #772]	; 0x304
  b4:	e1530004 	cmp	r3, r4
  b8:	11520004 	cmpne	r2, r4
  bc:	1affffe3 	bne	50 <AeRouteExDelRdcy+0x50>
  c0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
  c4:	e3510004 	cmp	r1, #4
  c8:	0a000014 	beq	120 <AeRouteExDelRdcy+0x120>
  cc:	e59e8000 	ldr	r8, [lr]
  d0:	e595c000 	ldr	ip, [r5]
  d4:	e158000c 	cmp	r8, ip
  d8:	1affffd7 	bne	3c <AeRouteExDelRdcy+0x3c>
  dc:	e59e8004 	ldr	r8, [lr, #4]
  e0:	e595c004 	ldr	ip, [r5, #4]
  e4:	e158000c 	cmp	r8, ip
  e8:	1affffd3 	bne	3c <AeRouteExDelRdcy+0x3c>
  ec:	e5958008 	ldr	r8, [r5, #8]
  f0:	e59ec008 	ldr	ip, [lr, #8]
  f4:	e158000c 	cmp	r8, ip
  f8:	1affffcf 	bne	3c <AeRouteExDelRdcy+0x3c>
  fc:	e59e800c 	ldr	r8, [lr, #12]
 100:	e595c00c 	ldr	ip, [r5, #12]
 104:	e158000c 	cmp	r8, ip
 108:	1affffcb 	bne	3c <AeRouteExDelRdcy+0x3c>
 10c:	e595c010 	ldr	ip, [r5, #16]
 110:	e59e5010 	ldr	r5, [lr, #16]
 114:	e155000c 	cmp	r5, ip
 118:	1affffc7 	bne	3c <AeRouteExDelRdcy+0x3c>
 11c:	eaffffd1 	b	68 <AeRouteExDelRdcy+0x68>
 120:	e5958000 	ldr	r8, [r5]
 124:	e59ec000 	ldr	ip, [lr]
 128:	e158000c 	cmp	r8, ip
 12c:	1affffc2 	bne	3c <AeRouteExDelRdcy+0x3c>
 130:	e59e8004 	ldr	r8, [lr, #4]
 134:	e595c004 	ldr	ip, [r5, #4]
 138:	e158000c 	cmp	r8, ip
 13c:	1affffbe 	bne	3c <AeRouteExDelRdcy+0x3c>
 140:	e5958008 	ldr	r8, [r5, #8]
 144:	e59ec008 	ldr	ip, [lr, #8]
 148:	e158000c 	cmp	r8, ip
 14c:	1affffba 	bne	3c <AeRouteExDelRdcy+0x3c>
 150:	e595c00c 	ldr	ip, [r5, #12]
 154:	e59e500c 	ldr	r5, [lr, #12]
 158:	e155000c 	cmp	r5, ip
 15c:	1affffb6 	bne	3c <AeRouteExDelRdcy+0x3c>
 160:	eaffffc0 	b	68 <AeRouteExDelRdcy+0x68>

Disassembly of section .text.AeRouteExLimit:

00000000 <AeRouteExLimit>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e1a0a000 	mov	sl, r0
   8:	e24dd02c 	sub	sp, sp, #44	; 0x2c
   c:	e1a07001 	mov	r7, r1
  10:	e5ba8310 	ldr	r8, [sl, #784]!	; 0x310
  14:	e58d0024 	str	r0, [sp, #36]	; 0x24
  18:	e158000a 	cmp	r8, sl
  1c:	e58d201c 	str	r2, [sp, #28]
  20:	e58d3020 	str	r3, [sp, #32]
  24:	e5989000 	ldr	r9, [r8]
  28:	0a000065 	beq	1c4 <AeRouteExLimit+0x1c4>
  2c:	e159000a 	cmp	r9, sl
  30:	e2488028 	sub	r8, r8, #40	; 0x28
  34:	058d8018 	streq	r8, [sp, #24]
  38:	12493028 	subne	r3, r9, #40	; 0x28
  3c:	158d3018 	strne	r3, [sp, #24]
  40:	e3570000 	cmp	r7, #0
  44:	0a000064 	beq	1dc <AeRouteExLimit+0x1dc>
  48:	e59d301c 	ldr	r3, [sp, #28]
  4c:	e2485004 	sub	r5, r8, #4
  50:	e3a04000 	mov	r4, #0
  54:	e243b004 	sub	fp, r3, #4
  58:	e59d3020 	ldr	r3, [sp, #32]
  5c:	e2436004 	sub	r6, r3, #4
  60:	e5950004 	ldr	r0, [r5, #4]
  64:	e2844001 	add	r4, r4, #1
  68:	e5b62004 	ldr	r2, [r6, #4]!
  6c:	e5bb1004 	ldr	r1, [fp, #4]!
  70:	ebfffffe 	bl	0 <AeBoundariesCheck>
			70: R_ARM_CALL	AeBoundariesCheck
  74:	e1540007 	cmp	r4, r7
  78:	e5a50004 	str	r0, [r5, #4]!
  7c:	1afffff7 	bne	60 <AeRouteExLimit+0x60>
  80:	e9981009 	ldmib	r8, {r0, r3, ip}
  84:	e0810390 	umull	r0, r1, r0, r3
  88:	e0832c90 	umull	r2, r3, r0, ip
  8c:	e023319c 	mla	r3, ip, r1, r3
  90:	e2922702 	adds	r2, r2, #524288	; 0x80000
  94:	e2a33000 	adc	r3, r3, #0
  98:	e3570004 	cmp	r7, #4
  9c:	e1a02a22 	lsr	r2, r2, #20
  a0:	e1822603 	orr	r2, r2, r3, lsl #12
  a4:	e1a03a23 	lsr	r3, r3, #20
  a8:	0a000019 	beq	114 <AeRouteExLimit+0x114>
  ac:	e5980000 	ldr	r0, [r8]
  b0:	e5981010 	ldr	r1, [r8, #16]
  b4:	e0810190 	umull	r0, r1, r0, r1
  b8:	e0030390 	mul	r3, r0, r3
  bc:	e0233192 	mla	r3, r2, r1, r3
  c0:	e0810290 	umull	r0, r1, r0, r2
  c4:	e0831001 	add	r1, r3, r1
  c8:	e1c821d8 	ldrd	r2, [r8, #24]
  cc:	e1510003 	cmp	r1, r3
  d0:	01500002 	cmpeq	r0, r2
  d4:	0a000008 	beq	fc <AeRouteExLimit+0xfc>
  d8:	e1cd21f0 	strd	r2, [sp, #16]
  dc:	e59d3020 	ldr	r3, [sp, #32]
  e0:	e1a02007 	mov	r2, r7
  e4:	e1cd00f8 	strd	r0, [sp, #8]
  e8:	e58d3000 	str	r3, [sp]
  ec:	e1a00008 	mov	r0, r8
  f0:	e59d1018 	ldr	r1, [sp, #24]
  f4:	e59d301c 	ldr	r3, [sp, #28]
  f8:	ebfffffe 	bl	0 <AeRouteExLimit>
			f8: R_ARM_CALL	AeRouteExNodeMultiCheck
  fc:	e159000a 	cmp	r9, sl
 100:	e1a08009 	mov	r8, r9
 104:	e5993000 	ldr	r3, [r9]
 108:	0a000005 	beq	124 <AeRouteExLimit+0x124>
 10c:	e1a09003 	mov	r9, r3
 110:	eaffffc5 	b	2c <AeRouteExLimit+0x2c>
 114:	e598c000 	ldr	ip, [r8]
 118:	e081029c 	umull	r0, r1, ip, r2
 11c:	e021139c 	mla	r1, ip, r3, r1
 120:	eaffffe8 	b	c8 <AeRouteExLimit+0xc8>
 124:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
 128:	e5930310 	ldr	r0, [r3, #784]	; 0x310
 12c:	e5904000 	ldr	r4, [r0]
 130:	e1540009 	cmp	r4, r9
 134:	11500009 	cmpne	r0, r9
 138:	0a000021 	beq	1c4 <AeRouteExLimit+0x1c4>
 13c:	e1a05003 	mov	r5, r3
 140:	e1a02005 	mov	r2, r5
 144:	e1a03007 	mov	r3, r7
 148:	e2441028 	sub	r1, r4, #40	; 0x28
 14c:	e2400028 	sub	r0, r0, #40	; 0x28
 150:	ebfffffe 	bl	0 <AeRouteExLimit>
			150: R_ARM_CALL	AeRouteExNodeRecurit
 154:	e5943000 	ldr	r3, [r4]
 158:	e1a00004 	mov	r0, r4
 15c:	e1540009 	cmp	r4, r9
 160:	11530009 	cmpne	r3, r9
 164:	e1a04003 	mov	r4, r3
 168:	03a02001 	moveq	r2, #1
 16c:	13a02000 	movne	r2, #0
 170:	e3520000 	cmp	r2, #0
 174:	0afffff1 	beq	140 <AeRouteExLimit+0x140>
 178:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
 17c:	e5930310 	ldr	r0, [r3, #784]	; 0x310
 180:	e5904000 	ldr	r4, [r0]
 184:	e15a0000 	cmp	sl, r0
 188:	115a0004 	cmpne	sl, r4
 18c:	0a00000c 	beq	1c4 <AeRouteExLimit+0x1c4>
 190:	e1a02007 	mov	r2, r7
 194:	e2441028 	sub	r1, r4, #40	; 0x28
 198:	e2400028 	sub	r0, r0, #40	; 0x28
 19c:	ebfffffe 	bl	0 <AeRouteExLimit>
			19c: R_ARM_CALL	AeRouteExNodeStgy
 1a0:	e5943000 	ldr	r3, [r4]
 1a4:	e1a00004 	mov	r0, r4
 1a8:	e153000a 	cmp	r3, sl
 1ac:	115a0004 	cmpne	sl, r4
 1b0:	e1a04003 	mov	r4, r3
 1b4:	03a02001 	moveq	r2, #1
 1b8:	13a02000 	movne	r2, #0
 1bc:	e3520000 	cmp	r2, #0
 1c0:	0afffff2 	beq	190 <AeRouteExLimit+0x190>
 1c4:	e59d0024 	ldr	r0, [sp, #36]	; 0x24
 1c8:	e1a01007 	mov	r1, r7
 1cc:	ebfffffe 	bl	0 <AeRouteExLimit>
			1cc: R_ARM_CALL	AeRouteExDelRdcy
 1d0:	e3a00000 	mov	r0, #0
 1d4:	e28dd02c 	add	sp, sp, #44	; 0x2c
 1d8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 1dc:	e9981009 	ldmib	r8, {r0, r3, ip}
 1e0:	e0810390 	umull	r0, r1, r0, r3
 1e4:	e0832c90 	umull	r2, r3, r0, ip
 1e8:	e023319c 	mla	r3, ip, r1, r3
 1ec:	e2922702 	adds	r2, r2, #524288	; 0x80000
 1f0:	e2a33000 	adc	r3, r3, #0
 1f4:	e1a02a22 	lsr	r2, r2, #20
 1f8:	e1822603 	orr	r2, r2, r3, lsl #12
 1fc:	e1a03a23 	lsr	r3, r3, #20
 200:	eaffffa9 	b	ac <AeRouteExLimit+0xac>
