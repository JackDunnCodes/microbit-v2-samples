#include "MicroBit.h"
//#include "ManagedString.h"
//#include "samples/Tests.h"
#include <string>


#define bit(n) (1 << n)
#define bit_set(var, mask) ((var) |= (mask))
#define bit_clear(var, mask) ((var) &= ~(mask))
#define bit_toggle(var, mask) ((var) ^= (mask))
#define bit_read(var, mask) ((var) & (mask))

MicroBit uBit;

//NRF52Pin srTx(ID_PIN_USBTX, MICROBIT_PIN_UART_TX, PIN_CAPABILITY_DIGITAL);
//NRF52Pin srRx(ID_PIN_USBRX, MICROBIT_PIN_UART_RX, PIN_CAPABILITY_DIGITAL);
//NRF52Serial serial(srTx, srRx, NRF_UARTE0);



const uint8_t MICROBIT_RADIO_POWER_LEVEL[] = {0xD8, 0xEC, 0xF0, 0xF4, 0xF8, 0xFC, 0x00, 0x04};


int main()
{
    uBit.init();
    uBit.buttonB.enable();
    uBit.serial.printf("Go!");
    uBit.display.setBrightness(255);
    uBit.display.image.setPixelValue(4, 2, 255);
    uBit.meshRadio.enable();
    uBit.display.image.setPixelValue(4, 3, 255);
    uBit.display.setBrightness(255);
    uBit.display.image.setPixelValue(4, 4, 255);
    int cnt = 0;
    while(true) {
        uBit.serial.printf("Loop!");
        uBit.display.image.setPixelValue(4, 0, (cnt++ * 20) % 255);
        if(uBit.buttonB.isPressed()) {
//            uBit.meshRadio.datagram.send("Excellent mesh");
            uBit.serial.printf("press!");
            uBit.display.image.setPixelValue(0, 0, 255);
        }
////        FrameBuffer *a = uBit.meshRadio.recv();
//        if((char *)a->payload == "Excellent mesh") {
//            uBit.display.image.setPixelValue(2, 2, 255);
//        }
//        uBit.sleep(50);
    }
}

