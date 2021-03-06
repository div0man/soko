/*
This file is part of Soko.

Soko is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Soko is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Soko. If not, see <http://www.gnu.org/licenses/>.

*/
#include <stdio.h>
#include <stdlib.h>
#include "level.h"

#define _ASSERT_RET(x, y) do { \
    free_level(&x); \
    fclose(y); \
    return NULL; \
} while(0)

#define _ASSERT_NULL(x, y, z) do { \
    if (x == NULL) { \
        _ASSERT_RET(y, z); \
    } \
} while(0)

void free_level(struct level **map)
{
    int i;

    if (*map != NULL) {
        if ((*map)->data != NULL) {
            for (i = 0; i < (*map)->height; i++) {
                if ((*map)->data[i] != NULL) {
                    free((*map)->data[i]);
                }
            }

            free((*map)->data);
        }

        free(*map);
        *map = NULL;
    }
}

struct level *read_level(char *level_name)
{
    char buffer[512];
    FILE *t;
    int i, j, soko = 0, tmp, skip = 1;
    struct level *map;

    sprintf(buffer, "levels/%s.dat", level_name);

    t = fopen(buffer, "r");

    if (!t) {
        return NULL;
    }

    map = malloc(sizeof(struct level));

    _ASSERT_NULL(map, map, t);

    if (fscanf(t, "%d %d", &map->width, &map->height) != 2 || map->width <= 0 || map->height <= 0) {
        _ASSERT_RET(map, t);
    }

    map->data = malloc(sizeof(char *) * map->height);

    _ASSERT_NULL(map->data, map, t);

    for (i = 0; i < map->height; i++) {
        map->data[i] = malloc(sizeof(char) * map->width);

        _ASSERT_NULL(map->data[i], map, t);

        for (j = 0; j < map->width; j++) {
            /* file does not contain enough information */
            if (feof(t) || fscanf(t, "%d", &tmp) != 1) {
                fclose(t);
                free_level(&map);
                return NULL;
            }

            /* handle overflow or underflow */
            if (tmp > LEVEL_KEY || tmp < LEVEL_EMPTY) {
                tmp = LEVEL_WALL;
            }

            map->data[i][j] = (unsigned char)tmp;

            if (map->data[i][j] == LEVEL_SOKOBAN) {
                /* soko already specified elsewhere. treat other soko values as walls */
                if (soko == 1) {
                    map->data[i][j] = LEVEL_WALL;
                } else {
                    soko = 1;
                    map->soko.x = j;
                    map->soko.y = i;
                }
            }
        }
    }

    while ((tmp = fgetc(t)) != EOF) {
        if (skip && (tmp == '\r' || tmp == '\n' || tmp == ' ')) {
            continue;
        }
        skip = 0;
        putchar(tmp);
    }

    fclose(t);

    /* if there is no soko specified in map assign it to pos (0, 0) */
    if (!soko) {
        map->soko.x = map->soko.y = 0;
        map->data[0][0] = LEVEL_SOKOBAN;
    }

    map->soko.player_on_beacon = map->soko.key_count = 0;

    return map;
}
