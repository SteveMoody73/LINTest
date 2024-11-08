#include <stdint.h>

#include "serial/serial.h"

class linbus
{
	public:
		// Constructors
		linbus(uint8_t Ch); // Constructor for Master Node
		linbus(uint8_t Ch, uint8_t ident); // Constructor for Slave Node
		
		// Methods
		
		// Writing data to bus
		int write(uint8_t add, uint8_t *data, uint8_t data_size); // write whole package
		int writeRequest(uint8_t add); // Write header only
		int writeResponse(uint8_t *data, uint8_t data_size); // Write response only
		int writeStream(uint8_t *data, uint8_t data_size); // Writing user data to LIN bus
		int read(uint8_t *data, uint8_t data_size); // read data from LIN bus, checksum and ident validation
		int readStream(uint8_t *data,uint8_t data_size); // read data from LIN bus
		int setSerial(); // set up Seril communication for receiving data.
		int busWakeUp(); // send wakeup frame for waking up all bus participants
	
	// Private methods and variables
	private:
		const unsigned long bound_rate = 19200; // 10417 is best for LIN Interface, most device should work
		const unsigned int period = 52; // in microseconds, 1s/10417
		uint8_t ch = 0; // which channel should be used
		uint8_t identuint8_t; // user defined Identification uint8_t
		int sleep(uint8_t sleep_state); // method for controlling transceiver modes (0 - sleep, 1 - normal)
		int sleep_config(uint8_t serial_No); // configuration of sleep pins
		int serial_pause(int no_bits); // for generating Synch Break
		bool validateParity(uint8_t ident); // for validating Identification uint8_t, can be modified for validating parity
		bool validateChecksum(uint8_t *data, uint8_t data_size); // for validating Checksum uint8_t
		uint8_t calcIdentParity(uint8_t ident);
		
};