# os-challenge-SHAshanties

<br /> 

# Table of Contents

- [General Information](#generalinformation)
- [Experiment 1 - Multiprocessing vs. Multithreading and ***On the Fly*** vs. ***Pooled***](#experiment-1---multiprocessing-vs-multithreading-and-on-the-fly-vs-pooled)
    - [Experiment 1a - Determining the Optimal Number of Processes of a Pre-Forked Server](#experiment-1a---determining-the-optimal-number-of-processes-of-a-pre-forked-server)
    - [Experiment 1b - Determining the Optimal Number of Threads of a Thread Pool Server](#experiment-1b---determining-the-optimal-number-of-threads-of-a-thread-pool-server)
    - [Experiment 1c - 4-way Comparison of Multiprocessing vs. Multithreading and ***On the Fly*** vs. ***Pooled***](#experiment-1c---4-way-comparison-of-multiprocessing-vs-multithreading-and-on-the-fly-vs-pooled)
- [Experiment 2 - Caching Requests](#experiment-2---caching-requests)
- [Experiment 3 - Splitting the Request Into Tasks](#experiment-3---splitting-the-request-into-tasks)
- [Experiment 4 - Nonblocking I/O](#experiment-4---nonblocking-io)
- [Experiment 5 - Priority Queue](#experiment-5---priority-queue)
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

nprocesses | avg. score 
:---:      |:---:            
1          | 188.714.681          
2          | 79.859.721          
3          | 43.543.070
4          | 25.659.675
5          | 28.900.904 
6          | 29.001.487 
7          | 28.809.463 
8          | 31.135.826 
9          | 29.873.306 
10         | 30.343.382 
15         | 33.180.782 
20         | 31.304.500
25         | 34.435.920
50         | 38.438.750
75         | 39.471.807
100        | 37.809.006

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

nthreads   | avg. score 
:---:      |:---:            
1          | 187.705.339
2          | 79.464.797
3          | 42.884.917 
4          | 24.895.742 
5          | 25.432.171 
6          | 25.442.396 
7          | 27.096.588 
8          | 28.200.715 
9          | 27.874.168 
10         | 28.351.505 
15         | 28.477.808 
20         | 33.355.355
25         | 35.717.697
50         | 38.801.047
75         | 40.486.609
100        | 41.152.935

**Table 2** - Number of processes, nprocesses and the corresponding the average score of 3 runs of the run-client-continuous configuration.

</center>


<br /> 

## Experiment 1c - 4-way Comparison of Multiprocessing vs. Multithreading and ***On the Fly*** vs. ***Pooled***
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

server       | avg. score continuous config. | avg. score delay config. | avg. score difficulty config. | avg. score total config.
:---         |:---:                          |:---:                     |:---:                          |:---:
otfProc      | 45.639.617                    | 85.047.194               | 118.839.452                   | 304.733.020
otfThread    | 40.984.470                    | 79.879.488               | 114.785.781                   | 257.494.808
pooledProc   | 26.952.785                    | 56.119.947               | 86.149.445                    | 175.179.777
pooledThread | 24.848.807                    | 54.961.863               | 80.417.041                    | 168.785.700

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

![Figure 4](/experiments/e2_cache/plot/cached_vs_thread_e2.png "Figure 4 - figure text")
**Figure 4** - Number of processes, nprocesses plotted against the average score of 3 runs of the run-client-continuous configuration. 

</center>

<center>

server       | avg. score continuous config. | avg. score delay config. | avg. score rep config.   | avg. score total config.
:---         |:---:                          |:---:                     |:---:                     |:---:
noCache      | 23.095.305                    | 52.696.637               | 27.749.345               | 79.471.613
Cache        | 12.866.916                    | 50.319.332               | 1.138.442                | 30.957.670

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

![Figure 5](/experiments/e3_split/plot/split_e3.png "Figure 5 - figure text")
**Figure 5** - Number of processes, nprocesses plotted against the average score of 3 runs of the run-client-continuous configuration. 

</center>

<center>

server       | avg. score continuous config. | avg. score delay config. | avg. score difficulty config. | avg. score total config.
:---         |:---:                          |:---:                     |:---:                     |:---:
noSplit      | 12.901.995                    | 50.807.699               | 72.256.643               | 32.018.256
Split        | 6.440.981                     | 47.436.048               | 64.075.291               | 33.719.540

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

![Figure 7](/experiments/e4_nonblocking_IO/plot/nonblock_e4.png "Figure 7 - figure text")
**Figure 7** - Number of processes, nprocesses plotted against the average score of 3 runs of the run-client-continuous configuration. 

</center>

<center>

server             | avg. score continuous config.   | avg. score delay config.   | avg. score total config.
:---               |:---:                            |:---:                       |:---:
Cache              | 12.724.445                      | 51.209.584                 | 33.476.872
NonblockingNoSplit | 24.772.857                      | 65.741.297                 | 103.205.123
NonblockingSplit   | 15.457.998                      | 60.914.793                 | 939.55.287

**Table 7** - Number of processes, nprocesses and the corresponding the average score of 3 runs of the run-client-continuous configuration.

</center>





we were thinking about using semaphores to block the 

not the silver bullet we were hoping for 

<br /> 
<br /> 

# Experiment 5 - Naive Priority Queue : Highest first

**Responsible:** 

Aleksandar

**Motivation:**

As more and more requests are sent to the server, a queue is inbound to happen. At this point, the implementation has to decide on which requests get to be executed first - and by which parameters. When a request is recieved it has a priority value attatched to it. This indicates the importance of execution and its priority over other requests with lower priority. 

Without any system to prioritize higher priority requests, the execution would be dictated simply by _"First-In-First-Out"_-principle, or **F.I.F.O**, with no regard for prioriisation. This could impact the overall benchmark performance in regards to a lower _lamba_-value.   

**Hypothesis:** 

By ordering requests in a priority queue, the requests with higher priority would be executed quicker. This could have an effect on the benchmark, perhapse improving performance due to execution time for high priority requests.

**Relevant files:**

- task_queue.c 
- task_queue.h
- task_priority_queue.c
- task_priority_queue.h
- simple_queue.h
- priority_server.c
- priority_server.h
- client/run-client-continuous.sh
- client/run-client-lambda.sh
- client/run-client-total.sh
- experiments/e5_prio/*

**Setup:**

For testing the priority queue implementation, we have compared the priority_server with the cached_server, on which is was further implemented. The two servers were tested 3 times each using run-client-continuous.sh, run-client-lambda.sh and run-client-total.sh. The result for each client test is then calculated as the average for the 3 executions. By using the three different tests, we can compare the two servers on quantity intake and lambda value. If the hypothesis is to be confirmed, we should see a difference in the execution of run-client-lambda.sh.

**Results:**

The average score of the pre-forked server vs. number of processes used can be seen plotted in Figure 1, as well as in Table 1.

<center>

![Figure 7](/experiments/e5_prio/plot/prio_e5.png "Figure 7 - figure text")
**Figure 7** - Comparison of _Cached thread pool server_ and _priority cached thread pool server_ 

</center>

<center>

server       | avg. score continuous config. | avg. score lambda config. |  score total config.
:---         |:---:                          |:---:                      |:---:                    
noPrio       | 11.821.705                    | 19.440.887                | 30.428.390              
Prio         | 11.945.608                    | 19.218.756                | 31.699.477              

**Table 7** - Average score for each server in the three tests 

</center>

**Conclusion** 

While not taking into account the waiting time of lower priority requests, the performance might be compromised by this side effect. Even if the higher priority requests are executed faster, the waiting time may become exstensive in certain scenarios in which the lower priority requests are put on hold while a large chunk of higher priority requests get executed. Evenmore, the priority queue is constantly being updated with new requests which are enqueued in their corresponding queue. This poses a risk of neglect of lower priority requests. A work around for this could be introducing additional parameters, such as queue time, same priority chunk size, etc.  

# Final Solution


more time scheduling

attribute 

process approach 

panic mode 
but split 
