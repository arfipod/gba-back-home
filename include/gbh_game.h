#ifndef GBH_GAME_H
#define GBH_GAME_H

#include "gbh_game_types.h"
#include "gbh_localization.h"

#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include <cstdint>

namespace gbh
{
    class Game
    {
    public:
        Game();
        void update();

    private:
        Mode _mode = Mode::title;
        TileKind _map[map_height][map_width] = {};
        Enemy _enemies[max_enemies];
        Item _items[max_items];

        bn::sprite_text_generator _text_generator;
        bn::vector<bn::sprite_ptr, view_width * view_height> _tile_sprites;
        bn::vector<bn::sprite_ptr, max_enemies> _enemy_sprites;
        bn::vector<bn::sprite_ptr, max_items> _item_sprites;
        bn::vector<bn::sprite_ptr, 1> _player_sprites;
        bn::vector<bn::sprite_ptr, 1> _attack_sprites;
        bn::vector<bn::sprite_ptr, 1> _boss_sprites;
        bn::vector<bn::sprite_ptr, 128> _text_sprites;

        Language _language = Language::spanish;
        uint32_t _rng = 0x51A7E5ED;
        int _pilgrim_index = 0;
        int _dwelling = 1;
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
        const char* _message = text_table[int(TextId::seek_threshold)].es;

        PilgrimKind pilgrim() const;
        const char* localize(const LocalizedText& value) const;
        const char* text(TextId id) const;
        void toggle_language();
        int current_max_fervor() const;
        int prayer_range() const;
        int enemy_sense_range() const;
        uint32_t rand_u32();
        int rand_range(int limit);
        void clear_scene();
        void set_mode(Mode next_mode);
        void draw_title();
        void update_title();
        void draw_pilgrim();
        void update_pilgrim();
        void start_new_run();
        void draw_lore();
        void update_lore();
        TileKind tile_at(int x, int y) const;
        bool path_walkable(TileKind tile) const;
        bool is_walkable(int x, int y) const;
        bool occupied_by_enemy(int x, int y) const;
        bool occupied_by_item(int x, int y) const;
        int enemy_at(int x, int y) const;
        int item_at(int x, int y) const;
        void reset_entities();
        void carve_room(int cx, int cy, int rx, int ry);
        void carve_corridor(int ax, int ay, int bx, int by);
        void compute_distances(int start_x, int start_y, int dist[map_height][map_width]) const;
        bool random_floor_cell(int& out_x, int& out_y, int min_start_distance);
        bool random_wall_next_to_floor(int& out_x, int& out_y);
        void generate_dungeon();
        void add_item(int slot, int x, int y, ItemKind kind, int memory_index);
        EnemyKind choose_enemy_kind(int index);
        void spawn_enemy(int slot, int x, int y, EnemyKind kind, bool guardian);
        int first_free_enemy_slot() const;
        bool find_floor_near(int target_x, int target_y, int& out_x, int& out_y, int radius);
        void enter_dungeon_dwelling();
        void create_dungeon_sprites();
        const bn::sprite_item& player_sprite_item() const;
        void refresh_all();
        void refresh_player();
        void refresh_view();
        bool relative_to_screen(int map_x, int map_y, int& sx, int& sy) const;
        void refresh_entities();
        void refresh_hud();
        void set_message(const char* text, int frames = 90);
        void update_dungeon();
        void update_pause_overlay();
        void update_timers_only();
        bool try_move_player(int dx, int dy);
        void pick_item_on_player();
        void remember(int memory_index);
        bool basic_attack();
        bool prayer_action();
        bool silence_action();
        bool enemy_adjacent_to_player() const;
        void give_direction_hint();
        void show_memory_or_signal();
        void open_silence_door(int x, int y);
        int first_free_item_slot() const;
        void false_door_trap(int x, int y);
        void spawn_attack_wave(int sx, int sy, int frames);
        void damage_enemy(int enemy_index, int amount, bool pacify);
        void damage_player(int amount, const char* message);
        void update_enemies();
        int enemy_move_period(const Enemy& current_enemy) const;
        void random_direction(int& mdx, int& mdy);
        void choose_step_towards(int dx_to_player, int dy_to_player, int& mdx, int& mdy);
        void check_defeat();
        void advance_dwelling();
        void enter_boss();
        bool boss_silence_window() const;
        void refresh_boss_text();
        void update_boss();
        void show_boss_memory();
        void draw_victory();
        void update_victory();
        void draw_defeat();
        const char* defeat_hint() const;
        void update_defeat();
    };
}

#endif
