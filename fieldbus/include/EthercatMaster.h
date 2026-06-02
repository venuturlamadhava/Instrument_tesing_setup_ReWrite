#pragma once
#include "ecrt.h"

static const int TOTAL_NUM_OF_PDOS=11;

extern ec_master_t* master;
extern ec_master_state_t master_state;

extern ec_domain_t* domain;
extern ec_domain_state_t domain_state;

extern ec_slave_config_t* sc0;
extern ec_slave_config_state_t sc0_state;

extern ec_pdo_entry_info_t slave0_pdo_entry[];
extern ec_pdo_info_t slave0_pdo[];
extern ec_sync_info_t slave0_syncs[];

extern ec_pdo_info_t drive_pdo_entry[];
extern ec_pdo_info_t drive_pdo[];
extern ec_sync_info_t drive_sync[];