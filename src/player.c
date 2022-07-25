#include "player.h"
#include "world.h"

#include "state.h"

#include "block.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float absf(float x) {
    return (x < 0 ? -x : x);
}

player_t* player_init(world_t *world, vec3f position, vec3f rotation) {
    player_t *temp = (player_t*)malloc(sizeof(player_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate memory for player.");
        return NULL;
    }

    temp->position = position;
    temp->rotation = rotation;

    temp->velocity = F2VEC3F(0, 0, 0);

    temp->facing = F2VEC3F(0, 0, -1);

    temp->camera = camera_init(
            CAMERA_PROJECTION_PERSPECTIVE,
            (float)state.window_width / state.window_height,
            60.f
    );

    temp->camera->offset = F2VEC3F(0, 0, 1);

    temp->current_block = BLOCK_GRASS;

    temp->moving.forward = 0;
    temp->moving.right = 0;
    temp->moving.up = 0;

    temp->world = world;

    return temp;
}

void player_destroy(player_t *p) {
    if(!p) return;

    camera_destroy(p->camera);

    free(p);
}

void player_tick(player_t *p) {
    if(p->rotation.x > 90.f) p->rotation.x = 90.f;
    else if(p->rotation.x < -90.f) p->rotation.x = -90.f;

    if(p->rotation.y > 360.f) p->rotation.y -= 360.f;
    else if(p->rotation.y < 0.f) p->rotation.y += 360.f;

    p->facing = F2VEC3F(
        -sinf(RADIANS(p->rotation.y)) * cosf(RADIANS(p->rotation.x)),
        sinf(RADIANS(p->rotation.x)),
        -cosf(RADIANS(p->rotation.y)) * cosf(RADIANS(p->rotation.x))
    );

    // Negative sin and cos where Y rotation
    // because the camera is looking at -z by default
    // (Y rotation responsible for the horizontal and depth movement)
    //if(free_cam) world->player->velocity.y = camera->facing.y * SPEED * move;
    vec3f v = {
        p->velocity.z * -sinf(RADIANS(p->rotation.y)) + p->velocity.x * cosf(RADIANS(p->rotation.y)),
        0.f,
        p->velocity.z * -cosf(RADIANS(p->rotation.y)) + p->velocity.x * -sinf(RADIANS(p->rotation.y))
    };

    v = vec3f_normalize(v);

    v.x *= 0.1f;
    v.z *= 0.1f;

    const float size_w = 0.3f;
    vec3f dir = {
        v.x >= 0 ? 1 : -1,
        0,
        v.z >= 0 ? 1 : -1
    };

    p->position.x += v.x;
    if(
        world_get_block(p->world, floorf(p->position.x + size_w * dir.x), p->position.y + 1.5f, ceilf(p->position.z + size_w)) || 
        world_get_block(p->world, floorf(p->position.x + size_w * dir.x), p->position.y + 1.5f, ceilf(p->position.z - size_w)) ||
        world_get_block(p->world, floorf(p->position.x + size_w * dir.x), p->position.y + 1.0f, ceilf(p->position.z + size_w)) || 
        world_get_block(p->world, floorf(p->position.x + size_w * dir.x), p->position.y + 1.0f, ceilf(p->position.z - size_w)) ||
        world_get_block(p->world, floorf(p->position.x + size_w * dir.x), p->position.y, ceilf(p->position.z + size_w)) || 
        world_get_block(p->world, floorf(p->position.x + size_w * dir.x), p->position.y, ceilf(p->position.z - size_w))
    ) {
        p->position.x -= v.x;
    }

    p->position.z += v.z;
    if(
        world_get_block(p->world, floorf(p->position.x + size_w), p->position.y + 1.5f, ceilf(p->position.z + size_w * dir.z)) || 
        world_get_block(p->world, floorf(p->position.x - size_w), p->position.y + 1.5f, ceilf(p->position.z + size_w * dir.z)) ||
        world_get_block(p->world, floorf(p->position.x + size_w), p->position.y + 1.0f, ceilf(p->position.z + size_w * dir.z)) || 
        world_get_block(p->world, floorf(p->position.x - size_w), p->position.y + 1.0f, ceilf(p->position.z + size_w * dir.z)) ||
        world_get_block(p->world, floorf(p->position.x + size_w), p->position.y, ceilf(p->position.z + size_w * dir.z)) || 
        world_get_block(p->world, floorf(p->position.x - size_w), p->position.y, ceilf(p->position.z + size_w * dir.z))
    ) {
        p->position.z -= v.z;
    }

    p->position.y += v.y;
    vec3i check = {
        .y = p->position.y
    };
    bool_e fall = TRUE;

    if(world_get_block(p->world, floorf(p->position.x + size_w), check.y, ceilf(p->position.z + size_w))) {
        fall = FALSE;
    } else if(world_get_block(p->world, floorf(p->position.x - size_w), check.y, ceilf(p->position.z + size_w))) {
        fall = FALSE;
    } else if(world_get_block(p->world, floorf(p->position.x + size_w), check.y, ceilf(p->position.z - size_w))) {
        fall = FALSE;
    } else if(world_get_block(p->world, floorf(p->position.x - size_w), check.y, ceilf(p->position.z - size_w))) {
        fall = FALSE;
    }

    if(!fall) {
        p->position.y = roundf(p->position.y);
        v.y = 0;
    }

    check.y = p->position.y + 1.8f;

    if(world_get_block(p->world, floorf(p->position.x + size_w), check.y, ceilf(p->position.z + size_w))) {
        p->position.y -= v.y;
        p->moving.up = 0;
    } else if(world_get_block(p->world, floorf(p->position.x - size_w), check.y, ceilf(p->position.z + size_w))) {
        p->position.y -= v.y;
        p->moving.up = 0;
    } else if(world_get_block(p->world, floorf(p->position.x + size_w), check.y, ceilf(p->position.z - size_w))) {
        p->position.y -= v.y;
        p->moving.up = 0;
    } else if(world_get_block(p->world, floorf(p->position.x - size_w), check.y, ceilf(p->position.z - size_w))) {
        p->position.y -= v.y;
        p->moving.up = 0;
    }

    p->camera->position = p->position;
    p->camera->position.y += 1.5f;
    p->camera->rotation = p->rotation;

    camera_update(p->camera);

    get_block_with_ray(p->world, p->camera->position, p->camera->facing, &p->ray);
}
