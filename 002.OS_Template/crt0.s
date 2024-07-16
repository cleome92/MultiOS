	.include "4412addr.inc"
	.include "option.inc"

	.code	32
	.text
 
	.global __start
__start:

@--------------------------------------------------
@ Exception Vector Configuration
@--------------------------------------------------

	b		ResetHandler
	b		HandlerUndef		@ HandlerUndef
	b		HandlerSVC			@ handler for SVC
	b		HandlerPabort 		@ HandlerPabort
	b		HandlerDabort		@ HandlerDabort
	b		.					@ reserved
	b		HandlerIRQ			@ HandlerIRQ
	b		.					@ HandlerFIQ

@--------------------------------------------------
@ IRQ Handler
@--------------------------------------------------

	.extern ISR_Vector
	.extern gucAppNum
	.extern getNextAppNum
	.extern gpaunContextAddress
	.extern debugPrint
	.extern debugPrintNum
HandlerIRQ:
    push {r0-r3, r12, lr}

    ldr r1, =ISR_Vector
    ldr r2, =ICCIAR_CPU0
    ldr r0, [r2]
    ldr r2, =0x3ff
    and r0, r0, r2
    ldr r0, [r1, r0, lsl #2]
    blx r0

	pop {r0-r3, r12, lr}

@ Context Save Start
	push {r0-r3}
    @ Save the current application number
    ldr r0, =gucAppNum
    ldr r0, [r0]

    @ Get the current context storage address
    ldr r1, =gpaunContextAddress
    ldr r2, [r1, r0, lsl #2]

    @ Save context
    str	r4, [r2, #16]
    str	r5, [r2, #20]
    str	r6, [r2, #24]
	str	r7, [r2, #28]
    str	r8, [r2, #32]
    str	r9, [r2, #36]
    str	r10, [r2, #40]
    str	r11, [r2, #44]
    str	r12, [r2, #48]

	mrs		r1, cpsr
	cps		#0x1f
	str sp, [r2, #52] @ Save sp
    str lr, [r2, #56] @ Save lr
	msr		cpsr_cxsf, r1

    mrs r3, spsr
    str r3, [r2, #64]

    pop {r0-r3}

	push {r4-r7}
    @ Save the current application number
    ldr r4, =gucAppNum
    ldr r4, [r4]

    @ Get the current context storage address
    ldr r5, =gpaunContextAddress
    ldr r6, [r5, r4, lsl #2]

	str	r0, [r6, #0]
    str	r1, [r6, #4]
    str	r2, [r6, #8]
	str	r3, [r6, #12]

	pop {r4-r7}
@ Context Save End

	push {r0-r3}
	mov r0, #0
	bl	debugPrint
	pop	{r0-r3}

@ Context Load Start
	push {r0-r3}

    @ Store the new application number
    ldr r1, =gucAppNum
    ldr r0, [r1]

	@ Get the new context load address
    ldr r1, =gpaunContextAddress
    ldr r2, [r1, r0, lsl #2]



    @ Load context
    ldr	r4, [r2, #16]
    ldr	r5, [r2, #20]
    ldr	r6, [r2, #24]
	ldr	r7, [r2, #28]
    ldr	r8, [r2, #32]
    ldr	r9, [r2, #36]
    ldr	r10, [r2, #40]
    ldr	r11, [r2, #44]
    ldr	r12, [r2, #48]
    ldr	r13, [r2, #52]
    ldr	r14, [r2, #56]

	mrs		r1, cpsr
	cps		#0x1f
    ldr sp, [r2, #52]
    ldr lr, [r2, #56]
	msr		cpsr_cxsf, r1

    ldr r3, [r2, #64]
    msr spsr, r3

    pop {r0-r3}

	push {r4-r7}
    @ Save the current application number
    ldr r4, =gucAppNum
    ldr r4, [r4]

    @ Get the current context storage address
    ldr r5, =gpaunContextAddress
    ldr r6, [r5, r4, lsl #2]

	ldr	r0, [r6, #0]
    ldr	r1, [r6, #4]
    ldr	r2, [r6, #8]
	ldr	r3, [r6, #12]

	pop {r4-r7}
@ Context Load End


	push {r0-r3}
	mov r0, #1
	bl	debugPrint
	pop	{r0-r3}

	push {r0-r3,r4,r5}

    mrs		r1, cpsr
	cps		#0x1f
    mov r4, sp
    mov r5, lr
	msr		cpsr_cxsf, r1

	mov	r0, r4
	bl debugPrintNum
	mov	r0, r5
	bl debugPrintNum

	pop {r0-r3,r4,r5}

    subs pc, lr, #4

@--------------------------------------------------
@ Exception Handler
@--------------------------------------------------

	.extern		Undef_Handler
	.extern		Dabort_Handler
	.extern		Pabort_Handler
	.extern		SVC_Handler

HandlerUndef:
	sub 	r0, lr, #4
	mrs		r1, spsr
	and		r1, r1, #0x1f
	bl		Undef_Handler


HandlerDabort:
	sub 	r0, lr, #8
	mrs		r1, spsr
	and		r1, r1, #0x1f
	bl		Dabort_Handler

HandlerPabort:
	sub 	r0, lr, #4
	mrs		r1, spsr
	and		r1, r1, #0x1f
	bl		Pabort_Handler

HandlerSVC:
	stmfd	sp!,{r0-r3, r12, lr}
	sub 	r0, lr, #4
	mrs		r1, spsr
	and		r1, r1, #0x1f
	bl		SVC_Handler
	ldmfd	sp!,{r0-r3, r12, pc}^

@--------------------------------------------------
@ Reset Handler Routine
@--------------------------------------------------

ResetHandler:

@--------------------------------------------------
@ Watchdog Disable
@--------------------------------------------------

	ldr		r0, =WTCON
	ldr		r1, =0x0
	str		r1, [r0]

@--------------------------------------------------
@ Coprocessor Config
@--------------------------------------------------

	@ NEON & VFP Enable

	LDR     r0,=0x05555555
	MCR     p15,0,r0,c1,c0,2
	LDR     R0,=(1<<30)
	VMSR    FPEXC,r0

	@ L1-I, L1-D Cache, Branch Predictor OFF
	mov		r1, #0
	MCR     p15,0,r1,c1,c0,0

	@ L2 Cache OFF
	mov		r1, #0
	MCR 	p15,0,R1,C1,C0,1

	@ I-Cache(12), Flow Prediction(11)

	LDR     r1,=(1<<12)|(1<<11) 	@ [4] I-Cache ON, Flow Prediction ON
	MCR     p15,0,r1,c1,c0,0

@--------------------------------------------------
@ CLOCK Configuration
@--------------------------------------------------

@--------------------------------------------------
@ DRAM Configuration
@--------------------------------------------------

@--------------------------------------------------
@ GPIO(LED OUTPUT) ON
@--------------------------------------------------
	
	@ LED Port Control

	ldr		r0, =GPM4CON
	ldr		r1, [r0]
	bic		r1, r1, #0xff<<16
	orr		r1, r1, #0x11<<16
	str		r1, [r0]

	@ Turn LED on (XO)

	ldr		r0, =GPM4DAT
	ldr		r1, [r0]
	bic		r1, r1, #0x3<<4
	orr		r1, r1, #0x1<<4
	str		r1, [r0]

@--------------------------------------------------
@ C Runtime Startup
@--------------------------------------------------

	@ RAM loading

	.extern __ZI_BASE__
	.extern __ZI_LIMIT__

	ldr		r0, =__ZI_BASE__
	ldr		r1, =__ZI_LIMIT__
	mov		r2, #0x0
1:
	cmp		r0, r1
	strlo	r2, [r0], #4
	blo		1b

	@ Stack mounting

	msr		cpsr_c, #(IRQ_BIT|FIQ_BIT|IRQ_MODE)
	ldr		sp, =IRQ_STACK_BASE

	msr		cpsr_c, #(IRQ_BIT|FIQ_BIT|FIQ_MODE)
	ldr		sp, =FIQ_STACK_BASE

	msr		cpsr_c, #(IRQ_BIT|FIQ_BIT|UNDEF_MODE)
	ldr		sp, =UNDEF_STACK_BASE

	msr		cpsr_c, #(IRQ_BIT|FIQ_BIT|ABORT_MODE)
	ldr		sp, =ABORT_STACK_BASE

	msr		cpsr_c, #(SVC_MODE)
	ldr		sp, =SVC_STACK_BASE

	@ Set Exception Vector Address

	ldr		r0, =DRAM_START
	mcr     p15,0,r0,c12,c0,0

	@ Call Main

	bl		Main

	@ HALT

	b		.

	.end
