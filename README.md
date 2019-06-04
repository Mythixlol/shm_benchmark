
**1000 Iterations with 5000 Elements**
<br>
**QueueLenght: 1000 Elements**

stock ffq  || `stock_X()`
1. server: queue -> dequeue same queue
2. server: queue -> dequeue sharedMem queue

stock ffq dequeue on new Thread ||  `(client_/server_)stockThread_X()`
1. server: queue -> dequeue // same queue
2. server: queue -> dequeue // sharedMem queue

modified ffq || `modified_X()`
1. server: queue -> dequeue // same queue
2. server: queue -> dequeue // sharedMem queue
  
  
modified ffq dequeue on new Thread  || `(client_/server_)modifiedThread_X()`
1. server: queue -> client: dequeue  // same queue, global
2. server: queue -> client: dequeue  // shared memory queue 

<br><br>

interprocess: `server_modifiedThread_2() => runClient()`
- use same server queue like in task 8. -> open new process -> shared memory queue dequeue

