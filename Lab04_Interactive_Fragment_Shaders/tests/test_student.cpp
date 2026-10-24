#include "Lab03Student.h"
#include "Lab03Math.h"
#include "bunny_data.h"
#include <cassert>
#include <cmath>
#include <iostream>
int main() {
    using namespace lab03;
    const auto box=computeAABB(BunnyMesh::vertices,BunnyMesh::vertexCount);
    auto sphere=student::computeBoundingSphere(BunnyMesh::vertices,BunnyMesh::vertexCount,box);
    const auto reference=computeBoundingSphere(BunnyMesh::vertices,BunnyMesh::vertexCount,box);
    assert(std::abs(sphere.radius-reference.radius)<1e-5);
    for(int n : {1,10,37,100,1000,10000}) {
        auto g=student::computeGridLayout(n,8.0/3.0,2.0);
        auto ref=computeGridLayout(n,8.0/3.0,2.0);
        assert(g.columns==ref.columns && g.rows==ref.rows);
        g.scale=student::computeFixedScale(g,sphere.radius);
        assert(std::abs(g.scale-computeFixedScale(ref,reference.radius))<1e-5);
        auto tr=student::buildInstanceTransforms(g,1.1);
        assert((int)tr.size()==n);
        assert(std::abs(tr.front().scale-g.scale)<1e-5);
        std::cout << "STUDENT PASS N=" << n << '\n';
    }
}
