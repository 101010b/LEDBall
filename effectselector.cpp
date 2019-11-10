#include <arduino.h>
#include "effectselector.h"

effectselector::effectselector():parametric("EffectSelect") {
  selected_effect = -1;
  total_effects = 0;
}
