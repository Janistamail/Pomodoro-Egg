#pragma once
#include <U8g2lib.h>

// วาดหน้าสัตว์ 16 ตัว (index 0-14 ปกติ, 15 = Moon Alien) ลงบนจอ 128x64
// (cx, cy) = จุดกึ่งกลางหน้า ขนาดรวมประมาณ 60x50 พิกเซล
namespace animals
{
const char *name(int8_t index);
void draw(U8G2 &g, int8_t index, int cx, int cy);
} // namespace animals
