#include "shared.hpp"
#include "./ffqMod.h"

void run_client();

int main(int argc, char *argv[])
{
    run_client();
}

void run_client()
{

    const char *name = "/shared";
    int fileSize = 192 + 128 + fd_size;

    int fd1 = shm_open(name, O_RDWR, 0777);
    ffq *ext_q = (ffq *)mmap(NULL, fileSize, PROT_WRITE | PROT_READ, MAP_SHARED, fd1, 0);
    int extract;
    int i = 0;
    auto threadStart = std::chrono::steady_clock::now();
    while (1)
    {
        int res = dequeue(ext_q, &extract);
        // if (res > 0)
        // printf("%d\n", extract);
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
