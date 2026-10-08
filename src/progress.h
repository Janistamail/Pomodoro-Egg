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
  uint16_t unlockedMask() const { return _mask; }  // bit i = ปลดล็อกตัว index i แล้ว
  bool isUnlocked(int8_t index) const { return index >= 0 && (_mask >> index) & 1; }
  int8_t nthUnlocked(uint8_t n) const;  // index ของตัวที่ปลดล็อกลำดับที่ n (เรียงตาม index)
  uint8_t rankOf(int8_t index) const;   // ตัว index เป็นลำดับที่เท่าไหร่ในกลุ่มที่ปลดล็อก

  void select(int8_t index);

  // นับรอบใหญ่เพิ่ม 1 แล้วคืน index ของตัวที่เพิ่งปลดล็อก (สุ่ม) หรือ -1 ถ้าไม่มี
  int8_t completeBigRound();

 private:
  void save();
  uint8_t _bigRounds = 0;
  int8_t _selected = -1;
  uint16_t _mask = 0;
};
