#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float screen_brightness;
uniform float blur_size;
uniform bool blur_fullscreen;
uniform bool blur_partial;

uniform vec4 blur_rect_position;

in vec2 texcoord;

layout(location = 0) out vec4 color;

vec4 fade_color(vec4 in_color) { return in_color * screen_brightness; }

void main() {
  vec4 in_color = texture(screen_texture, texcoord);
  color = in_color;

  // Gaussian Blur effect
  // based on https://www.shadertoy.com/view/Xltfzj
  float Pi = 6.28318530718;  // Pi*2

  // GAUSSIAN BLUR SETTINGS
  float directions =
      16.0;  // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
  float quality =
      4.0;  // BLUR QUALITY (Default 4.0 - More is better but slower)

  if (blur_fullscreen) {

    vec2 radius = vec2(blur_size, blur_size);

    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = texcoord;
  
    // Blur calculations
    for (float d = 0.0; d < Pi; d += Pi / directions) {
      for (float i = 1.0 / quality; i <= 1.0; i += 1.0 / quality) {
        color +=
            texture(screen_texture, uv + vec2(cos(d), sin(d)) * radius * i);
      }
    }
    color /= quality * directions - 15.0;
  } else if(blur_partial) {
    vec2 radius = vec2(blur_size, blur_size);

    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = texcoord;
    
    if (uv.x > blur_rect_position.x && uv.x < (blur_rect_position.x + blur_rect_position.z) &&
    uv.y > blur_rect_position.y && uv.y < (blur_rect_position.y + blur_rect_position.w)){
      // Pixel colour

      // Blur calculations
      for (float d = 0.0; d < Pi; d += Pi / directions) {
        for (float i = 1.0 / quality; i <= 1.0; i += 1.0 / quality) {
          color +=
              texture(screen_texture, uv + vec2(cos(d), sin(d)) * radius * i);
        }
      }
      color /= quality * directions - 15.0;
      color *= 0.4;
    }
  }

  color = fade_color(color);
}