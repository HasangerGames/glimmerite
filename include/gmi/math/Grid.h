#pragma once

#include "gmi/math/Vec2.h"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <type_traits>
#include <vector>

namespace gmi::collision {

using namespace gmi::math;

template<typename GridSize_T, typename EntityID_T>
concept GridC = std::is_unsigned_v<GridSize_T> && std::is_unsigned_v<EntityID_T>;

template<typename GridSize_T, typename EntityID_T>
    requires(GridC<GridSize_T, EntityID_T>)
class Grid {
public:
    Grid(GridSize_T worldSize, GridSize_T cellSize, EntityID_T maxEntityID);

    Grid(const Grid&) = delete;
    Grid(Grid&&) = delete;
    Grid& operator=(const Grid&) = delete;
    Grid& operator=(Grid&&) = delete;

    ~Grid();

    [[nodiscard]] float worldSize() const {
        return m_worldSize;
    }

    [[nodiscard]] GridSize_T cellSize() const {
        return m_cellSize;
    }

    [[nodiscard]] GridSize_T gridSize() const {
        return m_gridSize;
    }

    [[nodiscard]] EntityID_T maxEntityID() const {
        return m_maxEntityID;
    }

    void insertEntity(EntityID_T entityID, Vec2f min, Vec2f max);

    void removeEntity(EntityID_T entityID);

    const std::vector<EntityID_T>& queryAABB(Vec2f min, Vec2f max) const;

    const std::vector<EntityID_T>& queryPosition(Vec2f pos) const;

    const std::vector<EntityID_T>& queryEntity(EntityID_T entityID) const;

    const std::vector<EntityID_T>& queryLine(Vec2f lineStart, Vec2f lineEnd) const;

private:
    struct Cell {
        std::vector<EntityID_T> items;
    };

    using GridPos = Vec2<GridSize_T>;

    template<typename T>
    struct AABB {
        T min;
        T max;
    };

    using GridAABB = AABB<GridPos>;
    using WorldAABB = AABB<Vec2f>;

    struct EntityGridData {
        bool valid = false;
        uint32_t queryID;
        GridAABB bounds;
    };

    GridPos roundToGrid(Vec2f pos) const {
        return {
            (GridSize_T)std::clamp((int)(pos.x / m_cellSize), 0, (int)m_gridSize - 1),
            (GridSize_T)std::clamp((int)(pos.y / m_cellSize), 0, (int)m_gridSize - 1),
        };
    }

    const std::vector<EntityID_T>& queryGridAABB(GridAABB bounds) const {
        m_resultCache.clear();

        uint32_t query = getNextQueryID();

        for (GridSize_T y = bounds.min.y; y <= bounds.max.y; y++) {
            for (GridSize_T x = bounds.min.x; x <= bounds.max.x; x++) {
                Cell& cell = cellAt(x, y);

                for (EntityID_T entityId : cell.items) {
                    EntityGridData& entity = getEntityData(entityId);

                    if (entity.queryID != query) {
                        entity.queryID = query;
                        m_resultCache.push_back(entityId);
                    }
                }
            }
        }

        return m_resultCache;
    }
    /**
     * Width and height of the entire grid, in world units
     */
    float m_worldSize;

    /**
     * Width and height of each grid cell
     */
    GridSize_T m_cellSize;

    /**
     * Width and height of the entire grid, in grid units (world / cell size)
     */
    GridSize_T m_gridSize;

    /**
     * The size of m_cells
     */
    size_t m_cellCount;
    Cell* m_cells;

    Cell& cellAt(GridSize_T x, GridSize_T y) const {
        size_t idx = (y * m_gridSize) + x;
        assert(idx < m_cellCount);
        return m_cells[idx];
    }

    /**
     * The biggest possible entity ID and size of m_entityCache
     */
    EntityID_T m_maxEntityID;
    EntityGridData* m_entityCache;

    EntityGridData& getEntityData(EntityID_T ID) const {
        assert(ID <= m_maxEntityID);
        return m_entityCache[ID];
    };

    /**
     * Cached vector for query results to avoid allocations
     */
    mutable std::vector<EntityID_T> m_resultCache;
    mutable uint32_t m_nextQueryID = 1;
    uint32_t getNextQueryID() const {
        return m_nextQueryID++;
    }
};

template<typename GridSize_T, typename EntityID_T>
    requires(GridC<GridSize_T, EntityID_T>)
Grid<GridSize_T, EntityID_T>::Grid(GridSize_T worldSize, GridSize_T cellSize, EntityID_T maxEntityID) :
    m_worldSize(worldSize),
    m_cellSize(cellSize),
    m_gridSize(worldSize / cellSize),
    m_cellCount(m_gridSize * m_gridSize),
    m_maxEntityID(maxEntityID) {
    m_cells = new Cell[m_cellCount];
    m_entityCache = new EntityGridData[maxEntityID];

    m_resultCache.reserve(256);
}

template<typename GridSize_T, typename EntityID_T>
    requires(GridC<GridSize_T, EntityID_T>)
Grid<GridSize_T, EntityID_T>::~Grid() {
    delete[] m_cells;
    delete[] m_entityCache;
}

template<typename GridSize_T, typename EntityID_T>
    requires(GridC<GridSize_T, EntityID_T>)
void Grid<GridSize_T, EntityID_T>::insertEntity(EntityID_T entityID, Vec2f min, Vec2f max) {
    EntityGridData& entity = getEntityData(entityID);

    GridAABB localBounds = {
        .min = roundToGrid(min),
        .max = roundToGrid(max),
    };

    // entity already exists
    if (entity.valid) {
        GridAABB& bounds = entity.bounds;

        // if the bounds didn't change from the existing ones return earlier
        if (
            localBounds.min == bounds.min
            && localBounds.max == bounds.max
        ) {
            return;
        }

        if (bounds.min == bounds.max) {
            Cell& cell = cellAt(bounds.min.x, bounds.min.y);
            auto it = std::find(cell.items.begin(), cell.items.end(), entityID);
            if (it != cell.items.end()) {
                cell.items.erase(it);
            }
        } else {
            for (GridSize_T y = bounds.min.y; y <= bounds.max.y; y++) {
                for (GridSize_T x = bounds.min.x; x <= bounds.max.x; x++) {
                    Cell& cell = cellAt(x, y);
                    auto it = std::find(cell.items.begin(), cell.items.end(), entityID);
                    if (it != cell.items.end()) {
                        cell.items.erase(it);
                    }
                }
            }
        }
    }

    entity.valid = true;
    entity.bounds = localBounds;

    for (GridSize_T y = localBounds.min.y; y <= localBounds.max.y; y++) {
        for (GridSize_T x = localBounds.min.x; x <= localBounds.max.x; x++) {
            Cell& cell = cellAt(x, y);
            cell.items.push_back(entityID);
        }
    }
}

template<typename GridSize_T, typename EntityID_T>
    requires(GridC<GridSize_T, EntityID_T>)
void Grid<GridSize_T, EntityID_T>::removeEntity(EntityID_T entityID) {
    EntityGridData& entity = getEntityData(entityID);

    if (!entity.valid)
        return;

    GridAABB& bounds = entity.bounds;

    // erase entity from current grid cells its occupying
    if (bounds.min == bounds.max) {
        Cell& cell = cellAt(bounds.min.x, bounds.min.y);
        auto it = std::find(cell.items.begin(), cell.items.end(), entityID);
        if (it != cell.items.end()) {
            cell.items.erase(it);
        }
    } else {
        for (GridSize_T y = bounds.min.y; y <= bounds.max.y; y++) {
            for (GridSize_T x = bounds.min.x; x <= bounds.max.x; x++) {
                Cell& cell = cellAt(x, y);
                auto it = std::find(cell.items.begin(), cell.items.end(), entityID);
                if (it != cell.items.end()) {
                    cell.items.erase(it);
                }
            }
        }
    }

    entity.valid = false;
    entity.queryID = 0;
    entity.bounds = {{0, 0}, {0, 0}};
}

template<typename GridSize_T, typename EntityID_T>
    requires(GridC<GridSize_T, EntityID_T>)
const std::vector<EntityID_T>& Grid<GridSize_T, EntityID_T>::queryAABB(Vec2f min, Vec2f max) const {
    GridAABB bounds = {
        .min = roundToGrid(min),
        .max = roundToGrid(max),
    };

    return queryGridAABB(bounds);
}

template<typename GridSize_T, typename EntityID_T>
    requires(GridC<GridSize_T, EntityID_T>)
const std::vector<EntityID_T>& Grid<GridSize_T, EntityID_T>::queryPosition(Vec2f pos) const {
    GridPos gridPos = roundToGrid(pos);
    return cellAt(gridPos.x, gridPos.y).items;
}

template<typename GridSize_T, typename EntityID_T>
    requires(GridC<GridSize_T, EntityID_T>)
const std::vector<EntityID_T>& Grid<GridSize_T, EntityID_T>::queryEntity(EntityID_T entityID) const {
    EntityGridData& entity = m_entityCache[entityID];
    assert(entity.valid);
    return queryGridAABB(entity.bounds);
}

template<typename GridSize_T, typename EntityID_T>
    requires(GridC<GridSize_T, EntityID_T>)
const std::vector<EntityID_T>& Grid<GridSize_T, EntityID_T>::queryLine(Vec2f lineStart, Vec2f lineEnd) const {
    Vec2f diff = lineEnd - lineStart;

    int gridDirX = lineEnd.x >= lineStart.x ? 1 : -1;
    int gridDirY = lineEnd.y >= lineStart.y ? 1 : -1;

    float dirX =
        std::abs(diff.x) > 0.00001
        ? (gridDirX * (int)m_cellSize) / diff.x
        : FLT_MAX;

    float dirY =
        std::abs(diff.y) > 0.00001
        ? (gridDirY * (int)m_cellSize) / diff.y
        : FLT_MAX;

    // cell relative
    float relativeX = std::fmod(lineStart.x / m_cellSize, 1.0f);
    float relativeY = std::fmod(lineStart.y / m_cellSize, 1.0f);

    float x = dirX * (gridDirX > 0 ? 1.0f - relativeX : relativeX);
    float y = dirY * (gridDirY > 0 ? 1.0f - relativeY : relativeY);

    GridPos start = roundToGrid(lineStart);
    GridPos endCell = roundToGrid(lineEnd);

    int cellX = start.x;
    int cellY = start.y;

    uint32_t query = getNextQueryID();
    m_resultCache.clear();

    while (true) {
        Cell& cell = cellAt(cellX, cellY);

        for (EntityID_T entityId : cell.items) {
            EntityGridData& entity = getEntityData(entityId);
            if (entity.queryID != query) {
                entity.queryID = query;
                m_resultCache.push_back(entityId);
            }
        }

        if (cellX == endCell.x && cellY == endCell.y) {
            break;
        }

        if (x < y) {
            x += dirX;
            cellX += gridDirX;
            if (cellX < 0 || cellX >= m_gridSize) {
                break;
            }
        } else {
            y += dirY;
            cellY += gridDirY;
            if (cellY < 0 || cellY >= m_gridSize) {
                break;
            }
        }
    }

    return m_resultCache;
}

}
