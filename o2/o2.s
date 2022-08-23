.thumb
.syntax unified

.include "gpio_constants.s"     // Register-adresser og konstanter for GPIO
.include "sys-tick_constants.s" // Register-adresser og konstanter for SysTick

.text
	.global Start
Start:
	//Setter opp frekvensen
	LDR R0, = FREQUENCY / 10
	LDR R1, = SYSTICK_BASE + SYSTICK_LOAD
	STR R0, [R1]

	//Setter opp klokken
	LDR R0, = SYSTICK_BASE + SYSTICK_CTRL
	MOV R1, #0b110
	STR R1, [R0]

	//Setter hvor klokken skal starte
	LDR R0, = SYSTICK_BASE + SYSTICK_VAL
	MOV	R1, #0
	STR R1, [R0]

	//Setter opp Interupt
	LDR R0, = GPIO_BASE + GPIO_EXTIPSELH
	MOV R1, ~0b1111 << 4
	LDR R2, [R0]
	AND R1, R1, R2
	ORR R1, (0b0001 << 4)
	STR R1, [R0]
	
	//Skal skje ved falling edge
	LDR R0, = GPIO_BASE + GPIO_EXTIFALL
	MOV R1, #1 << 9
	LDR R2, [R0]
	ORR R1, R1, R2
	STR R1, [R0]

	//Setter interuptflag
	LDR R0, = GPIO_BASE + GPIO_IFS
	MOV R1, #0 << 9
	LDR R2, [R0]
	ORR R1, R1, R2
	STR R1, [R0]

	//Setter interupt til å være enable
	LDR R0, = GPIO_BASE + GPIO_IEN
	MOV R1, #1 << 9
	LDR R2, [R0]
	ORR R1, R1, R2
	STR R1, [R0]

	//Display
	LDR R1, = tenths
	LDR R2, = seconds
	LDR R3, = minutes
	MOV R4, #1
	MOV R5, #1
	MOV R6, #1

	//Toggle light
	LDR R7, = GPIO_BASE + (PORT_SIZE * LED_PORT) + GPIO_PORT_DOUT
	MOV R8, #0
Loop:
	B Loop

.global SysTick_Handler
.thumb_func
SysTick_Handler:

	//Thenths
	CMP R4, #10
	BNE tenthsHandler
	MOV R4, #0

	//Seconds
	CMP R5, #60
	BNE secondsHandler
	MOV R5, #0

	//Minutes
	CMP R6, #60
	BNE minutesHandler
	MOV R6, #0

minutesHandler:
	STR R6, [R3]
	ADD R6, #1

secondsHandler:
	STR R5, [R2]
	ADD R5, #1

toggleLight:
	EOR R8, R8, 0b100
	STR R8, [R7]

tenthsHandler:
	STR R4, [R1]
	ADD R4, #1
	BX LR


.global GPIO_ODD_IRQHandler
.thumb_func
GPIO_ODD_IRQHandler:
	LDR R0, = SYSTICK_BASE + SYSTICK_CTRL
	LDR R2, [R0]
	MOV R1, #1
	EOR R1, R1, R2
	STR R1, [R0]

	//Reset flag
	LDR R0, = GPIO_BASE + GPIO_IFC
	MOV R1, #1 << 9
	LDR R2, [R0]
	ORR R1, R1, R2
	STR R1, [R0]

	BX LR

NOP // Behold denne pÃ¥ bunnen av fila

