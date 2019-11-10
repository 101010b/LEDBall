#include <arduino.h>
#include <stdint.h>

#include "sparcle.h"

sparcle::sparcle():effect("Sparcle",0) {
  sparcN = newintparam("N", 1, 100, 10); 
  col = newcolparam("Col", 0, 0, 255);
  useglobalcol = newboolparam("global", true);
  decayr = newintparam("decayr", 0, 4, 2);
  decayg = newintparam("decayg", 0, 4, 2);
  decayb = newintparam("decayb", 0, 4, 2);
  globalcol = NULL;
}

void sparcle::setGlobalCol(colorgen *_col) {
  globalcol = _col;
}

void sparcle::tick() {
  uint8_t r, g, b;
  listdecayr(decayr->d.i.val);
  listdecayg(decayg->d.i.val);
  listdecayb(decayb->d.i.val);
  if (useglobalcol->d.b.val && globalcol) {
    r = globalcol->r;
    g = globalcol->g;
    b = globalcol->b;
  } else {
    r = col->d.c.r;
    g = col->d.c.g;
    b = col->d.c.b;
  }
  for (int i=0;i<sparcN->d.i.val;i++) {
    uint32_t idx = rand() % LEDS;
    list->r[idx]=r;
    list->g[idx]=g;
    list->b[idx]=b;
  }
}
