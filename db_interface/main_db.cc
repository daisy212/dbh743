// ****************************************************************************
//  main.cc                                                       DB48X project
// ****************************************************************************
//
//   File Description:
//
//      The DB48X main RPL loop
//
//
//
//C:\segger\embOS_Ultra_CortexM_ES_Obj_SFL_V5.20.0.0\STM32U585_db48x_v0.a\db48x\src\dmcp\main.cc
//
//
//
//
// ****************************************************************************
//   (C) 2022 Christophe de Dinechin <christophe@dinechin.org>
//   This software is licensed under the terms outlined in LICENSE.txt
// ****************************************************************************
//   This file is part of DB48X.
//
//   DB48X is free software: you can redistribute it and/or modify
//   it under the terms outlined in the LICENSE.txt file
//
//   DB48X is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// This code is distantly derived from the SwissMicro SDKDemo calculator

/*
Faire marcher les 4 timers : ok
Show et Setup : gestion du clavier avec Mailbox dans dmcp.cpp
Utiliser la description du clavier de target.cc ligne 112
Gestion usb, segger à partir de 910.56, eth over usb, + client ftp ??
Tester un changement de version v9.9

// ----------------------------------------------------------------------------
//   Return the complete key ID from HP48-style Row/Column/Plane position
// ----------------------------------------------------------------------------
// HP48 key codes are given in the form rc.ph, where r, c, p and h
// are one decimal digit each.
// r = row
// c = column
// p = plane
// h = hold (i.e. shift + key held simultaneously)
//
// On DMCP, we reinterpret "h" as "transient alpha"
//
// The plane is documented as follows:
// 0: Like 1
// 1: Unshifted
// 2: Left shift
// 3: Right shift
// 4: Alpha
// 5: Alpha left shift
// 6: Alpha right shift
//
// DB48X adds 7, 8 and 9 for lowercase alpha



passage à version 9.9Z : recopie du répertoire principal, une seule modif nécessaire :
	undefined symbol range::abs(runtime::gcp<range const> const&) referenced by symbol abs::evaluate(runtime::gcp<algebraic const> const&) (section .text.abs::evaluate(runtime::gcp<algebraic const> const&) in file functions.o)




essai avec spi sdcard

*/


#include "SEGGER_RTT.h"

#include "blitter.h"
#include "dmcp.h"
#include "expression.h"
#include "font.h"
#include "program.h"
#include "recorder.h"
#include "stack.h"
#include "sysmenu.h"
#include "target.h"
#include "user_interface.h"
#include "util.h"


#include "RTOS.h"
#include "FS.h"
#include "FS_OS.h"


#include "db_hardware_def.h"
#include "LS027B7DH01.h"

#if SIMULATOR
#  include "tests.h"
#endif


#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using std::max;
using std::min;

//extern LCD_Handle_t hlcd;


extern "C" {
//int __sysfn_read_key(int *pkey1, int *pkey2){};
//uint32_t sys_current_ms();

//#define lcd_line_addr(x)	(lcd_framebuffer+LCD_LINE_SIZE*x)		

//extern const uint8_t DM_keyb[];
//extern const char kb_fct_name[][3];

//extern uint32_t Cnt_ms ; 
//extern OS_MAILBOX      Mb_Keyboard;

}




// Place variables in SRAM3
//__attribute__((section(".sram3"))) uint32_t retained_data[1024];



// ============================================================================
//
// Those are put in the same file to guarantee initialization order
//
// ============================================================================

// Initialize the screen

//surface Screen((pixword *) lcd_line_addr(0), LCD_W, LCD_H, LCD_SCANLINE, LCD_W);
surface Screen((pixword *) LCD_GetFramebuffer(), LCD_W, LCD_H, LCD_SCANLINE, LCD_W);



// Pre-built patterns for shades of grey
const pattern pattern::black   = pattern(0, 0, 0);
const pattern pattern::gray10  = pattern(32, 32, 32);
const pattern pattern::gray25  = pattern(64, 64, 64);
const pattern pattern::gray50  = pattern(128, 128, 128);
const pattern pattern::gray75  = pattern(192, 192, 192);
const pattern pattern::gray90  = pattern(224, 224, 224);
const pattern pattern::white   = pattern(255, 255, 255);
const pattern pattern::invert  = pattern(~0ULL);

// Settings depend on patterns
settings Settings;

// Runtime must be initialized before ser interface, which contains GC pointers
runtime::gcptr *runtime::GCSafe;
runtime rt(nullptr, 0);
user_interface ui;

uint32_t last_keystroke_time = 0;
uint32_t  last_key            = 0;

RECORDER(main,          16, "Main RPL thread");
RECORDER(main_error,    16, "Errors in the main RPL thread");
RECORDER(tests_rpl,    256, "Test request processing on RPL");
RECORDER(refresh,       16, "Refresh requests");


static byte *lcd_buffer = nullptr;
static uint  row_min    = ~0;
static uint  row_max    = 0;

void mark_dirty(uint row)
// ----------------------------------------------------------------------------
//   Mark a screen range as dirty
// ----------------------------------------------------------------------------
{

    if (row < LCD_H)
    {
	#if DM42
        if (Settings.DMCPDisplayRefresh())
        {
            bitblt24(0, 8, row, 0, BLT_XOR, BLT_NONE);
        }
        else if (!lcd_buffer[52 * row - 2])
        {
		
            lcd_buffer[52 * row - 2] = 1;
            lcd_buffer[52 * row] ^= 1;
            if (row_min > row)
                row_min = row;
            if (row_max < row)
                row_max = row;		
		}
	#elif ( DBh743| DBu585)
	//	LCD_MarkLineModified(&hlcd, row);
		if (row_min > row)
			row_min = row;
        if (row_max < row)
			row_max = row;	

	LCD_MarkLineModified(&hlcd, row);

	#endif // DM42
    }



}


void refresh_dirty()
// ----------------------------------------------------------------------------
//  Send an LCD refresh request for the area dirtied by drawing
// ----------------------------------------------------------------------------
{
    uint start = sys_current_ms();
//	LCD_MarkLineModified(&hlcd, row_min);
//	LCD_MarkLineModified(&hlcd, row_max);
//	LCD_MarkAllLinesModified(&hlcd);
	
	LCD_Status_t res = LCD_UpdateModifiedLines(&hlcd);
	if (res != LCD_OK) SEGGER_RTT_printf(0, "\nT%06d:Lcd update err :%d, %d <ref < %d", Cnt_ms%1000000, (int)res, row_min, row_max);

    row_min = ~0;
    row_max = 0;
    program::refresh_time += sys_current_ms() - start;
}


void set_timer(uint timerid, uint period)
// ----------------------------------------------------------------------------
//   Conditionally set a timer based on period
// ----------------------------------------------------------------------------
{
    if (period >= 1000)
    {
        sys_timer_disable(timerid);
        if (period >= 60000)
            CLR_ST(STAT_CLK_WKUP_SECONDS);
        else
            SET_ST(STAT_CLK_WKUP_SECONDS);
    }
    else
    {
        sys_timer_start(timerid, period);
    }
}


void redraw_lcd(bool force)
// ----------------------------------------------------------------------------
//   Redraw the whole LCD
// ----------------------------------------------------------------------------
{
    uint start = sys_current_ms();

 //   SEGGER_RTT_printf(0,"\nBegin redraw at %u", start);

    // Draw the various components handled by the user interface
    ui.draw_start(force);

    ui.draw_header();
    ui.draw_battery();

    ui.draw_annunciators();

    ui.draw_menus();

    if (!ui.draw_help())
    {
        ui.draw_editor();
        ui.draw_cursor(true, ui.cursor_position());
        ui.draw_stack();
        if (!ui.draw_stepping_object())
            ui.draw_command();

    }
    ui.draw_error();

    // Refresh the screen
    refresh_dirty();

    // Compute next refresh
    uint end = sys_current_ms();
    uint period = ui.draw_refresh();
//    record(main,            "Refresh at %u (%u later), period %u", end, end - start, period);
 //   SEGGER_RTT_printf(0, "\nRefresh at %u (%u later), period %u", end, end - start, period);

    // Refresh screen moving elements after the requested period
    set_timer(TIMER1, period);
    program::display_time += end - start;
}


static void redraw_periodics()
// ----------------------------------------------------------------------------
//   Redraw the elements that move
// ----------------------------------------------------------------------------
{
    uint start       = program::read_time();
    uint dawdle_time = start - last_keystroke_time;

//SEGGER_RTT_printf(0,"\nPeriodics %u", start);
    ui.draw_start(false);
    ui.draw_header();
    ui.draw_battery();
    if (program::animated())
    {
        ui.draw_cursor(false, ui.cursor_position());
        ui.draw_menus();
    }
    refresh_dirty();

    // Slow things down if inactive for long enough
    uint period = ui.draw_refresh();
    if (!program::animated())
    {
        // Adjust refresh time based on time since last interaction
        // After 10s, update at most every 3s
        // After 1 minute, update at most every 10s
        // After 3 minutes, update at most once per minute
        if (dawdle_time > 180000 && period < 60000)
            period = 60000;
        else if (dawdle_time > 60000 && period < 10000)
            period = 10000;
        else if (dawdle_time > 10000 && period < 3000)
            period = 3000;
    }

    uint end = program::read_time();
//SEGGER_RTT_printf(0,"\nDawdling for %u at %u after %u", period, end, end-start);

    // Refresh screen moving elements after 0.1s
    set_timer(TIMER1, period);

    program::display_time += end - start;
}


static void handle_key(int key, bool repeating, bool talpha)
// ----------------------------------------------------------------------------
//   Handle all user-interface keys
// ----------------------------------------------------------------------------
{
    sys_timer_disable(TIMER0);
    bool consumed = ui.key(key, repeating, talpha);
    if (!consumed)
        beep(1835, 125);

    // Key repeat timer
    if (ui.repeating())
        sys_timer_start(TIMER0, repeating ? KB_DB_FIRST_PERIOD : KB_DB_FIRST_REPEAT);
}


void db48x_set_beep_mute(int val)
// ----------------------------------------------------------------------------
//   Set the beep flag (shared with firmware)
// ----------------------------------------------------------------------------
{
    Settings.BeepOff(val);
    Settings.SilentBeepOn(val);
}


int db48x_is_beep_mute()
// ----------------------------------------------------------------------------
//   Check the beep flag from our settings
// ----------------------------------------------------------------------------
{
    return Settings.BeepOff();
}


bool load_saved_keymap(cstring name)
// ----------------------------------------------------------------------------
//   Load the default system state file
// ----------------------------------------------------------------------------
{
    bool isdefault = false;
    char keymap_name[80] = { 0 };
    if (name)
    {
        file kcfg("/config/keymap.cfg", file::WRITING);
        if (kcfg.valid())
            kcfg.write(name, strlen(name));
    }

    file kcfg("config\\keymap.cfg", file::READING);
    if (kcfg.valid())
    {
        kcfg.read(keymap_name, sizeof(keymap_name)-1);
        for (size_t i = 0; i < sizeof(keymap_name); i++)
            if (keymap_name[i] == '\n')
                keymap_name[i] = 0;
    }
    else
    {
        strncpy(keymap_name, "/config/db48x.48k", sizeof(keymap_name));
        isdefault = true;
    }

    // Load default keymap
    if (!ui.load_keymap(keymap_name))
    {
        // Fail silently if we try to load a default file
        if (isdefault)
            rt.clear_error();
        else
            rt.command(command::static_object(object::ID_KeyMap));
        return false;
    }
    return true;
}


extern uint memory_size;
void program_init()
// ----------------------------------------------------------------------------
//   Initialize the program
// ----------------------------------------------------------------------------
{
    // Setup application menu callbacks
    run_menu_item_app = menu_item_run;
    menu_line_str_app = menu_item_description;
    is_beep_mute = db48x_is_beep_mute;
    set_beep_mute = db48x_set_beep_mute;

    lcd_buffer = LCD_GetFramebuffer();
//	lcd_buffer = lcd_framebuffer;


    // Setup default fonts
    font_defaults();

#if DM42
    // Give as much as memory as possible to the runtime
    // Experimentally, this is the amount of memory we need to leave free
    size_t size = sys_free_mem() - 10 * 1024;
    byte *memory = (byte *) malloc(size);
#elif DBh743
    size_t size = 1024 * 256;
    byte *memory = (byte *) OS_HEAP_malloc(size);
#elif DBu585
    // Give 384K bytes to the runtime u585 = 700ko memoire
	// not allocated by malloc, but fixed zone

#elif SIMULATOR
    // Give 4K bytes to the runtime to stress-test the GC
    size_t size = 1024 * memory_size;
    byte *memory = (byte *) malloc(size);
#endif
    rt.memory(memory, size);

    // Check if we have a state file to load
    load_system_state();
	bool res = load_saved_keymap();
SEGGER_RTT_printf(0,  "\nLoad keymap : %s\n", res ? "ok":"err");

    // Enable wakeup each minute (for clock update)
    SET_ST(STAT_CLK_WKUP_ENABLE);
}


void power_check(bool running, bool showimage)
// ----------------------------------------------------------------------------
//   Check power state, keep looping until it's safe to run
// ----------------------------------------------------------------------------
// Status flags:
// ST(STAT_PGM_END)   - Program should go to off state (set by auto off timer)
// ST(STAT_SUSPENDED) - Program signals it is ready for off
// ST(STAT_OFF)       - Program in off state (only [EXIT] key can wake it up)
// ST(STAT_RUNNING)   - OS doesn't sleep in this mode
{
    while (true)
    {
        // Already in off mode and suspended
        if ((ST(STAT_PGM_END) && ST(STAT_SUSPENDED)) ||
            // Go to sleep if no keys available
            (!ST(STAT_PGM_END) && key_empty()))
        {
            CLR_ST(STAT_RUNNING);
            static uint last_awake = 0;
            uint tin = sys_current_ms();
            if (last_awake)
                program::active_time += tin - last_awake;
            sys_sleep();
            uint tout = sys_current_ms();
            last_awake = tout;
            program::sleeping_time += tout - tin;
            program::run_cycles++;
        }
        if (ST(STAT_PGM_END) || ST(STAT_SUSPENDED))
        {
            // Wakeup in off state or going to sleep
            if (!ST(STAT_SUSPENDED))
            {
                bool lowbat = !program::on_usb && program::low_battery();
                if (lowbat)
                    ui.draw_message("Switched off due to low power",
                                    "Connect to USB to avoid losing memory",
                                    "Replace the battery as soon as possible");
                else if (running)
                    ui.draw_message("Switched off to conserve battery",
                                    "Press the ON/EXIT key to resume");
                else if (showimage)
                    draw_power_off_image(0);
                else
                    lcd_refresh_wait();

                sys_critical_start();
                SET_ST(STAT_SUSPENDED);
                LCD_power_off(0);
                sys_timer_disable(TIMER0);
                sys_timer_disable(TIMER1);
                SET_ST(STAT_OFF);
                sys_critical_end();
            }
            // Already in OFF -> just continue to sleep above
        }

        else if (ST(STAT_CLK_WKUP_FLAG))
        {
            // Clock wakeup (once per second or per minute)
            CLR_ST(STAT_CLK_WKUP_FLAG);
            if (running)
                break;
            redraw_periodics();
        }
        else if (ST(STAT_POWER_CHANGE))
        {
            // Power state change (to/from USB)
            CLR_ST(STAT_POWER_CHANGE);
            sys_timer_disable(TIMER0);
            sys_timer_disable(TIMER1);
            // Force reload battery with correct value at next clock refresh.
            ui.draw_battery(true);
            program::last_interrupted -= Settings.BatteryRefresh() - 1000;
        }
        else
        {
            break;
        }
    }

    // Well, we are woken-up
    SET_ST(STAT_RUNNING);

    // Get up from OFF state
    if (ST(STAT_OFF))
    {
        LCD_power_on();

        // Ensure that RTC readings after power off will be OK
        rtc_wakeup_delay();

        CLR_ST(STAT_OFF);

        // Check if we need to redraw
        program::read_battery();
        if (ui.showing_graphics())
            ui.draw_graphics(true);
        else
            redraw_lcd(true);
    }

    // We definitely reached active state, clear suspended flag
    CLR_ST(STAT_SUSPENDED);

		LCD_UpdateDisplay(&hlcd);

}

#ifndef SIMULATOR
extern const uint prog_build_id;
extern const uint qspi_build_id;
#endif



extern "C" void program_main()
// ----------------------------------------------------------------------------
//   DMCP main entry point and main loop
// ----------------------------------------------------------------------------
{
    int  key        = 0;
    bool transalpha = false;


#if DM42
    if (prog_build_id != qspi_build_id)
    {
        msg_box(t24,
                "Incompatible " PROGRAM_NAME " build ID\n"
                "Please reload program and QSPI\n"
                "from the same build",
                true);
        lcd_refresh();
        wait_for_key_press();
        return;
    }
#endif


    // Initialization
    program_init();
    redraw_lcd(true);
    last_keystroke_time = program::read_time();

    // Main loop
    while (true)
    {
        // Check power state, and switch off if necessary
        power_check(false);

        // Key is ready -> clear auto off timer
        bool hadKey = false;

        if (!key_empty())
        {
            reset_auto_off();
            key    = key_pop();
            hadKey = true;
SEGGER_RTT_printf(0, "/nGot key %d", key);

#if !WASM

#if SIMULATOR
            // Process test-harness commands
            record(tests_rpl, "Processing key %d, last=%d, command=%u",
                   key, last_key, test_command);
            if (key == tests::EXIT_PGM || key == tests::SAVE_PGM)
            {
                cstring path = get_reset_state_file();
                printf("Exit: saving state to %s\n", path);
                if (path && *path)
                    save_state_file(path);
                if (key == tests::EXIT_PGM)
                    break;
            }


#else // Real hardware
#define read_key __sysfn_read_key
#endif // SIMULATOR
#endif // !WASM

            // Check transient alpha mode
            if (key == KEY_UP || key == KEY_DOWN)
            {
                transalpha = true;
            }
            else if (transalpha)
            {
                int k1, k2;
//                int r = read_key(&k1, &k2);
int r=0;
                switch (r)
                {
                case 0:
                    transalpha = false;
                    break;
                case 1:
                    transalpha = k1 == KEY_UP || k1 == KEY_DOWN;
                    break;
                case 2:
                    transalpha = k1 == KEY_UP || k1 == KEY_DOWN
                        ||       k2 == KEY_UP || k2 == KEY_DOWN;
                    break;
                }
            }

        }
        bool repeating = key > 0
            && sys_timer_active(TIMER0)
            && sys_timer_timeout(TIMER0);



        if (repeating)
        {
            hadKey = true;
            record(main, "Repeating key %d", key);
        }

        // Fetch the key (<0: no key event, >0: key pressed, 0: key released)
        record(main, "Testing key %d (%+s)", key, hadKey ? "had" : "nope");
        if (key >= 0 && hadKey)
        {
#if SIMULATOR && !WASM
            process_test_key(key);
#endif // SIMULATOR && !WASM

            record(main, "Handle key %d last %d", key, last_key);
            handle_key(key, repeating, transalpha);
            record(main, "Did key %d last %d", key, last_key);

            // Redraw the LCD unless there is some type-ahead
            if (key_empty())
                redraw_lcd(false);

            // Record the last keystroke
            last_keystroke_time = program::read_time();
            record(main, "Last keystroke time %u", last_keystroke_time);
        }
        else
        {
            // Blink the cursor
            if (sys_timer_timeout(TIMER1))
                redraw_periodics();
            if (!key)
                sys_timer_disable(TIMER0);
        }
#if SIMULATOR && !WASM
        if (tests::running && test_command && key_empty())
            process_test_commands();
#endif // SIMULATOR && !WASM
    }
}



#if WASM
uint            memory_size           = 100;
volatile uint   test_command          = 0;
bool            noisy_tests           = false;
bool            no_beep               = false;
bool            tests::running        = false;

static void *rpl_thread(void *)
// ----------------------------------------------------------------------------
//   Run the RPL thread
// ----------------------------------------------------------------------------
{
    record(main, "Entering main thread");
    program_main();
    return nullptr;
}


int ui_init()
// ----------------------------------------------------------------------------
//   Initialization for the JavaScript version
// ----------------------------------------------------------------------------
{
    recorder_trace_set(".*error.*|.*warn.*");
    record(main, "ui_init invoked");
    pthread_t rpl;
    int rc = pthread_create(&rpl, nullptr, rpl_thread, nullptr);
    record(main, "pthread_create returned %d, %s", rc, strerror(rc));
    return 42;
}

#endif // WASM



#if SIMULATOR
void process_test_key(int key)
// ----------------------------------------------------------------------------
//   Process commands from the test harness
// ----------------------------------------------------------------------------
{
    record(tests_rpl, "Process test key %d, last was %d, command %u",
           key, last_key, test_command);
    if (key > 0)
        last_key = key;
    else if (last_key > 0)
        last_key = -last_key;
    record(tests_rpl, "Set last_key to %d for key %d", last_key, key);
}


void process_test_commands()
// ----------------------------------------------------------------------------
//   Process commands from the test harness
// ----------------------------------------------------------------------------
{
    record(tests_rpl, "Process test command %u with last key %d",
           test_command, last_key);

    if (test_command == tests::CLEARERR)
    {
        record(tests_rpl, "Clearing errors for tests");
        rt.clear_error();
    }
    else if (test_command == tests::CLEAR)
    {
        record(tests_rpl, "Clearing editor and stack for tests");
        rt.clear_error();
        ui.clear_editor();
        rt.drop(rt.depth());
        while (rt.run_next(0));
    }
    else if (test_command == tests::KEYSYNC)
    {
        record(tests_rpl, "Key sync requested");
    }
    if (!ui.showing_graphics())
        redraw_lcd(true);
    record(tests_rpl, "Done redrawing LCD after command %u, last=%d",
           test_command, last_key);
    test_command = 0;
}
#endif // SIMULATOR






extern "C" void dbu585_main_new()
// ----------------------------------------------------------------------------
//   DMCP main entry point and main loop
// ----------------------------------------------------------------------------
{
	char buff[80] = {0};
    int  key        = 0;
	uint32_t key_tmp, key_p1, key_p2, key_p3, keybdata;
	uint32_t wt_sleeping = 20;
    bool transalpha = false;
	bool key_release = false;

	bool res_init_sram = bkSRAM_Init();
	SEGGER_RTT_printf(0,  "\nCheck Sram Backup :  %s", res_init_sram ? "ok":"initialized");

// lcd init
	LCD_Status_t lcd_res = LCD_Sharp_Init(&hlcd, true);
	SEGGER_RTT_printf(0, "\nLCD status : %s", LCD_status_Desc[lcd_res]);
	if (lcd_res != LCD_OK){
		blink_error(blk_lcd);
	}

    // Initialization
	program_init();
    redraw_lcd(true);
    last_keystroke_time = program::read_time();

    // Main loop
    while (true)
    {
// Check power state, and switch off if necessary
//      power_check(false);

        // Key is ready -> clear auto off timer
        bool hadKey = false;

		char result = OS_MAILBOX_GetTimed(&Mb_Keyboard, &keybdata, wt_sleeping);
		if (result == 0)
		{ // an event keyboard is here
			if (0xffffffff == keybdata){		
	 			ui.draw_message("F1 F6 EXIT : reboot");
				while(1){}
			}
			else if (0xfffffffe == keybdata){		
	 			ui.draw_message("rtc updated by sntp");
					OS_TASK_Delay(1000);

				hadKey = false;
				key = -1;
				key_release = false;
				

			}
			else {


				hadKey = true;
				key_tmp = keybdata & 0xff;
				if (key_tmp>100) { // release
					key = 0;
					key_tmp %=100;
					key_release = true;
				} else {
					key = key_DB_to_DM(key_tmp);
					key_release = false;
				}
				key_p1 = key_DB_to_DM((keybdata>>8) & 0xff);
				key_p2 = key_DB_to_DM((keybdata>>16) & 0xff);
				key_p3 = key_DB_to_DM((keybdata>>24) & 0xff);
			}
//        if (!key_empty())
//        {
//            reset_auto_off();
SEGGER_RTT_printf(0, "\nGot key %08x, %02d, %02d, %02d %s %02d", keybdata, key_p1, key_p2, key_p3, key_release ?"Rl":"--", key );

          // Check transient alpha mode
//				||       key == KEY_UP || key == KEY_DOWN;

// Check transient alpha mode
            if (key == KEY_UP || key == KEY_DOWN)
            {
                transalpha = true;
            }
            else if (transalpha)
            {
                    transalpha = key_p1 == KEY_UP || key_p1 == KEY_DOWN
                        ||       key_p2 == KEY_UP || key_p2 == KEY_DOWN;
            }
        } // end get key event
		else { // waiting time out
			if (key == 0) 	key = -1;
// gestion power off / sleeping à faire ici
		}
        bool repeating = key > 0
            && sys_timer_active(TIMER0)
            && sys_timer_timeout(TIMER0)
			;
        if (repeating )
        {
            hadKey = true;
            record(main, "Repeating key %d", key);
        }

        // Fetch the key (<0: no key event, >0: key pressed, 0: key released)
        record(main, "Testing key %d (%+s)", key, hadKey ? "had" : "nope");

        if (key >= 0 && hadKey)
        {
			snprintf(buff, sizeof(buff), "\n ==> %s %s", 				
				repeating ?"Rp":"--", 
				transalpha ?"Ta":"--");
			SEGGER_RTT_WriteString(0,  buff);

            record(main, "Handle key %d last %d", key, last_key);
            handle_key(key, repeating, transalpha);
            record(main, "Did key %d last %d", key, last_key);

            // Redraw the LCD unless there is some type-ahead
//            if (key_empty())		original
//            if (key_p1 == 0)
            redraw_lcd(false);

            // Record the last keystroke
            last_keystroke_time = program::read_time();
            record(main, "Last keystroke time %u", last_keystroke_time);
        }
        else
        {
            // Blink the cursor
            if (sys_timer_timeout(TIMER1))
                redraw_periodics();
            if (!key)
                sys_timer_disable(TIMER0);
        }
    }
}



