from crc import Calculator, Crc16
import time
import serial

FRAME_MAX_SIZE = 255
    
FEND  = 0xC0
FESC  = 0xDB
TFEND = 0xDC
TFESC = 0xDD

WAIT_FEND = 0
IN_FRAME = 1
IN_ESCAPE = 2

class CommandManager():
    
    crcalc = Calculator(Crc16.XMODEM)
    node_lbl_to_id = { 'NONE': 0, 'LPPM': 1, 'EPS': 2, 'UPPM': 3, 'HOLONAV': 4, 'ASTROBOARD': 5, 'GS': 6, 'FTDI': 7 }
    node_id_to_lbl = { v: k for k, v in node_lbl_to_id.items() }
    ptype_lbl_to_id = { 'NONE': 0, 'REQ': 1, 'RES': 2, 'ACK': 3, 'RES': 4 }
    ptype_id_to_lbl = { v: k for k, v in ptype_lbl_to_id.items() }

    def __init__(self, node: int, serial):
        self.frame = bytearray()
        self.state = WAIT_FEND
        self.node = node
        self.serial = serial

    def create_cmd(self, orgn: int, dest: int, echo: int, ptype: int, id: str, args: str):
        msg_data = [ orgn, dest, echo, ptype, len(id), len(args), id, 0, args, 0 ]

        msg_ba = bytearray()
        for d in msg_data:
            if isinstance(d, int): msg_ba.append(d & 0xFF)
            elif isinstance(d, str): msg_ba.extend(d.encode('ascii'))
            elif isinstance(d, (bytes, bytearray)): msg_ba.extend(d)
        crc16 = self.crcalc.checksum(msg_ba)
        msg_ba.extend(crc16.to_bytes(2, byteorder='little', signed=False))
        
        kiss_ba = bytearray()
        for b in msg_ba:
            if (b == FEND): kiss_ba.extend(b'\xDB\xDC')
            elif (b == FESC): kiss_ba.extend(b'\xDB\xDD')
            else: kiss_ba.append(b)

        pkt_ba = bytearray()
        pkt_ba.extend(b'\xC0\x00')
        pkt_ba.extend(kiss_ba)
        pkt_ba.extend(b'\xC0')
        return pkt_ba 

    def send_cmd_serial(self, orgn: int, dest: int, echo: int, ptype: int, id: str, args: str):
        if not (self.serial and self.serial.is_open): return (bytearray(), 0)
        ba = self.create_cmd(orgn, dest, echo, ptype, id, args)
        cnt = self.serial.write(ba)
        time.sleep(0.01)
        return (ba, cnt)

    def kiss_process_byte(self, byte: int):
        if self.state == WAIT_FEND:
            if byte == FEND:
                self.frame.clear()
                self.state = IN_FRAME
        elif self.state == IN_FRAME:
            if byte == FEND:
                if len(self.frame) > 0:
                    return True 
            elif byte == FESC:
                self.state = IN_ESCAPE
            else:
                if len(self.frame) < FRAME_MAX_SIZE:
                    self.frame.append(byte)
                else:
                    # overflow -> drop frame
                    self.frame.clear()
                    self.state = WAIT_FEND
        elif self.state == IN_ESCAPE:
            if byte == TFEND:
                self.frame.append(FEND)
                self.state = IN_FRAME
            elif byte == TFESC:
                self.frame.append(FESC)
                self.state = IN_FRAME
            else:
                # invalid escape -> drop frame
                self.frame.clear()
                self.state = WAIT_FEND
        return False 

    def parse_frame(self): 
        exp_msg_len = len(self.frame) - 1
        if exp_msg_len < 10: return None
        p = {}
        buf = self.frame[1:]
        size = 0

        p['orgn'] = buf[size]
        size += 1
        p['dest'] = buf[size]
        size += 1
        p['echo'] = buf[size]
        size += 1
        p['ptype'] = buf[size]
        size += 1
        p['id_len'] = buf[size]
        size += 1
        p['args_len'] = buf[size]
        size += 1

        if size + p['id_len'] > exp_msg_len: return None
        p['id'] = buf[size:size+p['id_len']].decode('ascii')
        size += p['id_len'] + 1
        
        if size + p['args_len'] > exp_msg_len: return None
        p['args'] = buf[size:size+p['args_len']].decode('ascii')
        size += p['args_len'] + 1

        if size + 2 > exp_msg_len: return None
        p['crc'] = buf[size] | (buf[size+1] << 8)
        size += 2

        p['size'] = size
        return p if size == len(self.frame) - 1 else None

    def cleanup_frame(self) -> None:
        self.frame.clear()
        self.state = IN_FRAME

    # Reads a single command frame out of a serial stream if available
    def parse_stream(self):
        if not (self.serial and self.serial.is_open): return None
        n_bytes = self.serial.in_waiting

        for _ in range(n_bytes):
            b = int.from_bytes(self.serial.read(1))
            if self.kiss_process_byte(b):
                p = self.parse_frame()

                if p is None:
                    self.cleanup_frame()
                    return None
                
                if p['dest'] != self.node:
                    # print(f'bad dest {p}')
                    self.cleanup_frame()
                    return None

                crc_calc = self.crcalc.checksum(self.frame[1:1+p['size']-2])
                if p['crc'] != crc_calc:
                    self.cleanup_frame()
                    return None
                
                self.cleanup_frame()
                return p 
                
        return None

    # Reads a single command out of an arbitrary input buffer 
    def parse_ba(self, ba):
        for i in range(len(ba)):
            b = ba[i]
            if self.kiss_process_byte(b):
                p = self.parse_frame()

                if p is None:
                    self.cleanup_frame()
                    return None
               
                if p['dest'] != self.node:
                    self.cleanup_frame()
                    return None

                crc_calc = self.crcalc.checksum(self.frame[1:1+p['size']-2])
                if p['crc'] != crc_calc:
                    self.cleanup_frame()
                    return None
                
                self.cleanup_frame()
                return p 
                
        return None
