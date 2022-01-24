#version 330 core
layout (location = 0) in vec4 in_position; // <vec2 pos, vec2 tex>
out vec2 texcoord;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(in_position.xy, 0.0, 1.0);
    texcoord = in_position.zw;
}  