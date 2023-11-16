#version 330 core

/////////////////////////////////////////////////////////////
//////////////////////// UNIFORMS ///////////////////////////
/////////////////////////////////////////////////////////////
uniform vec4 u_color;
uniform vec3 u_lightPos;

/////////////////////////////////////////////////////////////
//////////////////////// VARYING ////////////////////////////
/////////////////////////////////////////////////////////////
in vec3 v_fragPos;

/////////////////////////////////////////////////////////////
/////////////////////////// OUT /////////////////////////////
/////////////////////////////////////////////////////////////
layout(location = 0) out vec4 o_fragColor;

/////////////////////////////////////////////////////////////
////////////////////////// MAIN /////////////////////////////
/////////////////////////////////////////////////////////////
void main() {
    vec3 dx = dFdx(v_fragPos);
    vec3 dy = dFdy(v_fragPos);
    vec3 normal = normalize(cross(dx, dy));

    vec3 lightDir = normalize(u_lightPos - v_fragPos);
    float diffuseFactor = dot(normal, lightDir) * 0.5 + 0.5;

    o_fragColor = vec4(u_color.rgb * diffuseFactor, 1.0);
}
