# HoverTank1989

HoverTank1989 is a retro-futuristic physics-based hover tank simulation built with C++, DirectX, and DirectXTK. Unlike many games that rely on the computationally cheaper Euler method, this project utilizes a Runge-Kutta fourth-order ODE (RK4) to handle physics, offering superior numerical stability and a more accurate modeling of reality. Every movement within the environment is strictly governed by the laws of physics.

<p align="center">
  <img width="600" src="https://github.com/lehrj/HoverTank1989/blob/master/Images/Trailer01.png">
</p>

## Key Features

### The Archimedes Hover Drive
At the core of the vehicle's engineering is the Archimedes Hover Drive, a fictional technology designed for sci-fi levels of force. 
*   **Buoyancy-Based Stability:** The engine’s power output is dictated by an algorithm based on the Archimedes principle, delivering force perpendicular to the terrain to suspend the vehicle in gravity.
*   **Dual Motor Setup:** The tank utilizes two distinct motors: a hover motor for vertical lift and stability, and a drive motor for horizontal navigation controlled by the pilot or AI.

<p align="center">
  <img width="600" src="https://github.com/lehrj/HoverTank1989/blob/master/Images/ImpactReverseAngle.gif">
</p>

### Advanced Physics-Based Combat
The weaponry system is divided into two categories, each reacting dynamically with the physics engine:
*   **Kinetic Weapons:** Includes cannons, machine guns, and shotguns. Impact force is calculated in Newtons based on projectile mass and relative velocities, with each ammo type possessing a unique drag coefficient and muzzle velocities affecting its trajectory.
*   **Explosive Weapons:** Features standard explosives, MIRV cluster bombs, and guided missiles. These can trigger chain reactions and deliver significant kinetic impact within a blast sphere.
*   **Recoil & Stabilization:** Every shot generates a recoil force on the vehicle. To help compensate for the inherent instability of a hover tank a bit of old tech was added to help, an auto stabilized turret. Drawing inspiration from the M-4 Sherman’s historically underutilized gyroscopic stabilizer, the game emphasizes the difficulty of maintaining accuracy under fire, often necessitating the use of guided munitions for long-range targets.

<p align="center">
  <img width="600" src="https://github.com/lehrj/HoverTank1989/blob/master/Images/Blueprint.png">
</p>

### FGM-148 Javelin Inspired Missile Dynamics
The laser aimed guided missiles in HoverTank1989 are reverse-engineered from the flight dynamics of the Javelin anti-tank missile. Fired in pairs to neutralize yaw torque from recoil of their off-center launch position, they follow a complex six-phase flight path:
1.  **Soft Launch:** The missile is ejected before the rocket fires to ensure safe deployment.
2.  **Wing/Fin Deployment:** Center of pressure shift aft relative to the center of mass to reflect the missile shape transitions from cylinder to dart.
3.  **Stall Prevention:** The missile must accelerate to generate lift, a challenge given the hover tank is never truly stationary. Missile launch failure can occur if launched while in reverse above 35kph. Launching under these conditions is not advised unless it is for fun.
4.  **Climb:** The missile gains altitude for a top-down attack. Once airspeed is high enough for the airfoils to generate lift the missile is able to do this in a stable manor.
5.  **Intercept Trajectory & Terminal Guidance:** The missile calculates an optimal path to connect with the moving the target kinetically and detonates on impact. If the missile anticipates that it cannot make direct impact it will attempt to hit the target with secondary shotgun cone detonation with a larger area of effect.
6.  **Self Detonation Safety Switch:** If the missile’s target is destroyed prior to intercept it will self-detonate after climbing to a safe altitude. 

<p align="center">
  <img width="600" src="https://github.com/lehrj/HoverTank1989/blob/master/Images/Deploy.gif">
</p>

### NPC AI & Autonomous Navigation
NPC drones navigate the environment using waypoint-based movement and a sophisticated moving obstacle avoidance system. This is achieved through two detection volumes, one fixed and one projected ahead, allowing the AI to calculate the best path around moving objects [6]. If navigation is blocked, NPCs can utilize a boost jump to clear obstacles. This jump boost mode also includes a landing boost so soften the landing each time.

<p align="center">
  <img width="600" src="https://github.com/lehrj/HoverTank1989/blob/master/Images/NpcSpawner.gif">
</p>

### "Code the Art" Visual Style
To achieve its unique retro-futuristic aesthetic, the project employs a "code the art" approach. 
*   **Geometric Primitives:** 3D shapes are created frame-by-frame using C++ to control geometric primitives in an **additive constructive method**, similar to a 3D printer.
*   **Wire-frame Terrain:** The environment is rendered as a wire-frame height map generated from greyscale bitmap images, where pixel values determine the elevation of the terrain.

## Game Controls

Most controls are also gamepad supported but the mapping is in flux as new features are implemented and gameplay elements tuned

### Vehicle Movement Controls

Move Forward : NumPad 8

Move Back : NumPad 5

Strafe Left : NumPad 4

Turn Left : NumPad 7

Strafe Right : NumPad 6

Turn Right : NumPad 9

### Turret & Weapon Controls

Turret Yaw Left : NumPad 1

Turret Yaw Right : NumPad 3

Weapon Pitch Down : NumPad 0

Weapon Pitch Up : NumPad 2

Cycle Ammo Types : NumPad Decimal

Toggle Targeting Laser On/Off : ‘K’ key

Fire Selected Weapon : Spacebar

### Gamepad Controls

Cycle Missile Track State : A Button

Toggle Targeting Laser On/Off : B Button

Fire Missile : Right Bumper

Forward/Reverse/Strafe : Left Thumbstick

Turret/Zoom : Right Thumbstick

Rotate Tank: L/R Triggers

<p align="center">
  <img width="600" src="https://github.com/lehrj/HoverTank1989/blob/master/Images/ImpactReverseAngle.gif">
</p>

## Videos

YouTube video of guided missile here: https://youtu.be/3NURgkEC96w

YouTube video of weapons demo here: https://youtu.be/TSxC-AU32E0

YouTube video of NPC prototypes here: https://youtu.be/8a0KPadDvUc

