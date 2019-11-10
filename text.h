#ifndef __TEXT_H__
#define __TEXT_H__

#include "effect.h"
#include "colorgen.h"

class text: public effect {
  public:
  param_t *message;
  param_t *speed;
  param_t *col;
  param_t *useglobalcol;
  colorgen *globalcol;

  uint8_t curchar;
  uint16_t curcharw;
  uint16_t curcol;
  int16_t charpos;
  uint32_t curcharofs;

  text();
  void setGlobalCol(colorgen *_col);
  void nextchar(uint8_t c);
  void tick();
};



#endif
// EOF
