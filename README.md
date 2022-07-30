# Parallel Computing and Distributed Systems

In this project we used the C programming language to parallelize an algorithm, in this case we parallelize multiplication matrix algorithm.

## Phases of this project

### `Using Pthreads and OpenMP`

With theses libraries we were able to use shared memory programming, with the aim of being able to parallelize the algorithm.
We perform the tests on an 8-thread linux machine and perform a comparison with a serial programming of the algorithm.

### `Distributed Memory programming with MPI`

With this technology we were able to parallelize the algorithm but with a shared memory system, this means that we had several 8-threaded linux machines working at the same time, each one performing part of the task designated by the algorithm. For this part of the project, we made comparisons between the use of a single machine making use of all or some of its threads with the machines working together, this comparison was made in this way since the communication between the machines of the distributed system was a problem or an advantage depending on how quickly that communication develops.
