#include <string>
#include <iostream>
#include <cstdio>

// OS Specific sleep
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "serial/serial.h"
#include "linbus.h"

void my_sleep(unsigned long milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

void enumerate_ports()
{
    std::vector<serial::PortInfo> devices_found = serial::list_ports();

    std::vector<serial::PortInfo>::iterator iter = devices_found.begin();

    while (iter != devices_found.end())
    {
        serial::PortInfo device = *iter++;

        printf("(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(), device.hardware_id.c_str());
    }
}

void print_usage()
{
      printf("Usage: lintest {-e|<serial port address>} ");
}

int main(int argc, char **argv)
{
    try
    {
        if (argc < 2)
        {
            print_usage();
            return 0;
        }

        // Argument 1 is the serial port or enumerate flag
        std::string port(argv[1]);

        if (port == "-e")
        {
            enumerate_ports();
            return 0;
        }

        // port, baudrate, timeout in milliseconds
        serial::Serial serial_port(port, 19200, serial::Timeout::simpleTimeout(1000));

        if (!serial_port.isOpen())
        {
            printf("Unable to open serial port %s\n", argv[1]);
            return 0;
        }

        uint8_t lindata[] = { 31, 255, 0, 0, 3 };
        LINBus lin(&serial_port);
        lin.write(0xA3, lindata, 5);

        serial_port.close();
    }
    catch (std::exception &e)
    {
        printf("Unhandled Exception: %s\n", e.what());
    }

    return 0;
}
