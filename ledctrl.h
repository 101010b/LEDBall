#ifndef __LEDCTRL_H__
#define __LEDCTRL_H__

#include <Arduino.h>
#include <stdint.h>
#include <list>

#include "effect.h"
#include "effectselector.h"
#include "colorgen.h"
#include "sparcle.h"

#define PINCLK             2
#define PINDATA            0

#ifndef M_PI
  #define M_PI           3.14159265358979323846  /* pi */
#endif
#define EXPON 3
#define OSG 1.0/3.0
#define OSB 2.0/3.0

uint8_t fadeComponent(int a, int ff, int b);
uint8_t interpolate(uint8_t a, int ff, uint8_t b);
int minterpolate(int a, int f, int max,  int b);
uint8_t fromDouble(double d);
double sqr(double a);
double sin2(double a);
void randcol(uint8_t *r, uint8_t *g, uint8_t *b);

class ledctrl {
	public:

  // Raw data
	uint8_t LEDRED[LEDS];
	uint8_t LEDGREEN[LEDS];
	uint8_t LEDBLUE[LEDS];
	uint8_t GLOBAL;
    
  std::list<parametric *> globals;
  std::list<effect *> list;

  effectselector *effectselect=NULL;
  colorgen *colorgenA=NULL;
  colorgen *colorgenB=NULL;
  
	int length();
	void CLOCKBYTE(uint8_t b);
	void progLeds()	;
  void allOff();
	void allLeds(uint8_t red, uint8_t green, uint8_t blue);
	void startUpSequence();
	void setGlobal(uint16_t c);
	void setRange(uint16_t first, uint16_t last, uint16_t red, uint16_t green, uint16_t blue);
	void shiftEnds(char fromLeft, uint16_t red, uint16_t green, uint16_t blue);
	void shiftCenter(char FlowOut, uint16_t red, uint16_t green, uint16_t blue);
	void fadeTo(uint16_t ff, uint16_t red, uint16_t green, uint16_t blue);
	void cloudIn(char sym, int pos, int width, uint16_t red, uint16_t green, uint16_t blue);
  void errorCode(uint8_t x);

  void updatefromeffect(effect *e);
  effect *findeffect(char *e);
  
  int findeffectindex(effect *e);
  effect *getselectedeffect();
  parametric *findglobal(char *e);
  void selecteffect(char *e);
  parametric *findparametric(char *pname);
  void tick();
  	
	ledctrl();	
};

#endif
