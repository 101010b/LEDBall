#ifndef __COLORGEN_H__
#define __COLORGEN_H__

#include <arduino.h>
#include <stdint.h>

#include "parametric.h"

class colorgen: public parametric {
  public:
  uint8_t r,g,b;
  param_t *mincol;
  param_t *maxcol;
  param_t *newcoltime;
  uint8_t startr, startg, startb;
  uint8_t nextr, nextg, nextb;
  uint16_t colorstep;
  
  colorgen(char *name);
  uint8_t interpolate8(uint8_t start, uint8_t stop, uint16_t step, uint16_t steps);
  void newnextcol();
  void tick();
};

#endif
// EOF
