#include "gbh_assets.h"

#include "bn_sprite_items_attack_wave.h"
#include "bn_sprite_items_boss_false_door.h"
#include "bn_sprite_items_enemy_beast.h"
#include "bn_sprite_items_enemy_doubt.h"
#include "bn_sprite_items_enemy_haste.h"
#include "bn_sprite_items_enemy_noise.h"
#include "bn_sprite_items_enemy_shadow.h"
#include "bn_sprite_items_item_bread.h"
#include "bn_sprite_items_item_candle.h"
#include "bn_sprite_items_item_key.h"
#include "bn_sprite_items_item_relic.h"
#include "bn_sprite_items_player_down.h"
#include "bn_sprite_items_player_left.h"
#include "bn_sprite_items_player_right.h"
#include "bn_sprite_items_player_up.h"
#include "bn_sprite_items_tile_door.h"
#include "bn_sprite_items_tile_door_false.h"
#include "bn_sprite_items_tile_door_silence.h"
#include "bn_sprite_items_tile_exit.h"
#include "bn_sprite_items_tile_floor.h"
#include "bn_sprite_items_tile_shrine.h"
#include "bn_sprite_items_tile_shrine_spent.h"
#include "bn_sprite_items_tile_silence.h"
#include "bn_sprite_items_tile_wall.h"

namespace gbh
{
    const bn::sprite_item& tile_item(TileKind kind)
    {
        switch(kind)
        {
        case TileKind::wall:
            return bn::sprite_items::tile_wall;
        case TileKind::silence:
            return bn::sprite_items::tile_silence;
        case TileKind::door_open:
            return bn::sprite_items::tile_door;
        case TileKind::door_false:
            return bn::sprite_items::tile_door_false;
        case TileKind::door_silence:
            return bn::sprite_items::tile_door_silence;
        case TileKind::shrine:
            return bn::sprite_items::tile_shrine;
        case TileKind::shrine_spent:
            return bn::sprite_items::tile_shrine_spent;
        case TileKind::exit:
            return bn::sprite_items::tile_exit;
        case TileKind::floor:
        default:
            return bn::sprite_items::tile_floor;
        }
    }

    const bn::sprite_item& enemy_item(EnemyKind kind)
    {
        switch(kind)
        {
        case EnemyKind::shadow:
            return bn::sprite_items::enemy_shadow;
        case EnemyKind::beast:
            return bn::sprite_items::enemy_beast;
        case EnemyKind::noise:
            return bn::sprite_items::enemy_noise;
        case EnemyKind::haste:
            return bn::sprite_items::enemy_haste;
        case EnemyKind::doubt:
        default:
            return bn::sprite_items::enemy_doubt;
        }
    }

    const bn::sprite_item& item_sprite_item(ItemKind kind)
    {
        switch(kind)
        {
        case ItemKind::candle:
            return bn::sprite_items::item_candle;
        case ItemKind::key:
            return bn::sprite_items::item_key;
        case ItemKind::memory:
            return bn::sprite_items::item_relic;
        case ItemKind::bread:
        default:
            return bn::sprite_items::item_bread;
        }
    }

    const bn::sprite_item& player_item(int dir_x, int dir_y)
    {
        if(dir_x < 0)
        {
            return bn::sprite_items::player_left;
        }
        if(dir_x > 0)
        {
            return bn::sprite_items::player_right;
        }
        if(dir_y < 0)
        {
            return bn::sprite_items::player_up;
        }
        return bn::sprite_items::player_down;
    }

    const bn::sprite_item& attack_wave_item()
    {
        return bn::sprite_items::attack_wave;
    }

    const bn::sprite_item& boss_false_door_item()
    {
        return bn::sprite_items::boss_false_door;
    }
}
