#include "bn_core.h"
#include "gbh_game.h"

int main()
{
    bn::core::init();
    gbh::Game back_home;

    while(true)
    {
        back_home.update();
        bn::core::update();
    }
}
