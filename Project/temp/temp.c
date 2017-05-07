/* Includes ------------------------------------------------------------------*/
#include "temp.h"
#include "ds18b20.h"

void Temp_Init(void) {
	ds18b20_init(GPIOC, GPIO_Pin_6, TIM2);


	//This part is to read the temp value
	//ds18b20_read_temperature_all();
	simple_float temp = Temp_Read();
	//ds18b20_wait_for_conversion();
	Temp_WaitForConversion();
	printf("%d---\r\n", Temp_GetPrecision());
	Temp_Convert();

}

simple_float* Temp_Read(void){
	return ds18b20_read_temperature(one_wire_list_of_devices[0]);
}

void Temp_WaitForConversion(void){
	ds18b20_wait_for_conversion();
}

u8 Temp_GetPrecision(void){
	return precision;
}

void Temp_Convert(void){
	ds18b20_convert_temperature_all();
}