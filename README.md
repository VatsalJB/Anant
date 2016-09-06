#Objective
This code base aims to demonstrate as a proof of concept, the working model of a nanosat fully developed by undergrad students.

##FLP
This is the collection of the tasks the satellite aims to perform in a given "State". Contains a linked list with the nodes representing the tasks the satellite shall perform. They also have some additional meta-data about the running state and the process id.
The FLP asynchronoulsy executes the subprocesses and ensures that only 1 instance of each child process is running at a given time.
The linked list is reordered per the time of next execution which is updated using a hardcoded frequency of execution. 
