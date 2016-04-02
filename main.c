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
#include "ble.h"
#include "ble_acc_service.h"
#include "ble_connection.h"
#include "ble_dis.h"
#include "ble_gap.h"
#include "ble_hci.h"
#include "ble_stack.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_mma8453q.h"
#include "os_semaphore.h"
#include "os_thread.h"
#include "os_timer.h"
#include "pstorage.h"
#include "softdevice_handler.h"

static uint16_t connHanle = BLE_CONN_HANDLE_INVALID;
static ble_accSrvHandle_t accSrvHandle;
static dm_application_instance_t appHandle;
static ble_uuid_t advUuids[] = {
    { BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE }
};
static os_timerHandle_t accelSrvTimer;
static os_semHandle_t bleEventReady;
static os_threadHandle_t mainThreadHandle;

// Softdevice error handler
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

static void accSrVTimeout(void *args)
{
    drv_accelData_t accelData;
    accelData.x = rand() % 255;
    accelData.y = rand() % 255;
    accelData.z = 0;
    ble_accSrvUpdate(accSrvHandle, &accelData);
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timersInit(void)
{
    // Initialize timer module.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    os_timerConfig_t timerConf = {
            .name = "ACS",
            .period = 500,
            .oneShot = false,
            .callback = accSrVTimeout
    };
    accelSrvTimer = os_timerTaskNew(&timerConf, OSTIMER_WAIT_FOR_QUEUE);

    /* Error checking */
    if (accelSrvTimer == NULL)
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
}

/**@brief Function for starting application timers.
 */
static void timerTasksStart(void)
{
    os_timerTaskStart(accelSrvTimer);
}

/**@brief Function for initializing services that will be used by the application.
 *
 * @details Initialize the Heart Rate, Battery and Device Information services.
 */
static void servicesInit(void)
{
    uint32_t errCode;
    ble_dis_init_t dis_init;

    // Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str,
            (char *) MANUFACTURER_NAME);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

    errCode = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(errCode);

    ble_accSrvConfig_t accSrvConf = { .accelHandle = NULL };
    accSrvHandle = ble_accSrvInit(&accSrvConf);
}

/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in]   p_evt   Event received from the Connection Parameters Module.
 */
static void onConnParamsEvt(ble_conn_params_evt_t * p_evt)
{
    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED) {
        uint32_t errCode = sd_ble_gap_disconnect(connHanle,
                BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(errCode);
    }
}

static void onAdvEvent(ble_adv_evt_t ble_adv_evt)
{
}

/**@brief Function for receiving the Application's BLE Stack events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void onBlEevent(ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            connHanle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            connHanle = BLE_CONN_HANDLE_INVALID;
            break;
        default:
            break;
    }
}

/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void bleEventDispatch(ble_evt_t * p_ble_evt)
{
    dm_ble_evt_handler(p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_accSrvBleHandleEvent(accSrvHandle, p_ble_evt);
    onBlEevent(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
}

/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in]   sys_evt   System stack event.
 */
static void sysEventDispatch(uint32_t sys_evt)
{
    pstorage_sys_event_handler(sys_evt);
    ble_advertising_on_sys_evt(sys_evt);
}

/**
 * @brief Event handler for new BLE events
 *
 * This function is called from the SoftDevice handler.
 * It is called from interrupt level.
 *
 * @return The returned value is checked in the softdevice_handler module,
 *         using the APP_ERROR_CHECK macro.
 */
static uint32_t bleNewEventHandler(void)
{
    os_semIsrPost(bleEventReady);
    return NRF_SUCCESS;
}

static void mainThread(void * arg)
{
    uint32_t errCode;
    UNUSED_PARAMETER(arg);

    // Initialize.
    timersInit();
    ble_stackInit(bleNewEventHandler, STACK_OSC_EXTERNAL);
    ble_stackConfig(sysEventDispatch, bleEventDispatch);
    conn_deviceManagerInit(&appHandle, false);
    conn_gapParamsInit(DEVICE_NAME);
    conn_advertisingInit(advUuids, 1, onAdvEvent);
    servicesInit();
    conn_paramsInit(onConnParamsEvt);

    timerTasksStart();
    errCode = conn_advertisingStart(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(errCode);

    while (1) {
        /* Wait for event from SoftDevice */
        while (os_semIsrWait(bleEventReady) == false);
        intern_softdevice_events_execute();
    }
}

/**@brief Function for application main entry.
 */
int main(void)
{
    os_semConfig_t semConf;
    semConf.binary = true;
    bleEventReady = os_semNew(&semConf);

    if (bleEventReady == NULL)
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);

    // Threads are broken
    os_threadConfig_t mainThreadConf = {
            .name = "MT",
            .threadCallback = &mainThread,
            .threadArgs = NULL,
            .stackSize = STACK_SIZE_BIG,
            .priority = THREAD_PRIO_NORM
    };
    mainThreadHandle = os_threadNew(&mainThreadConf);

    /* Activate deep sleep mode */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    // Start FreeRTOS scheduler.
    os_startScheduler();

    while (true) {
        APP_ERROR_HANDLER(NRF_ERROR_FORBIDDEN);
    }
}
