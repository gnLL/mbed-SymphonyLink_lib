#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#include "BufferedSerial.h"
#include "SymphonyLink.h"
#include "ll_ifc_transport_pc.h"


#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define NEW_HOST_IFC

#if defined(WIN32) || defined (__MINGW32__)
#define SLEEP(x)        Sleep(x * 1000)
#define MS_SLEEP(x)     Sleep(x)
#else
#define SLEEP(x)        sleep(x)
#define MS_SLEEP(x)     usleep(x * 1000)
#endif

#define DEFAULT_CHANNEL            (0)
#define DEFAULT_BANDWIDTH        (3)
#define SENTINEL                (255)

// Helper macros to convert little-endian byte stream to native types
#define _HOST_IFC_LE_U32(x,o,y) ((((uint32_t)(((uint8_t*)&x)[o+y]))&0xFF) << (8*(y)))
#define HOST_IFC_LE_U32(x,o) (_HOST_IFC_LE_U32(x,o,0) + _HOST_IFC_LE_U32(x,o,1) + _HOST_IFC_LE_U32(x,o,2) + _HOST_IFC_LE_U32(x,o,3))

#define _HOST_IFC_LE_I16(x,o,y) ((((int16_t)(((uint8_t*)&x)[o+y]))&0xFF) << (8*(y)))
#define HOST_IFC_LE_I16(x,o) (_HOST_IFC_LE_I16(x,o,0) + _HOST_IFC_LE_I16(x,o,1))

// Build Date
#define BUILD_DATE (__BUILD_DATE);

// Build Number
#define VERSION_MAJOR                (1) // 0-255
#define VERSION_MINOR                (0) // 0-255
#define VERSION_TAG     (__BUILD_NUMBER) // 0-65535



int main() {

    //configure these to match your network and application tokens
    //Step 1 - set desired network token
    uint32_t network_token = 0x4f50454e;        //open token

    //Step 2 - set desired application token
    uint8_t app_token[APP_TOKEN_LEN] = {0x61,0x04,0xce,0xd4,0x8d,0x49,0xa8,0xfb,0xcd,0x1e};
    
    uint8_t QOS = 15;
    uint32_t flags;
    uint8_t data[2] = {0,0} ;       //TX data buffer.  Can be made bigger
    uint8_t rxdata[256];            //RX data buffer.  The max message size is 256

    bool success;
    uint8_t len;
     
   
    
    SymphonyLink sym(transport_write,transport_read);
    
    DigitalOut reset(PA_10);
    DigitalOut boot(PA_9);
    DigitalIn IRQ(PA_5);

    //Perform device reset and wait for it to become active
    reset = 1;
    wait_ms(10);
    reset = 0;
    wait_ms(2000);
   

    //send configuration data to initial the device
    sym.begin(network_token, app_token, ON, 15);

    //Wait until the module is connected to the network. This could be part of the 
    //main loop if the module is going to be shutdown as part of the application
    while (sym.connected == false)
    {
        sym.updateStatus();
        
        wait_ms(1000);        //This could be shorter
    }
    
    //System is now connected.
    
    while (1)
    {
        //Get an updated status of the module
        if(sym.updateStatus())
        {
            //Check for connection.  If the module is not connected to the network
            //it will scan and keep trying
            if (sym.connected)
            {
                //Send simple counter
                data[0]++;
                
                //Write bytes to Conductor
                success =  sym.write(data, 2);
                if( success)
                {
                 
                }
                else
                {
                   
                }
            }
            
            //Try to read bytes.  IF nothing is available, then the device will return false
            success = sym.read(rxdata, &len);
            if (success)
            {
                //Print out data received.
               
            }
        }
        else
        {
           
            data[1]++;
        }
        wait_ms(100);
    }        
}
    
    
