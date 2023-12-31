/////////////////////////////////////////////////////////////
//////////////////////// UNIFORMS ///////////////////////////
/////////////////////////////////////////////////////////////
uniform vec3 u_viewPos;

/////////////////////////////////////////////////////////////
//////////////////////// VARYING ////////////////////////////
/////////////////////////////////////////////////////////////
flat in int v_id;
in vec3 v_color;
in vec2 v_texCoord;
in vec3 v_fragPos;
in vec3 v_fragCameraPos;

/////////////////////////////////////////////////////////////
/////////////////////////// OUT /////////////////////////////
/////////////////////////////////////////////////////////////
layout(location = 0) out vec4 o_fragColor;
layout(location = 1) out int o_id;

#ifdef FOG
#include "lib/fog/fragment.glsl"
#endif

#ifdef BRIGHTNESS
#include "lib/brightness/fragment.glsl"
#endif

#ifdef PHONG
#include "lib/light/phong.glsl"
#include "lib/normal/fragment.glsl"
#endif

#ifdef PBR
#include "lib/light/pbr.glsl"
#include "lib/normal/fragment.glsl"
#endif

/////////////////////////////////////////////////////////////
////////////////////////// MAIN /////////////////////////////
/////////////////////////////////////////////////////////////
void super() {
    o_id = v_id;

#ifdef PHONG
    vec3 normal = getNormal();
    o_fragColor = phong(getFragmentMaterial(v_texCoord, normal), u_viewPos, v_fragPos);
#endif

#ifdef PBR
    vec3 normal = getNormal();
    o_fragColor = pbr(getFragmentMaterial(v_texCoord, normal), u_viewPos, v_fragPos);
#endif

#ifdef FOG
    o_fragColor = fog(o_fragColor, length(v_fragCameraPos));
#endif

#ifdef BRIGHTNESS
    saveBrightness(o_fragColor);
#endif
}
