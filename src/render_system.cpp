#include "render_system.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>

#include "common.hpp"

void RenderSystem::drawTexturedMesh(Entity entity, const mat3 &projection) {
  TransformComponent &transformcomp = registry->transforms.get(entity);
  // Transformation code, see Rendering and Transformation in the template
  // specification for more info Incrementally updates transformation matrix,
  // thus ORDER IS IMPORTANT
  vec2 cameraTarget =
      registry->camera.get(registry->camera.entities[0]).cameraTarget;
  (void)cameraTarget;
  vec2 cameraPosition =
      registry->camera.get(registry->camera.entities[0]).cameraPosition;
  vec2 cameraFOV = registry->camera.get(registry->camera.entities[0]).cameraFOV;
  Transform transform;
  if (registry->UIelements.has(entity)) {
    transform.translate(cameraPosition);
    transform.translate(transformcomp.position * cameraFOV);
    transform.rotate(transformcomp.rotation);
    transform.scale(transformcomp.scale);
  } else {
    transform.translate(transformcomp.position);
    transform.rotate(transformcomp.rotation);
    transform.scale(transformcomp.scale);
  }

  assert(registry->renderRequests.has(entity));
  const RenderRequest &render_request = registry->renderRequests.get(entity);

  const GLuint used_effect_enum = (GLuint)render_request.used_effect;
  assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
  const GLuint program = (GLuint)effects[used_effect_enum];

  // Setting shaders
  glUseProgram(program);
  gl_has_errors();

  assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
  const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
  const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

  // Setting vertex and index buffers
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  gl_has_errors();

  // Input data location as in the vertex buffer
  if (render_request.used_effect == EFFECT_ASSET_ID::SALMON) {
    GLint in_position_loc = glGetAttribLocation(program, "in_position");
    GLint in_color_loc = glGetAttribLocation(program, "in_color");
    gl_has_errors();

    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(ColoredVertex), (void *)0);
    gl_has_errors();

    glEnableVertexAttribArray(in_color_loc);
    glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(ColoredVertex), (void *)sizeof(vec3));
    gl_has_errors();

    if (render_request.used_effect == EFFECT_ASSET_ID::SALMON) {
      // Light up?
      GLint light_up_uloc = glGetUniformLocation(program, "light_up");
      assert(light_up_uloc >= 0);

      // !!! TODO A1: set the light_up shader variable using glUniform1i,
      // similar to the glUniform1f call below. The 1f or 1i specified the type,
      // here a single int.

      gl_has_errors();
    }
  } else if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED ||
             render_request.used_effect == EFFECT_ASSET_ID::ANIMATED ||
             render_request.used_effect == EFFECT_ASSET_ID::SPACE ||
             render_request.used_effect == EFFECT_ASSET_ID::PARALLAXED) {
    GLint in_position_loc = glGetAttribLocation(program, "in_position");
    GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
    gl_has_errors();
    assert(in_texcoord_loc >= 0);

    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(TexturedVertex), (void *)0);
    gl_has_errors();

    glEnableVertexAttribArray(in_texcoord_loc);
    glVertexAttribPointer(
        in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
        (void *)sizeof(
            vec3));  // note the stride to skip the preceeding vertex position
    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    gl_has_errors();

    assert(registry->renderRequests.has(entity));
    GLuint texture_id =
        texture_gl_handles[(GLuint)registry->renderRequests.get(entity)
                               .used_texture];

    glBindTexture(GL_TEXTURE_2D, texture_id);
    gl_has_errors();

    if (render_request.used_effect == EFFECT_ASSET_ID::PARALLAXED) {
      GLint cameraTransform_uloc =
          glGetUniformLocation(program, "cameraTransform");
      assert(cameraTransform_uloc >= 0);
      glUniform2f(cameraTransform_uloc, cameraPosition[0], cameraPosition[1]);
    }

    if (render_request.used_effect ==
        EFFECT_ASSET_ID::ANIMATED) {  // update sprite animation parameters
      SpriteAnimation &anim = registry->spriteAnimations.get(entity);
      GLint rows_uloc = glGetUniformLocation(program, "rows");
      GLint cols_uloc = glGetUniformLocation(program, "cols");
      GLint animation_uloc = glGetUniformLocation(program, "animation");
      GLint frame_uloc = glGetUniformLocation(program, "frame");
      assert(rows_uloc >= 0);
      assert(cols_uloc >= 0);
      assert(animation_uloc >= 0);
      assert(frame_uloc >= 0);

      glUniform1i(rows_uloc, anim.rows);
      glUniform1i(cols_uloc, anim.columns);
      glUniform1i(animation_uloc, anim.animation);
      glUniform1i(frame_uloc, anim.frame);
    }

    if (render_request.used_effect ==
        EFFECT_ASSET_ID::SPACE) {  // check if stepped on, if so, light up
      Space &space = registry->spaces.get(entity);

      GLint lightUp_uloc = glGetUniformLocation(program, "lightUp");
      glUniform1i(lightUp_uloc, space.player_stepped_on);
    }
  } else if (render_request.used_effect == EFFECT_ASSET_ID::BOARD ||
             render_request.used_effect == EFFECT_ASSET_ID::PEBBLE ||
             render_request.used_effect == EFFECT_ASSET_ID::SALMON) {
    GLint in_position_loc = glGetAttribLocation(program, "in_position");
    GLint in_color_loc = glGetAttribLocation(program, "in_color");
    gl_has_errors();

    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(ColoredVertex), (void *)0);
    gl_has_errors();

    glEnableVertexAttribArray(in_color_loc);
    glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(ColoredVertex), (void *)sizeof(vec3));
    gl_has_errors();
  } else if (render_request.used_effect == EFFECT_ASSET_ID::CLOUD) {
    GLint in_position_loc = glGetAttribLocation(program, "in_position");
    GLint in_normal_loc = glGetAttribLocation(program, "in_normal");
    
    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(ColoredVertex), (void *)0);
    gl_has_errors();
    glEnableVertexAttribArray(in_normal_loc);
    glVertexAttribPointer(in_normal_loc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(ColoredVertex), (void *)0);
    gl_has_errors();

    GLuint time_uloc = glGetUniformLocation(program, "time");
    assert(time_uloc >= 0);

    glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
  }
  else {
    assert(false && "Type of render request not supported");
  }

  // Getting uniform locations for glUniform* calls
  GLint color_uloc = glGetUniformLocation(program, "fcolor");
  const vec3 color =
      registry->colors.has(entity) ? registry->colors.get(entity) : vec3(1);
  glUniform3fv(color_uloc, 1, (float *)&color);
  gl_has_errors();

  // Get number of indices from index buffer, which has elements uint16_t
  GLint size = 0;
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  gl_has_errors();

  GLsizei num_indices = size / sizeof(uint16_t);
  // GLsizei num_triangles = num_indices / 3;

  GLint currProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
  // Setting uniform values to the currently bound program
  GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
  glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
  GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
  glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
  gl_has_errors();
  // Drawing of num_indices/3 triangles specified in the index buffer
  glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
  gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// water
void RenderSystem::drawToScreen() {
  // Setting shaders
  // get the water texture, sprite mesh, and program
  glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::UIFOCUS]);
  gl_has_errors();
  // Clearing backbuffer
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, w, h);
  glDepthRange(0, 10);
  glClearColor(1.f, 0, 0, 1.0);
  glClearDepth(1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl_has_errors();
  // Enabling alpha channel for textures
  glDisable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);

  // Draw the screen texture on the quad geometry
  glBindBuffer(GL_ARRAY_BUFFER,
               vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
  glBindBuffer(
      GL_ELEMENT_ARRAY_BUFFER,
      index_buffers[(GLuint)GEOMETRY_BUFFER_ID::
                        SCREEN_TRIANGLE]);  // Note, GL_ELEMENT_ARRAY_BUFFER
                                            // associates indices to the bound
                                            // GL_ARRAY_BUFFER
  gl_has_errors();

  // set screen state related shader parameters
  const GLuint ui_focus_program = effects[(GLuint)EFFECT_ASSET_ID::UIFOCUS];
  // Set clock
  GLuint time_uloc = glGetUniformLocation(ui_focus_program, "time");
  GLuint screen_brightness_uloc =
      glGetUniformLocation(ui_focus_program, "screen_brightness");
  GLuint blur_size_uloc = glGetUniformLocation(ui_focus_program, "blur_size");
  GLboolean blur_uloc =
      glGetUniformLocation(ui_focus_program, "blur_fullscreen");
  GLboolean blur_partial_uloc =
      glGetUniformLocation(ui_focus_program, "blur_partial");
  GLuint blur_rect_position =
      glGetUniformLocation(ui_focus_program, "blur_rect_position");

  // update uniform with screen states
  ScreenState &screen = registry->screenStates.get(screen_state_entity);
  glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
  glUniform1f(screen_brightness_uloc, screen.screen_brightness);
  glUniform1f(blur_size_uloc, screen.blur_size);
  glUniform1i(blur_uloc, screen.blur_fullscreen);
  glUniform1i(blur_partial_uloc, screen.blur_partial);
  glUniform4fv(blur_rect_position, 1, (float *)&screen.blur_rect_position);

  gl_has_errors();
  // Set the vertex position and vertex texture coordinates (both stored in the
  // same VBO)
  GLint in_position_loc = glGetAttribLocation(ui_focus_program, "in_position");
  glEnableVertexAttribArray(in_position_loc);
  glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),
                        (void *)0);
  gl_has_errors();

  // Bind our texture in Texture Unit 0
  glActiveTexture(GL_TEXTURE0);

  glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
  gl_has_errors();
  // Draw
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
                 nullptr);  // one triangle = 3 vertices; nullptr indicates that
                            // there is no offset from the bound index buffer
  gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw() {
  // Getting size of window
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  gl_has_errors();

  // First render to the custom framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
  gl_has_errors();
  // Clearing backbuffer
  glViewport(0, 0, w, h);
  glDepthRange(0.00001, 10);
  glClearColor(0, 0, 1, 1.0);
  glClearDepth(1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);  // native OpenGL does not work with a depth buffer
                             // and alpha blending, one would have to sort
                             // sprites back to front
  gl_has_errors();

  mat3 projection_2D = createProjectionMatrix();

  // Draw all textured meshes that have a position and size component but are
  // not ui elements
  for (Entity entity : registry->renderRequests.entities) {
    if (!registry->transforms.has(entity) || registry->UIpasses.has(entity))
      continue;
    // Note, its not very efficient to access elements indirectly via the entity
    // albeit iterating through all Sprites in sequence. A good point to
    // optimize
    drawTexturedMesh(entity, projection_2D);
  }

  // draw particle systems
  for (Entity entity : registry->particleSystems.entities) {
    ParticleSystem &ps = registry->particleSystems.get(entity);
    if (ps.particles_alive.size() > 0) {
      // set shader
      const GLuint program =
          (GLuint)effects[(GLuint)EFFECT_ASSET_ID::TEXTURED_PARTICLE];
      glUseProgram(program);

      const GLuint vbo = vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SPRITE];
      const GLuint ibo = index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SPRITE];

      // Setting vertex and index buffers
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      gl_has_errors();

      GLint in_position_loc = glGetAttribLocation(program, "in_position");
      GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
      gl_has_errors();
      assert(in_texcoord_loc >= 0);

      glEnableVertexAttribArray(in_position_loc);
      glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                            sizeof(TexturedVertex), (void *)0);
      gl_has_errors();

      glEnableVertexAttribArray(in_texcoord_loc);
      // note the stride to skip the preceeding vertex position
      glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE,
                            sizeof(TexturedVertex), (void *)sizeof(vec3));
      // Enabling and binding texture to slot 0
      glActiveTexture(GL_TEXTURE0);
      gl_has_errors();

      GLuint texture_id = texture_gl_handles[(GLuint)ps.texture];

      glBindTexture(GL_TEXTURE_2D, texture_id);
      gl_has_errors();

      GLuint projection_loc = glGetUniformLocation(program, "projection");
      glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection_2D);
      gl_has_errors();

      // instance stuff
      unsigned int instanceVBO;
      glGenBuffers(1, &instanceVBO);
      glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
      glBufferData(GL_ARRAY_BUFFER,
                   sizeof(glm::vec2) * ps.particles_alive.size(),
                   &ps.particles_position[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glEnableVertexAttribArray(2);
      glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                            (void *)0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glVertexAttribDivisor(2, 1);

      unsigned int instanceVBOsize;
      glGenBuffers(1, &instanceVBOsize);
      glBindBuffer(GL_ARRAY_BUFFER, instanceVBOsize);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ps.particles_alive.size(),
                   &ps.particles_size[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glEnableVertexAttribArray(3);
      glBindBuffer(GL_ARRAY_BUFFER, instanceVBOsize);
      glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                            (void *)0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glVertexAttribDivisor(3, 1);
      gl_has_errors();

      GLint size = 0;
      glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
      gl_has_errors();

      GLsizei num_indices = size / sizeof(uint16_t);
      (void)num_indices;

      // glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
      glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 6, ps.particles_alive.size());
      gl_has_errors();
    }
  }

  // Truly render to the screen
  drawToScreen();

  // Third rendering pass to only render UI elements
  // repeat below lines to re-enable alpha blending after second pass
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);  // native OpenGL does not work with a depth buffer
                             // and alpha blending, one would have to sort
                             // sprites back to front

  for (Entity entity :
       registry->UIpasses
           .entities)  // Doing this for performance as opposed to above method.
                       // May cause ordering issues... TBD
  {
    if (!registry->transforms.has(entity) ||
        !registry->UIpasses.get(entity).display)
      continue;
    drawTexturedMesh(entity, projection_2D);
  }

  for (auto text : text_render_array) {
    _renderText(text.text_block, text.pos, text.scale, text.color,
                text.font_type, text.line_space);
  }
  text_render_array.clear();

  glfwSwapBuffers(window);
  gl_has_errors();
}

mat3 RenderSystem::createProjectionMatrix() {
  vec2 cameraTarget =
      registry->camera.get(registry->camera.entities[0]).cameraTarget;
  vec2 cameraPosition =
      registry->camera.get(registry->camera.entities[0]).cameraPosition;
  vec2 cameraFOV = registry->camera.get(registry->camera.entities[0]).cameraFOV;
  vec2 cameraMotion =
      cameraTarget - cameraPosition;  // Get vector to target position
  cameraMotion =
      clamp(cameraMotion, -10.0f, 10.0f);  // max velocity to our camera

  cameraPosition = cameraPosition + cameraMotion;
  registry->camera.get(registry->camera.entities[0]).cameraPosition =
      cameraPosition;

  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  gl_has_errors();
  // Fake projection matrix, scales with respect to window coordinates
  // float aspectRatio = float(h) / float(w);

  float left = cameraPosition[0] - cameraFOV[0] / 2;
  float right = left + cameraFOV[0];
  float top = cameraPosition[1] - cameraFOV[1] / 2;
  float bottom = top + cameraFOV[1];
  // float right = left + (float)w / screen_scale;
  // float bottom = top + (float)h / screen_scale;

  float sx = 2.f / (right - left);
  float sy = 2.f / (top - bottom);
  float tx = -(right + left) / (right - left);
  float ty = -(top + bottom) / (top - bottom);
  return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

void RenderSystem::add_text_to_be_rendered(std::vector<std::string> text_block,
                                           glm::vec2 pos_percent, float scale,
                                           glm::vec3 color, FONTS font_type,
                                           float line_space) {
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);

  assert(pos_percent.x > 0 && pos_percent.x < 1);
  assert(pos_percent.y > 0 && pos_percent.y < 1);

  glm::vec2 pos = {pos_percent.x * w, pos_percent.y * h};
  Text2Display new_text = {text_block, pos,       scale,
                           color,      font_type, line_space};
  text_render_array.push_back(new_text);
}

void RenderSystem::add_text_block_to_be_rendered(
    const std::string text_block, const int LINE_LENGTH, glm::vec2 pos_percent,
    float scale, glm::vec3 color, FONTS font_type, float line_space) {
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);

  assert(pos_percent.x > 0 && pos_percent.x < 1);
  assert(pos_percent.y > 0 && pos_percent.y < 1);

  glm::vec2 pos = {pos_percent.x * w, pos_percent.y * h};

  std::vector<std::string> story_text_block;

  // count is the number of characters to display

  int i = 0;  // current character block to track
  int count = text_block.size();
  int line_text = 0;
  bool finished = false;
  int last_line_start = 0;
  while (!finished) {
    if (i >= count) {
      // push the last line
      story_text_block.push_back(
          text_block.substr(last_line_start, count - last_line_start));
      finished = true;
      continue;
    }

    // push a line
    if (i > ((line_text + 1) * LINE_LENGTH) && text_block[i] == ' ') {
      story_text_block.push_back(
          text_block.substr(last_line_start, i - last_line_start));
      line_text++;
      // printf("line len %d\n", i - last_line_start);
      last_line_start = i;
    }

    i += 1;
  }

  Text2Display new_text = {story_text_block, pos,       scale, color,
                           font_type,        line_space};
  text_render_array.push_back(new_text);
}

void RenderSystem::render_text_only(glm::vec3 background_color) {
  // Getting size of window
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  gl_has_errors();

  // First render to the custom framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
  gl_has_errors();
  // Clearing backbuffer
  glViewport(0, 0, w, h);
  glDepthRange(0.00001, 10);
  glClearColor(0, 0, 1, 1.0);
  glClearDepth(1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);  // native OpenGL does not work with a depth buffer
                             // and alpha blending, one would have to sort
                             // sprites back to front
  gl_has_errors();

  glClearColor(background_color.r, background_color.g, background_color.b,
               1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Truly render to the screen
  drawToScreen();

  for (auto text : text_render_array) {
    _renderText(text.text_block, text.pos, text.scale, text.color,
                text.font_type, text.line_space);
  }
  text_render_array.clear();

  glfwSwapBuffers(window);
  gl_has_errors();
}

void RenderSystem::render_text_with_background(TEXTURE_ASSET_ID bkg_id,
                                               vec2 position, vec2 scale) {
  auto background = Entity();
  Mesh &mesh = getMesh(GEOMETRY_BUFFER_ID::SPRITE);
  registry->meshPtrs.emplace(background, &mesh);

  // Initialize the position, scale, and physics components
  // Setting initial motion values
  TransformComponent &transform = registry->transforms.emplace(background);
  transform.position = position;
  transform.rotation = 0.f;
  transform.scale = scale;

  // Create and (empty) Cat component to be able to refer to all cats
  registry->renderRequests.insert(
      background,
      {bkg_id, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE});

  // Getting size of window

  int w, h;
  glfwGetFramebufferSize(window, &w, &h);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  gl_has_errors();

  // First render to the custom framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
  gl_has_errors();
  // Clearing backbuffer
  glViewport(0, 0, w, h);
  glDepthRange(0.00001, 10);
  glClearColor(0, 0, 1, 1.0);
  glClearDepth(1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);  // native OpenGL does not work with a depth buffer
                             // and alpha blending, one would have to sort
                             // sprites back to front
  gl_has_errors();

  mat3 projection_2D = createProjectionMatrix();

  drawTexturedMesh(background, projection_2D);

  registry->renderRequests.remove(background);

  // Truly render to the screen
  drawToScreen();

  for (auto text : text_render_array) {
    _renderText(text.text_block, text.pos, text.scale, text.color,
                text.font_type, text.line_space);
  }
  text_render_array.clear();

  glfwSwapBuffers(window);
  gl_has_errors();
}
void RenderSystem::_renderText(std::vector<std::string> text_block,
                               glm::vec2 pos, float scale, glm::vec3 color,
                               FONTS font_type, float line_space) {
  // activate corresponding render state
  const GLuint program = (GLuint)effects[(GLuint)EFFECT_ASSET_ID::TEXT];
  // Setting shaders
  glUseProgram(program);

  GLint in_position_loc = glGetAttribLocation(program, "in_position");

  // Generate vertex arrays and buffers
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(in_position_loc);
  glVertexAttribPointer(in_position_loc, 4, GL_FLOAT, GL_FALSE,
                        4 * sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindVertexArray(0);
  gl_has_errors();

  glUniform3f(glGetUniformLocation(program, "fcolor"), color.x, color.y,
              color.z);
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<float>(w), 0.0f, static_cast<float>(h));
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE,
                     glm::value_ptr(projection));
  gl_has_errors();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glActiveTexture(GL_TEXTURE0);
  // glBindVertexArray(VAO);

  // iterate through all characters
  float max_vertical_height = 0;
  float pos_line_y = pos.y;
  float pos_line_initial_x = pos.x;

  for (auto text : text_block) {
    pos.x = pos_line_initial_x;
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
      Character ch;
      switch (font_type) {
        case FONTS::REGULAR:
          ch = regular_characters[*c];
          break;
        case FONTS::BOLD:
          ch = bold_characters[*c];
          break;
        case FONTS::ITALIC:
          ch = italic_characters[*c];
          break;
        case FONTS::LIGHT:
          ch = light_characters[*c];
          break;
        default:
          break;
      }
      if (ch.Size.y > max_vertical_height) {
        max_vertical_height = ch.Size.y;
      }
      float xpos = pos.x + ch.Bearing.x * scale;
      float ypos = pos_line_y - (ch.Size.y - ch.Bearing.y) * scale;

      float w = ch.Size.x * scale;
      float h = ch.Size.y * scale;
      // update VBO for each character
      float vertices[6][4] = {
          {xpos, ypos + h, 0.0f, 0.0f},    {xpos, ypos, 0.0f, 1.0f},
          {xpos + w, ypos, 1.0f, 1.0f},

          {xpos, ypos + h, 0.0f, 0.0f},    {xpos + w, ypos, 1.0f, 1.0f},
          {xpos + w, ypos + h, 1.0f, 0.0f}};
      // render glyph texture over quad
      glBindTexture(GL_TEXTURE_2D, ch.TextureID);
      // update content of VBO memory
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      // render quad
      glDrawArrays(GL_TRIANGLES, 0, 6);
      // now advance cursors for next glyph (note that advance is number of 1/64
      // pixels)
      pos.x += (ch.Advance >> 6) *
               scale;  // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    pos_line_y -= max_vertical_height * (1.0 + line_space);
  }
  // unbind the vertex array
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
  gl_has_errors();
}