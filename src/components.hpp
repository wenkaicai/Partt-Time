#pragma once
#include <unordered_map>
#include <vector>

#include "../ext/stb_image/stb_image.h"
#include "common.hpp"

enum class ITEMS {
  NONE,
  MINI_MUSHROOM,
  MEGA_MUSHROOM,
};

// Player component
struct Player {
  int move_freely =
      0;  // can the active player move freely (or is it on the board?)
  int points = 10;
  int standing = 0;  // 0 = first place, 1 = second...
  ITEMS item_1 = ITEMS::NONE;
  ITEMS item_2 = ITEMS::NONE;
  ITEMS item_3 = ITEMS::NONE;
  ITEMS player_effect = ITEMS::NONE;  // updated after an item is used
  bool squished = 0;  // flag so that we don't keep stealing coins after a
  // player has been squished
  int player_id;
};

// The data for movement along the board (ie. how many spaces left to move,
// current space the player is on, etc.
struct PlayerBoardMovement {
  int roll_count_left = 0;
  Entity current_space;
  Entity target_space;  // The next space we want to walk to (updates i
                        // roll_count > 0 and we enter a new space)
};

struct Camera {
  vec2 cameraPosition = {600, 337};
  vec2 cameraTarget = {600, 337};
  vec2 cameraFOV = {1200, 675};
};

// All data relevant to the transformation of objects
struct TransformComponent {
  vec2 position = {0, 0};
  float rotation = 0;
  vec2 scale = {10, 10};
};

struct Velocity {
  vec2 velocity = {0, 0};
};

struct SpriteAnimation {
  int rows = 1;
  int columns = 1;

  float animation_speed = 0.0f;  // in frames per second
  float elapsed_time = 0.0f;

  int animation = 0;
  int frame = 0;
};

struct UIelement  // TODO rename to screen_space_entity or something like that
{
  // During rendering the position of the sprite in the transform component will
  // be in screen space.
};

struct UIPass {
  // Flag such that the entity is drawn on the third pass (After screen
  // darkenning and blurring)
  bool display = 1;
};

// Stucture to store collision information
struct Collision {
  // Note, the first object is stored in the ECS container.entities
  Entity other;  // the second object involved in the collision
  Collision(Entity &other) { this->other = other; };
};
// Flag to check for collision (instead of checking all entities with a
// Transform component)
struct Collider {};

enum class SPACE_TYPE {
  SPACE_BLUE,
  SPACE_RED,
  SPACE_MUSHROOM,
  SPACE_BOMB,
  SPACE_SPRING,
  SPACE_QUESTION,
  SPACE_FORTUNE,
  SPACE_DIRECTION
};

struct Space {
  SPACE_TYPE type = SPACE_TYPE::SPACE_BLUE;  // type of space
  int player_stepped_on = 0;  // plays sound when player enters space and change
                              // to light up texture (UV offset on shader).
  int takes_movement = 1;     // reduces player's roll count by this much
  Entity next_space;
};

struct DirectionSpace {
  Entity space1;
  Entity space2;
  Entity space3;
  int space1_key;
  bool space1_needMini = 0;  // do we need to be mini to go there
  int space2_key;
  bool space2_needMini = 0;
  int space3_key;
  bool space3_needMini = 0;
};

// Data structure for toggling debug mode
struct Debug {
  bool in_debug_mode = 0;
  bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState {
  float screen_brightness = 1;
  float blur_size = 0.02;
  bool blur_fullscreen = false;
  float darken_screen_factor = -1;
  bool blur_partial = false;
  vec4 blur_rect_position = {0, 0, 0, 0};  // x, y, w, h
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent {
  // Note, an empty struct has size 1
};

// A timer that will be associated to dying salmon
struct DeathTimer {
  float counter_ms = 1000;
};

struct TextTimer {
  float idle_ms = 1000;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl &
// salmon.vs.glsl)
struct ColoredVertex {
  vec3 position;
  vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex {
  vec3 position;
  vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh {
  static bool loadFromOBJFile(std::string obj_path,
                              std::vector<ColoredVertex> &out_vertices,
                              std::vector<uint16_t> &out_vertex_indices,
                              vec2 &out_size);
  vec2 original_size = {1, 1};
  std::vector<ColoredVertex> vertices;
  std::vector<uint16_t> vertex_indices;
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to
 * it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
  SPACE_BLUE,
  SPACE_RED,
  SPACE_MUSHROOM,
  SPACE_BOMB,
  SPACE_SPRING,
  SPACE_QUESTION,
  SPACE_FORTUNE,
  SPACE_DIRECTION,
  DOGE,
  HELP_MAINBOARD,
  DICE,
  BKGD_0,
  BKGD_1,
  DIGITS_WHITE,
  PLAYER_INFO,
  RANKINGS,
  ITEMS,
  ITEMCARDS,
  TEXT,
  FISH,
  // TURTLE,
  ENEMY,
  BLOCK,
  BKGD_SHOWER,
  BLOCK_1,
  CAT,
  FOOD,
  PLAYER_DOGE,
  BKGD_PLANIT,
  BKGD_MAC,
  ROCK_MAC,
  PLANET_PLANIT,
  ENERGY_PLANIT,
  DOGE_ROCKET,
  DOGE_MAC,
  COIN,
  BKGD_CONSTRAINED,
  DOGE_CONSTRAINED,
  BKGD_DAYCARE,
  BKGD_GESTURE,
  CHEW_TOYS,
  FOOD_BOWL_FULL,
  FOOD_BOWL_EMPTY,
  WATER_BOWL_FULL,
  WATER_BOWL_EMPTY,
  TEXTURE_COUNT
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
  COLOURED,
  PEBBLE,
  BOARD,
  TEXTURED,
  UIFOCUS,
  SPACE,
  ANIMATED,
  PARALLAXED,
  SALMON,
  WATER,
  TEXT,
  TEXTURED_PARTICLE,
  CLOUD,
  EFFECT_COUNT
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
  SALMON,
  BOARD,
  CLOUD,
  SPRITE,
  PEBBLE,
  DEBUG_LINE,
  SCREEN_TRIANGLE,
  GEOMETRY_COUNT
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
  TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
  EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
  GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

struct ParticleSystem {
  float particleSystemLifetime = 2000;  // how long the particle system lives
                                        // for before deleting itself in ms
  float spawningRate = 0.1f;  // how often a particle spawns in seconds
  float spawningAngle =
      90.0f;          // angle to spawn particles in (degrees, 0 is axis {1,0})
  int coneAngle = 0;  // makes it spawn around a cone of angle coneAngle from
                      // the axis specified by spawningAngle
  float initialSpeed = 500.0f;
  float speedRandomness =
      0.25f;  // float from 0 - 1 where 0 is no ramdomness and where 1 can spawn
              // with speed between 0 and initialSpeed

  float initialLifetime = 5000.0f;  // in milliseconds how long an individual
                                    // particle lives for before killing itself
  float lifetimeRandomness = 0.25f;

  float particleSize = 50.0f;            // size of particle sprites
  float particleSizeRandomness = 0.25f;  // randomness to size

  vec2 particleAcceleration = {0, 1000};

  TEXTURE_ASSET_ID texture = TEXTURE_ASSET_ID::COIN;  // texture to use

  float life = 0.0f;  // current life of system, not a parameter will only die
                      // after it reaches lifetime and all particles are dead
  float particleSpawnTimeout = 0.0f;
  int num_alive = 0;

  // particles the system has emmited
  std::vector<bool> particles_alive;
  std::vector<vec2> particles_position;
  std::vector<vec2> particles_velocity;
  std::vector<float> particles_size;
  std::vector<float> particles_life;
};

/**
 * @brief GameMode of... Game
 *
 */
enum class GameMode {
  MAC_GAME,
  PLANIT_GAME,
  SHOWER_GAME,
  CONSTRAINED_CHAOS_GAME,
  DAYCARE_GAME,
  GAME_COUNT
};

// temporary light up timer when salmon scores
struct LightUp {
  float light_ms = 1000;
};

struct FishAi {
  float distX = 0.f;
  float distY = 0.f;
  float diff = 0.f;
};

struct TurtleAi {
  float distX = 0.f;
  float distY = 0.f;
};

// PLANIT COMPONENTS
struct Planet {};
struct Target {};

// SHOWER COMPONENTS
struct Acceleration {
  vec2 acceleration = {0, 300};  // positive is down
};
struct Point {
  int player_points = 0;
};
struct Grounded {};  // flag if player is touching ground and can jump
// Turtles and pebbles have a hard shell
struct HardShell {};
// Fish and Salmon have a soft shell
struct SoftShell {};
struct EnemyAi {};
struct Block {};
struct Bird {};

// MAC COMPONENTS
struct Rock {};

// DAYCARE COMPONENTS
enum class DC_ACTIVITY {
  NONE,
  DRINKING,
  EATING,
  PLAYING,
};
struct DC_Draggable {};
struct DC_ProgressBar {
  DC_ACTIVITY type = DC_ACTIVITY::DRINKING;
};
struct DC_Fighter {};
struct DC_Puppy {
  // task progresses
  float drinking = 0.f;
  float eating = 0.f;
  float playing = 0.f;
};
struct DC_FoodBowl {
  float amount = 1.f;
};
struct DC_WaterBowl {
  float amount = 1.f;
};
struct DC_ChewToy {};
struct DC_Obstacle {};
struct DC_DebugLine {};
struct DC_WanderTarget {
  vec2 target = vec2(0, 0);
};
