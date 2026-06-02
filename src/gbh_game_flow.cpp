#include "gbh_game.h"

#include "bn_bg_palettes.h"
#include "bn_color.h"
#include "bn_keypad.h"
#include "bn_log.h"
#include "bn_music_items.h"
#include "bn_sound_items.h"
#include "common_fixed_8x8_sprite_font.h"

namespace gbh
{

Game::Game() :
    _text_generator(common::fixed_8x8_sprite_font)
{
    bn::bg_palettes::set_transparent_color(bn::color(2, 1, 4));
    _text_generator.set_center_alignment();
    bn::music_items::pilgrimage.play(0.45);
    set_mode(Mode::title);
    BN_LOG("gba-back-home design slice boot");
}

void Game::update()
{
    switch(_mode)
    {
    case Mode::title:
        update_title();
        break;
    case Mode::pilgrim:
        update_pilgrim();
        break;
    case Mode::lore:
        update_lore();
        break;
    case Mode::dungeon:
        update_dungeon();
        break;
    case Mode::boss:
        update_boss();
        break;
    case Mode::victory:
        update_victory();
        break;
    case Mode::defeat:
        update_defeat();
        break;
    }
}

PilgrimKind Game::pilgrim() const
{
    return PilgrimKind(_pilgrim_index);
}

const char* Game::localize(const LocalizedText& value) const
{
    return gbh::localize(_language, value);
}

const char* Game::text(TextId id) const
{
    return gbh::text(_language, id);
}

void Game::toggle_language()
{
    _language = _language == Language::spanish ? Language::english : Language::spanish;
    bn::sound_items::pickup.play(0.65);
}

int Game::current_max_fervor() const
{
    return max_fervor + (pilgrim() == PilgrimKind::faith ? 1 : 0);
}

int Game::prayer_range() const
{
    return pilgrim() == PilgrimKind::faith ? 3 : 2;
}

int Game::enemy_sense_range() const
{
    int range = 6 + (_dwelling >= 6 ? 1 : 0);
    if(pilgrim() == PilgrimKind::peace)
    {
        --range;
    }
    return range;
}

uint32_t Game::rand_u32()
{
    _rng = (_rng * 1664525u) + 1013904223u;
    return _rng;
}

int Game::rand_range(int limit)
{
    return int((rand_u32() >> 16) % uint32_t(limit));
}

void Game::clear_scene()
{
    _tile_sprites.clear();
    _enemy_sprites.clear();
    _item_sprites.clear();
    _player_sprites.clear();
    _attack_sprites.clear();
    _boss_sprites.clear();
    _text_sprites.clear();
}

void Game::set_mode(Mode next_mode)
{
    clear_scene();
    _mode = next_mode;

    switch(next_mode)
    {
    case Mode::title:
        draw_title();
        break;
    case Mode::pilgrim:
        draw_pilgrim();
        break;
    case Mode::lore:
        draw_lore();
        break;
    case Mode::dungeon:
        enter_dungeon_dwelling();
        break;
    case Mode::boss:
        enter_boss();
        break;
    case Mode::victory:
        draw_victory();
        break;
    case Mode::defeat:
        draw_defeat();
        break;
    }
}

void Game::draw_title()
{
    _text_sprites.clear();
    _text_generator.set_center_alignment();
    _text_generator.generate(0, -64, "GBA BACK HOME", _text_sprites);
    _text_generator.generate(0, -46, text(TextId::title_subtitle), _text_sprites);
    _text_generator.generate(0, -18, text(TextId::title_genre), _text_sprites);
    _text_generator.generate(0, 8, text(TextId::title_start), _text_sprites);
    _text_generator.generate(0, 24, text(TextId::title_peace_seed), _text_sprites);
    _text_generator.generate(0, 52, text(TextId::title_language), _text_sprites);
}

void Game::update_title()
{
    if(bn::keypad::select_pressed())
    {
        toggle_language();
        draw_title();
    }
    else if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
    {
        bn::sound_items::pickup.play(0.8);
        set_mode(Mode::pilgrim);
    }
    else if(bn::keypad::b_pressed())
    {
        _pilgrim_index = int(PilgrimKind::peace);
        start_new_run();
        set_mode(Mode::lore);
    }
}

void Game::draw_pilgrim()
{
    _text_sprites.clear();
    _text_generator.set_center_alignment();
    _text_generator.generate(0, -66, text(TextId::pilgrim_title), _text_sprites);
    _text_generator.generate(0, -40, localize(pilgrim_names[_pilgrim_index]), _text_sprites);
    _text_generator.generate(0, -18, localize(pilgrim_rules[_pilgrim_index]), _text_sprites);
    _text_generator.generate(0, 20, text(TextId::pilgrim_change), _text_sprites);
    _text_generator.generate(0, 36, text(TextId::pilgrim_confirm), _text_sprites);
    _text_generator.generate(0, 56, text(TextId::title_language), _text_sprites);
}

void Game::update_pilgrim()
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
        set_mode(Mode::lore);
    }
    else if(bn::keypad::b_pressed())
    {
        set_mode(Mode::title);
    }
    else if(bn::keypad::select_pressed())
    {
        toggle_language();
        draw_pilgrim();
    }
}

void Game::start_new_run()
{
    _rng = 0xA7C00100u + uint32_t(_pilgrim_index * 0x1F123BB5u);
    _dwelling = 1;
    _hp = max_hp;
    _fervor = 4 + (pilgrim() == PilgrimKind::peace ? 1 : 0);
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

void Game::draw_lore()
{
    _text_sprites.clear();
    _text_generator.set_center_alignment();
    _text_generator.generate(0, -68, text(TextId::lore_title), _text_sprites);
    _text_generator.generate(0, -46, localize(pilgrim_names[_pilgrim_index]), _text_sprites);
    _text_generator.generate(0, -22, localize(dwelling_lore[_dwelling - 1]), _text_sprites);
    _text_generator.generate(0, -4, localize(dwelling_rules[_dwelling - 1]), _text_sprites);
    _text_generator.generate(0, 28, text(TextId::action_enter), _text_sprites);
    _text_generator.generate(0, 44, text(TextId::action_title), _text_sprites);
    _text_generator.generate(0, 60, text(TextId::lore_controls), _text_sprites);
}

void Game::update_lore()
{
    if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
    {
        bn::sound_items::door.play(0.8);
        set_mode(Mode::dungeon);
    }
    else if(bn::keypad::b_pressed())
    {
        set_mode(Mode::title);
    }
    else if(bn::keypad::select_pressed())
    {
        toggle_language();
        draw_lore();
    }
}

void Game::check_defeat()
{
    if(_hp <= 0)
    {
        bn::sound_items::hit.play(0.8);
        set_mode(Mode::defeat);
    }
}

void Game::advance_dwelling()
{
    bn::sound_items::door.play(0.9);
    if(_dwelling >= 7)
    {
        set_mode(Mode::boss);
        return;
    }

    ++_dwelling;
    set_mode(Mode::lore);
}
}
