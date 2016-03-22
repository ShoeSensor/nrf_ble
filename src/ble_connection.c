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

#include "ble_connection.h"
#include "ble_config.h"

#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "boards.h"
#include "device_manager.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_error.h"
#include "pstorage.h"
#include "softdevice_handler.h"

static void paramsErrorHandler(uint32_t errCode)
{
    APP_ERROR_HANDLER(errCode);
}

static uint32_t devManErrorHandler(dm_handle_t handle, dm_event_t event,
        ret_code_t result)
{
    APP_ERROR_CHECK(result);
    return NRF_SUCCESS;
}

uint32_t conn_deviceManagerInit(dm_application_instance_t appHandle,
        bool doEraseBonds)
{
    uint32_t errCode;
    dm_application_param_t devManParams;
    dm_init_param_t initParam = {
            .clear_persistent_data = doEraseBonds
    };
    memset(&devManParams.sec_param, 0, sizeof(devManParams.sec_param));
    errCode = pstorage_init();
    APP_ERROR_CHECK(errCode);

    errCode = dm_init(&initParam);
    APP_ERROR_CHECK(errCode);

    devManParams.evt_handler = devManErrorHandler;
    devManParams.service_type = DM_PROTOCOL_CNTXT_GATT_SRVR_ID;
    devManParams.sec_param.bond = SEC_PARAM_BOND;
    devManParams.sec_param.mitm = SEC_PARAM_MITM;
    devManParams.sec_param.io_caps = SEC_PARAM_IO_CAPABILITIES;
    devManParams.sec_param.oob = SEC_PARAM_OOB;
    devManParams.sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    devManParams.sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
    errCode = dm_register(&appHandle, &devManParams);
    APP_ERROR_CHECK(errCode);
    return errCode;
}

uint32_t conn_gapParamsInit(const char* deviceName)
{
    uint32_t errCode;
    ble_gap_conn_params_t gapConnParams;
    ble_gap_conn_sec_mode_t sec_mode;

    memset(&gapConnParams, 0, sizeof(gapConnParams));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    errCode = sd_ble_gap_device_name_set(&sec_mode, deviceName,
            strlen(deviceName));
    APP_ERROR_CHECK(errCode);

    // Bluetooth SIG actually has a standard appearance  for this application
    errCode = sd_ble_gap_appearance_set(
            BLE_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE);
    APP_ERROR_CHECK(errCode);

    gapConnParams.min_conn_interval = MIN_CONN_INTERVAL;
    gapConnParams.max_conn_interval = MAX_CONN_INTERVAL;
    gapConnParams.slave_latency = SLAVE_LATENCY;
    gapConnParams.conn_sup_timeout = CONN_SUP_TIMEOUT;

    errCode = sd_ble_gap_ppcp_set(&gapConnParams);
    APP_ERROR_CHECK(errCode);
    return errCode;
}

uint32_t conn_paramsInit(ble_conn_params_evt_handler_t paramsCallback)
{
    uint32_t errCode;
    ble_conn_params_init_t connInit;

    memset(&connInit, 0, sizeof(connInit));

    connInit.p_conn_params = NULL;
    connInit.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    connInit.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    connInit.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    connInit.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    connInit.disconnect_on_fail = false;
    connInit.evt_handler = paramsCallback;
    connInit.error_handler = paramsErrorHandler;

    errCode = ble_conn_params_init(&connInit);
    APP_ERROR_CHECK(errCode);
    return errCode;
}

uint32_t conn_advertisingInit(ble_uuid_t uuids[], ble_adv_evt_t advCallback)
{
    uint32_t errCode;
    ble_advdata_t adverData;
    ble_adv_modes_config_t advModeConf = {0};

    memset(&adverData, 0, sizeof(adverData));

    adverData.name_type = BLE_ADVDATA_FULL_NAME;
    adverData.include_appearance = true;
    adverData.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE; /**< Only use BLE*/
    // UUID's
    adverData.uuids_complete.p_uuids = uuids;
    adverData.uuids_complete.uuid_cnt = ARRAY_SIZE(uuids);

    advModeConf.ble_adv_slow_enabled = BLE_ADV_SLOW_ENABLED;
    advModeConf.ble_adv_directed_slow_interval = APP_ADV_INTERVAL;
    advModeConf.ble_adv_directed_slow_timeout = APP_ADV_TIMEOUT_IN_SECONDS;

    errCode = ble_advertising_init(&adverData, NULL, &advModeConf, NULL, NULL);
    APP_ERROR_CHECK(errCode);
    return errCode;
}

uint32_t conn_advertisingStart(ble_adv_mode_t advMode)
{
    uint32_t errCode;
    errCode = ble_advertising_start(advMode);
    APP_ERROR_CHECK(errCode);
    return errCode;
}
