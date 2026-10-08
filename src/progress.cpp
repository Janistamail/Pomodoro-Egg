#include "progress.h"

#include <Preferences.h>
#include <esp_system.h>

#include "config.h"

static Preferences prefs;
static const char *NS = "egg";

void Progress::load()
{
  prefs.begin(NS, true);
  _bigRounds = prefs.getUChar("rounds", 0);
  _selected = prefs.getChar("sel", -1);
  if (prefs.isKey("mask"))
  {
    _mask = prefs.getUShort("mask", 0);
  }
  else
  {
    // ข้อมูลเก่าที่ยังไม่มี mask: ตัวที่ปลดล็อกแล้วคือไล่ตาม index จากตัวแรก
    uint8_t n = min<uint8_t>(_bigRounds / ROUNDS_PER_UNLOCK, ANIMAL_COUNT);
    _mask = (1U << n) - 1;
    if (_bigRounds >= SECRET_ROUNDS)
      _mask |= 1U << SECRET_INDEX;
  }
  prefs.end();
}

void Progress::save()
{
  prefs.begin(NS, false);
  prefs.putUChar("rounds", _bigRounds);
  prefs.putChar("sel", _selected);
  prefs.putUShort("mask", _mask);
  prefs.end();
}

void Progress::reset()
{
  _bigRounds = 0;
  _selected = -1;
  _mask = 0;
  save();
}

uint8_t Progress::unlockedCount() const
{
  return __builtin_popcount(_mask);
}

int8_t Progress::nthUnlocked(uint8_t n) const
{
  for (int8_t i = 0; i <= SECRET_INDEX; i++)
  {
    if (isUnlocked(i) && n-- == 0)
      return i;
  }
  return -1;
}

uint8_t Progress::rankOf(int8_t index) const
{
  uint8_t rank = 0;
  for (int8_t i = 0; i < index; i++)
    rank += isUnlocked(i);
  return rank;
}

void Progress::select(int8_t index)
{
  _selected = index;
  save();
}

int8_t Progress::completeBigRound()
{
  if (_bigRounds >= SECRET_ROUNDS)
    return -1; // จบเกมแล้ว ไม่นับต่อ

  _bigRounds++;
  int8_t unlocked = -1;
  if (_bigRounds == SECRET_ROUNDS)
  {
    unlocked = SECRET_INDEX;
  }
  else if (_bigRounds % ROUNDS_PER_UNLOCK == 0)
  {
    // สุ่มจากตัวธรรมดาที่ยังไม่มี
    int8_t locked[ANIMAL_COUNT];
    uint8_t n = 0;
    for (int8_t i = 0; i < ANIMAL_COUNT; i++)
    {
      if (!isUnlocked(i))
        locked[n++] = i;
    }
    if (n > 0)
      unlocked = locked[esp_random() % n];
  }
  if (unlocked >= 0)
  {
    _mask |= 1U << unlocked;
    _selected = unlocked; // ตัวใหม่ถูกเลือกให้อัตโนมัติ
  }
  save();
  return unlocked;
}
