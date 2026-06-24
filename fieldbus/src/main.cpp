#include<iostream>
#include <time.h>
#include <sys/mman.h>
#include<signal.h>
#include <unistd.h>
#include <atomic>

#include "EthercatMaster.h"
#include "shared_mem.h"
#include "info.h"

#define CYCLE_TIME_NS (1000000) // 1ms
#define WARMUP_CYCLES 3000      //300ms warm-up for DC sync


struct timespec wakeup_time;

// to check slave and master states
static int counter = 0;

//process data pointer

static uint8_t *domain_pd = nullptr;

//Using atomic signal handler and threads see update immediately
static std::atomic<bool> running{true};

DriveData Drive_data[NUM_OF_DRIVES];

//To Access Shared memory
SharedMemory fieldbus_shared_m;
SharedData* data = nullptr;

static bool all_drives_op=false;
static RfidState rfid_state =RfidState::WAIT_OP;
static int settle_cycles = 0;

//====================================
//Slave 0 offsets (PIC32 I/O MODULE - ALIAS 0, POSITION 0)
//===========================================
//RxPDO (output to slave)
static unsigned int off_slave0_control_word;
static unsigned int off_slave0_header_out;
static unsigned int off_slave0_instrument_type_out;
static unsigned int off_slave0_usage_limit_out;
static unsigned int off_slave0_instrument_version_out;
static unsigned int off_slave0_mfg_country_code_out;
static unsigned int off_slave0_mfg_plant_code_out;
static unsigned int off_slave0_mfg_date_out;
static unsigned int off_slave0_mfg_month_out;
static unsigned int off_slave0_mfg_year_out;
static unsigned int off_slave0_qc_status_out;
static unsigned int off_slave0_used_hsptl_code_out;
static unsigned int off_slave0_number_of_useage_out;
static unsigned int off_slave0_last_used_date_out;
static unsigned int off_slave0_last_used_month_out;
static unsigned int off_slave0_last_used_year_out;
static unsigned int off_slave0_last_used_hour_out;


//TxPDO entry (input from slave)
static unsigned int off_slave0_status_word;
static unsigned int off_slave0_header_in;
static unsigned int off_slave0_instrument_type_in;
static unsigned int off_slave0_usage_limit_in;
static unsigned int off_slave0_instrument_version_in;
static unsigned int off_slave0_mfg_country_code_in;
static unsigned int off_slave0_mfg_plant_code_in;
static unsigned int off_slave0_mfg_date_in;
static unsigned int off_slave0_mfg_month_in;
static unsigned int off_slave0_mfg_year_in;
static unsigned int off_slave0_qc_status_in;
static unsigned int off_slave0_used_hsptl_code_in;
static unsigned int off_slave0_number_of_usage_in;
static unsigned int off_slave0_last_used_date_in;
static unsigned int off_slave0_last_used_month_in;
static unsigned int off_slave0_last_used_year_in;
static unsigned int off_slave0_last_used_hour_in;

//==========================================================
//PDO entry registration - ALL 5 slaves
//==========================================================


ec_pdo_entry_reg_t domain_reg[]=
{
    //-------------Slave 0 (PIC32 I/O, alias 0, position 0)--------------
    //RxPDO 0x1600
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_control_word},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_header_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_instrument_type_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_usage_limit_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_instrument_version_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_mfg_country_code_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_mfg_plant_code_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_mfg_date_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_mfg_month_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_mfg_year_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_qc_status_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_used_hsptl_code_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_number_of_useage_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_last_used_date_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_last_used_month_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_last_used_year_out},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x7000, 0x01, &off_slave0_last_used_hour_out},

    //TxPDO 0x1A00
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x01, &off_slave0_status_word},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x02, &off_slave0_header_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x03, &off_slave0_instrument_type_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x04, &off_slave0_usage_limit_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x05, &off_slave0_instrument_version_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x06, &off_slave0_mfg_country_code_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x07, &off_slave0_mfg_plant_code_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x08, &off_slave0_mfg_date_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x09, &off_slave0_mfg_month_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x10, &off_slave0_mfg_year_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x11, &off_slave0_qc_status_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x12, &off_slave0_used_hsptl_code_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x13, &off_slave0_number_of_usage_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x14, &off_slave0_last_used_date_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x15, &off_slave0_last_used_month_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x16, &off_slave0_last_used_year_in},
    {0, slaves[0].position, slaves[0].vendor_id, slaves[0].product_code, 0x6000, 0x17, &off_slave0_last_used_hour_in},

    //--------------Slave 1 (drive, alias 0, position 1)--------------------
    {0, slaves[1].position, slaves[1].vendor_id, slaves[1].product_code, 0x6041, 0x00, &Drive_data[0].off_status_word},
    {0, slaves[1].position, slaves[1].vendor_id, slaves[1].product_code, 0x6064, 0x00, &Drive_data[0].off_actual_position},
    {0, slaves[1].position, slaves[1].vendor_id, slaves[1].product_code, 0x606c, 0x00, &Drive_data[0].off_actual_velocity},
    {0, slaves[1].position, slaves[1].vendor_id, slaves[1].product_code, 0x6077, 0x00, &Drive_data[0].off_actual_torque},
    {0, slaves[1].position, slaves[1].vendor_id, slaves[1].product_code, 0x2600, 0x00, &Drive_data[0].off_digital_inputs},
    {0, slaves[1].position, slaves[1].vendor_id, slaves[1].product_code, 0x6061, 0x00, &Drive_data[0].off_mode_display},
    {0, slaves[1].position, slaves[1].vendor_id, slaves[1].product_code, 0x6040, 0x00, &Drive_data[0].off_control_word},
    {0, slaves[1].position, slaves[1].vendor_id, slaves[1].product_code, 0x607a, 0x00, &Drive_data[0].off_target_position},
    {0, slaves[1].position, slaves[1].vendor_id, slaves[1].product_code, 0x60ff, 0x00, &Drive_data[0].off_target_velocity},
    {0, slaves[1].position, slaves[1].vendor_id, slaves[1].product_code, 0x6071, 0x00, &Drive_data[0].off_target_torque},
    {0, slaves[1].position, slaves[1].vendor_id, slaves[1].product_code, 0x6060, 0x00, &Drive_data[0].off_mode_operation},

    //--------------Slave 2 (drive, alias 0, position 2)--------------------
    {0, slaves[2].position, slaves[2].vendor_id, slaves[2].product_code, 0x6041, 0x00, &Drive_data[1].off_status_word},
    {0, slaves[2].position, slaves[2].vendor_id, slaves[2].product_code, 0x6064, 0x00, &Drive_data[1].off_actual_position},
    {0, slaves[2].position, slaves[2].vendor_id, slaves[2].product_code, 0x606c, 0x00, &Drive_data[1].off_actual_velocity},
    {0, slaves[2].position, slaves[2].vendor_id, slaves[2].product_code, 0x6077, 0x00, &Drive_data[1].off_actual_torque},
    {0, slaves[2].position, slaves[2].vendor_id, slaves[2].product_code, 0x2600, 0x00, &Drive_data[1].off_digital_inputs},
    {0, slaves[2].position, slaves[2].vendor_id, slaves[2].product_code, 0x6061, 0x00, &Drive_data[1].off_mode_display},
    {0, slaves[2].position, slaves[2].vendor_id, slaves[2].product_code, 0x6040, 0x00, &Drive_data[1].off_control_word},
    {0, slaves[2].position, slaves[2].vendor_id, slaves[2].product_code, 0x607a, 0x00, &Drive_data[1].off_target_position},
    {0, slaves[2].position, slaves[2].vendor_id, slaves[2].product_code, 0x60ff, 0x00, &Drive_data[1].off_target_velocity},
    {0, slaves[2].position, slaves[2].vendor_id, slaves[2].product_code, 0x6071, 0x00, &Drive_data[1].off_target_torque},
    {0, slaves[2].position, slaves[2].vendor_id, slaves[2].product_code, 0x6060, 0x00, &Drive_data[1].off_mode_operation},

    //--------------Slave 3 (drive, alias 0, position 3)--------------------
    {0, slaves[3].position, slaves[3].vendor_id, slaves[3].product_code, 0x6041, 0x00, &Drive_data[2].off_status_word},
    {0, slaves[3].position, slaves[3].vendor_id, slaves[3].product_code, 0x6064, 0x00, &Drive_data[2].off_actual_position},
    {0, slaves[3].position, slaves[3].vendor_id, slaves[3].product_code, 0x606c, 0x00, &Drive_data[2].off_actual_velocity},
    {0, slaves[3].position, slaves[3].vendor_id, slaves[3].product_code, 0x6077, 0x00, &Drive_data[2].off_actual_torque},
    {0, slaves[3].position, slaves[3].vendor_id, slaves[3].product_code, 0x2600, 0x00, &Drive_data[2].off_digital_inputs},
    {0, slaves[3].position, slaves[3].vendor_id, slaves[3].product_code, 0x6061, 0x00, &Drive_data[2].off_mode_display},
    {0, slaves[3].position, slaves[3].vendor_id, slaves[3].product_code, 0x6040, 0x00, &Drive_data[2].off_control_word},
    {0, slaves[3].position, slaves[3].vendor_id, slaves[3].product_code, 0x607a, 0x00, &Drive_data[2].off_target_position},
    {0, slaves[3].position, slaves[3].vendor_id, slaves[3].product_code, 0x60ff, 0x00, &Drive_data[2].off_target_velocity},
    {0, slaves[3].position, slaves[3].vendor_id, slaves[3].product_code, 0x6071, 0x00, &Drive_data[2].off_target_torque},
    {0, slaves[3].position, slaves[3].vendor_id, slaves[3].product_code, 0x6060, 0x00, &Drive_data[2].off_mode_operation},

    //--------------Slave 4 (drive, alias 0, position 4)--------------------
    {0, slaves[3].position, slaves[4].vendor_id, slaves[4].product_code, 0x6041, 0x00, &Drive_data[3].off_status_word},
    {0, slaves[3].position, slaves[4].vendor_id, slaves[4].product_code, 0x6064, 0x00, &Drive_data[3].off_actual_position},
    {0, slaves[3].position, slaves[4].vendor_id, slaves[4].product_code, 0x606c, 0x00, &Drive_data[3].off_actual_velocity},
    {0, slaves[3].position, slaves[4].vendor_id, slaves[4].product_code, 0x6077, 0x00, &Drive_data[3].off_actual_torque},
    {0, slaves[3].position, slaves[4].vendor_id, slaves[4].product_code, 0x2600, 0x00, &Drive_data[3].off_digital_inputs},
    {0, slaves[3].position, slaves[4].vendor_id, slaves[4].product_code, 0x6061, 0x00, &Drive_data[3].off_mode_display},
    {0, slaves[3].position, slaves[4].vendor_id, slaves[4].product_code, 0x6040, 0x00, &Drive_data[3].off_control_word},
    {0, slaves[3].position, slaves[4].vendor_id, slaves[4].product_code, 0x607a, 0x00, &Drive_data[3].off_target_position},
    {0, slaves[3].position, slaves[4].vendor_id, slaves[4].product_code, 0x60ff, 0x00, &Drive_data[3].off_target_velocity},
    {0, slaves[3].position, slaves[4].vendor_id, slaves[4].product_code, 0x6071, 0x00, &Drive_data[3].off_target_torque},
    {0, slaves[3].position, slaves[4].vendor_id, slaves[4].product_code, 0x6060, 0x00, &Drive_data[3].off_mode_operation},

    {} //terminator
};

//=====================================================================
//Signal handler - ensures clean shutdown on Ctrl+c
//=====================================================================
void signal_handler(int sig)
{
    (void)sig;
    running = false;
}

// ============================================================
// State check functions
// ============================================================
void check_master_state() {
    ec_master_state_t ms;
    ecrt_master_state(master, &ms);

    if (ms.slaves_responding != master_state.slaves_responding) {
        printf("%u slave(s) responding.\n", ms.slaves_responding);
    }
    if (ms.al_states != master_state.al_states) {
        printf("AL states: 0x%02X.\n", ms.al_states);
    }
    if (ms.link_up != master_state.link_up) {
        printf("Link is %s.\n", ms.link_up ? "up" : "down");
    }

    master_state = ms;
}

void check_domain_state() {
    ec_domain_state_t ds;
    ecrt_domain_state(domain, &ds);

    if (ds.working_counter != domain_state.working_counter) {
        printf("Domain: WC %u.\n", ds.working_counter);
    }
    if (ds.wc_state != domain_state.wc_state) {
        printf("Domain: State %u.\n", ds.wc_state);
    }

    domain_state = ds;
}

void check_slave0_state() {
    ec_slave_config_state_t s;
    ecrt_slave_config_state(sc0, &s);

    if (s.al_state != sc0_state.al_state) {
        printf("Slave 0: state 0x%02X.\n", s.al_state);
    }
    if (s.online != sc0_state.online) {
        printf("Slave 0: %s.\n", s.online ? "Online" : "Offline");
    }
    if (s.operational != sc0_state.operational) {
        printf("Slave 0: %soperational.\n", s.operational ? "" : "Not ");
    }

    sc0_state = s;
}

void check_drive_state(int i) {
    ec_slave_config_state_t sc;
    ecrt_slave_config_state(Drive_data[i].sc_slave_config, &sc);

    if (sc.al_state != Drive_data[i].sc_slave_config_state.al_state) {
        printf("Slave %d: state 0x%02X.\n", i + 1, sc.al_state);
    }
    if (sc.online != Drive_data[i].sc_slave_config_state.online) {
        printf("Slave %d: %s.\n", i + 1, sc.online ? "Online" : "Offline");
    }
    if (sc.operational != Drive_data[i].sc_slave_config_state.operational) {
        printf("Slave %d: %soperational.\n", i + 1, sc.operational ? "" : "Not ");
    }

    Drive_data[i].sc_slave_config_state = sc;
}


// ============================================================
// Warm-up: run cycles to let DC lock before main loop
// ============================================================
void warmup_dc_sync() {
    std::cout << "Warming up DC sync (" << WARMUP_CYCLES << " cycles)..." << std::endl;

    struct timespec warmup_time;
    clock_gettime(CLOCK_MONOTONIC, &warmup_time);

    for (int i = 0; i < WARMUP_CYCLES && running; i++) {
        warmup_time.tv_nsec += CYCLE_TIME_NS;
        while (warmup_time.tv_nsec >= 1000000000) {
            warmup_time.tv_nsec -= 1000000000;
            warmup_time.tv_sec++;
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &warmup_time, NULL);

        ecrt_master_receive(master);
        ecrt_domain_process(domain);

        // DC sync
        struct timespec tn;
        clock_gettime(CLOCK_MONOTONIC, &tn);
        ecrt_master_application_time(master,
            tn.tv_sec * 1000000000ULL + tn.tv_nsec);
        ecrt_master_sync_reference_clock(master);
        ecrt_master_sync_slave_clocks(master);

        ecrt_domain_queue(domain);
        ecrt_master_send(master);
    }

    // Print state after warm-up
    check_master_state();
    check_domain_state();
    check_slave0_state();
    for (int i = 0; i < NUM_OF_DRIVES; i++) {
        check_drive_state(i);
    }
    std::cout << "Warm-up complete." << std::endl;
}



