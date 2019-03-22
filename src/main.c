/**
  ******************************************************************************
  * @file    main.c
  * @author  Anton Kabanov
  * @version V1.0
  * @date    22-March-2019
  * @brief   Default main function.
  ******************************************************************************
*/
#include "stm32f10x.h"

#define IN_MS_TO_POPUGAIS(MS)	(2 * MS - 1)

void led_init(void);
void buttons_init(void);
void timer_init(void);

volatile uint16_t a = IN_MS_TO_POPUGAIS(500);
volatile uint16_t b = IN_MS_TO_POPUGAIS(500);

int main(void)
{
	led_init(); 	//Инициализируем LED (С13)
	buttons_init(); //Инициализируем кнопки 1-4
	timer_init();   //Инициализируем таймер 3

	uint32_t last_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);

	for(;;)
	{
		uint32_t curr_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11); //cчитываем логическое состояние вывода 11
		if (curr_state != last_state)  //если произошла смена state
		{
			TIM_Cmd(TIM3, DISABLE);
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET); //LED off (setting 1 in BSRR)
			if (curr_state)
			{
				a = UINT16_MAX;
			}
			else
			{
				if (TIM_GetCounter(TIM3) >= IN_MS_TO_POPUGAIS(100)) { a = TIM_GetCounter(TIM3);}
			}
				TIM_SetCounter(TIM3, 0);
				TIM_SetAutoreload(TIM3, a);
				TIM_Cmd(TIM3, ENABLE);
		}
		last_state = curr_state;
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6))
			{b=IN_MS_TO_POPUGAIS(2000);}
		else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14))
			{b=IN_MS_TO_POPUGAIS(3500);}
		else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5))
			{b=IN_MS_TO_POPUGAIS(5000);}
		else
			{b=IN_MS_TO_POPUGAIS(500);}
	};
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update))
	{
		TIM_ClearFlag(TIM3, TIM_IT_Update);
		// Считываем логическое состояние вывода светодиода и инвертируем состояние
		if ( GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) ) //Если диод НЕ горит (it's pushed up by def)
		{
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);  //Reset13 сбрось и тем самым зажги (состояние b)
			TIM_SetAutoreload(TIM3, b); //arr тут может меняться
		}
		else
		{
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET); //Set13 и выключи (состояние а)
			TIM_SetAutoreload(TIM3, a);
		}
	}
}

void led_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	//  настраиваем LED как выход (push/pull out speed 2MHz) RM p.160
	GPIO_InitTypeDef gpio_init;
		gpio_init.GPIO_Pin = GPIO_Pin_13; // | GPIO_Pin_15; if needed
		gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
		gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio_init);
}

void buttons_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef gpio1_init;
		gpio1_init.GPIO_Pin = GPIO_Pin_11;
		gpio1_init.GPIO_Speed = GPIO_Speed_2MHz;
		gpio1_init.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &gpio1_init);

	GPIO_InitTypeDef gpio2_init;
		gpio2_init.GPIO_Pin = GPIO_Pin_6;
		gpio2_init.GPIO_Speed = GPIO_Speed_2MHz;
		gpio2_init.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &gpio2_init);

	GPIO_InitTypeDef gpio3_init;
		gpio3_init.GPIO_Pin = GPIO_Pin_14;
		gpio3_init.GPIO_Speed = GPIO_Speed_2MHz;
		gpio3_init.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &gpio3_init);

	GPIO_InitTypeDef gpio4_init;
		gpio4_init.GPIO_Pin = GPIO_Pin_5;
		gpio4_init.GPIO_Speed = GPIO_Speed_2MHz;
		gpio4_init.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &gpio4_init);
}

void timer_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseInitTypeDef tim;
		tim.TIM_ClockDivision = TIM_CKD_DIV1;
		tim.TIM_CounterMode = TIM_CounterMode_Up;
		tim.TIM_Prescaler = 36000-1;
		tim.TIM_Period = 1000-1;
	TIM_TimeBaseInit(TIM3, &tim);

	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE); // Update  Interrupt Enable DMA/Interrupt Enable Register

	NVIC_InitTypeDef nvicInit;
		nvicInit.NVIC_IRQChannel = TIM3_IRQn;
		nvicInit.NVIC_IRQChannelCmd = ENABLE;
		nvicInit.NVIC_IRQChannelPreemptionPriority = 0;
		nvicInit.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvicInit);

	TIM_Cmd(TIM3, ENABLE); // Start count
}
