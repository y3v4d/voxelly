#ifndef __FONT_LOADER_H__
#define __FONT_LOADER_H__

#include "glx/texture.h"

typedef struct {
    unsigned ID;

    unsigned x, y;
    unsigned w, h;

    unsigned xoff, yoff;
    unsigned xadv;
} charbmp_t;

typedef struct {
    charbmp_t *chars;
    unsigned count;

    texture_t *texture;
} fontbmp_t;

fontbmp_t* fontbmp_make(const char *path);
void fontbmp_close(fontbmp_t *p);

#endif