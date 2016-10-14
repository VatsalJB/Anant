#Objective
This code base aims to demonstrate as a proof of concept, the working model of a nanosat fully developed by undergrad students.

##FLP
This is the collection of the tasks the satellite aims to perform in a given "State". Contains a linked list with the nodes representing the tasks the satellite shall perform. They also have some additional meta-data about the running state and the process id.
The FLP asynchronoulsy executes the subprocesses and ensures that only 1 instance of each child process is running at a given time.
The linked list is reordered per the time of next execution which is updated using a hardcoded frequency of execution. 

##Housekeeping
This is the periodic housekeeping process that runs at a given (hardcoded) frequency. This process currently only does file I/O. TImestamps are added to the data in order to make it searchable. Random integers are used for representing the values currently due to the lack of hardware functions.

##Transmit
This process shall be used to transmit data over a bus to the COMMS for downlink. Again, this currently stores the info in a file that shall be integrated with a hardware level function to make it transmit the data to ground.

##Memory-management
This small API has been written to facilitate the memory allocation and deallocation for an embedded system like the Zedboard. The inbuilt malloc and free functions are slow and may lead to fragmentation. Hence, a custom made memory pool is being used here. This also helps keep track of memory usage and can be extended to keep a free list and alloc list for constant time alloc and free.
