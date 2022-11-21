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

## Testing 

variations of the continuous 
different seeds. 

files previous not mentioned redundant 


<br /> 
<br /> 


# Experiment 1 - Multiprocessing vs. Multithreading and ***On the Fly*** vs. ***Pooled***
**Responsible:** 

William

**Motivation:**

Our benchmark server was implemented using a "fork per request" approach, meaning that for each request the server would create a new process to perform the reverse hashing, after which the process would terminate. The overhead associated with creating a new process per request is not necessarily cheap in regards to performance, and using a different approach might also allow for better parallelization, so the first experiments were dedicated to investigating alternatives to this method.

The obvious alternative to multiprocessing is multithreading, and for both, two versions were experimented with, which we will refer to as ***On the Fly*** and ***Pooled***. ***On the Fly*** refers to creating a new process/thread for each request, while ***Pooled*** refers to having some fixed number of pre-forked/pre-threaded processes/threads ready for requests which will be reused with the intention of cutting down on the spawning related overhead and utilizing the hardware more efficiently. All four versions were experimented with simultaneously as to not introduce a bias that would for instance conclude ***On the fly*** multithreading beats ***On the Fly*** multiprocessing, and then continue to experiment with ***On the Fly*** multithreading and ***Pooled*** multithreading without considering ***Pooled*** multiprocessing. 

Before comparing the four approaches, a vital property of the ***Pooled*** versions had to be determined, namely the size of the pools, i.e. the number of pre-created processes/threads. The reasons for doing so builds on the assumption that some pool sizes grant better performance than others, and since the intention is to identify an optimal server, comparing a suboptimal ***Pooled*** server to a ***On the Fly*** server wouldn't make sense. Determining the optimal number of processes and threads for the ***Pooled*** multiprocessing server and the ***Pooled*** multithreading server, while testing the assumption that the pool sizes influence server performance, was the goal of the experiments 1a and 1b, respectively. 

<br /> 


## Experiment 1a - Determining the Optimal Number of Processes of a Pre-Forked Server 
**Hypothesis:** 

The number of processes of a pre-forked server significantly influences the server's performance.

**Relevant files/folders:**

- src/brute_force.c
- src/brute_force.h
- src/forked_server.c
- src/forked_server.h
- client/run-client-continuous.sh
- experiments/e1/e1a/

**Setup:**

For this experiment the run-client-continuous configuration was executed 3 times for every number of processes from 1 to 10, from 10 to 25 in steps of 5, and from 25 to 100 in steps of 25 (i.e., 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 50, 75, and 100) and the average was then taken of the 3 executions. This was done in order to cover a wide range of options for the number of processes, that would hopefully make some pattern emerge that could be concluded on in regards to how the number of pre-forked processes effected the server's performance. 

**Results:**

The average score of the pre-forked server vs. number of processes used, nprocesses, can be seen plotted in Figure 1, and the numerical values can be seen listed in Table 1.

<center>

![Figure 1](/experiments/e1_otf_vs_pooled/e1a/plot/preforked_e1a.png "Figure 1 - Number of processes, nprocesses, plotted against the average score of 3 runs of the run-client-continuous configuration for the pre-forked server.")
**Figure 1** - Number of processes, nprocesses, plotted against the average score of 3 runs of the run-client-continuous configuration for the pre-forked server. 

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

**Table 1** - Number of processes, nprocesses, and the corresponding average score of the 3 runs of the run-client-continuous configuration for the pre-forked server.

</center>
<br /> 

From the result a pattern emerged indicating that 1) the number of processes greatly influenced the performance of the server, with 1 process scoring over 7 times higher than 4 processes, and 2) that the optimal number of processes to use for the pre-forked server was 4, which led us to accept the hypothesis. 4 processes being optimal was most likely explained by the VM the server was running ontop of had 4 CPUs, which theory should result in less context switching while utilizing all 4 CPUs.      

Based on the results from experiment 1a, it was decided to use 4 pre-forked processes for comparing the four approaches.

<br /> 

## Experiment 1b - Determining the Optimal Number of Threads of a Thread Pool Server 
**Hypothesis:** 

The number of threads of a thread pool server significantly influences the server's performance. 

**Relevant files:**

- src/threaded_server.c
- src/threaded_server.h
- src/simple_queue.c
- src/simple_queue.h
- client/run-client-continuous.sh
- experiments/e1/e1b/

**Setup:**

Similar to experiment 1a, the setup involved running the run-client-continuous configuration 3 times for every number of threads from 1 to 10, from 10 to 25 in steps of 5, and from 25 to 100 in steps of 25 and then take the average of those 3 executions. Again, this was done in order to cover a wide range of options for the number of threads, that would hopefully make some pattern emerge that could be concluded on in regards to how the pool size of a thread pool server effected performance. 

**Results:**

The average score of the thread pool server vs. number of threads used can be seen plotted in Figure 2, and the numerical values can be seen listed in Table 2.

<center>

![Figure 2](/experiments/e1_otf_vs_pooled/e1b/plot/threadpool_e1b.png "Figure 2 - Number of threads, nthreads, plotted against the average score of 3 runs of the run-client-continuous configuration.")
**Figure 2** - Number of threads, nthreads, plotted against the average score of 3 runs of the run-client-continuous configuration. 

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

**Table 2** - Number of threads, nthreads, and the corresponding average score of 3 runs of the run-client-continuous configuration for the thread pool server.

</center>

Similar to experiment 1a, the number of threads used seemed to greatly influence the server's performance, and the optimal number was again found to be 4 for the number of threads (again, most likely due to the underlaying (virtual) hardware), and the hypothesis was accepted.  

Based on the results from experiment 1b, it was decided to use a thread pool size of 4 threads for comparing the four approaches.

<br /> 

## Experiment 1c - 4-way Comparison of Multiprocessing vs. Multithreading and ***On the Fly*** vs. ***Pooled***

Having determinded the optimal sizes of the ***Pooled*** versions, 4 processes for the pre-forked server and 4 threads for the thread pool server, an overall comparison of multiprocessing and multithreading was conducted, to determine the more optimal of the four server approaches. 

**Hypothesis:** 

Using multiprocessing (***On the Fly*** and ***Pooled***) will lead to a difference in server performance compared to using multithreading (***On the Fly*** and ***Pooled***) from which a more optimal server can be identified.

**Relevant files:**

- src/brute_force.c
- src/brute_force.h
- src/forked_server.c
- src/forked_server.h
- src/threaded_server.c
- src/threaded_server.h
- src/simple_queue.c
- src/simple_queue.h
- client/run-client-continuous.sh
- client/run-client-test-delay.sh
- client/run-client-test-difficulty.sh
- client/run-client-test-total.sh
- experiments/e1/e1c/

**Setup:**

In order to identify the more optimal server in regards to the challenge, as well as to identify potential downfalls of an approach, the four approaches were tested against four different client configurations: one using the continuous configuration, one with decreased delay (100.000), one with increased difficulty (60.000.000), and one mimicking the final run of the challenge (at the time of testing, the final challenge was a total of 1000 requests, instead of the 500 it was change to. This experiment used 1000 requests for the configuration mimicking the final challenge, while it was changed to 500 for the other experiments). 

Each configuration was executed 3 times per server and the average score of the 3 executions was used for making the comparisons. 

**Results:**

The average score of 3 runs of the four server approaches against the four client configurations can be seen plotted in Figure 3, and the numerical values can be seen listed in Table 3.

<center>

![Figure 3](/experiments/e1_otf_vs_pooled/e1c/plot/compare_e1c.png "Average score of 3 runs each of the four server approaches against the four client configurations.")
**Figure 3** - Average score of 3 runs each of the four server approaches against the four client configurations.

</center>

<center>

server       | avg. score continuous config. | avg. score delay config. | avg. score difficulty config. | avg. score total config.
:---         |:---:                          |:---:                     |:---:                          |:---:
otfProc      | 45.639.617                    | 85.047.194               | 118.839.452                   | 304.733.020
otfThread    | 40.984.470                    | 79.879.488               | 114.785.781                   | 257.494.808
pooledProc   | 26.952.785                    | 56.119.947               | 86.149.445                    | 175.179.777
pooledThread | 24.848.807                    | 54.961.863               | 80.417.041                    | 168.785.700

**Table 3** - Average score of 3 runs for the four server approaches against the four client configurations.

</center>

From the results, it was very clear that the ***Pooled*** versions outperformed the ***On the Fly*** versions across all four configurations. The ***Pooled*** versions were closer in terms of average score, although the thread pool version proved to be the most optimal across all four client configurations.

Based on the results the fork per request approach was substituted with the thread pool (using 4 threads) approach for the server implementaion. 


<br /> 
<br /> 


# Experiment 2 - Caching Requests
**Responsible:** 

William

**Motivation:**

Given that the client had a chance to send repeated requests, an experiment was dedicated to investigating whether or not caching the requests would lead to increased server performance, as the server would then be able to respond immediately rather than spending resources re-brute forcing the hash of the repeated request. 

This caching functionality was implemented using a hash table with a 1000 buckets to minimize the chance of conclusions that would lead to resources being spend on collision resolution. 

**Hypothesis:** 

Caching requests and using those to respond to repeated requests would increase server performance significantly.    

**Relevant files:**

- src/hash_table.c
- src/hash_table.h
- src/cached_server.c
- src/cached_server.h
- client/run-client-continuous.sh
- client/run-client-test-rep.sh
- client/run-client-test-total-500.sh
- test/experiments/e2/

**Setup:**

For testing the caching functionality, the thread pool server (the currently most optimal server) was executed against three client configurations with (refered to as Cache in Figure 4 and Table 4) and without (refered to as noCache in Figure 4 and Table 4) caching to determine if the caching improved the server's performance or not. The three client configurations were the continuous, a configuration with increased repetition chance (80%), and the one mimicking the final run of the challenge.  

**Results:**

The average score of 3 runs of the thread pool server with (Cache) and without (noCache) caching against the three client configurations can be seen plotted in Figure 4, and the numerical values can be seen listed in Table 4.

<center>

![Figure 4](/experiments/e2_cache/plot/cached_vs_thread_e2.png "Figure 4 - Average score of 3 runs of the thread pool server with (Cache) and without (noCa) caching against the three client configurations.")
**Figure 4** - Average score of 3 runs of the thread pool server with (Cache) and without (noCa) caching against the three client configurations.

</center>

<center>

server       | avg. score continuous config. | avg. score rep config.   | avg. score total config.
:---         |:---:                          |:---:                     |:---:
noCache      | 23.095.305                    | 27.749.345               | 79.471.613
Cache        | 12.866.916                    | 1.138.442                | 30.957.670

**Table 4** - Average score of 3 runs of the thread pool server with (Cache) and without (noCa) caching against the three client configurations.

</center>

The cached version of the thread pool server performed significantly better than the version without caching, resulting in an almost 50% performance increase for the continuous configuration and even more so for the final run mimicking configuration, which lead to the hypothesis being accepted.  

Based on the results the caching functionality was adopted into the server implementaion.


<br /> 
<br /> 


# Experiment 3 - Splitting the Request Into Tasks
**Responsible:** 

William

**Motivation:**
 
The cached thread pool server was dedicating each thread to a single request. An alternative to this could be to split the request into 4 tasks for each of the threads to work on in parallel, and in theory work through each request at a greater speed. Experiment 3 was dedicated to examine if this approach would lead to an increase in server performance. 

**Hypothesis:** 

Splitting the requests into chunks for the threads to work on will lead to an increase in the server's performance.

**Relevant files:**

- src/split_request_server.c
- src/split_request_server.h
- client/run-client-continous.sh
- client/run-client-test-difficulty.sh
- client/run-client-test-total-500.sh
- test/experiments/e3/

**Setup:**

For testing the split request functionality, the cached thread pool server (the currently most optimal server) was executed against three client configurations with (refered to as Split in Figure 5 and Table 5) and without (refered to as noSplit in Figure 5 and Table 5) split requests to determine if the splitting of request into tasks improved the server's performance or not. The three client configurations were the continuous, a configuration with increased difficulty (60.000.000), and the one mimicking the final run of the challenge.

**Results:**

The average score of 3 runs of the cached thread pool server with (Split) and without (noSPlit) splitting of the request into task against the three client configurations can be seen plotted in Figure 5, and the numerical values can be seen listed in Table 5.

<center>

![Figure 5](/experiments/e3_split/plot/split_e3.png "Figure 5 - Average score of 3 runs of the cached thread pool server with (Split) and without (noSPlit) splitting of the request into task against the three client configurations.")
**Figure 5** - Average score of 3 runs of the cached thread pool server with (Split) and without (noSPlit) splitting of the request into task against the three client configurations.

</center>

<center>

server       | avg. score continuous config. | avg. score difficulty config. | avg. score total config.
:---         |:---:                          |:---:                          |:---:
noSplit      | 12.901.995                    | 72.256.643                    | 32.018.256
Split        | 6.440.981                     | 64.075.291                    | 33.719.540

**Table 5** - Average score of 3 runs of the cached thread pool server with (Split) and without (noSPlit) splitting of the request into task against the three client configurations.

</center>

For the continuous configuration the server that was splitting the requests into tasks performed twice as good as the one that did not, but as the number of requests grew the score seemed to equal out, as seen for the total configuration, that was sending 500 requests. To examine this behavior further, the server with and without splitting was executed against a configuration with 1000 total requests, were the server that was splitting scored 65.442.650, while the server that did not scored 52.045.404, which meant that the splitting server performed worse for larger number of total requests. 

As the goal of the challenge was to implement an optimal server for the final run configuration, and not the continuous configuration, it was hard to justify the adoptation of the splitting request into tasks functionality, and ultimately the adoptaion of the splitting requests into tasks functionality was rejected based on the results of experiment 3. 

  
<br /> 
<br /> 


# Experiment 4 - Nonblocking I/O
**Responsible:** 

William

**Motivation:**

For the chaced thread pool server, the thread worker functions contained a blocking I/O call (`write()` to respond to the client). Having blocking I/O calls inside the thread worker function might cause them to stall, and worsen the performance of the server. For testing this, the `write()` call was moved to the "conducting" thread, the one that was doing the `accept()`, `read()` that would be in charge of doing all the I/O.

For the "conducting" thread to be able to respond to the client, while still accepting new requests, the I/O had to be made nonblocking (otherwise the "conducting" thread could get blocked on `accept()` after the last requests, while still having responses to `write()`), which was done using `epoll`.

Wanting to find a remedy for the behavior of splitting request into tasks, that would worsen as the total number of requests grew, it was decided to implement a split requests version of the nonblocking I/O as well, given the possibility that the blocking I/O, might have had something to do with the aforementioned behavior.
 
**Hypothesis:** 

Having blocking IO calls inside of the thread worker functions is worsening the server's performance and moving those calls to the conducting thread will increase server performance. 

**Relevant files:**

- src/nonblocking_IO_server.c
- src/nonblocking_IO_server.h
- client/run-client-continous.sh
- client/run-client-test-difficulty.sh
- client/run-client-test-total-500.sh
- test/experiments/e1/e1a/

**Setup:**
 
For testing the nonblocking I/O functionality, the cached thread pool server (the currently most optimal server), and a nonblocking I/O cached thread pool server server with (NonblockingSplit) and without (NonblockingNoSPlit) splitting of the request into task was executed against three client configurations. The three client configurations were the continuous, a configuration with decreased delay (100.000), and the one mimicking the final run of the challenge.

**Results:**

The average score of 3 runs of the cached thread pool server (Cache), and the nonblocking I/O server with (NonblockingSplit) and without (NonblockingNoSPlit) splitting of the request into task against the three client configurations can be seen plotted in Figure 6, and the numerical values can be seen listed in Table 6.

<center>

![Figure 6](/experiments/e4_nonblocking_IO/plot/nonblock_e4.png "Figure 6 - Average score of 3 runs of the cached thread pool server (Cache), and the nonblocking I/O server with (NonblockingSplit) and without (NonblockingNoSPlit) splitting of the request into task against the three client configurations.")
**Figure 6** - Average score of 3 runs of the cached thread pool server (Cache), and the nonblocking I/O server with (NonblockingSplit) and without (NonblockingNoSPlit) splitting of the request into task against the three client configurations.

</center>

<center>

server             | avg. score continuous config.   | avg. score delay config.   | avg. score total config.
:---               |:---:                            |:---:                       |:---:
Cache              | 12.724.445                      | 51.209.584                 | 33.476.872
NonblockingNoSplit | 24.772.857                      | 65.741.297                 | 103.205.123
NonblockingSplit   | 15.457.998                      | 60.914.793                 | 939.55.287

**Table 7** -  Average score of 3 runs of the cached thread pool server (Cache), and the nonblocking I/O server with (NonblockingSplit) and without (NonblockingNoSPlit) splitting of the request into task against the three client configurations.

</center>

From the results no performance increase from either the nonblocking I/O with or without splitting requests could be concluded. One quite likely reason for this, that we failed to notice before implementing the nonblocking I/O server, was that having the "conducting" thread constantly checking for responses to send out might have caused ineffiecient resource utilization, in contrast to the cached threaded server that would block and allow the worker threads to perform the reverse hashing. For the nonblocking approach to have been an improvement to server performance some sort of scheduling would perhaps have been necessary.  

Based on the results the hypothesis was rejected and the neither of the nonblocking I/O servers was not adopted into the server implementation.


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
- client/run-client-lambda-medium.sh
- client/run-client-lambda-low.sh
- client/run-client-lambda-lower.sh
- experiments/e5_prio/*

**Setup:**

For testing the priority queue implementation, we have compared the priority_server with the cached_server, on which is was further implemented. The two servers were tested 3 times each using run-client-continuous.sh, run-client-lambda.sh and run-client-total.sh. The result for each client test is then calculated as the average for the 3 executions. By using the three different tests, we can compare the two servers on quantity intake and lambda value. If the hypothesis is to be confirmed, we should see a difference in the execution of run-client-lambda.sh. In these tests the lambda value has been set to 0.25, 0.17 and 0.1 respectivly.

**Results:**

The average score of the pre-forked server vs. number of processes used can be seen plotted in Figure 1, as well as in Table 1.

<center>

![Figure 7](/experiments/e5_prio/plot/prio_e5.png "Figure 7 - figure text")
**Figure 7** - Comparison of _Cached thread pool server_ and _priority cached thread pool server_.

</center>

<center>

server       | avg. score continuous config. | avg. score lambda config. |  score total config.
:---         |:---:                          |:---:                      |:---:                    
noPrio       | 11.821.705                    | 19.440.887                | 30.428.390              
Prio         | 11.945.608                    | 19.218.756                | 31.699.477              

**Table 7** - Average score for each server in the three tests 

</center>

<center>
    
lambda  | noPrio avg. score | prio avg. score   |
:---    |:---:              |:---:              |                    
0.25    | x                 | x                 |
0.17    | x                 | x                 |
0.10    | 945.117.098       | 500.816.263       |  

**Table 8** - Average score for each server when only lambda is adjusted 
    
</center>

**Conclusion** 

While not taking into account the waiting time of lower priority requests, the performance might be compromised by this side effect. Even if the higher priority requests are executed faster, the waiting time may become exstensive in certain scenarios in which the lower priority requests are put on hold while a large chunk of higher priority requests get executed. Evenmore, the priority queue is constantly being updated with new requests which are enqueued in their corresponding queue. This poses a risk of neglect of lower priority requests. A work around for this could be introducing additional parameters, such as queue time, same priority chunk size, etc.

In the lambda tests, there was found to be a clear advantage in the priority queue as lambda is set lower.
However, due to the performance having negligible difference - an even performing a tiny bit worse in the continuous.sh andf total.sh, the implementation was discarded from the final solution.

As a final remark, the priority queue was in our server implemented in the hope of optimizing the performance rather than reliability. In case of future development, a more complex priority hiarichy could yield better results.


<br />
<br />

# Final Solution

The final solution consisted of a cached thread pool server implementation. 






more time scheduling

attribute 

process approach 

panic mode 
but split 


hash table vs array

reflect here? from experiment ... and from experiment ...
