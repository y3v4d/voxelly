#include "utils/img_loader.h"

#include <stdio.h>
#include <stdlib.h>

bmp_t* load_bmp(const char *path, int use_alpha, int reverse) {
    FILE *f = fopen(path, "rb");
    if(!f) {
        fprintf(stderr, "Couldn't open %s\n", path);
        return NULL;
    }

    bmp_t *temp = (bmp_t*)malloc(sizeof(bmp_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate bmp structure\n");
        return NULL;
    }

    byte_t buffer[16];
    fread(buffer, sizeof(byte_t) * 1, 2, f);

    printf("%c %c\n", buffer[0], buffer[1]);

    fseek(f, 8, SEEK_CUR);

    fread(buffer, sizeof(byte_t) * 4, 1, f);

    unsigned offset = *((int*)buffer);
    printf("offset: %d\n", offset);

    fseek(f, 4, SEEK_CUR);
    fread(buffer, sizeof(byte_t) * 4, 2, f);
   
    unsigned int w = ((int*)buffer)[0];
    unsigned int h = ((int*)buffer)[1];

    temp->width = w;
    temp->height = h;

    printf("w: %d h: %d\n", w, h);

    fseek(f, 2, SEEK_CUR);
    fread(buffer, sizeof(byte_t) * 2, 1, f);

    printf("BitsPerPixel: %d\n", *((short*)buffer));
    const unsigned BPP = *((short*)buffer) / 8;

    fseek(f, 4, SEEK_CUR);
    fread(buffer, sizeof(byte_t) * 4, 1, f);

    unsigned int size = *((int*)buffer);
    printf("Image size: %d\n", *((int*)buffer));

    temp->data = (byte_t*)malloc(sizeof(byte_t) * size);

    fseek(f, offset, SEEK_SET);

    fread(temp->data, sizeof(byte_t) * (use_alpha == 1 ? 4 : 3), w * h, f);

    fclose(f);

    if(reverse) {
        for(int y = 0; y < temp->height / 2; ++y) {
            byte_t *first = temp->data + (y * temp->width) * BPP;
            byte_t *last = temp->data + (temp->height - 1 - y) * temp->width * BPP;

            for(int x = 0; x < temp->width * BPP; ++x) {
                byte_t temp = first[x];
                first[x] = last[x];
                last[x] = temp;
            }
        }
    }

    return temp;
}

void free_bmp(bmp_t *p) {
    if(!p) return;

    if(p->data) free(p->data);
    free(p);
}
