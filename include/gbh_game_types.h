#ifndef GBH_GAME_TYPES_H
#define GBH_GAME_TYPES_H

#include <cstdint>

namespace gbh
{
    constexpr int map_width = 31;
    constexpr int map_height = 23;
    constexpr int view_width = 9;
    constexpr int view_height = 6;
    constexpr int tile_px = 16;
    constexpr int max_enemies = 8;
    constexpr int max_items = 8;
    constexpr int max_hp = 9;
    constexpr int max_fervor = 9;
    constexpr int max_rooms = 24;

    enum class Mode : uint8_t
    {
        title,
        pilgrim,
        lore,
        dungeon,
        boss,
        victory,
        defeat
    };

    enum class TileKind : uint8_t
    {
        wall,
        floor,
        silence,
        door_open,
        door_false,
        door_silence,
        shrine,
        shrine_spent,
        exit
    };

    enum class EnemyKind : uint8_t
    {
        doubt,
        shadow,
        beast,
        noise,
        haste
    };

    enum class ItemKind : uint8_t
    {
        bread,
        candle,
        key,
        memory
    };

    enum class PilgrimKind : uint8_t
    {
        faith,
        hope,
        charity,
        peace
    };

    struct Enemy
    {
        int x = 0;
        int y = 0;
        int hp = 0;
        int step_counter = 0;
        EnemyKind kind = EnemyKind::doubt;
        bool alive = false;
        bool guardian = false;
    };

    struct Item
    {
        int x = 0;
        int y = 0;
        int memory_index = 0;
        ItemKind kind = ItemKind::bread;
        bool active = false;
    };

    inline int abs_int(int value)
    {
        return value < 0 ? -value : value;
    }

    inline int clamp_int(int value, int min_value, int max_value)
    {
        if(value < min_value)
        {
            return min_value;
        }

        if(value > max_value)
        {
            return max_value;
        }

        return value;
    }

    inline bool inside_map(int x, int y)
    {
        return x >= 0 && x < map_width && y >= 0 && y < map_height;
    }

    inline int distance_to(int ax, int ay, int bx, int by)
    {
        return abs_int(ax - bx) + abs_int(ay - by);
    }

    inline constexpr const char* roman_dwelling[] = {
        "I", "II", "III", "IV", "V", "VI", "VII"
    };
}

#endif
