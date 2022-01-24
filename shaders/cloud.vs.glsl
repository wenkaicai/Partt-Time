#version 330

// Input attributes
in vec3 in_position;
in vec3 in_color;
in vec3 in_normal;


out vec3 vcolor;
out vec2 vpos;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform float time;

void main() {
  vec3 light_pos = vec3(0,2.0,2.0);
  vpos = in_position.xy;  // local coordinated before transform
  // translate vertices based on time and normals periodically to give an effect of cloud bubbling
  vpos[0] = vpos[0] + 0.012 * sin(time * in_normal[0]) + 0.012 * sin(time * in_normal[1]);
  vpos[1] = vpos[1] + 0.012 * sin(time * in_normal[0]) + 0.012 * sin(time * in_normal[1]);

  vec3 pos = projection * transform * vec3(vpos, 1.0);
  // calculate angle to light source
  vec3 L = normalize(light_pos - vec3(pos.xy, in_position.z));
  // dot it and max it to get diffuse light weight
  float lambertian = max(0.0, dot(L,in_normal));
  
  // ambient + diffuse * lambertian
  vcolor = vec3(0.08, 0.15, 0.25) + vec3(1.0,1.0,1.0) * min(lambertian,1.0);
  gl_Position = vec4(pos.xy, in_position.z, 1.0);
}