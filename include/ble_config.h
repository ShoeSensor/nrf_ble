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

#ifndef BLECONFIG_H
#define BLECONFIG_H

#include "app_timer.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define SRVC_CHANGED_CHAR_PRESENT           1                                   /**< Include the Service Changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device. */

#define DEVICE_NAME                         "ShoeSensor"                        /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME                   "NordicSemiconductor"               /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                    300                                 /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS          180                                 /**< The advertising time-out in units of seconds. */

#define APP_TIMER_PRESCALER                 0                                   /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE             4                                   /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL                   MSEC_TO_UNITS(10, UNIT_1_25_MS)    /**< Minimum acceptable connection interval (10 ms). */
#define MAX_CONN_INTERVAL                   MSEC_TO_UNITS(50, UNIT_1_25_MS)    /**< Maximum acceptable connection interval (50 ms). */
#define SLAVE_LATENCY                       0                                   /**< Slave latency. */
#define CONN_SUP_TIMEOUT                    MSEC_TO_UNITS(4000, UNIT_10_MS)     /**< Connection supervisory time-out (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY      5000                                /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY       30000                               /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT        3                                   /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                      1                                   /**< Perform bonding. */
#define SEC_PARAM_MITM                      0                                   /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES           BLE_GAP_IO_CAPS_NONE                /**< No I/O capabilities. */
#define SEC_PARAM_OOB                       0                                   /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE              7                                   /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE              16                                  /**< Maximum encryption key size. */

#define DEAD_BEEF                           0xDEADBEEF                          /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define OSTIMER_WAIT_FOR_QUEUE              2                                   /**< Number of ticks to wait for the timer queue to be ready */

#define ACCEL_BUF_SIZE                      50

#ifdef  __cplusplus
}
#endif

#endif  /* BLECONFIG_H */
