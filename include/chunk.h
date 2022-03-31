#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <stdint.h>
#include "glx/vbo.h"
#include "glx/vao.h"
#include "glx/shader.h"

#include "math/vec.h"
#include "constants.h"

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 16

typedef struct _chunk_s {
    int x, z; // position in chunks

    // ONE VERTEX DATA
    // x y z u v layer
    float *vertices;
    int vertex_count;

    int *indices;
    int index_count;

    int mesh_counter;

    vbo_t vbo, vio;
    vao_t vao;

    uint8_t *data;
} chunk_t;

chunk_t *initialize_chunk(int x, int z);
void prepare_chunk(chunk_t *p);
void chunk_render(chunk_t *p, shader_t *s);
void free_chunk(chunk_t *p);

uint8_t get_chunk_block(const chunk_t *p, int x, int y, int z);
void set_chunk_block(chunk_t *p, int x, int y, int z, uint8_t type);

const char* direction_name(direction_e d);
vec3f direction_to_vec3f(direction_e d);

#endif