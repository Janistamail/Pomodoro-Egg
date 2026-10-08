#include "animals.h"

#include "config.h"

namespace
{

// สไตล์หุ่นยนต์ทึบ: หัวเป็นแผ่นสีขาวทึบ มีหน้าจอ (visor) สีดำตัดเข้าไป และดวงตาเป็นไฟ LED

// ---------- ตัวช่วย ----------

// หัวทึบ + หมุดน็อตที่มุม
void plate(U8G2 &g, int cx, int cy, int hw, int hh, int r = 6)
{
  int in = r >= 8 ? 3 : 2;
  g.setDrawColor(1);
  g.drawRBox(cx - hw, cy - hh, 2 * hw, 2 * hh, r);
  g.setDrawColor(0);
  g.drawPixel(cx - hw + in, cy - hh + in);
  g.drawPixel(cx + hw - in - 1, cy - hh + in);
  g.drawPixel(cx - hw + in, cy + hh - in - 1);
  g.drawPixel(cx + hw - in - 1, cy + hh - in - 1);
  g.setDrawColor(1);
}

// หน้าจอสีดำบนหัว
void visor(U8G2 &g, int cx, int cy, int hw, int hh)
{
  g.setDrawColor(0);
  g.drawRBox(cx - hw, cy - hh, 2 * hw, 2 * hh, 4);
  g.setDrawColor(1);
}

// ตา LED สี่เหลี่ยมมน (w x h) กึ่งกลางที่ (x, y) พร้อมประกายสีดำ
void led(U8G2 &g, int x, int y, int w, int h)
{
  int x0 = x - w / 2, y0 = y - h / 2;
  if (w >= 4 && h >= 4)
    g.drawRBox(x0, y0, w, h, 1);
  else
    g.drawBox(x0, y0, w, h);
  g.setDrawColor(0);
  g.drawPixel(x0 + 1, y0 + 1);
  g.setDrawColor(1);
}

// ช่องตาสีดำมีไฟ LED ขาวอยู่ข้างใน (ใช้บนหัวที่ลงเงา)
void socket(U8G2 &g, int x, int y, int w, int h)
{
  g.setDrawColor(0);
  g.drawRBox(x - w / 2, y - h / 2, w, h, 2);
  g.setDrawColor(1);
  led(g, x, y, w - 2, h - 2);
}

// ช่องปากแบบตะแกรง
void grill(U8G2 &g, int cx, int y, int h)
{
  g.setDrawColor(0);
  for (int i = -2; i <= 2; i++)
    g.drawVLine(cx + i * 2, y, h);
  g.setDrawColor(1);
}

void ledEyes(U8G2 &g, int cx, int y, int dx, int w, int h)
{
  led(g, cx - dx, y, w, h);
  led(g, cx + dx, y, w, h);
}

// ตาเป็นจุดกลมมีประกาย
void dotEyes(U8G2 &g, int cx, int y, int dx, int r)
{
  g.drawDisc(cx - dx, y, r);
  g.drawDisc(cx + dx, y, r);
  g.setDrawColor(0);
  g.drawPixel(cx - dx - 1, y - 1);
  g.drawPixel(cx + dx - 1, y - 1);
  g.setDrawColor(1);
}

// ปากยิ้มเส้นสั้น ๆ
void smile(U8G2 &g, int cx, int y)
{
  g.drawLine(cx - 3, y, cx - 1, y + 2);
  g.drawHLine(cx - 1, y + 2, 3);
  g.drawLine(cx + 1, y + 2, cx + 3, y);
}

// ลงเงาแบบตารางหมากรุกสีดำทับพื้นที่สี่เหลี่ยม (ให้ดูเป็นขนสีเข้ม)
void shade(U8G2 &g, int x0, int y0, int x1, int y1)
{
  g.setDrawColor(0);
  for (int y = y0; y <= y1; y++)
    for (int x = x0; x <= x1; x++)
      if (((x + y) & 1) == 0)
        g.drawPixel(x, y);
  g.setDrawColor(1);
}

void antenna(U8G2 &g, int cx, int top)
{
  g.drawVLine(cx, top - 4, 4);
  g.drawDisc(cx, top - 5, 2);
}

void clearDisc(U8G2 &g, int x, int y, int r)
{
  g.setDrawColor(0);
  g.drawDisc(x, y, r);
  g.setDrawColor(1);
}

void blackTri(U8G2 &g, int x0, int y0, int x1, int y1, int x2, int y2)
{
  g.setDrawColor(0);
  g.drawTriangle(x0, y0, x1, y1, x2, y2);
  g.setDrawColor(1);
}

void sparkle(U8G2 &g, int x, int y, int r)
{
  g.drawHLine(x - r, y, 2 * r + 1);
  g.drawVLine(x, y - r, 2 * r + 1);
}

// ---------- สัตว์แต่ละตัว ----------

void cat(U8G2 &g, int cx, int cy)
{
  g.drawTriangle(cx - 19, cy - 6, cx - 17, cy - 24, cx - 4, cy - 13);
  g.drawTriangle(cx + 19, cy - 6, cx + 17, cy - 24, cx + 4, cy - 13);
  plate(g, cx, cy, 20, 15);
  blackTri(g, cx - 15, cy - 14, cx - 15, cy - 20, cx - 10, cy - 15);
  blackTri(g, cx + 15, cy - 14, cx + 15, cy - 20, cx + 10, cy - 15);
  visor(g, cx, cy, 16, 9);
  ledEyes(g, cx, cy - 2, 8, 5, 6);
  g.drawBox(cx - 1, cy + 2, 3, 2); // จมูก
  g.drawLine(cx - 4, cy + 6, cx, cy + 4);
  g.drawLine(cx, cy + 4, cx + 4, cy + 6);
  g.drawLine(cx - 20, cy + 1, cx - 27, cy - 1); // หนวด
  g.drawLine(cx - 20, cy + 5, cx - 27, cy + 7);
  g.drawLine(cx + 20, cy + 1, cx + 27, cy - 1);
  g.drawLine(cx + 20, cy + 5, cx + 27, cy + 7);
  antenna(g, cx, cy - 15);
}

void unicorn(U8G2 &g, int cx, int cy)
{
  // แผงคอ (ลงเงา) ทั้งสองข้างของหัว
  for (int sd = -1; sd <= 1; sd += 2)
  {
    int x = sd < 0 ? cx - 26 : cx + 17;
    g.drawRBox(x, cy - 12, 10, 10, 3);
    g.drawRBox(x + (sd < 0 ? -1 : 1), cy - 3, 9, 10, 3);
    g.drawRBox(x, cy + 6, 9, 9, 3);
    shade(g, x - 1, cy - 12, x + 10, cy + 15);
  }
  // หูแหลมเล็ก
  g.drawTriangle(cx - 16, cy - 9, cx - 15, cy - 20, cx - 8, cy - 14);
  g.drawTriangle(cx + 16, cy - 9, cx + 15, cy - 20, cx + 8, cy - 14);
  // หัว + หน้าจอ
  plate(g, cx, cy, 16, 15, 7);
  visor(g, cx, cy - 3, 13, 7);
  ledEyes(g, cx, cy - 3, 7, 4, 5);
  // จมูกกับปาก
  g.setDrawColor(0);
  g.drawPixel(cx - 3, cy + 9);
  g.drawPixel(cx + 3, cy + 9);
  g.drawLine(cx - 3, cy + 12, cx - 1, cy + 13);
  g.drawLine(cx + 3, cy + 12, cx + 1, cy + 13);
  g.drawHLine(cx - 1, cy + 13, 3);
  g.setDrawColor(1);
  // เขายูนิคอร์นเป็นเกลียว (ทำหน้าที่เป็นเสาอากาศ)
  g.drawTriangle(cx - 4, cy - 15, cx + 4, cy - 15, cx, cy - 32);
  g.setDrawColor(0);
  for (int i = 0; i < 4; i++)
  {
    int y = cy - 18 - i * 3;
    int half = (y - (cy - 32)) * 4 / 17;
    g.drawLine(cx - half, y + 1, cx + half, y - 1);
  }
  g.setDrawColor(1);
  g.drawDisc(cx, cy - 33, 1);
  uint8_t phase = (millis() / 250) % 3;
  sparkle(g, cx + 28, cy - 22, phase == 0 ? 3 : 1);
  sparkle(g, cx - 29, cy - 20, phase == 2 ? 3 : 1);
}

void corgi(U8G2 &g, int cx, int cy)
{
  // หูตั้งใหญ่แหลมแบบแผ่นโลหะ
  g.drawTriangle(cx - 21, cy - 1, cx - 18, cy - 28, cx - 4, cy - 12);
  g.drawTriangle(cx + 21, cy - 1, cx + 18, cy - 28, cx + 4, cy - 12);
  plate(g, cx, cy, 21, 15, 8);
  shade(g, cx - 19, cy - 13, cx + 19, cy + 1); // แผงขนส้ม
  blackTri(g, cx - 17, cy - 12, cx - 17, cy - 22, cx - 9, cy - 14);
  blackTri(g, cx + 17, cy - 12, cx + 17, cy - 22, cx + 9, cy - 14);
  g.drawBox(cx - 4, cy - 14, 9, 14);       // แถบขาวกลางหน้า
  g.drawRBox(cx - 10, cy + 1, 20, 14, 6);  // แผงปากยื่น
  socket(g, cx - 11, cy - 4, 7, 7);
  socket(g, cx + 11, cy - 4, 7, 7);
  g.setDrawColor(0);
  g.drawRBox(cx - 3, cy + 2, 7, 4, 1);     // จมูก
  g.drawRBox(cx - 5, cy + 8, 11, 6, 2);    // ปากอ้า
  g.setDrawColor(1);
  g.drawRBox(cx - 2, cy + 10, 5, 4, 1);    // ลิ้น
  antenna(g, cx, cy - 15);
}

void hamster(U8G2 &g, int cx, int cy)
{
  // หูกลมเล็ก
  g.drawDisc(cx - 14, cy - 13, 5);
  g.drawDisc(cx + 14, cy - 13, 5);
  plate(g, cx, cy, 18, 14, 9);
  shade(g, cx - 16, cy - 12, cx + 16, cy - 1); // ขนสีน้ำตาลด้านบน
  clearDisc(g, cx - 14, cy - 13, 2);
  clearDisc(g, cx + 14, cy - 13, 2);
  g.setDrawColor(0); // แถบเข้มกลางหัว
  g.drawVLine(cx, cy - 13, 7);
  g.drawVLine(cx - 1, cy - 13, 7);
  g.setDrawColor(1);
  // แก้มตุ้ยสองข้าง (กว้างกว่าหัว)
  for (int sd = -1; sd <= 1; sd += 2)
  {
    clearDisc(g, cx + sd * 17, cy + 5, 10);
    g.drawDisc(cx + sd * 17, cy + 5, 9);
    g.setDrawColor(0); // หนวด
    g.drawLine(cx + sd * 12, cy + 4, cx + sd * 20, cy + 2);
    g.drawLine(cx + sd * 12, cy + 7, cx + sd * 20, cy + 9);
    g.setDrawColor(1);
  }
  socket(g, cx - 8, cy - 5, 7, 7);
  socket(g, cx + 8, cy - 5, 7, 7);
  // จมูก ฟันหน้าคู่
  g.setDrawColor(0);
  g.drawRBox(cx - 2, cy, 5, 3, 1);
  g.drawRBox(cx - 5, cy + 4, 11, 8, 3);
  g.setDrawColor(1);
  g.drawBox(cx - 3, cy + 4, 3, 5);
  g.drawBox(cx + 1, cy + 4, 3, 5);
  // เมล็ดทานตะวันที่กำลังกอด
  g.drawFilledEllipse(cx, cy + 18, 5, 3);
  g.setDrawColor(0);
  g.drawLine(cx - 3, cy + 18, cx + 3, cy + 18);
  g.setDrawColor(1);
  g.drawDisc(cx - 7, cy + 17, 2);
  g.drawDisc(cx + 7, cy + 17, 2);
  antenna(g, cx, cy - 14);
}

void bear(U8G2 &g, int cx, int cy)
{
  g.drawDisc(cx - 16, cy - 12, 6);
  g.drawDisc(cx + 16, cy - 12, 6);
  plate(g, cx, cy, 20, 16, 8);
  clearDisc(g, cx - 16, cy - 12, 3);
  clearDisc(g, cx + 16, cy - 12, 3);
  visor(g, cx, cy, 16, 10);
  ledEyes(g, cx, cy - 4, 9, 4, 5);
  g.drawFilledEllipse(cx, cy + 4, 6, 4); // จมูกยื่น
  g.setDrawColor(0);
  g.drawBox(cx - 2, cy + 1, 5, 3);
  g.drawVLine(cx, cy + 4, 3);
  g.setDrawColor(1);
  antenna(g, cx, cy - 16);
}

void panda(U8G2 &g, int cx, int cy)
{
  // หูดำเป็นโมดูลสี่เหลี่ยมมนมีขอบขาว
  g.drawRBox(cx - 24, cy - 21, 12, 12, 5);
  g.drawRBox(cx + 12, cy - 21, 12, 12, 5);
  plate(g, cx, cy, 20, 15, 9);
  g.setDrawColor(0);
  g.drawRBox(cx - 22, cy - 19, 8, 8, 3);
  g.drawRBox(cx + 14, cy - 19, 8, 8, 3);
  g.drawRBox(cx - 15, cy - 9, 11, 14, 5); // ปื้นรอบตาสีดำ
  g.drawRBox(cx + 4, cy - 9, 11, 14, 5);
  g.setDrawColor(1);
  led(g, cx - 9, cy - 3, 4, 5);
  led(g, cx + 9, cy - 3, 4, 5);
  g.setDrawColor(0);
  g.drawRBox(cx - 3, cy + 5, 7, 4, 1); // จมูก
  g.setDrawColor(1);
  grill(g, cx, cy + 10, 3);
  antenna(g, cx, cy - 15);
}

void fox(U8G2 &g, int cx, int cy)
{
  g.drawTriangle(cx - 20, cy - 2, cx - 18, cy - 26, cx - 4, cy - 12);
  g.drawTriangle(cx + 20, cy - 2, cx + 18, cy - 26, cx + 4, cy - 12);
  g.drawRBox(cx - 20, cy - 15, 40, 20, 6); // หัวบน + คางแหลม
  g.drawTriangle(cx - 20, cy + 1, cx + 20, cy + 1, cx, cy + 17);
  blackTri(g, cx - 15, cy - 14, cx - 15, cy - 21, cx - 9, cy - 15);
  blackTri(g, cx + 15, cy - 14, cx + 15, cy - 21, cx + 9, cy - 15);
  visor(g, cx, cy - 3, 16, 7);
  g.drawTriangle(cx - 13, cy - 7, cx - 3, cy - 2, cx - 13, cy - 2); // ตาเฉียง
  g.drawTriangle(cx + 13, cy - 7, cx + 3, cy - 2, cx + 13, cy - 2);
  g.setDrawColor(0); // จมูกดำที่ปลายคาง
  g.drawDisc(cx, cy + 12, 2);
  g.drawLine(cx - 10, cy + 5, cx - 5, cy + 7); // ลายแก้ม
  g.drawLine(cx + 10, cy + 5, cx + 5, cy + 7);
  g.setDrawColor(1);
  antenna(g, cx, cy - 15);
}

void giraffe(U8G2 &g, int cx, int cy)
{
  // ออสซิโคน (เขาสั้น ปลายกลม) ทำหน้าที่เป็นเสาอากาศ
  for (int sd = -1; sd <= 1; sd += 2)
  {
    g.drawVLine(cx + sd * 6, cy - 22, 8);
    g.drawVLine(cx + sd * 6 + sd, cy - 22, 8);
    g.drawDisc(cx + sd * 6, cy - 23, 2);
  }
  // หูยื่นออกด้านข้าง
  g.drawFilledEllipse(cx - 20, cy - 8, 6, 3);
  g.drawFilledEllipse(cx + 20, cy - 8, 6, 3);
  g.setDrawColor(0);
  g.drawFilledEllipse(cx - 20, cy - 8, 3, 1);
  g.drawFilledEllipse(cx + 20, cy - 8, 3, 1);
  g.setDrawColor(1);
  plate(g, cx, cy, 15, 15, 6);
  // ลายปื้นของยีราฟ
  g.setDrawColor(0);
  g.drawRBox(cx - 12, cy - 13, 5, 4, 1);
  g.drawRBox(cx + 7, cy - 13, 5, 3, 1);
  g.drawRBox(cx - 13, cy + 5, 4, 5, 1);
  g.drawRBox(cx + 9, cy + 6, 5, 4, 1);
  g.setDrawColor(1);
  visor(g, cx, cy - 4, 11, 6);
  ledEyes(g, cx, cy - 4, 6, 4, 5);
  // จมูกยาวและปาก
  g.setDrawColor(0);
  g.drawRBox(cx - 6, cy + 5, 12, 8, 3);
  g.setDrawColor(1);
  g.drawPixel(cx - 3, cy + 8);
  g.drawPixel(cx + 3, cy + 8);
  g.drawHLine(cx - 2, cy + 11, 5);
}

void ufo(U8G2 &g, int cx, int cy)
{
  // เสาอากาศ
  antenna(g, cx, cy - 15);
  // โดมกระจก: ขอบขาว ข้างในดำ มีหน้าหุ่นเอเลี่ยน
  g.drawFilledEllipse(cx, cy - 2, 14, 14, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  g.setDrawColor(0);
  g.drawFilledEllipse(cx, cy - 2, 12, 12, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  g.setDrawColor(1);
  ledEyes(g, cx, cy - 7, 5, 3, 4);
  g.drawHLine(cx - 1, cy - 3, 3);
  g.drawPixel(cx - 9, cy - 8); // ประกายบนกระจก
  g.drawPixel(cx - 8, cy - 10);
  // ตัวจานบิน
  g.drawFilledEllipse(cx, cy + 3, 30, 9);
  g.setDrawColor(0);
  g.drawHLine(cx - 27, cy + 1, 55); // รอยต่อแผ่นโลหะ
  g.setDrawColor(1);
  // ไฟวิ่งรอบจาน
  uint8_t on = (millis() / 250) % 5;
  for (int i = 0; i < 5; i++)
  {
    int x = cx - 20 + i * 10;
    g.setDrawColor(0);
    g.drawRBox(x - 3, cy + 3, 7, 5, 2);
    g.setDrawColor(1);
    if (i == on)
      g.drawBox(x - 1, cy + 4, 3, 3);
    else
      g.drawPixel(x, cy + 5);
  }
  // ฐานล่าง + ลำแสงดูดตัวกระพริบ
  g.drawRBox(cx - 8, cy + 11, 16, 4, 2);
  uint8_t beam = (millis() / 200) % 3;
  for (int row = 0; row < 3; row++)
  {
    int y = cy + 17 + row * 3;
    int half = 8 + (row + 1) * 3;
    if (row == beam)
      g.drawHLine(cx - half, y, 2 * half + 1);
    else
    {
      g.drawPixel(cx - half, y);
      g.drawPixel(cx + half, y);
    }
  }
}

void otter(U8G2 &g, int cx, int cy)
{
  g.drawDisc(cx - 15, cy - 12, 4); // หูเล็กกลม
  g.drawDisc(cx + 15, cy - 12, 4);
  plate(g, cx, cy, 21, 15, 7);
  visor(g, cx, cy, 17, 9);
  dotEyes(g, cx, cy - 4, 9, 2);
  g.drawFilledEllipse(cx, cy + 4, 10, 5); // ปากกว้างแบน
  g.setDrawColor(0);
  g.drawFilledEllipse(cx, cy + 1, 3, 2); // จมูก
  g.drawVLine(cx, cy + 3, 3);
  g.drawPixel(cx - 6, cy + 4); // จุดหนวด
  g.drawPixel(cx - 7, cy + 6);
  g.drawPixel(cx + 6, cy + 4);
  g.drawPixel(cx + 7, cy + 6);
  g.setDrawColor(1);
  antenna(g, cx, cy - 15);
}

void koala(U8G2 &g, int cx, int cy)
{
  // หูกลมใหญ่ฟู
  g.drawRBox(cx - 31, cy - 20, 19, 19, 9);
  g.drawRBox(cx + 12, cy - 20, 19, 19, 9);
  plate(g, cx, cy, 19, 15, 10);
  shade(g, cx - 17, cy - 14, cx + 17, cy - 6);
  g.setDrawColor(0);
  g.drawRBox(cx - 28, cy - 17, 13, 13, 6);
  g.drawRBox(cx + 15, cy - 17, 13, 13, 6);
  g.setDrawColor(1);
  g.drawRBox(cx - 25, cy - 14, 7, 7, 3); // ขนในหู
  g.drawRBox(cx + 18, cy - 14, 7, 7, 3);
  g.setDrawColor(0);
  g.drawRBox(cx - 12, cy - 6, 4, 5, 1); // ตา
  g.drawRBox(cx + 8, cy - 6, 4, 5, 1);
  g.drawRBox(cx - 5, cy - 1, 11, 13, 5); // จมูกดำใหญ่
  g.setDrawColor(1);
  g.drawPixel(cx - 11, cy - 5);
  g.drawPixel(cx + 9, cy - 5);
  g.drawBox(cx - 3, cy + 1, 2, 3);
  antenna(g, cx, cy - 15);
}

void raccoon(U8G2 &g, int cx, int cy)
{
  g.drawTriangle(cx - 19, cy - 3, cx - 16, cy - 22, cx - 4, cy - 13);
  g.drawTriangle(cx + 19, cy - 3, cx + 16, cy - 22, cx + 4, cy - 13);
  plate(g, cx, cy, 20, 15);
  blackTri(g, cx - 15, cy - 14, cx - 15, cy - 19, cx - 10, cy - 15);
  blackTri(g, cx + 15, cy - 14, cx + 15, cy - 19, cx + 10, cy - 15);
  g.setDrawColor(0);
  g.drawRBox(cx - 18, cy - 7, 36, 11, 4); // หน้ากากรอบตา
  g.drawVLine(cx, cy - 14, 5);            // ลายหน้าผาก
  g.drawVLine(cx - 4, cy - 13, 4);
  g.drawVLine(cx + 4, cy - 13, 4);
  g.setDrawColor(1);
  ledEyes(g, cx, cy - 2, 8, 5, 4);
  g.setDrawColor(0);
  g.drawDisc(cx, cy + 7, 2);
  smile(g, cx, cy + 10);
  g.setDrawColor(1);
  antenna(g, cx, cy - 15);
}

void deer(U8G2 &g, int cx, int cy)
{
  for (int s = -1; s <= 1; s += 2) // เขากิ่งก้านหนา 2 พิกเซล
  {
    for (int o = 0; o < 2; o++)
    {
      g.drawLine(cx + s * (8 + o), cy - 13, cx + s * (11 + o), cy - 27);
      g.drawLine(cx + s * (10 + o), cy - 20, cx + s * (17 + o), cy - 25);
      g.drawLine(cx + s * (11 + o), cy - 27, cx + s * (16 + o), cy - 33);
      g.drawLine(cx + s * (11 + o), cy - 27, cx + s * (8 + o), cy - 33);
    }
  }
  g.drawFilledEllipse(cx - 19, cy - 8, 6, 3);
  g.drawFilledEllipse(cx + 19, cy - 8, 6, 3);
  plate(g, cx, cy, 16, 15, 7);
  g.setDrawColor(0); // จุดบนหัว
  g.drawPixel(cx - 4, cy - 12);
  g.drawPixel(cx + 4, cy - 12);
  g.drawPixel(cx, cy - 13);
  g.setDrawColor(1);
  visor(g, cx, cy + 1, 12, 8);
  ledEyes(g, cx, cy - 2, 6, 4, 5);
  g.drawRBox(cx - 2, cy + 3, 5, 3, 1); // จมูก
  antenna(g, cx, cy - 15);
}

void tiger(U8G2 &g, int cx, int cy)
{
  g.drawRBox(cx - 24, cy - 20, 12, 12, 5);
  g.drawRBox(cx + 12, cy - 20, 12, 12, 5);
  plate(g, cx, cy, 21, 15, 9);
  shade(g, cx - 19, cy - 13, cx + 19, cy + 13); // ขนส้ม
  g.setDrawColor(0);
  g.drawRBox(cx - 22, cy - 18, 8, 8, 3);
  g.drawRBox(cx + 14, cy - 18, 8, 8, 3);
  g.setDrawColor(1);
  g.drawRBox(cx - 10, cy + 1, 20, 14, 6); // แผงปากยื่น
  g.drawRBox(cx - 15, cy - 10, 11, 11, 3); // แผงรอบตา
  g.drawRBox(cx + 4, cy - 10, 11, 11, 3);
  // ลายพาดหน้าผากและแก้ม
  g.setDrawColor(0);
  g.drawBox(cx - 1, cy - 15, 3, 6);
  g.drawLine(cx - 6, cy - 14, cx - 4, cy - 10);
  g.drawLine(cx - 7, cy - 14, cx - 5, cy - 10);
  g.drawLine(cx + 6, cy - 14, cx + 4, cy - 10);
  g.drawLine(cx + 7, cy - 14, cx + 5, cy - 10);
  for (int sd = -1; sd <= 1; sd += 2)
  {
    g.drawLine(cx + sd * 21, cy - 1, cx + sd * 16, cy + 1);
    g.drawLine(cx + sd * 21, cy, cx + sd * 16, cy + 2);
    g.drawLine(cx + sd * 20, cy + 7, cx + sd * 12, cy + 8);
    g.drawLine(cx + sd * 20, cy + 8, cx + sd * 12, cy + 9);
  }
  g.drawRBox(cx - 3, cy + 2, 7, 4, 1); // จมูก
  g.setDrawColor(1);
  socket(g, cx - 9, cy - 5, 7, 7);
  socket(g, cx + 9, cy - 5, 7, 7);
  grill(g, cx, cy + 9, 3);
  antenna(g, cx, cy - 15);
}

void dragon(U8G2 &g, int cx, int cy)
{
  for (int s = -1; s <= 1; s += 2)
  {
    g.drawTriangle(cx + s * 8, cy - 13, cx + s * 15, cy - 11, cx + s * 15, cy - 25); // เขาสั้น
    g.drawTriangle(cx + s * 21, cy - 2, cx + s * 29, cy - 9, cx + s * 29, cy + 4);   // ครีบข้างหัว
  }
  plate(g, cx, cy, 21, 16, 8);
  g.drawTriangle(cx - 4, cy - 15, cx + 4, cy - 15, cx, cy - 23); // หนามกลางหัว
  visor(g, cx, cy, 17, 10);
  ledEyes(g, cx, cy - 4, 9, 5, 5);
  g.drawPixel(cx - 3, cy + 3); // รูจมูก
  g.drawPixel(cx + 3, cy + 3);
  g.drawLine(cx - 5, cy + 6, cx - 3, cy + 7); // ยิ้ม
  g.drawHLine(cx - 3, cy + 7, 7);
  g.drawLine(cx + 3, cy + 7, cx + 5, cy + 6);
  uint8_t puff = (millis() / 300) % 3; // ควันเล็ก ๆ ลอยขึ้น
  g.drawCircle(cx - 12 - puff, cy + 14 - puff * 3, 1);
  g.drawCircle(cx + 12 + puff, cy + 14 - puff * 3, 1);
  g.drawVLine(cx, cy - 27, 4); // เสาอากาศต่อจากหนามกลางหัว
  g.drawDisc(cx, cy - 28, 2);
}

void moonAlien(U8G2 &g, int cx, int cy)
{
  // พระจันทร์เสี้ยวด้านหลัง
  g.drawDisc(cx + 30, cy - 20, 8);
  clearDisc(g, cx + 34, cy - 23, 7);
  // พื้นผิวดวงจันทร์ มีหลุมอุกกาบาต
  g.drawEllipse(cx, cy + 18, 38, 5, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  g.drawEllipse(cx - 22, cy + 19, 5, 1);
  g.drawEllipse(cx + 20, cy + 18, 4, 1);
  g.drawEllipse(cx + 30, cy + 21, 3, 1);
  // เสาอากาศคู่
  for (int sd = -1; sd <= 1; sd += 2)
  {
    g.drawLine(cx + sd * 4, cy - 16, cx + sd * 8, cy - 26);
    g.drawLine(cx + sd * 4 + sd, cy - 16, cx + sd * 8 + sd, cy - 26);
    g.drawDisc(cx + sd * 8, cy - 28, 2);
  }
  // หัวใหญ่ทรงไข่ + ตาดำใหญ่เฉียงมีไฟ LED
  g.drawFilledEllipse(cx, cy - 8, 13, 10);
  g.setDrawColor(0);
  g.drawFilledEllipse(cx - 6, cy - 8, 5, 3);
  g.drawFilledEllipse(cx + 6, cy - 8, 5, 3);
  g.drawPixel(cx - 11, cy - 16); // หมุด
  g.drawPixel(cx + 11, cy - 16);
  g.drawHLine(cx - 1, cy - 1, 3); // ปาก
  g.setDrawColor(1);
  g.drawBox(cx - 7, cy - 9, 2, 2);
  g.drawBox(cx + 5, cy - 9, 2, 2);
  // ลำตัว + แผงอก LED
  plate(g, cx, cy + 8, 7, 6, 3);
  g.setDrawColor(0);
  g.drawRBox(cx - 4, cy + 5, 8, 4, 1);
  g.setDrawColor(1);
  if ((millis() / 400) % 2)
    g.drawBox(cx - 2, cy + 6, 4, 2);
  else
    g.drawPixel(cx, cy + 6);
  // แขน (ข้างขวาโบกมือ) และขา
  g.drawRBox(cx - 12, cy + 4, 3, 8, 1);
  int wave = (millis() / 300) % 2 ? 0 : -3;
  g.drawRBox(cx + 9, cy + 3 + wave, 3, 8, 1);
  g.drawRBox(cx - 5, cy + 14, 3, 4, 1);
  g.drawRBox(cx + 2, cy + 14, 3, 4, 1);
  uint8_t phase = (millis() / 250) % 4;
  sparkle(g, cx - 30, cy - 16, phase == 0 ? 3 : 1);
  sparkle(g, cx - 28, cy + 4, phase == 2 ? 3 : 1);
  sparkle(g, cx + 32, cy + 6, phase == 1 ? 3 : 1);
}

using DrawFn = void (*)(U8G2 &, int, int);
const DrawFn DRAWERS[SECRET_INDEX + 1] = {
    cat, unicorn, corgi, hamster, bear, panda, fox, giraffe,
    ufo, otter, koala, raccoon, deer, tiger, dragon, moonAlien};

const char *const NAMES[SECRET_INDEX + 1] = {
    "Cat", "Unicorn", "Corgi", "Hamster", "Bear", "Panda", "Fox", "Giraffe",
    "UFO", "Otter", "Koala", "Raccoon", "Deer", "Tiger", "Dragon", "Moon Alien"};

} // namespace

const char *animals::name(int8_t index)
{
  return (index >= 0 && index <= SECRET_INDEX) ? NAMES[index] : "?";
}

void animals::draw(U8G2 &g, int8_t index, int cx, int cy)
{
  if (index >= 0 && index <= SECRET_INDEX)
    DRAWERS[index](g, cx, cy);
}
