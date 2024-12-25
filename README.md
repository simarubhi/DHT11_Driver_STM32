
# DHT11 Temperature Sensor Driver Library For STM32

Driver library for the DHT11 temperature sensor for STM32 microcontrollers




## Usage

```c
#include "dht11_driver.h"

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
    
    ...
  }

}
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
    
    if (data.error) {
		  sprintf((char *)buffer, "There is an error reading the DHT11\r\n");
	  } else {
		  sprintf((char *)buffer, "Temperature: %.1f C, Humidity: %.1f%%\r\n", data.temperature, data.humidity);
	  }

	  HAL_UART_Transmit(&huart2, buffer, strlen((char *)buffer), HAL_MAX_DELAY);

	  HAL_Delay(3000);

    ...
  }

}
```

## Lessons Learned

What I learned

