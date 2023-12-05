# NOTE: WILL PROBABLY ONLY WORK ON APPLE SILICON
# Gravity Simulation

This project is an implementation of a 2D gravity simulation using the Barnes-Hut algorithm and a quadtree data structure. The simulation is written in C++ and utilizes the SFML library for rendering.

Please note that the current implementation is not optimal, and there are numerous improvements that can be made.

- [x] use [MVC](https://en.wikipedia.org/wiki/Model–view–controller) design pattern
- [x] fix the quadtree instead of reconstructing it
  - tried it, i think its slower but i have to look at it again some other time
- [x] change datastructure from an array of objects (AoO) to an [object of arrays](https://stackoverflow.com/questions/37988451/convert-array-of-objects-to-an-object-of-arrays) (OoA), this should improve memory access
- [x] lastly implement real multithreading and if possible do calculations directly on the gpu
  - kinda did it i think, it runs with $5-7$ fps with $n=500'000$
- [ ] some graphics API, not SFML lol
- [ ] implement [spacial hashing](https://en.wikipedia.org/wiki/Geometric_hashing) and some [integrator](https://en.wikipedia.org/wiki/Numerical_integration) for the physics

## Explanation

The [Barnes-Hut algorithm](https://en.wikipedia.org/wiki/Barnes–Hut_simulation) is a method used to compute the gravitational forces between bodies in a system, where each body has a mass, position, and velocity. A [quadtree](https://en.wikipedia.org/wiki/Quadtree#:~:text=A%20quadtree%20is%20a%20tree,into%20four%20quadrants%20or%20regions.) is used to divide the 2D space containing the bodies into distinct sections. This allows the algorithm to calculate gravitational forces more efficiently.

### Animation of the Algorithm

This simulation is done with $n=400'000$ bodies, $\theta=0.8$, and a time step of $\Delta t=0.1$. It takes about $1$ second to calculate the forces and update the positions of the bodies for each frame. The simulation is run for $\pm1200$ frames which took about $25$ minutes to complete.

![./images/galaxy.gif](./images/galaxy.gif)

## Issues

The Barnes-Hut algorithm is not perfect; therefore, this simulation only provides an approximation of a real gravitational system. In a real $2D$-gravitational field, the gravitational force is not calculated with the inverse square law $F_g = G \cdot \frac{m_1\cdot m_2}{r^2}$, but rather with the inverse distance law, where $F_g = G \cdot \frac{m_1\cdot m_2}{r}$. </br></br>
Unfortunately, the two-dimensional gravitational force appears incorrect and is not as interesting as its three-dimensional counterpart. As a result, this simulation represents just a $2D$ slice of a $3D$ gravitational field.

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
