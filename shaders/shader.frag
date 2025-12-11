#version 460 core

layout(location = 0) out vec4 color;

uniform sampler2DArray u_textureArray;
uniform int u_texture_layer;

in vec2 v_texture_coordinate;
flat in uint v_texture_layer;

void main()
{
    color = vec4(texture(u_textureArray, vec3(v_texture_coordinate.xy, v_texture_layer)));
}
