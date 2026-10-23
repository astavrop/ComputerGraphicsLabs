#pragma once
#include <cstddef>
#include <vector>

namespace lab03 {
struct Vec3 { double x=0, y=0, z=0; };
struct AABB { Vec3 min, max; };
struct BoundingSphere { Vec3 center; double radius=0; };
struct GridLayout {
    int count=0, columns=0, rows=0;
    double worldWidth=0, worldHeight=0;
    double cellWidth=0, cellHeight=0;
    double scale=0;
};
struct InstanceTransform {
    Vec3 translation;
    double scale=1;
    Vec3 anglesRadians;
};

// vertices: six floats per vertex (x,y,z,r,g,b), same as Lab01 / Lab02.
AABB computeAABB(const float* vertices, std::size_t vertexCount);
BoundingSphere computeBoundingSphere(const float* vertices, std::size_t vertexCount, const AABB& box);
GridLayout computeGridLayout(int count, double worldWidth, double worldHeight);
double computeFixedScale(const GridLayout& layout, double radius, double margin = 0.85);
Vec3 cellCenter(const GridLayout& layout, int instanceId);
std::vector<InstanceTransform> buildInstanceTransforms(const GridLayout& layout, double timeSeconds=0.0);
}
