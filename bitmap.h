#ifndef __BITMAP_H__
#define __BITMAP_H__

#include "effect.h"

class bitmap: public effect {
  public:
  param_t *speed;
  param_t *picture;
  int flowctr;

  bitmap();
  void usemap(const uint8_t *data);
  void usemap(char *s);
  void tick();

  void updateparams();
  void updateparam(param_t *param);
};

#endif
// EOF
