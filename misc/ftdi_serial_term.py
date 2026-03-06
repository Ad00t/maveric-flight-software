import serial
import time
import traceback
import threading
import sys
import os
import commands
from crc import Calculator, Crc16
from prompt_toolkit.application import Application
from prompt_toolkit.layout import Layout, HSplit, Window, ScrollablePane
from prompt_toolkit.layout.controls import FormattedTextControl
from prompt_toolkit.layout.dimension import Dimension
from prompt_toolkit.widgets import TextArea
from prompt_toolkit.formatted_text import ANSI
from prompt_toolkit.key_binding import KeyBindings
from prompt_toolkit.keys import Keys
from prompt_toolkit.data_structures import Point
from prompt_toolkit.formatted_text import to_formatted_text, merge_formatted_text
from prompt_toolkit.shortcuts import print_formatted_text
from prompt_toolkit.mouse_events import MouseEventType

SCROLL_SPEED = 3
MAX_LOGS = 10000

ftdi = serial.Serial(sys.argv[1], baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=1)

log_lines = []
is_manual_scrolling = False
scroll_top = 0
is_paused = False
cmd_hist = []
i_ch = 0

log_ctrl = FormattedTextControl(
    text=lambda: to_formatted_text(ANSI("".join(log_lines[scroll_top:scroll_top+os.get_terminal_size().lines]))),
    focusable=False
)
log_win = Window(content=log_ctrl, wrap_lines=True)
input_box = TextArea(height=1, prompt="> ", multiline=False)
kb = KeyBindings()
layout = Layout(HSplit([log_win, input_box]), focused_element=input_box)
app = Application(layout=layout, key_bindings=kb, full_screen=True, mouse_support=True)

@kb.add("enter") # Submit input
def _(event):
    global i_ch, cmd_hist
    cmd = input_box.text.strip()
    cmd_hist.append(cmd)
    i_ch = len(cmd_hist)
    try:
        send_command_str(cmd)
    except Exception as e:
        log(f'\033[31m[RPI] [ERROR] invalid command "{cmd}": {traceback.format_exc()}\r\n')
    finally:
        input_box.text = ""
        app.invalidate()
        
@kb.add("c-w") # Scroll up
def _(event):
    global is_manual_scrolling, log_lines, scroll_top
    is_manual_scrolling = True
    scroll_top = max(0, scroll_top-SCROLL_SPEED)
    
@kb.add("c-s") # Scroll down
def _(event):
    global is_manual_scrolling, log_lines, scroll_top
    scroll_top = min(max(0, len(log_lines)-os.get_terminal_size().lines + 1), scroll_top+SCROLL_SPEED)
    is_manual_scrolling = (scroll_top != max(0, len(log_lines)-os.get_terminal_size().lines))
        
@kb.add("c-b") # Go to beginning of log (least recent)
def _(event):
    global is_manual_scrolling, scroll_top
    scroll_top = 0
    is_manual_scrolling = True
    
@kb.add("c-e") # Go to end of log (most recent)
def _(event):
    global is_manual_scrolling, scroll_top, log_lines
    scroll_top = max(0, len(log_lines)-os.get_terminal_size().lines + 1)
    is_manual_scrolling = False
    
@kb.add("c-p") # Pause/resume toggle
def _(event):
    global is_paused
    if is_paused:
        is_paused = False
        log(f"\033[0m[RPI] logging resumed\r\n")
    else:
        log(f"\033[0m[RPI] logging paused\r\n")
        is_paused = True
        
@kb.add("up") # Backward in command history
def _(event):
    global i_ch, cmd_hist
    if len(cmd_hist) == 0: return
    i_ch = max(i_ch - 1, 0)
    input_box.text = cmd_hist[i_ch]
    input_box.buffer.cursor_position = len(input_box.text)
    app.invalidate()

@kb.add("down") # Forward in command history
def _(event):
    global i_ch, cmd_hist
    if len(cmd_hist) == 0: return
    i_ch = min(i_ch + 1, len(cmd_hist) - 1)
    input_box.text = cmd_hist[i_ch]
    input_box.buffer.cursor_position = len(input_box.text)
    app.invalidate()

@kb.add("c-c") # Exit
@kb.add("c-q")
def _(event):
    event.app.exit()
    
def string_to_binary(text):
    return ''.join(format(ord(char), '08b') for char in text)

def log(text):
    global log_text, log_lines, scroll_top, is_paused
    if is_paused: return
    # log_lines.append(string_to_binary(text))
    log_lines.append(text)
    if not is_manual_scrolling and len(log_lines) >= os.get_terminal_size().lines:
        scroll_top = len(log_lines) - os.get_terminal_size().lines + 1
    if len(log_lines) > MAX_LOGS:
        del log_lines[:MAX_LOGS//5]
        scroll_top = max(0, scroll_top-MAX_LOGS//5 + 1)
    app.invalidate()

def read_serial():
    while True:
        try:
            if ftdi and ftdi.is_open and ftdi.in_waiting > 0:
                line = ftdi.readline().decode('ascii', errors='replace')
                if len(line) > 0:
                    log(line) 
        except KeyboardInterrupt:
            log('\033[0m[RPI] [INFO] read_serial: quitting\r\n')
            if ftdi.is_open: 
                ftdi.close()
            break
        except Exception as e:
            log(f'\033[31m[RPI] [ERROR] read_serial: {traceback.format_exc()}\r\n')
            
def send_command_str(cmdstr):
    cmdstr = cmdstr.strip()
    spl = cmdstr.split(' ')
    ba, cnt = commands.send_cmd_serial(ftdi, int(spl[0]), int(spl[1]), int(spl[2]), spl[3], spl[4], ' '.join(spl[5:]))
    log(f"\033[0m[RPI] [INFO] sending cmd: cnt={cnt} {repr(ba.decode('ascii', errors='replace'))}\r\n")

if __name__ == "__main__":      
    rx_thread = threading.Thread(target=read_serial, daemon=True)
    rx_thread.start()
    try:
        app.run()
    except KeyboardInterrupt:
        log('\033[0m[RPI] [INFO] main: quitting\r\n')
        if ftdi.is_open: 
            ftdi.close()
    except Exception as e:
        log(f'\033[31m[RPI] [ERROR] main: {traceback.format_exc()}\r\n')
    finally:
        if ftdi.is_open:
            ftdi.close()

    
