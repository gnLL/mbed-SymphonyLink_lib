
#include "SymphonyLink.h"
#include "ll_ifc_symphony.h"
#include <string.h>
#include <mbed.h>

//to print debugging info on module state
//index is meant to be ll_state enum
static char* ll_state_strings[] = {"Error 0", "Connected", "DisConnected", "Init"};


SymphonyLink::SymphonyLink(int32_t (*fnTx)(uint8_t*, uint16_t), int32_t (*fnRx)(uint8_t *, uint16_t))
{
    //Set values to zero or default states
    transport_write = fnTx;
    transport_read = fnRx;
    _net_token = 0;
    memset(_app_token, 0, APP_TOKEN_LEN);
    _downlink_mode = LL_DL_OFF;
    _qos = 0;
    connected = false;
}

bool SymphonyLink::begin(uint32_t net_token, uint8_t* app_token, DownlinkMode dl_mode, uint8_t qos)
{
    uint8_t i;
    uint32_t ret;
    
    
    //clear flags from reseting and connecting
    ll_irq_flags(IRQ_FLAGS_RESET,&_IRQ);
    ll_get_state(&_modState,&_txState,&_rxState);

    
    _net_token = net_token;

    for(i=0;i<APP_TOKEN_LEN; i++)
    {
        _app_token[i] = app_token[i];
    }

    switch (dl_mode)
    {
        case OFF:
            _downlink_mode = LL_DL_OFF;
            break;
            
        case ON:
            _downlink_mode = LL_DL_ALWAYS_ON;
            break;
            
        case MAILBOX:
            _downlink_mode = LL_DL_MAILBOX;
            break;
    }
    
    _qos = qos;
    
    connected = false;

    //configure module and start connection
    ret = ll_config_set(_net_token, _app_token, _downlink_mode, _qos);
    
    wait_ms(100);
    
    updateStatus();
    
    return ret;

}



bool SymphonyLink::write(uint8_t* buf, uint16_t len)
{
    int32_t ret = -1;
    uint32_t flags;
    
    //First make sure we are connected
    if (_modState == LL_STATE_IDLE_CONNECTED )
    {
        //Check to see if we are currently transmitting
        //Send packet if not
        if(_txState == LL_TX_STATE_TRANSMITTING)
        {
             wait_ms(100);
        }
        else
        {
             ret = ll_message_send_ack(buf,len);
        }
    }
    
    //get the IRQ flags
    ret = ll_irq_flags(IRQ_FLAGS_RESET,&flags);
    
    //Keep looping until transmitt is done.
    //ToDo: refactor this into a new function so the main loop doesn not need to spin //on it
    while ((flags & IRQ_FLAGS_TX_DONE) == 0)
    {
        wait_ms(100);
        ret = ll_irq_flags(0,&flags);
        
    }
    
    ll_irq_flags(IRQ_FLAGS_TX_DONE,&flags);
    
    if (ret<0)
    {
        return false;
    }
    else
    {
        return true;
    }
}


bool SymphonyLink::read(uint8_t* buf, uint8_t* len)
{
    int16_t rssi;
    uint8_t snr;
    int32_t ret;
    
    if (_rxState == LL_RX_STATE_RECEIVED_MSG)
    {
            ret = ll_retrieve_message(buf,len, &rssi, &snr);
            if (ret<0)
            {
                return false;
            }
            else
            {
                return true;
            }
    }
    else
    {
        return false;
    }
    
}

bool SymphonyLink::updateStatus(void)
{
    int32_t ret;
    uint32_t IRQ;
     
    ll_rx_state rxState;
    ll_tx_state txState;
    ll_state modState;
    
    //clear flags from reseting and connecting
    ret = ll_irq_flags(IRQ_FLAGS_RESET,&IRQ);
    
    if(ret<0)
    {
        return false;
    }
    else
    {
        ret = ll_get_state(&modState,&txState,&rxState);
        
        _modState = modState;
        _txState = txState;
        _rxState = rxState;
        
        if(_rxState == LL_RX_STATE_RECEIVED_MSG)
        {
            
        }
        
        if(ret<0)
        {
            return false;   
        }
        else
        {
            if(_modState == LL_STATE_IDLE_CONNECTED )
            {
                connected = true;
            }
            else
            {
                connected = false;
            }
            return true;    
        }
    } 
}



