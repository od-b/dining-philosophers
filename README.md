# Dining Philosophers
Dining philosophers problem implemented using pthreads

---

### Problem statement
> Five philosophers dine together at the same table. Each philosopher has his own plate at the table. There is a fork between each plate. The dish served is a kind of spaghetti which has to be eaten with two forks. Each philosopher can only alternately think and eat. Moreover, a philosopher can only eat his spaghetti when he has both a left and right fork. Thus two forks will only be available when his two nearest neighbors are thinking, not eating. After an individual philosopher finishes eating, he will put down both forks.

_[wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)_

---

### Running the simulation

Compile with `make philosophers`. Run the `philosophers` executable.

Simulation parameters such as the number of threads and delay times are contained within `philosophers.h`. The program has not been extensively tested for bugs that might occur when changing these parameters.

The program will run indefinitely. Send a kill signal to exit.
