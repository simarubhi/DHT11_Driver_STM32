
# DHT11 Temperature Sensor Driver Library For STM32

![DHT11](https://img.shields.io/badge/Module-DHT11-blue)
![STM32](https://img.shields.io/badge/Microcontroller-STM32-white)
![Project Status](https://img.shields.io/badge/Project-Finished-brightgreen)


Driver library for the DHT11 temperature sensor for STM32 microcontrollers



## Usage
### Setup

Prior to generating code in STM32CubeIDE, set a basic timer (usually tim6 or tim7) to 1us.


![Clock Configuration](https://github.com/simarubhi/DHT11_Driver_STM32/blob/main/docs/clock_config.png)

The example below uses timer 6 which is a basic timer on the STM32 Nucleo F446RE board. It is configured to 84Mhz by default when checking the clock configuration tab of the IOC file. After checking what frequency your timer is running at go to the Pinout & Configuration tab, the timer you have chosen, and set the prescaler to whatever frequency -1 your timer is. Also ensure to set the counter period to the maximum value (usually 0xffff-1). Lastly set any GPIO capable pin of your choice to gpio_output. In the below example GPIO_PIN_1 has been chosen.

![Clock Prescaler](https://github.com/simarubhi/DHT11_Driver_STM32/blob/main/docs/clock_prescaler.png)


### Basic Implementation

main.c

```c
#include "dht11_driver.h"

...

int main(void)
{
  ...

  dht11_init(GPIOA, GPIO_PIN_1, &htim6); // Port, Pin, Timer

  ...

  while (1)
  {
    ...
    
    dht11_data data = dht11_data_read();
    
    ...

    HAL_Delay(3000); // Recommended to have at least 3 second delay between reads
  }

}
```

### Available Properties
```c
/**
 * @brief DHT11 data structure
 */
typedef struct {
    float humidity;      // Humidity value
    float temperature;   // Temperature value
    bool error;          // Indicates if there was an error
} dht11_data;
```

main.c
```c
dht11_data data = dht11_data_read(); // Get data from DHT11
float humidity = data.humidity; // Get the humidity
float temperature = data.temperature; // Get the temperature
bool error = data.error; // Check if there was an error in getting data (requires #include <stdbool.h>)

```

## Displaying Data Via UART

```c
#include "dht11_driver.h"
#include "stdio.h"
#include "string.h"

...

int main(void)
{
  ...

  dht11_init(GPIOA, GPIO_PIN_1, &htim6);

  ...

  while (1)
  {
    ...
    
    dht11_data data = dht11_data_read();
    
    if (data.error)
    {
	sprintf((char *)buffer, "There is an error reading the DHT11\r\n");
    }
    else
    {
	sprintf((char *)buffer, "Temperature: %.1f C, Humidity: %.1f%%\r\n", data.temperature, data.humidity);
    }

    HAL_UART_Transmit(&huart2, buffer, strlen((char *)buffer), HAL_MAX_DELAY);

    HAL_Delay(3000);

    ...
  }

}
```
![UART Demo](https://github.com/simarubhi/DHT11_Driver_STM32/blob/main/docs/uart_demo.png)


## 🔗 My Links
[![portfolio](https://img.shields.io/badge/my_portfolio-000?style=for-the-badge&logo=ko-fi&logoColor=white)](https://simarubhi.com/)
[![linkedin](https://img.shields.io/badge/linkedin-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/simar-ubhi/)

