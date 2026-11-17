#ifndef MAP_H
#define MAP_H

typedef enum { MAP_FOREST = 0, MAP_CAVE = 1 } MapType;

typedef enum {
    MOVE_BLOCKED,
    MOVE_OK,
    MOVE_MAP_CHANGED,
    MOVE_OUT_OF_STAM
} MoveResult;

typedef enum {
    WEATHER_CLEAR = 0,
    WEATHER_RAIN,
    WEATHER_FOG,
    WEATHER_HURRICANE
} WeatherType;

typedef struct Weather {
    WeatherType type;
    int         temperature;
    int         wind_power;
} Weather;

typedef struct {
    char** grid;
    int      width;
    int      height;
    MapType  type;
    int      level;
} Map;

struct Player;

Map* map_create(void);
void  map_destroy(Map* m);

int   map_generate_forest(Map* m, struct Player* h);
int   map_generate_cave(Map* m, struct Player* h);

void  map_print(const Map* m);

MoveResult map_try_move(Map* m, struct Player* h,
    int* day, int* hour, Weather* w,
    int dx, int dy);

Weather     weather_generate(void);
const char* weather_name(WeatherType t);

#endif
