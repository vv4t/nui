uniform bool u_horizontal;

float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void frag_pass()
{       
  vec2 tex_offset = 1.0 / vec2(textureSize(u_scene, 0));
  vec3 result = get_color(get_uv()).rgb * weight[0];
  
  if (u_horizontal) {
    for(int i = 1; i < 5; i++) {
      result += get_color(get_uv() + vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
      result += get_color(get_uv() - vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
    }
  } else {
    for(int i = 1; i < 5; i++) {
      result += get_color(get_uv() + vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];
      result += get_color(get_uv() - vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];
    }
  }
  
  set_frag(vec4(result, 1.0));
}
