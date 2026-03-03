// #define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include "uart_stdio.h"
#include "led.h"
#include "pir.h"
#include "display.h"
#include "timer.h"
#include "wifi.h"
#include "button.h"

// #include "uart.h"
#define MAX_STRING_LENGTH 100
#define MAX_MENU_OPTIONS 6

static bool _pir_active = false;
static int x = 0;
static char tmp_buff1[15];

static char _tcp_receive_buff[MAX_STRING_LENGTH] = {0};
static bool _tcp_string_received = false;
static char _stdio_input_buff[MAX_STRING_LENGTH] = {0};

char welcome_text[] = "Welcome from SEP4 IoT hardware!\n";

uint8_t menu(void)
{
    int choice = 0;
    puts("VIA UNIVERSITY COLLEGE SEP4 IoT Hardware DRIVERS DEMO");
    puts("\tMenu:");
    puts("\t1. Button and LED");
    puts("\t2. PIR Sensor");
    puts("\t3. Display");
    puts("\t4. WiFi");
    puts("\t5. stdio");
    puts("\t6. Timer");

    printf("Choose a driver to test (1-%d): ", MAX_MENU_OPTIONS);
    stdin_flush(); // Flush any leftover '\n' input from the buffer
    do
    {
        scanf("%d", &choice);
        printf("%d\n", choice);
        if(choice < 1 || choice > MAX_MENU_OPTIONS)
        {
            // Invalid input, clear the input buffer
            while (getchar() != '\n')
                ;
            printf("Invalid input. Please enter a number between 1 and %d: ", MAX_MENU_OPTIONS);
        }
    } while (choice < 1 || choice > MAX_MENU_OPTIONS);

    return (uint8_t)choice;
}

void pir_callback(void)
{
    if (_pir_active)
    {
        if (pir_get_state() == PIR_NO_MOTION)
        {
            led_off(1);
        }
        else
        {
            led_on(1);
        }
    }
}

void led2_callback(uint8_t id)
{
    static int8_t led_no = 2;
    led_toggle(led_no);
}

void start_stop_timer(uint8_t id)
{
    if (timer_get_state(id)) // Check if LED2 timer is active
        timer_pause(id);     // Pause the LED2 timer
    else
        timer_resume(id); // Resume the LED2 timer
}

void wifi_line_callback(const char *line)
{
    uint8_t _index;
    _index = strlen(_tcp_receive_buff);
    _tcp_receive_buff[_index] = '\r';
    _tcp_receive_buff[_index + 1] = '\n';
    _tcp_receive_buff[_index + 2] = '\0';
    _tcp_string_received = true;
}

static bool _quit()
{
    return (gets_nonblocking(tmp_buff1, sizeof(tmp_buff1)) > 0 && tmp_buff1[0] == 'q');
}

int main(void)
{
    static int led2_timer_id = 0;

    led_init();
    button_init();
    display_init();
    pir_init(pir_callback);

    // Initialize UART stdio at 115200 baud. Must be same on terminal.
    if (UART_OK != uart_stdio_init(115200))
    {
        led_on(4); // Turn on LED4 to indicate error
        while (1)
            ;
    }
    sei(); // Enable global interrupts

    while (1)
    {
        switch (menu())
        {
        case 1:
            puts("Button and LED driver. Type 'q' to exit.");
            puts("LED 4 vill blink. Push a button to light one of the other LEDs.");
            led_blink(4, 500); // Blink LED4 with 5000ms periode
            do
            {
                (button_get(1)) ? led_on(1) : led_off(1);
                (button_get(2)) ? led_on(2) : led_off(2);
                (button_get(3)) ? led_on(3) : led_off(3);
                _delay_ms(200);
            } while (!_quit());
            led_off(4);
            break;
        case 2:
            _pir_active = true;
            puts("PIR sensor driver. Type 'q' to exit.");
            puts("LED 1 should turn on when motion is detected.");
            do
            {
                _delay_ms(200);
            } while (!_quit());
            _pir_active = false;
            break;
        case 3:
            puts("Display driver. Type 'q' to exit.");
            puts("Type a number between -999 and 9999");
            while (scanf("%d", &x) == 1)
            {
                display_int(x);
                printf("Du skrev: %d\n", x);
            }
            break;
        case 4:
            wifi_init();
            puts("WiFi driver demo. Press Reset to exit.");
            puts("(SSID, password, server IP and port are hardcoded, change if needed)");
            wifi_command_join_AP("Erlands SEP4", "ViaUC1234");
            wifi_command_create_TCP_connection("10.184.216.102", 23, wifi_line_callback, _tcp_receive_buff);
            wifi_command_TCP_transmit((uint8_t *)welcome_text, strlen(welcome_text));

            while (1)
            {
                if (_tcp_string_received)
                {
                    printf("TCP received: %s\n", _tcp_receive_buff);
                    _tcp_receive_buff[0] = '\0';
                    _tcp_string_received = false;
                }
                if (gets_nonblocking(_stdio_input_buff, MAX_STRING_LENGTH) > 0)
                {
                    printf("You wrote: %s\n", _stdio_input_buff);
                    wifi_command_TCP_transmit((uint8_t *)_stdio_input_buff, strlen(_stdio_input_buff));
                }
                _delay_ms(200);
            }
            break;
        case 5:
            printf("stdio driver. Type a text to echo to the terminal.\n");
            int ch;
            stdin_flush(); // Flush any leftover input from the buffer
            do
            {
                ch = getchar();
                putchar(ch);
            } while (ch != '\n' && ch != EOF);
            break;
        case 6:
            puts("Timer driver demo. Type 'q' to exit.");
            puts("LED 2 will toggle every 100ms. Pressing button 2 will pause/resume the blinking.");
            if ((led2_timer_id = timer_create_sw(led2_callback, 100)) < 0)
            {
                puts("Timer create failed");
                _delay_ms(2000);
                break;
            }
            do
            {
                if(button_get(2)) 
                {
                    start_stop_timer(led2_timer_id); // Call the start/stop callback to toggle the timer
                    _delay_ms(300); // Debounce delay
                }
            } while (!_quit());
            led_off(2);
            timer_delete(led2_timer_id);
            break;
        default:
            printf("Ugyldigt valg.\n");
            continue; // Gå tilbage til menuen
        }
    }
    return 0;
}