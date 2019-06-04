g++ -o bench src/benchmarkServer.cpp src/ffqUnmod.c  -lrt -lpthread -latomic

g++ -o benchMod src/benchmarkModServer.cpp src/ffqMod.c  -lrt -lpthread -latomic
g++ -o benchClient src/benchmarkClient.cpp src/ffqMod.c  -lrt -lpthread -latomic


#./bench 1  
#./bench 2  
#./bench 3  
#./bench 4 
#./benchMod 1
#./benchMod 2
#./benchMod 3
#./benchMod 4
#./benchMod 5

./bench 1 > log/stock.log
./bench 2 > log/stock_shm.log
./bench 3 > log/thread.log
./bench 4 > log/thread_shm.log
./benchMod 1 > log/MODstock.log
./benchMod 2 > log/MODstock_shm.log
./benchMod 3 > log/MODthread.log
./benchMod 4 > log/MODthread_shm.log
./benchMod 5 > log/MODprocess.log
