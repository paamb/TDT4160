.thumb
.syntax unified

.include "gpio_constants.s"     // Register-adresser og konstanter for GPIO

.text
	.global Start
	
Start:
	LDR R0, = PORT_SIZE
	LDR R1, = LED_PORT
	MUL R0, R1, R0

	LDR R1, = GPIO_BASE
	ADD R0, R1, R0

	// R1 = GPIO_BASE + (PORT_SIZE * LED_PORT) + GPIO_PORT_DOUTCLR
	LDR R1, = GPIO_PORT_DOUTCLR
	ADD R1, R1, R0

	// R0 = GPIO_BASE + (PORT_SIZE * LED_PORT) + GPIO_PORT_DOUTSET
	LDR R2, = GPIO_PORT_DOUTSET
	ADD R0, R0, R2

	// R2 = GPIO_BASE + (PORT_SIZE * BUTTON_PORT) + GPIO_PORT_DIN
	LDR R2, = PORT_SIZE
	LDR R3, = BUTTON_PORT
	MUL R2, R2, R3
	LDR R3, = GPIO_BASE
	ADD R2, R2, R3
	LDR R3, = GPIO_PORT_DIN
	ADD R2, R2, R3

	MOV R3, #1
	LSL R3, R3, #LED_PIN

	MOV R4, #1
	LSL R4, R4, #BUTTON_PIN

Loop:
	LDR R5, [R2]
	CMP R5, R4
	BNE LightOn

LightOff:
	STR R3, [R1]
	B Loop

LightOn:
	STR R3, [R0]
	B Loop

NOP // Behold denne på bunnen av fila

