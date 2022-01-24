#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;
layout(location = 2) in vec2 offset;
layout(location = 3) in float size;

// Passed to fragment shader
out vec2 texcoord;

// Application data
// uniform mat3 transform;
uniform mat3 projection;

void main() {
  texcoord = in_texcoord;
  vec3 pos = projection * vec3((in_position.xy * size) + offset, 1.0);
  gl_Position = vec4(pos.xy, in_position.z, 1.0);
}