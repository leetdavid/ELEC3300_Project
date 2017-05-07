/* Includes ------------------------------------------------------------------*/
#include "temp.h"
#include "ds18b20.h"

void Temp_Init(void) {
	ds18b20_init(GPIOC, GPIO_Pin_6, TIM2);


	//This part is to read the temp value
	//ds18b20_read_temperature_all();
	Temp_Read();
	//ds18b20_wait_for_conversion();
	Temp_WaitForConversion();
	//printf("%d---\r\n", Temp_GetPrecision());
	Temp_Convert();

}

void Temp_Read(void){
	ds18b20_read_temperature_all();
}

void Temp_WaitForConversion(void){
	ds18b20_wait_for_conversion();
}
/*
u8 Temp_GetPrecision(void){
	return precision;
}*/

void Temp_Convert(void){
	ds18b20_convert_temperature_all();
}