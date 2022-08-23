#include "o3.h"
#include "gpio.h"
#include "systick.h"


typedef struct {
	volatile word CTRL;
	volatile word LOAD;
	volatile word VAL;
	volatile word CALIB;
} systick_map;

typedef struct {
	volatile word CTRL;
	volatile word MODEL;
	volatile word MODEH;
	volatile word DOUT;
	volatile word DOUTSET;
	volatile word DOUTCLR;
	volatile word DOUTTGL;
	volatile word DIN;
	volatile word PINLOCKN;
} gpio_port_map_t;

typedef struct {
	volatile gpio_port_map_t ports[6];
	volatile word unused_space[10];
	volatile word EXTIPSELL;
	volatile word EXTIPSELH;
	volatile word EXTIRISE;
	volatile word EXTIFALL;
	volatile word IEN;
	volatile word IF;
	volatile word IFS;
	volatile word IFC;
	volatile word ROUTE;
	volatile word INSENSE;
	volatile word LOCK;
	volatile word CTRL;
	volatile word CMD;
	volatile word EM4WUEN;
	volatile word EM4WUPOL;
	volatile word EM4WUCAUSE;
} gpio_map_t;

int globalseconds = 0;
int globalstage = 0;

systick_map* systickbase = (systick_map*) SYSTICK_BASE;
gpio_map_t* gpiobase = (gpio_map_t*) GPIO_BASE;



void wordshift(volatile word* address, int value, int pin){
	*address = *address & ~(0b1111 << pin);
	*address = *address | ((value) << pin);
}

// word* peker på en minneadresse. adress er altså en peker av typen word.
void bitshift(volatile word* address, int value, int pin){
	value = value << pin;
	//*address finner verdien på address
	value = value | *address;
	*address = value;
}

/**************************************************************************//**
 * @brief Konverterer nummer til string 
 * Konverterer et nummer mellom 0 og 99 til string
 *****************************************************************************/
void int_to_string(char *timestamp, unsigned int offset, int i) {
    if (i > 99) {
        timestamp[offset]   = '9';
        timestamp[offset+1] = '9';
        return;
    }

    while (i > 0) {
	    if (i >= 10) {
		    i -= 10;
		    timestamp[offset]++;
		
	    } else {
		    timestamp[offset+1] = '0' + i;
		    i=0;
	    }
    }
}

/**************************************************************************//**
 * @brief Konverterer 3 tall til en timestamp-string
 * timestamp-argumentet mÃ¥ vÃ¦re et array med plass til (minst) 7 elementer.
 * Det kan deklareres i funksjonen som kaller som "char timestamp[7];"
 * Kallet blir dermed:
 * char timestamp[7];
 * time_to_string(timestamp, h, m, s);
 *****************************************************************************/
void time_to_string(char *timestamp, int h, int m, int s) {
    timestamp[0] = '0';
    timestamp[1] = '0';
    timestamp[2] = '0';
    timestamp[3] = '0';
    timestamp[4] = '0';
    timestamp[5] = '0';
    timestamp[6] = '\0';

    int_to_string(timestamp, 0, h);
    int_to_string(timestamp, 2, m);
    int_to_string(timestamp, 4, s);
}

void display(){
	char timestamp[7];
	int time[3];
	time[2] = globalseconds / (60*60);
	time[1] = (globalseconds - time[2]*(60*60)) / 60;
	time[0] = globalseconds - (time[2]*(60*60) + time[1]*60);
	time_to_string(timestamp, time[2], time[1], time[0]);
	lcd_write(timestamp);
}

void GPIO_ODD_IRQHandler(void){
	if(globalstage == 0){
		globalseconds += 1;
	}
	else if(globalstage ==  1){
		globalseconds += 60;
	}
	else if(globalstage == 2){
		globalseconds += 60*60;
	}
	display();
	bitshift(&gpiobase -> IFC, 1, 9);
}


void GPIO_EVEN_IRQHandler(void){
	if (globalstage < 2){
		globalstage += 1;
	}
	else if(globalstage == 2){
		//Starting the clock
		globalstage += 1;
		systickbase -> CTRL |= 0b001;
	}
	else if(globalstage == 3){
		globalstage = 0;
		globalseconds = 0;
		gpiobase -> ports[GPIO_PORT_E].DOUT = 0b000;
	}
	bitshift(&gpiobase-> IFC, 1, 10);
}

void SysTick_Handler(void){
	globalseconds -= 1;
	if (globalseconds <= 0){
		systickbase -> CTRL = 0b110;
		gpiobase -> ports[GPIO_PORT_E].DOUT = 0b100;
		globalstage = 3;
	}
	display();
}

int main(void) {
    init();


    //Setting up the clock
    systickbase -> LOAD = FREQUENCY;
    systickbase -> CTRL = 0b110;
    systickbase -> VAL = 10000;


    //& Sending in the address for gpiobase ...
    // Setting inputs btn0
    wordshift(&gpiobase -> ports[GPIO_PORT_E].MODEL, GPIO_MODE_OUTPUT, 8);

    wordshift(&gpiobase->ports[GPIO_PORT_B].MODEH,  GPIO_MODE_INPUT, 4);
    // btn1
    wordshift(&gpiobase->ports[GPIO_PORT_B].MODEH,  GPIO_MODE_INPUT, 8);


    //Setting up btn0

    //Setting up extipshelh btn0
    wordshift(&gpiobase -> EXTIPSELH, 0b0001, 4);

    // Setting up extifall btn0
    bitshift(&gpiobase->EXTIFALL, 1, 9);

    //Setting up interuptflag btn0
    bitshift(&gpiobase->IF, 0, 9);

    //Setting up interupt to be enable btn0
    bitshift(&gpiobase->IEN, 1, 9);


    //Setting up btn1

    //Setting up extipshelh btn1
    wordshift(&gpiobase -> EXTIPSELH, 0b0001, 8);

    // Setting up extifall btn1
    bitshift(&gpiobase->EXTIFALL, 1, 10);

    //Setting up interuptflag btn1
    bitshift(&gpiobase->IF, 0, 10);

    //Setting up interupt to be enable btn1
    bitshift(&gpiobase->IEN, 1, 10);


    while(1){

    };
    return 0;
}


