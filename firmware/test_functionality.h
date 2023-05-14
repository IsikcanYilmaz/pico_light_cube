#include "pico/stdlib.h"
#include "hardware/timer.h"

void flasher();
void echo();
int64_t alarmCallback(alarm_id_t id, void *user_data);
bool repeatingAlarmCallback(struct repeating_timer *t);
bool testSecondaryRepeatingAlarmCallback(struct repeating_timer *t);
void setAlarm();
void setRepeatingAlarm();
void toggleLed();
void setLed(bool on);
uint64_t get_time(void);
void softwareReset(bool boot);
