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
>> also I added the SVMFunctions with a trained svm_model.h for outlier detection
>> The elf is about 24K, so well under Keil 32K limit if that is a concern
>> The version of the program called BareBones is about the same code and the elf is 84K.
>> Anyway, the main thing is how to use printf the Keil way.
