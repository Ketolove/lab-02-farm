#define _CRT_SECURE_NO_WARNINGS
#include "player.h"
#include "map.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <locale.h>
#endif

typedef struct {
    Player* hero;
    int     day;
    int     hour;
    Weather weather;
} Game;

static void print_hud(const Game* g, const Map* m) {
    if (!g || !g->hero) return;

    printf("HP: %d/%d | Стамина: %d/%d | Золото: %d",
        g->hero->cur_hp, g->hero->max_hp,
        g->hero->cur_stam, g->hero->max_stam,
        g->hero->gold);

    if (m) printf(" | Этаж: %d", m->level);

    printf(" | Время: День %d, %02d:00 | Погода: %s, %d°C, ветер %d\n",
        g->day, g->hour,
        weather_name(g->weather.type),
        g->weather.temperature,
        g->weather.wind_power);
}

static void print_main_menu(const Player* h) {
    printf("\nГерой: %s\n", h ? h->name : "не создан");
    printf(
        "0  - Выход\n"
        "1  - Создать / пересоздать персонажа\n"
        "2  - Карточка персонажа\n"
        "3  - Время и отдых (подменю)\n"
        "4  - Посмотреть инвентарь\n"
        "5  - Положить предмет в слот\n"
        "6  - Выбросить предмет\n"
        "7  - Уникальные находки\n"
        "8  - Поиск предмета по названию\n"
        "9  - Записать в дневник\n"
        "10 - Расшифровать старые записи\n"
        "11 - Отправиться в приключение\n");
}

static void print_time_menu(void) {
    printf("\n--- Время и отдых ---\n"
        "0 - Назад\n"
        "1 - Посмотреть на часы\n"
        "2 - Пропустить время (Ждать)\n"
        "3 - Отдых (Лечь спать)\n");
}

static void recreate_hero(Game* g) {
    if (g->hero) {
        printf("Персонаж уже существует (адрес: %p). Удалить и создать нового? (y/n): ",
            (void*)g->hero);
        char ans;
        if (scanf(" %c", &ans) != 1) { clear_input(); return; }
        clear_input();
        if (ans != 'y' && ans != 'Y') {
            printf("Отменено.\n");
            return;
        }
        player_destroy(g->hero);
        g->hero = NULL;
    }

    g->hero = player_create();
    if (!g->hero) {
        printf("Не удалось создать персонажа.\n");
        return;
    }

    g->day = 1;
    g->hour = 8;
    g->weather = weather_generate();
    printf("Погода на старте: %s, %d°C, ветер %d.\n",
        weather_name(g->weather.type),
        g->weather.temperature,
        g->weather.wind_power);
}

static void do_adventure(Game* g) {
    if (!g->hero) {
        printf("Сначала создайте персонажа.\n");
        return;
    }
    if (!player_is_ready(g->hero, g->hour)) {
        printf("Персонаж не готов к приключениям!\n");
        return;
    }

    Map* m = map_create();
    if (!m) {
        printf("Ошибка выделения памяти под карту.\n");
        return;
    }

    if (!map_generate_forest(m, g->hero)) {
        fprintf(stderr, "Ошибка генерации карты.\n");
        map_destroy(m);
        return;
    }

    printf("Сгенерирован лес %dx%d\n", m->width, m->height);
    print_hud(g, m);
    map_print(m);

    int running = 1;
    while (running) {
        printf("Направление (W/A/S/D, Q - выйти): ");
        char cmd;
        if (scanf(" %c", &cmd) != 1) {
            clear_input();
            break;
        }

        int dx = 0, dy = 0;
        switch (cmd) {
        case 'W': case 'w': dy = -1; break;
        case 'S': case 's': dy = 1; break;
        case 'A': case 'a': dx = -1; break;
        case 'D': case 'd': dx = 1; break;
        case 'Q': case 'q':
            printf("Герой отправился домой.\n");
            running = 0;
            continue;
        default:
            printf("Неизвестная команда.\n");
            continue;
        }

        MoveResult r = map_try_move(m, g->hero, &g->day, &g->hour,
            &g->weather, dx, dy);

        putchar('\n');
        print_hud(g, m);
        map_print(m);

        switch (r) {
        case MOVE_OUT_OF_STAM:
            printf("Обморок! Спасатели доставили героя домой, забрав половину золота.\n");
            g->hero->gold /= 2;
            running = 0;
            break;
        case MOVE_MAP_CHANGED:
            printf(m->type == MAP_CAVE ? "[Вы в пещере]\n" : "[Вы в лесу]\n");
            break;
        case MOVE_OK:
        case MOVE_BLOCKED:
        default:
            break;
        }
    }

    map_destroy(m);
}

static void time_submenu(Game* g) {
    if (!g->hero) {
        printf("Сначала создайте персонажа.\n");
        return;
    }

    int in_time = 1;
    while (in_time) {
        print_time_menu();
        int choice;
        if (!read_int(&choice) || choice < 0 || choice > 3) {
            printf("Ошибка ввода.\n");
            continue;
        }

        switch (choice) {
        case 0:
            in_time = 0;
            break;

        case 1:
            print_hud(g, NULL);
            break;

        case 2: {
            int max_wait = hours_until_2am(g->hour);
            printf("Сейчас: День %d, %02d:00. До ближайших 02:00 можно ждать не больше %d ч.\n",
                g->day, g->hour, max_wait);

            int h;
            printf("Сколько часов ждать? ");
            if (!read_int(&h) || h < 1 || h > max_wait) {
                printf("Ночью надо спать!\n");
                break;
            }
            advance_hours(&g->day, &g->hour, h);
            printf("Прошло %d ч. Теперь: День %d, %02d:00\n",
                h, g->day, g->hour);
            print_hud(g, NULL);
            break;
        }

        case 3:
            player_sleep(g->hero);
            g->day += 1;
            g->hour = 8;
            g->weather = weather_generate();
            printf("Новый день. Погода: %s, %d°C, ветер %d.\n",
                weather_name(g->weather.type),
                g->weather.temperature,
                g->weather.wind_power);
            printf("Вы проспали до утра. День %d, %02d:00\n", g->day, g->hour);
            print_hud(g, NULL);
            break;
        }
    }
}

static void write_diary(Game* g) {
    printf("Введите запись: ");
    char note[256];
    clear_input();
    if (!fgets(note, sizeof note, stdin)) return;
    note[strcspn(note, "\n")] = '\0';

    utils_write_diary(g->hero->name,
        g->day, g->hour,
        weather_name(g->weather.type),
        g->weather.temperature,
        g->weather.wind_power,
        g->hero,
        note);
}

int main(void) {
#ifdef _WIN32
    setlocale(LC_ALL, ".UTF-8");
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif
    srand((unsigned)time(NULL));

    Game g;
    g.hero = NULL;
    g.day = 1;
    g.hour = 8;
    g.weather = weather_generate();

    int running = 1;
    while (running) {
        print_main_menu(g.hero);

        int choice;
        if (!read_int(&choice) || choice < 0 || choice > 11) {
            printf("Ошибка ввода, попробуйте ещё раз!\n");
            continue;
        }

        switch (choice) {
        case 0:
            printf("Выход.\n");
            running = 0;
            break;

        case 1:
            recreate_hero(&g);
            break;

        case 2:
            if (!g.hero) { printf("Сначала создайте персонажа.\n"); break; }
            player_show_card(g.hero, g.day, g.hour, &g.weather);
            break;

        case 3:
            time_submenu(&g);
            break;

        case 4:
            if (!g.hero) { printf("Сначала создайте персонажа.\n"); break; }
            player_show_inventory(g.hero);
            break;

        case 5:
            if (!g.hero) { printf("Сначала создайте персонажа.\n"); break; }
            player_put_item(g.hero);
            break;

        case 6:
            if (!g.hero) { printf("Сначала создайте персонажа.\n"); break; }
            player_clean_slot(g.hero);
            break;

        case 7:
            if (!g.hero) { printf("Сначала создайте персонажа.\n"); break; }
            player_show_uniques(g.hero);
            break;

        case 8:
            if (!g.hero) { printf("Сначала создайте персонажа.\n"); break; }
            player_find_item(g.hero);
            break;

        case 9:
            if (!g.hero) { printf("Сначала создайте персонажа.\n"); break; }
            write_diary(&g);
            break;

        case 10:
            if (!g.hero) { printf("Сначала создайте персонажа.\n"); break; }
            utils_count_supplies();
            break;

        case 11:
            do_adventure(&g);
            break;
        }
    }

    if (g.hero) {
        player_destroy(g.hero);
        g.hero = NULL;
    }

    return 0;
}