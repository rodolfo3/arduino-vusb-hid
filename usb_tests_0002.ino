#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/eeprom.h>

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "oddebug.h" 

#ifdef __cplusplus
extern "C"{
#endif 
PROGMEM char usbHidReportDescriptor[22] = {    /* USB report descriptor */
    0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x09, 0x00,                    //   USAGE (Undefined)
    0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
    0xc0                           // END_COLLECTION
};


#ifdef __cplusplus
} // extern "C"
#endif

int i = 0;

usbMsgLen_t usbFunctionSetup (uchar *data)
{
  usbRequest_t *rq = (usbRequest_t*)((void *)data);
  if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) { /* HID class request */
      if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
        /* since we have only one report type, we can ignore the report-ID */
        // static uchar dataBuffer[1];  /* buffer must stay valid when usbFunctionSetup returns */
        // dataBuffer[0] = tx_read();
        // usbMsgPtr = dataBuffer; /* tell the driver which data to return */
        return 1; /* tell the driver to send 1 byte */
      } else {
        if(rq->bRequest == USBRQ_HID_SET_REPORT) {
          /* since we have only one report type, we can ignore the report-ID */
          // TODO: Check race issues?
          process(rq->wIndex.bytes[0]);
        }
    } else {
        /* ignore vendor type requests, we don't use any */
    }
    return 0;

}

void process(byte i) {
  digitalWrite(13, i ? HIGH : LOW);
}

void usbFunctionWriteOut (uchar *data, uchar len)
{
  Serial.write("usbFunctionWriteOut\n");
  digitalWrite(13, LOW);
}


void usbFunctionWrite (uchar *data, uchar len)
{
  Serial.write("usbFunctionWrite\n");
  digitalWrite(13, LOW);
}

void init() {
    // disable timer 0 overflow interrupt
    // (used for millis)
    /*
    TIMSK0&=!(1<<TOIE0);

    cli();

    usbInit();
      
    usbDeviceDisconnect();
    uchar   i;
    i = 0;
    while(--i){
        _delay_ms(1);
    }
    usbDeviceConnect();

      sei();
     */
    wdt_disable();
    odDebugInit();
    usbInit();
  
    usbDeviceDisconnect();  // force re-enumeration
    _delay_ms(250);
    usbDeviceConnect();
  
    sei();      

}

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  
  init();
  digitalWrite(13, HIGH);
}

void main_loop() {
  static uchar inbuf[1];
  inbuf[0] = 0xff; 
  if (usbInterruptIsReady()) {
    // Serial.write("usbInterruptIsReady() == true\n");
  
    // usbSetInterrupt(inbuf, 1);
  }
}

void loop() {
  usbPoll();
  main_loop();
}
