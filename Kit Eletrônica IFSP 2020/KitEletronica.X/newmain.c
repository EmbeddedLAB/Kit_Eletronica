#define F_CPU 16000000UL //frequência de trabalho
#include <avr/io.h> //definições do componente especificado
#include <avr/interrupt.h> //define algumas macros para as interrupções
#include <util/delay.h> //biblioteca para o uso das rotinas de delay

//Definições de macros para o trabalho com bits
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da variável Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado lógico do bit x da variável Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou 1 conforme leitura do bit

//Protótipo da função de interrupção, obrigatório para sua utilização
ISR(PCINT0_vect);
ISR(INT0_vect);
ISR(TIMER0_OVF_vect);

//Variáveis globais
volatile unsigned char freq_change = 0;
volatile unsigned int  tick_count = 0;
volatile unsigned int  tick_count_ref = 1000;

int main() {
    
    //Configuração das portas
    DDRB = 0b00000000;      //PORTB como entrada
    PORTB = 0b00111111;     //habilita pull-ups apenas nos botões
    DDRC = 0b11111111;      //PORTC definido como saída
    PORTC = 0b00000000;     //Desliga todas as saídas (exceto a borda de descida)
    DDRD = 0b11111011;      //Apenas o bit PD2 é entrada
    PORTD = 0b00000100;     //Habilita o resistor de pull-up do pino PD2
    
    //Configuração das interrupções externas
    PCICR = 0b00000001;     //hab. interrupção por mudança de sinal no PORTB
    PCMSK0 = 0b00111111;    //hab. os pinos PCINT0 a 5 para gerar interrupção
    EICRA = 0b00000010;     //A borda de descida gera uma interrupt em INT0
    EIMSK = 0b00000001;     //Habilita a interrupt INT0
    TCNT0 = 248;            //Inicia o timer para contagem de 500us
    
    //Configuração do Timer 0
    TCCR0B = 0b00000101; //TC0 com prescaler de 1024, a 16 MHz gera uma interrupção a cada 16,384 ms
    TIMSK0 = 0b00000001; //habilita a interrupção do TC0
    
    //posição inicial das chaves com debounce
    PORTC = (PINB & 0b00111100);
    set_bit(PORTC,1);
    
    sei();                  //habilita a chave geral das interrupções
    while (1) {
        //Função principal sem nenhuma instrução
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
    cpl_bit(PORTD, 3);                  //Gera frequência de 1khz
    tick_count++;
    if(tick_count == tick_count_ref){
        cpl_bit(PORTD, 4);              //Gera frequência variável
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