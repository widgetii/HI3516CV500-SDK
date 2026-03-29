
mpi_gdc.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l    df *ABS*	00000000 mpi_gdc.c
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.mpi_gdc_check_open	00000000 .text.mpi_gdc_check_open
00000000 l     F .text.mpi_gdc_check_open	0000009c mpi_gdc_check_open
00000000 l       .rodata.mpi_gdc_check_open.str1.4	00000000 .LC0
0000000c l       .rodata.mpi_gdc_check_open.str1.4	00000000 .LC1
00000000 l    d  .text.hi_mpi_gdc_add_pmf_task	00000000 .text.hi_mpi_gdc_add_pmf_task
00000000 l     F .text.hi_mpi_gdc_add_pmf_task	00000124 hi_mpi_gdc_add_pmf_task
00000000 l       .rodata.hi_mpi_gdc_add_pmf_task.str1.4	00000000 .LC2
00000000 l    d  .text.hi_mpi_gdc_fisheye_pos_query_dst_to_src	00000000 .text.hi_mpi_gdc_fisheye_pos_query_dst_to_src
00000000 l    d  .text.HI_MPI_GDC_BeginJob	00000000 .text.HI_MPI_GDC_BeginJob
00000000 l    d  .text.HI_MPI_GDC_EndJob	00000000 .text.HI_MPI_GDC_EndJob
00000000 l    d  .text.HI_MPI_GDC_CancelJob	00000000 .text.HI_MPI_GDC_CancelJob
00000000 l    d  .text.HI_MPI_GDC_SetConfig	00000000 .text.HI_MPI_GDC_SetConfig
00000000 l    d  .text.HI_MPI_GDC_AddCorrectionTask	00000000 .text.HI_MPI_GDC_AddCorrectionTask
00000000 l    d  .text.HI_MPI_GDC_AddCorrectionExTask	00000000 .text.HI_MPI_GDC_AddCorrectionExTask
00000000 l    d  .text.HI_MPI_GDC_AddPMFTask	00000000 .text.HI_MPI_GDC_AddPMFTask
00000000 l    d  .text.HI_MPI_GDC_FisheyePosQueryDst2Src	00000000 .text.HI_MPI_GDC_FisheyePosQueryDst2Src
00000000 l    d  .bss.g_gdc_mutex	00000000 .bss.g_gdc_mutex
00000000 l     O .bss.g_gdc_mutex	00000018 g_gdc_mutex
00000000 l    d  .data.g_gdc_mpi_fd	00000000 .data.g_gdc_mpi_fd
00000000 l     O .data.g_gdc_mpi_fd	00000004 g_gdc_mpi_fd
00000000 l    d  .rodata.__FUNCTION__.10547	00000000 .rodata.__FUNCTION__.10547
00000000 l     O .rodata.__FUNCTION__.10547	00000013 __FUNCTION__.10547
00000000 l    d  .rodata.__FUNCTION__.10551	00000000 .rodata.__FUNCTION__.10551
00000000 l     O .rodata.__FUNCTION__.10551	00000017 __FUNCTION__.10551
00000000 l    d  .rodata.hi_mpi_gdc_add_pmf_task.str1.4	00000000 .rodata.hi_mpi_gdc_add_pmf_task.str1.4
00000000 l    d  .rodata.mpi_gdc_check_open.str1.4	00000000 .rodata.mpi_gdc_check_open.str1.4
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000         *UND*	00000000 pthread_mutex_lock
00000000         *UND*	00000000 pthread_mutex_unlock
00000000         *UND*	00000000 open
00000000         *UND*	00000000 printf
00000000         *UND*	00000000 memset
00000000         *UND*	00000000 memcpy_s
00000000         *UND*	00000000 ioctl
00000000         *UND*	00000000 __stack_chk_fail
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 __stack_chk_guard
00000000 g     F .text.hi_mpi_gdc_fisheye_pos_query_dst_to_src	0000000c hi_mpi_gdc_fisheye_pos_query_dst_to_src
00000000 g     F .text.HI_MPI_GDC_BeginJob	00000074 HI_MPI_GDC_BeginJob
00000000 g     F .text.HI_MPI_GDC_EndJob	00000098 HI_MPI_GDC_EndJob
00000000 g     F .text.HI_MPI_GDC_CancelJob	00000098 HI_MPI_GDC_CancelJob
00000000 g     F .text.HI_MPI_GDC_SetConfig	0000000c HI_MPI_GDC_SetConfig
00000000 g     F .text.HI_MPI_GDC_AddCorrectionTask	0000000c HI_MPI_GDC_AddCorrectionTask
00000000 g     F .text.HI_MPI_GDC_AddCorrectionExTask	0000000c HI_MPI_GDC_AddCorrectionExTask
00000000 g     F .text.HI_MPI_GDC_AddPMFTask	00000004 HI_MPI_GDC_AddPMFTask
00000000 g     F .text.HI_MPI_GDC_FisheyePosQueryDst2Src	00000004 HI_MPI_GDC_FisheyePosQueryDst2Src



Disassembly of section .text.mpi_gdc_check_open:

00000000 <mpi_gdc_check_open>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e59f5078 	ldr	r5, [pc, #120]	; 84 <mpi_gdc_check_open+0x84>
   8:	e59f4078 	ldr	r4, [pc, #120]	; 88 <mpi_gdc_check_open+0x88>
   c:	e08f5005 	add	r5, pc, r5
  10:	e08f4004 	add	r4, pc, r4
  14:	e1a00005 	mov	r0, r5
  18:	ebfffffe 	bl	0 <pthread_mutex_lock>
			18: R_ARM_CALL	pthread_mutex_lock
  1c:	e5943000 	ldr	r3, [r4]
  20:	e3730001 	cmn	r3, #1
  24:	0a000004 	beq	3c <mpi_gdc_check_open+0x3c>
  28:	e59f005c 	ldr	r0, [pc, #92]	; 8c <mpi_gdc_check_open+0x8c>
  2c:	e08f0000 	add	r0, pc, r0
  30:	ebfffffe 	bl	0 <pthread_mutex_unlock>
			30: R_ARM_CALL	pthread_mutex_unlock
  34:	e3a00000 	mov	r0, #0
  38:	e8bd8070 	pop	{r4, r5, r6, pc}
  3c:	e59f004c 	ldr	r0, [pc, #76]	; 90 <mpi_gdc_check_open+0x90>
  40:	e3a02000 	mov	r2, #0
  44:	e3a01002 	mov	r1, #2
  48:	e08f0000 	add	r0, pc, r0
  4c:	ebfffffe 	bl	0 <open>
			4c: R_ARM_CALL	open
  50:	e5840000 	str	r0, [r4]
  54:	e3700001 	cmn	r0, #1
  58:	1afffff2 	bne	28 <mpi_gdc_check_open+0x28>
  5c:	e1a00005 	mov	r0, r5
  60:	ebfffffe 	bl	0 <pthread_mutex_unlock>
			60: R_ARM_CALL	pthread_mutex_unlock
  64:	e59f1028 	ldr	r1, [pc, #40]	; 94 <mpi_gdc_check_open+0x94>
  68:	e59f0028 	ldr	r0, [pc, #40]	; 98 <mpi_gdc_check_open+0x98>
  6c:	e08f1001 	add	r1, pc, r1
  70:	e08f0000 	add	r0, pc, r0
  74:	ebfffffe 	bl	0 <printf>
			74: R_ARM_CALL	printf
  78:	e3080010 	movw	r0, #32784	; 0x8010
  7c:	e34a0025 	movt	r0, #40997	; 0xa025
  80:	e8bd8070 	pop	{r4, r5, r6, pc}
  84:	00000070 	.word	0x00000070
			84: R_ARM_REL32	.bss.g_gdc_mutex
  88:	00000070 	.word	0x00000070
			88: R_ARM_REL32	.data.g_gdc_mpi_fd
  8c:	00000058 	.word	0x00000058
			8c: R_ARM_REL32	.bss.g_gdc_mutex
  90:	00000040 	.word	0x00000040
			90: R_ARM_REL32	.LC0
  94:	00000020 	.word	0x00000020
			94: R_ARM_REL32	.rodata.__FUNCTION__.10547
  98:	00000020 	.word	0x00000020
			98: R_ARM_REL32	.LC1

Disassembly of section .text.hi_mpi_gdc_add_pmf_task:

00000000 <hi_mpi_gdc_add_pmf_task>:
   0:	e59fc104 	ldr	ip, [pc, #260]	; 10c <hi_mpi_gdc_add_pmf_task+0x10c>
   4:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   8:	e24ddd15 	sub	sp, sp, #1344	; 0x540
   c:	e08fc00c 	add	ip, pc, ip
  10:	e24dd008 	sub	sp, sp, #8
  14:	e59f30f4 	ldr	r3, [pc, #244]	; 110 <hi_mpi_gdc_add_pmf_task+0x110>
  18:	e28d5008 	add	r5, sp, #8
  1c:	e2454008 	sub	r4, r5, #8
  20:	e1a08000 	mov	r8, r0
  24:	e79c3003 	ldr	r3, [ip, r3]
  28:	e1a06001 	mov	r6, r1
  2c:	e1a07002 	mov	r7, r2
  30:	e3a01000 	mov	r1, #0
  34:	e3a02d15 	mov	r2, #1344	; 0x540
  38:	e1a00004 	mov	r0, r4
  3c:	e5933000 	ldr	r3, [r3]
  40:	e58d3544 	str	r3, [sp, #1348]	; 0x544
  44:	e3a03000 	mov	r3, #0
  48:	ebfffffe 	bl	0 <memset>
			48: R_ARM_CALL	memset
  4c:	ebfffffe 	bl	0 <hi_mpi_gdc_add_pmf_task>
			4c: R_ARM_CALL	mpi_gdc_check_open
  50:	e3500000 	cmp	r0, #0
  54:	1a000028 	bne	fc <hi_mpi_gdc_add_pmf_task+0xfc>
  58:	e3560000 	cmp	r6, #0
  5c:	0a00001e 	beq	dc <hi_mpi_gdc_add_pmf_task+0xdc>
  60:	e3570000 	cmp	r7, #0
  64:	0a00001c 	beq	dc <hi_mpi_gdc_add_pmf_task+0xdc>
  68:	e58d8000 	str	r8, [sp]
  6c:	e3a03fb2 	mov	r3, #712	; 0x2c8
  70:	e1a01003 	mov	r1, r3
  74:	e1a02006 	mov	r2, r6
  78:	e1a00005 	mov	r0, r5
  7c:	ebfffffe 	bl	0 <memcpy_s>
			7c: R_ARM_CALL	memcpy_s
  80:	e3a03048 	mov	r3, #72	; 0x48
  84:	e1a02007 	mov	r2, r7
  88:	e1a01003 	mov	r1, r3
  8c:	e28d0fb6 	add	r0, sp, #728	; 0x2d8
  90:	ebfffffe 	bl	0 <memcpy_s>
			90: R_ARM_CALL	memcpy_s
  94:	e59f3078 	ldr	r3, [pc, #120]	; 114 <hi_mpi_gdc_add_pmf_task+0x114>
  98:	e3041e05 	movw	r1, #19973	; 0x4e05
  9c:	e3441540 	movt	r1, #17728	; 0x4540
  a0:	e1a02004 	mov	r2, r4
  a4:	e08f3003 	add	r3, pc, r3
  a8:	e5930000 	ldr	r0, [r3]
  ac:	ebfffffe 	bl	0 <ioctl>
			ac: R_ARM_CALL	ioctl
  b0:	e59f2060 	ldr	r2, [pc, #96]	; 118 <hi_mpi_gdc_add_pmf_task+0x118>
  b4:	e59f3054 	ldr	r3, [pc, #84]	; 110 <hi_mpi_gdc_add_pmf_task+0x110>
  b8:	e08f2002 	add	r2, pc, r2
  bc:	e7923003 	ldr	r3, [r2, r3]
  c0:	e5932000 	ldr	r2, [r3]
  c4:	e59d3544 	ldr	r3, [sp, #1348]	; 0x544
  c8:	e0332002 	eors	r2, r3, r2
  cc:	1a00000d 	bne	108 <hi_mpi_gdc_add_pmf_task+0x108>
  d0:	e28ddd15 	add	sp, sp, #1344	; 0x540
  d4:	e28dd008 	add	sp, sp, #8
  d8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
  dc:	e59f1038 	ldr	r1, [pc, #56]	; 11c <hi_mpi_gdc_add_pmf_task+0x11c>
  e0:	e59f0038 	ldr	r0, [pc, #56]	; 120 <hi_mpi_gdc_add_pmf_task+0x120>
  e4:	e08f1001 	add	r1, pc, r1
  e8:	e08f0000 	add	r0, pc, r0
  ec:	ebfffffe 	bl	0 <printf>
			ec: R_ARM_CALL	printf
  f0:	e3080006 	movw	r0, #32774	; 0x8006
  f4:	e34a0025 	movt	r0, #40997	; 0xa025
  f8:	eaffffec 	b	b0 <hi_mpi_gdc_add_pmf_task+0xb0>
  fc:	e3080010 	movw	r0, #32784	; 0x8010
 100:	e34a0025 	movt	r0, #40997	; 0xa025
 104:	eaffffe9 	b	b0 <hi_mpi_gdc_add_pmf_task+0xb0>
 108:	ebfffffe 	bl	0 <__stack_chk_fail>
			108: R_ARM_CALL	__stack_chk_fail
 10c:	000000f8 	.word	0x000000f8
			10c: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 110:	00000000 	.word	0x00000000
			110: R_ARM_GOT32	__stack_chk_guard
 114:	00000068 	.word	0x00000068
			114: R_ARM_REL32	.data.g_gdc_mpi_fd
 118:	00000058 	.word	0x00000058
			118: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
 11c:	00000030 	.word	0x00000030
			11c: R_ARM_REL32	.rodata.__FUNCTION__.10551
 120:	00000030 	.word	0x00000030
			120: R_ARM_REL32	.LC2

Disassembly of section .text.hi_mpi_gdc_fisheye_pos_query_dst_to_src:

00000000 <hi_mpi_gdc_fisheye_pos_query_dst_to_src>:
   0:	e3080008 	movw	r0, #32776	; 0x8008
   4:	e34a0025 	movt	r0, #40997	; 0xa025
   8:	e12fff1e 	bx	lr

Disassembly of section .text.HI_MPI_GDC_BeginJob:

00000000 <HI_MPI_GDC_BeginJob>:
   0:	e92d4010 	push	{r4, lr}
   4:	e1a04000 	mov	r4, r0
   8:	ebfffffe 	bl	0 <HI_MPI_GDC_BeginJob>
			8: R_ARM_CALL	mpi_gdc_check_open
   c:	e3500000 	cmp	r0, #0
  10:	1a000011 	bne	5c <HI_MPI_GDC_BeginJob+0x5c>
  14:	e3540000 	cmp	r4, #0
  18:	0a000007 	beq	3c <HI_MPI_GDC_BeginJob+0x3c>
  1c:	e59f3044 	ldr	r3, [pc, #68]	; 68 <HI_MPI_GDC_BeginJob+0x68>
  20:	e3a01c4e 	mov	r1, #19968	; 0x4e00
  24:	e3481004 	movt	r1, #32772	; 0x8004
  28:	e1a02004 	mov	r2, r4
  2c:	e08f3003 	add	r3, pc, r3
  30:	e8bd4010 	pop	{r4, lr}
  34:	e5930000 	ldr	r0, [r3]
  38:	eafffffe 	b	0 <ioctl>
			38: R_ARM_JUMP24	ioctl
  3c:	e59f1028 	ldr	r1, [pc, #40]	; 6c <HI_MPI_GDC_BeginJob+0x6c>
  40:	e59f0028 	ldr	r0, [pc, #40]	; 70 <HI_MPI_GDC_BeginJob+0x70>
  44:	e08f1001 	add	r1, pc, r1
  48:	e08f0000 	add	r0, pc, r0
  4c:	ebfffffe 	bl	0 <printf>
			4c: R_ARM_CALL	printf
  50:	e3080006 	movw	r0, #32774	; 0x8006
  54:	e34a0025 	movt	r0, #40997	; 0xa025
  58:	e8bd8010 	pop	{r4, pc}
  5c:	e3080010 	movw	r0, #32784	; 0x8010
  60:	e34a0025 	movt	r0, #40997	; 0xa025
  64:	e8bd8010 	pop	{r4, pc}
  68:	00000034 	.word	0x00000034
			68: R_ARM_REL32	.data.g_gdc_mpi_fd
  6c:	00000020 	.word	0x00000020
			6c: R_ARM_REL32	.rodata.__FUNCTION__.10551
  70:	00000020 	.word	0x00000020
			70: R_ARM_REL32	.LC2

Disassembly of section .text.HI_MPI_GDC_EndJob:

00000000 <HI_MPI_GDC_EndJob>:
   0:	e59f2080 	ldr	r2, [pc, #128]	; 88 <HI_MPI_GDC_EndJob+0x88>
   4:	e59f3080 	ldr	r3, [pc, #128]	; 8c <HI_MPI_GDC_EndJob+0x8c>
   8:	e08f2002 	add	r2, pc, r2
   c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
  10:	e24dd00c 	sub	sp, sp, #12
  14:	e7923003 	ldr	r3, [r2, r3]
  18:	e5933000 	ldr	r3, [r3]
  1c:	e58d3004 	str	r3, [sp, #4]
  20:	e3a03000 	mov	r3, #0
  24:	e58d0000 	str	r0, [sp]
  28:	ebfffffe 	bl	0 <HI_MPI_GDC_EndJob>
			28: R_ARM_CALL	mpi_gdc_check_open
  2c:	e3500000 	cmp	r0, #0
  30:	1a000010 	bne	78 <HI_MPI_GDC_EndJob+0x78>
  34:	e59f3054 	ldr	r3, [pc, #84]	; 90 <HI_MPI_GDC_EndJob+0x90>
  38:	e3041e02 	movw	r1, #19970	; 0x4e02
  3c:	e3441004 	movt	r1, #16388	; 0x4004
  40:	e1a0200d 	mov	r2, sp
  44:	e08f3003 	add	r3, pc, r3
  48:	e5930000 	ldr	r0, [r3]
  4c:	ebfffffe 	bl	0 <ioctl>
			4c: R_ARM_CALL	ioctl
  50:	e59f203c 	ldr	r2, [pc, #60]	; 94 <HI_MPI_GDC_EndJob+0x94>
  54:	e59f3030 	ldr	r3, [pc, #48]	; 8c <HI_MPI_GDC_EndJob+0x8c>
  58:	e08f2002 	add	r2, pc, r2
  5c:	e7923003 	ldr	r3, [r2, r3]
  60:	e5932000 	ldr	r2, [r3]
  64:	e59d3004 	ldr	r3, [sp, #4]
  68:	e0332002 	eors	r2, r3, r2
  6c:	1a000004 	bne	84 <HI_MPI_GDC_EndJob+0x84>
  70:	e28dd00c 	add	sp, sp, #12
  74:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)
  78:	e3080010 	movw	r0, #32784	; 0x8010
  7c:	e34a0025 	movt	r0, #40997	; 0xa025
  80:	eafffff2 	b	50 <HI_MPI_GDC_EndJob+0x50>
  84:	ebfffffe 	bl	0 <__stack_chk_fail>
			84: R_ARM_CALL	__stack_chk_fail
  88:	00000078 	.word	0x00000078
			88: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  8c:	00000000 	.word	0x00000000
			8c: R_ARM_GOT32	__stack_chk_guard
  90:	00000044 	.word	0x00000044
			90: R_ARM_REL32	.data.g_gdc_mpi_fd
  94:	00000034 	.word	0x00000034
			94: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_

Disassembly of section .text.HI_MPI_GDC_CancelJob:

00000000 <HI_MPI_GDC_CancelJob>:
   0:	e59f2080 	ldr	r2, [pc, #128]	; 88 <HI_MPI_GDC_CancelJob+0x88>
   4:	e59f3080 	ldr	r3, [pc, #128]	; 8c <HI_MPI_GDC_CancelJob+0x8c>
   8:	e08f2002 	add	r2, pc, r2
   c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
  10:	e24dd00c 	sub	sp, sp, #12
  14:	e7923003 	ldr	r3, [r2, r3]
  18:	e5933000 	ldr	r3, [r3]
  1c:	e58d3004 	str	r3, [sp, #4]
  20:	e3a03000 	mov	r3, #0
  24:	e58d0000 	str	r0, [sp]
  28:	ebfffffe 	bl	0 <HI_MPI_GDC_CancelJob>
			28: R_ARM_CALL	mpi_gdc_check_open
  2c:	e3500000 	cmp	r0, #0
  30:	1a000010 	bne	78 <HI_MPI_GDC_CancelJob+0x78>
  34:	e59f3054 	ldr	r3, [pc, #84]	; 90 <HI_MPI_GDC_CancelJob+0x90>
  38:	e3041e01 	movw	r1, #19969	; 0x4e01
  3c:	e3441004 	movt	r1, #16388	; 0x4004
  40:	e1a0200d 	mov	r2, sp
  44:	e08f3003 	add	r3, pc, r3
  48:	e5930000 	ldr	r0, [r3]
  4c:	ebfffffe 	bl	0 <ioctl>
			4c: R_ARM_CALL	ioctl
  50:	e59f203c 	ldr	r2, [pc, #60]	; 94 <HI_MPI_GDC_CancelJob+0x94>
  54:	e59f3030 	ldr	r3, [pc, #48]	; 8c <HI_MPI_GDC_CancelJob+0x8c>
  58:	e08f2002 	add	r2, pc, r2
  5c:	e7923003 	ldr	r3, [r2, r3]
  60:	e5932000 	ldr	r2, [r3]
  64:	e59d3004 	ldr	r3, [sp, #4]
  68:	e0332002 	eors	r2, r3, r2
  6c:	1a000004 	bne	84 <HI_MPI_GDC_CancelJob+0x84>
  70:	e28dd00c 	add	sp, sp, #12
  74:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)
  78:	e3080010 	movw	r0, #32784	; 0x8010
  7c:	e34a0025 	movt	r0, #40997	; 0xa025
  80:	eafffff2 	b	50 <HI_MPI_GDC_CancelJob+0x50>
  84:	ebfffffe 	bl	0 <__stack_chk_fail>
			84: R_ARM_CALL	__stack_chk_fail
  88:	00000078 	.word	0x00000078
			88: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  8c:	00000000 	.word	0x00000000
			8c: R_ARM_GOT32	__stack_chk_guard
  90:	00000044 	.word	0x00000044
			90: R_ARM_REL32	.data.g_gdc_mpi_fd
  94:	00000034 	.word	0x00000034
			94: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_

Disassembly of section .text.HI_MPI_GDC_SetConfig:

00000000 <HI_MPI_GDC_SetConfig>:
   0:	e3080008 	movw	r0, #32776	; 0x8008
   4:	e34a0025 	movt	r0, #40997	; 0xa025
   8:	e12fff1e 	bx	lr

Disassembly of section .text.HI_MPI_GDC_AddCorrectionTask:

00000000 <HI_MPI_GDC_AddCorrectionTask>:
   0:	e3080008 	movw	r0, #32776	; 0x8008
   4:	e34a0025 	movt	r0, #40997	; 0xa025
   8:	e12fff1e 	bx	lr

Disassembly of section .text.HI_MPI_GDC_AddCorrectionExTask:

00000000 <HI_MPI_GDC_AddCorrectionExTask>:
   0:	e3080008 	movw	r0, #32776	; 0x8008
   4:	e34a0025 	movt	r0, #40997	; 0xa025
   8:	e12fff1e 	bx	lr

Disassembly of section .text.HI_MPI_GDC_AddPMFTask:

00000000 <HI_MPI_GDC_AddPMFTask>:
   0:	eafffffe 	b	0 <HI_MPI_GDC_AddPMFTask>
			0: R_ARM_JUMP24	hi_mpi_gdc_add_pmf_task

Disassembly of section .text.HI_MPI_GDC_FisheyePosQueryDst2Src:

00000000 <HI_MPI_GDC_FisheyePosQueryDst2Src>:
   0:	eafffffe 	b	0 <HI_MPI_GDC_FisheyePosQueryDst2Src>
			0: R_ARM_JUMP24	hi_mpi_gdc_fisheye_pos_query_dst_to_src
