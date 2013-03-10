/*
 * (����)��ԊȒP�ȃ}���`�^�X�N����
 * C����(�ƃA�Z���u��)�ɂ�����
 *
 * MCU = atmega168p
 * F_CPU = 8MHz
 *
 * H.25 3/10
 */
#include <avr/io.h>
#include <avr/interrupt.h>

// �X�^�b�N�|�C���^
#define __SP__ (*((uint16_t *)0x5D))

// �֐��v���g�^�C�v
void task1(void); // PB0��_��
void task2(void); // PD0��_��

// �ݒ�
#define TASK_MAX_NUM 2      // �^�X�N�̐�
#define TASK1_SP_DEF 0x03FF // �^�X�N1�̃X�^�b�N�|�C���^�����l
#define TASK2_SP_DEF 0x02FF // �^�X�N2�̃X�^�b�N�|�C���^�����l

// �^�X�N����p�\����
typedef struct _TaskContext {
	uint8_t state;
	uint16_t sp;
	void (*task)(void);
} TaskContext;

// ����p�f�[�^
volatile int8_t task_num;  // ���s���^�X�N�̔ԍ�
volatile TaskContext task_contexts[TASK_MAX_NUM];

//----------------------------------------
// �^�C�}�[���荞��(�^�X�N�؂�ւ�)
//----------------------------------------
ISR ( TIMER0_OVF_vect, ISR_NAKED )
{
	// ���W�X�^�ޔ�
	__asm__ __volatile__ (
		"push r31\n\t"
		"push r30\n\t"
		"push r29\n\t"
		"push r28\n\t"
		"push r27\n\t"
		"push r26\n\t"
		"push r25\n\t"
		"push r24\n\t"
		"push r23\n\t"
		"push r22\n\t"
		"push r21\n\t"
		"push r20\n\t"
		"push r19\n\t"
		"push r18\n\t"
		"push r17\n\t"
		"push r16\n\t"
		"push r15\n\t"
		"push r14\n\t"
		"push r13\n\t"
		"push r12\n\t"
		"push r11\n\t"
		"push r10\n\t"
		"push r9\n\t"
		"push r8\n\t"
		"push r7\n\t"
		"push r6\n\t"
		"push r5\n\t"
		"push r4\n\t"
		"push r3\n\t"
		"push r2\n\t"
		"push r1\n\t"
		"push r0\n\t"
		"in r0, __SREG__\n\t"
		"push r0\n\t"
	::);
	
	// �X�^�b�N�|�C���^�̒l�ۑ�
	if (task_num < TASK_MAX_NUM) {
		task_contexts[task_num].sp = __SP__;
	}
	
	// �^�X�N�ԍ��ύX
	task_num = (task_num + 1) % TASK_MAX_NUM;
	PORTC = task_num; // �m�F�p
	
	// �X�^�b�N�|�C���^�ύX
	__SP__ = task_contexts[task_num].sp;
	
	// �܂��N������ĂȂ��Ȃ�N��
	if (task_contexts[task_num].state == 0) {
		task_contexts[task_num].state = 1;
		sei();
		task_contexts[task_num].task();
	}
	
	// ���W�X�^�l����
	__asm__ __volatile__ (
		"pop r0\n\t"
		"out __SREG__, r0\n\t"
		"pop r0\n\t"
		"pop r1\n\t"
		"pop r2\n\t"
		"pop r3\n\t"
		"pop r4\n\t"
		"pop r5\n\t"
		"pop r6\n\t"
		"pop r7\n\t"
		"pop r8\n\t"
		"pop r9\n\t"
		"pop r10\n\t"
		"pop r11\n\t"
		"pop r12\n\t"
		"pop r13\n\t"
		"pop r14\n\t"
		"pop r15\n\t"
		"pop r16\n\t"
		"pop r17\n\t"
		"pop r18\n\t"
		"pop r19\n\t"
		"pop r20\n\t"
		"pop r21\n\t"
		"pop r22\n\t"
		"pop r23\n\t"
		"pop r24\n\t"
		"pop r25\n\t"
		"pop r26\n\t"
		"pop r27\n\t"
		"pop r28\n\t"
		"pop r29\n\t"
		"pop r30\n\t"
		"pop r31\n\t"
		"reti\n\t"
	::);
}

//----------------------------------------
// ���C��(�������������s��)
//----------------------------------------
int main(void)
{
	PORTC = 0x00;
	DDRC  = 0x7F; // �^�X�N��Ԋm�F�p
	
	task_contexts[0].state = 0;
	task_contexts[0].sp = TASK1_SP_DEF;
	task_contexts[0].task = task1;
	
	task_contexts[1].state = 0;
	task_contexts[1].sp = TASK2_SP_DEF;
	task_contexts[1].task = task2;
	
	task_num = -1;
	
	TCCR0B = (1<<CS02) | (1<<CS00); // �v���X�P�[�� 1/1024
	TIMSK0 = (1<<TOIE0);            // OVF���荞�݋���
	
	sei();
	
	TCNT0 = 255;
	
	while(1) {
	}
	return 0;
}

//----------------------------------------
// �^�X�N1 (PB0�_��)
//----------------------------------------
void task1()
{
	uint16_t i, j;
	
	PORTB = 0x00;
	DDRB  = 0xFF;
	
	while (1) {
		for (i = 0; i < 100; i++) {
			for (j = 0; j < 1000; j++) {
				__asm__ __volatile__ ("nop");
			}
		}
		PORTB = PORTB ^ 0x01;
	}
}

//----------------------------------------
// �^�X�N2 (PD0�_��)
//----------------------------------------
void task2()
{
	uint16_t i, j;
	
	PORTD = 0x00;
	DDRD  = 0xFF;
	
	while (1) {
		for (i = 0; i < 500; i++) {
			for (j = 0; j < 1000; j++) {
				__asm__ __volatile__ ("nop");
			}
		}
		PORTD = PORTD ^ 0x01;
	}
}
