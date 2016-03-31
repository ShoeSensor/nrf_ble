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

#ifndef BLETRANSFER_H
#define BLETRANSFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "nrf_mma8453q.h"

#include "ble.h"
#include "ble_types.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define ACC_UUID_BASE       {0x37, 0xA1, 0xCC, 0x00, 0xF7, 0x21, 0x11, 0xE5, \
                                0x8C, 0x65, 0x00, 0x02, 0xA5, 0xD5, 0xC5, 0x1B}
#define ACC_UUID_SERVICE    0x6726
#define ACC_UUID_DATA_CHAR  0x6727

typedef struct accSrvHandle *ble_accSrvHandle_t;

typedef struct {
    drv_accelHandle_t accelHandle;
} ble_accSrvConfig_t;

/**
 * @brief Initialize the accelerometer service.
 * @details Add the service and characteristics to the softdevice.
 *
 * @param conf Configuration for the service.
 * @return A new handle to the service.
 */
ble_accSrvHandle_t ble_accSrvInit(ble_accSrvConfig_t *conf);

/**
 * @brief Handle a BLE event in the service.
 * @details Handle the connect, disconnect and write events.
 *
 * @param handle Handle to the service.
 * @param bleEvent BLE event received from the softdevice.
 */
void ble_accSrvBleHandleEvent(ble_accSrvHandle_t handle, ble_evt_t *bleEvent);

/**
 * @brief Update the accelerometer values send to the client.
 *
 * @param handle Handle to the BLE service
 * @param accData New accelerometer data.
 *
 * @return NRF_SUCCES if succes, error code on fail.
 */
uint32_t ble_accSrvUpdate(ble_accSrvHandle_t handle, drv_accelData_t *accData);

/**
 * Get the UUID type of the service, defined in the softdevice.
 * @param handle Handle to the service.
 * @return UUID type.
 */
uint8_t ble_accSrvGetUuidType(ble_accSrvHandle_t handle);

#ifdef  __cplusplus
}
#endif

#endif  /* BLETRANSFER_H */
