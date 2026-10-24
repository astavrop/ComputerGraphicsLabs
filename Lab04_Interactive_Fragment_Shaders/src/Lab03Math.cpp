#include "Lab03Math.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace lab03 {
namespace {
void requireMesh(const float* v, std::size_t n) {
    if (!v || !n) throw std::invalid_argument("Bunny vertex array must not be empty");
}
}
AABB computeAABB(const float* v, std::size_t n) {
    requireMesh(v,n);
    const double inf = std::numeric_limits<double>::infinity();
    AABB b{{inf,inf,inf},{-inf,-inf,-inf}};
    for (std::size_t i=0; i<n; ++i) {
        const double x=v[6*i], y=v[6*i+1], z=v[6*i+2];
        b.min.x=std::min(b.min.x,x); b.min.y=std::min(b.min.y,y); b.min.z=std::min(b.min.z,z);
        b.max.x=std::max(b.max.x,x); b.max.y=std::max(b.max.y,y); b.max.z=std::max(b.max.z,z);
    }
    return b;
}
BoundingSphere computeBoundingSphere(const float* v, std::size_t n, const AABB& b) {
    requireMesh(v,n);
    BoundingSphere s;
    s.center={(b.min.x+b.max.x)*0.5,(b.min.y+b.max.y)*0.5,(b.min.z+b.max.z)*0.5};
    double squaredRadius=0;
    for (std::size_t i=0; i<n; ++i) {
        const double dx=v[i*6]-s.center.x, dy=v[i*6+1]-s.center.y, dz=v[i*6+2]-s.center.z;
        squaredRadius=std::max(squaredRadius,dx*dx+dy*dy+dz*dz);
    }
    s.radius=std::sqrt(squaredRadius);
    if (!(s.radius>0)) throw std::invalid_argument("Degenerate Bunny mesh");
    return s;
}
GridLayout computeGridLayout(int n, double W, double H) {
    if(n<1 || n>10000 || !(W>0) || !(H>0))
        throw std::invalid_argument("Need 1..10000 Bunnies and positive world-space viewport dimensions");
    GridLayout g;
    g.count=n; g.worldWidth=W; g.worldHeight=H;
    g.columns=std::min(n,static_cast<int>(std::ceil(std::sqrt(static_cast<double>(n)*W/H))));
    g.rows=(n+g.columns-1)/g.columns;
    g.cellWidth=W/g.columns; g.cellHeight=H/g.rows;
    return g;
}
double computeFixedScale(const GridLayout& g, double radius, double margin) {
    if(g.count<1 || !(radius>0) || !(margin>0 && margin<1))
        throw std::invalid_argument("Invalid grid, sphere radius, or 0<margin<1");
    return margin*std::min(g.cellWidth,g.cellHeight)/(2.0*radius);
}
Vec3 cellCenter(const GridLayout& g, int instanceId) {
    if(instanceId<0 || instanceId>=g.count) throw std::out_of_range("Invalid instance id");
    const int col=instanceId%g.columns, row=instanceId/g.columns;
    return {-g.worldWidth*0.5+(col+0.5)*g.cellWidth,
             +g.worldHeight*0.5-(row+0.5)*g.cellHeight, 0.0};
}
std::vector<InstanceTransform> buildInstanceTransforms(const GridLayout& g, double t) {
    std::vector<InstanceTransform> out;
    out.reserve(static_cast<std::size_t>(g.count));
    for (int i=0;i<g.count;++i) {
        InstanceTransform inst;
        inst.translation=cellCenter(g,i);
        inst.scale=g.scale;
        inst.anglesRadians={t*0.41+i*0.13, t*0.63+i*0.07,t*0.27+i*0.11};
        out.push_back(inst);
    }
    return out;
}
}
