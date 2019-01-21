
We need 2 queues for this task:
1. queue that will store unsorted lines from the input file (let's call it DequeOfStringsFromInputFile).
2. queue that will store sorted vectors (let's call it DequeOfSortedLines).

The task has 3 main subtasks:
1.reading the lines one by one from the input file and inserting them into DequeOfStringsFromInputFile.
2. parsing and sorting of each line
3. writing each line into the output file

I had 2 possible ways to implement this task.

each subtask will be solved by a dedicated class. Pros of this way are: encapsulation (no global queues
and methods,etc),code organization. Cons: waste of time on all the overhead that is associated with
objects creation/dealing/calling.

each subtask will be solved by one of 3 functions for each of above-mentioned subtasks.
the main thread will create 3 threads and will give to each of them one of the functions.
both queues will be accessible from all the threads.
Pros of this way are: better performance, better modifiability(each function can be easily modified) than
the version with objects has.
Cons: global data structures and functions.

During the program execution, 4 threads will be running simultaneously :

One thread will read the lines from the input file and will insert them into DequeOfStringsFromInputFile.

The second thread will read a string from DequeOfStringsFromInputFile, will parse and sort it and will
insert the sorted line into DequeOfSortedLines.

Third thread will read sorted lin from DequeOfSortedLines and will write it into the output file.

That's the optimal solution bearing in mind testability of the program. If we will have more than one
thread reading/writing from the file/queue, we will not be able to predict the order in which the lines
will be written into the output file. If so, we will not be able to test the program.
