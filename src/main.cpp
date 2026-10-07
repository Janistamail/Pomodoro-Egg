#include <Arduino.h>
#include <OneButton.h>
#include <U8g2lib.h>
#include <Wire.h>

#include "config.h"
#include "progress.h"

enum class State
{
  Sleep,
  Normal,
  Select,
  Focus,
  Break,
  Hatch
};

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
OneButton button(PIN_BUTTON, true, true); // active low + pullup ภายใน
Progress progress;

State state = State::Sleep;
uint32_t stateStart = 0; // millis() ตอนเข้า state ปัจจุบัน
uint8_t smallRound = 0;  // 0-3 รอบเล็กปัจจุบัน
uint8_t selectCursor = 0;
int8_t hatchAnimal = -1;

// ---------- state ----------

uint32_t elapsed() { return millis() - stateStart; }

void enterState(State next)
{
  state = next;
  stateStart = millis();
}

void goHome()
{
  enterState(progress.selected() >= 0 ? State::Normal : State::Sleep);
}

void finishBigRound()
{
  int8_t unlocked = progress.completeBigRound();
  Serial.printf("big round %u done, unlocked=%d\n", progress.bigRounds(), unlocked);
  if (unlocked >= 0)
  {
    hatchAnimal = unlocked;
    enterState(State::Hatch);
  }
  else
  {
    goHome();
  }
}

void updateState()
{
  switch (state)
  {
  case State::Normal:
    if (elapsed() >= IDLE_TO_SLEEP_MS)
      enterState(State::Sleep);
    break;
  case State::Focus:
    if (elapsed() >= FOCUS_MS)
      enterState(State::Break);
    break;
  case State::Break:
    if (elapsed() >= BREAK_MS)
    {
      smallRound++;
      if (smallRound < SMALL_ROUNDS_PER_BIG)
      {
        enterState(State::Focus);
      }
      else
      {
        finishBigRound();
      }
    }
    break;
  default:
    break;
  }
}

// ---------- ปุ่ม ----------

void onClick()
{
  Serial.println("DEBUG: click");
  switch (state)
  {
  case State::Sleep:
    if (progress.selected() >= 0)
      enterState(State::Normal);
    break;
  case State::Normal:
    stateStart = millis(); // รีเซ็ตเวลานับเข้า Sleep
    break;
  case State::Select:
    selectCursor = (selectCursor + 1) % progress.unlockedCount();
    break;
  case State::Hatch:
    if (elapsed() >= HATCH_SHAKE_MS + HATCH_CRACK_MS)
      enterState(State::Normal);
    break;
  default:
    break;
  }
}

void onDoubleClick()
{
  if (state != State::Sleep && state != State::Normal)
    return;
  if (progress.unlockedCount() == 0)
    return;
  selectCursor = max<int8_t>(progress.selected(), 0);
  enterState(State::Select);
}

void onLongPress()
{
  Serial.println("DEBUG: long press");
  switch (state)
  {
  case State::Sleep:
  case State::Normal:
    smallRound = 0;
    enterState(State::Focus);
    break;
  case State::Select:
    progress.select(selectCursor);
    enterState(State::Normal);
    break;
  case State::Focus:
  case State::Break:
    goHome(); // ยกเลิก รอบนี้ไม่นับ
    break;
  default:
    break;
  }
}

// ---------- วาดจอ (placeholder รอใส่ bitmap จริง) ----------

void drawCentered(const char *text, int y)
{
  u8g2.drawStr((128 - u8g2.getStrWidth(text)) / 2, y, text);
}

void animalName(int8_t index, char *out, size_t size)
{
  if (index == SECRET_INDEX)
  {
    snprintf(out, size, "SECRET");
  }
  else
  {
    snprintf(out, size, "Animal %d", index + 1);
  }
}

void drawSleep()
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
  u8g2.drawHLine(54, 54, 20); // ปาก
}

void drawNormal()
{
  char name[16];
  animalName(progress.selected(), name, sizeof(name));
  int bob = (millis() / 500) % 2 * 3; // ขยับขึ้นลง 2 เฟรม
  u8g2.setFont(u8g2_font_ncenB14_tr);
  drawCentered(name, 36 + bob);
}

void drawSelect()
{
  char name[16], line[24];
  animalName(selectCursor, name, sizeof(name));
  u8g2.setFont(u8g2_font_6x10_tr);
  drawCentered("SELECT", 10);
  snprintf(line, sizeof(line), "%d/%d", selectCursor + 1, progress.unlockedCount());
  drawCentered(line, 62);
  u8g2.setFont(u8g2_font_ncenB14_tr);
  drawCentered(name, 40);
}

void drawTimer(const char *label, uint32_t duration)
{
  uint32_t e = elapsed();
  uint32_t remain = (e < duration ? duration - e : 0) * TIME_DIVISOR / 1000; // วินาทีตามเวลาจริง
  char line[24];

  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 10, label);
  snprintf(line, sizeof(line), "%d/%d", smallRound + 1, SMALL_ROUNDS_PER_BIG);
  u8g2.drawStr(128 - u8g2.getStrWidth(line), 10, line);

  snprintf(line, sizeof(line), "%02lu:%02lu", (unsigned long)(remain / 60), (unsigned long)(remain % 60));
  u8g2.setFont(u8g2_font_logisoso24_tn);
  drawCentered(line, 48);
}

// หน้าโฟกัส: หน้าตั้งใจ (คิ้วเฉียง) + เวลานับถอยหลังด้านล่าง
void drawFocus()
{
  uint32_t e = elapsed();
  uint32_t remain = (e < FOCUS_MS ? FOCUS_MS - e : 0) * TIME_DIVISOR / 1000;
  char line[24];

  u8g2.drawRBox(34, 14, 20, 16, 4); // ตา
  u8g2.drawRBox(74, 14, 20, 16, 4);
  u8g2.drawLine(30, 5, 54, 11); // คิ้วเฉียงลงเข้าหากลาง
  u8g2.drawLine(30, 6, 54, 12);
  u8g2.drawLine(98, 5, 74, 11);
  u8g2.drawLine(98, 6, 74, 12);
  u8g2.drawHLine(54, 38, 20); // ปากตรง

  u8g2.setFont(u8g2_font_6x10_tr);
  snprintf(line, sizeof(line), "%d/%d", smallRound + 1, SMALL_ROUNDS_PER_BIG);
  u8g2.drawStr(0, 62, line);

  snprintf(line, sizeof(line), "%02lu:%02lu", (unsigned long)(remain / 60), (unsigned long)(remain % 60));
  u8g2.setFont(u8g2_font_logisoso16_tn);
  drawCentered(line, 62);
}

void drawHatch()
{
  uint32_t e = elapsed();
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
    char name[16];
    animalName(hatchAnimal, name, sizeof(name));
    u8g2.setFont(u8g2_font_6x10_tr);
    drawCentered("NEW!", 12);
    u8g2.setFont(u8g2_font_ncenB14_tr);
    drawCentered(name, 42);
  }
}

void render()
{
  u8g2.clearBuffer();
  switch (state)
  {
  case State::Sleep:
    drawSleep();
    break;
  case State::Normal:
    drawNormal();
    break;
  case State::Select:
    drawSelect();
    break;
  case State::Focus:
    drawFocus();
    break;
  case State::Break:
    drawTimer("BREAK", BREAK_MS);
    break;
  case State::Hatch:
    drawHatch();
    break;
  }
  u8g2.sendBuffer();
}

// ---------- คำสั่งทดสอบทาง Serial ----------

#ifdef DEBUG_FAST
void handleSerial()
{
  if (!Serial.available())
    return;
  char c = Serial.read();
  if (c == 'r')
  { // ล้างความคืบหน้า
    progress.reset();
    enterState(State::Sleep);
  }
  else if (c == '+' && state != State::Hatch)
  { // จบรอบใหญ่ทันที
    finishBigRound();
  }
  Serial.printf("rounds=%u unlocked=%u selected=%d\n", progress.bigRounds(),
                progress.unlockedCount(), progress.selected());
}
#endif

// ---------- main ----------

void setup()
{
  Serial.begin(115200);
  Serial.println("DEBUG: boot");
  Wire.begin(PIN_SDA, PIN_SCL);
  u8g2.begin();

  progress.load();

  button.attachClick(onClick);
  button.attachDoubleClick(onDoubleClick);
  button.attachLongPressStart(onLongPress);

  goHome();
}

void loop()
{
  button.tick();
  updateState();
#ifdef DEBUG_FAST
  handleSerial();
#endif

  static uint32_t lastBeat = 0;
  if (millis() - lastBeat >= 1000)
  {
    lastBeat = millis();
    Serial.printf("DEBUG: alive %lu ms, state=%d (0=Sleep 1=Normal 2=Select 3=Focus 4=Break 5=Hatch)\n", (unsigned long)millis(), (int)state);
  }

  static uint32_t lastFrame = 0;
  if (millis() - lastFrame >= 50)
  { // ~20 fps
    lastFrame = millis();
    render();
  }
}
