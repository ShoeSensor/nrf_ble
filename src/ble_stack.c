/*
 * Copyright 2016 Bart Monhemius.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ble_stack.h"

#include <string.h>

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "app_timer.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_stack_handler_types.h"
#include "softdevice_handler.h"
#include "boards.h"

uint32_t ble_stackInit(softdevice_evt_schedule_func_t schedCallback,
        STACK_OSC_SOURCE oscSource)
{
    uint32_t errCode;
    uint32_t err_code;
    ble_enable_params_t bleParams;
    memset(&bleParams, 0, sizeof(bleParams));

    //APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, schedCallback);

    bleParams.gatts_enable_params.service_changed = SRVC_CHANGED_CHAR_PRESENT;
    errCode = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
            PERIPHERAL_LINK_COUNT, &bleParams);

    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);

    errCode = softdevice_enable(&bleParams);

    return errCode;
}

uint32_t ble_stackConfig(sys_evt_handler_t sysEventHandler,
        ble_evt_handler_t bleEventHandler)
{
    uint32_t errCode;

    errCode = softdevice_sys_evt_handler_set(sysEventHandler);
//    APP_ERROR_CHECK(errCode);

    errCode = softdevice_ble_evt_handler_set(bleEventHandler);
//    APP_ERROR_CHECK(errCode);
    return errCode;
}
