#include <arduino.h>
#include <stdint.h>

#include "rotor.h"

const uint8_t rowcos[19]={0,44,87,127,164,195,221,240,251,255,251,240,221,195,164,128,87,44,0};
const uint8_t colexp1[60]={255,246,238,230,222,215,208,201,194,187,181,175,169,164,158,153,148,143,138,133,129,125,120,116,113,109,105,102,98,95,92,89,
  86,83,80,77,75,72,70,67,65,63,61,59,57,55,53,51,50,48,46,45,43,42,40,39,38,36,35,34};
const uint8_t colexp2[60]={255,234,215,197,181,166,153,140,129,118,109,100,92,84,77,71,65,60,55,50,46,43,39,36,33,30,28,25,23,21,20,18,
  16,15,14,13,12,11,10,9,8,7,7,6,6,5,5,4,4,4,3,3,3,2,2,2,2,2,1,1};
const uint8_t colexp3[60]={255,215,181,153,129,109,92,77,65,55,46,39,33,28,23,20,16,14,12,10,8,7,6,5,4,3,3,2,2,1,1,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const uint8_t colexp4[60]={255,181,129,92,65,46,33,23,16,12,8,6,4,3,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const uint8_t colexp1b[60]={0,46,85,119,147,171,191,207,221,231,239,246,250,253,254,255,254,252,250,247,244,240,236,231,227,222,217,212,206,201,196,191,
  185,180,175,170,165,160,156,151,146,142,137,133,129,125,121,117,113,110,106,103,99,96,93,90,87,84,81,79};
const uint8_t colexp2b[60]={0,95,161,205,233,248,255,254,250,242,232,220,208,196,183,171,159,148,137,127,117,108,100,92,85,78,72,66,61,56,51,47,
  43,40,37,34,31,28,26,24,22,20,18,17,15,14,13,12,11,10,9,8,8,7,6,6,5,5,4,4};
const uint8_t colexp3b[60]={0,161,233,255,250,232,208,183,159,137,117,100,85,72,61,51,43,37,31,26,22,18,15,13,11,9,8,6,5,4,4,3,
  2,2,2,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const uint8_t colexp4b[60]={0,237,255,212,162,120,87,62,44,31,22,16,11,8,5,4,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

typedef const uint8_t *colexp_t;
const colexp_t colexp[8] = {colexp1, colexp2, colexp3, colexp4, colexp1b, colexp2b, colexp3b, colexp4b};

rotor::rotor():effect("Rotor",0) {
  rotv = newintparam("v", -50, 50, 25);
  col = newcolparam("Col", 255, 255, 255);
  useglobalcol = newboolparam("global", true);
  decayr = newintparam("decayr", 0, 7, 3);
  decayg = newintparam("decayg", 0, 7, 3);
  decayb = newintparam("decayb", 0, 7, 1);
  globalcol = NULL;
  ofs=0;
  rotdelay=0;

  xtabler = (uint8_t*) malloc(LEDW);
  xtableg = (uint8_t*) malloc(LEDW);
  xtableb = (uint8_t*) malloc(LEDW);
  
  recalcshape();
}

void rotor::recalcshape() {
  uint8_t r,g,b;
  if (useglobalcol->d.b.val && globalcol) {
    r = globalcol->r;
    g = globalcol->g;
    b = globalcol->b;
  } else {
    r = col->d.c.r;
    g = col->d.c.g;
    b = col->d.c.b;
  }
  const uint8_t *exptabler = colexp[decayr->d.i.val];
  const uint8_t *exptableg = colexp[decayg->d.i.val];
  const uint8_t *exptableb = colexp[decayb->d.i.val];
  int dir = (rotv->d.i.val<0)?-1:1;

  for (int i=0;i<LEDW;i++) {
    int x = (i + ofs) % LEDW;
    uint16_t xr = (dir==1)?exptabler[x]:exptabler[LEDW-1-x];
    uint16_t xg = (dir==1)?exptableg[x]:exptableg[LEDW-1-x];
    uint16_t xb = (dir==1)?exptableb[x]:exptableb[LEDW-1-x];
    xtabler[i] = (xr * r) / 256;
    xtableg[i] = (xg * g) / 256;
    xtableb[i] = (xb * b) / 256;    
  }

  for (int i=0;i<LEDS;i++) {
    int x = phitable[i];
    int y = thetatable[i];
    list->r[i] = ((uint16_t)xtabler[x] * rowcos[y]) / 256;
    list->g[i] = ((uint16_t)xtableg[x] * rowcos[y]) / 256;
    list->b[i] = ((uint16_t)xtableb[x] * rowcos[y]) / 256;    
  }
  
/*  
  for (int u=0;u<LEDW;u++) {
    int x=(u + ofs) % LEDW;
    uint16_t xr = (dir==1)?exptabler[x]:exptabler[LEDW-1-x];
    uint16_t xg = (dir==1)?exptableg[x]:exptableg[LEDW-1-x];
    uint16_t xb = (dir==1)?exptableb[x]:exptableb[LEDW-1-x];
    uint16_t vr = (xr * r) / 256;
    uint16_t vg = (xg * g) / 256;
    uint16_t vb = (xb * b) / 256;
    for (int y=0;y<LEDH;y++) {
      int q = y * LEDW + u;
      field->r[q] = (vr * rowcos[y]) / 256;
      field->g[q] = (vg * rowcos[y]) / 256;
      field->b[q] = (vb * rowcos[y]) / 256;
    }
  }*/
}

void rotor::updateparams() {
  //recalcshape();
}

void rotor::updateparam(param_t *param) {
  //if ((param == rotv) || (param == decayr)|| (param == decayg)|| (param == decayb))
  //  recalcshape();
}

void rotor::setGlobalCol(colorgen *_col) {
  globalcol = _col;
}

void rotor::tick() {
  recalcshape();
  
  int rv = rotv->d.i.val;
  if (rv == 0) return;
  int dir = (rv < 0)?-1:1;
  rv = (rv < 0)?-rv:rv;
  if (rv < 20) {
    int spd = 21 - rv;
    if (rotdelay >= spd) {
      if (dir > 0) {
        ofs++;
        if (ofs >= LEDW) ofs-=LEDW;
      } else {
        ofs--;
        if (ofs < 0) ofs+=LEDW;
      }
      rotdelay = 0;      
    }
    rotdelay++;
  } else {
    int spd = rv - 19;
    if (dir > 0) {
      ofs+=spd;
      if (ofs >= LEDW) ofs-=LEDW;
    } else {
      ofs-=spd;
      if (ofs < 0) ofs+=LEDW;
    }
  }
}
