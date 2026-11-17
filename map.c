#define _CRT_SECURE_NO_WARNINGS
#include "map.h"
#include "player.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_SIZE 5
#define MAX_SIZE 30
#define WALL     '#'
#define EMPTY    '.'
#define TREE     'T'
#define ROCK     'R'
#define HERO     '@'
#define CAVE_IN  'O'
#define CAVE_OUT 'X'

#define STEP_STAM        1
#define MAP_CHANGED_STAM 5
#define MAP_CHANGED_GOLD 10

static const char* weather_names[] = { "Ясно", "Ливень", "Туман", "Ураган" };

const char* weather_name(WeatherType t) { return weather_names[t]; }

Weather weather_generate(void) {
    Weather w;
    w.type = (WeatherType)(rand() % 4);

    switch (w.type) {
    case WEATHER_CLEAR:
        w.temperature = 15 + rand() % 16;
        w.wind_power = rand() % 4;
        break;
    case WEATHER_RAIN:
        w.temperature = 5 + rand() % 11;
        w.wind_power = 3 + rand() % 5;
        break;
    case WEATHER_FOG:
        w.temperature = 0 + rand() % 11;
        w.wind_power = rand() % 3;
        break;
    case WEATHER_HURRICANE:
    default:
        w.temperature = -5 + rand() % 11;
        w.wind_power = 7 + rand() % 4;
        break;
    }
    return w;
}

Map* map_create(void) {
    Map* m = (Map*)malloc(sizeof(Map));
    if (!m) return NULL;
    m->grid = NULL;
    m->width = 0;
    m->height = 0;
    m->type = MAP_FOREST;
    m->level = 1;
    return m;
}

static void free_grid(Map* m) {
    if (!m->grid) return;
    for (int i = 0; i < m->height; i++) free(m->grid[i]);
    free(m->grid);
    m->grid = NULL;
    m->width = 0;
    m->height = 0;
}

void map_destroy(Map* m) {
    if (!m) return;
    free_grid(m);
    debug_free(m);
}

static int alloc_grid(Map* m, int w, int h) {
    m->width = w;
    m->height = h;
    m->grid = (char**)malloc((size_t)h * sizeof(char*));
    if (!m->grid) return 0;

    for (int i = 0; i < h; i++) {
        m->grid[i] = (char*)malloc((size_t)w);
        if (!m->grid[i]) {
            for (int j = 0; j < i; j++) free(m->grid[j]);
            free(m->grid);
            m->grid = NULL;
            m->width = m->height = 0;
            return 0;
        }
        memset(m->grid[i], EMPTY, (size_t)w);
    }
    return 1;
}

static int is_inside(const Map* m, int x, int y) {
    return x > 0 && x < m->width - 1 && y > 0 && y < m->height - 1;
}

static int place_hero(Map* m, Player* h) {
    if (!h) return 0;
    for (int t = 0; t < 1000; t++) {
        int x = 1 + rand() % (m->width - 2);
        int y = 1 + rand() % (m->height - 2);
        if (m->grid[y][x] == EMPTY) {
            h->x = x;
            h->y = y;
            m->grid[y][x] = HERO;
            return 1;
        }
    }
    return 0;
}

static int place_symbol(Map* m, char sym) {
    for (int t = 0; t < 1000; t++) {
        int x = 1 + rand() % (m->width - 2);
        int y = 1 + rand() % (m->height - 2);
        if (m->grid[y][x] == EMPTY) {
            m->grid[y][x] = sym;
            return 1;
        }
    }
    return 0;
}

int map_generate_forest(Map* m, Player* h) {
    free_grid(m);

    int w = MIN_SIZE + rand() % (MAX_SIZE - MIN_SIZE + 1);
    int hh = MIN_SIZE + rand() % (MAX_SIZE - MIN_SIZE + 1);
    if (!alloc_grid(m, w, hh)) return 0;

    m->type = MAP_FOREST;

    int obs = 15 + rand() % 16;
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            if (x == 0 || y == 0 || x == m->width - 1 || y == m->height - 1)
                m->grid[y][x] = WALL;
            else if (rand() % 100 < obs)
                m->grid[y][x] = (rand() % 2) ? TREE : ROCK;
            else
                m->grid[y][x] = EMPTY;
        }
    }

    if (!place_hero(m, h)) {
        if (h) { h->x = 1; h->y = 1; }
        m->grid[1][1] = HERO;
    }
    place_symbol(m, CAVE_IN);
    return 1;
}

int map_generate_cave(Map* m, Player* h) {
    free_grid(m);

    int w = 5 + rand() % 4;
    int hh = 5 + rand() % 4;
    if (!alloc_grid(m, w, hh)) return 0;

    m->type = MAP_CAVE;

    int obs = 20 + rand() % 21;
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            if (x == 0 || y == 0 || x == m->width - 1 || y == m->height - 1)
                m->grid[y][x] = WALL;
            else if (rand() % 100 < obs)
                m->grid[y][x] = (rand() % 2) ? TREE : ROCK;
            else
                m->grid[y][x] = EMPTY;
        }
    }

    if (!place_hero(m, h)) {
        if (h) { h->x = 1; h->y = 1; }
        m->grid[1][1] = HERO;
    }
    place_symbol(m, CAVE_OUT);
    return 1;
}

void map_print(const Map* m) {
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) putchar(m->grid[y][x]);
        putchar('\n');
    }
}

static int hurricane_blocks_transition(Player* h, Weather* w) {
    if (w->type != WEATHER_HURRICANE) return 0;
    if ((rand() % 100) >= 20) return 0;   /* 80% — пронесло */

    h->cur_stam -= MAP_CHANGED_STAM;
    printf("Ураган! Герой не смог перейти — ветер отбросил его назад.\n");
    return 1;
}

static MoveResult do_transition(Map* m, Player* h, int* day, int* hour,
    Weather* w, int to_cave) {
    (void)day;

    int cost = MAP_CHANGED_STAM;
    if (w->type == WEATHER_RAIN) {
        cost *= 2;
        printf("Ливень: переход стоит вдвое дороже.\n", cost);
    }

    if (hurricane_blocks_transition(h, w)) {
        if (h->cur_stam <= 0) {
            printf("Выносливость на нуле — герой падает без сил!\n");
            return MOVE_OUT_OF_STAM;
        }
        return MOVE_BLOCKED;
    }

    h->gold += MAP_CHANGED_GOLD * m->level;
    h->cur_stam -= cost;
    (*hour) += 1;
    m->level += 1;

    int ok = to_cave ? map_generate_cave(m, h) : map_generate_forest(m, h);
    if (!ok) {
        fprintf(stderr, "Ошибка генерации карты.\n");
        exit(1);
    }

    *w = weather_generate();
    printf("%s %dx%d. Новая погода: %s, %d°C, ветер %d.\n",
        to_cave ? "Пещера" : "Лес",
        m->width, m->height,
        weather_name(w->type), w->temperature, w->wind_power);

    if (h->cur_stam <= 0) {
        printf("Выносливость на нуле — герой падает без сил!\n");
        return MOVE_OUT_OF_STAM;
    }
    if (*hour == 2) {
        printf("Наступило 02:00 — герой падает без сил!\n");
        return MOVE_OUT_OF_STAM;
    }
    return MOVE_MAP_CHANGED;
}

MoveResult map_try_move(Map* m, Player* h, int* day, int* hour,
    Weather* w, int dx, int dy) {
    int nx = h->x + dx;
    int ny = h->y + dy;

    if (!is_inside(m, nx, ny)) {
        printf("Герой упёрся в стену!\n");
        return MOVE_BLOCKED;
    }

    char target = m->grid[ny][nx];
    if (target == WALL || target == TREE || target == ROCK) {
        printf("Путь заблокирован (%c).\n", target);
        return MOVE_BLOCKED;
    }

    if (w->type == WEATHER_HURRICANE && (rand() % 100) < 20) {
        int dirs[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
        int start = rand() % 4;
        int blown = 0;

        for (int k = 0; k < 4; k++) {
            int bx = h->x + dirs[(start + k) % 4][0];
            int by = h->y + dirs[(start + k) % 4][1];
            if (!is_inside(m, bx, by)) continue;
            if (m->grid[by][bx] != EMPTY) continue;

            m->grid[h->y][h->x] = EMPTY;
            h->x = bx;
            h->y = by;
            m->grid[by][bx] = HERO;
            blown = 1;
            break;
        }

        h->cur_stam -= STEP_STAM;
        if (blown)
            printf("Ураган! Героя сдуло на (%d, %d).\n", h->x, h->y);
        else
            printf("Ураган! Герой не удержался на ногах, но вокруг всё занято.\n");

        if (h->cur_stam <= 0) {
            printf("Выносливость на нуле — герой падает без сил!\n");
            return MOVE_OUT_OF_STAM;
        }
        return MOVE_OK;
    }

    if (target == CAVE_IN) {
        printf("Вы спускаетесь в пещеру...\n");
        return do_transition(m, h, day, hour, w, /*to_cave=*/1);
    }
    if (target == CAVE_OUT) {
        printf("Вы выбираетесь на поверхность...\n");
        return do_transition(m, h, day, hour, w, /*to_cave=*/0);
    }

    int cost = STEP_STAM;
    if (w->type == WEATHER_RAIN) {
        cost *= 2;
        printf("Ливень: шаг стоит вдвое дороже.\n", cost);
    }

    m->grid[h->y][h->x] = EMPTY;
    h->x = nx;
    h->y = ny;
    h->cur_stam -= cost;
    m->grid[ny][nx] = HERO;

    printf("Герой шагнул на (%d, %d).\n", nx, ny);

    if (h->cur_stam <= 0) {
        printf("Выносливость на нуле — герой падает без сил!\n");
        return MOVE_OUT_OF_STAM;
    }
    return MOVE_OK;
}