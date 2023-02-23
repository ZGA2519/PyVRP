#ifndef HGS_VRPTW_ROUTE_H
#define HGS_VRPTW_ROUTE_H

#include "Node.h"
#include "TimeWindowSegment.h"

#include <array>
#include <bit>
#include <cassert>
#include <iosfwd>

class Route
{
    std::vector<Node *> nodes;  // List of nodes (in order) in this solution.

    int load_;             // Current route load.
    bool isLoadFeasible_;  // Whether current load is feasible.

    int timeWarp_;             // Current route time warp.
    bool isTimeWarpFeasible_;  // Whether current time warp is feasible.

    // Populates the nodes vector.
    void setupNodes();

    // Sets the route center angle.
    void setupAngle();

    // Sets forward node time windows.
    void setupRouteTimeWindows();

public:  // TODO make fields private
    ProblemData const *data;

    int idx;             // Route index
    Node *depot;         // Pointer to the associated depot
    double angleCenter;  // Angle of the barycenter of the route

    /**
     * @return The client or depot node at the given position.
     */
    [[nodiscard]] inline Node *operator[](size_t position) const;

    /**
     * Tests if this route is feasible.
     *
     * @return true if the route is feasible, false otherwise.
     */
    [[nodiscard]] inline bool isFeasible() const;

    /**
     * Determines whether this route is load-feasible.
     *
     * @return true if the route exceeds the vehicle capacity, false otherwise.
     */
    [[nodiscard]] inline bool hasExcessCapacity() const;

    /**
     * Determines whether this route is time-feasible.
     *
     * @return true if the route has time warp, false otherwise.
     */
    [[nodiscard]] inline bool hasTimeWarp() const;

    /**
     * @return Total load on this route.
     */
    [[nodiscard]] inline int load() const;

    /**
     * @return Total time warp on this route.
     */
    [[nodiscard]] inline int timeWarp() const;

    /**
     * @return true if this route is empty, false otherwise.
     */
    [[nodiscard]] inline bool empty() const;

    /**
     * @return Number of clients in this route.
     */
    [[nodiscard]] inline size_t size() const;

    /**
     * Calculates time window data for segment [start, end].
     */
    [[nodiscard]] inline TimeWindowSegment twBetween(size_t start,
                                                     size_t end) const;

    /**
     * Calculates the distance for segment [start, end].
     */
    [[nodiscard]] inline int distBetween(size_t start, size_t end) const;

    /**
     * Calculates the load for segment [start, end].
     */
    [[nodiscard]] inline int loadBetween(size_t start, size_t end) const;

    /**
     * Updates this route. To be called after swapping nodes/changing the
     * solution.
     */
    void update();
};

bool Route::isFeasible() const
{
    return !hasExcessCapacity() && !hasTimeWarp();
}

bool Route::hasExcessCapacity() const { return !isLoadFeasible_; }

bool Route::hasTimeWarp() const 
{
#ifdef VRP_NO_TIME_WINDOWS
    return false;
#else
    return !isTimeWarpFeasible_; 
#endif
}

Node *Route::operator[](size_t position) const
{
    assert(position > 0);
    return nodes[position - 1];
}

int Route::load() const { return load_; }

int Route::timeWarp() const { return timeWarp_; }

bool Route::empty() const { return size() == 0; }

size_t Route::size() const
{
    return nodes.size() - 1;  // exclude end depot
}

TimeWindowSegment Route::twBetween(size_t start, size_t end) const
{
    assert(start <= end);

    auto tws = nodes[start - 1]->tw;

    for (size_t step = start; step != end; ++step)
        tws = TimeWindowSegment::merge(tws, nodes[step]->tw);

    return tws;
}

int Route::distBetween(size_t start, size_t end) const
{
    assert(start <= end && end <= nodes.size());

    auto const startDist = start == 0 ? 0 : nodes[start - 1]->cumulatedDistance;
    auto const endDist = nodes[end - 1]->cumulatedDistance;

    assert(startDist <= endDist);

    return endDist - startDist;
}

int Route::loadBetween(size_t start, size_t end) const
{
    assert(start <= end && end <= nodes.size());

    auto const *startNode = start == 0 ? depot : nodes[start - 1];
    auto const atStart = data->client(startNode->client).demand;
    auto const startLoad = startNode->cumulatedLoad;
    auto const endLoad = nodes[end - 1]->cumulatedLoad;

    assert(startLoad <= endLoad);

    return endLoad - startLoad + atStart;
}

// Outputs a route into a given ostream in CVRPLib format
std::ostream &operator<<(std::ostream &out, Route const &route);

#endif  // HGS_VRPTW_ROUTE_H