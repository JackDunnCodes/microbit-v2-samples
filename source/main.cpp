#include "MicroBit.h"
#include "ManagedString.h"
#include "samples/Tests.h"
#include <string>
#define RX false

MicroBit uBit;
static uint8_t locount = 0;
static uint8_t hicount = 0;

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

extern "C" void RADIO_IRQHandler(void)
{
    // immediately set off timer, hopefully improve determinism
    NRF_TIMER0->PRESCALER = 0;
    NRF_TIMER0->CC[0] = 3200; // 200 microseconds
    NRF_TIMER0->TASKS_START=1;
    if(NRF_RADIO->EVENTS_CRCOK)
    {
        NRF_RADIO->EVENTS_CRCOK = 0;
        if(NRF_RADIO->CRCSTATUS == 1)
        {
            *sample = (int)NRF_RADIO->RSSISAMPLE;

            // Now move on to the next buffer, if possible.
            // The queued packet will get the rssi value set above.
            rxBuf = NRF_RADIO->

            
            // Set the new buffer for DMA
            NRF_RADIO->PACKETPTR = (uint32_t) MicroBitRadio::instance->getRxBuf();
        }
        else
        {
            MicroBitRadio::instance->setRSSI(0);
        }

        
        NRF_RADIO->TASKS_DISABLE = 1;
    } else {
        // cancel timer
        NRF_TIMER0->TASKS_STOP = 1;
        NRF_TIMER0->TASKS_CLEAR = 1;
    }

    if(NRF_RADIO->EVENTS_TXREADY)
    {
        NRF_RADIO->EVENTS_TXREADY = 0;
        NRF_RADIO->EVENTS_END = 0;

        NRF_RADIO->SHORTS &= ~RADIO_SHORTS_DISABLED_TXEN_Msk;
        NRF_RADIO->SHORTS |=  RADIO_SHORTS_DISABLED_RXEN_Msk;
    }
    if(NRF_RADIO->EVENTS_RXREADY)
    {
        NRF_RADIO->EVENTS_RXREADY = 0;
        NRF_RADIO->SHORTS |=  RADIO_SHORTS_DISABLED_TXEN_Msk;
        NRF_RADIO->SHORTS &= ~RADIO_SHORTS_DISABLED_RXEN_Msk;
        NRF_RADIO->PACKETPTR = rxBuf;

        // Start listening and wait for the END event
        NRF_RADIO->TASKS_START = 1;
    }

    
    if(NRF_RADIO->EVENTS_PHYEND)
    {
        NRF_RADIO->EVENTS_PHYEND = 0;
        NRF_RADIO->TASKS_DISABLE = 1;
    }
}

extern "C" void TIMER_IRQhandler(void)
{
    if(NRF_TIMER0->EVENTS_COMPARE[0])
    {
        NRF_TIMER0->EVENTS_COMPARE[0] = 0;
        NRF_RADIO->TASKS_START = 1;
        NRF_TIMER0->TASKS_STOP = 1;
        NRF_TIMER0->TASKS_CLEAR = 1;
    }
}

static void sendMeshData(ManagedString s) {
    
}

static void onData(MicroBitEvent)
{
    PacketBuffer b = uBit.radio.datagram.recv();
    if(b[0] == 'u' && b[1] =='B' && b[2] =='t' && b[3] == 'm' && b[4] == 's' && b[5] == 'h'){
        if ((b[6] >= hicount) && (b[7] > locount)) {
            hicount = b[6]; locount = b[7];
            uBit.radio.datagram.send(b);
        }
    }
    if (b[8] == '1')
        uBit.display.print("A");

    if (b[8] == '2')
        uBit.display.print("B");
}

int main()
{
    NVIC_SetVector(IRQn_Type::TIMER0_IRQn, TIMER_IRQhandler);
    NVIC_SetPriority(IRQn_Type::TIMER0_IRQn, 2);
    NVIC_EnableIRQ(IRQn_Type::TIMER0_IRQn);

    NVIC_SetVector(IRQn_Type::RADIO_IRQn, RADIO_IRQHandler);
    NVIC_SetPriority(IRQn_Type::RADIO_IRQn, 2);
    NVIC_EnableIRQ(IRQn_Type::RADIO_IRQn);
    uBit.init();
    uBit.messageBus.listen(DEVICE_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();
    uBit.display.print("K");
    uBit.radio.datagram.send("1");

    while(1) {
        if (uBit.buttonA.isPressed())
            sendMeshData("1");

        else if (uBit.buttonB.isPressed())
            sendMeshData("2");

        uBit.sleep(100);
    }

    microbit_panic( 999 );
}

