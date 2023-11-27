#include "MicroBit.h"
//#include "ManagedString.h"
//#include "samples/Tests.h"
#include <string>


#define bit(n) (1 << n)
#define bit_set(var, mask) ((var) |= (mask))
#define bit_clear(var, mask) ((var) &= ~(mask))
#define bit_toggle(var, mask) ((var) ^= (mask))
#define bit_read(var, mask) ((var) & (mask))

//MicroBit uBit;

NRF52Pin srTx(ID_PIN_USBTX, MICROBIT_PIN_UART_TX, PIN_CAPABILITY_DIGITAL);
NRF52Pin srRx(ID_PIN_USBRX, MICROBIT_PIN_UART_RX, PIN_CAPABILITY_DIGITAL);
NRF52Serial serial(srTx, srRx, NRF_UARTE0);

// struct FB_S
//     {
//         uint8_t         length;                             // The length of the remaining bytes in the packet. includes protocol/version/group fields, excluding the length field itself.
//         uint8_t         version;                            // Protocol version code.
//         uint8_t         group;                              // ID of the group to which this packet belongs.
//         uint8_t         protocol;                           // Inner protocol number c.f. those issued by IANA for IP protocols

//         uint8_t         payload[MICROBIT_RADIO_MAX_PACKET_SIZE];    // User / higher layer protocol data
//         FrameBuffer     *next;                              // Linkage, to allow this and other protocols to queue packets pending processing.
//         int             rssi;                               // Received signal strength of this frame.
//     } typedef FrameBuffer;

FrameBuffer *rxBuf;
int *sample;
int interrupt =0;

extern "C" void my_RADIO_IRQHandler(void)
{
    interrupt += 1;
    // bit_set(NRF_GPIO->OUT, bit(2));
    // // immediately set off timer, hopefully improve determinism
    // NRF_TIMER1->TASKS_START=1;
    // // bit_clear(NRF_GPIO->OUT, bit(2));
    // if(NRF_RADIO->EVENTS_CRCOK)
    // {
    //     NRF_RADIO->EVENTS_CRCOK = 0;
    //     if(NRF_RADIO->CRCSTATUS == 1)
    //     {
    //         NRF_RADIO->TASKS_DISABLE = 1;
    //         *sample = (int)NRF_RADIO->RSSISAMPLE;

    //         // Now move on to the next buffer, if possible.
    //         // The queued packet will get the rssi value set above.

            
    //         // Set the new buffer for DMA
    //         //NRF_RADIO->PACKETPTR = (uint32_t) MicroBitRadio::instance->getRxBuf();
            
    //     }
    //     else
    //     {
            
    //     }

        
        
    // } else {
    //     // cancel timer
    //     NRF_TIMER1->TASKS_STOP = 1;
    //     NRF_TIMER1->TASKS_CLEAR = 1;
    // }

    // if(NRF_RADIO->EVENTS_TXREADY)
    // {
    //     NRF_RADIO->EVENTS_TXREADY = 0;
    //     NRF_RADIO->EVENTS_END = 0;

    //     NRF_RADIO->SHORTS &= ~RADIO_SHORTS_DISABLED_TXEN_Msk;
    //     NRF_RADIO->SHORTS |=  RADIO_SHORTS_DISABLED_RXEN_Msk;
    // }
    // if(NRF_RADIO->EVENTS_RXREADY)
    // {
    //     NRF_RADIO->EVENTS_RXREADY = 0;
    //     NRF_RADIO->SHORTS |=  RADIO_SHORTS_DISABLED_TXEN_Msk;
    //     NRF_RADIO->SHORTS &= ~RADIO_SHORTS_DISABLED_RXEN_Msk;
    //     NRF_RADIO->PACKETPTR = (uint32_t) rxBuf;

    //     // Start listening and wait for the END event
    //     NRF_RADIO->TASKS_START = 1;
    // }

    
    // if(NRF_RADIO->EVENTS_PHYEND)
    // {
    //     NRF_RADIO->EVENTS_PHYEND = 0;
    //     NRF_RADIO->TASKS_DISABLE = 1;
    // }
    
}

void my_TIMER_IRQhandler(void)
{
    interrupt += 1;
    NRF_TIMER1->INTENSET=1 << 16;
    // if(NRF_TIMER1->EVENTS_COMPARE[0])
    // {
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;
        // NRF_TIMER1->TASKS_CLEAR = 1;
    // }
}


const uint8_t MICROBIT_RADIO_POWER_LEVEL[] = {0xD8, 0xEC, 0xF0, 0xF4, 0xF8, 0xFC, 0x00, 0x04};


int main()
{
    serial.printf("1");
    bit_set(NRF_GPIO->DIR, bit(2));
    // If this is the first time we've been enable, allocate out receive buffers.
    if (rxBuf == NULL)
        rxBuf = new FrameBuffer();

    if (rxBuf == NULL)
        return DEVICE_NO_RESOURCES;

    // Enable the High Frequency clock on the processor. This is a pre-requisite for
    // the RADIO module. Without this clock, no communication is possible.
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;//Open an additional monitorlows the RADIO hardware to perform
    // address matching for us, and only generate an interrupt when a packet matching our group is received.
    NRF_RADIO->BASE0 = MICROBIT_RADIO_BASE_ADDRESS;
    NRF_RADIO->PREFIX0 = 0;

    // Join the default group. This will configure the remaining byte in the RADIO hardware module.
    //setGroup(this->group);

    // The RADIO hardware module supports the use of multip#define bit(n) (1 << n)
#define bit_set(var, mask) ((var) |= (mask))
#define bit_clear(var, mask) ((var) &= ~(mask))
#define bit_toggle(var, mask) ((var) ^= (mask))
#define bit_read(var, mask) ((var) & (mask))
    // Enable automatic 16bit CRC generation and checking, and configure how the CRC is calculated.
    NRF_RADIO->CRCCNF = RADIO_CRCCNF_LEN_Two;
    NRF_RADIO->CRCINIT = 0xFFFF;
    NRF_RADIO->CRCPOLY = 0x11021;

    // Set the start random value of the data whitening algorithm. This can be any non zero number.
    NRF_RADIO->DATAWHITEIV = 0x18;

    // Set up the RADIO module to read and write from our internal buffer.
    NRF_RADIO->PACKETPTR = (uint32_t)rxBuf;

    // Configure the hardware to issue an interrupt whenever a task is complete (e.g. send/receive).
    bit_set(NRF_GPIO->OUT, bit(2));
    bit_set(NRF_RADIO->INTENSET, RADIO_INTENSET_RXREADY_Msk 
                                | RADIO_INTENSET_TXREADY_Msk
                                | RADIO_INTENSET_CRCOK_Msk
                                | RADIO_INTENSET_PHYEND_Msk);
    
    

    serial.printf("2");

    NRF_RADIO->SHORTS |= RADIO_SHORTS_ADDRESS_RSSISTART_Msk;

    // Start listening for the next packet
    NRF_RADIO->EVENTS_RXREADY = 0;
    NRF_RADIO->TASKS_RXEN = 1;
    serial.printf("a");
    while(NRF_RADIO->EVENTS_RXREADY == 0);
    serial.printf("3");
    bit_clear(NRF_GPIO->OUT, bit(2));

    NRF_RADIO->EVENTS_END = 0;
    serial.printf("a");
    NRF_RADIO->TASKS_START = 1;
    serial.printf("b");
    // NVIC_ClearPendingIRQ(RADIO_IRQn);
    serial.printf("c");
    // NVIC_EnableIRQ(RADIO_IRQn);
    serial.printf("4");

    // register ourselves for a callback event, in order to empty the receive queue.
    //status |= DEVICE_COMPONENT_STATUS_IDLE_TICK;

    NRF_TIMER1->PRESCALER = 4;
    NRF_TIMER1->CC[0] = 23; // 200 microseconds
    NRF_TIMER1->INTENSET=1 << 16;
    NRF_TIMER1->SHORTS=1;
    // NRF_TIMER1->BITMODE=3;
    serial.printf("5");

    // Done. Record that our RADIO is configured.
    //status |= MICROBIT_RADIO_STATUS_INITIALISED;

    NVIC_SetVector(TIMER1_IRQn, (uint32_t)my_TIMER_IRQhandler);
    NVIC_SetPriority(TIMER1_IRQn, 2);
    NVIC_EnableIRQ(TIMER1_IRQn);
    serial.printf("5");

    NRF_TIMER1->TASKS_START = 1;
    serial.printf("5");

    NVIC_SetVector(IRQn_Type::RADIO_IRQn, (uint32_t)my_RADIO_IRQHandler);
    serial.printf("6");
    //NVIC_SetPriority(IRQn_Type::RADIO_IRQn, 2);
    NVIC_EnableIRQ(IRQn_Type::RADIO_IRQn);
    serial.printf("6");

    

    // uBit.init();
    // uBit.messageBus.listen(DEVICE_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    // uBit.radio.enable();
    //uBit.display.print("K");

    while(1) {
        serial.printf("%d", interrupt);
        //uBit.sleep(100);
    }

    //microbit_panic( 999 );
}

