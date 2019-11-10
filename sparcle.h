#ifndef __SPARCLE_H__
#define __SPARCLE_H__

#include "effect.h"
#include "colorgen.h"

class sparcle: public effect {
  public:
  param_t *sparcN;
  param_t *col;
  param_t *useglobalcol;
  param_t *decayr;
  param_t *decayg;
  param_t *decayb;
  colorgen *globalcol;

  sparcle();
  void setGlobalCol(colorgen *_col);
  void tick();
};

#endif
// EOF
