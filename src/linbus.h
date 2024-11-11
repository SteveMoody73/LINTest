#include <stdint.h>

#include "serial/serial.h"
// OS Specific sleep
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

class linbus
{
	public:
		// Constructors
		linbus(serial::Serial *serial);					// Constructor for Master Node
		linbus(serial::Serial *serial, uint8_t ident);	// Constructor for Slave Node
				
		// Writing data to bus
		int write(uint8_t add, uint8_t *data, uint8_t data_size);	// write whole package
		int writeRequest(uint8_t add);								// Write header only
		int writeResponse(uint8_t *data, uint8_t data_size);		// Write response only
		int writeStream(uint8_t *data, uint8_t data_size);			// Writing user data to LIN bus
		int read(uint8_t *data, uint8_t data_size);					// read data from LIN bus, checksum and ident validation
		int readStream(uint8_t *data,uint8_t data_size);			// read data from LIN bus
		int busWakeUp();											// send wakeup frame for waking up all bus participants
	
	// Private methods and variables
	private:
        serial::Serial* _serial;
		const unsigned long baud_rate = 19200;                     // 10417 is best for LIN Interface, most device should work
        const uint8_t SYNC_BYTE = 0x55;
		uint8_t identByte;                                          // user defined Identification Byte
        int linbus::sleep(uint8_t sleep_state);                     // Set the transcieve sleep state (CS)
        int linbus::sleep_config();                                 // Configure the sleep pin (CS)  

        void send_break();                              // for generating Synch Break
		bool validateParity(uint8_t ident);                         // for validating Identification Byte, can be modified for validating parity
		bool validateChecksum(uint8_t *data, uint8_t data_size);    // for validating Checksum Byte
		uint8_t calcIdentParity(uint8_t ident);

        void Delay(unsigned long milliseconds)
        {
            #ifdef _WIN32
                        Sleep(milliseconds);
            #else
                        usleep(milliseconds * 1000);
            #endif
        }

};
