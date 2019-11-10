#ifndef __EFFECTSELECTOR_H__
#define __EFFECTSELECTOR_H__

#include <arduino.h>
#include <stdint.h>

#include "parametric.h"

class effectselector: public parametric {
  public:
  int8_t selected_effect;
  int8_t total_effects;

  effectselector();
};

#endif
// EOF
