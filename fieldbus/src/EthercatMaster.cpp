#include<iostream>
#include "EthercatMaster.h"

//Master & Domain - defined here
ec_master_t* master=nullptr;
ec_master_state_t master_state={};

ec_domain_t* domain=nullptr;
ec_domain_state_t domain_state={};

ec_slave_config_t* sc0=nullptr;
ec_slave_config_state_t sc0_state={};

//-------slave0-------

ec_pdo_entry_info_t slave0_pdo_entry[]={
    {0x7000, 0x01, 8},
    {0x7000, 0x02, 8},
    {0x7000, 0x03, 8},
    {0x7000, 0x04, 8},
    {0x7000, 0x05, 16},
    {0x7000, 0x06, 8},
    {0x7000, 0x07, 8},
    {0x7000, 0x08, 8},
    {0x7000, 0x09, 8},
    {0x7000, 0x10, 8},
    {0x7000, 0x11, 8},
    {0x7000, 0x12, 16},
    {0x7000, 0x13, 8},
    {0x7000, 0x14, 8},
    {0x7000, 0x15, 8},
    {0x7000, 0x16, 8},
    {0x7000, 0x17, 8},
    {0x6000, 0x01, 8},
    {0x6000, 0x02, 8},
    {0x6000, 0x03, 8},
    {0x6000, 0x04, 8},
    {0x6000, 0x05, 16},
    {0x6000, 0x06, 8},
    {0x6000, 0x07, 8},
    {0x6000, 0x08, 8},
    {0x6000, 0x09, 8},
    {0x6000, 0x10, 8},
    {0x6000, 0x11, 8},
    {0x6000, 0x12, 16},
    {0x6000, 0x13, 8},
    {0x6000, 0x14, 8},
    {0x6000, 0x15, 8},
    {0x6000, 0x16, 8},
    {0x6000, 0x17, 8}
};

ec_pdo_info_t slave0_pdo[]={
    {0x1600,17,slave0_pdo_entry},
    {0x1A00,17,slave0_pdo_entry+17}
};

ec_sync_info_t slave0_syncs[]=
{
    {0, EC_DIR_OUTPUT,0,NULL,EC_WD_DISABLE},
    {1, EC_DIR_INPUT,0,NULL,EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT,1,slave0_pdo,EC_WD_ENABLE},
    {3, EC_DIR_INPUT,1,slave0_pdo+1, EC_WD_DISABLE},
    {0xff}
};

ec_pdo_entry_info_t drive_pdo_entry[TOTAL_NUM_OF_PDOS]=
{

    //RxPDO
    {0x6040, 0x00, 16}, //control word}
    {}
};
