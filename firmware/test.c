#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include "hardware/timer.h"
#include "test_functionality.h"
#include "usr_input.h"
#include "animation_manager.h"
#include "addr_led_driver.h"

int main() {
	stdio_init_all();
	if (cyw43_arch_init()) {
		printf("Wi-Fi init failed");
		return -1;
	}
	UserInput_Init();
	AddrLedDriver_Init();
	AnimationMan_Init();
	toggleLed();

	// AddrLedDriver_Test();
	while(1)
	{
		UserInput_Service();
	}
	return 0;
}
