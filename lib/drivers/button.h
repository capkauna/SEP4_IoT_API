/*
 * button.h
 *
 * Created: 02-03-2026
 *  Author: ERL
 */ 
#pragma once

#include <stdint.h>
#include <stdbool.h>

void button_init();
bool button_get(uint8_t button_no);
uint8_t button_scan();

