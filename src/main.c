/**
  ******************************************************************************
  * @file    main.c
  * @author  Anton Kabanov
  * @version V1.0
  * @date    07-December-2018
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
//		if ( GPIOA->IDR & GPIO_IDR_IDR6  ) {b=IN_MS_TO_POPUGAIS(2000);}
//		// если мы хотим, чтобы при отключении кнопки вновь b=500ms нужно сделать curr & last states этой кнопки
//		if ( GPIOC->IDR & GPIO_IDR_IDR14 ) {b=IN_MS_TO_POPUGAIS(3500);}
//		if ( GPIOA->IDR & GPIO_IDR_IDR5  ) {b=IN_MS_TO_POPUGAIS(5000);}
//		if (~(GPIOA->IDR & GPIO_IDR_IDR6) & ~(GPIOC->IDR & GPIO_IDR_IDR14) & ~(GPIOA->IDR & GPIO_IDR_IDR5))
//		{
//			{b=IN_MS_TO_POPUGAIS(500);}
//		}
		if (GPIOA->IDR & GPIO_IDR_IDR6)
			{b=IN_MS_TO_POPUGAIS(2000);}
		else if (GPIOC->IDR & GPIO_IDR_IDR14)
			{b=IN_MS_TO_POPUGAIS(3500);}
		else if (GPIOA->IDR & GPIO_IDR_IDR5)
			{b=IN_MS_TO_POPUGAIS(5000);}
		else
			{b=IN_MS_TO_POPUGAIS(500);}
	};
}

void TIM3_IRQHandler(void)
//Функция обработчика прерывания от таймера 3
{
//  Если произошло прерывание по переполнению таймера
//	Сбрасываем флаг переполнения
	if (TIM3->SR & TIM_SR_UIF)
	{
		TIM3->SR &= ~TIM_SR_UIF; // Clean UIF Flag
		// Считываем логическое состояние вывода светодиода и инвертируем состояние
		if ( GPIOC->ODR & GPIO_ODR_ODR13 ) //Если диод НЕ горит (it's pushed up by def)
		{
			GPIOC->BSRR = GPIO_BSRR_BR13;  //Reset13 сбрось и тем самым зажги (состояние b)
			TIM3->ARR = b; //arr тут может меняться
		}
		else
		{
			GPIOC->BSRR = GPIO_BSRR_BS13; //Set13 и выключи (состояние а)
			TIM3->ARR = a;
		}
	}
	else
//	Прерывание произошло от триггера
	{
		TIM3->SR &= ~TIM_SR_TIF; // Cleaning TIF Flag
		TIM3->EGR |= TIM_EGR_UG; //Updating generation
		GPIOC->ODR |=GPIO_ODR_ODR13; //гасим диод
		TIM3->PSC = 36000 - 1;
		TIM3->ARR = UINT16_MAX;
		TIM3->CR1 |= TIM_CR1_CEN; // Start count
	}
}

void led_init(void)
// В этой процедуре инициализируем LED
{
//  Включаем тактирование порта C
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
//  сброс состояния порта С pin 13
	GPIOC->CRH &= ~(GPIO_CRH_CNF13_1 | GPIO_CRH_CNF13_0 | GPIO_CRH_MODE13_1 | GPIO_CRH_MODE13_0);
//  настраиваем LED как выход (push/pull out speed 2MHz) RM p.160
	GPIOC->CRH |= GPIO_CRH_MODE13_1;
}

void buttons_init(void)
// В этой инициализируем кнопки 1-4
{
	// Включаем тактирование недостающих портов А и В (C уже включен)
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN; //  | RCC_APB2ENR_IOPCEN;

	// Сбрасываем все 4 кнопки в соответствии с вариантом 7
	// сброс состояния порта  А pin 6 (кнопка 2) и
	GPIOA->CRL &= ~(GPIO_CRL_CNF6_1 | GPIO_CRL_CNF6_0 | GPIO_CRL_MODE6_1 | GPIO_CRL_MODE6_0
							  // pin 5 (кнопка 4)
				  | GPIO_CRL_CNF5_1 | GPIO_CRL_CNF5_0 | GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0);
	// сброс состояния порта  B pin 11 (кнопка 1)
	GPIOB->CRH &= ~(GPIO_CRH_CNF11_1 | GPIO_CRH_CNF11_0 | GPIO_CRH_MODE11_1 | GPIO_CRH_MODE11_0);
	// сброс состояния порта  C pin 14 (кнопка 3)
	GPIOC->CRH &= ~(GPIO_CRH_CNF14_1 | GPIO_CRH_CNF14_0 | GPIO_CRH_MODE14_1 | GPIO_CRH_MODE14_0);

	// Настраиваем все 4 кнопки, как вход с подтяжкой к земле (pulled-down) RM p.160
	GPIOA->CRL |= GPIO_CRL_CNF6_1 | GPIO_CRL_CNF5_1; //кнопки 2 и 4
	GPIOA->ODR &= ~(GPIO_ODR_ODR6 | GPIO_ODR_ODR5); // хотя это и не обязательно, там и так нули

	GPIOB->CRH |= GPIO_CRH_CNF11_1; //кнопка 1
	GPIOB->ODR &= ~GPIO_ODR_ODR11;

	GPIOC->CRH |= GPIO_CRH_CNF14_1; //кнопка 3
	GPIOC->ODR &= ~GPIO_ODR_ODR14;
}

void timer_init(void)
//инициализация таймера 3
{
//  Включаем тактирование
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
//  Разрешаем прерывания по переполнению таймера и по triggery
//  DMA/Interrupt Enable Register
	TIM3->DIER |= TIM_DIER_UIE; // Update  Interrupt Enable
//	TIM3->DIER |= TIM_DIER_TIE; // Trigger Interrupt Enable
//  Включение прерывания таймера
	NVIC_EnableIRQ(TIM3_IRQn);
//  Запускаем таймер 3 на тактовой частоте в 1 kHz
	TIM3->PSC = 36000 - 1;
//  Период - 1000 тактов => 1000/1000 = 1 Hz
	TIM3->ARR = 1000 - 1;
	TIM3->CR1 |= TIM_CR1_CEN; // Start count
}
