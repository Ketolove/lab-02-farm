#define _CRT_SECURE_NO_WARNINGS
#include "utils.h"
#include "player.h"
#include <stdlib.h>
#include <string.h>

void clear_input(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int read_int(int* out) {
    if (scanf_s("%d", out) != 1) {
        clear_input();
        return 0;
    }
    return 1;
}

int read_word(char* buf, int max_len) {
    (void)max_len;
    if (scanf("%31s", buf) != 1) {
        clear_input();
        return 0;
    }
    clear_input();
    return 1;
}

void debug_free(void* ptr) {
    if (!ptr) return;
    printf("Память структуры по адресу %p освобождена\n", ptr);
    free(ptr);
}

static const char* default_items[] = {
    "0 Пусто", "1 Дерево", "2 Камень", "3 Семена", "4 Лейка",
    "5 Ведро", "6 Удобрение", "7 Корзина", "8 Грабли", "9 Лопата"
};

int utils_read_items(char names[][32], int max_items) {
    for (int i = 0; i < max_items; i++) {
        strncpy(names[i], default_items[i], 31);
        names[i][31] = '\0';
    }
    return max_items;
}

void utils_write_diary(const char* hero_name,
    int day, int hour,
    const char* weather_name, int temp, int wind,
    const Player* p,
    const char* note) {
    FILE* fp = fopen("diary.txt", "a");
    if (!fp) {
        printf("Ошибка открытия файла.\n");
        return;
    }

    fprintf(fp, "Фермер: %s\n", hero_name);
    fprintf(fp, "Текущее время: День %d, %02d:00\n", day, hour);
    fprintf(fp, "Погода: %s, %d°C, ветер %d\n", weather_name, temp, wind);
    fprintf(fp, "Список предметов в инвентаре:\n");

    if (p) {
        for (int i = 0; i < p->item_count; i++)
            fprintf(fp, "%s ", p->item_catalog[i] + 2);
    }
    fprintf(fp, "\nЗаметка:\n%s\n", note);
    for (int i = 0; i < 68; i++) fputc('-', fp);
    fputc('\n', fp);

    fclose(fp);
    printf("Запись сохранена.\n");
}

typedef struct {
    char* name;
    int   total;
} Supply;

void utils_count_supplies(void) {
    FILE* fp = fopen("input.txt", "r");
    if (!fp) {
        printf("Ошибка: файл input.txt не найден.\n");
        return;
    }

    Supply* arr = NULL;
    int     count = 0;
    char    buf[1024];

    while (fgets(buf, sizeof buf, fp)) {
        char name[32];
        int  cnt;
        if (sscanf(buf, "%31s %d", name, &cnt) != 2) continue;
        if (cnt <= 0) continue;

        int found = -1;
        for (int k = 0; k < count; k++) {
            if (strcmp(arr[k].name, name) == 0) { found = k; break; }
        }

        if (found >= 0) {
            arr[found].total += cnt;
        }
        else {
            Supply* tmp = realloc(arr, (size_t)(count + 1) * sizeof * tmp);
            if (!tmp) { printf("Ошибка выделения памяти.\n"); break; }
            arr = tmp;

            size_t len = strlen(name);
            arr[count].name = malloc(len + 1);
            if (!arr[count].name) { printf("Ошибка выделения памяти.\n"); break; }
            memcpy(arr[count].name, name, len + 1);
            arr[count].total = cnt;
            count++;
        }
    }
    fclose(fp);

    if (count == 0) {
        printf("В input.txt не найдено корректных поставок.\n");
        free(arr);
        return;
    }

    FILE* out = fopen("output.txt", "w");
    for (int i = 0; i < count; i++) {
        if (out) fprintf(out, "%s: %d шт.\n", arr[i].name, arr[i].total);
        printf("%s: %d шт.\n", arr[i].name, arr[i].total);
    }
    if (out) fclose(out);

    for (int i = 0; i < count; i++) free(arr[i].name);
    free(arr);

    printf("Сводка записана в output.txt.\n");
}

int hours_until_2am(int current_hour) {
    if (current_hour < 2)  return 2 - current_hour;
    if (current_hour == 2) return 24;
    return 24 - current_hour + 2;
}

void advance_hours(int* day, int* hour, int delta) {
    int total = *hour + delta;
    *day += total / 24;
    *hour = total % 24;
}