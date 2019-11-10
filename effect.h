#ifndef __EFFECT_H__
#define __EFFECT_H__

#include <arduino.h>
#include <stdint.h>

#include "parametric.h"

#define LEDS 392
#define LEDW 60
#define LEDH 19

extern const int16_t bandtable[1140];
extern const uint16_t ledindex[392];
extern const int8_t xtable[392];
extern const int8_t ytable[392];
extern const int8_t ztable[392];
extern const uint8_t phitable[392];
extern const uint8_t thetatable[392];

typedef struct ledlist_s {
  uint8_t r[LEDS];
  uint8_t g[LEDS];
  uint8_t b[LEDS];
} ledlist_t;

typedef struct ledfield_s {
  uint8_t r[LEDW*LEDH];
  uint8_t g[LEDW*LEDH];
  uint8_t b[LEDW*LEDH];
} ledfield_t;

class effect: public parametric {
  public:
  bool isfield;
  ledlist_t *list;
  ledfield_t *field;
  
  effect(char *_name, bool _isfield);
  
  void listdecay(uint8_t decayfactor);  
  void listdecayr(uint8_t decayfactor);
  void listdecayg(uint8_t decayfactor);
  void listdecayb(uint8_t decayfactor);

  void shiftbufferleft(uint8_t *buffer, uint8_t n);
  void shiftbufferright(uint8_t *buffer, uint8_t n);
  void rollbufferleft(uint8_t *buffer, uint8_t n);
  void rollbufferright(uint8_t *buffer, uint8_t n);
  void rollfield(int8_t n);
  void shiftfield(int8_t n);
};


#endif
// EOF
