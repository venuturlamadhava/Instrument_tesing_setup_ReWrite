#pragma once
#include<cstdint>             //It provides fixed-size integer
#include<string>        //It allows you to work with text (words, sentences) in C++.
#include<cstddef>   //It gives you standard definitions for sizes, indexes, and null pointers.
#include<thread>
//#include<pthread.h>     //It allows your program to run multiple tasks (threads) at the same time.

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
    
};
