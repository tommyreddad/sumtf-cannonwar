# sumtf-cannonwar

Super Ultra Mambo Tango Foxtrot Cannon Wars. A small game written as
part of the course COMP 521 which I took at McGill University ages
ago, way back in 2012. Mostly, a C++ exercise involving a
demonstration of [Perlin
noise](https://en.wikipedia.org/wiki/Perlin_noise) for procedural
terrain generation, and including a very basic physics system with
collision resolution between circular entities.

# Building

Requires [SFML2](https://www.sfml-dev.org/) (and its dependencies) to
compile. On Arch Linux, this is the package
[sfml](https://www.archlinux.org/packages/community/x86_64/sfml/).

To compile, simply run `make`.

The built object files should then reside in the `./build` directory.

# Usage

~~~
sumtf-cannonwars [-d] [-m mapname]
~~~

* `-d` specifies demonstration mode
* `-m mapname` specifies the name of the map (in the `./maps/` directory) to be loaded. If this option is not specified, the map `default.map` will be loaded.

# Demonstration Mode
In demonstration mode, the following 3 demonstrations are presented:
1. A cannonball ignoring cannon collision after colliding with the mountain.
1. A cannonball-to-cannonball collision.
1. Game over after a direct hit on a cannon.

The demonstration lasts until each of these 3 complete, or a timeout occurs.

# Gameplay

Use the Up/Down arrow keys to tilt your cannon. Use the Left/Right
arrow keys to change your cannon's firepower. Hit spacebar to fire!

You win if your cannonball hits the enemy cannon before touching the
ground. There's not much of a challenge, since the enemy cannon is
totally stationary. Instead, try having fun with the wind and
collision physics!

# Details

Collision resolution has not been properly modeled using conservation
of momentum; we use the impulse approximation shown in class. However,
collision detection is properly modeled: we find the exact time of
collision and move the colliding entities so that they are just
touching.

The global wind force is presented on the top right corner of the
screen. The wind direction is the arrow's direction, and wind's
strength is represented by the arrow's thickness. Note that wind is
disabled in demonstration mode.

Press ESCAPE to exit the game at any point (except in demonstration
mode, where the game exits after all demonstrations complete, or after
a timeout).
