#include "progress.h"

#include <Preferences.h>

#include "config.h"

static Preferences prefs;
static const char *NS = "egg";

void Progress::load() {
  prefs.begin(NS, true);
  _bigRounds = prefs.getUChar("rounds", 0);
  _selected = prefs.getChar("sel", -1);
  prefs.end();
}

void Progress::save() {
  prefs.begin(NS, false);
  prefs.putUChar("rounds", _bigRounds);
  prefs.putChar("sel", _selected);
  prefs.end();
}

void Progress::reset() {
  _bigRounds = 0;
  _selected = -1;
  save();
}

uint8_t Progress::unlockedCount() const {
  uint8_t n = min<uint8_t>(_bigRounds / ROUNDS_PER_UNLOCK, ANIMAL_COUNT);
  if (_bigRounds >= SECRET_ROUNDS) n++;
  return n;
}

void Progress::select(int8_t index) {
  _selected = index;
  save();
}

int8_t Progress::completeBigRound() {
  if (_bigRounds >= SECRET_ROUNDS) return -1;  // จบเกมแล้ว ไม่นับต่อ

  _bigRounds++;
  int8_t unlocked = -1;
  if (_bigRounds == SECRET_ROUNDS) {
    unlocked = SECRET_INDEX;
  } else if (_bigRounds % ROUNDS_PER_UNLOCK == 0 &&
             _bigRounds <= ANIMAL_COUNT * ROUNDS_PER_UNLOCK) {
    unlocked = _bigRounds / ROUNDS_PER_UNLOCK - 1;
  }
  if (unlocked >= 0) _selected = unlocked;  // ตัวใหม่ถูกเลือกให้อัตโนมัติ
  save();
  return unlocked;
}
