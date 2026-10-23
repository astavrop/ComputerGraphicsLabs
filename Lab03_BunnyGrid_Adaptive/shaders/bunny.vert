#version 330 core
layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aColor;
uniform vec3 uMeshCenter;
uniform int uColumns;
uniform vec2 uCellSize;
uniform vec2 uWorldSize;
uniform float uFixedScale;
uniform float uRadius;
uniform float uTime;
uniform int uSphereOutline;
out vec3 vColor;

mat3 rotateX(float a) {
    float c=cos(a),s=sin(a);
    return mat3(1,0,0, 0,c,s, 0,-s,c);
}
mat3 rotateY(float a) {
    float c=cos(a),s=sin(a);
    return mat3(c,0,-s, 0,1,0, s,0,c);
}
mat3 rotateZ(float a) {
    float c=cos(a),s=sin(a);
    return mat3(c,s,0, -s,c,0, 0,0,1);
}
void main() {
    // TODO 5 (optional shader exercise): explain how gl_InstanceID identifies
    // a Bunny without any per-instance CPU draw call (as in Lab02).
    int col=gl_InstanceID % uColumns;
    int row=gl_InstanceID / uColumns;
    vec2 center = vec2(-0.5*uWorldSize.x+(float(col)+0.5)*uCellSize.x,
                        0.5*uWorldSize.y-(float(row)+0.5)*uCellSize.y);
    vec3 centered = aPosition-uMeshCenter;
    float seed=float(gl_InstanceID);
    // Distinct rotations on X, Y, Z for every Bunny. The sphere is invariant.
    mat3 R=rotateZ(uTime*0.27+seed*0.11)*rotateY(uTime*0.63+seed*0.07)*rotateX(uTime*0.41+seed*0.13);
    vec3 p = uSphereOutline==1 ? aPosition*uRadius : R*centered;
    p *= uFixedScale; // Fixed after layout initialization: never changes with rotation.
    p.xy += center;
    gl_Position = vec4(
    2.0 * p.x / uWorldSize.x,
    2.0 * p.y / uWorldSize.y,
    2.0 * p.z / uWorldSize.y,
    1.0
);
    vColor = uSphereOutline==1 ? vec3(0.35,0.95,0.9) : aColor;
    //set your own color for each vertex
    //vColor = vec3(0.9,0.35,0.9);
}
