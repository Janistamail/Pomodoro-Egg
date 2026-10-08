#include <Arduino.h>
#include <OneButton.h>
#include <Wire.h>

#include "buzzer.h"
#include "config.h"
#include "display.h"
#include "progress.h"
#include "state.h"

OneButton button(PIN_BUTTON, true, true); // active low + pullup ภายใน
Progress progress;

State state = State::Sleep;
uint32_t stateStart = 0; // millis() ตอนเข้า state ปัจจุบัน
uint8_t smallRound = 0;  // 0-3 รอบเล็กปัจจุบัน
uint8_t selectCursor = 0;
uint8_t collectionCursor = 0;
int8_t hatchAnimal = -1;
uint8_t hatchStage = 0; // 0 = สั่น, 1 = แตก, 2 = โชว์ตัว (ใช้ปล่อยเสียงครั้งเดียวต่อช่วง)

// ---------- state ----------

uint32_t elapsed() { return millis() - stateStart; }

void enterState(State next)
{
  state = next;
  stateStart = millis();
  switch (next)
  {
  case State::Focus:
    buzzer::play(Sound::Focus);
    break;
  case State::Break:
    buzzer::play(Sound::Break);
    break;
  case State::Success:
    buzzer::play(Sound::Success);
    break;
  case State::Hatch:
    hatchStage = 0;
    buzzer::play(Sound::Shake);
    break;
  default:
    break;
  }
}

// ปลดล็อกครบทุกตัวแล้ว และรอบนี้ไม่ได้ปลดล็อกตัวใหม่
bool gameComplete()
{
  return progress.bigRounds() >= SECRET_ROUNDS && hatchAnimal < 0;
}

void goHome()
{
  enterState(State::Normal); // หน้าปกติ: ไข่ (ยังไม่มีตัว) หรือตัวที่เลือก แล้วหลับเองเมื่อครบเวลา
}

void finishBigRound()
{
  int8_t unlocked = progress.completeBigRound();
  Serial.printf("big round %u done, unlocked=%d\n", progress.bigRounds(), unlocked);
  hatchAnimal = unlocked; // -1 = ไม่ได้ปลดล็อกตัวใหม่ ไม่ต้องฟัก
  enterState(State::Success);
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
  case State::Success:
    if (elapsed() >= (gameComplete() ? SUCCESS_TROPHY_MS : SUCCESS_MS))
    {
      if (hatchAnimal >= 0)
        enterState(State::Hatch);
      else
        goHome();
    }
    break;
  case State::Hatch:
    if (hatchStage == 0 && elapsed() >= HATCH_SHAKE_MS)
    {
      hatchStage = 1;
      buzzer::play(Sound::Crack);
    }
    else if (hatchStage == 1 && elapsed() >= HATCH_SHAKE_MS + HATCH_CRACK_MS)
    {
      hatchStage = 2;
      buzzer::play(Sound::Hatch);
    }
    if (elapsed() >= HATCH_SHAKE_MS + HATCH_CRACK_MS + HATCH_REVEAL_MS)
      goHome();
    break;
  default:
    break;
  }
}

// ---------- ปุ่ม ----------

void onClick()
{
  Serial.println("DEBUG: click");
  if (state == State::Sleep || state == State::Normal || state == State::Select ||
      state == State::Collection)
    buzzer::play(Sound::Click);
  switch (state)
  {
  case State::Sleep:
    enterState(State::Normal);
    break;
  case State::Normal:
    stateStart = millis(); // รีเซ็ตเวลานับเข้า Sleep
    break;
  case State::Collection:
    collectionCursor = (collectionCursor + 1) % (ANIMAL_COUNT + 1);
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
  Serial.printf("DEBUG: double click (state=%d unlocked=%u)\n", (int)state, progress.unlockedCount());
  if (state != State::Sleep && state != State::Normal)
    return;
  if (progress.unlockedCount() == 0)
    return;
  selectCursor = progress.rankOf(max<int8_t>(progress.selected(), 0));
  enterState(State::Select);
}

void onMultiClick()
{
  Serial.printf("DEBUG: multi click x%d (state=%d)\n", button.getNumberClicks(), (int)state);
  if (button.getNumberClicks() != 3)
    return;
  if (state != State::Sleep && state != State::Normal)
    return;
  buzzer::play(Sound::Click);
  collectionCursor = 0;
  enterState(State::Collection);
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
    progress.select(progress.nthUnlocked(selectCursor));
    enterState(State::Normal);
    break;
  case State::Collection:
    enterState(State::Normal);
    break;
  case State::Focus:
  case State::Break:
    buzzer::play(Sound::Cancel);
    goHome(); // ยกเลิก รอบนี้ไม่นับ
    break;
  default:
    break;
  }
}

// ---------- วาดจอ (โค้ดวาดอยู่ใน display.cpp) ----------

void render()
{
  View v = {state, elapsed(), smallRound, selectCursor, progress.unlockedCount(), hatchAnimal, progress.selected(), collectionCursor, progress.unlockedMask(), progress.nthUnlocked(selectCursor), gameComplete()};
  display::render(v);
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
  else if (c == '+' && state != State::Hatch && state != State::Success)
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
  display::begin();
  buzzer::begin();

  progress.load();
  Serial.printf("DEBUG: loaded rounds=%u unlocked=%u selected=%d\n", progress.bigRounds(),
                progress.unlockedCount(), progress.selected());

  button.setClickMs(500); // เว้นระหว่างการกดได้นานขึ้นเล็กน้อย กดรัวสามครั้งง่ายขึ้น
  button.attachClick(onClick);
  button.attachDoubleClick(onDoubleClick);
  button.attachMultiClick(onMultiClick);
  button.attachLongPressStart(onLongPress);

  goHome();
}

void loop()
{
  button.tick();
  updateState();
  buzzer::update();
#ifdef DEBUG_FAST
  handleSerial();
#endif

  static uint32_t lastBeat = 0;
  if (millis() - lastBeat >= 1000)
  {
    lastBeat = millis();
    Serial.printf("DEBUG: alive %lu ms, state=%d (0=Sleep 1=Normal 2=Select 3=Focus 4=Break 5=Success 6=Hatch 7=Collection)\n", (unsigned long)millis(), (int)state);
  }

  static uint32_t lastFrame = 0;
  if (millis() - lastFrame >= 50)
  { // ~20 fps
    lastFrame = millis();
    render();
  }
}
