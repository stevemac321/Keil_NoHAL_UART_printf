#include "stm32f4xx.h"
#include <stdio.h>
#include <stdio.h>

#define NUM_COLS 128
#define NUM_ROWS 10

// Define FILE structure required by Keil
FILE __stdout;

int fputc(int ch, FILE *f);
void uart2_init(void);
void adc1_init(void);
void print_live_row(int len);

int main(void)
{
	uart2_init();
	adc1_init();
	printf("test\n");
	
	for(int i=0; i < NUM_ROWS; i++) {
		 print_live_row(NUM_COLS);
	} 
}
void uart2_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // PA2 = TX, PA3 = RX
    GPIOA->MODER &= ~((3 << (2 * 2)) | (3 << (3 * 2)));
    GPIOA->MODER |=  ((2 << (2 * 2)) | (2 << (3 * 2)));

    GPIOA->AFR[0] &= ~((0xF << (4 * 2)) | (0xF << (4 * 3)));
    GPIOA->AFR[0] |=  ((7 << (4 * 2)) | (7 << (4 * 3))); // AF7

    USART2->BRR = 139; // 16MHz / 115200 = ~139
    USART2->CR1 = USART_CR1_TE | USART_CR1_UE;
}
// Implement fputc to redirect printf to USART2
int fputc(int ch, FILE *f)
{
		(void)f;
    while (!(USART2->SR & USART_SR_TXE));  // Wait until transmit buffer is empty
    USART2->DR = ch;                       // Send character
    return ch;
}
void adc1_init(void)
{
    // 1. Enable clocks for GPIOA and ADC1
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   // Enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;    // Enable ADC1 clock

    // 2. Configure PA0 as analog input (ADC_IN0)
    GPIOA->MODER |= GPIO_MODER_MODER0;     // MODER0 = 11 (analog mode)
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR0;    // No pull-up/pull-down

    // 3. Set ADC prescaler (PCLK2 divided by 4)
    ADC->CCR &= ~ADC_CCR_ADCPRE;
    ADC->CCR |= ADC_CCR_ADCPRE_0;          // ADCPRE = 01 ? PCLK2 / 4

    // 4. Configure ADC1
    ADC1->CR1 = 0;                         // 12-bit resolution, single conversion
    ADC1->CR2 = ADC_CR2_CONT;              // Continuous conversion mode (optional)

    ADC1->SQR1 = 0;                        // One conversion in sequence
    ADC1->SQR3 = 0;                        // Channel 0 (PA0)

    ADC1->SMPR2 |= ADC_SMPR2_SMP0_2;       // Sample time = 144 cycles

    // 5. Enable ADC
    ADC1->CR2 |= ADC_CR2_ADON;             // Power on ADC
    for (volatile int i = 0; i < 1000; ++i); // Brief delay

    // Optional: Start conversion
    // ADC1->CR2 |= ADC_CR2_SWSTART;
}
void print_live_row(int len)
{
	for (int i = 0; i < len; ++i) {
		// Start ADC conversion
		ADC1->CR2 |= ADC_CR2_SWSTART;

		// Wait until conversion is complete
		while (!(ADC1->SR & ADC_SR_EOC));

		// Read ADC value
		uint16_t raw = ADC1->DR;

		// Convert to voltage (assuming Vref = 3.3V and 12-bit resolution)
		float f = (float)raw * (3.3f / 4095.0f);
		printf("%.5f ", f);
	}
	printf("\n");
}

