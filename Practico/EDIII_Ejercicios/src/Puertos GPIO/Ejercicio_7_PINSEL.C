/*Utilice dos grupos de 4 pines de entrada (A_in, B_in) y un interruptor para seleccionar una operación.
• El interruptor (conectado a un pin) debe decidir si los valores de A_in y B_in se suman o se restan.
• El valor absoluto del resultado debe mostrarse en 4 LEDs de salida.
• Utilice un LED adicional para mostrar si se genera un overflow en la suma o si el resultado de la resta es
negativo
 *
 *
 * */


#include "LPC17xx.h"
#include <stdbool.h>

typedef enum {
	WAIT_A,
	WAIT_B,
	WAIT_OP,
	EXCE
}ESTADOS;

typedef enum{
	suma ,//suma
	resta //resta
}OPCODE;
void configOpeatorA(){
	//Configuramos los pines P0.0 al P0.3  inclusive
	LPC_PINCON->PINSEL0 &= ~ (0XFF << 0);
	LPC_PINCON->PINMODE0 |= (0XFF << 0);

	LPC_GPIO0->FIODIR &=~(0XF <<0);

}

void configOperatorB(){
	//Configuramos los pines P2.0 al P2.3
	LPC_PINCON->PINSEL4 &= ~ (0XFF << 0);
	LPC_PINCON ->PINMODE4 |= (0XFF << 0);

	LPC_GPIO2->FIODIR &=~(0XF <<0);
}
void configPinOpcode (){
	LPC_PINCON->PINSEL0 &= ~ (3 << 8);
	LPC_PINCON ->PINMODE0 |= (3 << 8);

	LPC_GPIO0->FIODIR &=  ~ (1 << 4);

}
void configPinsOutput (){
	LPC_PINCON->PINSEL1 &=~ (0XFF <<12);
	LPC_PINCON->PINMODE1 |= (0XFF <<12);


	LPC_GPIO0->FIODIR |= (0X0F << 22);
	LPC_GPIO0->FIOCLR |= (0X0F << 22);
}
void configFlags (){
	LPC_PINCON->PINSEL0 &= ~ (0X0F<<10);
	LPC_PINCON ->PINMODE0 |= (0X0F<<10);

	LPC_GPIO0->FIODIR |= (3<<5);
	LPC_GPIO0->FIOCLR |= (3<<5);
}
void display(uint8_t r){

	LPC_GPIO0 -> FIOSET = (r) & (0x0F<<22);
}
void sumar(uint8_t op_a , uint8_t op_b){

	uint8_t result = op_a + op_b;
	if (result > 15){
		LPC_GPI0->FIOSET |= (1 <<5); // P0.5 para el led de overflow
	}
	display(result);
}

void restar(uint8_t op_a , uint8_t op_b){
	if (op_a >= op_b){
		uint8_t result = op_a - op_b;
	}else {
		uint8_t result = op_b - op_a;
		LPC_GPIO0->FIOSET |= (1 << 6); //P0.6 BIT DEL CARRY
	}
	display(result);
}
void Alu (){
	static ESTADOS EstadoActual = WAIT_A;
	static OPCODE OperacionActual;

	switch(EstadoActual){
		case WAIT_A;

			static uint8_t OP_A = (LPC_GPIO0->FIOSET & (0XF<<0));
			LPC_GPIO0->FIOCLR |= (0X0F << 0);
			EstadoActual = WAIT_B;
			break;

		case WAIT_B;

			static uint8_t OP_B = (LPC_GPIO2->FIOSET & (0XF<<0));
			EstadoActual = WAIT_OP;
			break;

		case WAIT_OP;

		//USAMOS LOS DOS PRIMEROS PINS DEL PORT 0 PARA DETERMINA LA OPERACION
			if (LPC_GPIO0->FIODIR & (1 << 4);)){
				operacionActual = resta;
			}else {operacionActual = suma;}
			EstadoActual = EXCE;
			break;

		case EXCE;

		if (operacionActual == suma){
			sumar(OP_A, OP_B);
		}else {
			restar(OP_A, OP_B);
		}
		display(0X00);
		EstadoActual = WAIT_A;

		break;
	}

int main (){
	configOpeatorA();
	configOperatorB();
	configPinsOutput ();
	while (1){
		Alu();
	}
	return 0;
};
