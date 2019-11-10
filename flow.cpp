#include <arduino.h>
#include <stdint.h>

#include "flow.h"

#define FLOW_DOWN 0
#define FLOW_UP 1
#define FLOW_POLES 2
#define FLOW_EQ 3
#define FLOW_SPIRALDOWN 4
#define FLOW_SPIRALUP 5
#define FLOW_MAX 5

const uint16_t spiral[392]={391,386,385,390,389,388,387,380,379,384,383,382,381,371,377,372,360,361,355,347,353,348,336,337,331,323,329,324,311,318,312,370,
  376,375,378,374,373,368,359,366,367,362,356,346,352,351,354,350,349,344,335,342,343,338,332,322,328,327,330,326,325,320,310,317,
  319,313,307,299,369,286,365,364,363,262,345,249,341,340,339,225,321,212,316,315,314,305,300,294,285,292,287,281,358,357,261,268,
  263,257,248,255,250,244,334,333,224,231,226,220,211,218,213,207,309,308,298,304,306,301,291,293,288,273,279,274,267,269,264,254,
  256,251,236,242,237,230,232,227,217,219,214,198,204,199,303,302,295,284,290,289,282,272,278,280,275,270,260,266,265,258,247,253,
  252,245,235,241,243,238,233,223,229,228,221,210,216,215,208,197,203,206,205,200,297,296,180,175,181,283,164,277,271,276,151,259,
  143,138,144,246,127,240,234,239,114,222,106,101,107,209,89,202,196,201,188,183,174,179,182,177,176,163,170,165,159,150,157,152,
  146,137,142,145,140,139,126,133,128,122,113,120,115,109,100,105,108,103,102,88,95,96,90,187,194,189,190,173,178,172,169,171,
  166,156,158,153,136,141,135,132,134,129,119,121,116,99,104,98,94,97,91,193,195,191,184,65,66,162,168,167,160,149,155,154,
  147,41,42,125,131,130,123,112,118,117,110,16,17,87,93,92,85,186,192,71,72,67,161,53,148,47,48,43,124,29,111,22,
  23,24,86,77,185,64,70,73,68,61,52,57,58,60,59,55,54,40,46,49,44,37,28,33,34,36,35,31,30,15,21,25,
  19,18,76,81,82,84,83,79,78,63,69,62,51,56,50,39,45,38,27,32,26,14,20,13,75,80,74,4,5,0,1,2,
  3,11,6,7,8,9,10,12};


flow::flow():effect("Flow",0) {
  fm = newintparam("mode", FLOW_DOWN, FLOW_MAX, FLOW_DOWN); 
  fs = newintparam("speed", 0, 50, 15); 
  flowtabler = (uint8_t*) malloc(LEDW);
  flowtableg = (uint8_t*) malloc(LEDW);
  flowtableb = (uint8_t*) malloc(LEDW);
  memset(flowtabler,0,LEDW);
  memset(flowtableg,0,LEDW);
  memset(flowtableb,0,LEDW);
  flowctr=0;
  globalcol = NULL;
}

void flow::setGlobalCol(colorgen *_col) {
  globalcol = _col;
}

void shiftbuffer(uint8_t *buffer, int n, uint8_t fill) {
  if (n <= 0) return;
  if (n >= LEDW) {
    memset(buffer,fill, LEDW);
    return;
  }
  memmove(buffer+n, buffer, LEDW-n);
  memset(buffer,fill,n);
}

void flow::tick() {
  uint8_t r, g, b;
  if (!globalcol) return;
  if (fs->d.i.val < 20) {
    // 0..19
    int spd = 21 - fs->d.i.val; // 2..21
    if (flowctr >= spd) {
      flowctr = 0;
      shiftbuffer(flowtabler, 1, globalcol->r);
      shiftbuffer(flowtableg, 1, globalcol->g);
      shiftbuffer(flowtableb, 1, globalcol->b);
    }
    flowctr++;
  } else {
    // 20..
    int spd = fs->d.i.val - 19; // 1..x
    shiftbuffer(flowtabler, spd, globalcol->r);
    shiftbuffer(flowtableg, spd, globalcol->g);
    shiftbuffer(flowtableb, spd, globalcol->b);
  }
  
  switch (fm->d.i.val) {
    case FLOW_UP:
      for (int i=0;i<LEDS;i++) {
        list->r[i] = flowtabler[thetatable[i]];
        list->g[i] = flowtableg[thetatable[i]];
        list->b[i] = flowtableb[thetatable[i]];
      }
      break;
    case FLOW_DOWN:
      for (int i=0;i<LEDS;i++) {
        list->r[i] = flowtabler[18-thetatable[i]];
        list->g[i] = flowtableg[18-thetatable[i]];
        list->b[i] = flowtableb[18-thetatable[i]];
      }
      break;
    case FLOW_EQ:
      for (int i=0;i<LEDS;i++) {
        uint8_t theta = thetatable[i];
        if (theta < 9) {
          list->r[i] = flowtabler[theta];
          list->g[i] = flowtableg[theta];
          list->b[i] = flowtableb[theta];
        } else {
          list->r[i] = flowtabler[18-theta];
          list->g[i] = flowtableg[18-theta];
          list->b[i] = flowtableb[18-theta];
        }
      }
      break;
    case FLOW_POLES:
      for (int i=0;i<LEDS;i++) {
        uint8_t theta = thetatable[i];
        if (theta < 9) {
          list->r[i] = flowtabler[8-theta];
          list->g[i] = flowtableg[8-theta];
          list->b[i] = flowtableb[8-theta];
        } else {
          list->r[i] = flowtabler[theta-9];
          list->g[i] = flowtableg[theta-9];
          list->b[i] = flowtableb[theta-9];
        }
      }
      break;
    case FLOW_SPIRALUP:
      for (int i=LEDS-1;i > 0;i--) {
        int a = spiral[i];
        int b = spiral[i-1];
        list->r[a]=list->r[b];
        list->g[a]=list->g[b];
        list->b[a]=list->b[b];
      }
      list->r[spiral[0]] = globalcol->r;
      list->g[spiral[0]] = globalcol->g;
      list->b[spiral[0]] = globalcol->b;
      break;
    case FLOW_SPIRALDOWN:
      for (int i=0;i < LEDS-1;i++) {
        int a = spiral[i];
        int b = spiral[i+1];
        list->r[a]=list->r[b];
        list->g[a]=list->g[b];
        list->b[a]=list->b[b];
      }
      list->r[spiral[LEDS-1]] = globalcol->r;
      list->g[spiral[LEDS-1]] = globalcol->g;
      list->b[spiral[LEDS-1]] = globalcol->b;
      break;            
  }
}
