from crc import Calculator, Crc16
import time
import serial

FEND = 0xC0
DATA_FRAME = 0x00
REQUEST = 0 
RESPONSE = 1

crcalc = Calculator(Crc16.XMODEM)

def create_cmd(orgn: int, dest: int, echo: int, ptype: int, id: str, args: str) -> bytearray:
    data = [ FEND, DATA_FRAME, orgn, dest, echo, ptype, len(id), len(args), id, 0, args, 0 ]
    ba = bytearray()
    for d in data:
        if isinstance(d, int): ba.append(d & 0xFF)
        elif isinstance(d, str): ba.extend(d.encode('ascii'))
        elif isinstance(d, (bytes, bytearray)): ba.extend(d)
    crc16 = crcalc.checksum(ba)
    ba.extend(crc16.to_bytes(2, byteorder='little', signed=False))
    ba.append(FEND & 0xFF)
    return ba

def send_cmd(serial: serial.Serial, orgn: int, dest: int, echo: int, ptype: int, id: str, args: str) -> tuple:
    if not (serial and serial.is_open): return (bytearray(), 0)
    ba = create_cmd(orgn, dest, echo, ptype, id, args)
    cnt = serial.write(ba)
    time.sleep(0.01)
    return (ba, cnt)

def read_cmd(serial: serial.Serial) -> tuple | None:
    pass
    # error_cmd = (False, 0, 0, 0, '', '')
    # if not (serial and serial.is_open and serial.in_waiting == 0): 
    #     return error_cmd
    # raw_cmd = serial.read_until(FEND)
    # if raw_cmd[0] != 0xCD:
    #     return error_cmd


    
    

