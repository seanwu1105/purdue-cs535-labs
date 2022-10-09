# Lab 3

## Spec

Develop a simple playable game with shooting and moving camera around. With the
following minimal requirements:

- Running in full screen
- Camera (the player) turns left and right and moves forward and backward on a
  2D plane.
- There are two kinds of objects displayed as a wireframe.
  - Static objects (good). If you touch it, the object will disappear, and you
    will move faster
  - Moving objects (bad). If they touch you, your speed will decrease.
- The floor will be displayed as a teaselated triangular mesh
- You can have three shots. They will disappear if you shoot any of the two
  kinds of objects. Shooting the good object does NOT give you higher speed.
  Once the shot is either out of the scene or you shot an object, you can shoot
  again.
- 5 extra points: Add animation of the objects. They can change color after
  being shot, jump, shrink to zero, etc.

## Getting Started

- Press <kbd>W</kbd>, <kbd>S</kbd>, <kbd>A</kbd> and <kbd>D</kbd> to move
- Press left mouse to shoot a bullet
