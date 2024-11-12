#include "linbus.h"

/* LIN PACKET:
   It consist of:
    ___________ __________ _______ ____________ _________
   |           |          |       |            |         |
   |Synch Break|Synch byte|ID byte| Data bytes |Checksum |
   |___________|__________|_______|____________|_________|

   Every byte have start bit and stop bit and it is send LSB first.
   Synch Break - 13 bits of dominant state ("0"), followed by 1 bit recesive state ("1")
   Synch byte - byte for Bound rate syncronization, always 0x55
   ID byte - consist of parity, length and address; parity is determined by LIN standard and depends from address and message length
   Data bytes - user defined; depend on devices on LIN bus
   Checksum - inverted 256 checksum; data bytes are sumed up and then inverted
*/

// CONSTRUCTORS
LINBus::LINBus(serial::Serial* serial)
{
    _serial = serial;
    sleep_config();
}

LINBus::LINBus(serial::Serial* serial, uint8_t ident)
{
    sleep_config();
    identByte = ident; // saving idet to private variable
    sleep(1);          // Transceiver is always in Normal Mode
}

// PUBLIC METHODS
// WRITE methods
// Creates a LIN packet and then send it via USART(Serial) interface.
int LINBus::write(uint8_t ident, uint8_t *data, uint8_t data_size)
{
    // TODO:
    // Not sure what limit is on LIN bus, may be better to dynamically allocate
    uint8_t sendbytes[32];  

    // Calculate checksum
    uint8_t suma = 0;
    sendbytes[0] = SYNC_BYTE;
    sendbytes[1] = ident;
    int index = 2;

    for (int i = 0; i < data_size; i++)
    {
        sendbytes[index++] = data[i]; // write data to serial
        suma = suma + data[i];
    }
    uint8_t checksum = 255 - suma;
    sendbytes[index] = checksum;

    // Start interface
    sleep(1); // Go to Normal mode

    // Synch Break
    send_break();

    _serial->write(sendbytes, index);
    
    sleep(0); // Go to Sleep mode
    return 1;
}

int LINBus::writeRequest(uint8_t ident)
{
    // Create Header
    uint8_t identuint8_t = (ident & 0x3f) | calcIdentParity(ident);
    uint8_t header[2] = {0x55, identuint8_t};

    // Start interface
    sleep(1); // Go to Normal mode

    // Synch Break
    send_break();

    _serial->write(header, 2);  // write data to serial

    sleep(0); // Go to Sleep mode
    return 1;
}

int LINBus::writeResponse(uint8_t *data, uint8_t data_size)
{
    uint8_t sendbytes[32];

    // Calculate checksum
    uint8_t suma = 0;
    for (int i = 0; i < data_size; i++)
    {
        sendbytes[i] = data[i];
        suma = suma + data[i];
    }
    uint8_t checksum = 255 - suma;
    sendbytes[data_size] = checksum;

    // Start interface
    sleep(1); // Go to Normal mode

    // Send data via Serial interface
    _serial->write(data, data_size + 1); // write data to serial
    
    sleep(0); // Go to Sleep mode
    return 1;
}

int LINBus::writeStream(uint8_t *data, uint8_t data_size)
{
    // Start interface
    sleep(1); // Go to Normal mode

    // Synch Break
    send_break();
    _serial->write(data, data_size);

    sleep(0); // Go to Sleep mode
    return 1;
}

int LINBus::read(uint8_t *data, uint8_t data_size)
{
    //uint8_t rec[data_size + 3];
    // TODO: Check limit and maybe dynamically allocate if needed
    uint8_t rec[64];

    // TODO: Based on the arduino code, serial.read() is called to check if there is any data
    // available and returns -1 if not. This may read and discard the first byte so data read may
    // have to be adjusted
    //if (Serial1.read() != -1)
    size_t bytes_read = _serial->read(data, data_size + 3);
    if (bytes_read == data_size + 3)
    {
        if ((validateParity(rec[1])) & (validateChecksum(rec, data_size + 3)))
        {
            for (int j = 0; j < data_size; j++)
            {
                data[j] = rec[j + 2];
            }
            return 1;
        }
    }
    else
        return -1;

    return 0;
}

int LINBus::readStream(uint8_t data[], uint8_t data_size)
{
    //uint8_t rec[data_size];
    // TODO: Check limit and maybe dynamically allocate if needed
    uint8_t rec[64];

    // TODO: Based on the arduino code, serial.read() is called to check if there is any data
    // available and returns -1 if not. This may read and discard the first byte so data read may
    // have to be adjusted
    //if (Serial1.read() != -1)
    size_t bytes_read = _serial->read(data, data_size);

    if (bytes_read == data_size)
    {
        for (int j = 0; j < data_size; j++)
        {
            data[j] = rec[j];
        }
        return 1;
    }

    return 0;
}

// PRIVATE METHODS
void LINBus::send_break()
{
    uint8_t data[1] = { 0 };
    _serial->setBaudrate(baud_rate / 2);
    _serial->write(data, 1);
    _serial->setBaudrate(baud_rate);
}

int LINBus::sleep(uint8_t sleep_state)
{
    // TODO: Need to control the CS signal line to the transiever 
    if (sleep_state == 1)
    {
    }
    else if (sleep_state == 0)
    {
    }
    Delay(20); // According to TJA1021 datasheet this is needed for proper working
    return 1;
}

int LINBus::sleep_config()
{
    // TODO: Set up the CS port for the raspberry pi GPIO header
    return 1;
}

bool LINBus::validateParity(uint8_t ident)
{
    if (ident == identByte)
        return true;
    else
        return false;
}

bool LINBus::validateChecksum(unsigned char *data, uint8_t data_size)
{
    uint8_t checksum = data[data_size - 1];
    uint8_t suma = 0;
    for (int i = 2; i < data_size - 1; i++)
        suma = suma + data[i];
    uint8_t v_checksum = 255 - suma - 1;
    if (checksum == v_checksum)
        return true;
    else
        return false;
}

int LINBus::busWakeUp()
{
    // TODO: Wake up pin not connected, should be controlled using the CS line, can delete if not needed
    return 1;
}

/* Create the Lin ID parity */
#define BIT(data, shift) ((ident & (1 << shift)) >> shift)
uint8_t LINBus::calcIdentParity(uint8_t ident)
{
    uint8_t p0 = BIT(ident, 0) ^ BIT(ident, 1) ^ BIT(ident, 2) ^ BIT(ident, 4);
    uint8_t p1 = ~(BIT(ident, 1) ^ BIT(ident, 3) ^ BIT(ident, 4) ^ BIT(ident, 5));
    return (p0 | (p1 << 1)) << 6;
}
