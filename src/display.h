#pragma once
#include <Arduino.h>

#include "state.h"

// ข้อมูลที่หน้าจอต้องใช้วาด (main.cpp เป็นคนเติมให้ทุกเฟรม)
struct View
{
  State state;
  uint32_t elapsed;      // ms ตั้งแต่เข้า state ปัจจุบัน
  uint8_t smallRound;    // 0-3
  uint8_t selectCursor;
  uint8_t unlockedCount;
  int8_t hatchAnimal;
  int8_t selectedAnimal; // -1 = ยังไม่ได้เลือกตัว
  uint8_t collectionCursor;
  uint16_t unlockedMask;
  int8_t selectAnimal; // ตัวที่ cursor ของหน้า Select ชี้อยู่
  bool gameComplete;   // ปลดล็อกครบทุกตัวแล้ว: หน้าสำเร็จโชว์แบบ "ครบแล้ว" แทน XP
};

namespace display
{
void begin();
void render(const View &v);
} // namespace display
