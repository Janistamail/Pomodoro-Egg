#pragma once
#include <Arduino.h>

// ความคืบหน้าของเกม เก็บลง flash (NVS)
class Progress {
 public:
  void load();
  void reset();

  uint8_t bigRounds() const { return _bigRounds; }
  int8_t selected() const { return _selected; }  // -1 = ยังไม่มีตัวละคร
  uint8_t unlockedCount() const;

  void select(int8_t index);

  // นับรอบใหญ่เพิ่ม 1 แล้วคืน index ของตัวที่เพิ่งปลดล็อก หรือ -1 ถ้าไม่มี
  int8_t completeBigRound();

 private:
  void save();
  uint8_t _bigRounds = 0;
  int8_t _selected = -1;
};
