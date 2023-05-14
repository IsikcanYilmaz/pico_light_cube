#include "test_functionality.h"
// #include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
// #include "hardware/timer.h"
#include "hardware/pwm.h"
#include "hardware/watchdog.h"
#include "device/usbd.h"
#include "pico/bootrom.h"

void flasher() 
{
	uint16_t delay = 10;
	uint16_t lowDelay = 10;
	uint16_t highDelay = 1000;
	uint16_t delayIncrement = 10;
	bool up = true;
	while(up)
	{
		printf("Hello, world!\n");
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
		sleep_ms(delay);
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
		sleep_ms(delay);
		delay += (up ? delayIncrement : -delayIncrement);
		if (delay >= highDelay || delay <= lowDelay)
		{
			up = !up;
		}
	}
}

void echo()
{
	char buf[128];
	uint16_t idx = 0;
	bool ledOn = true;
	while(1)
	{
		buf[idx] = getchar();
		printf("%c", buf[idx]);
		if (buf[idx] == '\n')
		{
			printf("n %s\n", buf);
			memset(&buf, 0x00, sizeof(buf));
			idx = 0;
		}
		else if (buf[idx] == '\r')
		{
			printf("r %s\n", buf);
			memset(&buf, 0x00, sizeof(buf));
			idx = 0;
		}
		else if (buf[idx] == '\b')
		{
			printf("b %s\n", buf);
			memset(&buf, 0x00, sizeof(buf));
			idx = 0;
		}
		else
		{
			idx++;
		}
		ledOn = !ledOn;
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, (ledOn ? 1 : 0));
	}
}

// TIMER STUFF

uint64_t get_time(void) {
	// Reading low latches the high value
	uint32_t lo = timer_hw->timelr;
	uint32_t hi = timer_hw->timehr;
	return ((uint64_t) hi << 32u) | lo;
}

uint64_t get_time_from_timer(alarm_id_t id)
{
	uint32_t lo = timer_hw->timelr;
	uint32_t hi = timer_hw->timehr;
	return ((uint64_t) hi << 32u) | lo;
}

int64_t alarmCallback(alarm_id_t id, void *user_data)
{
	printf("alarm callback %d\n", id);
}

bool repeatingAlarmCallback(struct repeating_timer *t)
{
	printf("Repeating alarm callback %d\n", time_us_64());
	return true;
}

bool testSecondaryRepeatingAlarmCallback(struct repeating_timer *t)
{
	printf("Second alarm callback %d\n", time_us_64());
	return true;
}

void setAlarm()
{
	add_alarm_in_ms(2000, alarmCallback, NULL, false);
}

void setRepeatingAlarm()
{
	struct repeating_timer timer;
	add_repeating_timer_ms(1000, repeatingAlarmCallback, NULL, &timer);
}

void toggleLed()
{
	static bool l = false;
	l = !l;
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, l);
}

void setLed(bool on)
{
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, (on ? 1 : 0));
}

void softwareReset(bool boot)
{
	printf("Resetting ");
	if (boot)
	{
		printf("into boot mode\n");
		reset_usb_boot(0,0);
	}
	else
	{
		tud_disconnect();
		watchdog_enable(1,0);
		while(1);
	}
}
