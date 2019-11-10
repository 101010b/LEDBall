#include <arduino.h>
#include <stdint.h>

#include "parametric.h"

parametric::parametric(char *_name) {
  strcpy(name, _name);
  params = NULL;
}

param_t *parametric::addparam(param_t *p) {
  if (!p) return NULL;
  if (p->type == PARAM_VOID) {
    free(p);
    return NULL;
  }
  param_t *q = params;
  p->next = NULL;
  if (q) {
    while (q->next) q=q->next;
    q->next = p;
  } else {
    params = p;
  }  
  return p;
}

param_t *parametric::newparam(char *name) {
  if (strlen(name) > 31) return NULL;
  param_t *p = (param_t *)malloc(sizeof(param_t));
  if (!p) return NULL;
  strcpy(p->name, name);
  p->type = PARAM_VOID;
  return p;
}

param_t *parametric::newintparam(char *name, int32_t minval, int32_t maxval, int32_t defaultval) {
  param_t *p = newparam(name);
  if (!p) return NULL;
  p->type = PARAM_INT;
  p->d.i.minval = minval;
  p->d.i.maxval = maxval;
  p->d.i.defaultval = defaultval;
  p->d.i.val = defaultval;
  return addparam(p);
}

param_t *parametric::newcolparam(char *name, uint8_t rdefault, uint8_t gdefault, uint8_t bdefault) {
  param_t *p = newparam(name);
  if (!p) return NULL;
  p->type = PARAM_COL;
  p->d.c.r = p->d.c.rdefault=rdefault;
  p->d.c.g = p->d.c.gdefault=gdefault;
  p->d.c.b = p->d.c.bdefault=bdefault;
  return addparam(p);
}

param_t *parametric::newboolparam(char *name, bool defaultval) {
  param_t *p = newparam(name);
  if (!p) return NULL;
  p->type = PARAM_BOOL;
  p->d.b.val = p->d.b.defaultval = defaultval;
  return addparam(p);
}

param_t *parametric::newstringparam(char *name, char *sdefault) {
  param_t *p = newparam(name);
  if (!p) return NULL;
  p->type = PARAM_STRING;
  if (sdefault && strlen(sdefault)) {
    p->d.s.defaultval = (char *)malloc(strlen(sdefault)+1);
    if (!p->d.s.defaultval) {
      free(p);
      return NULL;
    }
    strcpy(p->d.s.defaultval, sdefault);    
    p->d.s.val = (char *)malloc(strlen(sdefault)+1);
    if (!p->d.s.val) {
      free(p->d.s.defaultval);
      free(p);
      return NULL;
    }
    strcpy(p->d.s.val, sdefault);
  } else {
    p->d.s.defaultval = NULL;
    p->d.s.val = NULL;
  }
  return addparam(p);
}

param_t *parametric::findparam(char *name) {
  if (!name) return NULL;
  if (!*name) return NULL;
  param_t *q = params;
  while (q) {
    if (strcasecmp(name,q->name) == 0) {
      return q;
    }
    q=q->next;
  }
  return NULL;
}

bool parametric::setparam(param_t *p, bool val) {
  if (!p) return false;
  if (p->type != PARAM_BOOL) return false;
  p->d.b.val = val;
  return true;
}

bool parametric::setparam(param_t *p, int val) {
  if (!p) return false;
  if (p->type == PARAM_BOOL) {
    p->d.b.val = (val)?true:false;
    return true;    
  }
  if (p->type == PARAM_COL) {
    uint32_t cv = val;
    p->d.c.r = ((cv && 0x00FF0000) >> 16);
    p->d.c.g = ((cv && 0x0000FF00) >> 8);
    p->d.c.b = (cv && 0x000000FF);
    return true;
  }
  if (p->type != PARAM_INT) return false;
  if (val < p->d.i.minval) return false;
  if (val > p->d.i.maxval) return false;
  p->d.i.val = val;
  return true;  
}

bool parametric::setparam(param_t *p, uint8_t r, uint8_t g, uint8_t b) {
  if (!p) return false;
  if (p->type != PARAM_COL) return false;
  p->d.c.r = r;
  p->d.c.g = g;
  p->d.c.b = b;
  return true;  
}

bool parametric::decodestringcol(char *val, uint8_t *r, uint8_t *g, uint8_t *b) {
  if (!val || !*val) return false;
  if ((strcasecmp(val,"k")==0) || (strcasecmp(val,"OFF")==0) || (strcasecmp(val,"BLACK")==0)) {
    *r = *g = *b = 0;
    return true;
  }
  if ((strcasecmp(val,"w")==0) || (strcasecmp(val,"ON")==0) || (strcasecmp(val,"WHITE")==0)) {
    *r = *g = *b = 255;
    return true;
  }
  if ((strcasecmp(val,"r")==0) || (strcasecmp(val,"red")==0)) { 
    *r = 255; *g = 0; *b = 0;
    return true;
  }
  if ((strcasecmp(val,"g")==0) || (strcasecmp(val,"green")==0)) { 
    *r = 0; *g = 255; *b = 0;
    return true;
  }
  if ((strcasecmp(val,"b")==0) || (strcasecmp(val,"blue")==0)) { 
    *r = 0; *g = 0; *b = 255;
    return true;
  }
  if ((strcasecmp(val,"c")==0) || (strcasecmp(val,"cyan")==0)) { 
    *r = 0; *g = 255; *b = 255;
    return true;
  }
  if ((strcasecmp(val,"m")==0) || (strcasecmp(val,"magenta")==0)) { 
    *r = 255; *g = 0; *b = 255;
    return true;
  }
  if ((strcasecmp(val,"y")==0) || (strcasecmp(val,"yellow")==0)){ 
    *r = 255; *g = 255; *b = 0;
    return true;
  }
  if (*val == '#') {
    // Hex token
    if (strlen(val) != 7) return false;
    char *ep;
    uint32_t cv = strtol(val,&ep,16);
    if (ep && *ep) return false;
    *r = ((cv && 0x00FF0000) >> 16);
    *g = ((cv && 0x0000FF00) >> 8);
    *b = (cv && 0x000000FF);
  }
  char *p = val;
  char *ep;
  char *start = p;
  while (*p && (*p != ',')) p++;
  if (!*p) return false;
  char backup = *p;
  *p = 0;
  int t = strtol(start,&ep,0);
  if ((ep && *ep) || (t < 0) || (t > 255)) { *p = backup; return false; }
  *p = backup;
  *r = t;
  p++;
  start = p;
  while (*p && (*p != ',')) p++;
  if (!*p) return false;
  backup = *p;
  *p = 0;
  t = strtol(start,&ep,0);
  if ((ep && *ep) || (t < 0) || (t > 255)) { *p = backup; return false; }
  *p = backup;
  *g = t;
  p++;
  t = strtol(p,&ep,0);
  if ((ep && *ep) || (t < 0) || (t > 255)) return false;
  *b = t;  
  return true;  
}

bool parametric::setparam(param_t *p, char *val) {
  if (!p) return false;
  if (p->type == PARAM_STRING) {
    // Direct use
    if (p->d.s.val) {
      free(p->d.s.val);
      p->d.s.val = NULL;
    }
    if (val && *val) {
      p->d.s.val = (char*)malloc(strlen(val)+1);
      if (!p->d.s.val) return false;
      strcpy(p->d.s.val,val);
    }
    return true;
  }
  // Generic -- must convert
  if (!val) return false;
  if (!*val) return false;
  switch (p->type) {
    case PARAM_BOOL:
      if ((strcasecmp(val,"0")==0) || (strcasecmp(val,"off")==0) ||
          (strcasecmp(val,"no")==0) || (strcasecmp(val,"false")==0)) {
        p->d.b.val = false;
        return true;
      }
      if ((strcasecmp(val,"1")==0) || (strcasecmp(val,"on")==0) || 
          (strcasecmp(val,"yes")==0) || (strcasecmp(val,"true")==0)) {
        p->d.b.val = true;
        return true;
      }
      return false;
    case PARAM_INT:
      char *ep;
      int i;
      i=strtol(val,&ep,0);
      if (ep && *ep) return false;
      if ((i < p->d.i.minval) || (i > p->d.i.maxval)) return false;
      p->d.i.val = i;
      return true;
    case PARAM_COL:
      uint8_t r, g, b;
      if (decodestringcol(val, &r, &g, &b)) {
        p->d.c.r = r; 
        p->d.c.g = g;
        p->d.c.b = b;
        return true;
      }
      return false;
  }
  return false;
}

bool parametric::formatparam(param_t *p, char *buffer, int buflen) {
  if (!p) return false;
  if (!buffer) return false;
  if (buflen < 1) return false;
  switch (p->type) {
    case PARAM_BOOL:
      if (buflen < 4) return false;
      if (p->d.b.val) 
        strcpy(buffer,"on");
      else
        strcpy(buffer,"off");
      return true;
    case PARAM_INT:
      if (buflen < 10) return false;
      sprintf(buffer,"%i",p->d.i.val);
      return true;
    case PARAM_COL:
      if (buflen < 12) return false;
      sprintf(buffer,"%i,%i,%i",p->d.c.r, p->d.c.g, p->d.c.b);
      return true;
    case PARAM_STRING:
      if (!p->d.s.val) {
        buffer[0]=0;
        return true;
      }
      if (buflen < strlen(p->d.s.val)+1) return false;
      strcpy(buffer,p->d.s.val);
      return true;
  }
  return false;
}

bool parametric::resetparam(param_t *p) {
  if (!p) return false;
  switch (p->type) {
    case PARAM_BOOL:
      p->d.b.val = p->d.b.defaultval;
      return true;
    case PARAM_INT:
      p->d.i.val = p->d.i.defaultval;
      return true;
    case PARAM_COL:
      p->d.c.r = p->d.c.rdefault;  
      p->d.c.g = p->d.c.gdefault;  
      p->d.c.b = p->d.c.bdefault;  
      return true;
    case PARAM_STRING:
      if (p->d.s.val) {
        free(p->d.s.val);
        p->d.s.val = NULL;
      }
      if (p->d.s.defaultval) {
        p->d.s.val = (char *)malloc(strlen(p->d.s.defaultval)+1);
        if (!p->d.s.val) 
          return false;
        strcpy(p->d.s.val, p->d.s.defaultval);
      }
      return true;
  }
  return false;
}

bool parametric::resetparams() {
  param_t *p = params;
  while (p) {
    if (!resetparam(p)) return false;
    p=p->next;
  }
  return true;
}

void parametric::updateparams() {
  // Nothing to do  
}

void parametric::updateparam(param_t *param) {
  // Nothing to do
}

void parametric::tick() {
  // Nothing to do
}
