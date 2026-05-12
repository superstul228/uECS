#include <iostream>
#include "uecs.hpp"
struct Position { int x, y; };
struct Health { int current, max; };
struct PlayerTag {};
int main() {
    uecs::Registry reg;
    
    auto player = reg.Create();

    reg.Add<Position>(player, {10, 20});
    reg.Add<Health>(player, {100, 100});
    reg.Add<PlayerTag>(player, {});

    auto* pos = reg.Get<Position>(player);
    auto* hp = reg.Get<Health>(player);
    
    if (pos && hp) {
        std::cout << "Player at (" << pos->x << ", " << pos->y << ")\n";
        std::cout << "Health: " << hp->current << "/" << hp->max << "\n";
    }
    
    if (hp) {
        hp->current -= 30;
        std::cout << "After damage: " << hp->current << "/" << hp->max << "\n";
    }	
    if (!reg.Get<Health>(player)) {
        std::cout << "Player destroyed\n";
    }
    
    return 0;
}
