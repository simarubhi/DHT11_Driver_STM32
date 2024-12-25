/**
 *  @file dht11_driver.c
 *	@brief DHT11 Temperature Sensor Driver Library
 *  @date Created on: Dec 24, 2024
 *  @author Author: Simar Singh Ubhi
 *  @version 1.0.0
 *
 */

#include "dht11_driver.h"
#include <string.h>

//**********************Private Variables**********************//
static GPIO_TypeDef *dht_gpio_port; // GPIOA
static uint16_t dht_gpio_pin; // GPIO_PIN_1, GPIO_PIN_2, etc.
static TIM_HandleTypeDef *dht_tim; // &htim6, &htim12, etc.

//**********************Private Functions**********************//
/**
 *	@brief Microsecond delay
 *	@param us the duration of microseconds to delay
 */
static void delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(dht_tim, 0);
    while (__HAL_TIM_GET_COUNTER(dht_tim) < us);
}

/**
 *	@brief Check if timer has exceeded limit (used to stop infinite while loops)
 *	@param timeout_duration check if the timer has exceeded this duration
 */
static bool check_timeout(uint16_t timeout_duration) {
    return (__HAL_TIM_GET_COUNTER(dht_tim) > timeout_duration);
}

/**
 *	@brief Change GPIO Pin mode between output and input
 *	@param *pin_mode declare if pin mode should be output or input
 */
static void set_gpio_pin_mode(const char *pin_mode) {
    GPIO_InitTypeDef GPIO_Struct; // Used to HAL GPIO

    if (strcmp(pin_mode, "OUTPUT") == 0) {
        GPIO_Struct.Mode = GPIO_MODE_OUTPUT_PP;
    } else if (strcmp(pin_mode, "INPUT") == 0) {
        GPIO_Struct.Mode = GPIO_MODE_INPUT;
    } else {
        return;
    }

    // Standard GPIO initialization
    GPIO_Struct.Pin = dht_gpio_pin;
    GPIO_Struct.Pull = GPIO_NOPULL;
    GPIO_Struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    HAL_GPIO_Init(dht_gpio_port, &GPIO_Struct);
}

//**********************Public API Functions**********************//
/**
 *	@brief Initializes DHT11 port, pin, and timer
 *	@param *gpio_port usually GPIOA | GPIOB
 *	@param gpio_pin GPIO_PIN_1, GPIO_PIN_2, etc.
 *	@param *tim &htim6, &htim12, etc.
 */
void dht11_init(GPIO_TypeDef *gpio_port, uint16_t gpio_pin, TIM_HandleTypeDef *tim) {
    dht_gpio_port = gpio_port;
    dht_gpio_pin = gpio_pin;
    dht_tim = tim;

    HAL_TIM_Base_Start(tim); // Start timer count for rest of program
}

/**
 *	@brief Starts communication with DHT11 and reads and processes 40 bit response
 */
dht11_data dht11_data_read() {
    uint8_t data[40] = {0};
    dht11_data results = {0};
    results.error = false;


    //**********************Initialization Sequence Begin**********************//
    set_gpio_pin_mode("OUTPUT");

    HAL_GPIO_WritePin(dht_gpio_port, dht_gpio_pin, GPIO_PIN_RESET); // Pull down to start communication with DHT11
    HAL_Delay(20); // At least 18ms low pull required

    HAL_GPIO_WritePin(dht_gpio_port, dht_gpio_pin, GPIO_PIN_SET); // Pull up and wait for response
    set_gpio_pin_mode("INPUT"); // Change pin mode to input to receive DHT11 response

    __HAL_TIM_SET_COUNTER(dht_tim, 0); // Reset timer to check for timeout
    while (HAL_GPIO_ReadPin(dht_gpio_port, dht_gpio_pin) == GPIO_PIN_SET) {
        if (check_timeout(200)) { // If improper response from DHT11 timeout and throw error
            results.error = true;
            return results;
        }
    }

    __HAL_TIM_SET_COUNTER(dht_tim, 0);
    while (HAL_GPIO_ReadPin(dht_gpio_port, dht_gpio_pin) == GPIO_PIN_RESET) {
        if (check_timeout(200)) {
            results.error = true;
            return results;
        }
    }

    __HAL_TIM_SET_COUNTER(dht_tim, 0);
    while (HAL_GPIO_ReadPin(dht_gpio_port, dht_gpio_pin) == GPIO_PIN_SET) {
        if (check_timeout(200)) {
            results.error = true;
            return results;
        }
    }
    //**********************Initialization Sequence End**********************//

    // Read 40 bits of data
    for (uint8_t i = 0; i < 40; i++) {
        __HAL_TIM_SET_COUNTER(dht_tim, 0);
        while (HAL_GPIO_ReadPin(dht_gpio_port, dht_gpio_pin) == GPIO_PIN_RESET) { // Wait for DHT11 to pull high
            if (check_timeout(200)) {
                results.error = true;
                return results;
            }
        }

        delay_us(40);

        data[i] = (HAL_GPIO_ReadPin(dht_gpio_port, dht_gpio_pin) == GPIO_PIN_SET); // If DHT11 is still high then duration is of high is longer than 28us thus 1 indication, otherwise 0

        __HAL_TIM_SET_COUNTER(dht_tim, 0);
        while (HAL_GPIO_ReadPin(dht_gpio_port, dht_gpio_pin) == GPIO_PIN_SET) { // Wait for DHT11 to pull low indicating next bit
            if (check_timeout(200)) {
                results.error = true;
                return results;
            }
        }
    }

    uint8_t humidity_int = 0, humidity_dec = 0, temperature_int = 0, temperature_dec = 0;

    // First 8 bits humidity integer part
    for (uint8_t i = 0; i < 8; i++) {
        humidity_int = (humidity_int << 1) | data[i]; // Shift bits to the left by 1 make right most bit 1 if data[i] is 1
    }

    // Bits 8 to 15 humidity decimal part
    for (uint8_t i = 8; i < 16; i++) {
        humidity_dec = (humidity_dec << 1) | data[i];
    }

    // Bits 16 to 23 temperature integer part
    for (uint8_t i = 16; i < 24; i++) {
        temperature_int = (temperature_int << 1) | data[i];
    }

    // Bits 24 to 31 temperature decimal part
    for (uint8_t i = 24; i < 32; i++) {
        temperature_dec = (temperature_dec << 1) | data[i];
    }

    // Convert to floats
    results.humidity = humidity_int + (humidity_dec / 10.0);
    results.temperature = temperature_int + (temperature_dec / 10.0);

    return results;
}
