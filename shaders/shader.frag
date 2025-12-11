#version 460 core

layout(location = 0) out vec4 color;

uniform sampler2D u_texture;
uniform sampler2DArray u_textureArray;

layout(location = 0) in vec2 v_texture_coordinate;
layout(location = 1) flat in uint v_texture_layer;

void main()
{
    color = vec4(texture(u_textureArray, vec3(v_texture_coordinate.xy, v_texture_layer)));
}
