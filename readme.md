# Gravity Simulation

This project is an implementation of a 2D gravity simulation using the Barnes-Hut algorithm and a quadtree data structure. The simulation is written in C++ and uses the SFML library for rendering. </br></br>
Its worth noting that the implementation is terrible (as of now) and there are lots of improvements to be made.

- [ ] use [MVC](https://en.wikipedia.org/wiki/Model–view–controller) design pattern
- [ ] fix the quadtree instead of reconstructing it
- [ ] implement [spacial hashing](https://en.wikipedia.org/wiki/Geometric_hashing) and some [integrator](https://en.wikipedia.org/wiki/Numerical_integration) for the physics
- [ ] change datastructure from an array of objects (AoO) to an [object of arrays](https://stackoverflow.com/questions/37988451/convert-array-of-objects-to-an-object-of-arrays) (OoA), this should improve memory access
- [ ] some graphics API, not SFML lol
- [ ] lastly implement real multithreading and if possible do calculations directly on the gpu

## Overview

The [Barnes-Hut algorithm](https://en.wikipedia.org/wiki/Barnes–Hut_simulation) is a method used to compute the gravitational forces between bodies in a system, where each body has a mass, position, and velocity. A [quadtree](https://en.wikipedia.org/wiki/Quadtree#:~:text=A%20quadtree%20is%20a%20tree,into%20four%20quadrants%20or%20regions.) is used to divide the 2D space containing the bodies into distinct sections. This allows the algorithm to calculate gravitational forces more efficiently.
The [Barnes-Hut algorithm](https://en.wikipedia.org/wiki/Barnes–Hut_simulation) is a method used to compute the gravitational forces between bodies in a system, where each body has a mass, position, and velocity. A [quadtree](https://en.wikipedia.org/wiki/Quadtree#:~:text=A%20quadtree%20is%20a%20tree,into%20four%20quadrants%20or%20regions.) is used to divide the 2D space containing the bodies into distinct sections. This allows the algorithm to calculate gravitational forces more efficiently.

## Animation of the Algorithm

This animation was done with $n=100000$ particles, $\theta = 0.8$, $dt = 0.1$. Its $1000$ frames rendered to a GIF with $30 \text{ fps}$.
This animation was done with $n=100000$ particles, $\theta = 0.8$, $dt = 0.1$. Its $1000$ frames rendered to a GIF with $30 \text{ fps}$.
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
cmake .
```

4. Run the simulation:

```bash
make run
```

or

```bash
make && ./gravity_sim
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
