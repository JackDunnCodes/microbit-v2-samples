#include "MicroBit.h"
#include "ManagedString.h"
#include "samples/Tests.h"
#include <string>
#define RX false

MicroBit uBit;
static uint8_t locount = 0;
static uint8_t hicount = 0;

extern "C" void RADIO_IRQHandler(void)
{
    if(NRF_RADIO->EVENTS_TXREADY)
    {
        NRF_RADIO->EVENTS_TXREADY = 0;
        NRF_RADIO->EVENTS_END = 0;

        NRF_RADIO->SHORTS &= ~RADIO_SHORTS_DISABLED_TXEN_Msk;
        NRF_RADIO->SHORTS |=  RADIO_SHORTS_END_RXEN_Msk;

        NRF_RADIO->TASKS_START = 1;
    }
    if(NRF_RADIO->EVENTS_RXREADY)
    {
        NRF_RADIO->EVENTS_RXREADY = 0;
        NRF_RADIO->SHORTS &= ~RADIO_SHORTS_END_RXEN_Msk;
        NRF_RADIO->SHORTS |=  RADIO_SHORTS_DISABLED_TXEN_Msk;

        // Start listening and wait for the END event
        NRF_RADIO->TASKS_START = 1;
    }

    if(NRF_RADIO->EVENTS_END)
    {
        NRF_RADIO->EVENTS_END = 0;
        if(NRF_RADIO->CRCSTATUS == 1)
        {
            int sample = (int)NRF_RADIO->RSSISAMPLE;

            // Associate this packet's rssi value with the data just
            // transferred by DMA receive
            MicroBitRadio::instance->setRSSI(-sample);

            // Now move on to the next buffer, if possible.
            // The queued packet will get the rssi value set above.
            MicroBitRadio::instance->queueRxBuf();

            // Set the new buffer for DMA
            NRF_RADIO->PACKETPTR = (uint32_t) MicroBitRadio::instance->getRxBuf();
        }
        else
        {
            MicroBitRadio::instance->setRSSI(0);
        }

        // Start listening and wait for the END event
        NRF_RADIO->TASKS_START = 1;
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

