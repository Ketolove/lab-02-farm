#define _CRT_SECURE_NO_WARNINGS
#include "player.h"
#include "map.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Player* player_create(void) {
    Player* p = (Player*)malloc(sizeof(Player));
    if (!p) {
        printf("Ошибка выделения памяти.\n");
        return NULL;
    }

    p->cur_hp = 100;
    p->max_hp = 100;
    p->cur_stam = 100;
    p->max_stam = 100;
    p->gold = 0;
    p->x = 0;
    p->y = 0;
    p->name[0] = '\0';
    memset(p->inventory, 0, sizeof p->inventory);

    p->item_count = utils_read_items(p->item_catalog, MAX_ITEMS);

    player_ask_name(p);
    printf("Персонаж создан по адресу %p\n", (void*)p);
    return p;
}

void player_destroy(Player* p) {
    debug_free(p);
}

void player_ask_name(Player* p) {
    while (1) {
        printf("Как Вас зовут?\n");
        if (!read_word(p->name, MAX_NAME_LEN)) {
            printf("Ошибка ввода!\n");
            continue;
        }
        printf("Здравствуйте, %s!\n", p->name);
        return;
    }
}

void player_show_card(const Player* p, int day, int hour, const Weather* w) {
    if (!p) {
        printf("Персонаж ещё не создан.\n");
        return;
    }
    printf("\n=== Карточка персонажа ===\n");
    printf("Имя:     %s\n", p->name);
    printf("HP:      %d/%d\n", p->cur_hp, p->max_hp);
    printf("Стамина: %d/%d\n", p->cur_stam, p->max_stam);
    printf("Золото:  %d\n", p->gold);
    printf("Время:   День %d, %02d:00\n", day, hour);
    printf("Погода:  %s, %d°C, ветер %d\n",
        weather_name(w->type), w->temperature, w->wind_power);
    printf("Инвентарь:\n");
    for (int i = 0; i < INV_LEN; i++) {
        int id = p->inventory[i];
        printf("  Слот %d: [%d] %s\n", i, id, p->item_catalog[id] + 2);
    }
}

void player_show_inventory(const Player* p) {
    for (int i = 0; i < INV_LEN; i++) {
        int id = p->inventory[i];
        printf("Слот %d: [%d] - %s\n", i, id, p->item_catalog[id] + 2);
    }
}

void player_put_item(Player* p) {
    int slot, id;

    printf("Введите номер слота (от 0 до %d): ", INV_LEN - 1);
    if (!read_int(&slot)) { printf("Ошибка ввода!\n"); return; }

    printf("Введите ID предмета: ");
    if (!read_int(&id)) { printf("Ошибка ввода!\n"); return; }

    if (slot < 0 || slot >= INV_LEN) {
        printf("Ошибка: нет такого слота! (допустимо 0..%d)\n", INV_LEN - 1);
        return;
    }
    if (id < 0 || id >= p->item_count) {
        printf("Ошибка: нет такого ID! (допустимо 0..%d)\n", p->item_count - 1);
        return;
    }

    p->inventory[slot] = id;
    printf("В слот %d записан предмет [%d] (%s)\n",
        slot, id, p->item_catalog[id] + 2);
}

void player_clean_slot(Player* p) {
    int slot;
    printf("Введите номер слота: ");
    if (!read_int(&slot)) { printf("Ошибка ввода!\n"); return; }

    if (slot < 0 || slot >= INV_LEN) {
        printf("Ошибка: нет такого слота! (допустимо 0..%d)\n", INV_LEN - 1);
        return;
    }
    p->inventory[slot] = 0;
    printf("Слот %d очищен\n", slot);
}

void player_show_uniques(const Player* p) {
    player_show_inventory(p);
    printf("\n");

    int any = 0;
    for (int id = 1; id < p->item_count; id++) {
        int cnt = 0;
        for (int s = 0; s < INV_LEN; s++)
            if (p->inventory[s] == id) cnt++;
        if (cnt == 0) continue;

        any = 1;
        if (cnt == 1)
            printf("ID %d (%s) - уникален\n", id, p->item_catalog[id] + 2);
        else
            printf("ID %d (%s) - не уникален, лежит в %d слотах\n",
                id, p->item_catalog[id] + 2, cnt);
    }
    if (!any) printf("В инвентаре нет предметов.\n");
}

void player_find_item(const Player* p) {
    char name[MAX_NAME_LEN];
    printf("Что Вы ищете?\n");
    if (!read_word(name, MAX_NAME_LEN)) {
        printf("Ошибка ввода!\n");
        return;
    }

    int found_id = -1;
    for (int i = 0; i < p->item_count; i++) {
        if (strcmp(name, p->item_catalog[i] + 2) == 0) {
            found_id = i;
            break;
        }
    }
    if (found_id == -1) {
        printf("Предмет \"%s\" не найден в каталоге.\n", name);
        return;
    }

    int found_any = 0;
    for (int s = 0; s < INV_LEN; s++) {
        if (p->inventory[s] == found_id) {
            printf("Слот %d: [ID %d] (%s)\n",
                s, found_id, p->item_catalog[found_id] + 2);
            found_any = 1;
        }
    }
    if (!found_any)
        printf("Предмет \"%s\" (ID %d) есть в каталоге, но в инвентаре отсутствует.\n",
            name, found_id);
}

void player_sleep(Player* p) {
    if (!p) return;
    p->cur_hp = p->max_hp;
    p->cur_stam = p->max_stam;
}

int player_is_ready(const Player* p, int hour) {
    if (!p) return 0;
    return p->cur_stam > 0 && (hour < 2 || hour >= 8);
}