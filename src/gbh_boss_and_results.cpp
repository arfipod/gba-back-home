#include "gbh_game.h"

#include "gbh_assets.h"
#include "bn_keypad.h"
#include "bn_log.h"
#include "bn_sound_items.h"
#include "bn_string.h"

namespace gbh
{

void Game::enter_boss()
{
    _boss_marks = 0;
    _boss_turn = 0;
    _message = text(TextId::boss_intro);
    _message_timer = 160;
    _boss_sprites.push_back(boss_false_door_item().create_sprite(0, -18));
    _boss_sprites[0].set_z_order(-20);
    refresh_boss_text();
    BN_LOG("false door boss reached memories ", _memory_count, " mercy ", _mercy_count);
}

bool Game::boss_silence_window() const
{
    return (_boss_turn % 4) == 2;
}

void Game::refresh_boss_text()
{
    _text_sprites.clear();
    _text_generator.set_center_alignment();
    _text_generator.generate(0, -74, text(TextId::boss_title), _text_sprites);

    bn::string<48> status;
    status += "HP";
    status += bn::to_string<4>(_hp);
    status += " L";
    status += bn::to_string<4>(_fervor);
    status += text(TextId::boss_seals_label);
    status += bn::to_string<4>(_boss_marks);
    _text_generator.generate(0, 18, status, _text_sprites);

    _text_generator.generate(0, 38,
                             boss_silence_window() ? text(TextId::boss_silence_window) : text(TextId::boss_noise_window),
                             _text_sprites);
    _text_generator.generate(0, 54, text(TextId::boss_controls), _text_sprites);
    if(_message_timer > 0)
    {
        _text_generator.generate(0, 70, _message, _text_sprites);
    }
}

void Game::update_boss()
{
    bool acted = false;
    if(bn::keypad::r_pressed())
    {
        if(boss_silence_window())
        {
            ++_boss_marks;
            set_message(text(TextId::true_silence), 90);
            bn::sound_items::prayer.play(0.85);
        }
        else
        {
            set_message(text(TextId::still_noise), 70);
        }
        acted = true;
    }
    else if(bn::keypad::b_pressed())
    {
        if(_fervor < 2)
        {
            set_message(text(TextId::no_light), 60);
        }
        else
        {
            _fervor -= 2;
            if(boss_silence_window())
            {
                ++_boss_marks;
                set_message(text(TextId::door_empties), 90);
                bn::sound_items::prayer.play(0.9);
            }
            else
            {
                damage_player(1, text(TextId::prayer_hurts));
            }
        }
        acted = true;
    }
    else if(bn::keypad::a_pressed())
    {
        damage_player(1, text(TextId::hit_feeds));
        acted = true;
    }
    else if(bn::keypad::l_pressed())
    {
        show_boss_memory();
    }
    else if(bn::keypad::start_pressed())
    {
        set_mode(Mode::title);
    }

    if(acted)
    {
        ++_turns;
        ++_boss_turn;
        if(! boss_silence_window() && (_boss_turn % 3) == 0)
        {
            damage_player(1, text(TextId::noise_bites));
        }
        if(_boss_marks >= 3)
        {
            set_mode(Mode::victory);
            return;
        }
        refresh_boss_text();
        check_defeat();
    }

    update_timers_only();
}

void Game::show_boss_memory()
{
    if(_memory_count >= 4)
    {
        set_message(text(TextId::boss_memory_good), 100);
    }
    else
    {
        set_message(text(TextId::boss_memory_low), 100);
    }
}

void Game::draw_victory()
{
    _text_sprites.clear();
    _text_generator.set_center_alignment();
    _text_generator.generate(0, -64, text(TextId::victory_title), _text_sprites);
    _text_generator.generate(0, -34, localize(memory_texts[0]), _text_sprites);
    _text_generator.generate(0, -18, localize(memory_texts[6]), _text_sprites);

    bn::string<48> line;
    line += "MEM";
    line += bn::to_string<4>(_memory_count);
    line += text(TextId::hud_mercy_label);
    line += bn::to_string<4>(_mercy_count);
    line += " T";
    line += bn::to_string<8>(_turns);
    _text_generator.generate(0, 14, line, _text_sprites);
    _text_generator.generate(0, 42, text(TextId::victory_restart), _text_sprites);
    _text_generator.generate(0, 58, text(TextId::action_title), _text_sprites);
    BN_LOG("victory turns ", _turns, " memories ", _memory_count, " mercy ", _mercy_count);
}

void Game::update_victory()
{
    if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
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
        draw_victory();
    }
}

void Game::draw_defeat()
{
    _text_sprites.clear();
    _text_generator.set_center_alignment();
    _text_generator.generate(0, -58, text(TextId::defeat_title), _text_sprites);
    _text_generator.generate(0, -28, text(TextId::defeat_fall), _text_sprites);
    _text_generator.generate(0, -12, defeat_hint(), _text_sprites);
    _text_generator.generate(0, 22, text(TextId::defeat_retry), _text_sprites);
    _text_generator.generate(0, 42, text(TextId::action_title), _text_sprites);
    BN_LOG("defeat dwelling ", _dwelling, " turns ", _turns);
}

const char* Game::defeat_hint() const
{
    if(_fervor <= 0)
    {
        return text(TextId::defeat_hint_no_light);
    }
    if(_dwelling >= 5)
    {
        return text(TextId::defeat_hint_haste);
    }
    return text(TextId::defeat_hint_shadow);
}

void Game::update_defeat()
{
    if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
    {
        _hp = max_hp;
        _fervor = 4 + (pilgrim() == PilgrimKind::peace ? 1 : 0);
        if(_fervor > current_max_fervor())
        {
            _fervor = current_max_fervor();
        }
        _has_key = false;
        set_mode(Mode::dungeon);
    }
    else if(bn::keypad::b_pressed())
    {
        set_mode(Mode::title);
    }
    else if(bn::keypad::select_pressed())
    {
        toggle_language();
        draw_defeat();
    }
}
}
