import serial
import time
from datetime import datetime
import traceback
import threading
import sys
import os
from mcp import MCPManager
from crc import Calculator, Crc16
from prompt_toolkit.application import Application
from prompt_toolkit.layout import Layout, HSplit, Window, ScrollablePane
from prompt_toolkit.layout.controls import FormattedTextControl
from prompt_toolkit.layout.dimension import Dimension
from prompt_toolkit.widgets import TextArea
from prompt_toolkit.formatted_text import ANSI, to_formatted_text
from prompt_toolkit.key_binding import KeyBindings
from prompt_toolkit.keys import Keys
from prompt_toolkit.data_structures import Point
from prompt_toolkit.formatted_text import merge_formatted_text
from prompt_toolkit.shortcuts import print_formatted_text
from prompt_toolkit.mouse_events import MouseEventType
from prompt_toolkit.layout.utils import explode_text_fragments
from prompt_toolkit.utils import get_cwidth

SCROLL_SPEED = 3
MAX_LOGS = 10000

KNRM = "\033[0m"
KRED = "\033[31m"
KGRN = "\033[32m"
KYEL = "\033[33m"
KBLU = "\033[34m"
KMAG = "\033[35m"
KCYN = "\033[36m"
KWHT = "\033[37m"

ftdi = serial.Serial(sys.argv[1], baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=1)
mcpmgr = MCPManager(ftdi)

log_lines = []
log_wrapped_rows = []
log_row_heights = []
layout_width = None
scroll_top = 0  # row index into the wrapped log view
is_manual_scrolling = False
is_paused = False
is_recording = False
recording_file = None
pkt_hist = []
i_ch = 0
log_total_rows = 0
log_lock = threading.RLock()


def terminal_width() -> int:
    return max(1, os.get_terminal_size().columns)


def visible_height() -> int:
    # Keep one row for the input box.
    return max(1, os.get_terminal_size().lines - 1)


def wrap_record_to_rows(text: str, width: int):
    """Wrap one ANSI-formatted record into terminal rows, preserving styles."""
    fragments = explode_text_fragments(to_formatted_text(ANSI(text)))
    rows = []
    current_row = []
    current_width = 0
    saw_anything = False

    for fragment in fragments:
        style = fragment[0]
        ch = fragment[1]
        saw_anything = True

        if ch == "\r":
            continue

        if ch == "\n":
            rows.append(current_row)
            current_row = []
            current_width = 0
            continue

        ch_width = max(0, get_cwidth(ch))

        if ch_width > 0 and current_row and current_width + ch_width > width:
            rows.append(current_row)
            current_row = []
            current_width = 0

        current_row.append((style, ch))
        current_width += ch_width

    if current_row or not rows or not saw_anything:
        rows.append(current_row)

    return rows


def rebuild_wrapped_cache(width: int) -> None:
    global layout_width, log_wrapped_rows, log_row_heights, log_total_rows

    log_wrapped_rows = [wrap_record_to_rows(record, width) for record in log_lines]
    log_row_heights = [len(rows) for rows in log_wrapped_rows]
    log_total_rows = sum(log_row_heights)
    layout_width = width


def ensure_wrapped_cache() -> None:
    width = terminal_width()
    global layout_width
    with log_lock:
        if layout_width != width:
            rebuild_wrapped_cache(width)


def visible_scroll_bottom() -> int:
    return max(0, log_total_rows - visible_height())


def build_visible_log_fragments():
    ensure_wrapped_cache()

    with log_lock:
        start_row = max(0, min(scroll_top, visible_scroll_bottom()))
        end_row = min(log_total_rows, start_row + visible_height())

        fragments = []
        current_row_index = 0
        first_rendered_row = True

        for record_rows in log_wrapped_rows:
            for row in record_rows:
                if current_row_index >= end_row:
                    return fragments

                if current_row_index >= start_row:
                    if not first_rendered_row:
                        fragments.append(("", "\n"))
                    fragments.extend(row if row else [("", "")])
                    first_rendered_row = False

                current_row_index += 1

        return fragments


log_ctrl = FormattedTextControl(
    text=lambda: build_visible_log_fragments(),
    focusable=False,
)
log_win = Window(content=log_ctrl, wrap_lines=False)
input_box = TextArea(height=1, prompt="> ", multiline=False)
kb = KeyBindings()
layout = Layout(HSplit([log_win, input_box]), focused_element=input_box)
app = Application(layout=layout, key_bindings=kb, full_screen=True, mouse_support=True)


@kb.add("enter")  # Submit input
def _(event):
    global i_ch, pkt_hist
    pktstr = input_box.text.strip()
    pkt_hist.append(pktstr)
    i_ch = len(pkt_hist)
    try:
        send_pkt_str(pktstr)
    except Exception:
        log_error(f'invalid pkt "{pktstr}": {traceback.format_exc()}')
    finally:
        input_box.text = ""
        app.invalidate()


@kb.add("c-w")  # Scroll up
def _(event):
    global is_manual_scrolling, scroll_top
    is_manual_scrolling = True
    scroll_top = max(0, scroll_top - SCROLL_SPEED)
    app.invalidate()


@kb.add("c-s")  # Scroll down
def _(event):
    global is_manual_scrolling, scroll_top
    ensure_wrapped_cache()
    bottom = visible_scroll_bottom()
    scroll_top = min(bottom, scroll_top + SCROLL_SPEED)
    is_manual_scrolling = (scroll_top != bottom)
    app.invalidate()


@kb.add("c-b")  # Go to beginning of log (least recent)
def _(event):
    global is_manual_scrolling, scroll_top
    scroll_top = 0
    is_manual_scrolling = True
    app.invalidate()


@kb.add("c-e")  # Go to end of log (most recent)
def _(event):
    global is_manual_scrolling, scroll_top
    ensure_wrapped_cache()
    scroll_top = visible_scroll_bottom()
    is_manual_scrolling = False
    app.invalidate()


@kb.add("c-p")  # Pause/resume logging toggle
def _(event):
    global is_paused
    if is_paused:
        is_paused = False
        log_info(f"logging resumed")
    else:
        log_info(f"logging paused")
        is_paused = True


@kb.add("c-r")  # Start/stop recording toggle
def _(event):
    global is_recording, recording_file
    if is_recording:
        log_info(f"stopped recording")
        is_recording = False
        recording_file.close()
    else:
        is_recording = True
        if not os.path.exists("logs/"):
            os.mkdir("logs/")
        timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        recording_file = open(f"logs/log_{timestamp}.txt", "w")
        log_info(f"started recording")


@kb.add("up")  # Backward in command history
def _(event):
    global i_ch, pkt_hist
    if len(pkt_hist) == 0:
        return
    i_ch = max(i_ch - 1, 0)
    input_box.text = pkt_hist[i_ch]
    input_box.buffer.cursor_position = len(input_box.text)
    app.invalidate()


@kb.add("down")  # Forward in command history
def _(event):
    global i_ch, pkt_hist
    if len(pkt_hist) == 0:
        return
    i_ch = min(i_ch + 1, len(pkt_hist) - 1)
    input_box.text = pkt_hist[i_ch]
    input_box.buffer.cursor_position = len(input_box.text)
    app.invalidate()


@kb.add("c-c")  # Exit
@kb.add("c-q")
def _(event):
    event.app.exit()


def string_to_binary(text):
    return ''.join(format(ord(char), '08b') for char in text)


def bytes_to_mixed_ascii_hex(data):
    out = []
    for b in data:
        if 32 <= b <= 126:  # printable ASCII range
            out.append(chr(b))
        else:
            out.append(f"\\x{b:02x}")
    return ''.join(out)


def log(text):
    global log_lines, log_wrapped_rows, log_row_heights, log_total_rows, scroll_top
    global is_paused, is_recording, recording_file, is_manual_scrolling

    if is_paused:
        return

    width = terminal_width()

    with log_lock:
        if layout_width != width:
            rebuild_wrapped_cache(width)

        # log_lines.append(string_to_binary(text))
        log_lines.append(text)
        rows = wrap_record_to_rows(text, width)
        log_wrapped_rows.append(rows)
        log_row_heights.append(len(rows))
        log_total_rows += len(rows)

        if len(log_lines) > MAX_LOGS:
            trim_count = MAX_LOGS // 5
            removed_rows = sum(log_row_heights[:trim_count])
            del log_lines[:trim_count]
            del log_wrapped_rows[:trim_count]
            del log_row_heights[:trim_count]
            log_total_rows -= removed_rows
            scroll_top = max(0, scroll_top - removed_rows)

        bottom = visible_scroll_bottom()
        if not is_manual_scrolling:
            scroll_top = bottom
        else:
            scroll_top = min(scroll_top, bottom)

    if is_recording and recording_file is not None:
        recording_file.write(text)
        recording_file.flush()
    app.invalidate()


def epoch_time_ms():
    return round(time.time() * 1000)


def log_trace(msg):
    log(f"{KNRM}{epoch_time_ms()} [TRACE] [FTDI] {msg}\n")


def log_debug(msg):
    log(f"{KWHT}{epoch_time_ms()} [DEBUG] [FTDI] {msg}\n")


def log_info(msg):
    log(f"{KCYN}{epoch_time_ms()} [INFO] [FTDI] {msg}\n")


def log_warn(msg):
    log(f"{KYEL}{epoch_time_ms()} [WARN] [FTDI] {msg}\n")


def log_error(msg):
    log(f"{KRED}{epoch_time_ms()} [ERROR] [FTDI] {msg}\n")


def log_res(msg):
    log(f"{KMAG}{epoch_time_ms()} [RES] [FTDI] {msg}\n")


def read_serial():
    while True:
        try:
            p = mcpmgr.parse_stream()
            if p is not None:
                if p['id'] == 'ftdi_log':
                    log(p['args'])
                else:
                    log_res(f"{p}")
        except KeyboardInterrupt:
            log_info('read_serial: quitting')
            if ftdi.is_open:
                ftdi.close()
            break
        except Exception:
            log_error(f'read_serial: {traceback.format_exc()}')


def send_pkt_str(pktstr):
    pktstr = pktstr.strip()
    spl = pktstr.split(' ')
    ba, cnt = mcpmgr.send_pkt_serial(
            int(spl[0]), int(spl[1]), int(spl[2]), int(spl[3]),
            spl[4], ' '.join(spl[5:]) if len(spl) > 5 else ''
    )
    log_info(f"sending pkt: cnt={cnt} [ {bytes_to_mixed_ascii_hex(ba)} ]")


if __name__ == "__main__":
    rx_thread = threading.Thread(target=read_serial, daemon=True)
    rx_thread.start()
    try:
        app.run()
    except KeyboardInterrupt:
        log_info('main: quitting')
        if ftdi.is_open:
            ftdi.close()
    except Exception:
        log_error(f'main: {traceback.format_exc()}')
    finally:
        if ftdi.is_open:
            ftdi.close()
