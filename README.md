# Dining Philosophers
Simple approach to the dining philosophers problem, originally formulated by Edsger W. Dijkstra

This implementation uses a shared mutex and one condvar per philosopher.

The solution is fair if we define fair as to not give any one philosopher an advantage based on factors such as order of execution.
This can be observed by the varying results produced by the simulation.

---

### Compiling and running

A makefile is set up. Compile using `make` or `make philosophers`.

Usage: 
```
philosophers <num_philosophers> <n_meals>
```
`<num_philosophers>`: number of philosopher threads to create  
`<n_meals>`: When any one philosopher reaches this meal count, they will all exit at the end of their current cycle.  

Help / Info / Example: 
```
philosophers [--h | --help]
```

Definitions within `src/philosophers.h` controls delay timings (how long time it takes to eat, put down forks).

---
### Bugs
Inline table printing breaks for a sufficient amount of philosophers. 
This does not affect functionality, but the terminal will be flooded with prints.  
To only print summary and not the entire table, set `PRINT_STATUS_TABLE` to 0, within `src/philosophers.h`, or provide it as a compile time definition (e.g. `make philosophers CFLAGS="-D PRINT_STATUS_TABLE=0"`). 

---

### Problem statement
> Five philosophers dine together at the same table. Each philosopher has his own plate at the table. There is a fork between each plate. The dish served is a kind of spaghetti which has to be eaten with two forks. Each philosopher can only alternately think and eat. Moreover, a philosopher can only eat his spaghetti when he has both a left and right fork. Thus two forks will only be available when his two nearest neighbors are thinking, not eating. After an individual philosopher finishes eating, he will put down both forks.

_[wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)_
