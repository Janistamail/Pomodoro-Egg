#pragma once
#include <Arduino.h>

// ---- ขา ----
constexpr uint8_t PIN_SDA = 21;
constexpr uint8_t PIN_SCL = 22;
constexpr uint8_t PIN_BUTTON = 32; // อีกขาของปุ่มต่อ GND
constexpr uint8_t PIN_BUZZER = 25; // passive buzzer (ขา + ต่อขานี้, อีกขาต่อ GND)

// ---- เวลา ----
#ifdef DEBUG_FAST
constexpr uint32_t TIME_DIVISOR = 60;
#else
constexpr uint32_t TIME_DIVISOR = 1;
#endif

#ifdef DEBUG_FAST
constexpr uint32_t FOCUS_MS = 10UL * 1000;  // โหมดทดสอบ: โฟกัส 10 วินาที
constexpr uint32_t BREAK_MS = 3UL * 1000;   // โหมดทดสอบ: พัก 3 วินาที
constexpr uint32_t IDLE_TO_SLEEP_MS = 10UL * 1000;  // โหมดทดสอบ: หน้าปกติ 10 วินาทีแล้วหลับ
#else
constexpr uint32_t FOCUS_MS = 25UL * 60 * 1000;
constexpr uint32_t BREAK_MS = 5UL * 60 * 1000;
constexpr uint32_t IDLE_TO_SLEEP_MS = 3UL * 60 * 1000;  // หน้าปกติ 3 นาทีแล้วหลับ
#endif
constexpr uint8_t SMALL_ROUNDS_PER_BIG = 4;

// ---- เกม ----
constexpr uint8_t ANIMAL_COUNT = 15;           // ตัวปกติ index 0-14
constexpr uint8_t SECRET_INDEX = ANIMAL_COUNT; // ตัวซีเคร็ต index 15
constexpr uint8_t ROUNDS_PER_UNLOCK = 3;
constexpr uint8_t SECRET_ROUNDS = 100;

// ---- อนิเมชันไข่ฟัก ----
constexpr uint32_t HATCH_SHAKE_MS = 3000;
constexpr uint32_t HATCH_CRACK_MS = 1000;
constexpr uint32_t HATCH_REVEAL_MS = 3000; // โชว์ตัวใหม่ก่อนกลับหน้าปกติ

// ---- หน้าสำเร็จ (จบรอบใหญ่) ----
constexpr uint32_t SUCCESS_XP_MS = 1500;     // โชว์ +XP ก่อน
constexpr uint32_t SUCCESS_TROPHY_MS = 2500; // แล้วโชว์ถ้วยพร้อมอนิเมชัน
constexpr uint32_t SUCCESS_MS = SUCCESS_XP_MS + SUCCESS_TROPHY_MS;
constexpr uint16_t XP_PER_BIG_ROUND = 1000;
