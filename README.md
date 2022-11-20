# os-challenge-SHAshanties

# Table of Contents

0. [General Information](#generalinformation)



# 0 General Information 

This sections contains general information regarding the project and should be read before any of the other sections. The 
other sections can be read in any order, but as the implementation has been iteratively developed reading them in sequence 
is recommended. 



# 1 A Better Foundation: Multiprocessing vs. Multithreading and ***On the Fly*** vs. ***Pooled***
**Responsible:** 

William


**Motivation:**

Our benchmark server was implemented using a "fork per request" approach, meaning that for each request the server would create a new process to perform the reverse hashing, after which the process would terminate. The overhead associated with creating a new process per request is not necessarily cheap in regards to performance, and using a different approach might also allow for better parallelization, so the first experiments were dedicated to investigating alternatives to this method.


The obvious alternative to multiprocessing is multithreading, and for both, two versions were experimented with, which we will refer to as ***On the Fly*** and ***Pooled***. ***On the Fly*** refers to creating a new process/thread for each request, while ***Pooled*** refers to having some fixed number of pre-forked/pre-threaded processes/threads ready for requests which will be reused with the intention of cutting down on the spawning related overhead and utilizing the hardware more efficiently. 

Before comparing the four approaches, a vital property of the ***Pooled*** versions had to be determined, namely the size of the pools, i.e. the number of pre-created processes/threads. The reasons for doing so builds on the assumption that some pool sizes grant better performance than others, and since the intention is to identify an optimal server, comparing a suboptimal ***Pooled*** server to a ***On the Fly*** server wouldn't make sense. Determining the optimal number of processes and threads for the ***Pooled*** multiprocessing server and the ***Pooled*** multithreading server, while testing the assumption that the pool sizes influence server performance, was the goal of the experiments 1a and 1b, respectively. 

## 1.1 Experiment 1a - Determining the Optimal Number of Processes of a Pre-Forked Server 
---
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


![Figure 1](/test/experiments/e1_otf_vs_pooled/e1a/plot/preforked_e1a.png "Figure 1 - figure text")
**Figure 1** - Number of processes, nprocesses plotted against the average score of 3 runs of the run-client-continuous configuration. 

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

</center>

**Table 1** - Number of processes, nprocesses and the corresponding the average score of 3 runs of the run-client-continuous configuration.









## 1.2 Experiment 1b - Determining the Optimal Number of Threads of a Thread Pool Server 
---



## 1.3 Experiment 1c - 
---








# 2 Caching Requests





# 3 Splitting the Request Into Tasks 





Did discover that 


# 4 Nonblocking I/O

we were thinking about using semaphores to block the 




# 5 Priority Management

**Responsible:**



**Motivation:**

Our benchmark server was implemented using a "fork per request" approach, meaning that for each request the server would create a new process to perform the reverse hashing, after which the process would terminate. The overhead associated with creating a new process per request is not necessarily cheap in regards to performance, and using a different approach might also allow for better parallelization, so the first experiments were dedicated to investigating alternatives to this method.


The obvious alternative to multiprocessing is multithreading, and for both, two versions were experimented with, which we will refer to as ***On the Fly*** and ***Pooled***. ***On the Fly*** refers to creating a new process/thread for each request, while ***Pooled*** refers to having some fixed number of pre-forked/pre-threaded processes/threads ready for requests which will be reused with the intention of cutting down on the spawning related overhead and utilizing the hardware more efficiently. 

Before comparing the four approaches, a vital property of the ***Pooled*** versions had to be determined, namely the size of the pools, i.e. the number of pre-created processes/threads. The reasons for doing so builds on the assumption that some pool sizes grant better performance than others, and since the intention is to identify an optimal server, comparing a suboptimal ***Pooled*** server to a ***On the Fly*** server wouldn't make sense. Determining the optimal number of processes and threads for the ***Pooled*** multiprocessing server and the ***Pooled*** multithreading server, while testing the assumption that the pool sizes influence server performance, was the goal of the experiments 1a and 1b, respectively. 

## 5.1 Experiment 5 - 
---
**Hypothesis:** 



**Relevant files:**

- 
-
-

**Setup:**



**Results:**

The average score of the pre-forked server vs. number of processes used can be seen plotted in Figure 1, as well as in Table 1.


![Figure 1](/test/experiments/e1_otf_vs_pooled/e1a/plot/preforked_e1a.png "Figure 1 - figure text")
**Figure 1** - Number of processes, nprocesses plotted against the average score of 3 runs of the run-client-continuous configuration. 

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

</center>

**Table 1** - Number of processes, nprocesses and the corresponding the average score of 3 runs of the run-client-continuous configuration.



conclusion til 






# 6 Final Solution


more time scheduling