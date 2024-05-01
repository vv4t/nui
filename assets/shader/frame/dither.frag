uniform sampler2D iChannel0;

float bayer_matrix[16] = float[] (
  -0.5,      0.0,    -0.375,   0.125,
   0.25,    -0.25,    0.375,  -0.125,
  -0.3125,   0.1875, -0.4375,  0.0625,
   0.4375,  -0.0625,  0.3125, -0.1875
);

float color_range = 8.0;
float bayer_r = 1.0 / 32.0;

void frame_shade(out vec4 frag_color, in vec2 frag_coord)
{
  ivec2 uv = ivec2(frag_coord * vec2(textureSize(iChannel0, 0)));

  vec3 tex_color = texture(iChannel0, frag_coord).rgb;
  vec3 dither_color = tex_color + bayer_r * bayer_matrix[(uv.y % 4) * 4 + (uv.x % 4)];
  vec3 nearest_color = round(dither_color * color_range) / color_range;
  
  frag_color = vec4(nearest_color, 1.0);
}
