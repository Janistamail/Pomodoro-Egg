#pragma once
#include <Arduino.h>

// เสียงสำหรับ passive buzzer (ไม่บล็อก: เรียก update() ทุก loop)
enum class Sound
{
  Click,   // กดปุ่ม
  Focus,   // เริ่มโฟกัส
  Break,   // เริ่มพัก
  Cancel,  // ยกเลิกรอบ
  Success, // จบรอบใหญ่
  Shake,   // ไข่สั่น
  Crack,   // ไข่แตก
  Hatch    // ตัวใหม่ออกมา
};

namespace buzzer
{
void begin();
void play(Sound s);
void update();
} // namespace buzzer
