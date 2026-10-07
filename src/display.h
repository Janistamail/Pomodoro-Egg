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
};

namespace display
{
void begin();
void render(const View &v);
} // namespace display
