#version 330 core

/////////////////////////////////////////////////////////////
//////////////////////// ATTRIBUTES /////////////////////////
/////////////////////////////////////////////////////////////
layout(location = 0) in vec3 a_vertexPosition;
layout(location = 1) in vec2 a_vertexTextureCoord;

/////////////////////////////////////////////////////////////
///////////////////////// VARYING ///////////////////////////
/////////////////////////////////////////////////////////////
out vec2 v_texCoord;

/////////////////////////////////////////////////////////////
////////////////////////// MAIN /////////////////////////////
/////////////////////////////////////////////////////////////
void main() {
    v_texCoord = a_vertexTextureCoord;
    gl_Position = vec4(a_vertexPosition, 1.0);
}