## works for STM32Fxx in general, the driver file is for STM32F401RE so plop in the correct one for your STM32Fxx
> I have not tried it with my H7 yet... it probably works.
>Uses the uVision recommended way to divert printf to uart.  Don't provide a _write function, instead:
>>// Define FILE structure required by Keil
>FILE __stdout;

>int fputc(int ch, FILE *f)
{
		(void)f;
    while (!(USART2->SR & USART_SR_TXE));  // Wait until transmit buffer is empty
    USART2->DR = ch;                       // Send character
    return ch;
}
>>  Also shows how to get voltage samples via ADC, init ADC and init usart.
