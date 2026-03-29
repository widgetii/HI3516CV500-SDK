
as_buf.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l    df *ABS*	00000000 as_buf.c
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.as_buf_init	00000000 .text.as_buf_init
00000000 l    d  .text.as_buf_get_free	00000000 .text.as_buf_get_free
00000000 l    d  .text.as_buf_get_busy	00000000 .text.as_buf_get_busy
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.as_buf_init	000000b0 as_buf_init
00000000 g     F .text.as_buf_get_free	00000044 as_buf_get_free
00000000 g     F .text.as_buf_get_busy	00000044 as_buf_get_busy



Disassembly of section .text.as_buf_init:

00000000 <as_buf_init>:
   0:	e3500000 	cmp	r0, #0
   4:	0a000027 	beq	a8 <as_buf_init+0xa8>
   8:	e3510f4b 	cmp	r1, #300	; 0x12c
   c:	e303384c 	movw	r3, #14412	; 0x384c
  10:	e92d40f0 	push	{r4, r5, r6, r7, lr}
  14:	23a01f4b 	movcs	r1, #300	; 0x12c
  18:	e340304b 	movt	r3, #75	; 0x4b
  1c:	e3034844 	movw	r4, #14404	; 0x3844
  20:	e340404b 	movt	r4, #75	; 0x4b
  24:	e280684b 	add	r6, r0, #4915200	; 0x4b0000
  28:	e0804004 	add	r4, r0, r4
  2c:	e2866a03 	add	r6, r6, #12288	; 0x3000
  30:	e0803003 	add	r3, r0, r3
  34:	e3510000 	cmp	r1, #0
  38:	e5864844 	str	r4, [r6, #2116]	; 0x844
  3c:	e5864848 	str	r4, [r6, #2120]	; 0x848
  40:	e586384c 	str	r3, [r6, #2124]	; 0x84c
  44:	e5863850 	str	r3, [r6, #2128]	; 0x850
  48:	0a000014 	beq	a0 <as_buf_init+0xa0>
  4c:	e304c030 	movw	ip, #16432	; 0x4030
  50:	e3045028 	movw	r5, #16424	; 0x4028
  54:	e007019c 	mul	r7, ip, r1
  58:	e0803005 	add	r3, r0, r5
  5c:	e2802028 	add	r2, r0, #40	; 0x28
  60:	e0875005 	add	r5, r7, r5
  64:	e1a01004 	mov	r1, r4
  68:	e0805005 	add	r5, r0, r5
  6c:	e5022028 	str	r2, [r2, #-40]	; 0xffffffd8
  70:	e1a0e003 	mov	lr, r3
  74:	e5831004 	str	r1, [r3, #4]
  78:	e5834000 	str	r4, [r3]
  7c:	e5813000 	str	r3, [r1]
  80:	e1a01003 	mov	r1, r3
  84:	e083300c 	add	r3, r3, ip
  88:	e1530005 	cmp	r3, r5
  8c:	e082200c 	add	r2, r2, ip
  90:	1afffff5 	bne	6c <as_buf_init+0x6c>
  94:	e2471008 	sub	r1, r7, #8
  98:	e0800001 	add	r0, r0, r1
  9c:	e5860848 	str	r0, [r6, #2120]	; 0x848
  a0:	e3a00000 	mov	r0, #0
  a4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
  a8:	e3e00000 	mvn	r0, #0
  ac:	e12fff1e 	bx	lr

Disassembly of section .text.as_buf_get_free:

00000000 <as_buf_get_free>:
   0:	e3500000 	cmp	r0, #0
   4:	012fff1e 	bxeq	lr
   8:	e280284b 	add	r2, r0, #4915200	; 0x4b0000
   c:	e3033844 	movw	r3, #14404	; 0x3844
  10:	e340304b 	movt	r3, #75	; 0x4b
  14:	e2822a03 	add	r2, r2, #12288	; 0x3000
  18:	e0803003 	add	r3, r0, r3
  1c:	e5922844 	ldr	r2, [r2, #2116]	; 0x844
  20:	e1520003 	cmp	r2, r3
  24:	12420901 	subne	r0, r2, #16384	; 0x4000
  28:	15923004 	ldrne	r3, [r2, #4]
  2c:	12400028 	subne	r0, r0, #40	; 0x28
  30:	15922000 	ldrne	r2, [r2]
  34:	03a00000 	moveq	r0, #0
  38:	15823004 	strne	r3, [r2, #4]
  3c:	15832000 	strne	r2, [r3]
  40:	e12fff1e 	bx	lr

Disassembly of section .text.as_buf_get_busy:

00000000 <as_buf_get_busy>:
   0:	e3500000 	cmp	r0, #0
   4:	012fff1e 	bxeq	lr
   8:	e280284b 	add	r2, r0, #4915200	; 0x4b0000
   c:	e303384c 	movw	r3, #14412	; 0x384c
  10:	e340304b 	movt	r3, #75	; 0x4b
  14:	e2822a03 	add	r2, r2, #12288	; 0x3000
  18:	e0803003 	add	r3, r0, r3
  1c:	e592284c 	ldr	r2, [r2, #2124]	; 0x84c
  20:	e1520003 	cmp	r2, r3
  24:	12420901 	subne	r0, r2, #16384	; 0x4000
  28:	15923004 	ldrne	r3, [r2, #4]
  2c:	12400028 	subne	r0, r0, #40	; 0x28
  30:	15922000 	ldrne	r2, [r2]
  34:	03a00000 	moveq	r0, #0
  38:	15823004 	strne	r3, [r2, #4]
  3c:	15832000 	strne	r2, [r3]
  40:	e12fff1e 	bx	lr
