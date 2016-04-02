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

#include "ble_acc_service.h"

#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "ble.h"
#include "app_util.h"
#include "app_error.h"

struct accSrvHandle {
    uint16_t serviceHandle;
    uint16_t connHandle;
    ble_srv_cccd_security_mode_t accCharAttr;
    ble_gap_conn_sec_mode_t accReportReadPerm;
    ble_gatts_char_handles_t charXHandle;
    ble_gatts_char_handles_t charYHandle;
    drv_accelHandle_t accelHandle;
    uint8_t uuidType;
};

static void onConnect(ble_accSrvHandle_t handle, ble_evt_t *bleEvent)
{
    handle->connHandle = bleEvent->evt.gap_evt.conn_handle;
    if(handle->accelHandle != NULL)
        drv_accelEnable(handle->accelHandle);
}

static void onDisconnect(ble_accSrvHandle_t handle, ble_evt_t *bleEvent)
{
    handle->connHandle = BLE_CONN_HANDLE_INVALID;
    if(handle->accelHandle != NULL)
        drv_accelDisable(handle->accelHandle);
}

static void onWrite(ble_accSrvHandle_t handle, ble_evt_t *bleEvent)
{
    // No write for noew
}

static uint32_t addXChar(ble_accSrvHandle_t handle) {
    uint32_t errCode;
    ble_uuid_t accCharUuid;

    ble_gatts_char_md_t gattChar;
    ble_gatts_attr_md_t gattAttr;
    ble_gatts_attr_t gattAttrVal;
    ble_gatts_attr_md_t cccdAttr;

    memset(&cccdAttr, 0, sizeof(cccdAttr));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccdAttr.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccdAttr.write_perm);
    cccdAttr.vloc = BLE_GATTS_VLOC_STACK;

    memset(&gattChar, 0, sizeof(gattChar));
    gattChar.char_props.read = 1;
    gattChar.char_props.notify = 0;
    gattChar.p_char_user_desc = NULL;
    gattChar.p_char_user_desc = NULL;
    gattChar.p_char_pf = NULL;
    gattChar.p_cccd_md = &cccdAttr;
    gattChar.p_sccd_md = NULL;

    memset(&gattAttr, 0, sizeof(gattAttr));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&gattAttr.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&gattAttr.write_perm);
    gattAttr.vloc = BLE_GATTS_VLOC_STACK;
    gattAttr.rd_auth = 0;
    gattAttr.wr_auth = 0;
    gattAttr.vlen = 0;

    accCharUuid.type = handle->uuidType;
    accCharUuid.uuid = ACC_UUID_X_CHAR;

    memset(&gattAttrVal, 0, sizeof(gattAttrVal));
    gattAttrVal.p_uuid = &accCharUuid;
    gattAttrVal.p_attr_md = &gattAttr;
    gattAttrVal.init_len = sizeof(uint8_t);
    gattAttrVal.max_len = sizeof(uint8_t);
    gattAttrVal.init_offs = 0;
    gattAttrVal.p_value = NULL;

    errCode = sd_ble_gatts_characteristic_add(handle->serviceHandle,
            &gattChar,
            &gattAttrVal,
            &handle->charXHandle);
    APP_ERROR_CHECK(errCode);
    return errCode;
}

static uint32_t addYChar(ble_accSrvHandle_t handle)
{
    uint32_t errCode;
    ble_uuid_t accCharUuid;

    ble_gatts_char_md_t gattChar;
    ble_gatts_attr_md_t gattAttr;
    ble_gatts_attr_t gattAttrVal;
    ble_gatts_attr_md_t cccdAttr;

    memset(&cccdAttr, 0, sizeof(cccdAttr));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccdAttr.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccdAttr.write_perm);
    cccdAttr.vloc = BLE_GATTS_VLOC_STACK;

    memset(&gattChar, 0, sizeof(gattChar));
    gattChar.char_props.read = 1;
    gattChar.char_props.notify = 0;
    gattChar.p_char_user_desc = NULL;
    gattChar.p_char_user_desc = NULL;
    gattChar.p_char_pf = NULL;
    gattChar.p_cccd_md = &cccdAttr;
    gattChar.p_sccd_md = NULL;

    memset(&gattAttr, 0, sizeof(gattAttr));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&gattAttr.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&gattAttr.write_perm);
    gattAttr.vloc = BLE_GATTS_VLOC_STACK;
    gattAttr.rd_auth = 0;
    gattAttr.wr_auth = 0;
    gattAttr.vlen = 0;

    accCharUuid.type = handle->uuidType;
    accCharUuid.uuid = ACC_UUID_Y_CHAR;

    memset(&gattAttrVal, 0, sizeof(gattAttrVal));
    gattAttrVal.p_uuid = &accCharUuid;
    gattAttrVal.p_attr_md = &gattAttr;
    gattAttrVal.init_len = sizeof(uint8_t);
    gattAttrVal.max_len = sizeof(uint8_t);
    gattAttrVal.init_offs = 0;
    gattAttrVal.p_value = NULL;

    errCode = sd_ble_gatts_characteristic_add(handle->serviceHandle,
            &gattChar,
            &gattAttrVal,
            &handle->charYHandle);
    APP_ERROR_CHECK(errCode);
    return errCode;
}

ble_accSrvHandle_t ble_accSrvInit(ble_accSrvConfig_t *conf)
{
    uint32_t errCode;

    ble_accSrvHandle_t handle;
    ble_uuid_t accSrvUuid;
    ble_uuid128_t baseuuid = {ACC_UUID_BASE};

    // Add services
    handle = calloc(1, sizeof(struct accSrvHandle));
    handle->accelHandle = conf->accelHandle;

    errCode = sd_ble_uuid_vs_add(&baseuuid, &handle->uuidType);
    APP_ERROR_CHECK(errCode);

    accSrvUuid.type = handle->uuidType;
    accSrvUuid.uuid = ACC_UUID_SERVICE;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&handle->accReportReadPerm);

    errCode = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
            &accSrvUuid, &handle->serviceHandle);
    APP_ERROR_CHECK(errCode);

    // Add characteristic
    addYChar(handle);
    addXChar(handle);

    return handle;
}

void ble_accSrvBleHandleEvent(ble_accSrvHandle_t handle, ble_evt_t *bleEvent)
{
    switch(bleEvent->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            onConnect(handle, bleEvent);
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            onDisconnect(handle, bleEvent);
            break;
        case BLE_GATTS_EVT_WRITE:
            onWrite(handle, bleEvent);
            break;
    }
}

uint32_t ble_accSrvUpdate(ble_accSrvHandle_t handle, drv_accelData_t *accData)
{
    uint32_t errCode;
    ble_gatts_value_t gattXVal = {
        .len = 1,
        .offset = 0,
        .p_value = &accData->x
    };

    ble_gatts_value_t gattYVal = {
        .len = 1,
        .offset = 0,
        .p_value = &accData->y
    };

    errCode = sd_ble_gatts_value_set(handle->connHandle,
            handle->charXHandle.value_handle,
            &gattXVal);

    if(errCode != NRF_SUCCESS)
        return errCode;

    errCode = sd_ble_gatts_value_set(handle->connHandle,
        handle->charYHandle.value_handle,
        &gattYVal);

    if(errCode != NRF_SUCCESS)
        return errCode;

    return NRF_SUCCESS;
}

uint8_t ble_accSrvGetUuidType(ble_accSrvHandle_t handle)
{
    return handle->uuidType;
}
