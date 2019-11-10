#ifndef __RAIN_H__
#define __RAIN_H__

#include "effect.h"
#include "colorgen.h"

class rain: public effect {
  public:
  param_t *sparcN;
  param_t *col;
  param_t *useglobalcol;
  param_t *decay;
  colorgen *globalcol;

  rain();
  void setGlobalCol(colorgen *_col);
  void tick();
};



#endif
// EOF
