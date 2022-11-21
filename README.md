# os-challenge-SHAshanties

<br /> 

# Table of Contents

- [General Information](#generalinformation)
- [Experiment 1 - Multiprocessing vs. Multithreading and ***On the Fly*** vs. ***Pooled***](#e1)
    - [Experiment 1a - Determining the Optimal Number of Processes of a Pre-Forked Server](#e1b)
    - [Experiment 1b - Determining the Optimal Number of Threads of a Thread Pool Server](#e1b)
    - [Experiment 1c -](#e1b)
- [Experiment 2 - Caching Requests](#e2)
- [Experiment 3 - Splitting the Request Into Tasks](#e2)
- [Experiment 4 - Nonblocking I/O](#e2)
- [Experiment 5 - Priority Queue](#e2)
- [Final Solution](#final-solution)

<br /> 
<br /> 

# General Information 

This sections contains general information regarding the project and should be read before any of the other sections. The 
other sections can be read in any order, but as the implementation has been iteratively developed reading them in sequence 
is recommended. 

<br /> 
<br /> 

# Experiment 1 - Multiprocessing vs. Multithreading and ***On the Fly*** vs. ***Pooled***
**Responsible:** 

William


**Motivation:**

Our benchmark server was implemented using a "fork per request" approach, meaning that for each request the server would create a new process to perform the reverse hashing, after which the process would terminate. The overhead associated with creating a new process per request is not necessarily cheap in regards to performance, and using a different approach might also allow for better parallelization, so the first experiments were dedicated to investigating alternatives to this method.


The obvious alternative to multiprocessing is multithreading, and for both, two versions were experimented with, which we will refer to as ***On the Fly*** and ***Pooled***. ***On the Fly*** refers to creating a new process/thread for each request, while ***Pooled*** refers to having some fixed number of pre-forked/pre-threaded processes/threads ready for requests which will be reused with the intention of cutting down on the spawning related overhead and utilizing the hardware more efficiently. 

Before comparing the four approaches, a vital property of the ***Pooled*** versions had to be determined, namely the size of the pools, i.e. the number of pre-created processes/threads. The reasons for doing so builds on the assumption that some pool sizes grant better performance than others, and since the intention is to identify an optimal server, comparing a suboptimal ***Pooled*** server to a ***On the Fly*** server wouldn't make sense. Determining the optimal number of processes and threads for the ***Pooled*** multiprocessing server and the ***Pooled*** multithreading server, while testing the assumption that the pool sizes influence server performance, was the goal of the experiments 1a and 1b, respectively. 

<br /> 


## Experiment 1a - Determining the Optimal Number of Processes of a Pre-Forked Server 
**Hypothesis:** 

The number of processes of a pre-forked server significantly influences the server's performance. 

**Relevant files:**

- src/forked_server.c
- src/forked_server.h
- client/run-client-milestone.sh
- test/experiments/e1/e1a/

**Setup:**

For this experiment the run-client-continuous configuration was executed 3 times for every number of processes from 1 to 10, from 10 to 25 in steps of 5, and from 25 to 100 in steps of 25 (i.e., 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 50, 75, and 100) and the average was then taken of the 3 executions. This was done in order to cover a wide range of options for the number of processes, that would hopefully make some pattern emerge that could be concluded on in regards to how the number of pre-forked processes effected performance. 

**Results:**

The average score of the pre-forked server vs. number of processes used can be seen plotted in Figure 1, and the numerical values can be seen in Table 1.

<center>

![Figure 1](/experiments/e1_otf_vs_pooled/e1a/plot/preforked_e1a.png "Figure 1 - figure text")
**Figure 1** - Number of processes, nprocesses plotted against the average score of 3 runs of the run-client-continuous configuration. 

</center>

<center>

nprocesses | average score 
---        |---            
1          | 1.99          
2          | 1.89          
3          | 1.89 
4          | 1.89 
5          | 1.89 
6          | 1.89 
7          | 1.89 
8          | 1.89 
9          | 1.89 
10         | 1.89 
15         | 1.89 
20         |
25         |
50         |
75         |
100        |


**Table 1** - Number of processes, nprocesses and the corresponding the average score of 3 runs of the run-client-continuous configuration.

</center>








<br /> 

## Experiment 1b - Determining the Optimal Number of Threads of a Thread Pool Server 
**Hypothesis:** 

The number of processes of a pre-forked server significantly influences the server's performance. 

**Relevant files:**

- src/forked_server.c
- src/forked_server.h
- client/run-client-milestone.sh
- test/experiments/e1/e1a/

**Setup:**

For this experiment the run-client-continuous configuration was executed 3 times for every number of processes from 1 to 10, from 10 to 25 in steps of 5, and from 25 to 100 in steps of 25 (i.e., 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 50, 75, and 100) and the average was then taken of the 3 executions. This was done in order to cover a wide range of options for the number of processes, that would hopefully make some pattern emerge that could be concluded on in regards to how the number of pre-forked processes effected performance. 

**Results:**

The average score of the pre-forked server vs. number of processes used can be seen plotted in Figure 1, as well as in Table 1.

<center>

![Figure 2](/experiments/e1_otf_vs_pooled/e1b/plot/threadpool_e1b.png "Figure 2 - figure text")
**Figure 2** - Number of processes, nprocesses plotted against the average score of 3 runs of the run-client-continuous configuration. 

</center>

<center>

nthreads | average score 
---        |---            
1          | 1.99          
2          | 1.89          
3          | 1.89 
4          | 1.89 
5          | 1.89 
6          | 1.89 
7          | 1.89 
8          | 1.89 
9          | 1.89 
10         | 1.89 
15         | 1.89 
20         |
25         |
50         |
75         |
100        |


**Table 2** - Number of processes, nprocesses and the corresponding the average score of 3 runs of the run-client-continuous configuration.

</center>


<br /> 

## Experiment 1c - 
**Hypothesis:** 

The number of processes of a pre-forked server significantly influences the server's performance. 

**Relevant files:**

- src/forked_server.c
- src/forked_server.h
- client/run-client-milestone.sh
- test/experiments/e1/e1a/

**Setup:**

For this experiment the run-client-continuous configuration was executed 3 times for every number of processes from 1 to 10, from 10 to 25 in steps of 5, and from 25 to 100 in steps of 25 (i.e., 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 50, 75, and 100) and the average was then taken of the 3 executions. This was done in order to cover a wide range of options for the number of processes, that would hopefully make some pattern emerge that could be concluded on in regards to how the number of pre-forked processes effected performance. 

husk at sige at det er 1000 her, før ændringen i final 


**Results:**

The average score of the pre-forked server vs. number of processes used can be seen plotted in Figure 1, as well as in Table 1.

<center>

![Figure 3](/experiments/e1_otf_vs_pooled/e1c/plot/compare_e1c.png "Figure 3 - figure text")
**Figure 3** - Number of processes, nprocesses plotted against the average score of 3 runs of the run-client-continuous configuration. 

</center>

<center>

server     | avg. score continuous config. | avg.  
---        |---            
1          | 1.99          
2          | 1.89          
3          | 1.89 
4          | 1.89 
5          | 1.89 
6          | 1.89 
7          | 1.89 
8          | 1.89 
9          | 1.89 
10         | 1.89 
15         | 1.89 
20         |
25         |
50         |
75         |
100        |


**Table 3** - Number of processes, nprocesses and the corresponding the average score of 3 runs of the run-client-continuous configuration.

</center>





<br /> 
<br /> 

# Experiment 2 - Caching Requests
**Responsible:** 

**Motivation:**

**Hypothesis:** 

The number of processes of a pre-forked server significantly influences the server's performance. 

**Relevant files:**

- src/forked_server.c
- src/forked_server.h
- client/run-client-milestone.sh
- test/experiments/e1/e1a/

**Setup:**

For this experiment the run-client-continuous configuration was executed 3 times for every number of processes from 1 to 10, from 10 to 25 in steps of 5, and from 25 to 100 in steps of 25 (i.e., 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 50, 75, and 100) and the average was then taken of the 3 executions. This was done in order to cover a wide range of options for the number of processes, that would hopefully make some pattern emerge that could be concluded on in regards to how the number of pre-forked processes effected performance. 

**Results:**

The average score of the pre-forked server vs. number of processes used can be seen plotted in Figure 1, as well as in Table 1.

<center>

![Figure 4](experiments\e2_cache\plot\cached_vs_thread_e2.png "Figure 4 - figure text")
**Figure 4** - Number of processes, nprocesses plotted against the average score of 3 runs of the run-client-continuous configuration. 

</center>

<center>

server     | avg. score continuous config. | avg.  
---        |---            
1          | 1.99          
2          | 1.89          
3          | 1.89 
4          | 1.89 
5          | 1.89 
6          | 1.89 
7          | 1.89 
8          | 1.89 
9          | 1.89 
10         | 1.89 
15         | 1.89 
20         |
25         |
50         |
75         |
100        |


**Table 4** - Number of processes, nprocesses and the corresponding the average score of 3 runs of the run-client-continuous configuration.

</center>




<br /> 
<br /> 

# Experiment 3 - Splitting the Request Into Tasks
**Responsible:** 

**Motivation:**
 
**Hypothesis:** 

The number of processes of a pre-forked server significantly influences the server's performance. 

**Relevant files:**

- src/forked_server.c
- src/forked_server.h
- client/run-client-milestone.sh
- test/experiments/e1/e1a/

**Setup:**

For this experiment the run-client-continuous configuration was executed 3 times for every number of processes from 1 to 10, from 10 to 25 in steps of 5, and from 25 to 100 in steps of 25 (i.e., 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 50, 75, and 100) and the average was then taken of the 3 executions. This was done in order to cover a wide range of options for the number of processes, that would hopefully make some pattern emerge that could be concluded on in regards to how the number of pre-forked processes effected performance. 

**Results:**

The average score of the pre-forked server vs. number of processes used can be seen plotted in Figure 1, as well as in Table 1.

<center>

![Figure 5](experiments\e3_split\plot\split_e3.png "Figure 5 - figure text")
**Figure 5** - Number of processes, nprocesses plotted against the average score of 3 runs of the run-client-continuous configuration. 

</center>

<center>

server     | avg. score continuous config. | avg.  
---        |---            
1          | 1.99          
2          | 1.89          
3          | 1.89 
4          | 1.89 
5          | 1.89 
6          | 1.89 
7          | 1.89 
8          | 1.89 
9          | 1.89 
10         | 1.89 
15         | 1.89 
20         |
25         |
50         |
75         |
100        |


**Table 5** - Number of processes, nprocesses and the corresponding the average score of 3 runs of the run-client-continuous configuration.

</center>





Did discover that
  
<br /> 
<br /> 


# Experiment 4 - Nonblocking I/O
**Responsible:** 




**Motivation:**

 
**Hypothesis:** 

The number of processes of a pre-forked server significantly influences the server's performance. 

**Relevant files:**

- src/forked_server.c
- src/forked_server.h
- client/run-client-milestone.sh
- test/experiments/e1/e1a/

**Setup:**

For this experiment the run-client-continuous configuration was executed 3 times for every number of processes from 1 to 10, from 10 to 25 in steps of 5, and from 25 to 100 in steps of 25 (i.e., 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 50, 75, and 100) and the average was then taken of the 3 executions. This was done in order to cover a wide range of options for the number of processes, that would hopefully make some pattern emerge that could be concluded on in regards to how the number of pre-forked processes effected performance. 

**Results:**

The average score of the pre-forked server vs. number of processes used can be seen plotted in Figure 1, as well as in Table 1.

<center>

![Figure 6](experiments\e4_nonblocking_IO\plot\nonblock_e4.png "Figure 6 - figure text")
**Figure 6** - Number of processes, nprocesses plotted against the average score of 3 runs of the run-client-continuous configuration. 

</center>

<center>

server     | avg. score continuous config. | avg.  
---        |---            
1          | 1.99          
2          | 1.89          
3          | 1.89 
4          | 1.89 
5          | 1.89 
6          | 1.89 
7          | 1.89 
8          | 1.89 
9          | 1.89 
10         | 1.89 
15         | 1.89 
20         |
25         |
50         |
75         |
100        |


**Table 6** - Number of processes, nprocesses and the corresponding the average score of 3 runs of the run-client-continuous configuration.

</center>





we were thinking about using semaphores to block the 

not the silver bullet we were hoping for 

<br /> 
<br /> 

# Experiment 5 - Priority Queue
**Responsible:** 


**Motivation:**


**Hypothesis:** 

The number of processes of a pre-forked server significantly influences the server's performance. 

**Relevant files:**

- src/forked_server.c
- src/forked_server.h
- client/run-client-milestone.sh
- test/experiments/e1/e1a/

**Setup:**

For this experiment the run-client-continuous configuration was executed 3 times for every number of processes from 1 to 10, from 10 to 25 in steps of 5, and from 25 to 100 in steps of 25 (i.e., 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 50, 75, and 100) and the average was then taken of the 3 executions. This was done in order to cover a wide range of options for the number of processes, that would hopefully make some pattern emerge that could be concluded on in regards to how the number of pre-forked processes effected performance. 

**Results:**

The average score of the pre-forked server vs. number of processes used can be seen plotted in Figure 1, as well as in Table 1.

<center>

![Figure 7](experiments\e5_prio\plot\prio_e5.png "Figure 7 - figure text")
**Figure 7** - Number of processes, nprocesses plotted against the average score of 3 runs of the run-client-continuous configuration. 

</center>

<center>

server     | avg. score continuous config. | avg.  
---        |---            
1          | 1.99          
2          | 1.89          
3          | 1.89 
4          | 1.89 
5          | 1.89 
6          | 1.89 
7          | 1.89 
8          | 1.89 
9          | 1.89 
10         | 1.89 
15         | 1.89 
20         |
25         |
50         |
75         |
100        |


**Table 7** - Number of processes, nprocesses and the corresponding the average score of 3 runs of the run-client-continuous configuration.

</center>



<br /> 
<br /> 

# Final Solution


more time scheduling

attribute 

process approach 

panic mode 
but split 