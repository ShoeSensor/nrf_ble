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
 
#include <stdint.h>
#include <string.h>
#include "app_error.h"
#include "ble_config.h"
#include "ble_connection.h"
#include "ble_stack.h"
#include "os_timer.h"
#include "os_thread.h"
#include "os_mutex.h"
#include "pstorage.h"

//TODO improve error handling

static dm_application_instance_t devManHandle;
static os_threadHandle_t mainThreadHandle;
static os_mutexHandle_t mutex;

/**
 * Error callback for the softdevice
 * @param line_num		Line number where an error occurred
 * @param p_file_name	File where the error is
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

static uint32_t softDeviceHandler(void)
{
    os_mutexIsrUnLock(mutex);
    return 0;
}

static void bleEventHandler(ble_evt_t *bleEvent)
{
    dm_ble_evt_handler(bleEvent);
    ble_conn_params_on_ble_evt(bleEvent);
    ble_advertising_on_ble_evt(bleEvent);

}

static void sysEventHandler(uint32_t sysEvent)
{
    pstorage_sys_event_handler(sysEvent);
    ble_advertising_on_sys_evt(sysEvent);

}

//static void bleAdvHandler(ble_adv_evt_t bleAdvEvent)
//{
//
//}

static void connParamHandler(ble_conn_params_evt_t *connParamEvent)
{

}

static void mainThread(void *arg)
{
    while(1) {
        while(os_mutexLock(mutex));
        intern_softdevice_events_execute();
    }
}

int main(void)
{
    uint32_t errCode;
    os_threadConfig_t mainConfig = {
            .name = "main",
            .threadCallback = mainThread,
            .threadArgs = NULL,
            .stackSize = STACK_SIZE_DEFAULT,
            .priority = THREAD_PRIO_NORM
    };

    mainThreadHandle = os_threadNew(&mainConfig);
    mutex = os_mutexNew();

    ble_stackInit(softDeviceHandler, STACK_OSC_EXTERNAL);
    ble_stackConfig(sysEventHandler, bleEventHandler);
    conn_deviceManagerInit(&devManHandle, false);
    conn_gapParamsInit(DEVICE_NAME);
    conn_advertisingInit(NULL, 0, NULL);
    conn_paramsInit(connParamHandler);

    errCode = conn_advertisingStart(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(errCode);
    os_startScheduler();
    while(1);
}
