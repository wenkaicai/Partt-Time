#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int rows;
uniform int cols;
uniform int animation;
uniform int frame;

void main() {
  float offset_frame = 1.0f / cols;
  float offset_animation = 1.0f / rows;
  texcoord = vec2((in_texcoord[0] / cols) + (frame * offset_frame),
                  (in_texcoord[1] / rows) + (animation * offset_animation));

  vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
  gl_Position = vec4(pos.xy, in_position.z, 1.0);
}