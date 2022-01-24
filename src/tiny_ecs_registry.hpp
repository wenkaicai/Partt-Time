#pragma once
#include <vector>

#include "tiny_ecs.hpp"

class ECSRegistry {
 public:
  ComponentContainer<RenderRequest> renderRequests;
  ComponentContainer<DeathTimer> deathTimers;
  ComponentContainer<TransformComponent> transforms;
  ComponentContainer<Velocity> velocities;
  ComponentContainer<SpriteAnimation> spriteAnimations;
  ComponentContainer<UIelement> UIelements;
  ComponentContainer<UIPass> UIpasses;
  ComponentContainer<Collision> collisions;
  ComponentContainer<Collider> colliders;
  ComponentContainer<Mesh *> meshPtrs;
  ComponentContainer<ScreenState> screenStates;
  ComponentContainer<DebugComponent> debugComponents;
  ComponentContainer<vec3> colors;
  ComponentContainer<Camera> camera;
  ComponentContainer<Space> spaces;  // to be removed
  ComponentContainer<ParticleSystem> particleSystems; // has to be here so render system can know about the texture that the system uses
  //ComponentContainer<Particle> particles; // has to be here so rendersystem can call drawArrayInstanced

  ECSRegistry() {
    registry_list.push_back(&renderRequests);
    registry_list.push_back(&deathTimers);
    registry_list.push_back(&transforms);
    registry_list.push_back(&velocities);
    registry_list.push_back(&spriteAnimations);
    registry_list.push_back(&UIelements);
    registry_list.push_back(&UIpasses);
    registry_list.push_back(&collisions);
    registry_list.push_back(&colliders);
    registry_list.push_back(&meshPtrs);
    registry_list.push_back(&screenStates);
    registry_list.push_back(&debugComponents);
    registry_list.push_back(&colors);
    registry_list.push_back(&camera);
    registry_list.push_back(&spaces);
    registry_list.push_back(&particleSystems);
    //registry_list.push_back(&particles);
  }

  // releases all associated resources
  virtual ~ECSRegistry() = default;

  void clear_all_components() {
    for (ContainerInterface *reg : registry_list) reg->clear();
  }

  void list_all_components() {
    printf("Debug info on all registry entries:\n");
    for (ContainerInterface *reg : registry_list)
      if (reg->size() > 0)
        printf("%4d components of type %s\n", (int)reg->size(),
               typeid(*reg).name());
  }

  void list_all_components_of(Entity e) {
    printf("Debug info on components of entity %u:\n", (unsigned int)e);
    for (ContainerInterface *reg : registry_list)
      if (reg->has(e)) printf("type %s\n", typeid(*reg).name());
  }

  void remove_all_components_of(Entity e) {
    for (ContainerInterface *reg : registry_list) reg->remove(e);
  }

 protected:
  // Callbacks to remove a particular or all entities in the system
  std::vector<ContainerInterface *> registry_list;
};