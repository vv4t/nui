#include "renderer.hpp"
#include "mesh_builder.hpp"
#include "shader_builder.hpp"
#include <iostream>

#define BUFFER_WIDTH 800
#define BUFFER_HEIGHT 800

renderer_t::renderer_t(game_t& game)
  : m_vertex_buffer(256),
    m_game(game),
    m_depth(BUFFER_WIDTH, BUFFER_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_FLOAT),
    m_normal(texture_t(BUFFER_WIDTH, BUFFER_HEIGHT, GL_RGBA, GL_RGBA16F, GL_FLOAT)),
    m_buffer {
      texture_t(BUFFER_WIDTH, BUFFER_HEIGHT, GL_RGBA, GL_RGBA16F, GL_FLOAT),
      texture_t(BUFFER_WIDTH, BUFFER_HEIGHT, GL_RGBA, GL_RGBA16F, GL_FLOAT)
    },
    m_target{
      target_t({ binding_t(GL_COLOR_ATTACHMENT0, m_buffer[0]) }),
      target_t({ binding_t(GL_COLOR_ATTACHMENT0, m_buffer[1]) })
    },
    m_gbuffer_target({
      binding_t(GL_COLOR_ATTACHMENT0, m_buffer[0]),
      binding_t(GL_COLOR_ATTACHMENT1, m_normal),
      binding_t(GL_DEPTH_ATTACHMENT, m_depth)
    }),
    m_gbuffer(
      shader_builder_t()
      .source_vertex_shader("assets/planar-map.vert")
      .source_fragment_shader("assets/gbuffer.frag")
      .attach(m_camera)
      .attach(m_lighting)
      .bind("u_albedo", 0)
      .bind("u_normal", 1)
      .bind("u_roughness", 2)
      .compile()
    ),
    m_point_light_scatter(
      shader_builder_t()
      .source_deferred_shader("assets/point-light-scatter.frag")
      .attach(m_camera)
      .attach(m_lighting)
      .compile()
    ),
    m_water(
      shader_builder_t()
      .source_deferred_shader("assets/water.frag")
      .attach(m_camera)
      .compile()
    ),
    m_ssr(shader_builder_t().source_deferred_shader("assets/ssr.frag").compile()),
    m_ssao(shader_builder_t().source_deferred_shader("assets/ssao.frag").compile()),
    m_dither(shader_builder_t().source_frame_shader("assets/dither.frag").compile()),
    m_tone_map(shader_builder_t().source_frame_shader("assets/tone-map.frag").compile())
{
  std::vector<vec3> samples;
  
  for (int i = 0; i < 32; i++) {
    float x = (rand() % 256) / 256.0f * 2.0 - 1.0;
    float y = (rand() % 256) / 256.0f * 2.0 - 1.0;
    float z = (rand() % 256) / 256.0f;
    float t = (rand() % 256) / 256.0f;

    samples.push_back(vec3(x, y, z).normalize() * t);
  }

  m_ssao.uniform_vec3_array("u_samples", samples);

  m_textures.reserve(64);
  init_assets();
  m_lighting.add_light(vec3(-6,1,-8), vec3(20,32,32));
  m_lighting.add_light(vec3(6,4,16), vec3(32,20,32));
}

void renderer_t::bind() {
  m_vertex_buffer.bind();
}

float t = 0.0;

void renderer_t::render() {
  t += 0.01;

  transform_t &camera_transform = m_game.get_transform(m_game.get_camera());
  
  m_camera.move(camera_transform.position, camera_transform.rotation);
  
  m_gbuffer_target.bind();
  glViewport(0, 0, BUFFER_WIDTH, BUFFER_HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_gbuffer.bind();
  draw_entities();
  m_gbuffer_target.unbind();

  int c = 0;
  
  m_normal.bind(1);
  m_depth.bind(2);
  
  m_target[!c].bind();
  m_buffer[c].bind(0);
  draw_buffer(BUFFER_WIDTH, BUFFER_HEIGHT, m_ssr);
  m_target[!c].unbind();
  c = !c;
  
  m_target[!c].bind();
  m_buffer[c].bind(0);
  draw_buffer(BUFFER_WIDTH, BUFFER_HEIGHT, m_ssao);
  m_target[!c].unbind();
  c = !c;

  m_target[!c].bind();
  m_buffer[c].bind(0);
  m_water.uniform_float("g_time", t);
  draw_buffer(BUFFER_WIDTH, BUFFER_HEIGHT, m_water);
  m_target[!c].unbind();
  c = !c;

  m_target[!c].bind();
  m_buffer[c].bind(0);
  draw_buffer(BUFFER_WIDTH, BUFFER_HEIGHT, m_point_light_scatter);
  m_target[!c].unbind();
  c = !c;

  // m_target[0].bind();
  m_buffer[c].bind(0);
  draw_buffer(800, 800, m_tone_map);
  // m_target[0].unbind();
  
  // m_buffer[0].bind(0);
  // draw_buffer(800, 800, m_dither);
}

void renderer_t::draw_buffer(int width, int height, shader_t& shader) {
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shader.bind();
  m_meshes[MESH_PLANE].draw();
}

void renderer_t::draw_entities() {
  for (entity_t entity = 0; entity < m_game.entity_count(); entity++) {
    if (m_game.has_component(entity, HAS_MODEL | HAS_TRANSFORM)) {
      transform_t& transform = m_game.get_transform(entity);
      model_t& model = m_game.get_model(entity);
      
      mat4 T_rotation = mat4::rotate_zyx(transform.rotation);
      mat4 T_translation = mat4::translate(transform.position);
      mat4 T_scale = mat4::scale(transform.scale);
      
      m_camera.sub(T_rotation * T_scale * T_translation);
      m_materials[model.material].albedo.bind(0);
      m_materials[model.material].normal.bind(1);
      m_materials[model.material].roughness.bind(2);
      m_meshes[model.mesh].draw();
    }
  }
}

void renderer_t::init_assets() {
  mesh_builder_t mesh_builder;

  mesh_builder = mesh_builder_t();
  mesh_builder.push_quad(mat4::identity(), mat4::identity());
  m_meshes.push_back(m_vertex_buffer.push(mesh_builder.compile()));
  
  mesh_builder = mesh_builder_t();
  mesh_builder.push_cuboid(vec3(0.0), vec3(1.0));
  m_meshes.push_back(m_vertex_buffer.push(mesh_builder.compile()));
  
  texture_t& default_albedo = m_textures.emplace_back(1, 1, GL_RGBA, GL_RGBA32F, GL_UNSIGNED_BYTE, std::vector { 0xffffffffu });
  texture_t& default_normal = m_textures.emplace_back(1, 1, GL_RGBA, GL_RGBA32F, GL_UNSIGNED_BYTE, std::vector { 0xffff8080u });
  texture_t& default_roughness = m_textures.emplace_back(1, 1, GL_RGBA, GL_RGBA32F, GL_UNSIGNED_BYTE, std::vector { 0xff101010u });
  m_materials.push_back(material_t(default_albedo, default_normal, default_roughness));

  texture_t& brick_albedo = m_textures.emplace_back("assets/brick/albedo.jpg");
  texture_t& brick_normal = m_textures.emplace_back("assets/brick/normal.jpg");
  texture_t& brick_roughness = m_textures.emplace_back("assets/brick/roughness.jpg");
  m_materials.push_back(material_t(brick_albedo, brick_normal, brick_roughness));

  texture_t& grass_albedo = m_textures.emplace_back("assets/grass/albedo.jpg");
  texture_t& grass_normal = m_textures.emplace_back("assets/grass/normal.jpg");
  texture_t& grass_roughness = m_textures.emplace_back("assets/grass/roughness.jpg");
  m_materials.push_back(material_t(grass_albedo, grass_normal, grass_roughness));

  texture_t& tile_albedo = m_textures.emplace_back("assets/tile/albedo.jpg");
  texture_t& tile_normal = m_textures.emplace_back("assets/tile/normal.jpg");
  texture_t& tile_roughness = m_textures.emplace_back("assets/tile/roughness.jpg");
  m_materials.push_back(material_t(tile_albedo, tile_normal, tile_roughness));
}
