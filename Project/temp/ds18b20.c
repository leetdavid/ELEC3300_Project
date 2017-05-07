#include "ds18b20.h"

u8 precision = 3;
one_wire_device *one_wire_list_of_devices;
u8 one_wire_devices_list_size = 2;

void ds18b20_init(GPIO_TypeDef *gpio, u16 port, TIM_TypeDef *timer) {
	one_wire_init(gpio, port, timer);
	one_wire_list_of_devices = one_wire_search_rom(&one_wire_devices_list_size);
}

u8 ds18b20_get_precision(void) {
	return precision;
}

ds18b20_devices ds18b20_get_devices(bool scan) {
    if (scan) {
        one_wire_devices_list_size = 0;
        one_wire_list_of_devices = one_wire_search_rom(&one_wire_devices_list_size);
    }
    ds18b20_devices ret;
    ret.size = one_wire_devices_list_size;
    ret.devices = one_wire_list_of_devices;
    return ret;
}

void ds18b20_set_precision(u8 p) {
	precision = p;
	one_wire_reset_pulse();

	one_wire_write_byte(0xCC); // Skip ROM
	one_wire_write_byte(0x4E); // Write scratchpad

	one_wire_write_byte(0x4B);
	one_wire_write_byte(0x46);
	// set precision
	one_wire_write_byte(0x1F | (precision << 5));
}

void ds18b20_convert_temperature_simple(void) {
	one_wire_reset_pulse();
	one_wire_write_byte(0xCC); // Skip ROM
	one_wire_write_byte(0x44); // Convert temperature
}

void ds18b20_convert_temperature(one_wire_device device) {
	one_wire_reset_pulse();
	one_wire_match_rom(device); // Match ROM
	one_wire_write_byte(0x44); // Convert temperature
}

void ds18b20_convert_temperature_all(void) {
	u8 i = 0;
	for (i = 0; i < one_wire_devices_list_size; ++i) {
		ds18b20_convert_temperature(one_wire_list_of_devices[i]);
	}
}

simple_float ds18b20_read_temperature_simple(void) {
	one_wire_reset_pulse();
	one_wire_write_byte(0xCC); // Skip ROM
	one_wire_write_byte(0xBE); // Read scratchpad

	return ds18b20_decode_temperature();
}

simple_float ds18b20_read_temperature(one_wire_device device) {
	one_wire_reset_pulse();
	one_wire_match_rom(device); // Match ROM
	one_wire_write_byte(0xBE); // Read scratchpad

	return ds18b20_decode_temperature();
}

simple_float* ds18b20_read_temperature_all(void) {
	u8 i = 0;
	simple_float *temperatures;
	temperatures = malloc(one_wire_devices_list_size * sizeof(simple_float));

	for (i = 0; i < one_wire_devices_list_size; ++i) {
		temperatures[i] = ds18b20_read_temperature(one_wire_list_of_devices[i]);
	}
	return temperatures;
}

simple_float ds18b20_decode_temperature(void) {
	int i;
	u8 crc;
	u8 data[9];
    simple_float f;
    f.is_valid = false;
	one_wire_reset_crc();

	for (i = 0; i < 9; ++i) {
		data[i] = one_wire_read_byte();
		crc = one_wire_crc(data[i]);
	}
	if (crc != 0) {
		return f;
	}

	u8 temp_msb = data[1]; // Sign byte + lsbit
	u8 temp_lsb = data[0]; // Temp data plus lsb
    float temp = (temp_msb << 8 | temp_lsb) / powf(2, 4);
    int rest = (temp - (int)temp) * 1000.0;

    f.integer = (u8)temp;
    f.fractional = rest;
    f.is_valid = true;

    char buffer[10];
    sprintf(buffer, "%d.%d\r", (int)temp, rest);
    //usart2_print(buffer);
    return f;
}

void ds18b20_wait_for_conversion(void) {
	if (precision == 0) {
		delay(95);
	} else if (precision == 1) {
		delay(190);
	} else if (precision == 2) {
		delay(380);
	} else if (precision == 3) {
		delay(750);
	}
}