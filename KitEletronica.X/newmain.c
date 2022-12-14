#define F_CPU 16000000UL //frequ�ncia de trabalho
#include <avr/io.h> //defini��es do componente especificado
#include <avr/interrupt.h> //define algumas macros para as interrup��es
#include <util/delay.h> //biblioteca para o uso das rotinas de delay

//Defini��es de macros para o trabalho com bits
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da vari�vel Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da vari�vel Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado l�gico do bit x da vari�vel Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou 1 conforme leitura do bit

//Prot�tipo da fun��o de interrup��o, obrigat�rio para sua utiliza��o
ISR(PCINT0_vect);
ISR(INT0_vect);
ISR(TIMER0_OVF_vect);

//Vari�veis globais
volatile unsigned char freq_change = 0;
volatile unsigned int  tick_count = 0;
volatile unsigned int  tick_count_ref = 1000;

int main() {
    
    //Configura��o das portas
    DDRB = 0b00000000;      //PORTB como entrada
    PORTB = 0b00111111;     //habilita pull-ups apenas nos bot�es
    DDRC = 0b11111111;      //PORTC definido como sa�da
    PORTC = 0b00000000;     //Desliga todas as sa�das (exceto a borda de descida)
    DDRD = 0b11111011;      //Apenas o bit PD2 � entrada
    PORTD = 0b00000100;     //Habilita o resistor de pull-up do pino PD2
    
    //Configura��o das interrup��es externas
    PCICR = 0b00000001;     //hab. interrup��o por mudan�a de sinal no PORTB
    PCMSK0 = 0b00111111;    //hab. os pinos PCINT0 a 5 para gerar interrup��o
    EICRA = 0b00000010;     //A borda de descida gera uma interrupt em INT0
    EIMSK = 0b00000001;     //Habilita a interrupt INT0
    TCNT0 = 248;            //Inicia o timer para contagem de 500us
    
    //Configura��o do Timer 0
    TCCR0B = 0b00000101; //TC0 com prescaler de 1024, a 16 MHz gera uma interrup��o a cada 16,384 ms
    TIMSK0 = 0b00000001; //habilita a interrup��o do TC0
    
    //posi��o inicial das chaves com debounce
    PORTC = (PINB & 0b00111100);
    set_bit(PORTC,1);
    
    sei();                  //habilita a chave geral das interrup��es
    while (1) {
        //Fun��o principal sem nenhuma instru��o
    }
}
//------------------------------------------------------------------------------
ISR(INT0_vect){
 
    _delay_ms(10);
    freq_change++;
    if(freq_change == 3)    freq_change = 0;
    
    if(freq_change == 0)         tick_count_ref = 1000;
    else if(freq_change == 1)    tick_count_ref = 100;
    else if(freq_change == 2)    tick_count_ref = 10;
    tick_count = 0;
}

//------------------------------------------------------------------------------
ISR(TIMER0_OVF_vect){
    
    TCNT0 = 248;
    cpl_bit(PORTD, 3);                  //Gera frequ�ncia de 1khz
    tick_count++;
    if(tick_count == tick_count_ref){
        cpl_bit(PORTD, 4);              //Gera frequ�ncia vari�vel
        tick_count = 0;
    }
    


}

//------------------------------------------------------------------------------
ISR(PCINT0_vect) {
    //Testa qual o pino que foi acionado
    //***************************
    //Borda de subida - S1
    if (!(tst_bit(PINB, 0)))
        set_bit(PORTC, 0);
    
    else if (tst_bit(PINB, 0))
        clr_bit(PORTC, 0);
    //***************************
    //Borda de descida - S2
    if (!(tst_bit(PINB, 1)))
        clr_bit(PORTC, 1);
            
    else if (tst_bit(PINB, 1))
        set_bit(PORTC, 1);
    //***************************
    //Chave S3
    if (!(tst_bit(PINB, 2)))
        clr_bit(PORTC, 2);
            
    else if (PINB & (1 << 2))
        set_bit(PORTC, 2);
    //***************************
    //Chave S4
    if (!(tst_bit(PINB, 3)))
        clr_bit(PORTC, 3);
            
    else if (PINB & (1 << 3))
        set_bit(PORTC, 3);
    //***************************
    //Chave S5
    if (!(tst_bit(PINB, 4)))
        clr_bit(PORTC, 4);
            
    else if (PINB & (1 << 4))
        set_bit(PORTC, 4);
    //***************************
    //Chave S6
    if (!(tst_bit(PINB, 5)))
        clr_bit(PORTC, 5);
            
    else if (PINB & (1 << 5))
        set_bit(PORTC, 5);
    //***************************
    
    _delay_ms(10);
}
//------------------------------------------------------------------------------