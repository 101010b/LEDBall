#ifndef __FLOW_H__
#define __FLOW_H__

#include <arduino.h>
#include <stdint.h>

#include "effect.h"
#include "colorgen.h"

class flow: public effect {
  public:
  param_t *fm;
  param_t *fs;
  colorgen *globalcol;

  uint8_t *flowtabler;
  uint8_t *flowtableg;
  uint8_t *flowtableb;
  int flowctr;

  flow();
  void setGlobalCol(colorgen *_col);
  void tick();

};

#endif
// EOF
