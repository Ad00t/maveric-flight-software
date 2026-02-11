import serial
import time
import struct
import numpy as np

class TAD102063:

    def __init__(self, port, baud, par, stop):
        """ Register format: [index, data count, map idex, error code, data type]
                index: Denotes the register that the command will read/write from/to.
                data count: Indicates the length of the register being read/writtent from/to.
                map idex: Determins which part of the register is being accesses; always a value from 0 to 2.
                error code: Indicates if there was an error in a command. Always 0 when sending commands.
                data type: Indicates the kind of data stored in a register.
        """
                
        # Table 6-2. User Register (0)
        self.FACT = [0x00, 0x01, 0x00, 0x00, "uint16"]
        self.SNID = [0x01, 0x03, 0x00, 0x00, "char"]
        self.CONF = [0x04, 0x01, 0x00, 0x00, "uint8"]
        self.TIME = [0x05, 0x01, 0x00, 0x00, "uint8"]
        self.DATE = [0x06, 0x01, 0x00, 0x00, "uint8"]
        self.LLA_REF = [0x07, 0x03, 0x00, 0x00, "float"]
        self.Q_REF = [10, 0x04, 0x00, 0x00, "float"]
        self.POINTING_AXIS = [14, 0x03, 0x00, 0x00, "float"]
        self.TLE = [17, 35, 0x00, 0x00, "char"]
        self.GGA = [52, 32, 0x00, 0x00, "char"]
        self.ZDA = [84, 16, 0x00, 0x00, "char"]
        self.SV_USER = [100, 3, 0x00, 0x00, "float"]
        self.MTQ_USER = [103, 3, 0x00, 0x00, "float"]
        self.CMG0_G_RATE_USER = [106, 1, 0x00, 0x00, "float"]
        self.CMG0_G_TOR_USER = [107, 1, 0x00, 0x00, "float"]
        self.CMG1_G_RATE_USER = [108, 1, 0x00, 0x00, "float"]
        self.CMG1_G_TOR_USER = [109, 1, 0x00, 0x00, "float"]
        self.CMG2_G_RATE_USER = [110, 1, 0x00, 0x00, "float"]
        self.CMG2_G_TOR_USER = [111, 1, 0x00, 0x00, "float"]
        self.CMG3_G_RATE_USER = [112, 1, 0x00, 0x00, "float"]
        self.CMG3_G_TOR_USER = [113, 1, 0x00, 0x00, "float"]
        self.STAT = [128, 1, 0x00, 0x00, "uint8"]
        self.ACT_ERR = [129, 1, 0x00, 0x00, "uint8"]
        self.SEN_ERR = [130, 1, 0x00, 0x00, "uint8"]
        self.CSS_ERR = [131, 1, 0x00, 0x00, "uint8"]
        self.Q = [132, 4, 0x00, 0x00, "float"]
        self.RATE = [136, 3, 0x00, 0x00, "float"]
        self.LLA = [139, 3, 0x00, 0x00, "float"]
        self.ATT_ERROR = [142, 3, 0x00, 0x00, "float"]
        self.ATT_ERROR_RATE = [145, 3, 0x00, 0x00, "float"]
        self.ADCS_TMP = [148, 1, 0x00, 0x00, "int16"]
        self.CMG0_TMP = [149, 1, 0x00, 0x00, "int16"]
        self.CMG1_TMP = [150, 1, 0x00, 0x00, "int16"]
        self.CMG2_TMP = [151, 1, 0x00, 0x00, "int16"]
        self.CMG3_TMP = [152, 1, 0x00, 0x00, "int16"]
        self.FSS_TMP1 = [153, 1, 0x00, 0x00, "int16"]
        self.FSS_TMP2 = [154, 1, 0x00, 0x00, "int16"]
        self.FSS_TMP3 = [155, 1, 0x00, 0x00, "int16"]
        self.SV = [156, 3, 0x00, 0x00, "float"]
        self.MAG = [159, 3, 0x00, 0x00, "float"]
        
        # Table 6-3. Sensor/Actuator Register (1)
        self.MAG_MAT = [0, 9, 1, 0, "float"]
        self.MAG_VEC = [9, 3, 1, 0, "float"]
        self.MAG_STAT = [12, 1, 1, 0, "uint8"]
        self.MAG0_S = [13, 3, 1, 0, "float"]
        self.MAG1_S = [16, 3, 1, 0, "float"]
        self.MAG2_S = [19, 3, 1, 0, "float"]
        self.MAG3_S = [22, 3, 1, 0, "float"]
        self.MAG4_S = [25, 3, 1, 0, "float"]
        self.MAG5_S = [28, 3, 1, 0, "float"]
        self.FSS_STAT = [31, 1, 1, 0, "uint8"]
        self.FSS0_SV = [32, 1, 1, 0, "uint16"]
        self.FSS0_PDSUM = [33, 1, 1, 0, "uint16"]
        self.FSS1_SV = [34, 1, 1, 0, "uint16"]
        self.FSS1_PDSUM = [35, 1, 1, 0, "uint16"]
        self.FSS2_SV = [36, 1, 1, 0, "uint16"]
        self.FSS2_PDSUM = [37, 1, 1, 0, "uint16"]
        self.FSS3_SV = [38, 1, 1, 0, "uint16"]
        self.FSS3_PDSUM = [39, 1, 1, 0, "uint16"]
        self.FSS4_SV = [40, 1, 1, 0, "uint16"]
        self.FSS4_PDSUM = [41, 1, 1, 0, "uint16"]
        self.FSS5_SV = [42, 1, 1, 0, "uint16"]
        self.FSS5_PDSUM = [43, 1, 1, 0, "uint16"]
        self.IMU_STAT = [44, 1, 1, 0, "uint8"]
        self.IMU0_S = [45, 3, 1, 0, "float"]
        self.IMU1_S = [48, 3, 1, 0, "float"]
        self.IMU2_S = [51, 3, 1, 0, "float"]
        self.IMU3_S = [54, 3, 1, 0, "float"]
        self.STR_STAT = [57, 1, 1, 0, "uint8"]
        self.STR0_S = [58, 4, 1, 0, "float"]
        self.STR1_S = [62, 4, 1, 0, "float"]
        self.CSS = [66, 12, 1, 0, "float"]
        self.CMG_STAT = [78, 1, 1, 0, "uint8"]
        self.CMG0_G_ANGLE = [79, 1, 1, 0, "float"]
        self.CMG0_W_RATE = [80, 1, 1, 0, "float"]
        self.CMG1_G_ANGLE = [81, 1, 1, 0, "float"]
        self.CMG1_W_RATE = [82, 1, 1, 0, "float"]
        self.CMG2_G_ANGLE = [83, 1, 1, 0, "float"]
        self.CMG2_W_RATE = [84, 1, 1, 0, "float"]
        self.CMG3_G_ANGLE = [85, 1, 1, 0, "float"]
        self.CMG3_W_RATE = [86, 1, 1, 0, "float"]
        self.MTQ = [87, 3, 1, 0, "float"]
        self.CMG0_G_RATE = [90, 1, 1, 0, "float"]
        self.CMG0_W_ACC = [91, 1, 1, 0, "float"]
        self.CMG1_G_RATE = [92, 1, 1, 0, "float"]
        self.CMG1_W_ACC = [93, 1, 1, 0, "float"]
        self.CMG2_G_RATE = [94, 1, 1, 0, "float"]
        self.CMG2_W_ACC = [95, 1, 1, 0, "float"]
        self.CMG3_G_RATE = [96, 1, 1, 0, "float"]
        self.CMG3_W_ACC = [97, 1, 1, 0, "float"]
        
        # Table 6-4. Parameter Register (2)
        self.MASS = [0, 1, 2, 0, "float"]
        self.INE_TEN = [1, 9, 2, 0, "float"]
        self.POS_HB_B = [10, 3, 2, 0, "float"]
        self.ORIEN_HB = [13, 4, 2, 0, "float"]
        self.MAG_INFO = [17, 1, 2, 0, "uint8"]
        self.MAG0_ORIEN_BS = [18, 4, 2, 0, "float"]
        self.MAG1_ORIEN_BS = [22, 4, 2, 0, "float"]
        self.MAG2_ORIEN_BS = [26, 4, 2, 0, "float"]
        self.MAG3_ORIEN_BS = [30, 4, 2, 0, "float"]
        self.MAG4_ORIEN_BS = [34, 4, 2, 0, "float"]
        self.MAG5_ORIEN_BS = [38, 4, 2, 0, "float"]
        self.FSS_INFO = [42, 1, 2, 0, "uint8"]
        self.FSS0_ORIEN_BS = [43, 4, 2, 0, "float"]
        self.FSS1_ORIEN_BS = [47, 4, 2, 0, "float"]
        self.FSS2_ORIEN_BS = [51, 4, 2, 0, "float"]
        self.FSS3_ORIEN_BS = [55, 4, 2, 0, "float"]
        self.FSS4_ORIEN_BS = [59, 4, 2, 0, "float"]
        self.FSS5_ORIEN_BS = [63, 4, 2, 0, "float"]
        self.IMU_INFO = [67, 1, 2, 0, "uint8"]
        self.IMU0_ORIEN_BS = [68, 4, 2, 0, "float"]
        self.IMU1_ORIEN_BS = [72, 4, 2, 0, "float"]
        self.IMU2_ORIEN_BS = [76, 4, 2, 0, "float"]
        self.IMU3_ORIEN_BS = [80, 4, 2, 0, "float"]
        self.STR_INFO = [84, 1, 2, 0, "uint8"]
        self.STR0_ORIEN_BS = [85, 4, 2, 0, "float"]
        self.STR1_ORIEN_BS = [89, 4, 2, 0, "float"]
        self.NVM = [255, 1, 2, 0, "uint8"]
        
        self.user = [self.FACT, self.SNID, self.CONF, self.TIME, self.DATE,
                     self.LLA_REF, self.Q_REF, self.POINTING_AXIS, self.TLE,
                     self.GGA, self.SV_USER, self.MTQ_USER, self.CMG0_G_RATE_USER,
                     self.CMG0_G_TOR_USER, self.CMG1_G_RATE_USER, self.CMG1_G_TOR_USER,
                     self.CMG2_G_RATE_USER, self.CMG2_G_TOR_USER, self.CMG3_G_RATE_USER,
                     self.CMG3_G_TOR_USER, self.STAT, self.ACT_ERR, self.SEN_ERR, self.CSS_ERR,
                     self.Q, self.RATE, self.LLA, self.ATT_ERROR, self.ATT_ERROR_RATE, self.ADCS_TMP,
                     self.CMG0_TMP, self.CMG1_TMP, self.CMG2_TMP, self.CMG3_TMP, self.FSS_TMP1,
                     self.FSS_TMP2, self.FSS_TMP3, self.SV, self.MAG, self.MAG_MAT,
                     self.MAG_VEC, self.MAG_STAT, self.MAG0_S, self.MAG1_S, self.MAG2_S,
                     self.MAG3_S, self.MAG4_S, self.MAG5_S, self.FSS_STAT, self.FSS0_SV,
                     self.FSS0_PDSUM, self.FSS1_SV, self.FSS1_PDSUM, self.FSS2_SV, self.FSS2_PDSUM,
                     self.FSS3_SV, self.FSS3_PDSUM, self.FSS4_SV, self.FSS4_PDSUM, self.FSS5_SV,
                     self.FSS5_PDSUM, self.IMU_STAT, self.IMU0_S, self.IMU1_S, self.IMU2_S, self.IMU3_S,
                     self.STR_STAT, self.STR0_S, self.STR1_S, self.CSS, self.CMG_STAT, self.CMG0_G_ANGLE,
                     self.CMG0_W_RATE, self.CMG1_G_ANGLE, self.CMG1_W_RATE, self.CMG2_G_ANGLE, self.CMG2_W_RATE,
                     self.CMG3_G_ANGLE, self.CMG3_W_RATE, self.MTQ, self.CMG0_G_RATE, self.CMG0_W_ACC,
                     self.CMG1_G_RATE, self.CMG1_W_ACC, self.CMG2_G_RATE, self.CMG2_W_ACC, self.CMG3_G_RATE,
                     self.CMG3_W_ACC, self.MASS, self.INE_TEN, self.POS_HB_B, self.ORIEN_HB, self.MAG_INFO,
                     self.MAG0_ORIEN_BS, self.MAG1_ORIEN_BS, self.MAG2_ORIEN_BS, self.MAG3_ORIEN_BS,
                     self.MAG4_ORIEN_BS, self.MAG5_ORIEN_BS, self.FSS_INFO, self.FSS0_ORIEN_BS, self.FSS1_ORIEN_BS,
                     self.FSS2_ORIEN_BS, self.FSS3_ORIEN_BS, self.FSS4_ORIEN_BS, self.FSS5_ORIEN_BS, self.IMU_INFO,
                     self.IMU0_ORIEN_BS, self.IMU1_ORIEN_BS, self.IMU2_ORIEN_BS, self.IMU3_ORIEN_BS, self.STR_INFO,
                     self.STR0_ORIEN_BS, self.STR1_ORIEN_BS, self.NVM]
        
        self.names = ['FACT', 'SNID', 'CONF', 'TIME', 'DATE',
                     'LLA_REF', 'Q_REF', 'POINTING_AXIS', 'TLE',
                     'GGA', 'SV_USER', 'MTQ_USER', 'CMG0_G_RATE_USER',
                     'CMG0_G_TOR_USER', 'CMG1_G_RATE_USER', 'CMG1_G_TOR_USER',
                     'CMG2_G_RATE_USER', 'CMG2_G_TOR_USER', 'CMG3_G_RATE_USER',
                     'CMG3_G_TOR_USER', 'STAT', 'ACT_ERR', 'SEN_ERR', 'CSS_ERR',
                     'Q', 'RATE', 'LLA', 'ATT_ERROR', 'ATT_ERROR_RATE', 'ADCS_TMP',
                     'CMG0_TMP', 'CMG1_TMP', 'CMG2_TMP', 'CMG3_TMP', 'FSS_TMP1',
                     'FSS_TMP2', 'FSS_TMP3', 'SV', 'MAG', 'MAG_MAT', 'MAG_VEC',
                     'MAG_STAT', 'MAG0_S', 'MAG1_S', 'MAG2_S', 'MAG3_S', 'MAG4_S',
                     'MAG5_S', 'FSS_STAT', 'FSS0_SV', 'FSS0_PDSUM', 'FSS1_SV',
                     'FSS1_PDSUM', 'FSS2_SV', 'FSS2_PDSUM', 'FSS3_SV', 'FSS3_PDSUM',
                     'FSS4_SV', 'FSS4_PDSUM', 'FSS5_SV', 'FSS5_PDSUM', 'IMU_STAT',
                     'IMU0_S', 'IMU1_S', 'IMU2_S', 'IMU3_S', 'STR_STAT', 'STR0_S',
                     'STR1_S', 'CSS', 'CMG_STAT', 'CMG0_G_ANGLE', 'CMG0_W_RATE',
                     'CMG1_G_ANGLE', 'CMG1_W_RATE', 'CMG2_G_ANGLE', 'CMG2_W_RATE',
                     'CMG3_G_ANGLE', 'CMG3_W_RATE', 'MTQ', 'CMG0_G_RATE', 'CMG0_W_ACC',
                     'CMG1_G_RATE', 'CMG1_W_ACC', 'CMG2_G_RATE', 'CMG2_W_ACC',
                     'CMG3_G_RATE', 'CMG3_W_ACC', 'MASS', 'INE_TEN', 'POS_HB_B', 'ORIEN_HB', 'MAG_INFO',
                     'MAG0_ORIEN_BS', 'MAG1_ORIEN_BS', 'MAG2_ORIEN_BS', 'MAG3_ORIEN_BS',
                     'MAG4_ORIEN_BS', 'MAG5_ORIEN_BS', 'FSS_INFO', 'FSS0_ORIEN_BS', 'FSS1_ORIEN_BS',
                     'FSS2_ORIEN_BS', 'FSS3_ORIEN_BS', 'FSS4_ORIEN_BS', 'FSS5_ORIEN_BS', 'IMU_INFO',
                     'IMU0_ORIEN_BS', 'IMU1_ORIEN_BS', 'IMU2_ORIEN_BS', 'IMU3_ORIEN_BS', 'STR_INFO',
                     'STR0_ORIEN_BS', 'STR1_ORIEN_BS', 'NVM']
        
        self.test_TLE = '1 41917U 17003A   25268.25419250 -.00000030  00000+0 -17901-4 0  99902 41917  86.4034 199.2004 0002404  75.3595 284.7868 14.34217625455235'
        self.blank_TLE = '0 000000 000000   00000000000000 0000000000  0000000 00000000 0  00000 00000  0000000 00000000 0000000  0000000 00000000 00000000000000000'
        
        if par == 'NONE':
            serial_parity = serial.PARITY_NONE
        else:
            serial_parity = serial.PARITY_NONE
            print('Parity warning')
        if stop == 1:
            serial_stop = serial.STOPBITS_ONE
        else:
            serial_stop = serial.STOPBITS_ONE
            print('Stop warning!')    
        self.ser = serial.Serial(port,
                                 baudrate=baud,
                                 parity= serial_parity,
                                 stopbits = serial_stop,
                                 timeout = 1)
    
    def gen_cmd(self, cmd_type, reg_info, message = None):
        if cmd_type == "read":
            byte_1 = 0xC9
            byte_2 = reg_info[0]
            byte_3 = reg_info[1]
            byte_4 = reg_info[2]<<4+reg_info[3]
            byte_5 = (0xFF - (byte_1 + byte_2 + byte_3 + byte_4)%256 + 0x01)%256
        
            command = f'{byte_1:02x}' + ' ' + f'{byte_2:02x}' + ' ' + f'{byte_3:02x}' + ' ' + f'{byte_4:02x}'+ ' ' + f'{byte_5:02x}'
    
        if cmd_type == "write":
            byte_1 = 0xC8
            byte_2 = reg_info[0]
            byte_3 = reg_info[1]
            byte_4 = reg_info[2]<<4+reg_info[3]
            byte_5 = (0xFF - (byte_1 + byte_2 + byte_3 + byte_4 + message[1])%256 + 0x01)%256
        
            command = f'{byte_1:02x}' + ' ' + f'{byte_2:02x}' + ' ' + f'{byte_3:02x}' + ' ' + f'{byte_4:02x}'+ ' ' + f'{message[0]}'+ ' ' + f'{byte_5:02x}'
        
        return command
    
    def gen_message(self, value, message_type):
        if message_type == "float":
            value_length = len(value)
            final_message = ''
            full_message = ''
            cksum = 0
            for val in range(value_length):
                
                message = '0x'+hex(int.from_bytes(struct.pack('<f',value[val]), byteorder='little'))[2:].zfill(8)
                i = len(message)-2
                while i>0:
                    final_message = f'{final_message}' +' '+ f'{message[i:i+2].zfill(2)}'
                    cksum = cksum + int(message[i:i+2].zfill(2),16)
                    i = i-2
            full_message = f'{full_message}' + f'{final_message}'
            final_message = [full_message[1:], cksum]
        if message_type == "time":
            hour = str(time.gmtime().tm_hour).zfill(2)
            minute = str(time.gmtime().tm_min).zfill(2)
            second = str(time.gmtime().tm_sec).zfill(2)
            cksum = int('0x'+second,16) + int('0x'+minute,16) + int('0x'+hour,16)
            time_message = f'00 {second} {minute} {hour}'
            final_message = [time_message, cksum]
        if message_type == "date":
            year = str(time.gmtime().tm_year).zfill(2)
            month = str(time.gmtime().tm_mon).zfill(2)
            day = str(time.gmtime().tm_mday).zfill(2)
            wday = str(time.gmtime().tm_wday).zfill(2)
            cksum = int('0x'+year,16) + int('0x'+month,16) + int('0x'+day,16) + int('0x'+wday,16)
            time_message = f'{wday} {day} {month} {year[2:4]}'
            final_message = [time_message, cksum]
        if message_type == "TLE":
            string = ''
            cksum = 0
            for i in range(len(value)):
                char = value[i].encode('utf-8').hex()
                string = f'{char} {string}'
                cksum = cksum + int('0x'+char,16)
            final_message = [f'00 00 {string[:-1]}', cksum]
        return final_message
    
    def send_command(self, cmd):
        command = bytes.fromhex(cmd)
        count = self.ser.write(command)
#         print("\033[0m[RPI] sending command: "+cmd)
        time.sleep(.01)
        #msg = (self.ser.readline()).decode('ascii')
        return True

    def read_data(self):
        try:
            received_data = self.ser.read(1000)
#             print("Received data ",f'{received_data}')
#             print(f"bits?: {received_data:b}")
            #cksum = np.sum(received_data)
            #print("Checksum is ",cksum)
            cksum = 0x00
            for i in received_data:
                #print(f'{i:02x}')
                cksum = cksum + i
#             print('Checksum', cksum%256)
            if cksum%256 == 0x00:
                return received_data
            else:
                print("Receive ERROR!")
                return 0x00
        except:
            print("Receive ERROR!")
            return 0x00
       
    def bits_to_hex_string(self, bits):
        imax = len(bits)*2
        i = imax
        hex_string = ''
        bits_as_bits = bin(int.from_bytes(bits, byteorder='little'))[2:].zfill(5*imax)
        in_hex_temp = hex(int(bits_as_bits, 2))

        while i > 0:
            hex_string = f'{hex_string}' +' '+ f'{in_hex_temp[i:i+2].zfill(2)}'
            i = i - 2
        return hex_string
    
    def proc_raw(self,data, data_table):
#         print("Raw Data:", data)
        string_length = len(data)
#         print("Raw Data:",bits_to_hex_string(data))
#         print("String Length", string_length)
        string_message_only = data[4 : string_length - 1]
#         print("Message only: ",string_message_only)
        proc_data = "Empty"
        
        if data_table[4] == "float":
            i = 0
            imax = len(string_message_only)
            proc_data = []
            while i < imax:
                proc_data_temp = struct.unpack('<f',string_message_only[i:i+4])
                proc_data.append(proc_data_temp)
                i = i + 4
#             print("Processed data is: ",proc_data)
            return proc_data
        
        if data_table[4] == "char":
            if data_table[0] == 17: # Processes TLE character strings
                proc_data = ''
                for i in range(len(string_message_only)):
                    temp = str(string_message_only[(len(string_message_only)-i-1):(len(string_message_only)-i)] ,'ascii')
                    proc_data = f'{proc_data}' + f'{temp}'
                proc_data = proc_data.replace('\x00' , '')
            else: # Processes all other character strings, only SNID should be relevant
                proc_data = str(string_message_only, encoding='ascii')
            return proc_data
        
        if data_table[4] == "uint16" or data_table[4] == "int16":
            proc_data_0 = int.from_bytes(string_message_only[0:2], byteorder='little')
            proc_data_1 = int.from_bytes(string_message_only[2:4], byteorder='little')
            proc_data = [proc_data_0,proc_data_1]
#             print("Processed data is: ",proc_data)
            return proc_data
        
        if data_table[4] == "uint8":
            proc_data = '0b'+bin(int.from_bytes(string_message_only, byteorder='little'))[2:].zfill(32)
#             print(f"Processed data is: {proc_data}")
            return proc_data
        
        print("Format not supported.")
        
    def user_data(self,proc_data,register,names):
        if register[2] == 0 and register[0] == 5:
            print(names,": ",int(proc_data[2:6],2),int(proc_data[6:10],2),":",
                  int(proc_data[10:14],2),int(proc_data[14:18],2),":",
                  int(proc_data[18:22],2),int(proc_data[22:26],2), sep = "", end = '\r\n')
        elif register[2] == 0 and register[0] == 6:
            print(names,": ","Year: ", int(proc_data[2:6],2),int(proc_data[6:10],2), # Year
                  " ,Month: ", int(proc_data[10:14],2),int(proc_data[14:18],2), # Month
                  " ,Day: ", int(proc_data[18:22],2),int(proc_data[22:26],2), # Day
                  " ,Weekday: ", int(proc_data[30:34],2), sep = "", end = '\r\n') # Week
        elif register[2] == 0 and register[0] == 128:
            print(names,": ",proc_data, sep = "", end = '\r\n')
        else:
            print(names,": ",proc_data, sep = "", end = '\r\n')
    
    def close(self):
        if self.ser != None:
            self.ser.close()
             
def main():
    ppm = TAD102063('/dev/ttyUSB0',115200,'NONE',1)
    counter = 0
    try:
        raw_data_list = []
        counter = 0
        for register in ppm.user:
            ppm.send_command(ppm.gen_cmd("read", register))
            time.sleep(.001)
            raw_data = ppm.read_data()
            raw_data_list.append(raw_data)
            proc_data = ppm.proc_raw(raw_data, register)
            ppm.user_data(proc_data,register,ppm.names[counter])
            counter += 1
#         while True:
#             ppm.read_data()
#             time.sleep(.01)
#             if counter == 5:
#                 ppm.send_command(ppm.gen_cmd("read", ppm.MAG))
#                 counter = 0
#                 time.sleep(.01)
#             else:
#                 counter+=1
    except KeyboardInterrupt:
            print("Read finished")
    ppm.close()
if __name__ == "__main__":
    main()