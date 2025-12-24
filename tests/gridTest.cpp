#include "gmi/math/Grid.h"
#include "gmi/math/Shape.h"

struct Entity {
    gmi::collision::Rect bounds;
    uint32_t id;
};

int main() {
    // some really small tests to make sure shit works ig

    gmi::collision::Grid<uint32_t, uint32_t> grid(1024, 16, (1 << 16) - 1);

    Entity entityA{
        .bounds = {{0, 0}, {20, 20}},
        .id = 9594
    };

    grid.insertEntity(entityA.id, entityA.bounds.min, entityA.bounds.max);

    const auto& result1 = grid.queryEntity(entityA.id);

    assert(result1.size() == 1);
    assert(result1[0] == entityA.id);

    const auto& result2 = grid.queryAABB({0, 0}, {10, 10});
    assert(result2.size() == 1);
    assert(result2[0] == entityA.id);

    const auto& result3 = grid.queryAABB({90, 90}, {100, 100});
    assert(result3.size() == 0);

    // negative positions should be clamped and still work
    const auto& result4 = grid.queryAABB({-90, -90}, {100, 100});
    assert(result4.size() == 1);
    assert(result4[0] == entityA.id);

    Entity entityB{
        .bounds = gmi::collision::Rect::fromDims(5, 5, {grid.worldSize() / 2, grid.worldSize() / 2}),
        .id = 5324
    };
    grid.insertEntity(entityB.id, entityB.bounds.min, entityB.bounds.max);

    const auto& result5 = grid.queryLine({grid.worldSize(), 0}, {0, grid.worldSize()});
    assert(result5.size() == 1);
    assert(result5[0] == entityB.id);

    return 0;
}
