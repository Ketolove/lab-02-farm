#ifndef PLAYER_H
#define PLAYER_H

#define INV_LEN      10
#define MAX_ITEMS    10
#define MAX_NAME_LEN 32

typedef struct Weather Weather;

typedef struct {
    char name[MAX_NAME_LEN];
    int  cur_hp, max_hp;
    int  cur_stam, max_stam;
    int  gold;
    int  x, y;
    int  inventory[INV_LEN];
    char item_catalog[MAX_ITEMS][MAX_NAME_LEN];
    int  item_count;
} Player;

Player* player_create(void);
void    player_destroy(Player* p);

void player_ask_name(Player* p);

void player_show_card(const Player* p, int day, int hour, const Weather* w);

void player_show_inventory(const Player* p);
void player_put_item(Player* p);
void player_clean_slot(Player* p);
void player_show_uniques(const Player* p);
void player_find_item(const Player* p);

void player_sleep(Player* p);
int  player_is_ready(const Player* p, int hour);

#endif