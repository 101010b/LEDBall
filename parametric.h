#ifndef __PARAMETRIC_H__
#define __PARAMETRIC_H__

typedef struct intparam_s {
  int32_t minval;
  int32_t maxval;
  int32_t defaultval;
  int32_t val;
} intparam_t;

typedef struct colparam_s {
  uint8_t r,g,b;
  uint8_t rdefault, gdefault, bdefault;
} colparam_t;

typedef struct boolparam_s {
  bool val;
  bool defaultval;
} boolparam_t;

typedef struct stringparam_s {
  char *val;
  char *defaultval;
} stringparam_t;

#define PARAM_VOID 0
#define PARAM_INT 1
#define PARAM_COL 2
#define PARAM_BOOL 3
#define PARAM_STRING 4

typedef struct param_s {
  char name[32];
  uint8_t type;
  union {
    intparam_t i;
    colparam_t c;
    boolparam_t b;
    stringparam_t s;
  } d;
  struct param_s *next;
} param_t;

class parametric {
  public:
  char name[32];
  param_t *params;

  param_t *findparam(char *name);
  bool setparam(param_t *p, bool val);
  bool setparam(param_t *p, int val);
  bool setparam(param_t *p, uint8_t r, uint8_t g, uint8_t b);
  bool setparam(param_t *p, char *val);
  bool resetparam(param_t *p);
  bool resetparams();
  param_t *newparam(char *name);
  param_t *addparam(param_t *p);
  param_t *newintparam(char *name, int32_t minval, int32_t maxval, int32_t defaultval);
  param_t *newboolparam(char *name, bool defaultval);
  param_t *newcolparam(char *name, uint8_t rdefault, uint8_t gdefault, uint8_t bdefault);
  param_t *newstringparam(char *name, char *sdefault);
  bool decodestringcol(char *val, uint8_t *r, uint8_t *g, uint8_t *b);
  bool formatparam(param_t *p, char *buffer, int buflen);
  virtual void updateparams();
  virtual void updateparam(param_t *param);
  virtual void tick();

  parametric(char *_name);
};

#endif
// EOF
