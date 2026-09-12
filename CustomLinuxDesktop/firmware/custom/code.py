import usb_hid
from hid.keyboard import Keyboard
from hid.keycode import Keycode
from hid.consumer_control import ConsumerControl
from hid.consumer_control_code import ConsumerControlCode
from hid.keyboard_layout_us import KeyboardLayoutUS
import board
import microcontroller
import digitalio
import time
import pwmio  #PWM模块


bl_pwm_value = 32767   		#中间值
ad_pwm_value = 32767       	#中间值
        
try:
    # Set up a keyboard device.
    kbd = Keyboard(usb_hid.devices)
    layout = KeyboardLayoutUS(kbd)
    consumer_control = ConsumerControl(usb_hid.devices)

except Exception as e:
    # 如果初始化失败，记录错误信息
    print(f"Initialization failed: {e}")
    # 复位RP2040
    microcontroller.reset()

# 初始化 GP22 引脚
gp22 = digitalio.DigitalInOut(board.GP22)
gp22.direction = digitalio.Direction.OUTPUT
# 设置 GP22 引脚为高电平
gp22.value = False

# 初始化 GP19 引脚
gp19 = digitalio.DigitalInOut(board.GP19)
gp19.direction = digitalio.Direction.OUTPUT
# 设置 GP19 引脚为高电平
gp19.value = False

gp21 = digitalio.DigitalInOut(board.GP21)
gp21.direction = digitalio.Direction.OUTPUT
# 设置 GP21 引脚为高电平
gp21.value = False

# 初始化 PWM 输出引脚 GP22 用于呼吸灯效果
# gp22_pwm = pwmio.PWMOut(board.GP22, frequency=5000, duty_cycle=0)

# 初始化BL PWM
BL_PWM_RP = pwmio.PWMOut(board.GP20, frequency=5000, duty_cycle=5000)

# 初始化AD PWM
AD_PWM_RP = pwmio.PWMOut(board.GP18, frequency=5000, duty_cycle=32700)

def toggle_gp22():
    gp22.value = not gp22.value
    print(f"GP22 state toggled to: {gp22.value}")
    kbd.press(Keycode.CAPS_LOCK)
    kbd.release(Keycode.CAPS_LOCK)

def start_breathing_light():
    tmp_time = 0
    stop_loop = True  # 用于跳出 while 循环的标志
    kbd = None  # 确保 kbd 被初始化
    gp22_temp = gp22.value #记录当前的大写键状态

    for i in range(1,10+1):
        gp22.value = not gp22.value
        # print("GP22 TEST:",gp22.value)
        time.sleep(0.3)

    try:
        while stop_loop:
            tmp_time +=1

            if tmp_time >= 255:
                gp22.value = not gp22.value
                # print("GP22 IS:",gp22.value)
                tmp_time = 0

            current_keys = set(scan_keyboard())

            # Handle keys that are pressed
            for key in current_keys:
                if key is not None:
                    time.sleep(0.2)
                    if key is not None:
                        if kbd:
                            kbd.release_all()

                        gp22.value = gp22_temp
                        gp21.value = not gp21.value
                        stop_loop = False  # 设置标志
                        break
    finally:
        if kbd:
            kbd.release_all()
            gp22.value = gp22_temp


def toggle_gp19():
    gp19.value = not gp19.value
    # print(f"GP19 state toggled to: {gp19.value}")

def toggle_gp21():
    gp21.value = not gp21.value
    # print(f"GP21 state toggled to: {gp21.value}")
    start_breathing_light()

def ad_pwm_down():
    global ad_pwm_value
    if ad_pwm_value <= 0:
        ad_pwm_value = 0
    else:
        ad_pwm_value -= 6553
    ad_pwm_value = min(max(ad_pwm_value, 0), 65535)
    # 更新占空比
    AD_PWM_RP.duty_cycle = ad_pwm_value
    print("pwm_value == ", ad_pwm_value)

def ad_pwm_up():
    global ad_pwm_value
    if ad_pwm_value >= 65535:
        ad_pwm_value = 65535
    else:
        ad_pwm_value += 6553
    ad_pwm_value = min(max(ad_pwm_value, 0), 65535)
    # 更新占空比
    AD_PWM_RP.duty_cycle = ad_pwm_value
    print("pwm_value == ", ad_pwm_value)

#因为背光是用npn三极管下拉控制的，所以PWM占空比越高，背光越暗，所以下列pwm_up实际上是减少占空比，pwm_down是增加占空比
def bl_pwm_up():
    global bl_pwm_value
    if bl_pwm_value <= 0:
        bl_pwm_value = 0
    else:
        bl_pwm_value -= 6553
    bl_pwm_value = min(max(bl_pwm_value, 0), 65535)
    # 更新占空比
    BL_PWM_RP.duty_cycle = bl_pwm_value
    print("pwm_value == ",bl_pwm_value)
        
def bl_pwm_down():
    global bl_pwm_value
    if bl_pwm_value >= 65535:
        bl_pwm_value = 65535
    else:
        bl_pwm_value += 6553
    bl_pwm_value = min(max(bl_pwm_value, 0), 65535)
    # 更新占空比
    BL_PWM_RP.duty_cycle = bl_pwm_value
    print("pwm_value == ",bl_pwm_value)

def shift_left_bracket():
    kbd.press(Keycode.SHIFT, Keycode.LEFT_BRACKET)
    # kbd.release(Keycode.SHIFT)
    kbd.release_all()

def shift_right_bracket():
    kbd.press(Keycode.SHIFT, Keycode.RIGHT_BRACKET)
    # kbd.release(Keycode.SHIFT)
    kbd.release_all()

def shift_backslash():
    kbd.press(Keycode.SHIFT, Keycode.BACKSLASH)
    # kbd.release(Keycode.SHIFT)
    kbd.release_all()

def shift_grave_accent():
    kbd.press(Keycode.SHIFT, Keycode.GRAVE_ACCENT)
    # kbd.release(Keycode.SHIFT)
    kbd.release_all()

def lock_screen():
#锁屏(在linux上是休眠)
    kbd.press(Keycode.WINDOWS, Keycode.L)
    kbd.release_all()

def scan_previous_track():
    consumer_control.press(ConsumerControlCode.SCAN_PREVIOUS_TRACK)
    consumer_control.release()

def play_pause():
    consumer_control.press(ConsumerControlCode.PLAY_PAUSE)
    consumer_control.release()

def scan_next_track():
    consumer_control.press(ConsumerControlCode.SCAN_NEXT_TRACK)
    consumer_control.release()


# --- PocketTerm35 / TrailShell customisation --------------------------------
# The six face and shoulder buttons originally sent the plain letters
# l r x y b a. Every one of those letters also exists on the main QWERTY rows
# (L=row3col8, R=row2col3, X=row4col1, Y=row2col5, B=row4col4, A=row3col0), so
# the gamepad cluster was a set of duplicates that could never be used as
# shortcuts without stealing letters from typing.
#
# They now send F13-F18, which nothing else on this keyboard uses and which
# essentially no Linux application binds by default. Held with Fn they still
# send the original letters, so no capability is lost.
#
# These are raw USB HID usage IDs from the Keyboard/Keypad page (0x07), because
# this build's Keycode class does not define anything above F12.
F13 = 0x68
F14 = 0x69
F15 = 0x6A
F16 = 0x6B
F17 = 0x6C
F18 = 0x6D

# The D-pad is deliberately left as real arrow keys: it is the ONLY source of
# arrow keys on this keyboard, so remapping it would leave none at all.


# Define custom function keys using a dictionary
CUSTOM_KEYS = {
    # Add more custom keys as needed
    "FN_KEY"                : -100,
    "FN_MUTE"               : -101,
    "FN_VOLUME_DOWN"        : -102,
    "FN_VOLUME_UP"          : -103,
    "FN_LOCK_SCREEN"        : -104,
    "FN_BL_CONTROL_SCREEN"  : -105,
    "FN_BL_PWM_DOWN"        : -106,
    "FN_BL_PWM_UP"          : -107,
    "SHIFT_GRAVE_ACCENT"    : -108,
    "SHIFT_BACKSLASH"       : -109,
    "SHIFT_LEFT_BRACKET"    : -110,
    "SHIFT_RIGHT_BRACKET"   : -111
}

SPECIAL_KEY_FUNCTIONS = {
    CUSTOM_KEYS["FN_MUTE"]                  : toggle_gp19,
    CUSTOM_KEYS["FN_VOLUME_DOWN"]           : ad_pwm_down,
    CUSTOM_KEYS["FN_VOLUME_UP"]             : ad_pwm_up,
    CUSTOM_KEYS["FN_LOCK_SCREEN"]           : lock_screen,
    CUSTOM_KEYS["FN_BL_CONTROL_SCREEN"]     : toggle_gp21,
    CUSTOM_KEYS["FN_BL_PWM_DOWN"]           : bl_pwm_down,
    CUSTOM_KEYS["FN_BL_PWM_UP"]             : bl_pwm_up,
    CUSTOM_KEYS["SHIFT_GRAVE_ACCENT"]       : shift_grave_accent,
    CUSTOM_KEYS["SHIFT_BACKSLASH"]          : shift_backslash,
    CUSTOM_KEYS["SHIFT_LEFT_BRACKET"]       : shift_left_bracket,
    CUSTOM_KEYS["SHIFT_RIGHT_BRACKET"]      : shift_right_bracket,
    Keycode.CAPS_LOCK                       : toggle_gp22,
    ConsumerControlCode.SCAN_PREVIOUS_TRACK : scan_previous_track,
    ConsumerControlCode.PLAY_PAUSE          : play_pause,
    ConsumerControlCode.SCAN_NEXT_TRACK     : scan_next_track
}

# Define the number of rows and columns
NUM_ROWS = 7
NUM_COLS = 10

# Define the key map for the keyboard matrix
KEY_MAP = [
    #col0                       col1                                col2                                    col3                                col4                                        col5                                    col6                                    col7                                    col8                        col9                                        
    [Keycode.UP_ARROW,          Keycode.LEFT_ARROW,                 Keycode.DOWN_ARROW,                     Keycode.RIGHT_ARROW,                F13,                                        F14,                                    F15,                                    F16,                                    F17,                        F18],                   # row 0  (D-pad stays arrows; L R X Y B A -> F13-F18)
    [Keycode.ONE,               Keycode.TWO,                        Keycode.THREE,                          Keycode.FOUR,                       Keycode.FIVE,                               Keycode.SIX,                            Keycode.SEVEN,                          Keycode.EIGHT,                          Keycode.NINE,               Keycode.ZERO],          # row 1
    [Keycode.Q,                 Keycode.W,                          Keycode.E,                              Keycode.R,                          Keycode.T,                                  Keycode.Y,                              Keycode.U,                              Keycode.I,                              Keycode.O,                  Keycode.P],             # row 2
    [Keycode.A,                 Keycode.S,                          Keycode.D,                              Keycode.F,                          Keycode.G,                                  Keycode.H,                              Keycode.J,                              Keycode.K,                              Keycode.L,                  Keycode.BACKSPACE],     # row 3
    [Keycode.Z,                 Keycode.X,                          Keycode.C,                              Keycode.V,                          Keycode.B,                                  Keycode.N,                              Keycode.M,                              Keycode.FORWARD_SLASH,                  Keycode.ENTER,              None],                  # row 4
    [Keycode.TAB,               Keycode.CAPS_LOCK,                  Keycode.MINUS,                          Keycode.EQUALS,                     Keycode.SEMICOLON,                          Keycode.QUOTE,                          Keycode.COMMA,                          Keycode.PERIOD,                         Keycode.SHIFT,              None],                  # row 5
    [CUSTOM_KEYS["FN_KEY"],     Keycode.CONTROL,                    Keycode.LEFT_ALT,                       Keycode.PRINT_SCREEN,               Keycode.SPACE,                              Keycode.PAUSE,                          Keycode.RIGHT_ALT,                      Keycode.WINDOWS,                        CUSTOM_KEYS["FN_KEY"],      None]                   # row 6
]

# Define the FN key map for the keyboard matrix
FN_MAP = [
    #col0                       col1                                col2                                    col3                                col4                                        col5                                    col6                                    col7                                    col8                        col9                                        
    [Keycode.UP_ARROW,          Keycode.LEFT_ARROW,                 Keycode.DOWN_ARROW,                     Keycode.RIGHT_ARROW,                Keycode.L,                                  Keycode.R,                              Keycode.X,                              Keycode.Y,                              Keycode.B,                  Keycode.A],             # row 0
    [Keycode.F1,                Keycode.F2,                         Keycode.F3,                             Keycode.F4,                         Keycode.F5,                                 Keycode.F6,                             Keycode.F7,                             Keycode.F8,                             Keycode.F9,                 Keycode.F10],           # row 1
    [Keycode.ESCAPE,            CUSTOM_KEYS["FN_MUTE"],             CUSTOM_KEYS["FN_VOLUME_DOWN"],          CUSTOM_KEYS["FN_VOLUME_UP"],        ConsumerControlCode.SCAN_PREVIOUS_TRACK,    ConsumerControlCode.PLAY_PAUSE,         ConsumerControlCode.SCAN_NEXT_TRACK,    CUSTOM_KEYS["FN_LOCK_SCREEN"],          Keycode.F11,                Keycode.F12],           # row 2
    [Keycode.GRAVE_ACCENT,      CUSTOM_KEYS["SHIFT_GRAVE_ACCENT"],  Keycode.BACKSLASH,                      CUSTOM_KEYS["SHIFT_BACKSLASH"],     CUSTOM_KEYS["SHIFT_LEFT_BRACKET"],          CUSTOM_KEYS["SHIFT_RIGHT_BRACKET"],     Keycode.LEFT_BRACKET,                   Keycode.RIGHT_BRACKET,                  Keycode.L,                  Keycode.DELETE],        # row 3
    [Keycode.INSERT,            Keycode.HOME,                       CUSTOM_KEYS["FN_BL_CONTROL_SCREEN"],    Keycode.END,                        Keycode.PAGE_UP,                            Keycode.PAGE_DOWN,                      Keycode.SCROLL_LOCK,                    Keycode.FORWARD_SLASH,                  Keycode.ENTER,              None],                  # row 4
    [Keycode.TAB,               Keycode.CAPS_LOCK,                  CUSTOM_KEYS["FN_BL_PWM_DOWN"],          CUSTOM_KEYS["FN_BL_PWM_UP"],        Keycode.SEMICOLON,                          Keycode.QUOTE,                          Keycode.COMMA,                          Keycode.PERIOD,                         Keycode.SHIFT,              None],                  # row 5
    [CUSTOM_KEYS["FN_KEY"],     Keycode.CONTROL,                    Keycode.LEFT_ALT,                       Keycode.PRINT_SCREEN,               Keycode.SPACE,                              Keycode.PAUSE,                          Keycode.RIGHT_ALT,                      Keycode.WINDOWS,                        CUSTOM_KEYS["FN_KEY"],      None]                   # row 6
]

# Define the row and column pins
row_pins = [board.GP16, board.GP10, board.GP11, board.GP12, board.GP13, board.GP14, board.GP15]
col_pins = [board.GP0,  board.GP1,  board.GP2,  board.GP3,  board.GP4,  board.GP5,  board.GP6,  board.GP7,  board.GP8,  board.GP9]

# Initialize row and column pins
row_gpio = [digitalio.DigitalInOut(pin) for pin in row_pins]
col_gpio = [digitalio.DigitalInOut(pin) for pin in col_pins]

# Set rows as inputs with pull-up and columns as outputs set to low
for row in row_gpio:
    row.direction = digitalio.Direction.INPUT
    row.pull = digitalio.Pull.UP


for col in col_gpio:
    col.direction = digitalio.Direction.OUTPUT
    col.value = False

# Function to scan the keyboard matrix
def scan_keyboard():
    keys_pressed = []
    fn_active = False

    for col_index, col_pin in enumerate(col_gpio):
        col_pin.direction = digitalio.Direction.OUTPUT
        col_pin.value = True

        for row_index, row_pin in enumerate(row_gpio):
            row_pin.direction = digitalio.Direction.INPUT
            row_pin.pull = digitalio.Pull.DOWN

            if row_pin.value:  # Key is pressed
                time.sleep(0.05)  # Debounce delay
                if row_pin.value:
                    key = KEY_MAP[row_index][col_index]
                    if key == CUSTOM_KEYS["FN_KEY"]:
                        fn_active = True
                    keys_pressed.append((row_index, col_index))  # 记录按键位置

        col_pin.value = False  # Set the column back to low

    active_map = FN_MAP if fn_active else KEY_MAP

    translated_keys = []
    for row_index, col_index in keys_pressed:
        translated_key = active_map[row_index][col_index]
        translated_keys.append(translated_key)
        print("ROW", row_index, "COL", col_index)

    return translated_keys

previous_keys = set()

# Main loop
try:
    while True:
        current_keys = set(scan_keyboard())

        # Handle keys that were released
        for key in previous_keys - current_keys:
            if key is not None and key >= 0:
                kbd.release(key)

        # Handle keys that are pressed
        for key in current_keys - previous_keys:
            if key is not None:
                if key in SPECIAL_KEY_FUNCTIONS:
                    SPECIAL_KEY_FUNCTIONS[key]()
                elif key >= 0:  # 过滤掉负值键码
                    kbd.press(key)

        previous_keys = current_keys

        time.sleep(0.01)  # Small delay to avoid too fast key presses

except Exception as e:
    print(f"An error occurred: {e}")
    # 进行必要的清理操作，比如释放所有按键
    try:
        kbd.release_all()
    except Exception as cleanup_error:
        print(f"Error during cleanup: {cleanup_error}")
    # 复位RP2040
    microcontroller.reset()
