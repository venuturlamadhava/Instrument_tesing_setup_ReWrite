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
static unsigned int off_slave0_plant_code_out;
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
static unsigned int off_instrument_type_in;
static unsigned int off_usage_limit_in;
static unsigned int off_slave0_instrument_version_in;
static unsigned int off_slave0_mfg_country_code_in;
static unsigned int off_slave0_plant_code_in;
static unsigned int off_slave0_mfg_date_in;
static unsigned int off_slave0_mfg_month_in;
static unsigned int off_slave0_mfg_year_in;
static unsigned int off_slave0_qc_status_in;
static unsigned int off_slave0_ysed_hsptl_code_in;
static unsigned int off_slave0_number_of_useage_in;
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



}