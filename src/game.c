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
#include "game.h"
#include <stdio.h>

void _update_map(level *map, int new_x, int new_y) {
    if (map->player_on_beacon) {
        map->data[map->player_y][map->player_x] = LEVEL_BEACON;
        map->player_on_beacon = 0;
    }
    else {
        map->data[map->player_y][map->player_x] = LEVEL_TERRAIN;
    }

    map->player_x = new_x;
    map->player_y = new_y;

    if (map->data[new_y][new_x] == LEVEL_BEACON) {
        map->player_on_beacon = 1;
    }

    map->data[new_y][new_x] = LEVEL_SOKOBAN;
}

void play(level *map, int dx, int dy) {
    int new_player_x = map->player_x + dx;
    int new_player_y = map->player_y + dy;
    int box_x, box_y, beacon_toggle = 0;

    printf("(%d, %d)\n", dx, dy);

    if (new_player_x < 0 || new_player_y < 0 || new_player_x >= map->width || new_player_y >= map->height) return;

    switch (map->data[new_player_y][new_player_x]) {
        case LEVEL_KEY:
            map->key_count++;
            _update_map(map, new_player_x, new_player_y);
            printf("Found a key!\n");
            break;
        case LEVEL_DOOR:
            if (map->key_count > 0) {
                map->key_count--;
                _update_map(map, new_player_x, new_player_y);
            }
            break;
        case LEVEL_BEACON:
        case LEVEL_TERRAIN:
            _update_map(map, new_player_x, new_player_y);
            break;
        case LEVEL_B_BEACON:
            beacon_toggle = 1;
        case LEVEL_BOX:
            box_x = new_player_x + dx;
            box_y = new_player_y + dy;
            if (box_x >= 0 && box_y >= 0 && box_x < map->width && box_y < map->height) {
                switch (map->data[box_y][box_x]) {
                    case LEVEL_BEACON:
                        map->data[box_y][box_x] = LEVEL_B_BEACON;
                        _update_map(map, new_player_x, new_player_y);
                        if (beacon_toggle) map->player_on_beacon = 1;
                        break;
                    case LEVEL_TERRAIN:
                        map->data[box_y][box_x] = LEVEL_BOX;
                        _update_map(map, new_player_x, new_player_y);
                        if (beacon_toggle) map->player_on_beacon = 1;
                        break;
                }
            }
            break;
    }
}

int check_win(level *map) {
    int i, j, beacon = 0;

    for (i = 0; i < map->width; i++) {
        for (j = 0; j < map->height; j++) {
            if (map->data[j][i] == LEVEL_BEACON) {
                beacon = 1;
                break;
            }
        }
    }

    if (!beacon && !map->player_on_beacon) {
        return 1;
    }

    return 0;
}

