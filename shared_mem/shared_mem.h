#pragma once
#include<cstdint>             //It provides fixed-size integer
#include<string>        //It allows you to work with text (words, sentences) in C++.
#include<cstddef>   //It gives you standard definitions for sizes, indexes, and null pointers.
#include<thread>
//#include<pthread.h>     //It allows your program to run multiple tasks (threads) at the same time.
using namespace std;

constexpr const char* SHM_NAME="/shared_mem";
constexpr int NUM_OF_DRIVES=4;

enum class modes:std::int8_t
{
    UNKNOWN=-1,
    CSP=8,
    CSV,
    CST
};

enum class CIA402_State:std::int16_t
{
    NOT_READY_TO_SWITCH_ON,
    SWITCH_ON_DISABLED,
    READY_TO_SWITCH_ON,
    SWITCHED_ON,
    OPERATION_ENABLED,
    QUICK_STOP_ACTIVE,
    FAULT_REACTION_ACTIVE,
    FAULT,
    UNKNOWN
};

struct RFIDStatus
{
    std::uint8_t status_word=0;
    std::uint8_t instrument_id=0;
};

struct RFIDCommand
{
    std::uint8_t control_word=0;
    std::uint8_t header=0;
};

struct DriveStatus
{
    std::uint16_t status_word=0;
    std::int32_t actual_position=0;
    std::int32_t actual_velocity=0;
    std::int16_t actual_torque=0;
    std::uint32_t digital_input=0;
    std::int8_t mode_of_operation_display=0;
};

struct DriveCommand
{
    std::uint16_t control_word=0;
    std::int32_t target_position=0;
    std::int32_t target_velocity=0;
    std::int16_t target_torque=0;
    std::int8_t mode_of_operation=0;
};

struct SharedData
{
    pthread_mutex_t mutex; //It allows multiple threads to access shared data without conflicts.
    RFIDStatus rfid_status;
    RFIDCommand rfid_command;
    DriveStatus drive_status[NUM_OF_DRIVES];
    DriveCommand drive_command[NUM_OF_DRIVES]; 
    std::uint32_t fieldbus_heartbeat=0;
    std::uint32_t motion_heartbeat=0;
    bool all_drives_ready=false;
};

class SharedMemory
{
    private:
    SharedData* data=nullptr; //It is a pointer that will point to the shared data in memory.
    size_t size=sizeof(SharedData);
    bool is_creator=false;

    public:
    bool create();
    bool open();
    void lock();
    void unlock();
    SharedData* get_data();
    void close();
};