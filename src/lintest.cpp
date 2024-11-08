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
      printf("Usage: lintest {-e|<serial port address>} ";
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
        serial::Serial my_serial(port, 19200, serial::Timeout::simpleTimeout(1000));

        if (!my_serial.isOpen())
        {
            printf("Unable to open serial port %s\n", argv[1]);
            return 0;
        }

            size_t bytes_wrote = my_serial.write(test_string);

        // Test the timeout at 250ms
        my_serial.setTimeout(serial::Timeout::max(), 250, 0, 250, 0);
        count = 0;
        cout << "Timeout == 250ms, asking for 1 more byte than written." << endl;
        while (count < 10)
        {
            size_t bytes_wrote = my_serial.write(test_string);

            string result = my_serial.read(test_string.length() + 1);

            cout << "Iteration: " << count << ", Bytes written: ";
            cout << bytes_wrote << ", Bytes read: ";
            cout << result.length() << ", String read: " << result << endl;

            count += 1;
        }

        // Test the timeout at 250ms, but asking exactly for what was written
        count = 0;
        cout << "Timeout == 250ms, asking for exactly what was written." << endl;
        while (count < 10)
        {
            size_t bytes_wrote = my_serial.write(test_string);

            string result = my_serial.read(test_string.length());

            cout << "Iteration: " << count << ", Bytes written: ";
            cout << bytes_wrote << ", Bytes read: ";
            cout << result.length() << ", String read: " << result << endl;

            count += 1;
        }

        // Test the timeout at 250ms, but asking for 1 less than what was written
        count = 0;
        cout << "Timeout == 250ms, asking for 1 less than was written." << endl;
        while (count < 10)
        {
            size_t bytes_wrote = my_serial.write(test_string);

            string result = my_serial.read(test_string.length() - 1);

            cout << "Iteration: " << count << ", Bytes written: ";
            cout << bytes_wrote << ", Bytes read: ";
            cout << result.length() << ", String read: " << result << endl;

            count += 1;
        }
    }
    catch (exception &e)
    {
        cerr << "Unhandled Exception: " << e.what() << endl;
    }
}
