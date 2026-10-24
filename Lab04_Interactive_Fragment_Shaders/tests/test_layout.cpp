#include "Lab03Math.h"
#include "bunny_data.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

int main() {
    using namespace lab03;
    const AABB box=computeAABB(BunnyMesh::vertices,BunnyMesh::vertexCount);
    const BoundingSphere sphere=computeBoundingSphere(BunnyMesh::vertices,BunnyMesh::vertexCount,box);
    assert(sphere.radius>0);
    constexpr double eps=1e-5;
    for(std::size_t i=0; i<BunnyMesh::vertexCount;++i) {
        const double dx=BunnyMesh::vertices[i*6]-sphere.center.x;
        const double dy=BunnyMesh::vertices[i*6+1]-sphere.center.y;
        const double dz=BunnyMesh::vertices[i*6+2]-sphere.center.z;
        assert(dx*dx+dy*dy+dz*dz<=sphere.radius*sphere.radius+eps);
    }
    // Verify the grid formula for ALL 10,000 valid user choices.
    for(int n=1; n<=10000; ++n) {
        auto g=computeGridLayout(n,8.0/3.0,2.0);
        const double scale=computeFixedScale(g,sphere.radius);
        assert(g.columns*g.rows>=n && g.rows>0 && g.columns>0);
        assert(2.0*sphere.radius*scale <= 0.85*std::min(g.cellWidth,g.cellHeight)+eps);
    }
    for(int count: {1,2,3,10,37,100,256,999,1000,7500,10000}) {
        GridLayout g=computeGridLayout(count,8.0/3.0,2.0); // 800x600 aspect
        g.scale=computeFixedScale(g,sphere.radius,0.85);
        assert(g.columns*g.rows>=count);
        assert(g.scale>0);
        const double scaledRadius=sphere.radius*g.scale;
        assert(2*scaledRadius <= std::min(g.cellWidth,g.cellHeight)*0.85 + eps);
        for(int i=0;i<count;++i) {
            const Vec3 p=cellCenter(g,i);
            assert(std::abs(p.x)+scaledRadius<g.worldWidth*0.5+eps);
            assert(std::abs(p.y)+scaledRadius<g.worldHeight*0.5+eps);
            if(i+1<count) {
                const Vec3 q=cellCenter(g,i+1);
                const double dx=p.x-q.x,dy=p.y-q.y;
                assert(dx*dx+dy*dy+eps >= 4*scaledRadius*scaledRadius);
            }
        }
        const auto transforms=buildInstanceTransforms(g,1.5);
        assert(transforms.size()==(std::size_t)count);
        assert(std::abs(transforms[0].scale-g.scale)<eps);
        std::cout << "PASS N=" << count << " C="<<g.columns<<" R="<<g.rows
                  << " scale="<<g.scale<<'\n';
    }
    std::cout<<"ALL LAB03 layout tests passed\n";
}
