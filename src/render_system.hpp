#pragma once

#include <SDL.h>
#include <ft2build.h>

#include <array>
#include <map>
#include <memory>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"
#include FT_FREETYPE_H

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
  /**
   * The following arrays store the assets the game will use. They are loaded
   * at initialization and are assumed to not be modified by the render loop.
   *
   * Whenever possible, add to these lists instead of creating dynamic state
   * it is easier to debug and faster to execute for the computer.
   */
  std::array<GLuint, texture_count> texture_gl_handles;
  std::array<ivec2, texture_count> texture_dimensions;

  // Make sure these paths remain in sync with the associated enumerators.
  // Associated id with .obj path
  const std::vector<std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths = {
      std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::BOARD,
                                                 mesh_path("board.obj")),
      std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SALMON,
                                                 mesh_path("salmon.obj")),
      std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::CLOUD,
                                                 mesh_path("cloud_3d.obj"))
      // specify meshes of other assets here
  };

  // Make sure these paths remain in sync with the associated enumerators.
  const std::array<std::string, texture_count> texture_paths = {
      textures_path("space_blue.png"),
      textures_path("space_red.png"),
      textures_path("space_mushroom.png"),
      textures_path("space_bomb.png"),
      textures_path("space_spring.png"),
      textures_path("space_question.png"),
      textures_path("space_fortune.png"),
      textures_path("space_direction.png"),
      textures_path("doge.png"),
      textures_path("help_mainboard_controls.png"),
      textures_path("dice.png"),
      textures_path("bkgd_0.png"),
      textures_path("bkgd_1.png"),
      textures_path("digits_white.png"),
      textures_path("player_info.png"),
      textures_path("rankings.png"),
      textures_path("items.png"),
      textures_path("item_cards.png"),
      textures_path("text.png"),
      textures_path("fish.png"),
      // textures_path("turtle.png"),
      textures_path("enemy.png"),
      textures_path("block.png"),
      textures_path("bkgd_shower.png"),
      textures_path("block_1.png"),
      textures_path("cat.png"),
      textures_path("food.png"),
      textures_path("player_doge.png"),
      textures_path("bkgd_planit.png"),
      textures_path("bkgd_mac.png"),
      textures_path("rock_mac.png"),
      textures_path("planet_planit.png"),
      textures_path("energy_planit.png"),
      textures_path("doge_rocket.png"),
      textures_path("doge_mac.png"),
      textures_path("coin.png"),
      textures_path("bkgd_constrained.png"),
      textures_path("doge_constrained.png"),
      textures_path("bkgd_daycare.png"),
      textures_path("bkgd_gesture.png"),
      textures_path("chew_toys.png"),
      textures_path("food_bowl_full.png"),
      textures_path("food_bowl_empty.png"),
      textures_path("water_bowl_full.png"),
      textures_path("water_bowl_empty.png"),
  };

  std::array<GLuint, effect_count> effects;
  // Make sure these paths remain in sync with the associated enumerators.
  const std::array<std::string, effect_count> effect_paths = {
      shader_path("coloured"), shader_path("pebble"),
      shader_path("board"),    shader_path("textured"),
      shader_path("UIfocus"),  shader_path("space"),
      shader_path("animated"), shader_path("parallaxed"),
      shader_path("salmon"),   shader_path("water"),
      shader_path("text"),     shader_path("textured_particle"),
      shader_path("cloud")};

  std::array<GLuint, geometry_count> vertex_buffers;
  std::array<GLuint, geometry_count> index_buffers;
  std::array<Mesh, geometry_count> meshes;

 public:
  enum class FONTS { REGULAR, BOLD, ITALIC, LIGHT };
  // Initialize the window
  bool init(std::shared_ptr<ECSRegistry> registry, int width, int height,
            GLFWwindow *window);

  template <class T>
  void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices,
                     std::vector<uint16_t> indices);

  void initializeGlTextures();

  void initializeGlEffects();

  void initializeGlMeshes();
  Mesh &getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

  void initializeGlGeometryBuffers();
  // Initialize the screen texture used as intermediate render target
  // The draw loop first renders to this texture, then it is used for the water
  // shader
  bool initScreenTexture();

  // Destroy resources associated to one or all entities created by the system
  ~RenderSystem();

  // Draw all entities
  void draw();

  mat3 createProjectionMatrix();

  /**
   * @brief public facing interface of adding text.
   *
   * @param text text that should only includes 128 ASCII, includes array of
   * text.
   * @param pos_percent relative to the bottom left corner, in percentage
   * (0...1)
   * @param scale default: 1 -> 45 px
   * @param color RGB color in float
   * @param font_type Font type: {Regular, Bold}
   */
  void add_text_to_be_rendered(std::vector<std::string> text_block,
                               glm::vec2 pos_percent, float scale,
                               glm::vec3 color, FONTS font_type,
                               float line_space);

  /**
   * @brief
   *
   * @param text_block
   * @param LINE_LENGTH
   * @param pos_percent
   * @param scale
   * @param color
   * @param font_type
   */
  void add_text_block_to_be_rendered(const std::string text_block,
                                     const int LINE_LENGTH,
                                     glm::vec2 pos_percent, float scale,
                                     glm::vec3 color, FONTS font_type,
                                     float line_space);

  /**
   * @brief Just render the text
   *
   * @param background_color
   */
  void render_text_only(glm::vec3 background_color);

  /**
   * @brief
   *
   * @param bkg_id
   * @param position
   * @param scale
   */
  void render_text_with_background(TEXTURE_ASSET_ID bkg_id, vec2 position,
                                   vec2 scale);

 private:
  // Internal drawing functions for each entity type
  void drawTexturedMesh(Entity entity, const mat3 &projection);
  void drawToScreen();

  // Window handle
  GLFWwindow *window;
  float screen_scale;  // Screen to pixel coordinates scale factor (for apple
                       // retina display?)

  // Screen texture handles
  GLuint frame_buffer;
  GLuint off_screen_render_buffer_color;
  GLuint off_screen_render_buffer_depth;

  Entity screen_state_entity;

  // holds the scene state
  std::shared_ptr<ECSRegistry> registry;

  // text related functions
  struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2 Size;         // Size of glyph
    glm::ivec2 Bearing;      // Offset from baseline to left/top of glyph
    long int Advance;        // Offset to advance to next glyph
  };

  std::map<char, Character> regular_characters;
  std::map<char, Character> bold_characters;
  std::map<char, Character> italic_characters;
  std::map<char, Character> light_characters;

  GLuint VAO, VBO;

  /**
   * @brief private text renderer that actually render the text
   *
   * @param text
   * @param pos
   * @param scale
   * @param color
   */
  void _renderText(std::vector<std::string> text_block, glm::vec2 pos,
                   float scale, glm::vec3 color, FONTS font_type,
                   float line_space);
  struct Text2Display {
    std::vector<std::string> text_block;
    glm::vec2 pos;
    float scale;
    glm::vec3 color;
    FONTS font_type;
    float line_space;
  };
  std::vector<Text2Display> text_render_array;
};

bool loadEffectFromFile(const std::string &vs_path, const std::string &fs_path,
                        GLuint &out_program);
