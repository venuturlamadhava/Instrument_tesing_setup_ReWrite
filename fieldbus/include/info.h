#pragma once
#include <cstdint>
#include "ecrt.h"

struct slave_info 
{
    std::uint16_t alias;
    std::uint16_t position;
    std::uint32_t vendor_id;
    std::uint32_t product_code;
};

struct DriveData
{
    ec_slave_config* sc_slave_config;
    ec_slave_config_state_t sc_slave_config_state;
    unsigned int off_status_word;
    unsigned int off_actual_position;
    unsigned int off_actual_velocity;
    unsigned int off_actual_torque;
    unsigned int off_digital_inputs;
    unsigned int off_mode_display;
    unsigned int off_control_word;
    unsigned int off_target_position;
    unsigned int off_target_velocity;
    unsigned int off_target_torque;
    unsigned int off_mode_operation;
};

extern slave_info slaves[];

enum class RfidState : uint8_t
{
    WAIT_OP,
    SEND_DETECT,
    WAIT_PRESENT,
    SEND_READ_65,
    WAIT_DATA,
    SEND_ACK,
    WAIT_READY,
    DONE
};
