#pragma once
#include <Arduino.h>

// ---- ขา ----
constexpr uint8_t PIN_SDA = 21;
constexpr uint8_t PIN_SCL = 22;
constexpr uint8_t PIN_BUTTON = 32; // อีกขาของปุ่มต่อ GND

// ---- เวลา ----
#ifdef DEBUG_FAST
constexpr uint32_t TIME_DIVISOR = 60;
#else
constexpr uint32_t TIME_DIVISOR = 1;
#endif

constexpr uint32_t FOCUS_MS = 25UL * 60 * 1000 / TIME_DIVISOR;
constexpr uint32_t BREAK_MS = 5UL * 60 * 1000 / TIME_DIVISOR;
constexpr uint32_t IDLE_TO_SLEEP_MS = 5UL * 60 * 1000 / TIME_DIVISOR;
constexpr uint8_t SMALL_ROUNDS_PER_BIG = 4;

// ---- เกม ----
constexpr uint8_t ANIMAL_COUNT = 15;           // ตัวปกติ index 0-14
constexpr uint8_t SECRET_INDEX = ANIMAL_COUNT; // ตัวซีเคร็ต index 15
constexpr uint8_t ROUNDS_PER_UNLOCK = 3;
constexpr uint8_t SECRET_ROUNDS = 100;

// ---- อนิเมชันไข่ฟัก ----
constexpr uint32_t HATCH_SHAKE_MS = 3000;
constexpr uint32_t HATCH_CRACK_MS = 1000;
