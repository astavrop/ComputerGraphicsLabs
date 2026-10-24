#pragma once
#include "Lab03Math.h"
namespace lab03::student {
// STUDENT IMPLEMENTATION: four exercises implemented in src/Lab03Student.cpp.
BoundingSphere computeBoundingSphere(const float* vertices, std::size_t n, const AABB& box);
GridLayout computeGridLayout(int bunnyCount, double worldWidth, double worldHeight);
double computeFixedScale(const GridLayout& g, double radius, double margin=0.85);
std::vector<InstanceTransform> buildInstanceTransforms(const GridLayout& g, double timeSeconds=0.0);
}
