#include<iostream>
#include<signal.h>
#include<unistd.h>
#include<cstring>
#include "shared_mem.h"

volatile bool running = true;

void signal_handler(int sig)
{
    (void)sig;
    running = false;
}

int main()
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    SharedMemory shared_mem;
    if(!shared_mem.create())
    {
        std:: cout<< "Failed to create shared memory" << std::endl;
        return 1;
    }

    std:: cout << "Shared memory created at /dev/shm" << SHM_NAME << std::endl;
    std:: cout << "size:" << sizeof(SharedData) << "bytes" << std::endl;
    std::cout << "Waiting for fieldbus and motion module..." << std::endl;

    SharedData* data = shared_mem.get_data();
    SharedData local_copy;

    while(running)
    {
        usleep(100000);

        std::cout << "\n--- Heartbeats---- Fieldbus: " << local_copy.fieldbus_heartbeat
                  << " Motion: " << local_copy.motion_heartbeat << std::endl; 
        std::cout << "RFID SW: " <<(int)local_copy.rfid_status.status_word 
                  << "Instrument_ID:" << (int)local_copy.rfid_status.instrument_id
                  << " | CW: " << (int) local_copy.rfid_command.control_word<< std::endl;

        for(int i=0; i<NUM_OF_DRIVES; i++)
        {
            std::cout << "Drive " << i << " | "
                      << "SW:" <<local_copy.drive_status[i].status_word
                      << "Pos:"<< local_copy.drive_status[i].actual_position
                      << "Vel:"<< local_copy.drive_status[i].actual_velocity
                      << "Tor:"<< local_copy.drive_status[i].actual_torque
                      << "DI: "<< local_copy.drive_status[i].digital_input
                      << "Mode:" << (int)local_copy.drive_status[i].mode_of_operation_display
                      << " | CW: " << local_copy.drive_command[i].control_word
                      << "Tgt Pos: " << local_copy.drive_command[i].target_position
                      << "Tgt Vel: " << local_copy.drive_command[i].target_velocity
                      << "Tgt Tor: " << local_copy.drive_command[i].target_torque
                      << "Mode: " << (int)local_copy.drive_command[i].mode_of_operation
                      << std::endl;                    
        }
    }

    std::cout <<"\nShutting down shared memory..." <<std:: endl;
    shared_mem.close();
    std::cout << "sthared memory destryed" << std::endl;
    return 0;

}