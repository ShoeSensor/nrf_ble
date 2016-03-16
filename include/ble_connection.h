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

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Initialize ble GAP params. As the name is the only important in this case,
 * everything else is abstracted
 * @param deviceName Name the device advertises with
 */
void conn_gapParamsInit(const char* deviceName);

/**
 * Initialize ble connection parameters. All parameters are abstracted exept for
 * the parameter event handler.
 * @param paramsCallback Handle the connection parameter module. This is called
 * when the parameters are applied successfully or failed
 */
void conn_paramsInit(ble_conn_params_evt_handler_t paramsCallback);

/**
 * Initialize device manager. All parameters are abstracted. To change the
 * values, see ble_config.h
 * @param doEraseBonds Indicated if the device manager should delete bonding
 * information from the persistent memory during initialization.
 */
void conn_deviceManagerInit(bool doEraseBonds);

#ifdef	__cplusplus
}
#endif

#endif	/* BLECONNECTION_H */
