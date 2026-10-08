#include "display.h"

#include <U8g2lib.h>

#include "animals.h"
#include "config.h"

static U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// ---------- ตัวช่วย ----------

static void drawCentered(const char *text, int y)
{
  u8g2.drawStr((128 - u8g2.getStrWidth(text)) / 2, y, text);
}

static void animalName(int8_t index, char *out, size_t size)
{
  snprintf(out, size, "%s", animals::name(index));
}

// เวลาที่เหลือเป็น MM:SS (ปัดขึ้นเป็นวินาที ไม่โชว์มิลลิวินาที)
static void formatRemain(uint32_t elapsed, uint32_t duration, char *out, size_t size)
{
  uint32_t remain = (elapsed < duration ? duration - elapsed + 999 : 0) / 1000;
  snprintf(out, size, "%02lu:%02lu", (unsigned long)(remain / 60), (unsigned long)(remain % 60));
}

// ---------- วาดหน้าต่าง ๆ (placeholder รอใส่ bitmap จริง) ----------

static void drawSleep()
{
  bool blink = millis() % 3000 < 150;
  if (blink)
  {
    u8g2.drawBox(30, 28, 24, 3);
    u8g2.drawBox(74, 28, 24, 3);
  }
  else
  {
    u8g2.drawRBox(30, 16, 24, 26, 6);
    u8g2.drawRBox(74, 16, 24, 26, 6);
  }
  // ยิ้ม
  u8g2.drawLine(56, 50, 60, 54);
  u8g2.drawLine(56, 51, 60, 55);
  u8g2.drawHLine(60, 54, 8);
  u8g2.drawHLine(60, 55, 8);
  u8g2.drawLine(72, 50, 68, 54);
  u8g2.drawLine(72, 51, 68, 55);
}

static void drawEgg()
{
  int wobble = (millis() / 600) % 2 ? 1 : -1; // โยกซ้ายขวาเบา ๆ
  int cx = 64 + wobble;
  u8g2.drawEllipse(cx, 32, 18, 24);
  u8g2.drawEllipse(cx, 32, 17, 23);
  u8g2.drawDisc(cx - 6, 24, 2); // ลายจุดบนไข่
  u8g2.drawDisc(cx + 7, 34, 3);
  u8g2.drawDisc(cx - 4, 44, 2);
}

static void drawNormal(const View &v)
{
  // หน้าปกติแสดงตัวที่เลือกไว้ ถ้ายังไม่เคยเลือกให้แสดงไข่
  if (v.selectedAnimal < 0)
  {
    drawEgg();
    return;
  }

  // หายใจเบา ๆ ตลอดเวลา และกระโดดเด้งทุก 4 วินาที
  uint32_t t = millis() % 4000;
  int dy = (millis() / 500) % 2 ? 0 : -1;
  if (t < 400)
    dy -= 6 * 4 * t * (400 - t) / (400 * 400); // วิถีโค้งพาราโบลา สูงสุด 6 พิกเซล
  animals::draw(u8g2, v.selectedAnimal, 64, 34 + dy);
}

// หน้าสะสม: ดูทีละตัว ตัวที่ยังไม่ปลดล็อกโชว์เครื่องหมาย ?
static void drawCollection(const View &v)
{
  const int total = ANIMAL_COUNT + 1;
  int8_t i = v.collectionCursor;
  bool open = (v.unlockedMask >> i) & 1;
  char line[12];

  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.drawStr(0, 7, "COLLECTION");
  snprintf(line, sizeof(line), "%d/%d", i + 1, total); // หน้าปัจจุบัน/ทั้งหมด
  u8g2.drawStr(128 - u8g2.getStrWidth(line), 7, line);

  if (open)
  {
    animals::draw(u8g2, i, 64, 34);
    u8g2.setFont(u8g2_font_6x10_tr);
    drawCentered(animals::name(i), 63);
  }
  else
  {
    u8g2.setFont(u8g2_font_logisoso32_tr);
    drawCentered("?", 52);
    u8g2.setFont(u8g2_font_6x10_tr);
    drawCentered("???", 63);
  }
}

// หน้าเลือกตัว: โชว์ตัวที่ปลดล็อกแล้วทีละตัวแบบเดียวกับหน้าสะสม
static void drawSelect(const View &v)
{
  char line[12];

  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.drawStr(0, 7, "SELECT");
  snprintf(line, sizeof(line), "%d/%d", v.selectCursor + 1, v.unlockedCount);
  u8g2.drawStr(128 - u8g2.getStrWidth(line), 7, line);

  int dy = (millis() / 500) % 2 ? 0 : -1; // หายใจเบา ๆ
  animals::draw(u8g2, v.selectAnimal, 64, 34 + dy);
  u8g2.setFont(u8g2_font_6x10_tr);
  drawCentered(animals::name(v.selectAnimal), 63);
}

static void drawTimer(const View &v, const char *label, uint32_t duration)
{
  char line[24];

  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 10, label);
  u8g2.setFont(u8g2_font_5x7_tr);
  snprintf(line, sizeof(line), "%d/%d", v.smallRound + 1, SMALL_ROUNDS_PER_BIG);
  u8g2.drawStr(128 - u8g2.getStrWidth(line), 7, line);

  formatRemain(v.elapsed, duration, line, sizeof(line));
  u8g2.setFont(u8g2_font_logisoso24_tn);
  drawCentered(line, 48);
}

// หน้าโฟกัส: หน้าตั้งใจ (คิ้วเฉียง) + เวลานับถอยหลังด้านล่าง
static void drawFocus(const View &v)
{
  char line[24];

  u8g2.drawRBox(34, 14, 20, 16, 4); // ตา
  u8g2.drawRBox(74, 14, 20, 16, 4);
  u8g2.drawLine(30, 5, 54, 11); // คิ้วเฉียงลงเข้าหากลาง
  u8g2.drawLine(30, 6, 54, 12);
  u8g2.drawLine(98, 5, 74, 11);
  u8g2.drawLine(98, 6, 74, 12);
  u8g2.drawHLine(54, 38, 20); // ปากตรง

  u8g2.setFont(u8g2_font_5x7_tr);
  snprintf(line, sizeof(line), "%d/%d", v.smallRound + 1, SMALL_ROUNDS_PER_BIG);
  u8g2.drawStr(128 - u8g2.getStrWidth(line), 7, line); // มุมขวาบน

  formatRemain(v.elapsed, FOCUS_MS, line, sizeof(line));
  u8g2.setFont(u8g2_font_helvB12_tn);
  drawCentered(line, 62);
}

static void drawHatch(const View &v)
{
  uint32_t e = v.elapsed;
  if (e < HATCH_SHAKE_MS)
  {
    int shake = (millis() / 80) % 2 ? 3 : -3;
    u8g2.drawEllipse(64 + shake, 32, 16, 22);
  }
  else if (e < HATCH_SHAKE_MS + HATCH_CRACK_MS)
  {
    u8g2.drawEllipse(64, 32, 16, 22);
    u8g2.drawLine(50, 30, 58, 36); // รอยร้าว
    u8g2.drawLine(58, 36, 66, 28);
    u8g2.drawLine(66, 28, 78, 34);
  }
  else
  {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(0, 8, "NEW!");
    animals::draw(u8g2, v.hatchAnimal, 64, 34);
    drawCentered(animals::name(v.hatchAnimal), 63);
  }
}

static void drawSparkle(int x, int y, int r)
{
  u8g2.drawHLine(x - r, y, 2 * r + 1);
  u8g2.drawVLine(x, y - r, 2 * r + 1);
}

static void drawTrophy(int dy)
{
  u8g2.drawCircle(44, 17 + dy, 6); // หู
  u8g2.drawCircle(84, 17 + dy, 6);
  u8g2.drawBox(46, 6 + dy, 36, 12);
  u8g2.drawRBox(46, 6 + dy, 36, 26, 10); // ตัวถ้วย
  u8g2.drawBox(60, 32 + dy, 8, 6);       // ก้าน
  u8g2.drawBox(54, 38 + dy, 20, 3);
  u8g2.drawBox(48, 41 + dy, 32, 4);      // ฐาน
}

static void drawSuccess(const View &v)
{
  uint32_t e = v.elapsed;
  // ปลดล็อกครบทุกตัวแล้ว: ข้ามหน้า +XP ไปโชว์ถ้วยเลย
  uint32_t trophyStart = v.gameComplete ? 0 : SUCCESS_XP_MS;
  if (e < trophyStart)
  {
    // ตัวเลข XP นับขึ้นใน 1 วินาทีแรก
    uint32_t count = XP_PER_BIG_ROUND * min<uint32_t>(e, 1000) / 1000;
    char xp[16];
    snprintf(xp, sizeof(xp), "+%lu XP", (unsigned long)count);
    u8g2.setFont(u8g2_font_ncenB14_tr);
    drawCentered(xp, 40);
    return;
  }

  uint32_t t = e - trophyStart;
  int dy = 8;
  if (t < 400)
    dy += 24 * (400 - t) / 400;              // ถ้วยลอยขึ้นมา
  else
    dy += (millis() / 300) % 2 ? 0 : -2;     // แล้วเด้งเบา ๆ

  drawTrophy(dy);

  if (t >= 400)
  {
    // ประกายระยิบระยับ
    static const int8_t pos[4][2] = {{24, 14}, {104, 10}, {30, 48}, {100, 46}};
    for (uint8_t i = 0; i < 4; i++)
    {
      uint8_t phase = (millis() / 200 + i * 2) % 6;
      if (phase < 3)
        drawSparkle(pos[i][0], pos[i][1], phase == 1 ? 3 : 1);
    }
  }
}

// ---------- public ----------

void display::begin()
{
  u8g2.setBusClock(400000); // 100 kHz ทำให้ส่งภาพแต่ละเฟรมช้า (~90 ms) ปุ่มเลยอ่านไม่ทัน
  u8g2.begin();
}

void display::render(const View &v)
{
  u8g2.clearBuffer();
  switch (v.state)
  {
  case State::Sleep:
    drawSleep();
    break;
  case State::Normal:
    drawNormal(v);
    break;
  case State::Select:
    drawSelect(v);
    break;
  case State::Focus:
    drawFocus(v);
    break;
  case State::Break:
    drawTimer(v, "BREAK", BREAK_MS);
    break;
  case State::Success:
    drawSuccess(v);
    break;
  case State::Hatch:
    drawHatch(v);
    break;
  case State::Collection:
    drawCollection(v);
    break;
  }
  u8g2.sendBuffer();
}
