#version 330 core

/////////////////////////////////////////////////////////////
//////////////////////// ATTRIBUTES /////////////////////////
/////////////////////////////////////////////////////////////
layout(location = 0) in vec2 a_vertexPosition;
layout(location = 1) in vec2 a_vertexTextureCoord;

/////////////////////////////////////////////////////////////
//////////////////////// UNIFORMS ///////////////////////////
/////////////////////////////////////////////////////////////
uniform mat4 u_projection;
uniform mat4 u_model;
uniform mat4 u_ndcModel;

/////////////////////////////////////////////////////////////
///////////////////////// VARYING ///////////////////////////
/////////////////////////////////////////////////////////////
out vec2 v_texCoord;

/////////////////////////////////////////////////////////////
////////////////////////// MAIN /////////////////////////////
/////////////////////////////////////////////////////////////
void main() {
    v_texCoord = a_vertexTextureCoord;
    gl_Position = u_ndcModel * u_projection * u_model * vec4(a_vertexPosition.xy, 0.0, 1.0);
}