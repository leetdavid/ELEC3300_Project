/* Includes ------------------------------------------------------------------*/
#include "temp.h"
#include "ds18b20.h"

void Temp_Init(void) {
	ds18b20_init(GPIOC, GPIO_Pin_6, TIM2);


	//This part is to read the temp value
	ds18b20_read_temperature_all();
	ds18b20_wait_for_conversion();
	printf("%d---\r\n", ds18b20_get_precission());
	ds18b20_convert_temperature_all();


}