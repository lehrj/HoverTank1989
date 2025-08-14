# HoverTank1989
A retro themed physics-based hover tank game. Demo videos @[BaltimoreMagicWerks](https://www.youtube.com/playlist?list=PLKMR2Ppf1A4jEboMkoUcWpha-Y7QUaZ_o)

<p align="center">
  <img width="600" src="https://github.com/lehrj/HoverTank1989/blob/master/Images/Trailer01.png">
</p>

YouTube trailer of weapons demo here : https://youtu.be/TSxC-AU32E0

On display are 3 kinetic ammo types: cannon, machine gun, and shotgun, as well as 2 explosive ammo types. 
The kinetic ammo is “non-lethal” in that it does not apply damage to a target’s health, instead it applies Newton’s second law of motion (F = m * a) to play billiards with the NPC hover tanks. 

There are two explosive ammo types; the first is a standard shell with a single an explosive area of effect charge that detonates on impact. The second is a MIRV style mid-air deployed cluster munition, to saturate a large area in physics inducing explosive fun. Any NPC hit with an explosive round or caught within its explosive radius will detonate in a secondary explosion, which in turn can detonate other nearby NPCs in a chain reaction

YouTube trailer of NPC prototypes here : https://youtu.be/8a0KPadDvUc

Autonomous figure 8 track racing with NPC hovercrafts shows off their ability to navigate the track via waypoints while attempting to avoid other NPCs. The center crossover point ensures that this is not always possible to demonstrate the vehicle collision physics. The NPC hovercrafts have the ability to jump with their thrusters which is unlocked midway through the video. The NPCs will use the jump boost to avoid or pass other vehicles

<p align="center">
  <img width="600" src="https://github.com/lehrj/HoverTank1989/blob/master/Images/NpcSpawner.gif">
</p>

## Work in progress: Guided Missiles

<p align="center">
  <img width="600" src="https://github.com/lehrj/HoverTank1989/blob/master/Images/Deploy.gif">
</p>

The laser targeted self-guided missile system attempts to emulate the javelin anti-tank missile in both form & function. It utilizes a dual stage soft launch system, with the rocket ignition and wing deployment occurring after the missile is a safe distance from the launch vehicle.

<p align="center">
  <img width="600" src="https://github.com/lehrj/HoverTank1989/blob/master/Images/FlightPath.png">
</p>

Once fully deployed the missile accelerates past stall speed, at which point it can safely gain altitude for a top-down attack.  

<p align="center">
  <img width="600" src="https://github.com/lehrj/HoverTank1989/blob/master/Images/ImpactReverseAngle.gif">
</p>

Targeting is achieved with a vehicle mounted laser designator that changes color when a target is locked, along with an audio lock tone.  The missile is fire and forget, so once launched the user is able to acquire the next target immediately.

<p align="center">
  <img width="600" src="https://github.com/lehrj/HoverTank1989/blob/master/Images/Blueprint.png">
</p>

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

Rotate Tank : L/R Triggers
