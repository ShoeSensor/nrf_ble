# nrf_ble
Bluetooth smart abstraction layer for the NRF51

###Build instructions

This project can either be build be eclipse of from the commandline. The filepaths depends on the environment variable "SDK_ROOT" set.

#### Eclipse
To setup eclipse for ARM embedded development, follow this [tutorial](https://devzone.nordicsemi.com/tutorials/7/).

To build with eclipse:
**File->New->Makefile project with Existing Code**

Select the root directory of this repository and you are done.

Go to **Project->Properties->C/C++ Build->Environment** and add the variable SDK_ROOT and set it to the root directory of your SDK and you are good to go.

To allow debugging, Go to **Project->Properties->C/C++ Build** and uncheck "Use default build command". Now change it to: *make DEBUG=1*

#### Commandline
To build from the commandline, clone this repository into your SDK location. Execute *source $(SDKROOT)/envsetup.sh*. Now cd into the location of the repository and type *mka*. To flash the device type *make flash*.
