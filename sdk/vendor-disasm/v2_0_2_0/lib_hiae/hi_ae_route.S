
hi_ae_route.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l    df *ABS*	00000000 hi_ae_route.c
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.AeBoundaryMinCheck	00000000 .text.AeBoundaryMinCheck
00000000 l    d  .text.AeBoundaryMaxCheck	00000000 .text.AeBoundaryMaxCheck
00000000 l    d  .text.AeBoundariesCheck	00000000 .text.AeBoundariesCheck
00000000 l    d  .text.AeBoundaryMinCheck64	00000000 .text.AeBoundaryMinCheck64
00000000 l    d  .text.AeBoundaryMaxCheck64	00000000 .text.AeBoundaryMaxCheck64
00000000 l    d  .text.AeBoundariesCheck64	00000000 .text.AeBoundariesCheck64
00000000 l    d  .text.AeRatioCalculate	00000000 .text.AeRatioCalculate
00000000 l    d  .text.AeRoutePutBusy	00000000 .text.AeRoutePutBusy
00000000 l    d  .text.AeRoutePutBusyByPos	00000000 .text.AeRoutePutBusyByPos
00000000 l    d  .text.AeRouteGetFree	00000000 .text.AeRouteGetFree
00000000 l       .rodata.AeRouteGetFree.str1.4	00000000 .LC0
00000000 l    d  .text.AeRoutePutFree	00000000 .text.AeRoutePutFree
00000000 l    d  .text.AeRouteNodeMultiCheck	00000000 .text.AeRouteNodeMultiCheck
00000000 l    d  .text.AeRouteNodeRecurit	00000000 .text.AeRouteNodeRecurit
00000000 l    d  .text.AeRouteNodeStgy	00000000 .text.AeRouteNodeStgy
00000000 l    d  .text.AeRouteInit	00000000 .text.AeRouteInit
00000000 l    d  .text.AeRouteSFUpdate	00000000 .text.AeRouteSFUpdate
00000000 l    d  .text.AeRouteUpdate	00000000 .text.AeRouteUpdate
00000000 l    d  .text.AeRouteGetJoint	00000000 .text.AeRouteGetJoint
00000000 l    d  .text.AeRouteGetUpNode	00000000 .text.AeRouteGetUpNode
00000000 l    d  .text.AeRouteGetDwNode	00000000 .text.AeRouteGetDwNode
00000000 l    d  .text.AeRouteGetFirstNode	00000000 .text.AeRouteGetFirstNode
00000000 l    d  .text.AeRoutePrint	00000000 .text.AeRoutePrint
00000000 l       .rodata.AeRoutePrint.str1.4	00000000 .LC1
00000000 l    d  .text.AeRouteDelRdcy	00000000 .text.AeRouteDelRdcy
00000000 l    d  .text.AeRouteLimit	00000000 .text.AeRouteLimit
00000000 l    d  .rodata.AeRouteGetFree.str1.4	00000000 .rodata.AeRouteGetFree.str1.4
00000000 l    d  .rodata.AeRoutePrint.str1.4	00000000 .rodata.AeRoutePrint.str1.4
00000000 l    d  .rodata.__FUNCTION__.28172	00000000 .rodata.__FUNCTION__.28172
00000000 l     O .rodata.__FUNCTION__.28172	0000000f __FUNCTION__.28172
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.AeBoundaryMinCheck	0000000c AeBoundaryMinCheck
00000000 g     F .text.AeBoundaryMaxCheck	0000000c AeBoundaryMaxCheck
00000000 g     F .text.AeBoundariesCheck	0000001c AeBoundariesCheck
00000000 g     F .text.AeBoundaryMinCheck64	00000014 AeBoundaryMinCheck64
00000000 g     F .text.AeBoundaryMaxCheck64	00000014 AeBoundaryMaxCheck64
00000000 g     F .text.AeBoundariesCheck64	00000024 AeBoundariesCheck64
00000000         *UND*	00000000 __aeabi_uldivmod
00000000 g     F .text.AeRatioCalculate	00000048 AeRatioCalculate
00000000 g     F .text.AeRoutePutBusy	00000030 AeRoutePutBusy
00000000 g     F .text.AeRoutePutBusyByPos	00000030 AeRoutePutBusyByPos
00000000 g     F .text.AeRouteGetFree	00000080 AeRouteGetFree
00000000         *UND*	00000000 fprintf
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 stderr
00000000 g     F .text.AeRoutePutFree	00000030 AeRoutePutFree
00000000 g     F .text.AeRouteNodeMultiCheck	00000138 AeRouteNodeMultiCheck
00000000 g     F .text.AeRouteNodeRecurit	00000194 AeRouteNodeRecurit
00000000 g     F .text.AeRouteNodeStgy	0000008c AeRouteNodeStgy
00000000 g     F .text.AeRouteInit	00000064 AeRouteInit
00000000 g     F .text.AeRouteSFUpdate	00000174 AeRouteSFUpdate
00000000         *UND*	00000000 g_astAeCtx
00000000 g     F .text.AeRouteUpdate	00000174 AeRouteUpdate
00000000 g     F .text.AeRouteGetJoint	0000008c AeRouteGetJoint
00000000 g     F .text.AeRouteGetUpNode	00000018 AeRouteGetUpNode
00000000 g     F .text.AeRouteGetDwNode	00000018 AeRouteGetDwNode
00000000 g     F .text.AeRouteGetFirstNode	00000014 AeRouteGetFirstNode
00000000 g     F .text.AeRoutePrint	000000b4 AeRoutePrint
00000000         *UND*	00000000 printf
00000000 g     F .text.AeRouteDelRdcy	0000011c AeRouteDelRdcy
00000000 g     F .text.AeRouteLimit	000001c0 AeRouteLimit



Disassembly of section .text.AeBoundaryMinCheck:

00000000 <AeBoundaryMinCheck>:
   0:	e1500001 	cmp	r0, r1
   4:	31a00001 	movcc	r0, r1
   8:	e12fff1e 	bx	lr

Disassembly of section .text.AeBoundaryMaxCheck:

00000000 <AeBoundaryMaxCheck>:
   0:	e1500001 	cmp	r0, r1
   4:	21a00001 	movcs	r0, r1
   8:	e12fff1e 	bx	lr

Disassembly of section .text.AeBoundariesCheck:

00000000 <AeBoundariesCheck>:
   0:	e92d4010 	push	{r4, lr}
   4:	e1a04001 	mov	r4, r1
   8:	e1a01002 	mov	r1, r2
   c:	ebfffffe 	bl	0 <AeBoundariesCheck>
			c: R_ARM_CALL	AeBoundaryMaxCheck
  10:	e1a01004 	mov	r1, r4
  14:	e8bd4010 	pop	{r4, lr}
  18:	eafffffe 	b	0 <AeBoundariesCheck>
			18: R_ARM_JUMP24	AeBoundaryMinCheck

Disassembly of section .text.AeBoundaryMinCheck64:

00000000 <AeBoundaryMinCheck64>:
   0:	e1510003 	cmp	r1, r3
   4:	01500002 	cmpeq	r0, r2
   8:	31a00002 	movcc	r0, r2
   c:	31a01003 	movcc	r1, r3
  10:	e12fff1e 	bx	lr

Disassembly of section .text.AeBoundaryMaxCheck64:

00000000 <AeBoundaryMaxCheck64>:
   0:	e1510003 	cmp	r1, r3
   4:	01500002 	cmpeq	r0, r2
   8:	81a00002 	movhi	r0, r2
   c:	81a01003 	movhi	r1, r3
  10:	e12fff1e 	bx	lr

Disassembly of section .text.AeBoundariesCheck64:

00000000 <AeBoundariesCheck64>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e1a04002 	mov	r4, r2
   8:	e1a05003 	mov	r5, r3
   c:	e1cd21d0 	ldrd	r2, [sp, #16]
  10:	ebfffffe 	bl	0 <AeBoundariesCheck64>
			10: R_ARM_CALL	AeBoundaryMaxCheck64
  14:	e1a02004 	mov	r2, r4
  18:	e1a03005 	mov	r3, r5
  1c:	e8bd4070 	pop	{r4, r5, r6, lr}
  20:	eafffffe 	b	0 <AeBoundariesCheck64>
			20: R_ARM_JUMP24	AeBoundaryMinCheck64

Disassembly of section .text.AeRatioCalculate:

00000000 <AeRatioCalculate>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e59d4010 	ldr	r4, [sp, #16]
   8:	e59d3014 	ldr	r3, [sp, #20]
   c:	e0854492 	umull	r4, r5, r2, r4
  10:	e0255392 	mla	r5, r2, r3, r5
  14:	e1b030a1 	lsrs	r3, r1, #1
  18:	e1a02060 	rrx	r2, r0
  1c:	e0944002 	adds	r4, r4, r2
  20:	e0a55003 	adc	r5, r5, r3
  24:	e1903001 	orrs	r3, r0, r1
  28:	03a00001 	moveq	r0, #1
  2c:	03a01000 	moveq	r1, #0
  30:	e1a02000 	mov	r2, r0
  34:	e1a03001 	mov	r3, r1
  38:	e1a00004 	mov	r0, r4
  3c:	e1a01005 	mov	r1, r5
  40:	ebfffffe 	bl	0 <__aeabi_uldivmod>
			40: R_ARM_CALL	__aeabi_uldivmod
  44:	e8bd8070 	pop	{r4, r5, r6, pc}

Disassembly of section .text.AeRoutePutBusy:

00000000 <AeRoutePutBusy>:
   0:	e5903280 	ldr	r3, [r0, #640]	; 0x280
   4:	e281c020 	add	ip, r1, #32
   8:	e5902294 	ldr	r2, [r0, #660]	; 0x294
   c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
  10:	e2833001 	add	r3, r3, #1
  14:	e580c294 	str	ip, [r0, #660]	; 0x294
  18:	e280ee29 	add	lr, r0, #656	; 0x290
  1c:	e581e020 	str	lr, [r1, #32]
  20:	e5812024 	str	r2, [r1, #36]	; 0x24
  24:	e582c000 	str	ip, [r2]
  28:	e5803280 	str	r3, [r0, #640]	; 0x280
  2c:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

Disassembly of section .text.AeRoutePutBusyByPos:

00000000 <AeRoutePutBusyByPos>:
   0:	e5903280 	ldr	r3, [r0, #640]	; 0x280
   4:	e1a0c002 	mov	ip, r2
   8:	e92d4010 	push	{r4, lr}
   c:	e2833001 	add	r3, r3, #1
  10:	e5bc4020 	ldr	r4, [ip, #32]!
  14:	e281e020 	add	lr, r1, #32
  18:	e584e004 	str	lr, [r4, #4]
  1c:	e581c024 	str	ip, [r1, #36]	; 0x24
  20:	e5814020 	str	r4, [r1, #32]
  24:	e582e020 	str	lr, [r2, #32]
  28:	e5803280 	str	r3, [r0, #640]	; 0x280
  2c:	e8bd8010 	pop	{r4, pc}

Disassembly of section .text.AeRouteGetFree:

00000000 <AeRouteGetFree>:
   0:	e59fc068 	ldr	ip, [pc, #104]	; 70 <AeRouteGetFree+0x70>
   4:	e1a03000 	mov	r3, r0
   8:	e5b31288 	ldr	r1, [r3, #648]!	; 0x288
   c:	e08fc00c 	add	ip, pc, ip
  10:	e1510003 	cmp	r1, r3
  14:	0a000009 	beq	40 <AeRouteGetFree+0x40>
  18:	e5903284 	ldr	r3, [r0, #644]	; 0x284
  1c:	e1a02000 	mov	r2, r0
  20:	e1a00001 	mov	r0, r1
  24:	e5911004 	ldr	r1, [r1, #4]
  28:	e2433001 	sub	r3, r3, #1
  2c:	e410c020 	ldr	ip, [r0], #-32	; 0xffffffe0
  30:	e58c1004 	str	r1, [ip, #4]
  34:	e581c000 	str	ip, [r1]
  38:	e5823284 	str	r3, [r2, #644]	; 0x284
  3c:	e12fff1e 	bx	lr
  40:	e59f102c 	ldr	r1, [pc, #44]	; 74 <AeRouteGetFree+0x74>
  44:	e3a0304d 	mov	r3, #77	; 0x4d
  48:	e92d4010 	push	{r4, lr}
  4c:	e59f2024 	ldr	r2, [pc, #36]	; 78 <AeRouteGetFree+0x78>
  50:	e79c0001 	ldr	r0, [ip, r1]
  54:	e59f1020 	ldr	r1, [pc, #32]	; 7c <AeRouteGetFree+0x7c>
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
			78: R_ARM_REL32	.rodata.__FUNCTION__.28172
  7c:	00000018 	.word	0x00000018
			7c: R_ARM_REL32	.LC0

Disassembly of section .text.AeRoutePutFree:

00000000 <AeRoutePutFree>:
   0:	e5903284 	ldr	r3, [r0, #644]	; 0x284
   4:	e281c020 	add	ip, r1, #32
   8:	e590228c 	ldr	r2, [r0, #652]	; 0x28c
   c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
  10:	e2833001 	add	r3, r3, #1
  14:	e580c28c 	str	ip, [r0, #652]	; 0x28c
  18:	e280efa2 	add	lr, r0, #648	; 0x288
  1c:	e581e020 	str	lr, [r1, #32]
  20:	e5812024 	str	r2, [r1, #36]	; 0x24
  24:	e582c000 	str	ip, [r2]
  28:	e5803284 	str	r3, [r0, #644]	; 0x284
  2c:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

Disassembly of section .text.AeRouteNodeMultiCheck:

00000000 <AeRouteNodeMultiCheck>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e24dd00c 	sub	sp, sp, #12
   8:	e590e018 	ldr	lr, [r0, #24]
   c:	e1cd83d8 	ldrd	r8, [sp, #56]	; 0x38
  10:	e1cd64d0 	ldrd	r6, [sp, #64]	; 0x40
  14:	e59d5030 	ldr	r5, [sp, #48]	; 0x30
  18:	e1590007 	cmp	r9, r7
  1c:	01580006 	cmpeq	r8, r6
  20:	91a0c005 	movls	ip, r5
  24:	81a0c003 	movhi	ip, r3
  28:	e35e0003 	cmp	lr, #3
  2c:	0a000006 	beq	4c <AeRouteNodeMultiCheck+0x4c>
  30:	e591101c 	ldr	r1, [r1, #28]
  34:	e3510003 	cmp	r1, #3
  38:	0a000003 	beq	4c <AeRouteNodeMultiCheck+0x4c>
  3c:	e1c081f0 	strd	r8, [r0, #16]
  40:	e3a00000 	mov	r0, #0
  44:	e28dd00c 	add	sp, sp, #12
  48:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
  4c:	e3520000 	cmp	r2, #0
  50:	0afffff9 	beq	3c <AeRouteNodeMultiCheck+0x3c>
  54:	e5901000 	ldr	r1, [r0]
  58:	e59ce000 	ldr	lr, [ip]
  5c:	e15e0001 	cmp	lr, r1
  60:	0a000024 	beq	f8 <AeRouteNodeMultiCheck+0xf8>
  64:	e3a0b000 	mov	fp, #0
  68:	e1a0a00b 	mov	sl, fp
  6c:	e198c009 	orrs	ip, r8, r9
  70:	e1a04003 	mov	r4, r3
  74:	e1a06002 	mov	r6, r2
  78:	03a08001 	moveq	r8, #1
  7c:	e1cd24d0 	ldrd	r2, [sp, #64]	; 0x40
  80:	03a09000 	moveq	r9, #0
  84:	e1a07000 	mov	r7, r0
  88:	e1cd20f0 	strd	r2, [sp]
  8c:	e1a00008 	mov	r0, r8
  90:	e1a02001 	mov	r2, r1
  94:	e1a01009 	mov	r1, r9
  98:	ebfffffe 	bl	0 <AeRouteNodeMultiCheck>
			98: R_ARM_CALL	AeRatioCalculate
  9c:	e787010a 	str	r0, [r7, sl, lsl #2]
  a0:	e1a08000 	mov	r8, r0
  a4:	e795200b 	ldr	r2, [r5, fp]
  a8:	e794100b 	ldr	r1, [r4, fp]
  ac:	ebfffffe 	bl	0 <AeRouteNodeMultiCheck>
			ac: R_ARM_CALL	AeBoundariesCheck
  b0:	e1580000 	cmp	r8, r0
  b4:	e787010a 	str	r0, [r7, sl, lsl #2]
  b8:	0a000019 	beq	124 <AeRouteNodeMultiCheck+0x124>
  bc:	e247c004 	sub	ip, r7, #4
  c0:	e3a03000 	mov	r3, #0
  c4:	e3a04001 	mov	r4, #1
  c8:	e3a05000 	mov	r5, #0
  cc:	e5bc1004 	ldr	r1, [ip, #4]!
  d0:	e2833001 	add	r3, r3, #1
  d4:	e1530006 	cmp	r3, r6
  d8:	e00e0591 	mul	lr, r1, r5
  dc:	e0854194 	umull	r4, r5, r4, r1
  e0:	e08e5005 	add	r5, lr, r5
  e4:	1afffff8 	bne	cc <AeRouteNodeMultiCheck+0xcc>
  e8:	e3a00000 	mov	r0, #0
  ec:	e1c741f0 	strd	r4, [r7, #16]
  f0:	e28dd00c 	add	sp, sp, #12
  f4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
  f8:	e280e004 	add	lr, r0, #4
  fc:	e3a0a000 	mov	sl, #0
 100:	e28aa001 	add	sl, sl, #1
 104:	e15a0002 	cmp	sl, r2
 108:	e1a0b10a 	lsl	fp, sl, #2
 10c:	0affffca 	beq	3c <AeRouteNodeMultiCheck+0x3c>
 110:	e49e1004 	ldr	r1, [lr], #4
 114:	e5bc4004 	ldr	r4, [ip, #4]!
 118:	e1510004 	cmp	r1, r4
 11c:	1affffd2 	bne	6c <AeRouteNodeMultiCheck+0x6c>
 120:	eafffff6 	b	100 <AeRouteNodeMultiCheck+0x100>
 124:	e1cd24d0 	ldrd	r2, [sp, #64]	; 0x40
 128:	e3a00000 	mov	r0, #0
 12c:	e1c721f0 	strd	r2, [r7, #16]
 130:	e28dd00c 	add	sp, sp, #12
 134:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

Disassembly of section .text.AeRouteNodeRecurit:

00000000 <AeRouteNodeRecurit>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e1a08001 	mov	r8, r1
   8:	e24dd01c 	sub	sp, sp, #28
   c:	e1a05000 	mov	r5, r0
  10:	e1c001d0 	ldrd	r0, [r0, #16]
  14:	e58d2008 	str	r2, [sp, #8]
  18:	e58d3000 	str	r3, [sp]
  1c:	e1c821d0 	ldrd	r2, [r8, #16]
  20:	e1510003 	cmp	r1, r3
  24:	01500002 	cmpeq	r0, r2
  28:	0a000054 	beq	180 <AeRouteNodeRecurit+0x180>
  2c:	e59de000 	ldr	lr, [sp]
  30:	e35e0000 	cmp	lr, #0
  34:	0a000051 	beq	180 <AeRouteNodeRecurit+0x180>
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
  68:	1afffff8 	bne	50 <AeRouteNodeRecurit+0x50>
  6c:	e58dc00c 	str	ip, [sp, #12]
  70:	e35c0001 	cmp	ip, #1
  74:	9a000041 	bls	180 <AeRouteNodeRecurit+0x180>
  78:	e3a03000 	mov	r3, #0
  7c:	e1a04003 	mov	r4, r3
  80:	ea000004 	b	98 <AeRouteNodeRecurit+0x98>
  84:	e59d2000 	ldr	r2, [sp]
  88:	e2844001 	add	r4, r4, #1
  8c:	e1a03004 	mov	r3, r4
  90:	e1520003 	cmp	r2, r3
  94:	9a000039 	bls	180 <AeRouteNodeRecurit+0x180>
  98:	e7951104 	ldr	r1, [r5, r4, lsl #2]
  9c:	e7982104 	ldr	r2, [r8, r4, lsl #2]
  a0:	e0880104 	add	r0, r8, r4, lsl #2
  a4:	e1510002 	cmp	r1, r2
  a8:	e58d0004 	str	r0, [sp, #4]
  ac:	0afffff4 	beq	84 <AeRouteNodeRecurit+0x84>
  b0:	e5952018 	ldr	r2, [r5, #24]
  b4:	e1520003 	cmp	r2, r3
  b8:	1afffff1 	bne	84 <AeRouteNodeRecurit+0x84>
  bc:	e59d0008 	ldr	r0, [sp, #8]
  c0:	ebfffffe 	bl	0 <AeRouteNodeRecurit>
			c0: R_ARM_CALL	AeRouteGetFree
  c4:	e2503000 	subs	r3, r0, #0
  c8:	e58d3010 	str	r3, [sp, #16]
  cc:	0a00002e 	beq	18c <AeRouteNodeRecurit+0x18c>
  d0:	e59d7014 	ldr	r7, [sp, #20]
  d4:	e3a02000 	mov	r2, #0
  d8:	e2439004 	sub	r9, r3, #4
  dc:	e3a00001 	mov	r0, #1
  e0:	e1a03002 	mov	r3, r2
  e4:	e3a01000 	mov	r1, #0
  e8:	e3a0e003 	mov	lr, #3
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
 124:	1afffff0 	bne	ec <AeRouteNodeRecurit+0xec>
 128:	e59d200c 	ldr	r2, [sp, #12]
 12c:	e5953018 	ldr	r3, [r5, #24]
 130:	e2422001 	sub	r2, r2, #1
 134:	e58d200c 	str	r2, [sp, #12]
 138:	e1a04002 	mov	r4, r2
 13c:	e59d2010 	ldr	r2, [sp, #16]
 140:	e1c201f0 	strd	r0, [r2, #16]
 144:	e582301c 	str	r3, [r2, #28]
 148:	e1a01002 	mov	r1, r2
 14c:	e588e01c 	str	lr, [r8, #28]
 150:	e1a02005 	mov	r2, r5
 154:	e581e018 	str	lr, [r1, #24]
 158:	e1a05001 	mov	r5, r1
 15c:	e59d0008 	ldr	r0, [sp, #8]
 160:	ebfffffe 	bl	0 <AeRouteNodeRecurit>
			160: R_ARM_CALL	AeRoutePutBusyByPos
 164:	e3540001 	cmp	r4, #1
 168:	0a000004 	beq	180 <AeRouteNodeRecurit+0x180>
 16c:	e59d2000 	ldr	r2, [sp]
 170:	e3a04001 	mov	r4, #1
 174:	e1a03004 	mov	r3, r4
 178:	e1520003 	cmp	r2, r3
 17c:	8affffc5 	bhi	98 <AeRouteNodeRecurit+0x98>
 180:	e3a00000 	mov	r0, #0
 184:	e28dd01c 	add	sp, sp, #28
 188:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 18c:	e3e00000 	mvn	r0, #0
 190:	eafffffb 	b	184 <AeRouteNodeRecurit+0x184>

Disassembly of section .text.AeRouteNodeStgy:

00000000 <AeRouteNodeStgy>:
   0:	e92d40f0 	push	{r4, r5, r6, r7, lr}
   4:	e1c061d0 	ldrd	r6, [r0, #16]
   8:	e1c141d0 	ldrd	r4, [r1, #16]
   c:	e1570005 	cmp	r7, r5
  10:	01560004 	cmpeq	r6, r4
  14:	03a03003 	moveq	r3, #3
  18:	05803018 	streq	r3, [r0, #24]
  1c:	0581301c 	streq	r3, [r1, #28]
  20:	0a000012 	beq	70 <AeRouteNodeStgy+0x70>
  24:	2a000011 	bcs	70 <AeRouteNodeStgy+0x70>
  28:	e3520000 	cmp	r2, #0
  2c:	0a00000f 	beq	70 <AeRouteNodeStgy+0x70>
  30:	e590c000 	ldr	ip, [r0]
  34:	e5913000 	ldr	r3, [r1]
  38:	e15c0003 	cmp	ip, r3
  3c:	0280e004 	addeq	lr, r0, #4
  40:	0281c004 	addeq	ip, r1, #4
  44:	03a03000 	moveq	r3, #0
  48:	0a000004 	beq	60 <AeRouteNodeStgy+0x60>
  4c:	ea000009 	b	78 <AeRouteNodeStgy+0x78>
  50:	e49e5004 	ldr	r5, [lr], #4
  54:	e49c4004 	ldr	r4, [ip], #4
  58:	e1550004 	cmp	r5, r4
  5c:	1a000006 	bne	7c <AeRouteNodeStgy+0x7c>
  60:	e2833001 	add	r3, r3, #1
  64:	e1530002 	cmp	r3, r2
  68:	e1a06003 	mov	r6, r3
  6c:	1afffff7 	bne	50 <AeRouteNodeStgy+0x50>
  70:	e3a00000 	mov	r0, #0
  74:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
  78:	e3a06000 	mov	r6, #0
  7c:	e5806018 	str	r6, [r0, #24]
  80:	e3a00000 	mov	r0, #0
  84:	e581601c 	str	r6, [r1, #28]
  88:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

Disassembly of section .text.AeRouteInit:

00000000 <AeRouteInit>:
   0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   4:	e3a02000 	mov	r2, #0
   8:	e5802280 	str	r2, [r0, #640]	; 0x280
   c:	e3a03010 	mov	r3, #16
  10:	e5803284 	str	r3, [r0, #644]	; 0x284
  14:	e1a06000 	mov	r6, r0
  18:	e3a05004 	mov	r5, #4
  1c:	e1a04000 	mov	r4, r0
  20:	e2807d0a 	add	r7, r0, #640	; 0x280
  24:	e2802e29 	add	r2, r0, #656	; 0x290
  28:	e5802290 	str	r2, [r0, #656]	; 0x290
  2c:	e2803fa2 	add	r3, r0, #648	; 0x288
  30:	e5802294 	str	r2, [r0, #660]	; 0x294
  34:	e5803288 	str	r3, [r0, #648]	; 0x288
  38:	e580328c 	str	r3, [r0, #652]	; 0x28c
  3c:	e5845018 	str	r5, [r4, #24]
  40:	e1a01004 	mov	r1, r4
  44:	e584501c 	str	r5, [r4, #28]
  48:	e1a00006 	mov	r0, r6
  4c:	e2844028 	add	r4, r4, #40	; 0x28
  50:	ebfffffe 	bl	0 <AeRouteInit>
			50: R_ARM_CALL	AeRoutePutFree
  54:	e1540007 	cmp	r4, r7
  58:	1afffff7 	bne	3c <AeRouteInit+0x3c>
  5c:	e3a00000 	mov	r0, #0
  60:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

Disassembly of section .text.AeRouteSFUpdate:

00000000 <AeRouteSFUpdate>:
   0:	e59f3164 	ldr	r3, [pc, #356]	; 16c <AeRouteSFUpdate+0x16c>
   4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   8:	e30344b8 	movw	r4, #13496	; 0x34b8
   c:	e08f3003 	add	r3, pc, r3
  10:	e24dd00c 	sub	sp, sp, #12
  14:	e59f2154 	ldr	r2, [pc, #340]	; 170 <AeRouteSFUpdate+0x170>
  18:	e1a08000 	mov	r8, r0
  1c:	e0040094 	mul	r4, r4, r0
  20:	e7933002 	ldr	r3, [r3, r2]
  24:	e1a09001 	mov	r9, r1
  28:	e2846eae 	add	r6, r4, #2784	; 0xae0
  2c:	e58d3004 	str	r3, [sp, #4]
  30:	e1a05003 	mov	r5, r3
  34:	e085a004 	add	sl, r5, r4
  38:	e2866008 	add	r6, r6, #8
  3c:	e0836006 	add	r6, r3, r6
  40:	e1a00006 	mov	r0, r6
  44:	ebfffffe 	bl	0 <AeRouteSFUpdate>
			44: R_ARM_CALL	AeRouteInit
  48:	e59a39e0 	ldr	r3, [sl, #2528]	; 0x9e0
  4c:	e3530000 	cmp	r3, #0
  50:	13a05000 	movne	r5, #0
  54:	11a0700a 	movne	r7, sl
  58:	0a000028 	beq	100 <AeRouteSFUpdate+0x100>
  5c:	e1a00006 	mov	r0, r6
  60:	ebfffffe 	bl	0 <AeRouteSFUpdate>
			60: R_ARM_CALL	AeRouteGetFree
  64:	e2501000 	subs	r1, r0, #0
  68:	0a000024 	beq	100 <AeRouteSFUpdate+0x100>
  6c:	e59a29e8 	ldr	r2, [sl, #2536]	; 0x9e8
  70:	e301370c 	movw	r3, #5900	; 0x170c
  74:	e59ac9e4 	ldr	ip, [sl, #2532]	; 0x9e4
  78:	e3a00001 	mov	r0, #1
  7c:	e5812004 	str	r2, [r1, #4]
  80:	e3a0e000 	mov	lr, #0
  84:	e581c000 	str	ip, [r1]
  88:	e7973003 	ldr	r3, [r7, r3]
  8c:	e3530000 	cmp	r3, #0
  90:	03a03001 	moveq	r3, #1
  94:	059a29ec 	ldreq	r2, [sl, #2540]	; 0x9ec
  98:	159a39f0 	ldrne	r3, [sl, #2544]	; 0x9f0
  9c:	01a03213 	lsleq	r3, r3, r2
  a0:	e3590000 	cmp	r9, #0
  a4:	e5813008 	str	r3, [r1, #8]
  a8:	e5810010 	str	r0, [r1, #16]
  ac:	e581e014 	str	lr, [r1, #20]
  b0:	0a00000b 	beq	e4 <AeRouteSFUpdate+0xe4>
  b4:	e1a02000 	mov	r2, r0
  b8:	e1a0300e 	mov	r3, lr
  bc:	e2810004 	add	r0, r1, #4
  c0:	ea000000 	b	c8 <AeRouteSFUpdate+0xc8>
  c4:	e490c004 	ldr	ip, [r0], #4
  c8:	e28ee001 	add	lr, lr, #1
  cc:	e00b039c 	mul	fp, ip, r3
  d0:	e15e0009 	cmp	lr, r9
  d4:	e0832c92 	umull	r2, r3, r2, ip
  d8:	e08b3003 	add	r3, fp, r3
  dc:	1afffff8 	bne	c4 <AeRouteSFUpdate+0xc4>
  e0:	e1c121f0 	strd	r2, [r1, #16]
  e4:	e1a00006 	mov	r0, r6
  e8:	e2855001 	add	r5, r5, #1
  ec:	e28aa010 	add	sl, sl, #16
  f0:	ebfffffe 	bl	0 <AeRouteSFUpdate>
			f0: R_ARM_CALL	AeRoutePutBusy
  f4:	e59739e0 	ldr	r3, [r7, #2528]	; 0x9e0
  f8:	e1530005 	cmp	r3, r5
  fc:	8affffd6 	bhi	5c <AeRouteSFUpdate+0x5c>
 100:	e59d2004 	ldr	r2, [sp, #4]
 104:	e30334b8 	movw	r3, #13496	; 0x34b8
 108:	e2844ed7 	add	r4, r4, #3440	; 0xd70
 10c:	e0282893 	mla	r8, r3, r8, r2
 110:	e2844008 	add	r4, r4, #8
 114:	e5980d78 	ldr	r0, [r8, #3448]	; 0xd78
 118:	e0825004 	add	r5, r2, r4
 11c:	e5904000 	ldr	r4, [r0]
 120:	e1500005 	cmp	r0, r5
 124:	11540005 	cmpne	r4, r5
 128:	0a00000c 	beq	160 <AeRouteSFUpdate+0x160>
 12c:	e1a02009 	mov	r2, r9
 130:	e2441020 	sub	r1, r4, #32
 134:	e2400020 	sub	r0, r0, #32
 138:	ebfffffe 	bl	0 <AeRouteSFUpdate>
			138: R_ARM_CALL	AeRouteNodeStgy
 13c:	e5943000 	ldr	r3, [r4]
 140:	e1a00004 	mov	r0, r4
 144:	e1550004 	cmp	r5, r4
 148:	11530005 	cmpne	r3, r5
 14c:	e1a04003 	mov	r4, r3
 150:	03a02001 	moveq	r2, #1
 154:	13a02000 	movne	r2, #0
 158:	e3520000 	cmp	r2, #0
 15c:	0afffff2 	beq	12c <AeRouteSFUpdate+0x12c>
 160:	e3a00000 	mov	r0, #0
 164:	e28dd00c 	add	sp, sp, #12
 168:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 16c:	00000158 	.word	0x00000158
			16c: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 170:	00000000 	.word	0x00000000
			170: R_ARM_GOT32	g_astAeCtx

Disassembly of section .text.AeRouteUpdate:

00000000 <AeRouteUpdate>:
   0:	e59f3164 	ldr	r3, [pc, #356]	; 16c <AeRouteUpdate+0x16c>
   4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   8:	e30344b8 	movw	r4, #13496	; 0x34b8
   c:	e08f3003 	add	r3, pc, r3
  10:	e24dd00c 	sub	sp, sp, #12
  14:	e59f2154 	ldr	r2, [pc, #340]	; 170 <AeRouteUpdate+0x170>
  18:	e1a08000 	mov	r8, r0
  1c:	e0040094 	mul	r4, r4, r0
  20:	e7933002 	ldr	r3, [r3, r2]
  24:	e1a09001 	mov	r9, r1
  28:	e2846e73 	add	r6, r4, #1840	; 0x730
  2c:	e58d3004 	str	r3, [sp, #4]
  30:	e1a05003 	mov	r5, r3
  34:	e085a004 	add	sl, r5, r4
  38:	e2866008 	add	r6, r6, #8
  3c:	e0836006 	add	r6, r3, r6
  40:	e1a00006 	mov	r0, r6
  44:	ebfffffe 	bl	0 <AeRouteUpdate>
			44: R_ARM_CALL	AeRouteInit
  48:	e59a3630 	ldr	r3, [sl, #1584]	; 0x630
  4c:	e3530000 	cmp	r3, #0
  50:	13a05000 	movne	r5, #0
  54:	11a0700a 	movne	r7, sl
  58:	0a000028 	beq	100 <AeRouteUpdate+0x100>
  5c:	e1a00006 	mov	r0, r6
  60:	ebfffffe 	bl	0 <AeRouteUpdate>
			60: R_ARM_CALL	AeRouteGetFree
  64:	e2501000 	subs	r1, r0, #0
  68:	0a000024 	beq	100 <AeRouteUpdate+0x100>
  6c:	e59a2638 	ldr	r2, [sl, #1592]	; 0x638
  70:	e301370c 	movw	r3, #5900	; 0x170c
  74:	e59ac634 	ldr	ip, [sl, #1588]	; 0x634
  78:	e3a00001 	mov	r0, #1
  7c:	e5812004 	str	r2, [r1, #4]
  80:	e3a0e000 	mov	lr, #0
  84:	e581c000 	str	ip, [r1]
  88:	e7973003 	ldr	r3, [r7, r3]
  8c:	e3530000 	cmp	r3, #0
  90:	03a03001 	moveq	r3, #1
  94:	059a263c 	ldreq	r2, [sl, #1596]	; 0x63c
  98:	159a3640 	ldrne	r3, [sl, #1600]	; 0x640
  9c:	01a03213 	lsleq	r3, r3, r2
  a0:	e3590000 	cmp	r9, #0
  a4:	e5813008 	str	r3, [r1, #8]
  a8:	e5810010 	str	r0, [r1, #16]
  ac:	e581e014 	str	lr, [r1, #20]
  b0:	0a00000b 	beq	e4 <AeRouteUpdate+0xe4>
  b4:	e1a02000 	mov	r2, r0
  b8:	e1a0300e 	mov	r3, lr
  bc:	e2810004 	add	r0, r1, #4
  c0:	ea000000 	b	c8 <AeRouteUpdate+0xc8>
  c4:	e490c004 	ldr	ip, [r0], #4
  c8:	e28ee001 	add	lr, lr, #1
  cc:	e00b039c 	mul	fp, ip, r3
  d0:	e15e0009 	cmp	lr, r9
  d4:	e0832c92 	umull	r2, r3, r2, ip
  d8:	e08b3003 	add	r3, fp, r3
  dc:	1afffff8 	bne	c4 <AeRouteUpdate+0xc4>
  e0:	e1c121f0 	strd	r2, [r1, #16]
  e4:	e1a00006 	mov	r0, r6
  e8:	e2855001 	add	r5, r5, #1
  ec:	e28aa010 	add	sl, sl, #16
  f0:	ebfffffe 	bl	0 <AeRouteUpdate>
			f0: R_ARM_CALL	AeRoutePutBusy
  f4:	e5973630 	ldr	r3, [r7, #1584]	; 0x630
  f8:	e1530005 	cmp	r3, r5
  fc:	8affffd6 	bhi	5c <AeRouteUpdate+0x5c>
 100:	e59d2004 	ldr	r2, [sp, #4]
 104:	e30334b8 	movw	r3, #13496	; 0x34b8
 108:	e2844d27 	add	r4, r4, #2496	; 0x9c0
 10c:	e0282893 	mla	r8, r3, r8, r2
 110:	e2844008 	add	r4, r4, #8
 114:	e59809c8 	ldr	r0, [r8, #2504]	; 0x9c8
 118:	e0825004 	add	r5, r2, r4
 11c:	e5904000 	ldr	r4, [r0]
 120:	e1500005 	cmp	r0, r5
 124:	11540005 	cmpne	r4, r5
 128:	0a00000c 	beq	160 <AeRouteUpdate+0x160>
 12c:	e1a02009 	mov	r2, r9
 130:	e2441020 	sub	r1, r4, #32
 134:	e2400020 	sub	r0, r0, #32
 138:	ebfffffe 	bl	0 <AeRouteUpdate>
			138: R_ARM_CALL	AeRouteNodeStgy
 13c:	e5943000 	ldr	r3, [r4]
 140:	e1a00004 	mov	r0, r4
 144:	e1550004 	cmp	r5, r4
 148:	11530005 	cmpne	r3, r5
 14c:	e1a04003 	mov	r4, r3
 150:	03a02001 	moveq	r2, #1
 154:	13a02000 	movne	r2, #0
 158:	e3520000 	cmp	r2, #0
 15c:	0afffff2 	beq	12c <AeRouteUpdate+0x12c>
 160:	e3a00000 	mov	r0, #0
 164:	e28dd00c 	add	sp, sp, #12
 168:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 16c:	00000158 	.word	0x00000158
			16c: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 170:	00000000 	.word	0x00000000
			170: R_ARM_GOT32	g_astAeCtx

Disassembly of section .text.AeRouteGetJoint:

00000000 <AeRouteGetJoint>:
   0:	e5b0c290 	ldr	ip, [r0, #656]!	; 0x290
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
  2c:	0a000010 	beq	74 <AeRouteGetJoint+0x74>
  30:	e1a06002 	mov	r6, r2
  34:	e24c2020 	sub	r2, ip, #32
  38:	e2431020 	sub	r1, r3, #32
  3c:	e1c241d0 	ldrd	r4, [r2, #16]
  40:	e1a0c003 	mov	ip, r3
  44:	e1550007 	cmp	r5, r7
  48:	01540006 	cmpeq	r4, r6
  4c:	8a000003 	bhi	60 <AeRouteGetJoint+0x60>
  50:	e14341d0 	ldrd	r4, [r3, #-16]
  54:	e1570005 	cmp	r7, r5
  58:	01560004 	cmpeq	r6, r4
  5c:	9a000006 	bls	7c <AeRouteGetJoint+0x7c>
  60:	e5931000 	ldr	r1, [r3]
  64:	e1530000 	cmp	r3, r0
  68:	11510000 	cmpne	r1, r0
  6c:	e1a03001 	mov	r3, r1
  70:	1affffef 	bne	34 <AeRouteGetJoint+0x34>
  74:	e3a00000 	mov	r0, #0
  78:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
  7c:	e58e2000 	str	r2, [lr]
  80:	e3a00000 	mov	r0, #0
  84:	e5881000 	str	r1, [r8]
  88:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

Disassembly of section .text.AeRouteGetUpNode:

00000000 <AeRouteGetUpNode>:
   0:	e5913020 	ldr	r3, [r1, #32]
   4:	e2800e29 	add	r0, r0, #656	; 0x290
   8:	e1530000 	cmp	r3, r0
   c:	12430020 	subne	r0, r3, #32
  10:	03a00000 	moveq	r0, #0
  14:	e12fff1e 	bx	lr

Disassembly of section .text.AeRouteGetDwNode:

00000000 <AeRouteGetDwNode>:
   0:	e5913024 	ldr	r3, [r1, #36]	; 0x24
   4:	e2800e29 	add	r0, r0, #656	; 0x290
   8:	e1530000 	cmp	r3, r0
   c:	12430020 	subne	r0, r3, #32
  10:	03a00000 	moveq	r0, #0
  14:	e12fff1e 	bx	lr

Disassembly of section .text.AeRouteGetFirstNode:

00000000 <AeRouteGetFirstNode>:
   0:	e5b03290 	ldr	r3, [r0, #656]!	; 0x290
   4:	e1530000 	cmp	r3, r0
   8:	12430020 	subne	r0, r3, #32
   c:	03a00000 	moveq	r0, #0
  10:	e12fff1e 	bx	lr

Disassembly of section .text.AeRoutePrint:

00000000 <AeRoutePrint>:
   0:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
   4:	e1a06000 	mov	r6, r0
   8:	e24dd01c 	sub	sp, sp, #28
   c:	ebfffffe 	bl	0 <AeRoutePrint>
			c: R_ARM_CALL	AeRouteGetFirstNode
  10:	e5963280 	ldr	r3, [r6, #640]	; 0x280
  14:	e3530000 	cmp	r3, #0
  18:	0a000022 	beq	a8 <AeRoutePrint+0xa8>
  1c:	e3500000 	cmp	r0, #0
  20:	e1a04000 	mov	r4, r0
  24:	01a05000 	moveq	r5, r0
  28:	0a00001a 	beq	98 <AeRoutePrint+0x98>
  2c:	e59f707c 	ldr	r7, [pc, #124]	; b0 <AeRoutePrint+0xb0>
  30:	e3a05000 	mov	r5, #0
  34:	e08f7007 	add	r7, pc, r7
  38:	e1c481d0 	ldrd	r8, [r4, #16]
  3c:	e5943004 	ldr	r3, [r4, #4]
  40:	e1a01005 	mov	r1, r5
  44:	e5942000 	ldr	r2, [r4]
  48:	e1a00007 	mov	r0, r7
  4c:	e1cd81f0 	strd	r8, [sp, #16]
  50:	e594c01c 	ldr	ip, [r4, #28]
  54:	e2855001 	add	r5, r5, #1
  58:	e6ef5075 	uxtb	r5, r5
  5c:	e58dc008 	str	ip, [sp, #8]
  60:	e594c018 	ldr	ip, [r4, #24]
  64:	e58dc004 	str	ip, [sp, #4]
  68:	e594c008 	ldr	ip, [r4, #8]
  6c:	e58dc000 	str	ip, [sp]
  70:	ebfffffe 	bl	0 <printf>
			70: R_ARM_CALL	printf
  74:	e1a01004 	mov	r1, r4
  78:	e1a00006 	mov	r0, r6
  7c:	ebfffffe 	bl	0 <AeRoutePrint>
			7c: R_ARM_CALL	AeRouteGetUpNode
  80:	e5963280 	ldr	r3, [r6, #640]	; 0x280
  84:	e1a04000 	mov	r4, r0
  88:	e1550003 	cmp	r5, r3
  8c:	2a000005 	bcs	a8 <AeRoutePrint+0xa8>
  90:	e3540000 	cmp	r4, #0
  94:	1affffe7 	bne	38 <AeRoutePrint+0x38>
  98:	e2855001 	add	r5, r5, #1
  9c:	e6ef5075 	uxtb	r5, r5
  a0:	e1530005 	cmp	r3, r5
  a4:	8afffffb 	bhi	98 <AeRoutePrint+0x98>
  a8:	e28dd01c 	add	sp, sp, #28
  ac:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
  b0:	00000074 	.word	0x00000074
			b0: R_ARM_REL32	.LC1

Disassembly of section .text.AeRouteDelRdcy:

00000000 <AeRouteDelRdcy>:
   0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   4:	e1a04000 	mov	r4, r0
   8:	e5b42290 	ldr	r2, [r4, #656]!	; 0x290
   c:	e5923000 	ldr	r3, [r2]
  10:	e1530004 	cmp	r3, r4
  14:	11520004 	cmpne	r2, r4
  18:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
  1c:	e2807fa2 	add	r7, r0, #648	; 0x288
  20:	ea00000a 	b	50 <AeRouteDelRdcy+0x50>
  24:	e35c0003 	cmp	ip, #3
  28:	0a000025 	beq	c4 <AeRouteDelRdcy+0xc4>
  2c:	e795510c 	ldr	r5, [r5, ip, lsl #2]
  30:	e79ec10c 	ldr	ip, [lr, ip, lsl #2]
  34:	e155000c 	cmp	r5, ip
  38:	0a00000a 	beq	68 <AeRouteDelRdcy+0x68>
  3c:	e1a02003 	mov	r2, r3
  40:	e5923000 	ldr	r3, [r2]
  44:	e1530004 	cmp	r3, r4
  48:	11520004 	cmpne	r2, r4
  4c:	0a00001b 	beq	c0 <AeRouteDelRdcy+0xc0>
  50:	e512c008 	ldr	ip, [r2, #-8]
  54:	e242e020 	sub	lr, r2, #32
  58:	e5136008 	ldr	r6, [r3, #-8]
  5c:	e2435020 	sub	r5, r3, #32
  60:	e15c0006 	cmp	ip, r6
  64:	1affffee 	bne	24 <AeRouteDelRdcy+0x24>
  68:	e8931020 	ldm	r3, {r5, ip}
  6c:	e58e6018 	str	r6, [lr, #24]
  70:	e585c004 	str	ip, [r5, #4]
  74:	e58c5000 	str	r5, [ip]
  78:	e5906288 	ldr	r6, [r0, #648]	; 0x288
  7c:	e590e280 	ldr	lr, [r0, #640]	; 0x280
  80:	e590c284 	ldr	ip, [r0, #644]	; 0x284
  84:	e5863004 	str	r3, [r6, #4]
  88:	e24ee001 	sub	lr, lr, #1
  8c:	e5837004 	str	r7, [r3, #4]
  90:	e28cc001 	add	ip, ip, #1
  94:	e5925004 	ldr	r5, [r2, #4]
  98:	e5836000 	str	r6, [r3]
  9c:	e1550004 	cmp	r5, r4
  a0:	e5803288 	str	r3, [r0, #648]	; 0x288
  a4:	11a02005 	movne	r2, r5
  a8:	e580e280 	str	lr, [r0, #640]	; 0x280
  ac:	e5923000 	ldr	r3, [r2]
  b0:	e580c284 	str	ip, [r0, #644]	; 0x284
  b4:	e1530004 	cmp	r3, r4
  b8:	11520004 	cmpne	r2, r4
  bc:	1affffe3 	bne	50 <AeRouteDelRdcy+0x50>
  c0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
  c4:	e59e8000 	ldr	r8, [lr]
  c8:	e3510002 	cmp	r1, #2
  cc:	e595c000 	ldr	ip, [r5]
  d0:	0a00000a 	beq	100 <AeRouteDelRdcy+0x100>
  d4:	e158000c 	cmp	r8, ip
  d8:	1affffd7 	bne	3c <AeRouteDelRdcy+0x3c>
  dc:	e5958004 	ldr	r8, [r5, #4]
  e0:	e59ec004 	ldr	ip, [lr, #4]
  e4:	e158000c 	cmp	r8, ip
  e8:	1affffd3 	bne	3c <AeRouteDelRdcy+0x3c>
  ec:	e5955008 	ldr	r5, [r5, #8]
  f0:	e59ec008 	ldr	ip, [lr, #8]
  f4:	e155000c 	cmp	r5, ip
  f8:	1affffcf 	bne	3c <AeRouteDelRdcy+0x3c>
  fc:	eaffffd9 	b	68 <AeRouteDelRdcy+0x68>
 100:	e158000c 	cmp	r8, ip
 104:	1affffcc 	bne	3c <AeRouteDelRdcy+0x3c>
 108:	e5955004 	ldr	r5, [r5, #4]
 10c:	e59ec004 	ldr	ip, [lr, #4]
 110:	e155000c 	cmp	r5, ip
 114:	1affffc8 	bne	3c <AeRouteDelRdcy+0x3c>
 118:	eaffffd2 	b	68 <AeRouteDelRdcy+0x68>

Disassembly of section .text.AeRouteLimit:

00000000 <AeRouteLimit>:
   0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
   4:	e1a09001 	mov	r9, r1
   8:	e1a01000 	mov	r1, r0
   c:	e24dd034 	sub	sp, sp, #52	; 0x34
  10:	e5b1a290 	ldr	sl, [r1, #656]!	; 0x290
  14:	e58d002c 	str	r0, [sp, #44]	; 0x2c
  18:	e15a0001 	cmp	sl, r1
  1c:	e58d2024 	str	r2, [sp, #36]	; 0x24
  20:	e58d101c 	str	r1, [sp, #28]
  24:	e58d3028 	str	r3, [sp, #40]	; 0x28
  28:	e59ab000 	ldr	fp, [sl]
  2c:	0a00005a 	beq	19c <AeRouteLimit+0x19c>
  30:	e59d301c 	ldr	r3, [sp, #28]
  34:	e153000b 	cmp	r3, fp
  38:	e24a3020 	sub	r3, sl, #32
  3c:	e58d3018 	str	r3, [sp, #24]
  40:	124b3020 	subne	r3, fp, #32
  44:	059d3018 	ldreq	r3, [sp, #24]
  48:	e3590000 	cmp	r9, #0
  4c:	e58d3020 	str	r3, [sp, #32]
  50:	0a000057 	beq	1b4 <AeRouteLimit+0x1b4>
  54:	e59d3018 	ldr	r3, [sp, #24]
  58:	e3a06000 	mov	r6, #0
  5c:	e3a04001 	mov	r4, #1
  60:	e3a05000 	mov	r5, #0
  64:	e2437004 	sub	r7, r3, #4
  68:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
  6c:	e243a004 	sub	sl, r3, #4
  70:	e59d3028 	ldr	r3, [sp, #40]	; 0x28
  74:	e2438004 	sub	r8, r3, #4
  78:	e5970004 	ldr	r0, [r7, #4]
  7c:	e2866001 	add	r6, r6, #1
  80:	e5b82004 	ldr	r2, [r8, #4]!
  84:	e5ba1004 	ldr	r1, [sl, #4]!
  88:	ebfffffe 	bl	0 <AeRouteLimit>
			88: R_ARM_CALL	AeBoundariesCheck
  8c:	e0030590 	mul	r3, r0, r5
  90:	e1560009 	cmp	r6, r9
  94:	e0854094 	umull	r4, r5, r4, r0
  98:	e5a70004 	str	r0, [r7, #4]!
  9c:	e0835005 	add	r5, r3, r5
  a0:	1afffff4 	bne	78 <AeRouteLimit+0x78>
  a4:	e59d0018 	ldr	r0, [sp, #24]
  a8:	e1c021d0 	ldrd	r2, [r0, #16]
  ac:	e1530005 	cmp	r3, r5
  b0:	01520004 	cmpeq	r2, r4
  b4:	0a000007 	beq	d8 <AeRouteLimit+0xd8>
  b8:	e1cd21f0 	strd	r2, [sp, #16]
  bc:	e59d3028 	ldr	r3, [sp, #40]	; 0x28
  c0:	e1a02009 	mov	r2, r9
  c4:	e1cd40f8 	strd	r4, [sp, #8]
  c8:	e58d3000 	str	r3, [sp]
  cc:	e59d1020 	ldr	r1, [sp, #32]
  d0:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
  d4:	ebfffffe 	bl	0 <AeRouteLimit>
			d4: R_ARM_CALL	AeRouteNodeMultiCheck
  d8:	e59d301c 	ldr	r3, [sp, #28]
  dc:	e1a0a00b 	mov	sl, fp
  e0:	e153000b 	cmp	r3, fp
  e4:	e59b3000 	ldr	r3, [fp]
  e8:	11a0b003 	movne	fp, r3
  ec:	1affffcf 	bne	30 <AeRouteLimit+0x30>
  f0:	e59d302c 	ldr	r3, [sp, #44]	; 0x2c
  f4:	e5930290 	ldr	r0, [r3, #656]	; 0x290
  f8:	e59d301c 	ldr	r3, [sp, #28]
  fc:	e5904000 	ldr	r4, [r0]
 100:	e1500003 	cmp	r0, r3
 104:	11540003 	cmpne	r4, r3
 108:	0a000023 	beq	19c <AeRouteLimit+0x19c>
 10c:	e59d502c 	ldr	r5, [sp, #44]	; 0x2c
 110:	e1a02005 	mov	r2, r5
 114:	e1a03009 	mov	r3, r9
 118:	e2441020 	sub	r1, r4, #32
 11c:	e2400020 	sub	r0, r0, #32
 120:	ebfffffe 	bl	0 <AeRouteLimit>
			120: R_ARM_CALL	AeRouteNodeRecurit
 124:	e5943000 	ldr	r3, [r4]
 128:	e1a00004 	mov	r0, r4
 12c:	e15b0004 	cmp	fp, r4
 130:	1153000b 	cmpne	r3, fp
 134:	e1a04003 	mov	r4, r3
 138:	03a02001 	moveq	r2, #1
 13c:	13a02000 	movne	r2, #0
 140:	e3520000 	cmp	r2, #0
 144:	0afffff1 	beq	110 <AeRouteLimit+0x110>
 148:	e59d302c 	ldr	r3, [sp, #44]	; 0x2c
 14c:	e5930290 	ldr	r0, [r3, #656]	; 0x290
 150:	e59d301c 	ldr	r3, [sp, #28]
 154:	e5904000 	ldr	r4, [r0]
 158:	e1530000 	cmp	r3, r0
 15c:	11530004 	cmpne	r3, r4
 160:	0a00000d 	beq	19c <AeRouteLimit+0x19c>
 164:	e1a05003 	mov	r5, r3
 168:	e1a02009 	mov	r2, r9
 16c:	e2441020 	sub	r1, r4, #32
 170:	e2400020 	sub	r0, r0, #32
 174:	ebfffffe 	bl	0 <AeRouteLimit>
			174: R_ARM_CALL	AeRouteNodeStgy
 178:	e5943000 	ldr	r3, [r4]
 17c:	e1a00004 	mov	r0, r4
 180:	e1550004 	cmp	r5, r4
 184:	11530005 	cmpne	r3, r5
 188:	e1a04003 	mov	r4, r3
 18c:	03a02001 	moveq	r2, #1
 190:	13a02000 	movne	r2, #0
 194:	e3520000 	cmp	r2, #0
 198:	0afffff2 	beq	168 <AeRouteLimit+0x168>
 19c:	e59d002c 	ldr	r0, [sp, #44]	; 0x2c
 1a0:	e1a01009 	mov	r1, r9
 1a4:	ebfffffe 	bl	0 <AeRouteLimit>
			1a4: R_ARM_CALL	AeRouteDelRdcy
 1a8:	e3a00000 	mov	r0, #0
 1ac:	e28dd034 	add	sp, sp, #52	; 0x34
 1b0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
 1b4:	e3a04001 	mov	r4, #1
 1b8:	e3a05000 	mov	r5, #0
 1bc:	eaffffb8 	b	a4 <AeRouteLimit+0xa4>
