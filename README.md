1k Iterations with 1k Elements


stock ffq  || stock_X()
    1. server: queue -> dequeue same queue
    2. server: queue -> dequeue sharedMem queue

stock ffq dequeue on new Thread (client_/server_)stockThread_X()
    1. server: queue -> dequeue same queue
    2. server: queue -> dequeue sharedMem queue


modified ffq modified_X()
    1. server: queue -> dequeue // just simple pointer element. No memcpy, just pointer for performance reference 
    2. server: queue -> dequeue // with packet, No memcpy, just pointer for performance reference
    3. server: queue -> dequeue // with int, memcpy and extracting value
    4. server: queue -> dequeue // with packet, memcpy and extracting value


modified ffq with client modifiedClient_X()
    1. server: queue -> client: dequeue // just simple pointer element. No memcpy, just pointer for performance reference 
    2. server: queue -> client: dequeue // with packet, No memcpy, just pointer for performance reference
    3. server: queue -> client: dequeue // with int, memcpy and extracting value
    4. server: queue -> client: dequeue // with packet, memcpy and extracting value
