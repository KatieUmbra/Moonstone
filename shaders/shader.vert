#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texture_coordinate;
layout(location = 2) in uint texture_layer;

layout(location = 0) out vec2 v_texture_coordinate;
layout(location = 1) out uint v_texture_layer;
uniform mat4 u_model_view_projection;

void main()
{
    gl_Position = u_model_view_projection * vec4(position.xy, 0.0, 1.0);
    v_texture_coordinate = texture_coordinate;
    v_texture_layer = texture_layer;
}
