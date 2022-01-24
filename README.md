# Party Time

## Milestone 4

### Gameplay III

Entry point: `/src`

The game is now substantially more polished and unified. The assets for all the mini games match up and shading effects are mostly finalized.

### Swarm behavior

Entry point: `src/scenes/shower/systems/world_system.cpp`

Place holder "circles" are rendered in the background of the shower mini game to give the appearance of swarms of fireflies. This was built using
a boid flocking algorithm that loops through all entities with "Bird" components and calculates their velocities based on neighboring entities'
velocities.

### Advanced geometry shader

Entry point: `shaders/cloud.vs.glsl`

Clouds now spawn in the shower/steal the energy mini-game. The ckouds use an advanced vertex shader to transform the vertices periodically depending on time, and normals to create a cloud bubbling effect.

### Light

Entry point: `shaders/cloud.vs.glsl`

The clouds also feature lighting calculations in the vertex shader (Gourard diffuse shading model) with lighting calculated per-vertex based on normal and direction to light source (top middle of screen).


### Game balance

Entry point: `/doc/survive_in_space_game_balance.xlsx/`

Test the relationship between the maximum amount of rocks that may appear on our screen and the player's survival time. When the maximum number of rocks is 10, 15, 20, 25, 30, 35, 
we test the instances. Testing the scenarios where the maximum rocks are too few or too many are meaningless since when the rocks are too few, players may be able to survive for 
a long period. And also when the rocks are too many, players may be killed before all of the rocks have been popping out. We evaluate ten groups of the survival time data for 
each case and take the average survival time as the final value. Following that, we make a plot to compare the Survive time to Max Rocks. The association appears to be about 
negative linear. Because this is a mini-game for our main board, we're planning to go with a half-minute survival time. As a result, we've increased our final Max number of rocks 
from 20 (avg 29.9s) to 25 (avg 41.3s), which is almost 10 seconds less compared to our previous data.

### Mouse gestures

Entry point: `src/scenes/daycare/systems/world_system.cpp`

In order to refill the food and water bowls for the puppies in the daycare mini game, the player must trace a mouse gesture by picking up an empty bowl using the "F" key and the mouse.
This was accomplished by interpolating numerous points along the gesture path and checking whether the mouse is near the next gesture point every time the mouse moves. If the mouse
gets too far from the next point in the path, the gesture is failed and the player must restart the gesture.

## Milestone 3

### Gameplay II

Entry point: `/src`

New mini-games and polishing of old mini-games. The board now displays the points all player has and mushroom items give more effects (player can steal coins if giant, or get access to special paths when they are mini). PLayer rankings are also now displayed.

### Simple Pathfinding

Entry point: `/src/scenes/daycare/`

The puppies (the brown dots) path find around the obstacles (pink dots in the center) in the daycare. The might slightly overlap with the obstacles but they will never walk through
them. This is intentional. The path finding is easier to see using debug mode (see `Debugging Graphics`).

### Debugging Graphics

Entry point: `/src/scenes/daycare/`

When pressing `D` while playing the daycare mini game, the dogs' path options will appear as eight distinct directions. As a puppy approaches one of the obstacles (pink dots in the center), the invalid directions that the puppy will avoid will turn red.

### Particle Systems

Entry point: `/src/render_system.cpp::310`

Particle systems integrated onto the board scene to shoot up coins when the player gains coins. Uses glDrawArrayInstance to allow 1000s of particles (shown when the player lands on a fortune space (rainbow star high up on the map))

### External Integration

Entry point: `/src/render_system_init.cpp::44'

Freetype external library is used to display text onto screen for tutorial/story/points/controls etc.

### Tutorial

Entry point: `/src/scenes/switch/scene.hpp::78`

All mini-games now show the controls for that mini-game in a tutorial/story page before the mini-game is played.

### Story

Entry point: `/src/scenes/switch/scene.hpp::48`

All mini-games and the board now have a story accompanying them that is displayed just before the mini-game is played. OVerall story is about the DOGE kingdom vs the cats.

### Non-Convex Collisions

Entry point: `\src\scenes\ConcaveChaos\systems\physics_system.cpp` (If not present, then I was not able to fix the jank before the due date, so apologies if there is any confusion when grading)

Mini-game implements an ear-clipping triangulation formula to split non-convex shapes into triangles. Then, when non-convex polygons get close, their triangles are checked for collisions, and the polygons are moved apart by the distance of the overlap.
The objective of the game is to push away the polygons to the edges of the screen as quickly as possible.

### Constrained Physics

Entry point: `/src/scenes/ConstrainedPhysics/physiscs_system.cpp`

Mini-game implements spring-like forces to pull killer chains that kill the player if they come in contact with it. The player must get to the other side to survive.

### Hand Gestures w/ OpenCV

Entry point: prototype_cv2 BRANCH. Shower mini-game

This is a demo where we use OpenCV to get hand gesture (rotation) from Zoom to control multiple players (max 4) on the shower mini-game at the same time. Demo was between TA and Yiyi as it would required Gb of installation.

## Milestone 2

### Gameplay I

Entry point: `/src`

The game now has more gameplay features. For the board: players now have items and directions to choose. We have added 1 mini-game as well.

### State Machine

Entry point: `/src/scenes/board/world_system.cpp`

The main board has a "board state" that keeps track of what the current player is doing. For example, our starting node is "waiting for player confirmation" where we wait for the player to press SPACE to confirm the handing off of controls, from then the state moves on to "rolling" where the die is seen rolling, there the player can either press space to roll the die and move on to the "moving" state, or B to use an item where it moves to the "picking an item" state. There are more states in between the handle animation time outs such as when the player lands on a space (waits a given amount of time to play SFX) or when the player crosses an intersection we wait for input from the player.

### Adaptive resolution

Entry point: `/src/window_manager.cpp`

The game now has a fixed aspect-ratio and the camera has a fixed FOV independent of the window screen size

### Advanced Fragment Shader

Entry point: `/shaders/UIFocus.fs.glsl`

The game now had 3 rendering passes. First one renders most of the entities as normal, second pass is the deferred shading with a Gaussian blur effect & fading, the 3rd rendering pass renders the sprites that contain the UI pass component so that we can let the players "focus" on the UI elements when picking items for example.

### Save & Reload

Entry point: `/src/scenes/board/world_system.cpp`

If the player presses "S" or "L" while the player is waiting confirmation or rolling the game will Save and Load respectively.

### Audio Feedback

Entry point: `/src/scenes/board/world_system.cpp`

The game now features a lot of audio SFX to give feedback to the player (ie. using an item, moving, rolling the die, hitting the die, landing on spaces, no item error, etc.).

### Collision Resolution

Entry point: `/src/scenes/mac/world_system.cpp` and `/src/scenes/mac/physics_system.cpp`

The mini-games feature collision resolution so the rocks don't pass through each other and the player can't go outside the screen.

### Precise Collision / Advanced Physics

Entry point: `/src/scenes/mac/physics_system.cpp` and `/src/scenes/PrecisePhysicsMiniGame/physics_system.cpp`

The mini-game under `/src/scenes/mac` contains Precise Collision. The salmon mesh demonstrates precise collision with the borders of the mini-game, and we use convex polygons to approximate the other meshes. This enables the salmon to bounce off each other in a relatively real-life way.
The mini-game under `src/scenes/PrecisePhysicsMiniGame` contains Precise Physics. We utilize mid-point euler to approximate gravitational pull, which the player uses to hit the target.

### Advanced Scene System

Entry point: `/src/scenes/`

The game now splits the mini games and board into their own "scenes" each with their own registries, and calls to their own physics, ai, etc. systems and the window system is now decoupled from the world system so that scenes can share the same window but have separate world systems.

## Milestone 1

### Delay-agnostic design

Entry point: `/src`

The game is turn-based which allows for each player to make actions when they have received control from Zoom for the presenter’s input devices. We also have temporary scenes in place that pause the game to allow for enough time for players to switch Zoom controls before playing a mini game.

### Random/coded action

Entry point: `/src/games/main_game.cpp:139`

The active player can “roll” and get a random number to move along the board. This triggers the players’ transforms to update automatically depending on what is rolled.

### Sprite animation

Entry point: `/shaders/animated.vs.glsl` & `/src/world_system.cpp:176`

The Doge sprite has multiple different animations that it plays depending on the player’s orientation on the board. The game switches between these animations automatically.

### Textured Geometry

Entry point: `/data/textures` & `/data/meshes

### Parallax scrolling background

Entry point: `/shaders/parallaxed.vs.glsl` & `/src/render_System.cpp:78`

The stars move slightly slower than the camera, and use UV offsets to mimic this effect.

### Observer pattern

Entry point: `/src/games/main_game.hpp:28`

We have a system in place to have observers to watch for the start and end of mini games. We are able to use to this to update player scores based on the results of mini games and trigger further scene changes.

### Help

Entry point: `/src/components.hpp:47` & `/src/games/main_game.cpp:91`
The active player can press H to get a tooltip that displays using a UI element (a component) that keeps the sprites on the screen space.

### Keyboard/mouse control

Entry point: `/src/games/main_game.cpp:74` The key call back for the main board game.

### Camera control

Entry point: `/src/render_system.cpp:260`
Camera has a target position it will interpolate to from its current position. The target is the active player

### Basic collision detection

Entry point: `/src/games/main_Game.cpp:186`
Collision detection to light up spaces and play audio

### Graphics assets

Entry point: `/data/textures`

We have created custom assets for our player character as well as all the tiles on the board. The textures for our parallaxed background are free for commercial use and were found on OpenGameArt.org.

### Lag mitigation

Entry point: **N/A**

No lag has been experienced in our testing of the game. Steps to ensure this: A collider component so that our collision detection only checks entities with the collider component and not every entity that moves.

### Crash free

Entry point: **N/A**

The project does not crash.

### Project Management

Entry point: **N/A**

We created our own Slack channel where all of our communication takes place.
We have a Google Drive directory where all of our files are stored and where group members can collaborate.
We have also created a source control process in which we create feature branches that get merged into master through PR’s. PR’s are reviewed by at least 2 group members before being merged.
The designated project manager (Johann) also books rooms for meetings, makes sure all group members are on track and makes sure that other group members can help group members that are struggling.

### Scene Management System

Entry point: `/src/scene_system.cpp`

The RenderRequest component was moved out of the ECS so that multiple “scenes” can be rendered. Right now, aside from our board scene, we have scenes for mini games and switching players that are placeholders.

### Multiplayer System

Entry point: `/src/games/main_game.cpp:79`
Multiple players can be added, each will take turns and the power of Zoom remote control will be handed over by the host.
