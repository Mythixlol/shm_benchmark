#include "shared.hpp"
#include "./ffqUnmod.h"

void stock_1();
void stock_2();

void stockThread_1();
void server_stockThread_1();
void client_stockThread_1();

void stockThread_2();
void client_stockThread_2();
void server_stockThread_2();

int main(int argc, char *argv[])
{
    if (argc < 2)
        return -1;

    int cmd = strtol(argv[1], NULL, 10);

    for (int i = 0; i < runs; i++)
    {
        if (cmd == 1)
            stock_1();
        else if (cmd == 2)
            stock_2();
        else if (cmd == 3)
            stockThread_1();
        else if (cmd == 4)
            stockThread_2();
    }
}

// 1. server: queue -> dequeue same queue
void stock_1()
{
    ffq q;
    new_ffq(&q, fd_size, NULL);
    void **extract = (void **)malloc(1 * sizeof(int));

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < elements; i++)
    {
        spsc_enqueue(&q, &i);
        spsc_dequeue(&q, extract);
    }
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    std::cout << std::chrono::duration<float, std::milli>(diff).count() << "" << std::endl;
}

// 2. server : queue->dequeue sharedMem queue
void stock_2()
{
    const char *name = "/shared";

    int fd1 = shm_open(name, O_CREAT | O_RDWR, 0777);
    int truncated = ftruncate(fd1, fd_size);
    ffq *shm_q = (ffq *)mmap(NULL, fd_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd1, 0);
    new_ffq(shm_q, fd_size, NULL);
    void **extract = (void **)malloc(1 * sizeof(int));

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < elements; i++)
    {
        spsc_enqueue(shm_q, &i);
        spsc_dequeue(shm_q, extract);
    }
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    std::cout << std::chrono::duration<float, std::milli>(diff).count() << "" << std::endl;

    munmap(shm_q, fd_size);
    close(fd1);
}

// ################### Threading ##################
ffq q;

auto threadStart = std::chrono::steady_clock::now();

void stockThread_1()
{

    for (int i = 0; i < elements; i++)
    {
        dataSet[i] = i;
    }
    new_ffq(&q, fd_size, NULL);
    std::thread t2(client_stockThread_1);
    std::thread t1(server_stockThread_1);

    t2.join();
    t1.join();
}

void server_stockThread_1()
{
    // threadStart = std::chrono::steady_clock::now();
    for (int i = 0; i < elements; i++)
    {
        spsc_enqueue(&q, &dataSet[i]);
    }
}

void client_stockThread_1()
{
    void **extract = (void **)malloc(1 * sizeof(int));
    int i = 0;

    threadStart = std::chrono::steady_clock::now();
    while (1)
    {
        int res = spsc_dequeue(&q, extract);
        if (res > 0 && ++i == elements)
        {
            break;
        }
    }
    auto end = std::chrono::steady_clock::now();
    auto diff = end - threadStart;
    std::cout << std::chrono::duration<float, std::milli>(diff).count() << "" << std::endl;
}

ffq *shm_q;
auto threadStart2 = std::chrono::steady_clock::now();

void stockThread_2()
{
    for (int i = 0; i < elements; i++)
    {
        dataSet[i] = i;
    }

    const char *name = "/shared";

    int fd1 = shm_open(name, O_CREAT | O_RDWR, 0777);
    int truncated = ftruncate(fd1, fd_size);
    shm_q = (ffq *)mmap(NULL, fd_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd1, 0);
    new_ffq(shm_q, fd_size, NULL);

    std::thread t2(client_stockThread_2);
    std::thread t1(server_stockThread_2);

    t1.join();
    t2.join();

    munmap(shm_q, fd_size);
    close(fd1);
}

void server_stockThread_2()
{

    for (int i = 0; i < elements; i++)
    {
        spsc_enqueue(shm_q, &dataSet[i]);
    }
}

void client_stockThread_2()
{
    const char *name = "/shared";

    int fd1 = shm_open(name, O_RDWR, 0777);
    ffq *ext_q = (ffq *)mmap(NULL, fd_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd1, 0);
    void **extract = (void **)malloc(1 * sizeof(int));
    int i = 0;

    threadStart2 = std::chrono::steady_clock::now();
    while (1)
    {
        int res = spsc_dequeue(ext_q, extract);
        if (res > 0 && ++i == elements)
        {
            break;
        }
    }
    auto end = std::chrono::steady_clock::now();
    auto diff = end - threadStart2;
    std::cout << std::chrono::duration<float, std::milli>(diff).count() << "" << std::endl;

    munmap(shm_q, fd_size);
    close(fd1);
}
