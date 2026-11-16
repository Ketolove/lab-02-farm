
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#pragma warning(disable : 4996)
#include <windows.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define INV_LEN      10
#define MAX_ITEMS    10
#define MAX_NAME_LEN 32


char items[MAX_ITEMS][MAX_NAME_LEN] = {
    "0 Пусто", "1 Дерево", "2 Камень", "3 Семена", "4 Лейка",
    "5 Ведро", "6 Удобрение", "7 Корзина", "8 Грабли", "9 Лопата"
};

typedef enum {
    MAP_FOREST = 0,
    MAP_CAVE = 1,
} Map_types;



typedef enum {
    MOVE_BLOCKED,
    MOVE_OK,
    MOVE_MAP_CHANGED,
    MOVE_OUT_OF_STAM
} MoveResult;

typedef enum {
    STEP_STAM = 1,
    MAP_CHANGED_STAM = 5,
    MAP_CHANGED_GOLD = 10
} Delta;

typedef struct {
    int current_hour;
    int current_day;
} Timing;

#define MIN_SIZE 5
#define MAX_SIZE 30
#define WALL    '#'
#define EMPTY   '.'
#define TREE    'T'
#define ROCK    'R'
#define HERO    '@'

#define CAVE_IN   'O'
#define CAVE_OUT  'X'

typedef struct {
    char** grid;
    int width;
    int height;
    int type;
    int level;
} Map;

typedef struct Hero {
    char hero_name[MAX_NAME_LEN];
    int cur_hp;
    int max_hp;
    int cur_stam;
    int max_stam;
    int gold;
    int heroX;
    int heroY;
    char items[MAX_ITEMS][MAX_NAME_LEN];
} Hero;

Hero* hero = NULL;
Timing timing = { .current_day = 1, .current_hour = 8 };

void Your_Name(struct Hero* h) {
    int er_inp_flag = 1;
    while (er_inp_flag == 1) {
        printf("Как Вас зовут?\n");

        int res = scanf("%31s", h->hero_name);
        int c = getchar();

        if (res != 1) {
            printf("Ошибка ввода!\n");
            while ((c = getchar()) != '\n' && c != EOF) {}
        }
        else if (c != '\n' && c != EOF && c != ' ' && c != '\t') {
            printf("Ошибка ввода! Имя должно содержать не больше 32 символов!\n");
            while ((c = getchar()) != '\n' && c != EOF) {}
        }
        else {
            printf("Здравствуйте, %s!\n", h->hero_name);
            er_inp_flag = 0;
        }
    }
}


char item_names[MAX_ITEMS][MAX_NAME_LEN];
char** supplies_names = NULL;
int* supplies_totals = NULL;
int    supplies_count = 0;
int    item_count = 0;
int    inventory[INV_LEN] = { 0 };

void create_hero(void) {
    if (hero != NULL) {
        printf("Персонаж уже существует (адрес: %p). Удалить и создать нового? (y/n): ",
            (void*)hero);
        char ans;
        if (scanf(" %c", &ans) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            return;
        }
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        if (ans != 'y' && ans != 'Y') {
            printf("Отменено.\n");
            return;
        }
        printf("Освобождаю память по адресу %p\n", (void*)hero);
        free(hero);
        hero = NULL;
    }

    hero = (Hero*)malloc(sizeof(Hero));
    if (!hero) {
        printf("Ошибка выделения памяти.\n");
        return;
    }


    hero->cur_hp = 100;
    hero->max_hp = 100;
    hero->cur_stam = 50;
    hero->max_stam = 50;
    hero->gold = 0;
    hero->heroX = 0;
    hero->heroY = 0;
    hero->hero_name[0] = '\0';

    for (int i = 0; i < MAX_ITEMS; i++) {
        strncpy(hero->items[i], items[i], MAX_NAME_LEN - 1);
        hero->items[i][MAX_NAME_LEN - 1] = '\0';
    }

    Your_Name(hero);

    printf("Персонаж создан по адресу %p\n", (void*)hero);
}

void show_hero_card(Hero* h, Timing* t) {
    if (!h) {
        printf("Персонаж ещё не создан. Создайте его в пункте 1.\n");
        return;
    }
    printf("\n=== Карточка персонажа ===\n");
    printf("Имя:      %s\n", h->hero_name);
    printf("HP:       %d/%d\n", h->cur_hp, h->max_hp);
    printf("Стамина:  %d/%d\n", h->cur_stam, h->max_stam);
    printf("Золото:   %d\n", h->gold);
    printf("Время:    День %d, %02d:00\n", t->current_day, t->current_hour);
    printf("Инвентарь:\n");
    for (int i = 0; i < INV_LEN; i++) {
        int id = inventory[i];
        printf("  Слот %d: [%d] %s\n", i, id, h->items[id] + 2);
    }
}

void show_time(Timing* t) {
    printf("Текущее время: День %d, %02d:00\n", t->current_day, t->current_hour);
}

int hours_until_2am(Timing* t) {
    int now = t->current_hour;
    if (now < 2)   return 2 - now;
    if (now == 2)  return 24;
    return 24 - now + 2;
}

void wait(Timing* t) {
    int max_wait = hours_until_2am(t);
    printf("Сейчас: День %d, %02d:00. До ближайших 02:00 можно ждать не больше %d ч.\n",
        t->current_day, t->current_hour, max_wait);

    int hours;
    printf("Сколько часов ждать? ", max_wait);
    if (scanf_s("%d", &hours) != 1 || hours < 1 || hours > max_wait) {
        printf("Ночью надо спать!\n", max_wait);
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        return;
    }

    int total = t->current_hour + hours;
    t->current_day += total / 24;
    t->current_hour = total % 24;

    printf("Прошло %d ч. Теперь: День %d, %02d:00\n",
        hours, t->current_day, t->current_hour);
}

void sleep(Hero* h, Timing* t) {
    if (!h) { printf("Сначала создайте персонажа.\n"); return; }

    t->current_day += 1;
    t->current_hour = 8;

    h->cur_hp = h->max_hp;
    h->cur_stam = h->max_stam;

    printf("Вы проспали до утра. День %d, %02d:00\n", t->current_day, t->current_hour);
    printf("HP: %d/%d, Стамина: %d/%d\n",
        h->cur_hp, h->max_hp, h->cur_stam, h->max_stam);
}

void show_inventory(void) {
    for (int i = 0; i < INV_LEN; i++) {
        int id = inventory[i];
        printf("Слот %d: [%d] - %s\n", i, id, item_names[id] + 2);
    }
}

void put_item(void) {
    int num_slot, item_id;
    printf("Введите номер слота (от 0 до %d): ", INV_LEN - 1);
    if (scanf_s("%d", &num_slot) != 1) {
        printf("Ошибка ввода!\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        return;
    }
    printf("Введите ID предмета: ");
    if (scanf_s("%d", &item_id) != 1) {
        printf("Ошибка ввода!\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        return;
    }

    if (num_slot < 0 || num_slot >= INV_LEN) {
        printf("Ошибка: нет такого слота! (допустимо 0..%d)\n", INV_LEN - 1);
        return;
    }
    if (item_id < 0 || item_id >= MAX_ITEMS) {
        printf("Ошибка: нет такого ID! (допустимо 0..%d)\n", MAX_ITEMS - 1);
        return;
    }

    inventory[num_slot] = item_id;
    printf("В слот %d записан предмет [%d] (%s)\n",
        num_slot, item_id, item_names[item_id] + 2);
}

void clean_slot(void) {
    int num_slot_null;
    printf("Введите номер слота: ");
    if (scanf_s("%d", &num_slot_null) != 1) {
        printf("Ошибка ввода!\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        return;
    }
    if (num_slot_null < 0 || num_slot_null >= INV_LEN) {
        printf("Ошибка: нет такого слота! (допустимо 0..%d)\n", INV_LEN - 1);
        return;
    }
    inventory[num_slot_null] = 0;
    printf("Слот %d очищен\n", num_slot_null);
}

void show_uniques(void) {
    for (int i = 0; i < INV_LEN; i++) {
        int id = inventory[i];
        printf("Слот %d: [%d] - %s\n", i, id, item_names[id] + 2);
    }
    printf("\n");

    int any = 0;
    for (int id = 1; id < item_count; id++) {
        int cnt = 0;
        for (int slot = 0; slot < INV_LEN; slot++)
            if (inventory[slot] == id) cnt++;
        if (cnt == 0) continue;
        any = 1;
        if (cnt == 1)
            printf("ID %d (%s) - уникален\n", id, item_names[id] + 2);
        else
            printf("ID %d (%s) - не уникален, лежит в %d слотах\n",
                id, item_names[id] + 2, cnt);
    }

    if (!any) printf("В инвентаре нет предметов.\n");
}

void fill_defaults(void) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        strncpy(item_names[i], items[i], MAX_NAME_LEN - 1);
        item_names[i][MAX_NAME_LEN - 1] = '\0';
    }
    item_count = MAX_ITEMS;
}

void read_items(void) {
    FILE* fp = fopen("items.txt", "r");
    if (!fp) {
        printf("Ошибка: файл инвентаря не найден. Инвентарь будет заполнен по умолчанию.\n");
        fill_defaults();
        return;
    }


    int n = 0;
    item_count = n;
    if (item_count < MAX_ITEMS) {
        for (int i = item_count; i < MAX_ITEMS; i++) {
            strncpy(item_names[i], items[i], MAX_NAME_LEN - 1);
            item_names[i][MAX_NAME_LEN - 1] = '\0';
        }
        item_count = MAX_ITEMS;
    }
    fclose(fp);
}

void find_item(void) {
    char hero_find[MAX_NAME_LEN];

    printf("Что Вы ищете?\n");
    if (scanf("%31s", hero_find) != 1) {
        printf("Ошибка ввода!\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        return;
    }
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}

    int found_id = -1;
    for (int i = 0; i < item_count; i++) {
        if (strcmp(hero_find, item_names[i] + 2) == 0) {
            found_id = i;
            break;
        }
    }
    if (found_id == -1) {
        printf("Предмет \"%s\" не найден в каталоге.\n", hero_find);
        return;
    }

    int found_any = 0;
    for (int slot = 0; slot < INV_LEN; slot++) {
        if (inventory[slot] == found_id) {
            printf("Слот %d: [ID %d] (%s)\n",
                slot, inventory[slot], item_names[found_id] + 2);
            found_any = 1;
        }
    }
    if (!found_any)
        printf("Предмет \"%s\" (ID %d) есть в каталоге, но в инвентаре отсутствует.\n",
            hero_find, found_id);
}


void free_supplies(void) {
    if (supplies_names) {
        for (int i = 0; i < supplies_count; i++)
            free(supplies_names[i]);
        free(supplies_names);
        supplies_names = NULL;
    }
    free(supplies_totals);
    supplies_totals = NULL;
    supplies_count = 0;
}

void write_diary(struct Hero* h, Timing* t) {
    printf("Введите запись: ");
    char hero_routine[256];
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    if (!fgets(hero_routine, sizeof hero_routine, stdin)) return;
    hero_routine[strcspn(hero_routine, "\n")] = '\0';

    FILE* fp = fopen("diary.txt", "a");
    if (!fp) { printf("Ошибка открытия файла.\n"); return; }

    fprintf(fp, "Фермер: %s\nТекущее время: День %d, %02d:00\nСписок предметов в инвентаре:\n",
        h->hero_name, t->current_day, t->current_hour);
    for (int i = 0; i < 10; i++)
        fprintf(fp, "%s ", items[i] + 2);
    fprintf(fp, "\nЗаметка:\n%s\n", hero_routine);
    for (int i = 0; i < 68; i++) fprintf(fp, "-");
    fprintf(fp, "\n");
    fclose(fp);
    printf("Запись сохранена.\n");
}

void count_items(void) {
    free_supplies();

    FILE* fp = fopen("input.txt", "r");
    if (!fp) {
        printf("Ошибка: файл input.txt не найден.\n");
        return;
    }

    char buf[1024];
    while (fgets(buf, sizeof buf, fp)) {
        char name[MAX_NAME_LEN];
        int  cnt;
        if (sscanf(buf, "%31s %d", name, &cnt) != 2) continue;
        if (cnt <= 0) continue;

        int found = -1;
        for (int k = 0; k < supplies_count; k++) {
            if (strcmp(supplies_names[k], name) == 0) { found = k; break; }
        }

        if (found >= 0) {
            supplies_totals[found] += cnt;
        }
        else {
            char** tmp_names = realloc(supplies_names,
                (size_t)(supplies_count + 1) * sizeof * tmp_names);
            int* tmp_totals = realloc(supplies_totals,
                (size_t)(supplies_count + 1) * sizeof * tmp_totals);
            if (!tmp_names || !tmp_totals) {
                printf("Ошибка выделения памяти.\n");
                if (tmp_names)  supplies_names = tmp_names;
                if (tmp_totals) supplies_totals = tmp_totals;
                free_supplies();
                fclose(fp);
                return;
            }
            supplies_names = tmp_names;
            supplies_totals = tmp_totals;

            size_t len = strlen(name);
            supplies_names[supplies_count] = malloc(len + 1);
            if (!supplies_names[supplies_count]) {
                printf("Ошибка выделения памяти.\n");
                free_supplies();
                fclose(fp);
                return;
            }
            memcpy(supplies_names[supplies_count], name, len + 1);
            supplies_totals[supplies_count] = cnt;
            supplies_count++;
        }
    }
    fclose(fp);

    if (supplies_count == 0) {
        printf("В input.txt не найдено корректных поставок.\n");
        return;
    }

    FILE* out = fopen("output.txt", "w");
    if (!out) {
        printf("Ошибка открытия output.txt на запись.\n");
        return;
    }
    for (int i = 0; i < supplies_count; i++) {
        fprintf(out, "%s: %d шт.\n", supplies_names[i], supplies_totals[i]);
        printf("%s: %d шт.\n", supplies_names[i], supplies_totals[i]);
    }
    fclose(out);
    printf("Сводка записана в output.txt.\n");
}


int is_inside(const Map* m, int x, int y) {
    return x > 0 && x < m->width - 1 &&
        y > 0 && y < m->height - 1;
}

void free_map(Map* m, struct Hero* h) {
    if (!m->grid) return;
    for (int i = 0; i < m->height; i++) free(m->grid[i]);
    free(m->grid);
    m->grid = NULL;
    m->width = m->height = 0;
    h->heroX = h->heroY = 0;
}

int alloc_grid(Map* m, int width, int height) {
    m->width = width;
    m->height = height;
    m->grid = (char**)malloc(m->height * sizeof(char*));
    if (!m->grid) return 0;

    for (int i = 0; i < m->height; i++) {
        m->grid[i] = (char*)malloc(m->width * sizeof(char));
        if (!m->grid[i]) {
            for (int j = 0; j < i; j++) free(m->grid[j]);
            free(m->grid);
            m->grid = NULL;
            return 0;
        }
        memset(m->grid[i], EMPTY, m->width);
    }
    return 1;
}

int place_hero(Map* m, struct Hero* h) {
    for (int tries = 0; tries < 1000; tries++) {
        int x = 1 + rand() % (m->width - 2);
        int y = 1 + rand() % (m->height - 2);
        if (m->grid[y][x] == EMPTY) {
            h->heroX = x;
            h->heroY = y;
            m->grid[y][x] = HERO;
            return 1;
        }
    }
    return 0;
}

int place_symbol(Map* m, char symbol) {
    for (int tries = 0; tries < 1000; tries++) {
        int x = 1 + rand() % (m->width - 2);
        int y = 1 + rand() % (m->height - 2);
        if (m->grid[y][x] == EMPTY) {
            m->grid[y][x] = symbol;
            return 1;
        }
    }
    return 0;
}

int generate_map(Map* m, struct Hero* h) {
    int width = MIN_SIZE + rand() % (MAX_SIZE - MIN_SIZE + 1);
    int height = MIN_SIZE + rand() % (MAX_SIZE - MIN_SIZE + 1);

    if (!alloc_grid(m, width, height)) return 0;
    m->type = MAP_FOREST;

    int obstacle_percent = 15 + rand() % 16;

    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            if (x == 0 || y == 0 || x == m->width - 1 || y == m->height - 1) {
                m->grid[y][x] = WALL;
            }
            else {
                int r = rand() % 100;
                if (r < obstacle_percent)
                    m->grid[y][x] = (rand() % 2) ? TREE : ROCK;
                else
                    m->grid[y][x] = EMPTY;
            }
        }
    }

    if (!place_hero(m, h)) {
        h->heroX = 1; h->heroY = 1;
        m->grid[1][1] = HERO;
    }

    place_symbol(m, CAVE_IN);

    return 1;
}

int generate_cave(Map* m, struct Hero* h) {
    int width = 5 + rand() % 4;
    int height = 5 + rand() % 4;

    if (!alloc_grid(m, width, height)) return 0;
    m->type = MAP_CAVE;

    int obstacle_percent = 20 + rand() % 21;

    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            if (x == 0 || y == 0 || x == m->width - 1 || y == m->height - 1) {
                m->grid[y][x] = WALL;
            }
            else {
                int r = rand() % 100;
                if (r < obstacle_percent)
                    m->grid[y][x] = (rand() % 2) ? TREE : ROCK;
                else
                    m->grid[y][x] = EMPTY;
            }
        }
    }

    if (!place_hero(m, h)) {
        h->heroX = 1; h->heroY = 1;
        m->grid[1][1] = HERO;
    }

    place_symbol(m, CAVE_OUT);

    return 1;
}

void print_map(const Map* m) {
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++)
            putchar(m->grid[y][x]);
        putchar('\n');
    }
}

MoveResult try_move(Map* m, Hero* h, Timing* t, int dx, int dy) {
    int nx = h->heroX + dx;
    int ny = h->heroY + dy;

    if (!is_inside(m, nx, ny)) {
        printf("Герой упёрся в стену!\n");
        return MOVE_BLOCKED;
    }

    char target = m->grid[ny][nx];
    if (target == WALL || target == TREE || target == ROCK) {
        printf("Путь заблокирован (%c).\n", target);
        return MOVE_BLOCKED;
    }

    if (target == CAVE_IN) {
        printf("Вы спускаетесь в пещеру...\n");
        h->gold += MAP_CHANGED_GOLD * m->level;
        h->cur_stam -= MAP_CHANGED_STAM;
        t->current_hour += 1;
        m->level += 1;
        free_map(m, h);
        if (!generate_cave(m, h)) {
            fprintf(stderr, "Ошибка генерации пещеры.\n");
            exit(1);
        }
        printf("Пещера %dx%d:\n", m->width, m->height);
        return MOVE_MAP_CHANGED;
    }

    if (target == CAVE_OUT) {
        printf("Вы выбираетесь на поверхность...\n");
        h->gold += MAP_CHANGED_GOLD * m->level;
        h->cur_stam -= MAP_CHANGED_STAM;
        t->current_hour += 1;
        m->level += 1;
        free_map(m, h);
        if (!generate_map(m, h)) {
            fprintf(stderr, "Ошибка генерации леса.\n");
            exit(1);
        }
        printf("Лес %dx%d:\n", m->width, m->height);
        return MOVE_MAP_CHANGED;
    }

    m->grid[h->heroY][h->heroX] = EMPTY;
    h->heroX = nx;
    h->heroY = ny;
    h->cur_stam -= STEP_STAM;
    m->grid[ny][nx] = HERO;

    printf("Герой шагнул на (%d, %d).\n", nx, ny);

    if (h->cur_stam < 1) {
        printf("Выносливость на нуле — герой отправляется домой.\n");
        return MOVE_OUT_OF_STAM;
    }

    if (t->current_hour == 2) {
        printf("Наступило 02:00 — пора спать!\n");
        return MOVE_OUT_OF_STAM;
    }
    return MOVE_OK;
}

int is_ready(Hero* h, Timing* t) {
    return ((h) && (h->cur_stam > 0) && (t->current_hour < 2 || t->current_hour >= 8));
}

void print_hud(Hero* h, const Map* m, Timing* t) {
    if (!h) return;
    printf("HP: %d/%d | Выносливость: %d/%d | Золото: %d | Уровень: %d | Время: %02d:00\n",
        h->cur_hp, h->max_hp,
        h->cur_stam, h->max_stam,
        h->gold,
        m->level,
        t->current_hour);
}

void print_menu(Hero* h) {
    if (h)
        printf("\nГерой: %s\n", h->hero_name);
    else
        printf("\nГерой: не создан\n");

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

void print_menu_time() {
    printf("0 - Назад\n"
        "1 - Посмотреть на часы\n"
        "2 - Пропустить время (Ждать)\n"
        "3 - Отдых (Лечь спать)\n");
}

int main(void)
{
    setlocale(LC_ALL, ".UTF-8");
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    Map map = { 0 };
    map.level = 1;

    read_items();

    for (int i = 0; i < item_count; i++)
        printf("%s\n", item_names[i]);

    int iters = 1;
    while (iters == 1) {
        print_menu(hero);

        int us_inp;

        if ((scanf_s("%d", &us_inp) != 1) || (us_inp > 11) || (us_inp < 0)) {
            printf("Ошибка ввода, попробуйте ещё раз!\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }

        switch (us_inp) {
        case 0:
            printf("0 - Выход\n");
            free_supplies();
            iters = 0;
            return 0;
        case 1:
            printf("1 - Создать/пересоздать персонажа\n");
            create_hero();
            break;
        case 2:
            printf("2 - Карточка персонажа\n");
            show_hero_card(hero, &timing);
            break;
        case 3:
            if (!hero) { printf("Сначала создайте персонажа.\n"); break; }
            print_menu_time();
            int us_inp_time;

            if ((scanf_s("%d", &us_inp_time) != 1) || (us_inp_time > 3) || (us_inp_time < 0)) {
                printf("Ошибка ввода, попробуйте ещё раз!\n");
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {}
                continue;
            }

            switch (us_inp_time) {
            case 0: {
                printf("0 - Назад\n");
                break;
            }
            case 1: {
                printf("1 - Посмотреть на часы\n");
                show_time(&timing);
                break;
            }
            case 2: {
                printf("2 - Пропустить время (ждать)\n");
                wait(&timing);
                break;
            }
            case 3: {
                printf("3 - Отдых (лечь спать)\n");
                sleep(hero, &timing);
                break;
            }
            }
            break;
        case 4: {
            printf("4 - Посмотреть инвентарь\n");
            if (!hero) { printf("Сначала создайте персонажа.\n"); break; }
            show_inventory();
            break;
        }
        case 5:
            printf("5 - Положить предмет в слот\n");
            if (!hero) { printf("Сначала создайте персонажа.\n"); break; }
            put_item();
            break;
        case 6: {
            printf("6 - Выбросить предмет\n");
            if (!hero) { printf("Сначала создайте персонажа.\n"); break; }
            clean_slot();
            break;
        }
        case 7: {
            printf("7 - Уникальные находки\n");
            if (!hero) { printf("Сначала создайте персонажа.\n"); break; }
            show_uniques();
            break;
        }
        case 8: {
            printf("8 - Поиск предмета в рюкзаке по названию\n");
            if (!hero) { printf("Сначала создайте персонажа.\n"); break; }
            find_item();
            break;
        }
        case 9: {
            printf("9 - Записать состояние в дневник фермера\n");
            if (!hero) { printf("Сначала создайте персонажа.\n"); break; }
            write_diary(hero, &timing);
            break;
        }
        case 10: {
            printf("10 - Расшифровать старые записи\n");
            if (!hero) { printf("Сначала создайте персонажа.\n"); break; }
            count_items();
            break;
        }
        case 11: {
            printf("11 - Отправиться в приключение\n");

            if (!is_ready(hero, &timing)) {
                printf("Персонаж не готов к приключениям!\n");
                break;
            }

            srand((unsigned)time(NULL));

            if (!generate_map(&map, hero)) {
                fprintf(stderr, "Ошибка выделения памяти.\n");
                break;
            }

            printf("Сгенерирован лес %dx%d\n", map.width, map.height);
            print_hud(hero, &map, &timing);
            print_map(&map);

            int running = 1;
            while (running) {
                printf("Направление (W/A/S/D, Q - выйти): ");
                char cmd;
                if (scanf(" %c", &cmd) != 1) break;

                int dx = 0, dy = 0;
                switch (cmd) {
                case 'W': case 'w': case 'Ц': case 'ц': dy = -1; break;
                case 'S': case 's': case 'Ы': case 'ы': dy = 1; break;
                case 'A': case 'a': case 'Ф': case 'ф': dx = -1; break;
                case 'D': case 'd': case 'В': case 'в': dx = 1; break;
                case 'Q': case 'q': case 'Й': case 'й':
                    printf("Герой отправился домой.\n");
                    map.level = 0;
                    running = 0;
                    continue;
                default:
                    printf("Неизвестная команда.\n");
                    continue;
                }

                MoveResult r = try_move(&map, hero, &timing, dx, dy);

                putchar('\n');
                print_hud(hero, &map, &timing);
                print_map(&map);

                switch (r) {
                case MOVE_OUT_OF_STAM:
                    printf("Герой отправился домой (нет выносливости).\n");
                    map.level = 0;
                    hero->gold = hero->gold / 2;
                    running = 0;
                    break;
                case MOVE_MAP_CHANGED:
                    if (map.type == MAP_CAVE) printf("[Вы в пещере]\n");
                    else                       printf("[Вы в лесу]\n");
                    break;
                case MOVE_OK:
                case MOVE_BLOCKED:
                default:
                    break;
                }
            }
            free_map(&map, hero);
            break;
        }
        }
    }
    return 0;
}
