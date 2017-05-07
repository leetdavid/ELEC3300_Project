/* Includes ------------------------------------------------------------------*/
#include "temp.h"
#include "OneWire.h"
#include "stm32f10x_gpio.h"

void Temp_Init(void) {
	//initialize OWire *wire first
	//TODO

	//Connect to PB.10
	OWInit(wire, GPIOB, GPIO_Pin_10);
}

void Temp_Read(void){
	u8 i;
  	OWReset_search(wire);
	if(OWSearch(wire, addr) == FALSE){
		OWReset_search(wire);
		//No more addresses!
		return;
	}
	OWReset(wire);
	OWSelect(wire, addr);
	
	OWWrite_bit(wire, 0x44);
	//ds.write(0x44,1);

	present = OWReset(wire);
	OWSelect(wire, addr);
	OWWrite_bit(wire, 0xBE);
	//ds.write(0xBE);         // Read Scratchpad

	for ( i = 0; i < 9; i++) {           // we need 9 bytes
		data[i] = OWRead(wire);
		//data[i] = ds.read();
	}
}

void Temp_Convert(void){
  u16 temp = (data[1] << 8) + data[0];
  signbit = temp & 0x8000;
  if(signbit){
  	temp = (temp ^ 0xFFFF) + 1;
  }
  temp = (6 * temp) + temp/4;
  num_whole = temp/100;
  num_decimal = temp%100;
}