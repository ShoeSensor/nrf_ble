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

#ifndef BLECONNECTION_H
#define	BLECONNECTION_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "device_manager.h"

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Initialize ble GAP params. As the name is the only important in this case,
 * everything else is abstracted
 * @param deviceName Name the device advertises with
 * @return NRF_SUCCES if success, NRF_ERROR* if fail
 */
uint32_t conn_gapParamsInit(const char* deviceName);

/**
 * Initialize ble connection parameters. All parameters are abstracted except for
 * the parameter event handler.
 * @param paramsCallback Handle the connection parameter module. This is called
 * when the parameters are applied successfully or failed
 * @return NRF_SUCCES if success, NRF_ERROR* if fail
 */
uint32_t conn_paramsInit(ble_conn_params_evt_handler_t paramsCallback);

/**
 * Initialize device manager. All parameters are abstracted. To change the
 * values, see ble_config.h
 * @param appHandle Application identifier allocated by device manager
 * @param doEraseBonds Indicated if the device manager should delete bonding
 * information from the persistent memory during initialization.
 * @return NRF_SUCCES if success, NRF_ERROR* if fail
 */
uint32_t conn_deviceManagerInit(dm_application_instance_t appHandle,
		bool doEraseBonds);

/**
 * Initialize BLE advertising module
 * @param uuids	UUID's to advertise
 * @param advCallback Callback to handle the adversing events
 * @return NRF_SUCCES on success, error code otherwise
 */
uint32_t conn_advertisingInit(ble_uuid_t uuids[], ble_adv_evt_t advCallback);


/**
 * Start advertising
 * @param advMode Mode to advertise in
 * @return NRF_SUCCES if success, error code if error
 */
uint32_t conn_advertisingStart(ble_adv_mode_t advMode);

#ifdef	__cplusplus
}
#endif

#endif	/* BLECONNECTION_H */
