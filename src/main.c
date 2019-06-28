/**
  ******************************************************************************
  * @file    main.c
  * @author  Anton Kabanov 184-311
  * @version V1.0
  * @date    18-June-2019
  * @brief   Default main function.
  ******************************************************************************
*/
#include "stm32f1xx.h"

//#define TRUE 1
//#define FALSE 0
#define buff_size 128

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

//кольцевой буфер1, заданный массивом
uint8_t buff1[buff_size-1];
uint8_t write1 = 0;
uint8_t read1 = 0;
//uint8_t counter1 = 0;

//кольцевой буфер2, заданный массивом
uint8_t buff2[buff_size-1];
uint8_t write2 = 0;
uint8_t read2 = 0;
//uint8_t counter2 = 0;

uint8_t mess[] = "The MCU is ready to receive messages through UART1\r\n";
uint8_t rcvd_data1;
uint8_t rcvd_data2;

//uint8_t reception_complete = FALSE;
//uint8_t transmission_complete = FALSE;

//struct FIFO
//	{
//		uint8_t data[buff_size];
//		uint8_t read;
//		uint8_t write;
//		uint8_t counter;
//	};
//struct FIFO buff1 = {127, 0, 0, 0};

//struct FIFO
//{
//	uint8_t data[buff_size];
//	uint8_t read;
//	uint8_t write;
//	uint8_t counter;
//} buff1;
//
//struct FIFO buff2;

void SystemClockInit(void);
//void GPIO_Init(void);
//void LED_Init(void);
void UART1_Init(void);
void UART2_Init(void);
void Error_handler(void);
//void FIFO_Init(void);

int main(void)
{
	HAL_Init();
	SystemClockInit();
//	LED_Init();
	//GPIO_Init();
	UART1_Init();
	UART2_Init();
//	FIFO_Init();
//	struct FIFOBuff buff = {500, 0, 0, 0};
//	buff1.read = 0;
//	buff1.write = 0;
//	buff1.counter = 0;
//	buff2.read = 0;
//	buff2.write = 0;
//	buff2.counter = 0;
	HAL_UART_Transmit(&huart1, mess, sizeof(mess)+1, HAL_MAX_DELAY);

	HAL_UART_Receive_IT(&huart1, &rcvd_data1, 1);
	HAL_UART_Receive_IT(&huart2, &rcvd_data2, 1);

	for(;;)
	{
	}
}

void SystemClockInit(void)
{
	RCC_OscInitTypeDef osc;
	osc.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	osc.HSEState = RCC_HSE_ON;
	osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	osc.HSIState = RCC_HSI_OFF;
	osc.LSEState = RCC_LSE_OFF;
	osc.LSIState = RCC_LSI_OFF;
	osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc.PLL.PLLMUL = RCC_PLL_MUL9;
	osc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	osc.PLL.PLLState = RCC_PLL_ON;
	if (HAL_RCC_OscConfig(&osc) != HAL_OK)
	{
		Error_handler();
	}

	RCC_ClkInitTypeDef clk;
	clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clk.APB1CLKDivider = RCC_HCLK_DIV2;
	clk.APB2CLKDivider = RCC_HCLK_DIV1;
	clk.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK |
					RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_handler();
	}
}

void UART1_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 19200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_handler();
	}
}

void UART2_Init(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 57600;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_2;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		  Error_handler();
	}
}

void GPIO_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
}

//void LED_Init()
//{
//	__HAL_RCC_GPIOC_CLK_ENABLE();
//	GPIO_InitTypeDef led;
//	led.Mode = GPIO_MODE_OUTPUT_PP;
//	led.Pin = GPIO_PIN_13;
//	led.Pull = GPIO_NOPULL;
//	led.Speed = GPIO_SPEED_FREQ_LOW;
//	HAL_GPIO_Init(GPIOC, &led);
//}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_uarts_init;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    if(huart->Instance==USART1)
    {
    	__HAL_RCC_USART1_CLK_ENABLE();
	    gpio_uarts_init.Pin = GPIO_PIN_9;
	    gpio_uarts_init.Mode = GPIO_MODE_AF_PP;
	    gpio_uarts_init.Speed = GPIO_SPEED_FREQ_LOW;
	    HAL_GPIO_Init(GPIOA, &gpio_uarts_init);

	    gpio_uarts_init.Pin = GPIO_PIN_10;
	    gpio_uarts_init.Mode = GPIO_MODE_INPUT;
	    gpio_uarts_init.Pull = GPIO_NOPULL;
	    HAL_GPIO_Init(GPIOA, &gpio_uarts_init);

	    HAL_NVIC_SetPriority(USART1_IRQn,15,0);
	    HAL_NVIC_EnableIRQ(USART1_IRQn);

    }
    if(huart->Instance==USART2)
    {
    	__HAL_RCC_USART2_CLK_ENABLE();
    	gpio_uarts_init.Pin = GPIO_PIN_2;
    	gpio_uarts_init.Mode = GPIO_MODE_AF_PP;
    	gpio_uarts_init.Speed = GPIO_SPEED_FREQ_LOW;
    	HAL_GPIO_Init(GPIOA, &gpio_uarts_init);

    	gpio_uarts_init.Pin = GPIO_PIN_3;
    	gpio_uarts_init.Mode = GPIO_MODE_INPUT;
    	gpio_uarts_init.Pull = GPIO_NOPULL;
    	HAL_GPIO_Init(GPIOA, &gpio_uarts_init);
    	HAL_NVIC_SetPriority(USART2_IRQn,15,0);
    	HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
}

//void FIFO_Init(void)
//{
//	buff1.read = 0;
//	buff1.write = 0;
//	buff1.counter = 0;
//	buff1.data[counter] = 0;
//	buff2.read = 0;
//	buff2.write = 0;
//	buff2.counter = 0;
//}

void HAL_MspInit(void)
{
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)
	{
		HAL_UART_Receive_IT(&huart1, &rcvd_data1, 1);
		uint8_t tmp1 = rcvd_data1;
		HAL_NVIC_DisableIRQ(USART1_IRQn);
			buff1[write1] = tmp1;
			if ((write1-read1)<(buff_size-1))
			{
				if (write1 == buff_size-1)
				{
					write1 = 0;
				}
				else
				{
					write1++;
				}
			}
		HAL_NVIC_EnableIRQ(USART1_IRQn);
//		HAL_UART_Receive_IT(&huart1, &rcvd_data1, 1);
//		if (write1 > read1)
//		{
			HAL_UART_Transmit_IT(&huart2, &buff1[read1], 1);

//		}
//		HAL_UART_Receive_IT(&huart1, &rcvd_data1, 1);
	}
	if(huart->Instance==USART2)
	{
		HAL_UART_Receive_IT(&huart2, &rcvd_data2, 1);
		uint8_t tmp2 = rcvd_data2;
		HAL_NVIC_DisableIRQ(USART2_IRQn);
			buff2[write2] = tmp2;
			if ((write2-read2)<(buff_size-1))
			{
				if (write2 == buff_size-1)
				{
					write2 = 0;
				}
				else
				{
					write2++;
				}
			}
		HAL_NVIC_EnableIRQ(USART2_IRQn);
//		HAL_UART_Receive_IT(&huart2, &rcvd_data2, 1);
//		if (write2 > read2)
//		{
			HAL_UART_Transmit_IT(&huart1, &buff2[read2], 1);

//		}
//		HAL_UART_Receive_IT(&huart2, &rcvd_data2, 1);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)
	{
//		HAL_NVIC_DisableIRQ(USART1_IRQn);
		if ((write1-read1)<(buff_size-1))
		{
			if (read1 == buff_size-1)
			{
				read1 = 0;
			}
			else
			{
				read1++;
			}
		}
//		HAL_NVIC_EnableIRQ(USART1_IRQn);
//		HAL_UART_Receive_IT(&huart1, &rcvd_data1, 1);
//		if (write1 > read1)
//				{
//					HAL_UART_Transmit_IT(&huart2, &buff1[read1], 1);
//				}

	}
	if(huart->Instance==USART2)
	{
//		HAL_NVIC_DisableIRQ(USART2_IRQn);
		if ((write2-read2)<(buff_size-1))
		{
			if (read2 == buff_size-1)
			{
				read2 = 0;
			}
			else
			{
				read2++;
			}
		}
//		HAL_NVIC_EnableIRQ(USART2_IRQn);
//		HAL_UART_Receive_IT(&huart2, &rcvd_data2, 1);
//		if (write2 > read2)
//				{
//					HAL_UART_Transmit_IT(&huart1, &buff2[read2], 1);
//				}
	}
}

void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart2);
}

void Error_handler()
{
	while(1)
	{
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//		HAL_Delay(100);
	}
}
