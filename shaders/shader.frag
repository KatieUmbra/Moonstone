#version 460 core

layout(location = 0) out vec4 color;

uniform sampler2D u_texture;
uniform sampler2DArray u_textureArray;

layout(location = 0) in vec3 v_texture_coordinate;

void main()
{
    color = vec4(texture(u_textureArray, v_texture_coordinate));
}
