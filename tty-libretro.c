#include "defines.h"
#include "libretro.h"
#include "libretro-defs.h"
#include "tty.h"

static int ar2 = 0, shift = 0, ctrl;
static int use_callback;
extern retro_environment_t environ_cb;
extern retro_input_state_t input_state_cb;

struct keymap {
	int normal[RETROK_LAST];
	int shifted[RETROK_LAST];
};

static const struct keymap qwerty = {
	.normal = {
		[RETROK_ESCAPE]       = TTY_STOP,
		[RETROK_F1]           = 0201,          /* Repeat.  */
		[RETROK_F2]           = 003,           /* KT */
		[RETROK_F3]           = 0231,          /* -|--> */
		[RETROK_F4]           = 026,           /* |<--- */
		[RETROK_F5]           = 027,           /* |---> */
		[RETROK_F6]           = 0202,          /* ind su */
		[RETROK_F7]           = 0204,          /* block edit */
		[RETROK_F8]           = 0220,          /* step */
		[RETROK_F9]           = 014,           /* clear */
		[RETROK_F10]          = TTY_STOP,      /* Stop (red button)*/
		[RETROK_F11]          = 016,
		[RETROK_F12]          = 017,
		[RETROK_BREAK]        = TTY_STOP,

		[RETROK_BACKQUOTE]    = '`',
		[RETROK_1]            = '1',
		[RETROK_2]            = '2',
		[RETROK_3]            = '3',
		[RETROK_4]            = '4',
		[RETROK_5]            = '5',
		[RETROK_6]            = '6',
		[RETROK_7]            = '7',
		[RETROK_8]            = '8',
		[RETROK_9]            = '9',
		[RETROK_0]            = '0',
		[RETROK_MINUS]        = '-',
		[RETROK_EQUALS]       = '=',
		[RETROK_BACKSPACE]    = 0x18,

		[RETROK_TAB]          = 0x89,
		[RETROK_q]             = 'q',
		[RETROK_w]             = 'w',
		[RETROK_e]             = 'e',
		[RETROK_r]             = 'r',
		[RETROK_t]             = 't',
		[RETROK_y]             = 'y',
		[RETROK_u]             = 'u',
		[RETROK_i]             = 'i',
		[RETROK_o]             = 'o',
		[RETROK_p]             = 'p',
		[RETROK_LEFTBRACKET]  = '[',
		[RETROK_RIGHTBRACKET] = ']',
		[RETROK_BACKSLASH]    = '\\',
		
		[RETROK_a]             = 'a',
		[RETROK_s]             = 's',
		[RETROK_d]             = 'd',
		[RETROK_f]             = 'f',
		[RETROK_g]             = 'g',
		[RETROK_h]             = 'h',
		[RETROK_j]             = 'j',
		[RETROK_k]             = 'k',
		[RETROK_l]             = 'l',
		[RETROK_SEMICOLON]    = ';',
		[RETROK_QUOTE]        = '\'',
		[RETROK_RETURN]       = 012,
		
		[RETROK_z]             = 'z',
		[RETROK_x]             = 'x',
		[RETROK_c]             = 'c',
		[RETROK_v]             = 'v',
		[RETROK_b]             = 'b',
		[RETROK_n]             = 'n',
		[RETROK_m]             = 'm',
		[RETROK_COMMA]        = ',',
		[RETROK_PERIOD]       = '.',
		[RETROK_SLASH]        = '/',

		[RETROK_SPACE]        = ' ',

		[RETROK_LEFT]         = 010,
		[RETROK_UP]           = 032,
		[RETROK_RIGHT]        = 031,
		[RETROK_DOWN]         = 033,

		[RETROK_HOME]         = 023,         /* vs */
	},
	.shifted = {
		[RETROK_BACKQUOTE]    = '~',
		[RETROK_1]            = '!',
		[RETROK_2]            = '@',
		[RETROK_3]            = '#',
		[RETROK_4]            = '$',
		[RETROK_5]            = '%',
		[RETROK_6]            = '^',
		[RETROK_7]            = '&',
		[RETROK_8]            = '*',
		[RETROK_9]            = '(',
		[RETROK_0]            = ')',
		[RETROK_MINUS]        = '_',
		[RETROK_EQUALS]       = '+',

		[RETROK_q]             = 'Q',
		[RETROK_w]             = 'W',
		[RETROK_e]             = 'E',
		[RETROK_r]             = 'R',
		[RETROK_t]             = 'T',
		[RETROK_y]             = 'Y',
		[RETROK_u]             = 'U',
		[RETROK_i]             = 'I',
		[RETROK_o]             = 'O',
		[RETROK_p]             = 'P',
		[RETROK_LEFTBRACKET]  = '{',
		[RETROK_RIGHTBRACKET] = '}',
		[RETROK_BACKSLASH]    = '|',
		
		[RETROK_a]             = 'A',
		[RETROK_s]             = 'S',
		[RETROK_d]             = 'D',
		[RETROK_f]             = 'F',
		[RETROK_g]             = 'G',
		[RETROK_h]             = 'H',
		[RETROK_j]             = 'J',
		[RETROK_k]             = 'K',
		[RETROK_l]             = 'L',
		[RETROK_SEMICOLON]    = ':',
		[RETROK_QUOTE]        = '"',
		
		[RETROK_z]             = 'Z',
		[RETROK_x]             = 'X',
		[RETROK_c]             = 'C',
		[RETROK_v]             = 'V',
		[RETROK_b]             = 'B',
		[RETROK_n]             = 'N',
		[RETROK_m]             = 'M',
		[RETROK_COMMA]        = '<',
		[RETROK_PERIOD]       = '>',
		[RETROK_SLASH]        = '?'
	},
};

static const struct keymap jcuken = {
	.normal = {
		[RETROK_ESCAPE]       = TTY_STOP,
		[RETROK_F1]           = 0201,          /* Repeat.  */
		[RETROK_F2]           = 003,           /* KT */
		[RETROK_F3]           = 0231,          /* -|--> */
		[RETROK_F4]           = 026,           /* |<--- */
		[RETROK_F5]           = 027,           /* |---> */
		[RETROK_F6]           = 0202,          /* ind su */
		[RETROK_F7]           = 0204,          /* block edit */
		[RETROK_F8]           = 0220,          /* step */
		[RETROK_F9]           = 014,           /* clear */
		[RETROK_F10]          = TTY_STOP,      /* Stop (red button)*/

		// Those 3 don't completely match original as there are no additional keys in bottom row
		// on modern keyboard
		[RETROK_F11]          = 016,
		[RETROK_F12]          = 017,
		[RETROK_BREAK]        = TTY_STOP,

		[RETROK_BACKQUOTE]    = ';',
		[RETROK_1]            = '1',
		[RETROK_2]            = '2',
		[RETROK_3]            = '3',
		[RETROK_4]            = '4',
		[RETROK_5]            = '5',
		[RETROK_6]            = '6',
		[RETROK_7]            = '7',
		[RETROK_8]            = '8',
		[RETROK_9]            = '9',
		[RETROK_0]            = '0',
		[RETROK_MINUS]        = '-',
		[RETROK_EQUALS]       = '/',
		[RETROK_BACKSPACE]    = 0x18,

		[RETROK_TAB]          = 0x89,
		[RETROK_q]             = 'J',
		[RETROK_w]             = 'C',
		[RETROK_e]             = 'U',
		[RETROK_r]             = 'K',
		[RETROK_t]             = 'E',
		[RETROK_y]             = 'N',
		[RETROK_u]             = 'G',
		[RETROK_i]             = '[',
		[RETROK_o]             = ']',
		[RETROK_p]             = 'Z',
		[RETROK_LEFTBRACKET]   = 'H',
		[RETROK_RIGHTBRACKET]  = ':',
		// here should be extra key, use 102nd key
		[RETROK_OEM_102]         = 0x5f,
		
		[RETROK_a]             = 'F',
		[RETROK_s]             = 'Y',
		[RETROK_d]             = 'W',
		[RETROK_f]             = 'A',
		[RETROK_g]             = 'P',
		[RETROK_h]             = 'R',
		[RETROK_j]             = 'O',
		[RETROK_k]             = 'L',
		[RETROK_l]             = 'D',
		[RETROK_SEMICOLON]    = 'V',
		[RETROK_QUOTE]        = '\\',
		[RETROK_BACKSLASH]    = '.',
		[RETROK_RETURN]       = 012,
		
		[RETROK_z]             = 'Q',
		[RETROK_x]             = '^',
		[RETROK_c]             = 'S',
		[RETROK_v]             = 'M',
		[RETROK_b]             = 'I',
		[RETROK_n]             = 'T',
		[RETROK_m]             = 'X',
		[RETROK_COMMA]        = 'B',
		[RETROK_PERIOD]       = '@',
		[RETROK_SLASH]        = ',',

		[RETROK_SPACE]        = ' ',

		[RETROK_LEFT]         = 010,
		[RETROK_UP]           = 032,
		[RETROK_RIGHT]        = 031,
		[RETROK_DOWN]         = 033,

		[RETROK_HOME]         = 023,         /* vs */
	},
	.shifted = {
		[RETROK_BACKQUOTE]    = '+',
		[RETROK_1]            = '!',
		[RETROK_2]            = '"',
		[RETROK_3]            = '#',
		[RETROK_4]            = '$',
		[RETROK_5]            = '%',
		[RETROK_6]            = '&',
		[RETROK_7]            = '\'',
		[RETROK_8]            = '(',
		[RETROK_9]            = ')',
		[RETROK_0]            = '{',
		[RETROK_MINUS]        = '=',
		[RETROK_EQUALS]       = '?',

		[RETROK_q]            = 'j',
		[RETROK_w]            = 'c',
		[RETROK_e]            = 'u',
		[RETROK_r]            = 'k',
		[RETROK_t]            = 'e',
		[RETROK_y]            = 'n',
		[RETROK_u]            = 'g',
		[RETROK_i]            = '{',
		[RETROK_o]            = '}',
		[RETROK_p]            = 'z',
		[RETROK_LEFTBRACKET]  = 'h',
		[RETROK_RIGHTBRACKET] = '*',
		// here should be extra key, use 102nd key
		[RETROK_OEM_102]      = 0x7f,
		
		[RETROK_a]            = 'f',
		[RETROK_s]            = 'y',
		[RETROK_d]            = 'w',
		[RETROK_f]            = 'a',
		[RETROK_g]            = 'p',
		[RETROK_h]            = 'r',
		[RETROK_j]            = 'o',
		[RETROK_k]            = 'l',
		[RETROK_l]            = 'd',
		[RETROK_SEMICOLON]    = 'v',
		[RETROK_QUOTE]        = '|',
		[RETROK_BACKSLASH]    = '>',
		
		[RETROK_z]             = 'q',
		[RETROK_x]             = '~',
		[RETROK_c]             = 's',
		[RETROK_v]             = 'm',
		[RETROK_b]             = 'i',
		[RETROK_n]             = 't',
		[RETROK_m]             = 'x',
		[RETROK_COMMA]        = 'b',
		[RETROK_PERIOD]       = '`',
		[RETROK_SLASH]        = '<'
	},
};

static const struct keymap *current_keymap = &qwerty;
static bool curstate[RETROK_LAST];

static RETRO_CALLCONV void keyboard_cb(bool down, unsigned keycode,
      uint32_t character, uint16_t mod)
{
	if (keycode == 0 && down == 0 && character == 0) {
		memset(curstate, 0, sizeof(curstate));
		tty_keyevent(-1);
	}
	if (keycode >= RETROK_LAST)
		return;
	if (curstate[keycode] == down)
		return;
	curstate[keycode] = down;
	if (keycode == RETROK_LSUPER || keycode == RETROK_LALT) {
		ar2 = down;
		return;
	}
	if (keycode == RETROK_LSHIFT || keycode == RETROK_RSHIFT) {
		shift = down;
		return;
	}
	if (keycode == RETROK_LCTRL || keycode == RETROK_RCTRL) {
		ctrl = down;
		return;
	}
	if (!down) {
		tty_keyevent(-1);
		return;
	}
	int c = 0;
	if (shift && current_keymap->shifted[keycode]) {
		c = current_keymap->shifted[keycode];
	} else if (current_keymap->normal[keycode]) {
		c = current_keymap->normal[keycode];
	}
	if (c == 0) {
		return;
	}
	/* TODO: caps lock.  */
	if (ctrl && (c & 0100))
		c &= 037;
	if (keycode == RETROK_F11 && ar2)
		c = TTY_RESET;
	if (ar2) {
	    c |= 0200;
	}
	tty_keyevent(c);
}

void
tty_set_keymap()
{
    	struct retro_variable var;
	
	var.key = "bk_layout";
	var.value = NULL;

	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value && strcmp(var.value, "jcuken") == 0) {
		current_keymap = &jcuken;		
	} else {
		current_keymap = &qwerty;
	}
}

void
tty_poll() {
	if (use_callback)
		return;
	ar2 = input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LSUPER) || input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LALT);
	shift = input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LSHIFT) || input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RSHIFT);
	ctrl = input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LCTRL) || input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RCTRL);
	int change = 0;
	int value = 0;
	int c = -1;

	for (int keycode = 0; keycode < RETROK_LAST; keycode++) {
		int newstate = input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, keycode);
		if (curstate[keycode] != newstate) {
			int curc;
			curstate[keycode] = newstate;
			if (shift && current_keymap->shifted[keycode]) {
				curc = current_keymap->shifted[keycode];
			} else if (current_keymap->normal[keycode]) {
				curc = current_keymap->normal[keycode];
			}
			if (keycode == RETROK_F11)
				curc = TTY_RESET;
			if (curc) {
				change = 1;
				if (newstate)
					c = curc;
			}
		}
	}

	if (!change)
		return;
	
	if (c == -1) {
		tty_keyevent(c);
		return;
	}
	/* TODO: caps lock.  */
	if (ctrl && (c & 0100))
		c &= 037;
	if (ar2) {
	    c |= 0200;
	}
	tty_keyevent(c);
}

void
tty_open() {
    	struct retro_variable var;
	
	var.key = "bk_keyboard_type";
	var.value = NULL;

	use_callback = (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value && strcmp(var.value, "callback") == 0);

	if (use_callback) {
		static struct retro_keyboard_callback cb = { keyboard_cb };
		environ_cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &cb);
	}
}
