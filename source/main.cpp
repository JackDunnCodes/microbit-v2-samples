#include "MicroBit.h"
#include "ManagedString.h"
#include "samples/Tests.h"
#include <string>
#define RX false

MicroBit uBit;
static uint8_t locount = 0;
static uint8_t hicount = 0;

static void sendMeshData(ManagedString s) {
    auto b = new PacketBuffer((uint8_t *) s.toCharArray(), s.length()+8);
    for(int i=s.length(); i>=0; i--) {
        b->setByte(i+8, b->getByte(i));
    }
    b->setByte(0, 'u');
     b->setByte(1, 'B');
     b->setByte(2, 't');
    
    b->setByte(3, 'm');
     b->setByte(4, 's');
     b->setByte(5, 'h');
     if(locount >= 0b11111111) {
         locount = 0;
         hicount += 1;
     }
    locount += 1;
    b->setByte(6, hicount);
    b->setByte(7, locount);

    uBit.radio.datagram.send(*b);
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

