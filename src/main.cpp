#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_display.h"
#include "bn_bg_palettes.h"
#include "bn_color.h"
#include "bn_log.h"
#include "bn_music.h"
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
#include "bn_sprite_items_tile_shrine.h"
#include "bn_sprite_items_tile_exit.h"
#include "bn_sprite_items_player.h"
#include "bn_sprite_items_enemy_doubt.h"
#include "bn_sprite_items_enemy_shadow.h"
#include "bn_sprite_items_enemy_beast.h"
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
    constexpr int max_enemies = 7;
    constexpr int max_items = 6;
    constexpr int max_hp = 9;
    constexpr int max_fervor = 9;

    enum class mode
    {
        title,
        login,
        lore,
        dungeon,
        victory,
        defeat
    };

    enum class tile_kind : uint8_t
    {
        wall,
        floor,
        door,
        shrine,
        exit
    };

    enum class enemy_kind : uint8_t
    {
        doubt,
        shadow,
        beast
    };

    enum class item_kind : uint8_t
    {
        bread,
        candle,
        key,
        relic
    };

    struct enemy
    {
        int x = 0;
        int y = 0;
        int hp = 0;
        int step_counter = 0;
        enemy_kind kind = enemy_kind::doubt;
        bool alive = false;
    };

    struct item
    {
        int x = 0;
        int y = 0;
        item_kind kind = item_kind::bread;
        bool active = false;
    };

    const bn::sprite_item& tile_item(tile_kind kind)
    {
        switch(kind)
        {
        case tile_kind::wall:
            return bn::sprite_items::tile_wall;
        case tile_kind::door:
            return bn::sprite_items::tile_door;
        case tile_kind::shrine:
            return bn::sprite_items::tile_shrine;
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
        case item_kind::relic:
            return bn::sprite_items::item_relic;
        case item_kind::bread:
        default:
            return bn::sprite_items::item_bread;
        }
    }

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

    constexpr const char* profile_names[] = {
        "FAITH",
        "HOPE",
        "CHARITY",
        "PEACE"
    };

    constexpr const char* dwelling_lore[] = {
        "I  The door is self-knowledge.",
        "II The voice calls from afar.",
        "III Discipline makes the path.",
        "IV Water arrives without noise.",
        "V  The wound learns to trust.",
        "VI Night purifies desire.",
        "VII The house becomes home."
    };

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
            BN_LOG("gba-back-home starter boot");
        }

        void update()
        {
            switch(_mode)
            {
            case mode::title:
                update_title();
                break;
            case mode::login:
                update_login();
                break;
            case mode::lore:
                update_lore();
                break;
            case mode::dungeon:
                update_dungeon();
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
        bn::vector<bn::sprite_ptr, 128> _text_sprites;

        uint32_t _rng = 0x51A7E5ED;
        int _profile_index = 0;
        int _floor = 1;
        int _player_x = map_width / 2;
        int _player_y = map_height / 2;
        int _dir_x = 0;
        int _dir_y = 1;
        int _hp = max_hp;
        int _fervor = 4;
        int _relics = 0;
        int _turns = 0;
        int _attack_timer = 0;
        int _message_timer = 0;
        bool _has_key = false;
        const char* _message = "Seek the inner door.";

        void clear_scene()
        {
            _tile_sprites.clear();
            _enemy_sprites.clear();
            _item_sprites.clear();
            _player_sprites.clear();
            _attack_sprites.clear();
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
            case mode::login:
                draw_login();
                break;
            case mode::lore:
                draw_lore();
                break;
            case mode::dungeon:
                enter_dungeon_floor();
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
            _text_generator.generate(0, -46, "Road to the Father", _text_sprites);
            _text_generator.generate(0, -20, "Procedural dungeon crawler", _text_sprites);
            _text_generator.generate(0, 4, "A: enter", _text_sprites);
            _text_generator.generate(0, 18, "B: quick seed", _text_sprites);
            _text_generator.generate(0, 48, "START: new pilgrim", _text_sprites);
        }

        void update_title()
        {
            if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
            {
                bn::sound_items::pickup.play(0.8);
                set_mode(mode::login);
            }
            else if(bn::keypad::b_pressed())
            {
                _profile_index = 3;
                _rng ^= 0x00C0FFEE;
                bn::sound_items::pickup.play(0.8);
                set_mode(mode::lore);
            }
        }

        void draw_login()
        {
            _text_sprites.clear();
            _text_generator.set_center_alignment();
            _text_generator.generate(0, -64, "PILGRIM ACCESS", _text_sprites);
            _text_generator.generate(0, -42, "Choose a local key", _text_sprites);
            _text_generator.generate(0, -8, profile_names[_profile_index], _text_sprites);
            _text_generator.generate(0, 20, "LEFT/RIGHT changes", _text_sprites);
            _text_generator.generate(0, 34, "A confirms", _text_sprites);
            _text_generator.generate(0, 58, "Offline: ROM profile", _text_sprites);
        }

        void update_login()
        {
            if(bn::keypad::left_pressed())
            {
                _profile_index = (_profile_index + 3) % 4;
                bn::sound_items::pickup.play(0.7);
                draw_login();
            }
            else if(bn::keypad::right_pressed())
            {
                _profile_index = (_profile_index + 1) % 4;
                bn::sound_items::pickup.play(0.7);
                draw_login();
            }
            else if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
            {
                _rng = 0xA7C00100u + uint32_t(_profile_index * 0x1F123BB5u);
                _floor = 1;
                _hp = max_hp;
                _fervor = 4;
                _relics = 0;
                _turns = 0;
                _has_key = false;
                bn::sound_items::door.play(0.9);
                set_mode(mode::lore);
            }
        }

        void draw_lore()
        {
            _text_sprites.clear();
            _text_generator.set_center_alignment();
            _text_generator.generate(0, -66, "INNER CASTLE", _text_sprites);
            _text_generator.generate(0, -42, profile_names[_profile_index], _text_sprites);
            _text_generator.generate(0, -14, dwelling_lore[_floor - 1], _text_sprites);
            _text_generator.generate(0, 16, "Each room clears a shadow.", _text_sprites);
            _text_generator.generate(0, 34, "A: descend", _text_sprites);
            _text_generator.generate(0, 54, "B: return", _text_sprites);
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

        uint32_t rand_u32()
        {
            _rng = (_rng * 1664525u) + 1013904223u;
            return _rng;
        }

        int rand_range(int limit)
        {
            return int((rand_u32() >> 16) % uint32_t(limit));
        }

        tile_kind tile_at(int x, int y) const
        {
            if(! inside_map(x, y))
            {
                return tile_kind::wall;
            }

            return _map[y][x];
        }

        bool is_walkable(int x, int y) const
        {
            tile_kind tile = tile_at(x, y);
            return tile == tile_kind::floor || tile == tile_kind::door || tile == tile_kind::shrine || tile == tile_kind::exit;
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

        void carve_room(int cx, int cy, int radius)
        {
            for(int y = cy - radius; y <= cy + radius; ++y)
            {
                for(int x = cx - radius; x <= cx + radius; ++x)
                {
                    if(x > 0 && x < map_width - 1 && y > 0 && y < map_height - 1)
                    {
                        _map[y][x] = tile_kind::floor;
                    }
                }
            }
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

        bool random_floor_cell(int& out_x, int& out_y)
        {
            for(int attempt = 0; attempt < 240; ++attempt)
            {
                int x = 1 + rand_range(map_width - 2);
                int y = 1 + rand_range(map_height - 2);
                if(_map[y][x] == tile_kind::floor && !(x == _player_x && y == _player_y) && ! occupied_by_enemy(x, y) && ! occupied_by_item(x, y))
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
            _player_x = map_width / 2;
            _player_y = map_height / 2;
            int cx = _player_x;
            int cy = _player_y;
            _map[cy][cx] = tile_kind::floor;

            int walk_steps = 360 + (_floor * 34);
            for(int step = 0; step < walk_steps; ++step)
            {
                int dir = rand_range(4);
                if(dir == 0)
                {
                    --cx;
                }
                else if(dir == 1)
                {
                    ++cx;
                }
                else if(dir == 2)
                {
                    --cy;
                }
                else
                {
                    ++cy;
                }

                cx = clamp_int(cx, 1, map_width - 2);
                cy = clamp_int(cy, 1, map_height - 2);
                _map[cy][cx] = tile_kind::floor;

                if((step % 53) == 0)
                {
                    carve_room(cx, cy, 1 + rand_range(2));
                }
            }

            _map[_player_y][_player_x] = tile_kind::floor;

            int exit_x = _player_x;
            int exit_y = _player_y;
            int best_distance = -1;
            for(int y = 1; y < map_height - 1; ++y)
            {
                for(int x = 1; x < map_width - 1; ++x)
                {
                    if(_map[y][x] == tile_kind::floor)
                    {
                        int distance = abs_int(x - _player_x) + abs_int(y - _player_y);
                        if(distance > best_distance)
                        {
                            best_distance = distance;
                            exit_x = x;
                            exit_y = y;
                        }
                    }
                }
            }

            _map[exit_y][exit_x] = tile_kind::exit;

            int sx = 0;
            int sy = 0;
            if(random_floor_cell(sx, sy))
            {
                _map[sy][sx] = tile_kind::shrine;
            }

            for(int door_index = 0; door_index < 2 + (_floor / 2); ++door_index)
            {
                int dx = 0;
                int dy = 0;
                if(random_floor_cell(dx, dy))
                {
                    _map[dy][dx] = tile_kind::door;
                }
            }

            int item_count = 4 + (_floor > 3 ? 1 : 0);
            for(int index = 0; index < item_count && index < max_items; ++index)
            {
                int ix = 0;
                int iy = 0;
                if(random_floor_cell(ix, iy))
                {
                    _items[index].x = ix;
                    _items[index].y = iy;
                    _items[index].active = true;
                    if(index == 0)
                    {
                        _items[index].kind = item_kind::key;
                    }
                    else if(index == 1 || rand_range(3) == 0)
                    {
                        _items[index].kind = item_kind::candle;
                    }
                    else if(rand_range(4) == 0)
                    {
                        _items[index].kind = item_kind::relic;
                    }
                    else
                    {
                        _items[index].kind = item_kind::bread;
                    }
                }
            }

            int enemy_count = 3 + _floor;
            if(enemy_count > max_enemies)
            {
                enemy_count = max_enemies;
            }

            for(int index = 0; index < enemy_count; ++index)
            {
                int ex = 0;
                int ey = 0;
                if(random_floor_cell(ex, ey))
                {
                    _enemies[index].x = ex;
                    _enemies[index].y = ey;
                    _enemies[index].alive = true;
                    _enemies[index].step_counter = rand_range(20);
                    int type_roll = rand_range(10);
                    if(_floor >= 5 && type_roll > 6)
                    {
                        _enemies[index].kind = enemy_kind::beast;
                        _enemies[index].hp = 3;
                    }
                    else if(_floor >= 3 && type_roll > 4)
                    {
                        _enemies[index].kind = enemy_kind::shadow;
                        _enemies[index].hp = 2;
                    }
                    else
                    {
                        _enemies[index].kind = enemy_kind::doubt;
                        _enemies[index].hp = 1;
                    }
                }
            }
        }

        void enter_dungeon_floor()
        {
            _has_key = false;
            _message = dwelling_lore[_floor - 1];
            _message_timer = 150;
            _attack_timer = 0;
            generate_dungeon();

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

            _player_sprites.push_back(bn::sprite_items::player.create_sprite(0, 0));
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

            refresh_view();
            refresh_entities();
            refresh_hud();
            BN_LOG("generated floor ", _floor, " seed ", int(_rng));
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
            bn::string<64> first_line;
            first_line += "HP:";
            first_line += bn::to_string<4>(_hp);
            first_line += " F:";
            first_line += bn::to_string<4>(_fervor);
            first_line += " M:";
            first_line += bn::to_string<4>(_floor);
            if(_has_key)
            {
                first_line += " KEY";
            }
            _text_generator.generate(-116, -76, first_line, _text_sprites);

            _text_generator.set_right_alignment();
            bn::string<32> second_line;
            second_line += "R:";
            second_line += bn::to_string<4>(_relics);
            second_line += " T:";
            second_line += bn::to_string<8>(_turns);
            _text_generator.generate(116, -76, second_line, _text_sprites);

            if(_message_timer > 0)
            {
                _text_generator.set_center_alignment();
                _text_generator.generate(0, 68, _message, _text_sprites);
            }
        }

        void set_message(const char* text, int frames = 120)
        {
            _message = text;
            _message_timer = frames;
            refresh_hud();
        }

        void update_dungeon()
        {
            bool acted = false;

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
                basic_attack();
                acted = true;
            }
            else if(bn::keypad::b_pressed())
            {
                prayer_attack();
                acted = true;
            }
            else if(bn::keypad::start_pressed())
            {
                set_message("The dwelling reforms.", 80);
                generate_dungeon();
                refresh_view();
                refresh_entities();
                acted = true;
            }

            if(acted)
            {
                ++_turns;
                update_enemies();
                refresh_entities();
                refresh_hud();
            }

            if(_message_timer > 0)
            {
                --_message_timer;
                if(_message_timer == 0)
                {
                    refresh_hud();
                }
            }

            if(_attack_timer > 0)
            {
                --_attack_timer;
                if(!_attack_sprites.empty())
                {
                    _attack_sprites[0].set_rotation_angle(_attack_sprites[0].rotation_angle() + 9);
                }
                if(_attack_timer == 0)
                {
                    _attack_sprites.clear();
                }
            }

            if(_hp <= 0)
            {
                bn::sound_items::hit.play(0.8);
                set_mode(mode::defeat);
            }
        }

        bool try_move_player(int dx, int dy)
        {
            _dir_x = dx;
            _dir_y = dy;
            int next_x = _player_x + dx;
            int next_y = _player_y + dy;

            if(occupied_by_enemy(next_x, next_y))
            {
                basic_attack();
                return true;
            }

            if(! is_walkable(next_x, next_y))
            {
                bn::sound_items::hit.play(0.45);
                set_message("Closed stone.", 50);
                return true;
            }

            tile_kind next_tile = tile_at(next_x, next_y);
            if(next_tile == tile_kind::exit && !_has_key)
            {
                bn::sound_items::door.play(0.5);
                set_message("The key is missing.", 70);
                return true;
            }

            _player_x = next_x;
            _player_y = next_y;
            refresh_view();
            pick_item_on_player();

            if(next_tile == tile_kind::shrine)
            {
                _hp = max_hp;
                _fervor = max_fervor;
                set_message("Silence: life restored.", 90);
                bn::sound_items::prayer.play(0.8);
            }
            else if(next_tile == tile_kind::exit)
            {
                advance_floor();
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
                set_message("Bread for the road.", 75);
                break;
            case item_kind::candle:
                _fervor += 2;
                if(_fervor > max_fervor)
                {
                    _fervor = max_fervor;
                }
                set_message("Light received.", 75);
                break;
            case item_kind::key:
                _has_key = true;
                set_message("Inner key found.", 90);
                break;
            case item_kind::relic:
                ++_relics;
                set_message("Memory of grace.", 90);
                break;
            }

            bn::sound_items::pickup.play(0.8);
        }

        void basic_attack()
        {
            int tx = _player_x + _dir_x;
            int ty = _player_y + _dir_y;
            int enemy_index = enemy_at(tx, ty);
            spawn_attack_wave(_dir_x * tile_px, _dir_y * tile_px, 8);
            bn::sound_items::attack.play(0.8);

            if(enemy_index >= 0)
            {
                damage_enemy(enemy_index, 1);
                set_message("Strike of resolve.", 55);
            }
            else
            {
                set_message("The strike cuts shadow.", 40);
            }
        }

        void prayer_attack()
        {
            if(_fervor < 2)
            {
                set_message("Not enough fervor.", 60);
                bn::sound_items::hit.play(0.45);
                return;
            }

            _fervor -= 2;
            spawn_attack_wave(0, 0, 18);
            bn::sound_items::prayer.play(0.9);
            int hits = 0;
            for(int index = 0; index < max_enemies; ++index)
            {
                if(_enemies[index].alive)
                {
                    int distance = abs_int(_enemies[index].x - _player_x) + abs_int(_enemies[index].y - _player_y);
                    if(distance <= 2)
                    {
                        damage_enemy(index, 1);
                        ++hits;
                    }
                }
            }

            if(hits > 0)
            {
                set_message("Expanding prayer.", 70);
            }
            else
            {
                set_message("Prayer prepares.", 60);
            }
        }

        void spawn_attack_wave(int sx, int sy, int frames)
        {
            _attack_sprites.clear();
            bn::sprite_ptr sprite = bn::sprite_items::attack_wave.create_sprite(sx, sy);
            sprite.set_z_order(-30);
            _attack_sprites.push_back(sprite);
            _attack_timer = frames;
        }

        void damage_enemy(int enemy_index, int amount)
        {
            enemy& target = _enemies[enemy_index];
            target.hp -= amount;
            if(target.hp <= 0)
            {
                target.alive = false;
                if(_fervor < max_fervor && rand_range(3) == 0)
                {
                    ++_fervor;
                }
                bn::sound_items::hit.play(0.7);
            }
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
                    _hp -= damage;
                    set_message("The shadow wounds.", 60);
                    bn::sound_items::hit.play(0.65);
                    continue;
                }

                ++current_enemy.step_counter;
                int move_period = current_enemy.kind == enemy_kind::shadow ? 1 : 2;
                if((current_enemy.step_counter % move_period) != 0)
                {
                    continue;
                }

                int mdx = 0;
                int mdy = 0;
                if(distance < 7)
                {
                    if(abs_int(dx_to_player) > abs_int(dy_to_player))
                    {
                        mdx = dx_to_player > 0 ? 1 : -1;
                    }
                    else if(dy_to_player != 0)
                    {
                        mdy = dy_to_player > 0 ? 1 : -1;
                    }
                }
                else
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

                int nx = current_enemy.x + mdx;
                int ny = current_enemy.y + mdy;
                if((mdx != 0 || mdy != 0) && is_walkable(nx, ny) && ! occupied_by_enemy(nx, ny) && !(nx == _player_x && ny == _player_y))
                {
                    current_enemy.x = nx;
                    current_enemy.y = ny;
                }
            }
        }

        void advance_floor()
        {
            bn::sound_items::door.play(0.9);
            if(_floor >= 7)
            {
                set_mode(mode::victory);
                return;
            }

            ++_floor;
            set_mode(mode::lore);
        }

        void draw_victory()
        {
            _text_sprites.clear();
            _text_generator.set_center_alignment();
            _text_generator.generate(0, -58, "YOU HAVE COME HOME", _text_sprites);
            _text_generator.generate(0, -26, "The door was not far.", _text_sprites);
            _text_generator.generate(0, -10, "It was deep.", _text_sprites);
            _text_generator.generate(0, 20, "START: another pilgrimage", _text_sprites);
            _text_generator.generate(0, 42, "B: title", _text_sprites);
            BN_LOG("victory turns ", _turns, " relics ", _relics);
        }

        void update_victory()
        {
            if(bn::keypad::start_pressed() || bn::keypad::a_pressed())
            {
                _floor = 1;
                _hp = max_hp;
                _fervor = 4;
                _relics = 0;
                _turns = 0;
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
            _text_generator.generate(0, -58, "INNER NIGHT", _text_sprites);
            _text_generator.generate(0, -26, "Falling is not being lost.", _text_sprites);
            _text_generator.generate(0, -10, "Call again.", _text_sprites);
            _text_generator.generate(0, 24, "A: retry dwelling", _text_sprites);
            _text_generator.generate(0, 44, "B: title", _text_sprites);
            BN_LOG("defeat floor ", _floor, " turns ", _turns);
        }

        void update_defeat()
        {
            if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
            {
                _hp = max_hp;
                _fervor = 4;
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
