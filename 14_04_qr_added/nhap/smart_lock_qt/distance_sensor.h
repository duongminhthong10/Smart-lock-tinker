#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H
#include "config.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

void distance_sensor_init();
uint64_t doXungECHO(uint8_t pin);
uint64_t doXungECHOTimeout(uint8_t pin, uint64_t time_out);
int64_t doKhoangCach();
void *distanceHandle(void *arg);
void distanceThread();

#ifdef __cplusplus
}
#endif

#endif // DISTANCE_SENSOR_H
