#include "Lab03Student.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace lab03::student {
BoundingSphere computeBoundingSphere(const float* vertices, std::size_t n, const AABB& box) {
    // Exercise 1 (Geometry, 20 points) — completed
    // 1. Compute the AABB midpoint c.
    // 2. Iterate over all n vertices; positions have stride 6 (x,y,z,r,g,b).
    // 3. Find max squared distance from c to ANY bunny vertex.
    // 4. Return {c, sqrt(maxSquaredDistance)}.
    // A valid sphere must enclose ALL vertices and keep its radius under rotation.

    Vec3 max= box.max;
    Vec3 min = box.min;

    Vec3 center;

    center.x = (max.x + min.x) / 2.0f;
    center.y = (max.y + min.y) / 2.0f;
    center.z = (max.z + min.z) / 2.0f;
    double maxSquaredistance = 0;

    for (size_t i = 0; i < n; ++i)
    {
        float x = vertices[i * 6 + 0];
        float y = vertices[i * 6 + 1];
        float z = vertices[i * 6 + 2];

        float dx = x - center.x;
        float dy = y - center.y;
        float dz = z - center.z;

        float dSquared = dx*dx + dy*dy + dz*dz;
        maxSquaredistance = std::max(static_cast<double>(dSquared), maxSquaredistance);
   }

    double radius = sqrt(maxSquaredistance);

    return { center,radius };


}

GridLayout computeGridLayout(int bunnyCount, double worldWidth, double worldHeight) {
    // Exercise 2 (Layout, 25 points) — completed
    // Validate count: 1..10000; positive width and height.
    // C = min(N, ceil(sqrt(N * W/H)))
    // R = ceil(N/C) = (N+C-1)/C using INTEGER arithmetic.
    // cellWidth = W/C; cellHeight = H/R.
    // Return a GridLayout with all members initialized; scale may be 0 here.


    GridLayout grid;
    grid.worldHeight = worldHeight;
    grid.worldWidth = worldWidth;
    grid.count = bunnyCount;
    int ceilresult = std::ceil(sqrt(bunnyCount * (worldWidth / worldHeight)));
    int C = std::min(bunnyCount,ceilresult);
    int R = (bunnyCount + C - 1) / C;
    double cellWidth = worldWidth / C;
    double cellHeight = worldHeight / R;

    grid.columns = C;
    grid.rows = R;
    grid.cellHeight = cellHeight;
    grid.cellWidth = cellWidth;

    return grid;
}

double computeFixedScale(const GridLayout& g, double radius, double margin) {
    // Exercise 3 (No-overlap guarantee, 25 points) — completed
    // s = margin * min(cellWidth,cellHeight) / (2 * radius).
    // Validate radius>0 and 0<margin<1.
    // WHY does the bounding sphere stay the same size during rigid rotation?
    double s = 0;

    if(radius>0&&margin>0&&margin<1)
    s = (margin * std::min(g.cellWidth, g.cellHeight)/(2*radius));

    return s;
}

std::vector<InstanceTransform> buildInstanceTransforms(const GridLayout& g, double timeSeconds) {
    // Exercise 4 (Transforms, 30 points) — completed
    // For i in 0 .. g.count-1:
    //   col = i % g.columns; row = i / g.columns;
    //   translation.x = -W/2 + (col+0.5)*cellWidth;
    //   translation.y = +H/2 - (row+0.5)*cellHeight;
    //   translation.z = 0;
    //   scale = g.scale (SAME for every bunny; never time-dependent);
    //   anglesRadians = e.g. {timeSeconds*0.41+i*0.13,
    //                        timeSeconds*0.63+i*0.07, timeSeconds*0.27+i*0.11};
    // CPU exercise; the real graphics application derives equivalent transforms
    // from gl_InstanceID in a vertex shader (Lab02 technique).

    std::vector<InstanceTransform> trans;
    trans.resize(g.count);

    for (int i = 0; i < g.count; ++i)
    {
        int col = i % g.columns;
        int row = i / g.columns;
        double x = -g.worldWidth / 2.0 + (col + 0.5) * g.cellWidth;
        double y = g.worldHeight / 2.0 - (row + 0.5) * g.cellHeight;
        double z = 0.0;
        Vec3 tr;
        tr.x = x;
        tr.y = y;
        tr.z = z;
        trans[i].translation = tr;
        trans[i].scale = g.scale;
        trans[i].anglesRadians = {
    timeSeconds * 0.41 + i * 0.13,
    timeSeconds * 0.63 + i * 0.07,
    timeSeconds * 0.27 + i * 0.11
        };



    }
    return trans;
}
    
    

}
