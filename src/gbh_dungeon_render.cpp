#include "gbh_game.h"

#include "gbh_assets.h"
#include "bn_string.h"

namespace gbh
{

void Game::create_dungeon_sprites()
{
    _tile_sprites.clear();
    for(int row = 0; row < view_height; ++row)
    {
        for(int col = 0; col < view_width; ++col)
        {
            int sx = (col - (view_width / 2)) * tile_px;
            int sy = (row - (view_height / 2)) * tile_px;
            bn::sprite_ptr sprite = tile_item(TileKind::floor).create_sprite(sx, sy);
            sprite.set_z_order(40);
            _tile_sprites.push_back(sprite);
        }
    }

    _player_sprites.push_back(player_item(0, 1).create_sprite(0, 0));
    _player_sprites[0].set_z_order(-20);

    _enemy_sprites.clear();
    for(int index = 0; index < max_enemies; ++index)
    {
        bn::sprite_ptr sprite = enemy_item(EnemyKind::doubt).create_sprite(0, 0);
        sprite.set_visible(false);
        sprite.set_z_order(-10);
        _enemy_sprites.push_back(sprite);
    }

    _item_sprites.clear();
    for(int index = 0; index < max_items; ++index)
    {
        bn::sprite_ptr sprite = item_sprite_item(ItemKind::bread).create_sprite(0, 0);
        sprite.set_visible(false);
        sprite.set_z_order(-15);
        _item_sprites.push_back(sprite);
    }

    refresh_all();
}

const bn::sprite_item& Game::player_sprite_item() const
{
    return player_item(_dir_x, _dir_y);
}

void Game::refresh_all()
{
    refresh_view();
    refresh_entities();
    refresh_player();
    refresh_hud();
}

void Game::refresh_player()
{
    if(! _player_sprites.empty())
    {
        _player_sprites[0].set_item(player_sprite_item());
        _player_sprites[0].set_position(0, 0);
    }
}

void Game::refresh_view()
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

bool Game::relative_to_screen(int map_x, int map_y, int& sx, int& sy) const
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

void Game::refresh_entities()
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

void Game::refresh_hud()
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
    first_line += text(TextId::hud_dwelling_label);
    first_line += roman_dwelling[_dwelling - 1];
    if(_has_key)
    {
        first_line += text(TextId::hud_key_marker);
    }
    _text_generator.generate(-116, -76, first_line, _text_sprites);

    _text_generator.set_right_alignment();
    bn::string<32> second_line;
    second_line += "MEM";
    second_line += bn::to_string<4>(_memory_count);
    second_line += text(TextId::hud_mercy_label);
    second_line += bn::to_string<4>(_mercy_count);
    _text_generator.generate(116, -76, second_line, _text_sprites);

    if(_message_timer > 0)
    {
        _text_generator.set_center_alignment();
        _text_generator.generate(0, 68, _message, _text_sprites);
    }
}

void Game::set_message(const char* text, int frames)
{
    _message = text;
    _message_timer = frames;
    if(_mode == Mode::dungeon)
    {
        refresh_hud();
    }
    else if(_mode == Mode::boss)
    {
        refresh_boss_text();
    }
}

void Game::update_timers_only()
{
    if(_message_timer > 0)
    {
        --_message_timer;
        if(_message_timer == 0 && (_mode == Mode::dungeon || _mode == Mode::boss))
        {
            if(_mode == Mode::dungeon)
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

void Game::spawn_attack_wave(int sx, int sy, int frames)
{
    _attack_sprites.clear();
    bn::sprite_ptr sprite = attack_wave_item().create_sprite(sx, sy);
    sprite.set_z_order(-30);
    _attack_sprites.push_back(sprite);
    _attack_timer = frames;
}
}
