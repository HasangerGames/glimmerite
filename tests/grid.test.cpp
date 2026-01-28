#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cstddef>
#include <doctest/doctest.h>

#include "gmi/math/Vec2.h"

#define private public
#include "gmi/math/Grid.h"
#undef private

#include "gmi/math/Shape.h"

using namespace gmi::collision;

struct Entity {
    Rect bounds;
    uint32_t id;
};

TEST_CASE("gmi::collision::Grid tests") {
    Grid<uint32_t, uint32_t> grid(1024, 16, (1 << 16) - 1);

    REQUIRE(grid.worldSize() == 1024);
    REQUIRE(grid.cellSize() == 16);
    REQUIRE(grid.gridSize() == 64);
    REQUIRE(grid.maxEntityID() == 65535);

    for (size_t i = 0; i < grid.maxEntityID(); ++i) {
        INFO(i);
        const auto& cacheEntry = grid.m_entityCache[i];
        REQUIRE_FALSE(cacheEntry.valid);
        REQUIRE(cacheEntry.queryID == 0);
        REQUIRE(cacheEntry.bounds.min == Vec2u{});
        REQUIRE(cacheEntry.bounds.max == Vec2u{});
    }

    for (size_t i = 0; i < grid.m_cellCount; ++i) {
        INFO(i);
        REQUIRE(grid.m_cells[i].items.empty());
    }

    REQUIRE(grid.m_resultCache.empty());

    SUBCASE("Inserts entity correctly") {
        Entity entityA{
            .bounds = {{0, 0}, {20, 20}},
            .id = 9594
        };
        Entity entityB{
            .bounds = {{-10, -10}, {10, 10}},
            .id = 5823
        };

        grid.insertEntity(entityA.id, entityA.bounds.min, entityA.bounds.max);

        const auto& query1 = grid.queryEntity(entityA.id);
        CHECK(query1.size() == 1);
        CHECK(query1[0] == entityA.id);

        grid.insertEntity(entityB.id, entityB.bounds.min, entityB.bounds.max);

        const auto& query2 = grid.queryPosition(Vec2f{});

        CHECK(query2.size() == 2);
        CHECK(std::ranges::find(query2, entityA.id) != query2.cend());
        CHECK(std::ranges::find(query2, entityB.id) != query2.cend());
    }

    SUBCASE("Updates entity correctly") {
        Entity entityA{
            .bounds = {{0, 0}, {20, 20}},
            .id = 9594
        };
        Entity entityB{
            .bounds = {{-10, -10}, {10, 10}},
            .id = 5823
        };

        grid.insertEntity(entityA.id, entityA.bounds.min, entityA.bounds.max);
        grid.insertEntity(entityB.id, entityB.bounds.min, entityB.bounds.max);

        entityA.bounds.translate(Vec2f{-10, -20});
        // translate without insert (update)

        const auto query1 = grid.queryPosition(Vec2f{20, 20});
        CHECK(query1.size() == 1);
        CHECK(query1[0] == entityA.id);

        grid.insertEntity(entityA.id, entityA.bounds.min, entityA.bounds.max);

        const auto query2 = grid.queryPosition(Vec2f{20, 20});
        CHECK(query2.empty());

        const auto query3 = grid.queryPosition(Vec2f{-5, -5});
        CHECK(query3.size() == 2);
        CHECK(std::ranges::find(query3, entityA.id) != query3.cend());
        CHECK(std::ranges::find(query3, entityB.id) != query3.cend());
    }

    SUBCASE("Removes entity correctly") {
        Entity entityA{
            .bounds = {{0, 0}, {20, 20}},
            .id = 9594
        };
        Entity entityB{
            .bounds = {{-10, -10}, {10, 10}},
            .id = 5823
        };

        grid.insertEntity(entityA.id, entityA.bounds.min, entityA.bounds.max);
        grid.insertEntity(entityB.id, entityB.bounds.min, entityB.bounds.max);

        const auto& query1 = grid.queryPosition(Vec2f{});

        CHECK(query1.size() == 2);
        CHECK(std::ranges::find(query1, entityA.id) != query1.cend());
        CHECK(std::ranges::find(query1, entityB.id) != query1.cend());

        grid.removeEntity(entityB.id);

        const auto& query2 = grid.queryAABB(entityA.bounds.min, entityA.bounds.max);

        CHECK(query2.size() == 1);
        CHECK(query2[0] == entityA.id);

        CHECK_NOTHROW(grid.removeEntity(0));
    }

    SUBCASE("queryAABB") {
        Entity entityA{
            .bounds = {{0, 0}, {20, 20}}, // (0, 0); (1, 1)
            .id = 9594
        };
        Entity entityB{
            .bounds = {{20, 20}, {35, 35}}, // (1, 1); (2, 2)
            .id = 5823
        };
        Entity entityC{
            .bounds = {{0, 35}, {30, 50}}, // (0, 2); (1, 4)
            .id = 4082
        };

        grid.insertEntity(entityA.id, entityA.bounds.min, entityA.bounds.max);
        grid.insertEntity(entityB.id, entityB.bounds.min, entityB.bounds.max);
        grid.insertEntity(entityC.id, entityC.bounds.min, entityC.bounds.max);

        const auto& query1 = grid.queryAABB({0, 0}, {0, 0});
        CHECK(query1.size() == 1);
        CHECK(query1[0] == entityA.id);

        const auto& query2 = grid.queryAABB({25, 25}, {30, 30});
        CHECK(query2.size() == 2);
        CHECK(std::ranges::find(query2, entityA.id) != query2.cend());
        CHECK(std::ranges::find(query2, entityB.id) != query2.cend());

        const auto& query3 = grid.queryAABB({0, 50}, {30, 51});
        CHECK(query3.size() == 1);
        CHECK(query3[0] == entityC.id);

        const auto& query4 = grid.queryAABB({50, 50}, {60, 60});
        CHECK(query4.empty());
    }

    SUBCASE("queryPosition") {
        Entity entityA{
            .bounds = {{0, 0}, {20, 20}}, // (0, 0); (1, 1)
            .id = 9594
        };
        Entity entityB{
            .bounds = {{20, 20}, {35, 35}}, // (1, 1); (2, 2)
            .id = 5823
        };
        Entity entityC{
            .bounds = {{0, 35}, {30, 50}}, // (0, 2); (1, 4)
            .id = 4082
        };

        grid.insertEntity(entityA.id, entityA.bounds.min, entityA.bounds.max);
        grid.insertEntity(entityB.id, entityB.bounds.min, entityB.bounds.max);
        grid.insertEntity(entityC.id, entityC.bounds.min, entityC.bounds.max);

        const auto& query1 = grid.queryPosition({5, 5}); // (0, 0)
        CHECK(query1.size() == 1);
        CHECK(query1[0] == entityA.id);

        const auto& query2 = grid.queryPosition({35, 20}); // (2, 1)
        CHECK(query2.size() == 1);
        CHECK(query2[0] == entityB.id);

        const auto& query3 = grid.queryPosition({40, 0}); // (2, 0)
        CHECK(query3.empty());
    }

    SUBCASE("queryEntity") {
        Entity entityA{
            .bounds = {{0, 0}, {20, 20}}, // (0, 0); (1, 1)
            .id = 9594
        };
        Entity entityB{
            .bounds = {{20, 20}, {35, 35}}, // (1, 1); (2, 2)
            .id = 5823
        };
        Entity entityC{
            .bounds = {{0, 35}, {30, 50}}, // (0, 2); (1, 4)
            .id = 4082
        };

        grid.insertEntity(entityA.id, entityA.bounds.min, entityA.bounds.max);
        grid.insertEntity(entityB.id, entityB.bounds.min, entityB.bounds.max);
        grid.insertEntity(entityC.id, entityC.bounds.min, entityC.bounds.max);

        const auto& query1 = grid.queryEntity(entityA.id);
        CHECK(query1.size() == 2);
        CHECK(std::ranges::find(query1, entityA.id) != query1.cend());
        CHECK(std::ranges::find(query1, entityB.id) != query1.cend());

        const auto& query2 = grid.queryEntity(entityB.id);
        CHECK(query2.size() == 3);
        CHECK(std::ranges::find(query2, entityA.id) != query2.cend());
        CHECK(std::ranges::find(query2, entityB.id) != query2.cend());
        CHECK(std::ranges::find(query2, entityC.id) != query2.cend());

        const auto& query3 = grid.queryEntity(entityC.id);
        CHECK(query3.size() == 2);
        CHECK(std::ranges::find(query3, entityB.id) != query3.cend());
        CHECK(std::ranges::find(query3, entityC.id) != query3.cend());
    }

    SUBCASE("queryLine") {
        Entity entityA{
            .bounds = {{0, 0}, {20, 20}}, // (0, 0); (1, 1)
            .id = 9594
        };
        Entity entityB{
            .bounds = {{20, 20}, {35, 35}}, // (1, 1); (2, 2)
            .id = 5823
        };
        Entity entityC{
            .bounds = {{0, 35}, {30, 50}}, // (0, 2); (1, 4)
            .id = 4082
        };

        grid.insertEntity(entityA.id, entityA.bounds.min, entityA.bounds.max);
        grid.insertEntity(entityB.id, entityB.bounds.min, entityB.bounds.max);
        grid.insertEntity(entityC.id, entityC.bounds.min, entityC.bounds.max);

        const auto& query1 = grid.queryLine({0, 0}, { 30, 10 }); // (0, 0); (1, 0)
        CHECK(query1.size() == 1);
        CHECK(query1[0] == entityA.id);

        const auto& query2 = grid.queryLine({0, 40}, { 30, 0 }); // (0, 2); (1, 0)
        CHECK(query2.size() == 3);
        CHECK(std::ranges::find(query2, entityA.id) != query2.cend());
        CHECK(std::ranges::find(query2, entityB.id) != query2.cend());
        CHECK(std::ranges::find(query2, entityC.id) != query2.cend());

        const auto& query3 = grid.queryLine({50, 40}, { 60, 0 }); // (4, 2); (5, 0)
        CHECK(query2.empty());
    }
}
