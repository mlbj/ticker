#ifndef PULSE_H
#define PULSE_H

#include <stdbool.h>

void pulse_timer_init(void);
bool pulse_set_frequency(double freq_hz);
bool pulse_set_width(double width_s);
void pulse_set_output(bool enable);

#endif
