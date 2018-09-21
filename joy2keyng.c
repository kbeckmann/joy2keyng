// #define debug(...)
#define debug printf

/*
	joy2keyng, based on joy2key by Peter Amstutz etc.
	https://sourceforge.net/projects/joy2key/

	I just removed a lot of junk that I didn't need.
*/

#define XK_MISCELLANY 		1
#define XK_LATIN1 			1

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/joystick.h>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

static Display *display;
static int root_window;
static Window window;
static int screen;

#define DEFAULT_DEVICE "/dev/input/js0"

typedef enum {
	PRESS,
	RELEASE
} press_or_release_type;

static int jsfd = -1;
static char *device = DEFAULT_DEVICE;
static char numaxes;
static char numbuttons;
static struct js_event js;

unsigned int button_actions[256];

void sendkey(unsigned int keycode, press_or_release_type PoR)
{
	XEvent event;
	int revert;

	debug("%d %d\n", keycode, PoR);

	root_window = XDefaultRootWindow(display);
	XGetInputFocus(display, &window, &revert);

	memset(&event, 0, sizeof(event));
	event.xkey.type = KeyPress;
	event.xkey.root = RootWindow(display, screen);
	event.xkey.window = window;
	event.xkey.subwindow = window;
	event.xkey.same_screen = True;
	event.xkey.time = CurrentTime;
	event.xkey.display = display;
	event.xkey.type = PoR == PRESS ? KeyPress : KeyRelease;
	event.xkey.state = PoR == PRESS ? 0x10 : 0x11;
	event.xkey.keycode = keycode;

	debug("sendkey: keycode  0x%08x  %0d   ", event.xkey.keycode, keycode);
	debug("sendkey: keysym %s\n", XKeysymToString(XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0)));
	debug("state hex %x\n",event.xkey.state);

	XSendEvent(display, window, False, event.xkey.state, &event);
}

int argtokey(char *arg)
{
	if ((XStringToKeysym(arg)) == NoSymbol) {
		printf("argtokey:Can't find %s, check include/X11/keysymdef.h\n", arg);
		exit(1);
	}

	printf("argtokey:read key %s dblcheck %s\n", arg, XKeysymToString(XStringToKeysym(arg)));
	return XKeysymToKeycode(display,XStringToKeysym(arg));
}

int main(int argc, char *argv[])
{
	int parentwin = 0;
	fd_set close_set;
	struct timeval tv;

	display = XOpenDisplay(NULL);
	if(display == NULL) 
	{
		puts("Error opening X Display");
		exit(1);
	}

	if (!argv[1]) {
		printf(
			"Usage: %s -buttons [[<joystick button> <keycode>] [...]]\n\n"
			"Example: %s -buttons 6 s 5 d 4 f 3 space 2 j 1 k 0 l"
			, argv[0], argv[0]);
		exit(1);
	}

	for (int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-buttons")) {
			while (i + 1 < argc) {
				int joy_btn = atol(argv[++i]) & 0xFF;
				int key = argtokey(argv[++i]);
				button_actions[joy_btn] = key;
			}
		}
	}

	if((jsfd = open(device,O_RDONLY)) == -1) {
		printf("Error opening %s!\n", device);
		puts("Are you sure you have joystick support in your kernel?");
		return 1;
	}

	if (ioctl(jsfd, JSIOCGAXES, &numaxes)) {
		perror("joy2key: error getting axes");
		return 1;
	}

	if (ioctl(jsfd, JSIOCGBUTTONS, &numbuttons)) {
		perror("joy2key: error getting buttons");
		return 1;
	}

	screen = DefaultScreen(display);

	for(;;)
	{
		memset(&js, 0, sizeof(struct js_event));
		read(jsfd, &js, sizeof(struct js_event));

		switch(js.type)
		{
		case JS_EVENT_BUTTON:
			printf("js.number=%d action=%d value=%d\n", js.number, button_actions[js.number], !!js.value);
			if (button_actions[js.number]) {
				sendkey(button_actions[js.number], js.value ? PRESS : RELEASE);
			}
			break;
		}
		XFlush(display);
	}

	return 0;
}
