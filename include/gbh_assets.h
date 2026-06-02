#ifndef GBH_ASSETS_H
#define GBH_ASSETS_H

#include "gbh_game_types.h"
#include "bn_sprite_item.h"

namespace gbh
{
    const bn::sprite_item& tile_item(TileKind kind);
    const bn::sprite_item& enemy_item(EnemyKind kind);
    const bn::sprite_item& item_sprite_item(ItemKind kind);
    const bn::sprite_item& player_item(int dir_x, int dir_y);
    const bn::sprite_item& attack_wave_item();
    const bn::sprite_item& boss_false_door_item();
}

#endif
