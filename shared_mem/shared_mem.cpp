#include<iostream>
#include<sys/mman.h>
#include<fcntl.h>
#include<unistd.h>
#include<cstring>

#include "shared_mem.h"

bool SharedMemory::create()
{
    int fd=shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if(fd==-1)
    {
        std::cout << "Failed to create shared memory." << std::endl;
        return false;
    }
    if(ftruncate(fd, size)== -1)
    {
        std::cout << "Failed to set shared memory size" << std::endl;
        ::close(fd);
        return false;
    }

    void* ptr=mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,0);
    if (ptr == MAP_FAILED)
    {
        std:: cout << "Failed to map shared memory" << std::endl;
        ::close(fd);
        return false;
    }

    data = static_cast<SharedData*>(ptr);
    ::close(fd);

    memset(data,0,size);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&data->mutex, &attr);
    pthread_mutexattr_destroy(&attr);

    is_creator = true;
    return true;
    
}

bool::SharedMemory::open()
{
    int fd=shm_open(SHM_NAME, O_RDWR,0);
    if(fd == -1)
    {
        std::cout << "Failed to open shared memory. " << std::endl;
        return false;
    }
    void* ptr=mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED)
    {
        std::cout << "Failed to map shared memory." << std::endl;
        ::close(fd);
        return false;
    }
    data = static_cast<SharedData*>(ptr);
    ::close(fd);
    is_creator =false;
    return true;
}

void SharedMemory::lock()
{
    pthread_mutex_lock(&data->mutex);
}

void SharedMemory::unlock()
{
    pthread_mutex_unlock(&data->mutex);
}

SharedData* SharedMemory::get_data()
{
    return data;
}

void SharedMemory::close()
{
    if(data !=nullptr)
    {
        munmap(data, size);
    }
    data =nullptr;
    if(is_creator)
    {
        shm_unlink(SHM_NAME);
    }
    is_creator = false;
}



