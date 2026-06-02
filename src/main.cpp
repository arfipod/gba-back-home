#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_display.h"
#include "bn_bg_palettes.h"
#include "bn_color.h"
#include "bn_log.h"
#include "bn_music_items.h"
#include "bn_sound_items.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_vector.h"

#include <cstdint>

#include "common_fixed_8x8_sprite_font.h"

#include "bn_sprite_items_tile_floor.h"
#include "bn_sprite_items_tile_wall.h"
#include "bn_sprite_items_tile_door.h"
#include "bn_sprite_items_tile_door_false.h"
#include "bn_sprite_items_tile_door_silence.h"
#include "bn_sprite_items_tile_silence.h"
#include "bn_sprite_items_tile_shrine.h"
#include "bn_sprite_items_tile_shrine_spent.h"
#include "bn_sprite_items_tile_exit.h"
#include "bn_sprite_items_player_down.h"
#include "bn_sprite_items_player_up.h"
#include "bn_sprite_items_player_left.h"
#include "bn_sprite_items_player_right.h"
#include "bn_sprite_items_enemy_doubt.h"
#include "bn_sprite_items_enemy_shadow.h"
#include "bn_sprite_items_enemy_beast.h"
#include "bn_sprite_items_enemy_noise.h"
#include "bn_sprite_items_enemy_haste.h"
#include "bn_sprite_items_boss_false_door.h"
#include "bn_sprite_items_item_bread.h"
#include "bn_sprite_items_item_candle.h"
#include "bn_sprite_items_item_key.h"
#include "bn_sprite_items_item_relic.h"
#include "bn_sprite_items_attack_wave.h"

namespace
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

    enum class mode : uint8_t
    {
        title,
        pilgrim,
        lore,
        dungeon,
        boss,
        victory,
        defeat
    };

    enum class tile_kind : uint8_t
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

    enum class enemy_kind : uint8_t
    {
        doubt,
        shadow,
        beast,
        noise,
        haste
    };

    enum class item_kind : uint8_t
    {
        bread,
        candle,
        key,
        memory
    };

    enum class pilgrim_kind : uint8_t
    {
        faith,
        hope,
        charity,
        peace
    };

    struct enemy
    {
        int x = 0;
        int y = 0;
        int hp = 0;
        int step_counter = 0;
        enemy_kind kind = enemy_kind::doubt;
        bool alive = false;
        bool guardian = false;
    };

    struct item
    {
        int x = 0;
        int y = 0;
        int memory_index = 0;
        item_kind kind = item_kind::bread;
        bool active = false;
    };

    int abs_int(int value)
    {
        return value < 0 ? -value : value;
    }

    int clamp_int(int value, int min_value, int max_value)
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

    bool inside_map(int x, int y)
    {
        return x >= 0 && x < map_width && y >= 0 && y < map_height;
    }

    int distance_to(int ax, int ay, int bx, int by)
    {
        return abs_int(ax - bx) + abs_int(ay - by);
    }

    constexpr const char* roman_morada[] = {
        "I", "II", "III", "IV", "V", "VI", "VII"
    };

    constexpr const char* pilgrim_names[] = {
        "FE",
        "ESPERANZA",
        "CARIDAD",
        "PAZ"
    };

    constexpr const char* pilgrim_rules[] = {
        "+1 luz maxima.",
        "Una caida te levanta.",
        "La oracion pacifica.",
        "Menos ruido enemigo."
    };

    constexpr const char* dwelling_lore[] = {
        "I  Conocerse abre.",
        "II La llamada orienta.",
        "III La regla sostiene.",
        "IV La quietud revela.",
        "V  Confiar es andar.",
        "VI El fuego purifica.",
        "VII La casa llama."
    };

    constexpr const char* dwelling_rules[] = {
        "Aprende limites y luz.",
        "R escucha la llamada.",
        "Recursos escasos.",
        "R recupera luz segura.",
        "Puertas falsas abundan.",
        "Bestias guardan umbrales.",
        "Discierne la puerta."
    };

    constexpr const char* memory_texts[] = {
        "No estaba lejos.",
        "La voz no grita.",
        "La regla no encierra.",
        "Callar tambien anda.",
        "La luz basta cerca.",
        "El deseo se limpia.",
        "Era hondo."
    };

    const bn::sprite_item& tile_item(tile_kind kind)
    {
        switch(kind)
        {
        case tile_kind::wall:
            return bn::sprite_items::tile_wall;
        case tile_kind::silence:
            return bn::sprite_items::tile_silence;
        case tile_kind::door_open:
            return bn::sprite_items::tile_door;
        case tile_kind::door_false:
            return bn::sprite_items::tile_door_false;
        case tile_kind::door_silence:
            return bn::sprite_items::tile_door_silence;
        case tile_kind::shrine:
            return bn::sprite_items::tile_shrine;
        case tile_kind::shrine_spent:
            return bn::sprite_items::tile_shrine_spent;
        case tile_kind::exit:
            return bn::sprite_items::tile_exit;
        case tile_kind::floor:
        default:
            return bn::sprite_items::tile_floor;
        }
    }

    const bn::sprite_item& enemy_item(enemy_kind kind)
    {
        switch(kind)
        {
        case enemy_kind::shadow:
            return bn::sprite_items::enemy_shadow;
        case enemy_kind::beast:
            return bn::sprite_items::enemy_beast;
        case enemy_kind::noise:
            return bn::sprite_items::enemy_noise;
        case enemy_kind::haste:
            return bn::sprite_items::enemy_haste;
        case enemy_kind::doubt:
        default:
            return bn::sprite_items::enemy_doubt;
        }
    }

    const bn::sprite_item& item_sprite_item(item_kind kind)
    {
        switch(kind)
        {
        case item_kind::candle:
            return bn::sprite_items::item_candle;
        case item_kind::key:
            return bn::sprite_items::item_key;
        case item_kind::memory:
            return bn::sprite_items::item_relic;
        case item_kind::bread:
        default:
            return bn::sprite_items::item_bread;
        }
    }

    class game
    {
    public:
        game() :
            _text_generator(common::fixed_8x8_sprite_font)
        {
            bn::bg_palettes::set_transparent_color(bn::color(2, 1, 4));
            _text_generator.set_center_alignment();
            bn::music_items::pilgrimage.play(0.45);
            set_mode(mode::title);
            BN_LOG("gba-back-home design slice boot");
        }

        void update()
        {
            switch(_mode)
            {
            case mode::title:
                update_title();
                break;
            case mode::pilgrim:
                update_pilgrim();
                break;
            case mode::lore:
                update_lore();
                break;
            case mode::dungeon:
                update_dungeon();
                break;
            case mode::boss:
                update_boss();
                break;
            case mode::victory:
                update_victory();
                break;
            case mode::defeat:
                update_defeat();
                break;
            }
        }

    private:
        mode _mode = mode::title;
        tile_kind _map[map_height][map_width] = {};
        enemy _enemies[max_enemies];
        item _items[max_items];

        bn::sprite_text_generator _text_generator;
        bn::vector<bn::sprite_ptr, view_width * view_height> _tile_sprites;
        bn::vector<bn::sprite_ptr, max_enemies> _enemy_sprites;
        bn::vector<bn::sprite_ptr, max_items> _item_sprites;
        bn::vector<bn::sprite_ptr, 1> _player_sprites;
        bn::vector<bn::sprite_ptr, 1> _attack_sprites;
        bn::vector<bn::sprite_ptr, 1> _boss_sprites;
        bn::vector<bn::sprite_ptr, 128> _text_sprites;

        uint32_t _rng = 0x51A7E5ED;
        int _pilgrim_index = 0;
        int _morada = 1;
        int _player_x = map_width / 2;
        int _player_y = map_height / 2;
        int _dir_x = 0;
        int _dir_y = 1;
        int _hp = max_hp;
        int _fervor = 4;
        int _turns = 0;
        int _memories_mask = 0;
        int _memory_count = 0;
        int _mercy_count = 0;
        int _shrine_prayer = 0;
        int _silence_chain = 0;
        int _boss_marks = 0;
        int _boss_turn = 0;
        int _attack_timer = 0;
        int _message_timer = 0;
        int _exit_x = 0;
        int _exit_y = 0;
        int _key_x = 0;
        int _key_y = 0;
        int _last_memory = -1;
        bool _has_key = false;
        bool _hope_used = false;
        bool _paused = false;
        bool _last_action_silence = false;
        const char* _message = "Busca el umbral.";

        pilgrim_kind pilgrim() const
        {
            return pilgrim_kind(_pilgrim_index);
        }

        int current_max_fervor() const
        {
            return max_fervor + (pilgrim() == pilgrim_kind::faith ? 1 : 0);
        }

        int prayer_range() const
        {
            return pilgrim() == pilgrim_kind::faith ? 3 : 2;
        }

        int enemy_sense_range() const
        {
            int range = 6 + (_morada >= 6 ? 1 : 0);
            if(pilgrim() == pilgrim_kind::peace)
            {
                --range;
            }
            return range;
        }

        uint32_t rand_u32()
        {
            _rng = (_rng * 1664525u) + 1013904223u;
            return _rng;
        }

        int rand_range(int limit)
        {
            return int((rand_u32() >> 16) % uint32_t(limit));
        }

        void clear_scene()
        {
            _tile_sprites.clear();
            _enemy_sprites.clear();
            _item_sprites.clear();
            _player_sprites.clear();
            _attack_sprites.clear();
            _boss_sprites.clear();
            _text_sprites.clear();
        }

        void set_mode(mode next_mode)
        {
            clear_scene();
            _mode = next_mode;

            switch(next_mode)
            {
            case mode::title:
                draw_title();
                break;
            case mode::pilgrim:
                draw_pilgrim();
                break;
            case mode::lore:
                draw_lore();
                break;
            case mode::dungeon:
                enter_dungeon_morada();
                break;
            case mode::boss:
                enter_boss();
                break;
            case mode::victory:
                draw_victory();
                break;
            case mode::defeat:
                draw_defeat();
                break;
            }
        }

        void draw_title()
        {
            _text_sprites.clear();
            _text_generator.set_center_alignment();
            _text_generator.generate(0, -64, "GBA BACK HOME", _text_sprites);
            _text_generator.generate(0, -46, "Regreso a la casa", _text_sprites);
            _text_generator.generate(0, -18, "Roguelike interior", _text_sprites);
            _text_generator.generate(0, 8, "A: peregrinar", _text_sprites);
            _text_generator.generate(0, 24, "B: semilla paz", _text_sprites);
            _text_generator.generate(0, 52, "START: nuevo nombre", _text_sprites);
        }

        void update_title()
        {
            if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
            {
                bn::sound_items::pickup.play(0.8);
                set_mode(mode::pilgrim);
            }
            else if(bn::keypad::b_pressed())
            {
                _pilgrim_index = int(pilgrim_kind::peace);
                start_new_run();
                set_mode(mode::lore);
            }
        }

        void draw_pilgrim()
        {
            _text_sprites.clear();
            _text_generator.set_center_alignment();
            _text_generator.generate(0, -66, "NOMBRE DEL PEREGRINO", _text_sprites);
            _text_generator.generate(0, -40, pilgrim_names[_pilgrim_index], _text_sprites);
            _text_generator.generate(0, -18, pilgrim_rules[_pilgrim_index], _text_sprites);
            _text_generator.generate(0, 20, "LEFT/RIGHT cambia", _text_sprites);
            _text_generator.generate(0, 36, "A confirma", _text_sprites);
            _text_generator.generate(0, 56, "Cada nombre cambia la run", _text_sprites);
        }

        void update_pilgrim()
        {
            if(bn::keypad::left_pressed())
            {
                _pilgrim_index = (_pilgrim_index + 3) % 4;
                bn::sound_items::pickup.play(0.7);
                draw_pilgrim();
            }
            else if(bn::keypad::right_pressed())
            {
                _pilgrim_index = (_pilgrim_index + 1) % 4;
                bn::sound_items::pickup.play(0.7);
                draw_pilgrim();
            }
            else if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
            {
                start_new_run();
                set_mode(mode::lore);
            }
            else if(bn::keypad::b_pressed())
            {
                set_mode(mode::title);
            }
        }

        void start_new_run()
        {
            _rng = 0xA7C00100u + uint32_t(_pilgrim_index * 0x1F123BB5u);
            _morada = 1;
            _hp = max_hp;
            _fervor = 4 + (pilgrim() == pilgrim_kind::peace ? 1 : 0);
            if(_fervor > current_max_fervor())
            {
                _fervor = current_max_fervor();
            }
            _turns = 0;
            _memories_mask = 0;
            _memory_count = 0;
            _mercy_count = 0;
            _hope_used = false;
            _has_key = false;
            bn::sound_items::door.play(0.9);
        }

        void draw_lore()
        {
            _text_sprites.clear();
            _text_generator.set_center_alignment();
            _text_generator.generate(0, -68, "CASTILLO INTERIOR", _text_sprites);
            _text_generator.generate(0, -46, pilgrim_names[_pilgrim_index], _text_sprites);
            _text_generator.generate(0, -22, dwelling_lore[_morada - 1], _text_sprites);
            _text_generator.generate(0, -4, dwelling_rules[_morada - 1], _text_sprites);
            _text_generator.generate(0, 28, "A: entrar", _text_sprites);
            _text_generator.generate(0, 44, "B: titulo", _text_sprites);
            _text_generator.generate(0, 60, "R: callar / L: memoria", _text_sprites);
        }

        void update_lore()
        {
            if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
            {
                bn::sound_items::door.play(0.8);
                set_mode(mode::dungeon);
            }
            else if(bn::keypad::b_pressed())
            {
                set_mode(mode::title);
            }
        }

        tile_kind tile_at(int x, int y) const
        {
            if(! inside_map(x, y))
            {
                return tile_kind::wall;
            }

            return _map[y][x];
        }

        bool path_walkable(tile_kind tile) const
        {
            return tile == tile_kind::floor || tile == tile_kind::silence || tile == tile_kind::door_open ||
                   tile == tile_kind::shrine || tile == tile_kind::shrine_spent || tile == tile_kind::exit;
        }

        bool is_walkable(int x, int y) const
        {
            return path_walkable(tile_at(x, y));
        }

        bool occupied_by_enemy(int x, int y) const
        {
            for(int index = 0; index < max_enemies; ++index)
            {
                if(_enemies[index].alive && _enemies[index].x == x && _enemies[index].y == y)
                {
                    return true;
                }
            }

            return false;
        }

        bool occupied_by_item(int x, int y) const
        {
            for(int index = 0; index < max_items; ++index)
            {
                if(_items[index].active && _items[index].x == x && _items[index].y == y)
                {
                    return true;
                }
            }

            return false;
        }

        int enemy_at(int x, int y) const
        {
            for(int index = 0; index < max_enemies; ++index)
            {
                if(_enemies[index].alive && _enemies[index].x == x && _enemies[index].y == y)
                {
                    return index;
                }
            }

            return -1;
        }

        int item_at(int x, int y) const
        {
            for(int index = 0; index < max_items; ++index)
            {
                if(_items[index].active && _items[index].x == x && _items[index].y == y)
                {
                    return index;
                }
            }

            return -1;
        }

        void reset_entities()
        {
            for(int index = 0; index < max_enemies; ++index)
            {
                _enemies[index] = enemy();
            }

            for(int index = 0; index < max_items; ++index)
            {
                _items[index] = item();
            }
        }

        void carve_room(int cx, int cy, int rx, int ry)
        {
            for(int y = cy - ry; y <= cy + ry; ++y)
            {
                for(int x = cx - rx; x <= cx + rx; ++x)
                {
                    if(x > 0 && x < map_width - 1 && y > 0 && y < map_height - 1)
                    {
                        _map[y][x] = tile_kind::floor;
                    }
                }
            }
        }

        void carve_corridor(int ax, int ay, int bx, int by)
        {
            int x = ax;
            int y = ay;
            while(x != bx)
            {
                _map[y][x] = tile_kind::floor;
                x += bx > x ? 1 : -1;
            }
            while(y != by)
            {
                _map[y][x] = tile_kind::floor;
                y += by > y ? 1 : -1;
            }
            _map[y][x] = tile_kind::floor;
        }

        void compute_distances(int start_x, int start_y, int dist[map_height][map_width]) const
        {
            int queue_x[map_width * map_height];
            int queue_y[map_width * map_height];
            int head = 0;
            int tail = 0;

            for(int y = 0; y < map_height; ++y)
            {
                for(int x = 0; x < map_width; ++x)
                {
                    dist[y][x] = -1;
                }
            }

            if(! inside_map(start_x, start_y) || ! path_walkable(tile_at(start_x, start_y)))
            {
                return;
            }

            dist[start_y][start_x] = 0;
            queue_x[tail] = start_x;
            queue_y[tail] = start_y;
            ++tail;

            constexpr int dirs_x[] = { 1, -1, 0, 0 };
            constexpr int dirs_y[] = { 0, 0, 1, -1 };

            while(head < tail)
            {
                int x = queue_x[head];
                int y = queue_y[head];
                ++head;

                for(int dir = 0; dir < 4; ++dir)
                {
                    int nx = x + dirs_x[dir];
                    int ny = y + dirs_y[dir];
                    if(inside_map(nx, ny) && dist[ny][nx] < 0 && path_walkable(tile_at(nx, ny)))
                    {
                        dist[ny][nx] = dist[y][x] + 1;
                        queue_x[tail] = nx;
                        queue_y[tail] = ny;
                        ++tail;
                    }
                }
            }
        }

        bool random_floor_cell(int& out_x, int& out_y, int min_start_distance)
        {
            for(int attempt = 0; attempt < 260; ++attempt)
            {
                int x = 1 + rand_range(map_width - 2);
                int y = 1 + rand_range(map_height - 2);
                if(_map[y][x] == tile_kind::floor && distance_to(x, y, _player_x, _player_y) >= min_start_distance &&
                   ! occupied_by_enemy(x, y) && ! occupied_by_item(x, y))
                {
                    out_x = x;
                    out_y = y;
                    return true;
                }
            }

            return false;
        }

        bool random_wall_next_to_floor(int& out_x, int& out_y)
        {
            constexpr int dirs_x[] = { 1, -1, 0, 0 };
            constexpr int dirs_y[] = { 0, 0, 1, -1 };

            for(int attempt = 0; attempt < 260; ++attempt)
            {
                int x = 2 + rand_range(map_width - 4);
                int y = 2 + rand_range(map_height - 4);
                if(_map[y][x] != tile_kind::wall)
                {
                    continue;
                }

                int floor_neighbors = 0;
                for(int dir = 0; dir < 4; ++dir)
                {
                    int nx = x + dirs_x[dir];
                    int ny = y + dirs_y[dir];
                    if(path_walkable(tile_at(nx, ny)))
                    {
                        ++floor_neighbors;
                    }
                }

                if(floor_neighbors == 1)
                {
                    out_x = x;
                    out_y = y;
                    return true;
                }
            }

            return false;
        }

        void generate_dungeon()
        {
            for(int y = 0; y < map_height; ++y)
            {
                for(int x = 0; x < map_width; ++x)
                {
                    _map[y][x] = tile_kind::wall;
                }
            }

            reset_entities();
            _has_key = false;
            _paused = false;
            _shrine_prayer = 0;
            _silence_chain = 0;
            _attack_timer = 0;
            _player_x = map_width / 2;
            _player_y = map_height / 2;
            _dir_x = 0;
            _dir_y = 1;

            int rooms_x[max_rooms];
            int rooms_y[max_rooms];
            int room_count = 0;

            rooms_x[room_count] = _player_x;
            rooms_y[room_count] = _player_y;
            ++room_count;
            carve_room(_player_x, _player_y, 2, 1);

            int cx = _player_x;
            int cy = _player_y;
            int critical_rooms = 6 + _morada;
            if(critical_rooms > 13)
            {
                critical_rooms = 13;
            }

            for(int index = 0; index < critical_rooms && room_count < max_rooms; ++index)
            {
                int nx = 3 + rand_range(map_width - 6);
                int ny = 3 + rand_range(map_height - 6);
                if(distance_to(nx, ny, cx, cy) < 6)
                {
                    nx = clamp_int(cx + (rand_range(2) == 0 ? -7 : 7), 3, map_width - 4);
                    ny = clamp_int(cy + (rand_range(2) == 0 ? -5 : 5), 3, map_height - 4);
                }

                carve_corridor(cx, cy, nx, ny);
                carve_room(nx, ny, 1 + rand_range(2), 1 + rand_range(2));
                rooms_x[room_count] = nx;
                rooms_y[room_count] = ny;
                ++room_count;
                cx = nx;
                cy = ny;
            }

            int branch_count = 3 + (_morada / 2);
            for(int branch = 0; branch < branch_count && room_count < max_rooms; ++branch)
            {
                int base = rand_range(room_count);
                int bx = clamp_int(rooms_x[base] + (rand_range(2) == 0 ? -6 : 6), 3, map_width - 4);
                int by = clamp_int(rooms_y[base] + (rand_range(2) == 0 ? -4 : 4), 3, map_height - 4);
                carve_corridor(rooms_x[base], rooms_y[base], bx, by);
                carve_room(bx, by, 1 + rand_range(2), 1);
                rooms_x[room_count] = bx;
                rooms_y[room_count] = by;
                ++room_count;
            }

            int dist_from_start[map_height][map_width];
            compute_distances(_player_x, _player_y, dist_from_start);

            _exit_x = _player_x;
            _exit_y = _player_y;
            int best_exit = -1;
            for(int y = 1; y < map_height - 1; ++y)
            {
                for(int x = 1; x < map_width - 1; ++x)
                {
                    if(_map[y][x] == tile_kind::floor && dist_from_start[y][x] > best_exit)
                    {
                        best_exit = dist_from_start[y][x];
                        _exit_x = x;
                        _exit_y = y;
                    }
                }
            }

            _map[_exit_y][_exit_x] = tile_kind::exit;

            int dist_from_exit[map_height][map_width];
            compute_distances(_exit_x, _exit_y, dist_from_exit);
            _key_x = _player_x;
            _key_y = _player_y;
            int best_key_score = -1;
            for(int y = 1; y < map_height - 1; ++y)
            {
                for(int x = 1; x < map_width - 1; ++x)
                {
                    if(_map[y][x] == tile_kind::floor && dist_from_start[y][x] >= 7 && dist_from_exit[y][x] >= 5)
                    {
                        int score = dist_from_start[y][x] + dist_from_exit[y][x] + rand_range(4);
                        if(score > best_key_score)
                        {
                            best_key_score = score;
                            _key_x = x;
                            _key_y = y;
                        }
                    }
                }
            }

            if(best_key_score < 0)
            {
                int fallback_best = -1;
                for(int y = 1; y < map_height - 1; ++y)
                {
                    for(int x = 1; x < map_width - 1; ++x)
                    {
                        if(_map[y][x] == tile_kind::floor && dist_from_start[y][x] > fallback_best && !(x == _exit_x && y == _exit_y))
                        {
                            fallback_best = dist_from_start[y][x];
                            _key_x = x;
                            _key_y = y;
                        }
                    }
                }
            }

            add_item(0, _key_x, _key_y, item_kind::key, _morada - 1);

            int sx = 0;
            int sy = 0;
            if(random_floor_cell(sx, sy, 5))
            {
                _map[sy][sx] = tile_kind::shrine;
            }

            int silence_tiles = _morada >= 4 ? 3 : 1;
            for(int index = 0; index < silence_tiles; ++index)
            {
                int tx = 0;
                int ty = 0;
                if(random_floor_cell(tx, ty, 4))
                {
                    _map[ty][tx] = tile_kind::silence;
                }
            }

            int memory_x = 0;
            int memory_y = 0;
            if(random_floor_cell(memory_x, memory_y, 6))
            {
                add_item(1, memory_x, memory_y, item_kind::memory, _morada - 1);
            }

            int resource_count = 3 + (_morada <= 2 ? 1 : 0) - (_morada >= 6 ? 1 : 0);
            if(resource_count < 2)
            {
                resource_count = 2;
            }
            for(int index = 0; index < resource_count && (index + 2) < max_items; ++index)
            {
                int ix = 0;
                int iy = 0;
                if(random_floor_cell(ix, iy, 4))
                {
                    item_kind kind = (index % 2 == 0 || _morada >= 4) ? item_kind::candle : item_kind::bread;
                    add_item(index + 2, ix, iy, kind, _morada - 1);
                }
            }

            int false_doors = (_morada >= 5 ? 4 : 1 + (_morada / 3));
            for(int index = 0; index < false_doors; ++index)
            {
                int dx = 0;
                int dy = 0;
                if(random_wall_next_to_floor(dx, dy))
                {
                    _map[dy][dx] = tile_kind::door_false;
                }
            }

            int silence_doors = (_morada >= 4 ? 2 : 1);
            for(int index = 0; index < silence_doors; ++index)
            {
                int dx = 0;
                int dy = 0;
                if(random_wall_next_to_floor(dx, dy))
                {
                    _map[dy][dx] = tile_kind::door_silence;
                }
            }

            int enemy_count = 2 + _morada;
            if(enemy_count > max_enemies)
            {
                enemy_count = max_enemies;
            }

            for(int index = 0; index < enemy_count; ++index)
            {
                int ex = 0;
                int ey = 0;
                if(random_floor_cell(ex, ey, 5))
                {
                    enemy_kind kind = choose_enemy_kind(index);
                    spawn_enemy(index, ex, ey, kind, false);
                }
            }

            if(_morada >= 6)
            {
                int gx = 0;
                int gy = 0;
                if(find_floor_near(_exit_x, _exit_y, gx, gy, 4))
                {
                    int slot = first_free_enemy_slot();
                    if(slot >= 0)
                    {
                        spawn_enemy(slot, gx, gy, enemy_kind::beast, true);
                    }
                }
            }
        }

        void add_item(int slot, int x, int y, item_kind kind, int memory_index)
        {
            if(slot < 0 || slot >= max_items)
            {
                return;
            }

            _items[slot].x = x;
            _items[slot].y = y;
            _items[slot].kind = kind;
            _items[slot].memory_index = memory_index;
            _items[slot].active = true;
        }

        enemy_kind choose_enemy_kind(int index)
        {
            int roll = rand_range(12);
            if(_morada >= 6 && (roll >= 9 || index == 0))
            {
                return enemy_kind::beast;
            }
            if(_morada >= 5 && roll >= 7)
            {
                return enemy_kind::haste;
            }
            if(_morada >= 4 && roll == 0)
            {
                return enemy_kind::noise;
            }
            if(_morada >= 3 && roll >= 5)
            {
                return enemy_kind::shadow;
            }
            return enemy_kind::doubt;
        }

        void spawn_enemy(int slot, int x, int y, enemy_kind kind, bool guardian)
        {
            if(slot < 0 || slot >= max_enemies)
            {
                return;
            }

            _enemies[slot].x = x;
            _enemies[slot].y = y;
            _enemies[slot].kind = kind;
            _enemies[slot].guardian = guardian;
            _enemies[slot].alive = true;
            _enemies[slot].step_counter = rand_range(20);

            switch(kind)
            {
            case enemy_kind::beast:
                _enemies[slot].hp = 3;
                break;
            case enemy_kind::shadow:
            case enemy_kind::haste:
                _enemies[slot].hp = 2;
                break;
            case enemy_kind::noise:
            case enemy_kind::doubt:
            default:
                _enemies[slot].hp = 1;
                break;
            }
        }

        int first_free_enemy_slot() const
        {
            for(int index = 0; index < max_enemies; ++index)
            {
                if(! _enemies[index].alive)
                {
                    return index;
                }
            }
            return -1;
        }

        bool find_floor_near(int target_x, int target_y, int& out_x, int& out_y, int radius)
        {
            for(int attempt = 0; attempt < 180; ++attempt)
            {
                int x = clamp_int(target_x - radius + rand_range((radius * 2) + 1), 1, map_width - 2);
                int y = clamp_int(target_y - radius + rand_range((radius * 2) + 1), 1, map_height - 2);
                if(_map[y][x] == tile_kind::floor && ! occupied_by_enemy(x, y) && ! occupied_by_item(x, y) &&
                   distance_to(x, y, _player_x, _player_y) >= 5)
                {
                    out_x = x;
                    out_y = y;
                    return true;
                }
            }
            return random_floor_cell(out_x, out_y, 6);
        }

        void enter_dungeon_morada()
        {
            _message = dwelling_lore[_morada - 1];
            _message_timer = 120;
            generate_dungeon();
            create_dungeon_sprites();
            BN_LOG("morada ", _morada, " seed ", int(_rng));
        }

        void create_dungeon_sprites()
        {
            _tile_sprites.clear();
            for(int row = 0; row < view_height; ++row)
            {
                for(int col = 0; col < view_width; ++col)
                {
                    int sx = (col - (view_width / 2)) * tile_px;
                    int sy = (row - (view_height / 2)) * tile_px;
                    bn::sprite_ptr sprite = bn::sprite_items::tile_floor.create_sprite(sx, sy);
                    sprite.set_z_order(40);
                    _tile_sprites.push_back(sprite);
                }
            }

            _player_sprites.push_back(bn::sprite_items::player_down.create_sprite(0, 0));
            _player_sprites[0].set_z_order(-20);

            _enemy_sprites.clear();
            for(int index = 0; index < max_enemies; ++index)
            {
                bn::sprite_ptr sprite = bn::sprite_items::enemy_doubt.create_sprite(0, 0);
                sprite.set_visible(false);
                sprite.set_z_order(-10);
                _enemy_sprites.push_back(sprite);
            }

            _item_sprites.clear();
            for(int index = 0; index < max_items; ++index)
            {
                bn::sprite_ptr sprite = bn::sprite_items::item_bread.create_sprite(0, 0);
                sprite.set_visible(false);
                sprite.set_z_order(-15);
                _item_sprites.push_back(sprite);
            }

            refresh_all();
        }

        const bn::sprite_item& player_sprite_item() const
        {
            if(_dir_x < 0)
            {
                return bn::sprite_items::player_left;
            }
            if(_dir_x > 0)
            {
                return bn::sprite_items::player_right;
            }
            if(_dir_y < 0)
            {
                return bn::sprite_items::player_up;
            }
            return bn::sprite_items::player_down;
        }

        void refresh_all()
        {
            refresh_view();
            refresh_entities();
            refresh_player();
            refresh_hud();
        }

        void refresh_player()
        {
            if(! _player_sprites.empty())
            {
                _player_sprites[0].set_item(player_sprite_item());
                _player_sprites[0].set_position(0, 0);
            }
        }

        void refresh_view()
        {
            int index = 0;
            for(int row = 0; row < view_height; ++row)
            {
                for(int col = 0; col < view_width; ++col)
                {
                    int mx = _player_x + col - (view_width / 2);
                    int my = _player_y + row - (view_height / 2);
                    _tile_sprites[index].set_item(tile_item(tile_at(mx, my)));
                    ++index;
                }
            }
        }

        bool relative_to_screen(int map_x, int map_y, int& sx, int& sy) const
        {
            int dx = map_x - _player_x;
            int dy = map_y - _player_y;
            if(dx < -(view_width / 2) || dx > (view_width / 2) || dy < -(view_height / 2) || dy >= (view_height / 2))
            {
                return false;
            }

            sx = dx * tile_px;
            sy = dy * tile_px;
            return true;
        }

        void refresh_entities()
        {
            for(int index = 0; index < max_enemies; ++index)
            {
                if(_enemies[index].alive)
                {
                    int sx = 0;
                    int sy = 0;
                    if(relative_to_screen(_enemies[index].x, _enemies[index].y, sx, sy))
                    {
                        _enemy_sprites[index].set_item(enemy_item(_enemies[index].kind));
                        _enemy_sprites[index].set_position(sx, sy);
                        _enemy_sprites[index].set_visible(true);
                    }
                    else
                    {
                        _enemy_sprites[index].set_visible(false);
                    }
                }
                else
                {
                    _enemy_sprites[index].set_visible(false);
                }
            }

            for(int index = 0; index < max_items; ++index)
            {
                if(_items[index].active)
                {
                    int sx = 0;
                    int sy = 0;
                    if(relative_to_screen(_items[index].x, _items[index].y, sx, sy))
                    {
                        _item_sprites[index].set_item(item_sprite_item(_items[index].kind));
                        _item_sprites[index].set_position(sx, sy);
                        _item_sprites[index].set_visible(true);
                    }
                    else
                    {
                        _item_sprites[index].set_visible(false);
                    }
                }
                else
                {
                    _item_sprites[index].set_visible(false);
                }
            }
        }

        void refresh_hud()
        {
            _text_sprites.clear();
            _text_generator.set_left_alignment();
            bn::string<48> first_line;
            first_line += "HP";
            first_line += bn::to_string<4>(_hp);
            first_line += " L";
            first_line += bn::to_string<4>(_fervor);
            first_line += "/";
            first_line += bn::to_string<4>(current_max_fervor());
            first_line += " M";
            first_line += roman_morada[_morada - 1];
            if(_has_key)
            {
                first_line += " LL";
            }
            _text_generator.generate(-116, -76, first_line, _text_sprites);

            _text_generator.set_right_alignment();
            bn::string<32> second_line;
            second_line += "MEM";
            second_line += bn::to_string<4>(_memory_count);
            second_line += " P";
            second_line += bn::to_string<4>(_mercy_count);
            _text_generator.generate(116, -76, second_line, _text_sprites);

            if(_message_timer > 0)
            {
                _text_generator.set_center_alignment();
                _text_generator.generate(0, 68, _message, _text_sprites);
            }
        }

        void set_message(const char* text, int frames = 90)
        {
            _message = text;
            _message_timer = frames;
            if(_mode == mode::dungeon)
            {
                refresh_hud();
            }
            else if(_mode == mode::boss)
            {
                refresh_boss_text();
            }
        }

        void update_dungeon()
        {
            if(_paused)
            {
                update_pause_overlay();
                update_timers_only();
                return;
            }

            bool acted = false;
            _last_action_silence = false;

            if(bn::keypad::left_pressed())
            {
                acted = try_move_player(-1, 0);
            }
            else if(bn::keypad::right_pressed())
            {
                acted = try_move_player(1, 0);
            }
            else if(bn::keypad::up_pressed())
            {
                acted = try_move_player(0, -1);
            }
            else if(bn::keypad::down_pressed())
            {
                acted = try_move_player(0, 1);
            }
            else if(bn::keypad::a_pressed())
            {
                acted = basic_attack();
            }
            else if(bn::keypad::b_pressed())
            {
                acted = prayer_action();
            }
            else if(bn::keypad::r_pressed())
            {
                acted = silence_action();
            }
            else if(bn::keypad::l_pressed())
            {
                show_memory_or_signal();
            }
            else if(bn::keypad::select_pressed())
            {
                set_message("D andar A golpe B luz", 130);
            }
            else if(bn::keypad::start_pressed())
            {
                _paused = true;
                set_message("PAUSA A seguir B titulo", 600);
            }

#ifdef GBH_DEBUG
            if(bn::keypad::start_pressed() && bn::keypad::l_held() && bn::keypad::r_held())
            {
                set_message("La morada se rehace.", 80);
                generate_dungeon();
                refresh_all();
                acted = false;
            }
#endif

            if(acted)
            {
                ++_turns;
                update_enemies();
                refresh_all();
            }

            update_timers_only();
            check_defeat();
        }

        void update_pause_overlay()
        {
            if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
            {
                _paused = false;
                set_message("Regresa al camino.", 60);
            }
            else if(bn::keypad::b_pressed())
            {
                set_mode(mode::title);
            }
        }

        void update_timers_only()
        {
            if(_message_timer > 0)
            {
                --_message_timer;
                if(_message_timer == 0 && (_mode == mode::dungeon || _mode == mode::boss))
                {
                    if(_mode == mode::dungeon)
                    {
                        refresh_hud();
                    }
                    else
                    {
                        refresh_boss_text();
                    }
                }
            }

            if(_attack_timer > 0)
            {
                --_attack_timer;
                if(_attack_timer == 0)
                {
                    _attack_sprites.clear();
                }
            }
        }

        bool try_move_player(int dx, int dy)
        {
            _dir_x = dx;
            _dir_y = dy;
            _shrine_prayer = 0;
            _silence_chain = 0;
            int next_x = _player_x + dx;
            int next_y = _player_y + dy;

            int enemy_index = enemy_at(next_x, next_y);
            if(enemy_index >= 0)
            {
                return basic_attack();
            }

            tile_kind next_tile = tile_at(next_x, next_y);
            if(next_tile == tile_kind::door_false)
            {
                false_door_trap(next_x, next_y);
                return true;
            }
            if(next_tile == tile_kind::door_silence)
            {
                set_message("R frente al umbral.", 70);
                bn::sound_items::door.play(0.45);
                return true;
            }
            if(! is_walkable(next_x, next_y))
            {
                bn::sound_items::hit.play(0.45);
                set_message("La piedra ensena.", 50);
                return true;
            }
            if(next_tile == tile_kind::exit && ! _has_key)
            {
                bn::sound_items::door.play(0.5);
                set_message("Falta la llave interior.", 80);
                return true;
            }

            _player_x = next_x;
            _player_y = next_y;
            refresh_view();
            pick_item_on_player();

            if(next_tile == tile_kind::shrine)
            {
                set_message("R: orar tres turnos.", 90);
                bn::sound_items::prayer.play(0.6);
            }
            else if(next_tile == tile_kind::silence)
            {
                set_message("Aqui pesa el silencio.", 80);
            }
            else if(next_tile == tile_kind::exit)
            {
                advance_morada();
                return false;
            }

            return true;
        }

        void pick_item_on_player()
        {
            int index = item_at(_player_x, _player_y);
            if(index < 0)
            {
                return;
            }

            item& current_item = _items[index];
            current_item.active = false;
            switch(current_item.kind)
            {
            case item_kind::bread:
                _hp += 2;
                if(_hp > max_hp)
                {
                    _hp = max_hp;
                }
                set_message("Pan para el camino.", 75);
                break;
            case item_kind::candle:
                _fervor += 2;
                if(_fervor > current_max_fervor())
                {
                    _fervor = current_max_fervor();
                }
                set_message("Luz recibida.", 75);
                break;
            case item_kind::key:
                _has_key = true;
                set_message("Llave interior.", 90);
                break;
            case item_kind::memory:
                remember(current_item.memory_index);
                break;
            }

            bn::sound_items::pickup.play(0.8);
        }

        void remember(int memory_index)
        {
            int bit = 1 << memory_index;
            if((_memories_mask & bit) == 0)
            {
                _memories_mask |= bit;
                ++_memory_count;
            }
            _last_memory = memory_index;
            set_message(memory_texts[memory_index], 110);
        }

        bool basic_attack()
        {
            _shrine_prayer = 0;
            _silence_chain = 0;
            int tx = _player_x + _dir_x;
            int ty = _player_y + _dir_y;
            spawn_attack_wave(_dir_x * tile_px, _dir_y * tile_px, 8);
            bn::sound_items::attack.play(0.8);

            int enemy_index = enemy_at(tx, ty);
            if(enemy_index >= 0)
            {
                damage_enemy(enemy_index, 1, false);
                set_message("Resolucion.", 55);
                return true;
            }

            tile_kind target_tile = tile_at(tx, ty);
            if(target_tile == tile_kind::door_false)
            {
                false_door_trap(tx, ty);
                return true;
            }

            set_message("El golpe no discierne.", 45);
            return true;
        }

        bool prayer_action()
        {
            _shrine_prayer = 0;
            _silence_chain = 0;
            if(_fervor < 2)
            {
                set_message("Falta luz.", 60);
                bn::sound_items::hit.play(0.45);
                return false;
            }

            _fervor -= 2;
            spawn_attack_wave(0, 0, 18);
            bn::sound_items::prayer.play(0.9);
            int range = prayer_range();
            int effects = 0;

            for(int index = 0; index < max_enemies; ++index)
            {
                if(_enemies[index].alive && distance_to(_enemies[index].x, _enemies[index].y, _player_x, _player_y) <= range)
                {
                    bool pacify = pilgrim() == pilgrim_kind::charity && _enemies[index].kind != enemy_kind::beast;
                    damage_enemy(index, 1, pacify);
                    ++effects;
                }
            }

            for(int y = _player_y - range; y <= _player_y + range; ++y)
            {
                for(int x = _player_x - range; x <= _player_x + range; ++x)
                {
                    if(inside_map(x, y) && distance_to(x, y, _player_x, _player_y) <= range)
                    {
                        if(_map[y][x] == tile_kind::door_false)
                        {
                            _map[y][x] = tile_kind::wall;
                            ++effects;
                        }
                        else if(_map[y][x] == tile_kind::door_silence)
                        {
                            open_silence_door(x, y);
                            ++effects;
                        }
                    }
                }
            }

            if(effects > 0)
            {
                set_message(pilgrim() == pilgrim_kind::charity ? "Misericordia y luz." : "La luz discierne.", 85);
            }
            else
            {
                give_direction_hint();
            }

            return true;
        }

        bool silence_action()
        {
            _last_action_silence = true;
            ++_silence_chain;

            int tx = _player_x + _dir_x;
            int ty = _player_y + _dir_y;
            if(tile_at(tx, ty) == tile_kind::door_silence)
            {
                open_silence_door(tx, ty);
                set_message("El umbral cede.", 90);
                bn::sound_items::door.play(0.75);
                return true;
            }

            bool safe = ! enemy_adjacent_to_player();
            tile_kind here = tile_at(_player_x, _player_y);
            if(here == tile_kind::shrine)
            {
                ++_shrine_prayer;
                if(_shrine_prayer >= 3)
                {
                    _hp = max_hp;
                    _fervor = current_max_fervor();
                    _map[_player_y][_player_x] = tile_kind::shrine_spent;
                    _shrine_prayer = 0;
                    set_message("Silencio restaurado.", 110);
                    bn::sound_items::prayer.play(0.9);
                }
                else
                {
                    set_message("Permanece.", 70);
                    bn::sound_items::prayer.play(0.55);
                }
                return true;
            }

            if(safe && (_morada >= 4 || here == tile_kind::silence))
            {
                if(_fervor < current_max_fervor())
                {
                    ++_fervor;
                }
                if(_morada == 2 || _morada == 5)
                {
                    give_direction_hint();
                }
                else
                {
                    set_message("Callar da luz.", 70);
                }
            }
            else if(safe)
            {
                give_direction_hint();
            }
            else
            {
                set_message("Hay ruido cerca.", 60);
            }

            return true;
        }

        bool enemy_adjacent_to_player() const
        {
            return enemy_at(_player_x + 1, _player_y) >= 0 || enemy_at(_player_x - 1, _player_y) >= 0 ||
                   enemy_at(_player_x, _player_y + 1) >= 0 || enemy_at(_player_x, _player_y - 1) >= 0;
        }

        void give_direction_hint()
        {
            int target_x = _has_key ? _exit_x : _key_x;
            int target_y = _has_key ? _exit_y : _key_y;
            int dx = target_x - _player_x;
            int dy = target_y - _player_y;
            if(abs_int(dx) > abs_int(dy))
            {
                set_message(dx > 0 ? "La llamada va al ESTE." : "La llamada va al OESTE.", 80);
            }
            else if(dy != 0)
            {
                set_message(dy > 0 ? "La llamada va al SUR." : "La llamada va al NORTE.", 80);
            }
            else
            {
                set_message("La llamada esta aqui.", 80);
            }
        }

        void show_memory_or_signal()
        {
            if(_last_memory >= 0)
            {
                set_message(memory_texts[_last_memory], 130);
            }
            else if(_memory_count == 0)
            {
                set_message("Aun no hay memoria.", 90);
            }
            else
            {
                for(int index = 0; index < 7; ++index)
                {
                    if((_memories_mask & (1 << index)) != 0)
                    {
                        _last_memory = index;
                        set_message(memory_texts[index], 130);
                        return;
                    }
                }
            }
        }

        void open_silence_door(int x, int y)
        {
            constexpr int dirs_x[] = { 1, -1, 0, 0 };
            constexpr int dirs_y[] = { 0, 0, 1, -1 };
            _map[y][x] = tile_kind::door_open;

            for(int dir = 0; dir < 4; ++dir)
            {
                int fx = x + dirs_x[dir];
                int fy = y + dirs_y[dir];
                int bx = x - dirs_x[dir];
                int by = y - dirs_y[dir];
                if(inside_map(fx, fy) && path_walkable(tile_at(fx, fy)) && inside_map(bx, by))
                {
                    carve_room(bx, by, 1, 1);
                    int slot = first_free_item_slot();
                    if(slot >= 0 && ! occupied_by_item(bx, by) && ! occupied_by_enemy(bx, by))
                    {
                        add_item(slot, bx, by, rand_range(2) == 0 ? item_kind::candle : item_kind::bread, _morada - 1);
                    }
                    break;
                }
            }

            refresh_view();
            refresh_entities();
        }

        int first_free_item_slot() const
        {
            for(int index = 0; index < max_items; ++index)
            {
                if(! _items[index].active)
                {
                    return index;
                }
            }
            return -1;
        }

        void false_door_trap(int x, int y)
        {
            _map[y][x] = tile_kind::wall;
            damage_player(1, "La puerta falsa hiere.");
            int slot = first_free_enemy_slot();
            if(slot >= 0)
            {
                int ex = 0;
                int ey = 0;
                if(find_floor_near(_player_x, _player_y, ex, ey, 3))
                {
                    spawn_enemy(slot, ex, ey, _morada >= 5 ? enemy_kind::shadow : enemy_kind::noise, false);
                }
            }
            bn::sound_items::hit.play(0.7);
            refresh_view();
        }

        void spawn_attack_wave(int sx, int sy, int frames)
        {
            _attack_sprites.clear();
            bn::sprite_ptr sprite = bn::sprite_items::attack_wave.create_sprite(sx, sy);
            sprite.set_z_order(-30);
            _attack_sprites.push_back(sprite);
            _attack_timer = frames;
        }

        void damage_enemy(int enemy_index, int amount, bool pacify)
        {
            enemy& target = _enemies[enemy_index];
            target.hp -= amount;
            if(target.hp <= 0)
            {
                target.alive = false;
                if(pacify)
                {
                    ++_mercy_count;
                }
                else if(_fervor < current_max_fervor() && rand_range(3) == 0)
                {
                    ++_fervor;
                }
                bn::sound_items::hit.play(0.7);
            }
        }

        void damage_player(int amount, const char* message)
        {
            _hp -= amount;
            if(_hp <= 0 && pilgrim() == pilgrim_kind::hope && ! _hope_used)
            {
                _hope_used = true;
                _hp = 1;
                set_message("Esperanza levanta.", 100);
                bn::sound_items::prayer.play(0.9);
                return;
            }
            set_message(message, 75);
        }

        void update_enemies()
        {
            for(int index = 0; index < max_enemies; ++index)
            {
                enemy& current_enemy = _enemies[index];
                if(! current_enemy.alive)
                {
                    continue;
                }

                int dx_to_player = _player_x - current_enemy.x;
                int dy_to_player = _player_y - current_enemy.y;
                int distance = abs_int(dx_to_player) + abs_int(dy_to_player);

                if(distance == 1)
                {
                    int damage = current_enemy.kind == enemy_kind::beast ? 2 : 1;
                    damage_player(damage, current_enemy.kind == enemy_kind::beast ? "La bestia golpea." : "La sombra hiere.");
                    bn::sound_items::hit.play(0.65);
                    continue;
                }

                if(current_enemy.kind == enemy_kind::noise && distance <= 3 && _fervor > 0 && (current_enemy.step_counter % 2) == 0)
                {
                    --_fervor;
                    set_message("El ruido apaga luz.", 55);
                    continue;
                }

                ++current_enemy.step_counter;
                int move_period = enemy_move_period(current_enemy);
                if((current_enemy.step_counter % move_period) != 0)
                {
                    continue;
                }

                int mdx = 0;
                int mdy = 0;
                if(current_enemy.kind == enemy_kind::noise && distance < enemy_sense_range())
                {
                    random_direction(mdx, mdy);
                }
                else if(distance < enemy_sense_range() && ! current_enemy.guardian)
                {
                    choose_step_towards(dx_to_player, dy_to_player, mdx, mdy);
                }
                else if(distance < 5 && current_enemy.guardian)
                {
                    choose_step_towards(dx_to_player, dy_to_player, mdx, mdy);
                }
                else if(! current_enemy.guardian)
                {
                    random_direction(mdx, mdy);
                }

                int nx = current_enemy.x + mdx;
                int ny = current_enemy.y + mdy;
                if((mdx != 0 || mdy != 0) && is_walkable(nx, ny) && ! occupied_by_enemy(nx, ny) && !(nx == _player_x && ny == _player_y))
                {
                    if(current_enemy.kind == enemy_kind::shadow && tile_at(nx, ny) == tile_kind::shrine)
                    {
                        continue;
                    }
                    current_enemy.x = nx;
                    current_enemy.y = ny;
                }
            }
        }

        int enemy_move_period(const enemy& current_enemy) const
        {
            switch(current_enemy.kind)
            {
            case enemy_kind::shadow:
                return 1;
            case enemy_kind::haste:
                return _last_action_silence ? 2 : 1;
            case enemy_kind::beast:
                return 3;
            case enemy_kind::noise:
                return 2;
            case enemy_kind::doubt:
            default:
                return 2;
            }
        }

        void random_direction(int& mdx, int& mdy)
        {
            int roll = rand_range(4);
            if(roll == 0)
            {
                mdx = -1;
            }
            else if(roll == 1)
            {
                mdx = 1;
            }
            else if(roll == 2)
            {
                mdy = -1;
            }
            else
            {
                mdy = 1;
            }
        }

        void choose_step_towards(int dx_to_player, int dy_to_player, int& mdx, int& mdy)
        {
            if(abs_int(dx_to_player) > abs_int(dy_to_player))
            {
                mdx = dx_to_player > 0 ? 1 : -1;
            }
            else if(dy_to_player != 0)
            {
                mdy = dy_to_player > 0 ? 1 : -1;
            }
            else if(dx_to_player != 0)
            {
                mdx = dx_to_player > 0 ? 1 : -1;
            }
        }

        void check_defeat()
        {
            if(_hp <= 0)
            {
                bn::sound_items::hit.play(0.8);
                set_mode(mode::defeat);
            }
        }

        void advance_morada()
        {
            bn::sound_items::door.play(0.9);
            if(_morada >= 7)
            {
                set_mode(mode::boss);
                return;
            }

            ++_morada;
            set_mode(mode::lore);
        }

        void enter_boss()
        {
            _boss_marks = 0;
            _boss_turn = 0;
            _message = "No toda puerta es casa.";
            _message_timer = 160;
            _boss_sprites.push_back(bn::sprite_items::boss_false_door.create_sprite(0, -18));
            _boss_sprites[0].set_z_order(-20);
            refresh_boss_text();
            BN_LOG("false door boss reached memories ", _memory_count, " mercy ", _mercy_count);
        }

        bool boss_silence_window() const
        {
            return (_boss_turn % 4) == 2;
        }

        void refresh_boss_text()
        {
            _text_sprites.clear();
            _text_generator.set_center_alignment();
            _text_generator.generate(0, -74, "LA PUERTA FALSA", _text_sprites);

            bn::string<48> status;
            status += "HP";
            status += bn::to_string<4>(_hp);
            status += " L";
            status += bn::to_string<4>(_fervor);
            status += " SELLOS";
            status += bn::to_string<4>(_boss_marks);
            _text_generator.generate(0, 18, status, _text_sprites);

            _text_generator.generate(0, 38, boss_silence_window() ? "VENTANA DE SILENCIO" : "RUIDO: no ataques", _text_sprites);
            _text_generator.generate(0, 54, "R calla  B ora  A hiere", _text_sprites);
            if(_message_timer > 0)
            {
                _text_generator.generate(0, 70, _message, _text_sprites);
            }
        }

        void update_boss()
        {
            bool acted = false;
            if(bn::keypad::r_pressed())
            {
                if(boss_silence_window())
                {
                    ++_boss_marks;
                    set_message("Silencio verdadero.", 90);
                    bn::sound_items::prayer.play(0.85);
                }
                else
                {
                    set_message("Aun hay ruido.", 70);
                }
                acted = true;
            }
            else if(bn::keypad::b_pressed())
            {
                if(_fervor < 2)
                {
                    set_message("Falta luz.", 60);
                }
                else
                {
                    _fervor -= 2;
                    if(boss_silence_window())
                    {
                        ++_boss_marks;
                        set_message("La puerta se vacia.", 90);
                        bn::sound_items::prayer.play(0.9);
                    }
                    else
                    {
                        damage_player(1, "Orar sin oir duele.");
                    }
                }
                acted = true;
            }
            else if(bn::keypad::a_pressed())
            {
                damage_player(1, "Golpear la alimenta.");
                acted = true;
            }
            else if(bn::keypad::l_pressed())
            {
                show_boss_memory();
            }
            else if(bn::keypad::start_pressed())
            {
                set_mode(mode::title);
            }

            if(acted)
            {
                ++_turns;
                ++_boss_turn;
                if(! boss_silence_window() && (_boss_turn % 3) == 0)
                {
                    damage_player(1, "El ruido muerde.");
                }
                if(_boss_marks >= 3)
                {
                    set_mode(mode::victory);
                    return;
                }
                refresh_boss_text();
                check_defeat();
            }

            update_timers_only();
        }

        void show_boss_memory()
        {
            if(_memory_count >= 4)
            {
                set_message("Las memorias reconocen.", 100);
            }
            else
            {
                set_message("Pocas memorias alumbran.", 100);
            }
        }

        void draw_victory()
        {
            _text_sprites.clear();
            _text_generator.set_center_alignment();
            _text_generator.generate(0, -64, "HAS VUELTO A CASA", _text_sprites);
            _text_generator.generate(0, -34, "No estaba lejos.", _text_sprites);
            _text_generator.generate(0, -18, "Era hondo.", _text_sprites);

            bn::string<48> line;
            line += "MEM";
            line += bn::to_string<4>(_memory_count);
            line += " PAZ";
            line += bn::to_string<4>(_mercy_count);
            line += " T";
            line += bn::to_string<8>(_turns);
            _text_generator.generate(0, 14, line, _text_sprites);
            _text_generator.generate(0, 42, "A: otra peregrinacion", _text_sprites);
            _text_generator.generate(0, 58, "B: titulo", _text_sprites);
            BN_LOG("victory turns ", _turns, " memories ", _memory_count, " mercy ", _mercy_count);
        }

        void update_victory()
        {
            if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
            {
                start_new_run();
                set_mode(mode::lore);
            }
            else if(bn::keypad::b_pressed())
            {
                set_mode(mode::title);
            }
        }

        void draw_defeat()
        {
            _text_sprites.clear();
            _text_generator.set_center_alignment();
            _text_generator.generate(0, -58, "NOCHE INTERIOR", _text_sprites);
            _text_generator.generate(0, -28, "Caer no es perderse.", _text_sprites);
            _text_generator.generate(0, -12, defeat_hint(), _text_sprites);
            _text_generator.generate(0, 22, "A: repetir morada", _text_sprites);
            _text_generator.generate(0, 42, "B: titulo", _text_sprites);
            BN_LOG("defeat morada ", _morada, " turns ", _turns);
        }

        const char* defeat_hint() const
        {
            if(_fervor <= 0)
            {
                return "La fuente no se fuerza.";
            }
            if(_morada >= 5)
            {
                return "La prisa apago la lampara.";
            }
            return "Nombrar la sombra ayuda.";
        }

        void update_defeat()
        {
            if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
            {
                _hp = max_hp;
                _fervor = 4 + (pilgrim() == pilgrim_kind::peace ? 1 : 0);
                if(_fervor > current_max_fervor())
                {
                    _fervor = current_max_fervor();
                }
                _has_key = false;
                set_mode(mode::dungeon);
            }
            else if(bn::keypad::b_pressed())
            {
                set_mode(mode::title);
            }
        }
    };
}

int main()
{
    bn::core::init();
    game back_home;

    while(true)
    {
        back_home.update();
        bn::core::update();
    }
}
