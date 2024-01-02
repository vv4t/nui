#include "ngui.h"

#include "../renderer/gl.h"
#include "../renderer/shader.h"
#include "../renderer/mesh.h"
#include "../renderer/quad_buffer.h"
#include "../common/log.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#define MAX_INPUT_TEXT 256
#define NUM_GLYPHS 128
#define GLYPH_SIZE 64
#define GLYPH_MAP_WIDTH (GLYPH_SIZE * NUM_GLYPHS)

typedef struct {
  vec2_t uv;
  vec2_t uv_size;
  vec2_t size;
  vec2_t bearing;
  float advance;
} glyph_t;

typedef struct {
  char text[MAX_INPUT_TEXT];
} ngui_input_t;

typedef struct {
  quad_buffer_t quad_buffer;
  
  ngui_input_t console_input;
  
  glyph_t glyphs[NUM_GLYPHS];
  
  GLuint shader;
  GLuint glyph_map;
  
  mesh_t mesh;
} ngui_t;

static ngui_t ngui;

static bool ngui_load_glyph_map();
static bool ngui_load_text(const char *text, vec2_t pos);

bool ngui_init(int width, int height)
{
  quad_buffer_init(&ngui.quad_buffer, 256);
  
  if (!ngui_load_glyph_map()) {
    return false;
  }
  
  if (!mesh_buffer_new(&ngui.mesh, ngui.quad_buffer.vertices, ngui.quad_buffer.max_vertices)) {
    return false;
  }
  
  if (!shader_load(&ngui.shader, "ngui")) {
    return false;
  }
  
  ngui.console_input.text[0] = '>';
  ngui.console_input.text[1] = ' ';
  ngui.console_input.text[2] = 0;
  
  return true;
}

static bool ngui_load_text(const char *text, vec2_t pos)
{
  int num_char = strlen(text);
  
  float scale = 0.03;
  float x_pos = 0.0;
  
  for (int i = 0; i < num_char; i++) {
    glyph_t glyph = ngui.glyphs[text[i]];
    vec2_t glyph_pos = vec2_init(pos.x + x_pos + glyph.bearing.x * scale, pos.y - (glyph.size.y - glyph.bearing.y) * scale);
    vec2_t glyph_size = vec2_mulf(glyph.size, scale);
    
    quad_buffer_add(&ngui.quad_buffer, glyph_pos, glyph_size, glyph.uv, glyph.uv_size);
    x_pos += glyph.advance * scale;
  }
  
  if (!mesh_buffer_sub(&ngui.mesh, ngui.quad_buffer.vertices, 0, ngui.quad_buffer.max_vertices)) {
    return false;
  }
  
  return true;
}

void ngui_focus()
{
  ngui.console_input.text[0] = '>';
  ngui.console_input.text[1] = ' ';
  ngui.console_input.text[2] = 0;
  
  quad_buffer_reset(&ngui.quad_buffer);
  ngui_load_text(ngui.console_input.text, vec2_init(0.01, 0.01));
}

void ngui_text_input(const char *text)
{
  quad_buffer_reset(&ngui.quad_buffer);
  strncat(ngui.console_input.text, text, MAX_INPUT_TEXT - 1);
  
  ngui_load_text(ngui.console_input.text, vec2_init(0.01, 0.01));
}

void ngui_key_event(int key, int action)
{
  if (action != 1) {
    return;
  }
  
  if (key == 8) {
    if (strlen(ngui.console_input.text) > 2) {
      ngui.console_input.text[strlen(ngui.console_input.text) - 1] = 0;
      
      quad_buffer_reset(&ngui.quad_buffer);
      ngui_load_text(ngui.console_input.text, vec2_init(0.01, 0.01));
    }
  }
  
  if (key == 13) {
    ngui.console_input.text[2] = 0;
    
    quad_buffer_reset(&ngui.quad_buffer);
    ngui_load_text(ngui.console_input.text, vec2_init(0.01, 0.01));
  }
}

void ngui_unfocus()
{
  ngui.console_input.text[0] = 0;
  quad_buffer_reset(&ngui.quad_buffer);
  ngui_load_text(ngui.console_input.text, vec2_init(0.01, 0.01));
}

static bool ngui_load_glyph_map()
{
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    LOG_ERROR("failed to initialize freetype");
    return false;
  }
  
  FT_Face face;
  if (FT_New_Face(ft, "assets/font/saxmono.ttf", 0, &face)) {
    LOG_ERROR("failed to load font 'assets/font/anonymous_pro.ttf'");
    return false;
  }
  
  FT_Set_Pixel_Sizes(face, 0, GLYPH_SIZE);
  
  float *data = calloc(GLYPH_MAP_WIDTH * GLYPH_SIZE * 4, sizeof(float));
  
  for (int c = 0; c < NUM_GLYPHS; c++) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      return false;
    }
    
    int width = face->glyph->bitmap.width;
    int height = face->glyph->bitmap.rows;
    
    int bitmap_left = face->glyph->bitmap_left;
    int bitmap_top = face->glyph->bitmap_top;
    
    int pitch = face->glyph->bitmap.pitch;
    
    float glyph_scale = 1.0 / GLYPH_SIZE;
    
    ngui.glyphs[c].size = vec2_mulf(vec2_init(width, height), glyph_scale);
    ngui.glyphs[c].uv = vec2_init(c / (float) NUM_GLYPHS, 0.0);
    ngui.glyphs[c].uv_size = vec2_init(width * glyph_scale / (float) NUM_GLYPHS, height * glyph_scale);
    ngui.glyphs[c].bearing = vec2_mulf(vec2_init(bitmap_left, bitmap_top), glyph_scale);
    ngui.glyphs[c].advance = (face->glyph->advance.x >> 6) * glyph_scale;
    
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        float alpha = ((float) face->glyph->bitmap.buffer[(height - y - 1) * pitch + x]) / 256.0;
        
        int x_px = c * GLYPH_SIZE + x;
        int y_px = y;
        
        int idx = y_px * GLYPH_MAP_WIDTH + x_px;
        
        data[idx * 4 + 0] = alpha;
        data[idx * 4 + 1] = alpha;
        data[idx * 4 + 2] = alpha;
        data[idx * 4 + 3] = alpha;
      }
    }
  }
  
  glGenTextures(1, &ngui.glyph_map);
  glBindTexture(GL_TEXTURE_2D, ngui.glyph_map);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GLYPH_MAP_WIDTH, GLYPH_SIZE, 0, GL_RGBA, GL_FLOAT, data);
  
  free(data);
  
  return true;
} 

void ngui_render()
{
  glUseProgram(ngui.shader);
  glClear(GL_DEPTH_BUFFER_BIT);
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ngui.glyph_map);
  mesh_draw(ngui.mesh);
}
