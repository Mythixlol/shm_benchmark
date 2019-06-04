#include "shared.hpp"
#include "./ffqMod.h"

void modified_1();
void modified_2();

void modifiedThread_1();
void server_modifiedThread_1();
void client_modifiedThread_1();

void modifiedThread_2();
void server_modifiedThread_2();
void client_modifiedThread_2();

void modifiedProcess();
void runClient();

// thread 1
ffq *q;
// thread 2 and process
ffq *shm_q;

int main(int argc, char *argv[])
{

    if (argc < 2)
        return -1;
    int cmd = strtol(argv[1], NULL, 10);

    for (int i = 0; i < runs; i++)
    {
        if (cmd == 1)
            modified_1();
        else if (cmd == 2)
            modified_2();
        else if (cmd == 3)
            modifiedThread_1();
        else if (cmd == 4)
            modifiedThread_2();
        else if (cmd == 5)
            modifiedProcess();
    }
}

// 1. server: queue -> dequeue same queue
void modified_1()
{
    ffq *q;
    q = (struct ffq *)malloc(192 + 128 + fd_size);

    new_ffq(q, fd_size, NULL, 1);
    int extract;

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < elements; i++)
    {
        enqueue(q, &i);
        dequeue(q, &extract);
    }
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    std::cout << std::chrono::duration<float, std::milli>(diff).count() << "" << std::endl;
}

// 2. server : queue->dequeue sharedMem queue
void modified_2()
{
    // 128 queue Size || 128 gap-size
    int fileSize = 192 + 128 + fd_size;

    const char *name = "/shared";
    int fd1 = shm_open(name, O_CREAT | O_RDWR, 0777);
    int truncated = ftruncate(fd1, fileSize);
    ffq *shm_q = (ffq *)mmap(NULL, fileSize, PROT_WRITE | PROT_READ, MAP_SHARED, fd1, 0);

    new_ffq(shm_q, fd_size, NULL, 1);
    int extract;

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < elements; i++)
    {
        enqueue(shm_q, &i);
        dequeue(shm_q, &extract);
    }
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    std::cout << std::chrono::duration<float, std::milli>(diff).count() << "" << std::endl;

    munmap(shm_q, fd_size);
    close(fd1);
}

// ################### Threading ##################

auto threadStart = std::chrono::steady_clock::now();

void modifiedThread_1()
{
    for (int i = 0; i < elements; i++)
    {
        dataSet[i] = i;
    }

    q = (struct ffq *)malloc(192 + 128 + fd_size);

    new_ffq(q, fd_size, NULL, 0);
    int extract;

    // threadStart = std::chrono::steady_clock::now();
    std::thread t2(client_modifiedThread_1);
    std::thread t1(server_modifiedThread_1);
    t1.join();
    t2.join();
    // auto end = std::chrono::steady_clock::now();
    // auto diff = end - threadStart;
    // std::cout << std::chrono::duration<float, std::milli>(diff).count() << "" << std::endl;
}

void server_modifiedThread_1()
{
    for (int i = 0; i < elements; i++)
    {
        spsc_enqueue(q, &dataSet[i]);
    }
}

void client_modifiedThread_1()
{
    int i = 0;
    void **extract = (void **)malloc(1 * sizeof(int));
    threadStart = std::chrono::steady_clock::now();
    while (1)
    {
        int res = spsc_dequeue(q, extract);
        // if (res > 0)
        // printf("res %d\n", &extract);
        if (res > 0 && ++i >= elements)
        {
            break;
        }
    }
    auto end = std::chrono::steady_clock::now();
    auto diff = end - threadStart;
    std::cout << std::chrono::duration<float, std::milli>(diff).count() << "" << std::endl;
}

void modifiedThread_2()
{
    for (int i = 0; i < elements; i++)
    {
        dataSet[i] = i;
    }

    // 128 queue Size || 128 gap-size
    int fileSize = 192 + 128 + fd_size;

    const char *name = "/shared";
    int fd1 = shm_open(name, O_CREAT | O_RDWR, 0777);
    int truncated = ftruncate(fd1, fileSize);
    shm_q = (ffq *)mmap(NULL, fileSize, PROT_WRITE | PROT_READ, MAP_SHARED, fd1, 0);

    new_ffq(shm_q, fd_size, NULL, 1);

    // threadStart = std::chrono::steady_clock::now();
    std::thread t2(client_modifiedThread_2);
    std::thread t1(server_modifiedThread_2);
    t1.join();
    t2.join();
    // auto end = std::chrono::steady_clock::now();
    // auto diff = end - threadStart;
    // std::cout << std::chrono::duration<float, std::milli>(diff).count() << "" << std::endl;

    munmap(shm_q, fd_size);
    close(fd1);
}

void server_modifiedThread_2()
{
    for (int i = 0; i < elements; i++)
    {
        // printf("i: %d\n", i);
        enqueue(shm_q, &dataSet[i]);
    }
}

void client_modifiedThread_2()
{
    const char *name = "/shared";
    int fileSize = 192 + 128 + fd_size;

    int fd1 = shm_open(name, O_RDWR, 0777);
    ffq *ext_q = (ffq *)mmap(NULL, fileSize, PROT_WRITE | PROT_READ, MAP_SHARED, fd1, 0);
    int extract;
    int i = 0;
    threadStart = std::chrono::steady_clock::now();
    while (1)
    {
        int res = dequeue(ext_q, &extract);
        // if (res > 0)
        //     printf("ext: %d\n", extract);
        if (res > 0 && ++i == elements)
        {
            break;
        }
    }
    auto end = std::chrono::steady_clock::now();
    auto diff = end - threadStart;
    std::cout << std::chrono::duration<float, std::milli>(diff).count() << "" << std::endl;
    munmap(ext_q, fd_size);
    close(fd1);
}

void modifiedProcess()
{
    for (int i = 0; i < elements; i++)
    {
        dataSet[i] = i;
    }

    // 128 queue Size || 128 gap-size
    int fileSize = 192 + 128 + fd_size;

    const char *name = "/shared";
    int fd1 = shm_open(name, O_CREAT | O_RDWR, 0777);
    int truncated = ftruncate(fd1, fileSize);
    shm_q = (ffq *)mmap(NULL, fileSize, PROT_WRITE | PROT_READ, MAP_SHARED, fd1, 0);

    new_ffq(shm_q, fd_size, NULL, 1);
    // threadStart = std::chrono::steady_clock::now();

    std::thread t2(runClient);
    std::thread t1(server_modifiedThread_2);
    t1.join();
    t2.join();

    // auto end = std::chrono::steady_clock::now();
    // auto diff = end - threadStart;
    // std::cout << std::chrono::duration<float, std::milli>(diff).count() << "" << std::endl;

    munmap(shm_q, fd_size);
    close(fd1);
}

void runClient()
{
    int status = system("./benchClient");
}