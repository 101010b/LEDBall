#ifndef __ROTOR_H__
#define __ROTOR_H__

#include <arduino.h>
#include <stdint.h>

#include "effect.h"
#include "colorgen.h"

class rotor: public effect {
    public:
    param_t *rotv;
    param_t *col;
    param_t *useglobalcol;
    param_t *decayr;
    param_t *decayg;
    param_t *decayb;
    colorgen *globalcol;
    int ofs;
    int rotdelay;

    uint8_t *xtabler;
    uint8_t *xtableg;
    uint8_t *xtableb;

    rotor();
    
    void updateparams();
    void updateparam(param_t *param);
    void recalcshape();
    void setGlobalCol(colorgen *_col);
    void tick();
};

#endif
// EOF
