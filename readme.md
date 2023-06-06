# Gravity Simulation

This project is an implementation of a 2D gravity simulation using the Barnes-Hut algorithm and a quadtree data structure. The simulation is written in C++ and uses the SFML library for rendering.

## Overview

The Barnes-Hut algorithm is a method used to compute the gravitational forces between bodies in a system, where each body has a mass, position, and velocity. A quadtree is used to divide the 2D space containing the bodies into distinct sections. This allows the algorithm to calculate gravitational forces more efficiently.

## Animation of the Algorithm

![Animation of the Barnes-Hut algorithm](./images/animation.gif)

## Getting Started

To run the simulation, you will need a compiler that supports C++11 or later and the SFML library.

### Prerequisites

- GCC, Clang, or another C++11-compatible compiler
- SFML library

### Building and Running

To build and run the simulation:

1. Clone the repository:

```bash
git clone https://github.com/mazzeaar/gravity_sim.git
```

2. Navigate to the project directory:

```bash
cd gravity_sim
```

3. Build the project:

```bash
cmake .
```

4. Run the simulation:

```bash
make run
```
or
```bash
make && ./gravity_sim
```

## Honorable Mentions

- myself
- gpt4
- the guy who invented the barnes-hut algorithm

## License

Imagine having a license lol
