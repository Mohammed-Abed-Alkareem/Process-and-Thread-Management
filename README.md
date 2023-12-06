# Process-and-Thread-Management
process and thread management using POSIX and Pthreads.


## Part 1: Process Management
Description: Implement a program with a parent process that creates child processes using fork(). Use message passing with pipes for IPC between parent and child.
Requirements:
•	Create child processes using fork()
•	Implement IPC using pipes or shared memory
•	Manage process execution and termination
•	Error handling and cleanup
•	Comments and documentation
## Part 2: Multithreaded Processing
Description: Implement multithreaded processing using Pthreads. Coordinate threads and access to shared data.
Requirements:
•	Create threads using Pthreads
•	Coordinate threads to execute a task
•	Manage shared access to data between threads
•	Manage thread creation, joining/detachment, cleanup
•	Comments and documentation
## Part 3: Performance Measurement
Description: Compare process and threaded solutions by implementing a parallelizable computational task.
Requirements:
•	Implement task (e.g. matrix multiplication) using processes and threads
•	Measure and compare execution times
•	Vary number of processes/threads
•	Present clear performance comparison data
## Part 4: Thread Management
Description: Implement joining and detaching of threads. Measure impact on throughput.
Requirements:
•	Create joinable and detached threads
•	Measure throughput with different join/detach configurations
•	Analyze impact of join vs detach on throughput
•	Document implementation and analysis
