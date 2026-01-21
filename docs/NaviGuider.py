import serial
import time
import datetime

class NaviGuider:
    def __init__(self, port, baudrate=115200):
        """
        Initializes the NaviGuider object and establishes serial communication.

        Args:
            port (str): The serial port to connect to (e.g., 'COM3' on Windows, '/dev/ttyUSB0' on Linux).
            baudrate (int): The baud rate for serial communication (default: 115200).
        """
        try:
            self.ser = serial.Serial(port, baudrate, timeout=1)  # Adjust timeout as needed
            print(f"Connected to NaviGuider on port {port}")
        except serial.SerialException as e:
            print(f"Error: Could not connect to NaviGuider on port {port}. {e}")
            self.ser = None  # Set to None if connection fails
            
        self.sensors_continuous = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 14, 15, 16, 17, 20]
        
        # Record current time for timestamping telemetry
        self.start_time = int(time.time())
    
    def send_command(self, command):
        """
        Sends a command to the NaviGuider module.

        Args:
            command (str): The command string to send (e.g., 'v', 'n', 'O', etc.).
        """
        if self.ser:
            command_str = command + '\r'  # Append carriage return
            self.ser.write(command_str.encode('ascii'))
            print(f"Sent command: {command}")

    def read_response(self, i=1):
        """
        Reads the response from the NaviGuider module.

        Returns:
            str: The response string, or None if there is an error or timeout.
        """
        if self.ser:
            try:
                response = self.ser.readline().decode('ascii').strip()
                if len(response) > 1 and response[1].isdigit():
                    #timestamp_32,_,remainder = response.partition(',') # Split timestamp from the string
                    #timestamp = datetime.datetime.fromtimestamp(self.start_time + int(timestamp_32,10) / 32000)
                    
                    split_response = response.split(',')
                    if len(split_response) >= 2:
                        try:
                            split_response[0] = datetime.datetime.fromtimestamp(self.start_time + int(split_response[0],10) / 32000)
                            #split_response[2:-1] = list(map(float,split_response[2:-1]))
                        finally:
                            for i in range(len(split_response)):
                                try:
                                    split_response[i] = list(map(float,split_response[i]))
                                finally:
                                    return split_response
                    else:
                        return response
                    #print(f'{timestamp}: {remainder}')
                return response
            except UnicodeDecodeError:
                print("Error: Could not decode response from NaviGuider.")
                return None
            except IndexError:  # Handle potential IndexError when response is too short
                print("Error: Short or unexpected response from NaviGuider.")
                return None
        else:
            return None
        
    def start_sensor(self, sensor_id, data_rate):
        """
        Starts a sensor at a given rate.

        Args:
            sensor_id (int): The ID of the sensor to start (see Table 4-2).
            data_rate (int): The data rate for the sensor (aggregate rate <= 1200 Hz).
        """
        command = f"s {sensor_id},{data_rate}\r"
        self.send_command(command)
        response = self.read_response()
        if response:
            print(f"Sensor Start Response: {response}")
        else:
            print("Failed to start sensor.")
            
    def start_all_sensors(self):
        """
        Starts all continuous sensors a 1 Hz.

        """
        for register in self.sensors_continuous:
            self.start_sensor(register,1)
            
    def stop_all_sensors(self):
        """
        Stops all continuous sensors.

        """
        for register in self.sensors_continuous:
            self.start_sensor(register,0)
            
    def close(self):
        """
        Closes the serial port connection.
        """
        if self.ser:
            self.ser.close()
            print("Serial port closed.")
    
    """
    Command functions
    """
    
    def display_version(self):
        """
        Sends the 'v' command to display the NaviGuider version.
        """
        self.send_command('v')
        for i in range(8):
            response = self.read_response()
            if response:
                print(f"NaviGuider Version: {response}")
            else:
                print("Failed to retrieve version information.")

    def get_sensor_information(self):
        """
        Sends the 'n' command to display sensor information.
        """
        self.send_command('n')
        for i in range(18):
            response = self.read_response()
            if response:
                print(f"Sensor Information: {response}")
            else:
                print("Failed to retrieve sensor information.")
                
    def power_down(self):
        """
        Sends the 'P' command to power down the sensor.
        """
        self.send_command('P')
        response = self.read_response()
        if response:
            print(f"Sensor Information: {response}")
        else:
            print("Failed to retrieve sensor information.")

    def restart(self):
        """
        Sends the 'X' command to restart the sensor.
        """
        self.send_command('X')
        time.sleep(.5)
        response = self.read_response()
        if response:
            print(f"Sensor Information: {response}")
        else:
            print("Failed to retrieve sensor information.")

    def Set_Mounting_Option(self):
        """
        Sends the 'M2' command to set the units mounting orientation to "X" up 0.
        """
        self.send_command('M2\r')
        response = self.read_response()
        if response:
            print(f"Sensor Information: {response}")
        else:
            print("Failed to retrieve sensor information.")

    def Magnetometer_Mode(self):
        """
        Starts magnetometer sensors at continuous 1 Hz.
        """
        self.start_sensor(2,1)
        self.start_sensor(14,1)

# Example Usage
if __name__ == "__main__":
    # Replace 'COM3' with the correct serial port for your system
    try:
        naviguider = NaviGuider('COM3')
    
        if naviguider.ser:  # Only proceed if the connection was successful
            naviguider.restart()
            naviguider.Set_Mounting_Option()
            #naviguider.display_version()
            #naviguider.get_sensor_information()
    
            # Start all sensors at a rate of 1 Hz
            naviguider.start_all_sensors()
            #naviguider.Magnetometer_Mode()
    
            # Read some data for a short time
            for _ in range(100):
                data = naviguider.read_response()
                if(data):
                    print(f'{data[0]}: {data[1:]}')
                    #print(f"Received Data: {data}")
                #time.sleep(.1) # Sleep .1 seconds
                
            # Stop all sensors
            naviguider.stop_all_sensors
            
            # Power down the device
            naviguider.power_down()
            
    finally:
        naviguider.close()