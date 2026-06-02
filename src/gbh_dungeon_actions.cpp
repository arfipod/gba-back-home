#include "gbh_game.h"

#include "gbh_assets.h"
#include "bn_keypad.h"
#include "bn_sound_items.h"

namespace gbh
{

void Game::update_dungeon()
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
        set_message(text(TextId::help_controls), 130);
    }
    else if(bn::keypad::start_pressed())
    {
        _paused = true;
        set_message(text(TextId::pause), 600);
    }

#ifdef GBH_DEBUG
    if(bn::keypad::start_pressed() && bn::keypad::l_held() && bn::keypad::r_held())
    {
        set_message(text(TextId::dungeon_regenerated), 80);
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

void Game::update_pause_overlay()
{
    if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
    {
        _paused = false;
        set_message(text(TextId::resume_path), 60);
    }
    else if(bn::keypad::b_pressed())
    {
        set_mode(Mode::title);
    }
}

bool Game::try_move_player(int dx, int dy)
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

    TileKind next_tile = tile_at(next_x, next_y);
    if(next_tile == TileKind::door_false)
    {
        false_door_trap(next_x, next_y);
        return true;
    }
    if(next_tile == TileKind::door_silence)
    {
        set_message(text(TextId::silence_threshold_hint), 70);
        bn::sound_items::door.play(0.45);
        return true;
    }
    if(! is_walkable(next_x, next_y))
    {
        bn::sound_items::hit.play(0.45);
        set_message(text(TextId::wall_teaches), 50);
        return true;
    }
    if(next_tile == TileKind::exit && ! _has_key)
    {
        bn::sound_items::door.play(0.5);
        set_message(text(TextId::missing_key), 80);
        return true;
    }

    _player_x = next_x;
    _player_y = next_y;
    refresh_view();
    pick_item_on_player();

    if(next_tile == TileKind::shrine)
    {
        set_message(text(TextId::shrine_prompt), 90);
        bn::sound_items::prayer.play(0.6);
    }
    else if(next_tile == TileKind::silence)
    {
        set_message(text(TextId::silence_heavy), 80);
    }
    else if(next_tile == TileKind::exit)
    {
        advance_dwelling();
        return false;
    }

    return true;
}

void Game::pick_item_on_player()
{
    int index = item_at(_player_x, _player_y);
    if(index < 0)
    {
        return;
    }

    Item& current_item = _items[index];
    current_item.active = false;
    switch(current_item.kind)
    {
    case ItemKind::bread:
        _hp += 2;
        if(_hp > max_hp)
        {
            _hp = max_hp;
        }
        set_message(text(TextId::bread), 75);
        break;
    case ItemKind::candle:
        _fervor += 2;
        if(_fervor > current_max_fervor())
        {
            _fervor = current_max_fervor();
        }
        set_message(text(TextId::candle), 75);
        break;
    case ItemKind::key:
        _has_key = true;
        set_message(text(TextId::inner_key), 90);
        break;
    case ItemKind::memory:
        remember(current_item.memory_index);
        break;
    }

    bn::sound_items::pickup.play(0.8);
}

void Game::remember(int memory_index)
{
    int bit = 1 << memory_index;
    if((_memories_mask & bit) == 0)
    {
        _memories_mask |= bit;
        ++_memory_count;
    }
    _last_memory = memory_index;
    set_message(localize(memory_texts[memory_index]), 110);
}

bool Game::basic_attack()
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
        set_message(text(TextId::resolution), 55);
        return true;
    }

    TileKind target_tile = tile_at(tx, ty);
    if(target_tile == TileKind::door_false)
    {
        false_door_trap(tx, ty);
        return true;
    }

    set_message(text(TextId::strike_no_discern), 45);
    return true;
}

bool Game::prayer_action()
{
    _shrine_prayer = 0;
    _silence_chain = 0;
    if(_fervor < 2)
    {
        set_message(text(TextId::no_light), 60);
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
            bool pacify = pilgrim() == PilgrimKind::charity && _enemies[index].kind != EnemyKind::beast;
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
                if(_map[y][x] == TileKind::door_false)
                {
                    _map[y][x] = TileKind::wall;
                    ++effects;
                }
                else if(_map[y][x] == TileKind::door_silence)
                {
                    open_silence_door(x, y);
                    ++effects;
                }
            }
        }
    }

    if(effects > 0)
    {
        set_message(pilgrim() == PilgrimKind::charity ? text(TextId::mercy_light) : text(TextId::light_discerns), 85);
    }
    else
    {
        give_direction_hint();
    }

    return true;
}

bool Game::silence_action()
{
    _last_action_silence = true;
    ++_silence_chain;

    int tx = _player_x + _dir_x;
    int ty = _player_y + _dir_y;
    if(tile_at(tx, ty) == TileKind::door_silence)
    {
        open_silence_door(tx, ty);
        set_message(text(TextId::threshold_yields), 90);
        bn::sound_items::door.play(0.75);
        return true;
    }

    bool safe = ! enemy_adjacent_to_player();
    TileKind here = tile_at(_player_x, _player_y);
    if(here == TileKind::shrine)
    {
        ++_shrine_prayer;
        if(_shrine_prayer >= 3)
        {
            _hp = max_hp;
            _fervor = current_max_fervor();
            _map[_player_y][_player_x] = TileKind::shrine_spent;
            _shrine_prayer = 0;
            set_message(text(TextId::shrine_restored), 110);
            bn::sound_items::prayer.play(0.9);
        }
        else
        {
            set_message(text(TextId::remain), 70);
            bn::sound_items::prayer.play(0.55);
        }
        return true;
    }

    if(safe && (_dwelling >= 4 || here == TileKind::silence))
    {
        if(_fervor < current_max_fervor())
        {
            ++_fervor;
        }
        if(_dwelling == 2 || _dwelling == 5)
        {
            give_direction_hint();
        }
        else
        {
            set_message(text(TextId::silence_light), 70);
        }
    }
    else if(safe)
    {
        give_direction_hint();
    }
    else
    {
        set_message(text(TextId::noise_near), 60);
    }

    return true;
}

bool Game::enemy_adjacent_to_player() const
{
    return enemy_at(_player_x + 1, _player_y) >= 0 || enemy_at(_player_x - 1, _player_y) >= 0 ||
           enemy_at(_player_x, _player_y + 1) >= 0 || enemy_at(_player_x, _player_y - 1) >= 0;
}

void Game::give_direction_hint()
{
    int target_x = _has_key ? _exit_x : _key_x;
    int target_y = _has_key ? _exit_y : _key_y;
    int dx = target_x - _player_x;
    int dy = target_y - _player_y;
    if(abs_int(dx) > abs_int(dy))
    {
        set_message(dx > 0 ? text(TextId::call_east) : text(TextId::call_west), 80);
    }
    else if(dy != 0)
    {
        set_message(dy > 0 ? text(TextId::call_south) : text(TextId::call_north), 80);
    }
    else
    {
        set_message(text(TextId::call_here), 80);
    }
}

void Game::show_memory_or_signal()
{
    if(_last_memory >= 0)
    {
        set_message(localize(memory_texts[_last_memory]), 130);
    }
    else if(_memory_count == 0)
    {
        set_message(text(TextId::no_memory), 90);
    }
    else
    {
        for(int index = 0; index < 7; ++index)
        {
            if((_memories_mask & (1 << index)) != 0)
            {
                _last_memory = index;
                set_message(localize(memory_texts[index]), 130);
                return;
            }
        }
    }
}

void Game::open_silence_door(int x, int y)
{
    constexpr int dirs_x[] = { 1, -1, 0, 0 };
    constexpr int dirs_y[] = { 0, 0, 1, -1 };
    _map[y][x] = TileKind::door_open;

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
                add_item(slot, bx, by, rand_range(2) == 0 ? ItemKind::candle : ItemKind::bread, _dwelling - 1);
            }
            break;
        }
    }

    refresh_view();
    refresh_entities();
}

int Game::first_free_item_slot() const
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

void Game::false_door_trap(int x, int y)
{
    _map[y][x] = TileKind::wall;
    damage_player(1, text(TextId::false_door_hurts));
    int slot = first_free_enemy_slot();
    if(slot >= 0)
    {
        int ex = 0;
        int ey = 0;
        if(find_floor_near(_player_x, _player_y, ex, ey, 3))
        {
            spawn_enemy(slot, ex, ey, _dwelling >= 5 ? EnemyKind::shadow : EnemyKind::noise, false);
        }
    }
    bn::sound_items::hit.play(0.7);
    refresh_view();
}

void Game::damage_enemy(int enemy_index, int amount, bool pacify)
{
    Enemy& target = _enemies[enemy_index];
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

void Game::damage_player(int amount, const char* message)
{
    _hp -= amount;
    if(_hp <= 0 && pilgrim() == PilgrimKind::hope && ! _hope_used)
    {
        _hope_used = true;
        _hp = 1;
        set_message(text(TextId::hope_rises), 100);
        bn::sound_items::prayer.play(0.9);
        return;
    }
    set_message(message, 75);
}

void Game::update_enemies()
{
    for(int index = 0; index < max_enemies; ++index)
    {
        Enemy& current_enemy = _enemies[index];
        if(! current_enemy.alive)
        {
            continue;
        }

        int dx_to_player = _player_x - current_enemy.x;
        int dy_to_player = _player_y - current_enemy.y;
        int distance = abs_int(dx_to_player) + abs_int(dy_to_player);

        if(distance == 1)
        {
            int damage = current_enemy.kind == EnemyKind::beast ? 2 : 1;
            damage_player(damage, current_enemy.kind == EnemyKind::beast ? text(TextId::beast_hits) : text(TextId::shadow_hurts));
            bn::sound_items::hit.play(0.65);
            continue;
        }

        if(current_enemy.kind == EnemyKind::noise && distance <= 3 && _fervor > 0 && (current_enemy.step_counter % 2) == 0)
        {
            --_fervor;
            set_message(text(TextId::noise_drains), 55);
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
        if(current_enemy.kind == EnemyKind::noise && distance < enemy_sense_range())
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
            if(current_enemy.kind == EnemyKind::shadow && tile_at(nx, ny) == TileKind::shrine)
            {
                continue;
            }
            current_enemy.x = nx;
            current_enemy.y = ny;
        }
    }
}

int Game::enemy_move_period(const Enemy& current_enemy) const
{
    switch(current_enemy.kind)
    {
    case EnemyKind::shadow:
        return 1;
    case EnemyKind::haste:
        return _last_action_silence ? 2 : 1;
    case EnemyKind::beast:
        return 3;
    case EnemyKind::noise:
        return 2;
    case EnemyKind::doubt:
    default:
        return 2;
    }
}

void Game::random_direction(int& mdx, int& mdy)
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

void Game::choose_step_towards(int dx_to_player, int dy_to_player, int& mdx, int& mdy)
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
}
