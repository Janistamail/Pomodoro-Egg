#include "buzzer.h"

#include "config.h"

namespace
{
struct Note
{
  uint16_t freq; // Hz, 0 = เงียบ
  uint16_t ms;
};

// โน้ต (Hz)
constexpr uint16_t C5 = 523, E5 = 659, G5 = 784, C6 = 1047, E6 = 1319, G6 = 1568;

const Note CLICK[] = {{2000, 25}};
const Note FOCUS[] = {{C5, 90}, {0, 30}, {G5, 90}, {0, 30}, {C6, 160}};
const Note BREAK[] = {{C6, 90}, {0, 30}, {G5, 90}, {0, 30}, {C5, 160}};
const Note CANCEL[] = {{400, 120}, {0, 30}, {300, 180}};
const Note SUCCESS[] = {{C5, 120}, {E5, 120}, {G5, 120}, {C6, 200}, {0, 60},
                        {G5, 100}, {C6, 350}};
const Note SHAKE[] = {{300, 60}, {0, 60}, {350, 60}, {0, 60}, {300, 60}, {0, 60}, {350, 60}};
const Note CRACK[] = {{1800, 40}, {0, 20}, {2400, 40}, {0, 20}, {1200, 80}};
const Note HATCH[] = {{G5, 120}, {C6, 120}, {E6, 120}, {G6, 200}, {0, 60},
                      {E6, 120}, {G6, 400}};

struct Song
{
  const Note *notes;
  uint8_t len;
};

#define SONG(a) {a, sizeof(a) / sizeof(a[0])}
const Song SONGS[] = {SONG(CLICK), SONG(FOCUS), SONG(BREAK),  SONG(CANCEL),
                      SONG(SUCCESS), SONG(SHAKE), SONG(CRACK), SONG(HATCH)};
#undef SONG

const Song *current = nullptr;
uint8_t idx = 0;
uint32_t noteEnd = 0;

void startNote()
{
  const Note &n = current->notes[idx];
  if (n.freq)
    tone(PIN_BUZZER, n.freq);
  else
    noTone(PIN_BUZZER);
  noteEnd = millis() + n.ms;
}
} // namespace

namespace buzzer
{
void begin()
{
  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER);
}

void play(Sound s)
{
  current = &SONGS[(int)s];
  idx = 0;
  startNote();
}

void update()
{
  if (!current || (int32_t)(millis() - noteEnd) < 0)
    return;
  if (++idx >= current->len)
  {
    noTone(PIN_BUZZER);
    current = nullptr;
    return;
  }
  startNote();
}
} // namespace buzzer
