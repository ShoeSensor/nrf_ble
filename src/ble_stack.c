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

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_stack_handler_types.h"
#include "softdevice_handler.h"

uint32_t stack_init(softdevice_evt_schedule_func_t schedCallback,
        STACK_OSC_SOURCE oscSource)
{
    uint32_t errCode;
    ble_enable_params_t bleParams;
    memset(&bleParams, 0, sizeof(bleParams));

    switch(oscSource) {
        case STACK_OSC_INTERNAL:
            SOFTDEVICE_HANDLER_INIT(
                    NRF_CLOCK_LFCLKSRC_RC_250_PPM_TEMP_4000MS_CALIBRATION,
                    schedCallback);
            break;
        case STACK_OSC_EXTERNAL:
            SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM,
                    schedCallback);
            break;
    }

    bleParams.gatts_enable_params.service_changed = SRVC_CHANGED_CHAR_PRESENT;
    errCode = sd_ble_enable(&bleParams);
    APP_ERROR_CHECK(errCode);
    return errCode;
}

uint32_t stack_config(sys_evt_handler_t sysEventHandler,
        ble_evt_handler_t bleEventHandler)
{
    uint32_t errCode;

    errCode = softdevice_sys_evt_handler_set(sysEventHandler);
    APP_ERROR_CHECK(errCode);

    errCode = softdevice_ble_evt_handler_set(bleEventHandler);
    APP_ERROR_CHECK(errCode);
    return errCode;
}
