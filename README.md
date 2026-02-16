# MapReduce

### Project Overview

Write a high-level overview of your MapReduce implementation here.

Main: 

Considering that this is where the tasks will be divided, we have to come up with a process to split them up. 

We will have to fork() the tasks to get the child process and pass them on to the mappers and reducers. 

Here we will have to include logic to make sure we get everything back from the mappers, and pass them on to the reducers. Could maybe use pipe() to streamline the process? 

map: 

for the mappers, we will need to creat their "map" functions. we have to find a way to group them by similar numbers. after we have done this, the mappers can return back to main with the data, and main will delegate/split tasks for the reducers. 

reducers:

they will take in the data from the mappers once delegated from main. will have to incorporate logic to make reducers reduce based on byte range. 

implemention: 

were thinking about starting with main, but then completeing map.c. this will help us validate that we are sendng the correct things to the mapper. then we will work on the delegating logic, and finaly incorporate the reducer logic in main and in reduce.c

### Assumptions

Write any assumptions that we need to know about running your MapReduce implementation, or grading here.

### Work Split

Document the share of work that each group member did here. If you worked solo, please indicate so.

### AI Use

Document any AI use here. Refer to the writeup or the class syllabus if you are unsure of what is acceptable AI usage.
