
#ifndef SYMPHONYLINK_H
#define SYMPHONYLINK_H

#include "ll_ifc_consts.h"
#include "ll_ifc_symphony.h"

enum DownlinkMode
{   
    OFF = 0,
    ON ,
    MAILBOX 
};

class SymphonyLink {
    
    public:
        SymphonyLink(int32_t (*fnTx)(uint8_t*, uint16_t), int32_t (*fnRx)(uint8_t *, uint16_t));
        bool begin(uint32_t net_token, uint8_t* app_token, DownlinkMode dl_mode, uint8_t qos);
        bool write(uint8_t* buf, uint16_t len);
        bool read (uint8_t* buf, uint8_t* len);
        bool updateStatus();
        enum ll_rx_state getRxState();
        enum ll_tx_state getTxState();
        enum ll_state getModState();
        uint32_t getISR();
        volatile bool connected;
    
    private:    
        uint32_t _net_token;
        uint8_t _app_token[APP_TOKEN_LEN];
        ll_downlink_mode _downlink_mode;
        uint8_t _qos;
        ll_rx_state _rxState;
        ll_tx_state _txState;
        ll_state _modState;
        uint32_t _IRQ;
        int32_t (*transport_write)(uint8_t *buff, uint16_t len);
        int32_t(*transport_read)(uint8_t *buff, uint16_t len);
 
        
};



#endif // SYMPHONYLINK_H