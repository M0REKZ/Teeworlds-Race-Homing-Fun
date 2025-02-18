/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */
#include <string.h>
#include <engine/external/glfw/include/GL/glfw.h>

#include <engine/e_system.h>
#include <engine/e_client_interface.h>
#include <engine/e_config.h>

static int keyboard_state[2][1024]; /* TODO: fix this!! */
static int keyboard_current = 0;
static int keyboard_first = 1;


static struct
{
	unsigned char presses;
	unsigned char releases;
} input_count[2][1024] = {{{0}}, {{0}}};

static unsigned char input_state[2][1024] = {{0}, {0}};

static int input_current = 0;
static unsigned int last_release = 0;
static unsigned int release_delta = -1;

void inp_mouse_relative(int *x, int *y)
{
	static int last_x = 0, last_y = 0;
	static int last_sens = 100.0f;
	int nx, ny;
	float sens = config.inp_mousesens/100.0f;
	
	if(last_sens != config.inp_mousesens)
	{
		last_x = (last_x/(float)last_sens)*(float)config.inp_mousesens;
		last_y = (last_y/(float)last_sens)*(float)config.inp_mousesens;
		last_sens = config.inp_mousesens;
	}
	
	
	glfwGetMousePos(&nx, &ny);
	nx *= sens;
	ny *= sens;
	
	*x = nx-last_x;
	*y = ny-last_y;
	last_x = nx;
	last_y = ny;
}

enum
{
	INPUT_BUFFER_SIZE=32
};

static INPUT_EVENT input_events[INPUT_BUFFER_SIZE];
static int num_events = 0;

static void add_event(char c, int key, int flags)
{
	if(num_events != INPUT_BUFFER_SIZE)
	{
		input_events[num_events].ch = c;
		input_events[num_events].key = key;
		input_events[num_events].flags = flags;
		num_events++;
	}
}

int inp_num_events()
{
	return num_events;
}

void inp_clear_events()
{
	num_events = 0;
}

INPUT_EVENT inp_get_event(int index)
{
	if(index < 0 || index >= num_events)
	{
		INPUT_EVENT e = {0,0};
		return e;
	}
	
	return input_events[index];
}

static void char_callback(int character, int action)
{
	if(action == GLFW_PRESS && character < 256)
		add_event((char)character, 0, 0);
}

static void key_callback(int key, int action)
{
	if(action == GLFW_PRESS)
		add_event(0, key, INPFLAG_PRESS);
	else
		add_event(0, key, INPFLAG_RELEASE);
	
	if(action == GLFW_PRESS)
		input_count[input_current^1][key].presses++;
	if(action == GLFW_RELEASE)
		input_count[input_current^1][key].releases++;
	input_state[input_current^1][key] = action;
}

static void mousebutton_callback(int button, int action)
{
	if(action == GLFW_PRESS)
		add_event(0, KEY_MOUSE_FIRST+button, INPFLAG_PRESS);
	else
		add_event(0, KEY_MOUSE_FIRST+button, INPFLAG_RELEASE);
		
	if(action == GLFW_PRESS)
		input_count[input_current^1][KEY_MOUSE_FIRST+button].presses++;
	if(action == GLFW_RELEASE)
	{
		if(button == 0)
		{
			release_delta = time_get() - last_release;
			last_release = time_get();
		}
		input_count[input_current^1][KEY_MOUSE_FIRST+button].releases++;
	}
	input_state[input_current^1][KEY_MOUSE_FIRST+button] = action;
}


static void mousewheel_callback(int pos)
{
	if(pos > 0)
	{
		while(pos-- != 0)
		{
			input_count[input_current^1][KEY_MOUSE_WHEEL_UP].presses++;
			input_count[input_current^1][KEY_MOUSE_WHEEL_UP].releases++;
		}
		
		add_event(0, KEY_MOUSE_WHEEL_UP, INPFLAG_PRESS);
		add_event(0, KEY_MOUSE_WHEEL_UP, INPFLAG_RELEASE);
	}
	else if(pos < 0)
	{
		while(pos++ != 0)
		{
			input_count[input_current^1][KEY_MOUSE_WHEEL_DOWN].presses++;
			input_count[input_current^1][KEY_MOUSE_WHEEL_DOWN].releases++;
		}	

		add_event(0, KEY_MOUSE_WHEEL_DOWN, INPFLAG_PRESS);
		add_event(0, KEY_MOUSE_WHEEL_DOWN, INPFLAG_RELEASE);
	}
	glfwSetMouseWheel(0);
}


void inp_init()
{
	glfwEnable(GLFW_KEY_REPEAT);
	glfwSetCharCallback(char_callback);
	glfwSetKeyCallback(key_callback);
	glfwSetMouseButtonCallback(mousebutton_callback);
	glfwSetMouseWheelCallback(mousewheel_callback);
}

void inp_mouse_mode_absolute()
{
	glfwEnable(GLFW_MOUSE_CURSOR);
}

void inp_mouse_mode_relative()
{
	/*if (!config.gfx_debug_resizable)*/
	glfwDisable(GLFW_MOUSE_CURSOR);
}

int inp_mouse_doubleclick()
{
	return release_delta < (time_freq() >> 2);
}

void inp_clear_key_states()
{
	mem_zero(keyboard_state, sizeof(keyboard_state));
	mem_zero(input_count, sizeof(input_count));
}

int inp_key_presses(int key)
{
	return input_count[input_current][key].presses;
}

int inp_key_releases(int key)
{
	return input_count[input_current][key].releases;
}

int inp_key_state(int key)
{
	return input_state[input_current][key];
}

int inp_key_pressed(int key) { return keyboard_state[keyboard_current][key]; }
int inp_key_was_pressed(int key) { return keyboard_state[keyboard_current^1][key]; }
int inp_key_down(int key) { return inp_key_pressed(key)&&!inp_key_was_pressed(key); }
int inp_button_pressed(int button) { return keyboard_state[keyboard_current][button]; }

void inp_update()
{
    int i, v;
    
	/* clear and begin count on the other one */
	mem_zero(&input_count[input_current], sizeof(input_count[input_current]));
	mem_copy(input_state[input_current], input_state[input_current^1], sizeof(input_state[input_current]));
	input_current^=1;

    if(keyboard_first)
    {
        /* make sure to reset */
        keyboard_first = 0;
        inp_update();
    }
    
    keyboard_current = keyboard_current^1;
    for(i = 0; i < KEY_LAST; i++)
    {
	    if (i >= KEY_MOUSE_FIRST)
			v = glfwGetMouseButton(i-KEY_MOUSE_FIRST) == GLFW_PRESS ? 1 : 0;
		else
			v = glfwGetKey(i) == GLFW_PRESS ? 1 : 0;
        keyboard_state[keyboard_current][i] = v;
    }

	/* handle mouse wheel */
	/*
	i = glfwGetMouseWheel();
    keyboard_state[keyboard_current][KEY_MOUSE_WHEEL_UP] = 0;
    keyboard_state[keyboard_current][KEY_MOUSE_WHEEL_DOWN] = 0;
    if(w > 0)
    	keyboard_state[keyboard_current][KEY_MOUSE_WHEEL_UP] = 1;
    if(w < 0)
    	keyboard_state[keyboard_current][KEY_MOUSE_WHEEL_DOWN] = 1;
    glfwSetMouseWheel(0);*/
}
