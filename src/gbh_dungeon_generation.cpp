#include "gbh_game.h"

#include "bn_log.h"

namespace gbh
{

TileKind Game::tile_at(int x, int y) const
{
    if(! inside_map(x, y))
    {
        return TileKind::wall;
    }

    return _map[y][x];
}

bool Game::path_walkable(TileKind tile) const
{
    return tile == TileKind::floor || tile == TileKind::silence || tile == TileKind::door_open ||
           tile == TileKind::shrine || tile == TileKind::shrine_spent || tile == TileKind::exit;
}

bool Game::is_walkable(int x, int y) const
{
    return path_walkable(tile_at(x, y));
}

bool Game::occupied_by_enemy(int x, int y) const
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

bool Game::occupied_by_item(int x, int y) const
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

int Game::enemy_at(int x, int y) const
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

int Game::item_at(int x, int y) const
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

void Game::reset_entities()
{
    for(int index = 0; index < max_enemies; ++index)
    {
        _enemies[index] = Enemy();
    }

    for(int index = 0; index < max_items; ++index)
    {
        _items[index] = Item();
    }
}

void Game::carve_room(int cx, int cy, int rx, int ry)
{
    for(int y = cy - ry; y <= cy + ry; ++y)
    {
        for(int x = cx - rx; x <= cx + rx; ++x)
        {
            if(x > 0 && x < map_width - 1 && y > 0 && y < map_height - 1)
            {
                _map[y][x] = TileKind::floor;
            }
        }
    }
}

void Game::carve_corridor(int ax, int ay, int bx, int by)
{
    int x = ax;
    int y = ay;
    while(x != bx)
    {
        _map[y][x] = TileKind::floor;
        x += bx > x ? 1 : -1;
    }
    while(y != by)
    {
        _map[y][x] = TileKind::floor;
        y += by > y ? 1 : -1;
    }
    _map[y][x] = TileKind::floor;
}

void Game::compute_distances(int start_x, int start_y, int dist[map_height][map_width]) const
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

bool Game::random_floor_cell(int& out_x, int& out_y, int min_start_distance)
{
    for(int attempt = 0; attempt < 260; ++attempt)
    {
        int x = 1 + rand_range(map_width - 2);
        int y = 1 + rand_range(map_height - 2);
        if(_map[y][x] == TileKind::floor && distance_to(x, y, _player_x, _player_y) >= min_start_distance &&
           ! occupied_by_enemy(x, y) && ! occupied_by_item(x, y))
        {
            out_x = x;
            out_y = y;
            return true;
        }
    }

    return false;
}

bool Game::random_wall_next_to_floor(int& out_x, int& out_y)
{
    constexpr int dirs_x[] = { 1, -1, 0, 0 };
    constexpr int dirs_y[] = { 0, 0, 1, -1 };

    for(int attempt = 0; attempt < 260; ++attempt)
    {
        int x = 2 + rand_range(map_width - 4);
        int y = 2 + rand_range(map_height - 4);
        if(_map[y][x] != TileKind::wall)
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

void Game::generate_dungeon()
{
    for(int y = 0; y < map_height; ++y)
    {
        for(int x = 0; x < map_width; ++x)
        {
            _map[y][x] = TileKind::wall;
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
    int critical_rooms = 6 + _dwelling;
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

    int branch_count = 3 + (_dwelling / 2);
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
            if(_map[y][x] == TileKind::floor && dist_from_start[y][x] > best_exit)
            {
                best_exit = dist_from_start[y][x];
                _exit_x = x;
                _exit_y = y;
            }
        }
    }

    _map[_exit_y][_exit_x] = TileKind::exit;

    int dist_from_exit[map_height][map_width];
    compute_distances(_exit_x, _exit_y, dist_from_exit);
    _key_x = _player_x;
    _key_y = _player_y;
    int best_key_score = -1;
    for(int y = 1; y < map_height - 1; ++y)
    {
        for(int x = 1; x < map_width - 1; ++x)
        {
            if(_map[y][x] == TileKind::floor && dist_from_start[y][x] >= 7 && dist_from_exit[y][x] >= 5)
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
                if(_map[y][x] == TileKind::floor && dist_from_start[y][x] > fallback_best && !(x == _exit_x && y == _exit_y))
                {
                    fallback_best = dist_from_start[y][x];
                    _key_x = x;
                    _key_y = y;
                }
            }
        }
    }

    add_item(0, _key_x, _key_y, ItemKind::key, _dwelling - 1);

    int sx = 0;
    int sy = 0;
    if(random_floor_cell(sx, sy, 5))
    {
        _map[sy][sx] = TileKind::shrine;
    }

    int silence_tiles = _dwelling >= 4 ? 3 : 1;
    for(int index = 0; index < silence_tiles; ++index)
    {
        int tx = 0;
        int ty = 0;
        if(random_floor_cell(tx, ty, 4))
        {
            _map[ty][tx] = TileKind::silence;
        }
    }

    int memory_x = 0;
    int memory_y = 0;
    if(random_floor_cell(memory_x, memory_y, 6))
    {
        add_item(1, memory_x, memory_y, ItemKind::memory, _dwelling - 1);
    }

    int resource_count = 3 + (_dwelling <= 2 ? 1 : 0) - (_dwelling >= 6 ? 1 : 0);
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
            ItemKind kind = (index % 2 == 0 || _dwelling >= 4) ? ItemKind::candle : ItemKind::bread;
            add_item(index + 2, ix, iy, kind, _dwelling - 1);
        }
    }

    int false_doors = (_dwelling >= 5 ? 4 : 1 + (_dwelling / 3));
    for(int index = 0; index < false_doors; ++index)
    {
        int dx = 0;
        int dy = 0;
        if(random_wall_next_to_floor(dx, dy))
        {
            _map[dy][dx] = TileKind::door_false;
        }
    }

    int silence_doors = (_dwelling >= 4 ? 2 : 1);
    for(int index = 0; index < silence_doors; ++index)
    {
        int dx = 0;
        int dy = 0;
        if(random_wall_next_to_floor(dx, dy))
        {
            _map[dy][dx] = TileKind::door_silence;
        }
    }

    int enemy_count = 2 + _dwelling;
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
            EnemyKind kind = choose_enemy_kind(index);
            spawn_enemy(index, ex, ey, kind, false);
        }
    }

    if(_dwelling >= 6)
    {
        int gx = 0;
        int gy = 0;
        if(find_floor_near(_exit_x, _exit_y, gx, gy, 4))
        {
            int slot = first_free_enemy_slot();
            if(slot >= 0)
            {
                spawn_enemy(slot, gx, gy, EnemyKind::beast, true);
            }
        }
    }
}

void Game::add_item(int slot, int x, int y, ItemKind kind, int memory_index)
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

EnemyKind Game::choose_enemy_kind(int index)
{
    int roll = rand_range(12);
    if(_dwelling >= 6 && (roll >= 9 || index == 0))
    {
        return EnemyKind::beast;
    }
    if(_dwelling >= 5 && roll >= 7)
    {
        return EnemyKind::haste;
    }
    if(_dwelling >= 4 && roll == 0)
    {
        return EnemyKind::noise;
    }
    if(_dwelling >= 3 && roll >= 5)
    {
        return EnemyKind::shadow;
    }
    return EnemyKind::doubt;
}

void Game::spawn_enemy(int slot, int x, int y, EnemyKind kind, bool guardian)
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
    case EnemyKind::beast:
        _enemies[slot].hp = 3;
        break;
    case EnemyKind::shadow:
    case EnemyKind::haste:
        _enemies[slot].hp = 2;
        break;
    case EnemyKind::noise:
    case EnemyKind::doubt:
    default:
        _enemies[slot].hp = 1;
        break;
    }
}

int Game::first_free_enemy_slot() const
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

bool Game::find_floor_near(int target_x, int target_y, int& out_x, int& out_y, int radius)
{
    for(int attempt = 0; attempt < 180; ++attempt)
    {
        int x = clamp_int(target_x - radius + rand_range((radius * 2) + 1), 1, map_width - 2);
        int y = clamp_int(target_y - radius + rand_range((radius * 2) + 1), 1, map_height - 2);
        if(_map[y][x] == TileKind::floor && ! occupied_by_enemy(x, y) && ! occupied_by_item(x, y) &&
           distance_to(x, y, _player_x, _player_y) >= 5)
        {
            out_x = x;
            out_y = y;
            return true;
        }
    }
    return random_floor_cell(out_x, out_y, 6);
}

void Game::enter_dungeon_dwelling()
{
    _message = localize(dwelling_lore[_dwelling - 1]);
    _message_timer = 120;
    generate_dungeon();
    create_dungeon_sprites();
    BN_LOG("dwelling ", _dwelling, " seed ", int(_rng));
}
}
