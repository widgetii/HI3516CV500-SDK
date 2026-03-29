
mpi_audio.o:     file format elf32-littlearm

SYMBOL TABLE:
00000000 l    df *ABS*	00000000 mpi_audio.c
00000000 l    d  .text	00000000 .text
00000000 l    d  .data	00000000 .data
00000000 l    d  .bss	00000000 .bss
00000000 l    d  .text.mpi_aio_init	00000000 .text.mpi_aio_init
00000000 l    d  .text.mpi_aio_exit	00000000 .text.mpi_aio_exit
00000000 l    d  .text.HI_MPI_AUDIO_SetModParam	00000000 .text.HI_MPI_AUDIO_SetModParam
00000000 l       .rodata.HI_MPI_AUDIO_SetModParam.str1.4	00000000 .LC0
0000000c l       .rodata.HI_MPI_AUDIO_SetModParam.str1.4	00000000 .LC1
00000000 l    d  .text.HI_MPI_AUDIO_GetModParam	00000000 .text.HI_MPI_AUDIO_GetModParam
00000000 l    d  .text.HI_MPI_AUDIO_RegisterVQEModule	00000000 .text.HI_MPI_AUDIO_RegisterVQEModule
00000000 l       .rodata.HI_MPI_AUDIO_RegisterVQEModule.str1.4	00000000 .LC2
00000000 l    d  .bss.g_aiao_fdmutex	00000000 .bss.g_aiao_fdmutex
00000000 l     O .bss.g_aiao_fdmutex	00000018 g_aiao_fdmutex
00000000 l    d  .bss.g_aio_init	00000000 .bss.g_aio_init
00000000 l     O .bss.g_aio_init	00000004 g_aio_init
00000000 l    d  .data.g_aiofd	00000000 .data.g_aiofd
00000000 l     O .data.g_aiofd	00000004 g_aiofd
00000000 l    d  .rodata.HI_MPI_AUDIO_RegisterVQEModule.str1.4	00000000 .rodata.HI_MPI_AUDIO_RegisterVQEModule.str1.4
00000000 l    d  .rodata.HI_MPI_AUDIO_SetModParam.str1.4	00000000 .rodata.HI_MPI_AUDIO_SetModParam.str1.4
00000000 l    d  .rodata.__FUNCTION__.9562	00000000 .rodata.__FUNCTION__.9562
00000000 l     O .rodata.__FUNCTION__.9562	00000021 __FUNCTION__.9562
00000000 l    d  .note.GNU-stack	00000000 .note.GNU-stack
00000000 l    d  .comment	00000000 .comment
00000000 l    d  .ARM.attributes	00000000 .ARM.attributes
00000000 g     F .text.mpi_aio_init	00000024 mpi_aio_init
00000000 g     F .text.mpi_aio_exit	00000024 mpi_aio_exit
00000000 g     F .text.HI_MPI_AUDIO_SetModParam	000000a8 HI_MPI_AUDIO_SetModParam
00000000         *UND*	00000000 pthread_mutex_lock
00000000         *UND*	00000000 pthread_mutex_unlock
00000000         *UND*	00000000 ioctl
00000000         *UND*	00000000 open
00000000         *UND*	00000000 puts
00000000 g     F .text.HI_MPI_AUDIO_GetModParam	000000a8 HI_MPI_AUDIO_GetModParam
00000000 g     F .text.HI_MPI_AUDIO_RegisterVQEModule	0000004c HI_MPI_AUDIO_RegisterVQEModule
00000000         *UND*	00000000 fprintf
00000000         *UND*	00000000 _GLOBAL_OFFSET_TABLE_
00000000         *UND*	00000000 stderr



Disassembly of section .text.mpi_aio_init:

00000000 <mpi_aio_init>:
   0:	e59f3018 	ldr	r3, [pc, #24]	; 20 <mpi_aio_init+0x20>
   4:	e3a00000 	mov	r0, #0
   8:	e08f3003 	add	r3, pc, r3
   c:	e5932000 	ldr	r2, [r3]
  10:	e3520001 	cmp	r2, #1
  14:	13a02001 	movne	r2, #1
  18:	15832000 	strne	r2, [r3]
  1c:	e12fff1e 	bx	lr
  20:	00000010 	.word	0x00000010
			20: R_ARM_REL32	.bss.g_aio_init

Disassembly of section .text.mpi_aio_exit:

00000000 <mpi_aio_exit>:
   0:	e59f3018 	ldr	r3, [pc, #24]	; 20 <mpi_aio_exit+0x20>
   4:	e3a00000 	mov	r0, #0
   8:	e08f3003 	add	r3, pc, r3
   c:	e5932000 	ldr	r2, [r3]
  10:	e3520000 	cmp	r2, #0
  14:	13a02000 	movne	r2, #0
  18:	15832000 	strne	r2, [r3]
  1c:	e12fff1e 	bx	lr
  20:	00000010 	.word	0x00000010
			20: R_ARM_REL32	.bss.g_aio_init

Disassembly of section .text.HI_MPI_AUDIO_SetModParam:

00000000 <HI_MPI_AUDIO_SetModParam>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e1a06000 	mov	r6, r0
   8:	e59f5080 	ldr	r5, [pc, #128]	; 90 <HI_MPI_AUDIO_SetModParam+0x90>
   c:	e59f4080 	ldr	r4, [pc, #128]	; 94 <HI_MPI_AUDIO_SetModParam+0x94>
  10:	e08f5005 	add	r5, pc, r5
  14:	e08f4004 	add	r4, pc, r4
  18:	e1a00005 	mov	r0, r5
  1c:	ebfffffe 	bl	0 <pthread_mutex_lock>
			1c: R_ARM_CALL	pthread_mutex_lock
  20:	e5943000 	ldr	r3, [r4]
  24:	e3530000 	cmp	r3, #0
  28:	ba00000a 	blt	58 <HI_MPI_AUDIO_SetModParam+0x58>
  2c:	e59f0064 	ldr	r0, [pc, #100]	; 98 <HI_MPI_AUDIO_SetModParam+0x98>
  30:	e08f0000 	add	r0, pc, r0
  34:	ebfffffe 	bl	0 <pthread_mutex_unlock>
			34: R_ARM_CALL	pthread_mutex_unlock
  38:	e59f305c 	ldr	r3, [pc, #92]	; 9c <HI_MPI_AUDIO_SetModParam+0x9c>
  3c:	e3a01c53 	mov	r1, #21248	; 0x5300
  40:	e3441004 	movt	r1, #16388	; 0x4004
  44:	e1a02006 	mov	r2, r6
  48:	e08f3003 	add	r3, pc, r3
  4c:	e8bd4070 	pop	{r4, r5, r6, lr}
  50:	e5930000 	ldr	r0, [r3]
  54:	eafffffe 	b	0 <ioctl>
			54: R_ARM_JUMP24	ioctl
  58:	e59f0040 	ldr	r0, [pc, #64]	; a0 <HI_MPI_AUDIO_SetModParam+0xa0>
  5c:	e3a01002 	mov	r1, #2
  60:	e08f0000 	add	r0, pc, r0
  64:	ebfffffe 	bl	0 <open>
			64: R_ARM_CALL	open
  68:	e5840000 	str	r0, [r4]
  6c:	e3500000 	cmp	r0, #0
  70:	aaffffed 	bge	2c <HI_MPI_AUDIO_SetModParam+0x2c>
  74:	e1a00005 	mov	r0, r5
  78:	ebfffffe 	bl	0 <pthread_mutex_unlock>
			78: R_ARM_CALL	pthread_mutex_unlock
  7c:	e59f0020 	ldr	r0, [pc, #32]	; a4 <HI_MPI_AUDIO_SetModParam+0xa4>
  80:	e08f0000 	add	r0, pc, r0
  84:	ebfffffe 	bl	0 <puts>
			84: R_ARM_CALL	puts
  88:	e3e00000 	mvn	r0, #0
  8c:	e8bd8070 	pop	{r4, r5, r6, pc}
  90:	00000078 	.word	0x00000078
			90: R_ARM_REL32	.bss.g_aiao_fdmutex
  94:	00000078 	.word	0x00000078
			94: R_ARM_REL32	.data.g_aiofd
  98:	00000060 	.word	0x00000060
			98: R_ARM_REL32	.bss.g_aiao_fdmutex
  9c:	0000004c 	.word	0x0000004c
			9c: R_ARM_REL32	.data.g_aiofd
  a0:	00000038 	.word	0x00000038
			a0: R_ARM_REL32	.LC0
  a4:	0000001c 	.word	0x0000001c
			a4: R_ARM_REL32	.LC1

Disassembly of section .text.HI_MPI_AUDIO_GetModParam:

00000000 <HI_MPI_AUDIO_GetModParam>:
   0:	e92d4070 	push	{r4, r5, r6, lr}
   4:	e1a06000 	mov	r6, r0
   8:	e59f5080 	ldr	r5, [pc, #128]	; 90 <HI_MPI_AUDIO_GetModParam+0x90>
   c:	e59f4080 	ldr	r4, [pc, #128]	; 94 <HI_MPI_AUDIO_GetModParam+0x94>
  10:	e08f5005 	add	r5, pc, r5
  14:	e08f4004 	add	r4, pc, r4
  18:	e1a00005 	mov	r0, r5
  1c:	ebfffffe 	bl	0 <pthread_mutex_lock>
			1c: R_ARM_CALL	pthread_mutex_lock
  20:	e5943000 	ldr	r3, [r4]
  24:	e3530000 	cmp	r3, #0
  28:	ba00000a 	blt	58 <HI_MPI_AUDIO_GetModParam+0x58>
  2c:	e59f0064 	ldr	r0, [pc, #100]	; 98 <HI_MPI_AUDIO_GetModParam+0x98>
  30:	e08f0000 	add	r0, pc, r0
  34:	ebfffffe 	bl	0 <pthread_mutex_unlock>
			34: R_ARM_CALL	pthread_mutex_unlock
  38:	e59f305c 	ldr	r3, [pc, #92]	; 9c <HI_MPI_AUDIO_GetModParam+0x9c>
  3c:	e3051301 	movw	r1, #21249	; 0x5301
  40:	e3481004 	movt	r1, #32772	; 0x8004
  44:	e1a02006 	mov	r2, r6
  48:	e08f3003 	add	r3, pc, r3
  4c:	e8bd4070 	pop	{r4, r5, r6, lr}
  50:	e5930000 	ldr	r0, [r3]
  54:	eafffffe 	b	0 <ioctl>
			54: R_ARM_JUMP24	ioctl
  58:	e59f0040 	ldr	r0, [pc, #64]	; a0 <HI_MPI_AUDIO_GetModParam+0xa0>
  5c:	e3a01002 	mov	r1, #2
  60:	e08f0000 	add	r0, pc, r0
  64:	ebfffffe 	bl	0 <open>
			64: R_ARM_CALL	open
  68:	e5840000 	str	r0, [r4]
  6c:	e3500000 	cmp	r0, #0
  70:	aaffffed 	bge	2c <HI_MPI_AUDIO_GetModParam+0x2c>
  74:	e1a00005 	mov	r0, r5
  78:	ebfffffe 	bl	0 <pthread_mutex_unlock>
			78: R_ARM_CALL	pthread_mutex_unlock
  7c:	e59f0020 	ldr	r0, [pc, #32]	; a4 <HI_MPI_AUDIO_GetModParam+0xa4>
  80:	e08f0000 	add	r0, pc, r0
  84:	ebfffffe 	bl	0 <puts>
			84: R_ARM_CALL	puts
  88:	e3e00000 	mvn	r0, #0
  8c:	e8bd8070 	pop	{r4, r5, r6, pc}
  90:	00000078 	.word	0x00000078
			90: R_ARM_REL32	.bss.g_aiao_fdmutex
  94:	00000078 	.word	0x00000078
			94: R_ARM_REL32	.data.g_aiofd
  98:	00000060 	.word	0x00000060
			98: R_ARM_REL32	.bss.g_aiao_fdmutex
  9c:	0000004c 	.word	0x0000004c
			9c: R_ARM_REL32	.data.g_aiofd
  a0:	00000038 	.word	0x00000038
			a0: R_ARM_REL32	.LC0
  a4:	0000001c 	.word	0x0000001c
			a4: R_ARM_REL32	.LC1

Disassembly of section .text.HI_MPI_AUDIO_RegisterVQEModule:

00000000 <HI_MPI_AUDIO_RegisterVQEModule>:
   0:	e59f1034 	ldr	r1, [pc, #52]	; 3c <HI_MPI_AUDIO_RegisterVQEModule+0x3c>
   4:	e3a030c0 	mov	r3, #192	; 0xc0
   8:	e59f0030 	ldr	r0, [pc, #48]	; 40 <HI_MPI_AUDIO_RegisterVQEModule+0x40>
   c:	e08f1001 	add	r1, pc, r1
  10:	e92d4010 	push	{r4, lr}
  14:	e59f2028 	ldr	r2, [pc, #40]	; 44 <HI_MPI_AUDIO_RegisterVQEModule+0x44>
  18:	e7910000 	ldr	r0, [r1, r0]
  1c:	e59f1024 	ldr	r1, [pc, #36]	; 48 <HI_MPI_AUDIO_RegisterVQEModule+0x48>
  20:	e08f2002 	add	r2, pc, r2
  24:	e08f1001 	add	r1, pc, r1
  28:	e5900000 	ldr	r0, [r0]
  2c:	ebfffffe 	bl	0 <fprintf>
			2c: R_ARM_CALL	fprintf
  30:	e3080009 	movw	r0, #32777	; 0x8009
  34:	e34a0014 	movt	r0, #40980	; 0xa014
  38:	e8bd8010 	pop	{r4, pc}
  3c:	00000028 	.word	0x00000028
			3c: R_ARM_GOTPC	_GLOBAL_OFFSET_TABLE_
  40:	00000000 	.word	0x00000000
			40: R_ARM_GOT32	stderr
  44:	0000001c 	.word	0x0000001c
			44: R_ARM_REL32	.rodata.__FUNCTION__.9562
  48:	0000001c 	.word	0x0000001c
			48: R_ARM_REL32	.LC2
