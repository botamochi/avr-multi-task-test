/*
 * (多分)一番簡単なマルチタスク処理
 * C言語(とアセンブリ)による実装
 *
 * MCU = atmega168p
 * F_CPU = 8MHz
 *
 * H.25 3/10
 */
#include <avr/io.h>
#include <avr/interrupt.h>

// スタックポインタ
#define __SP__ (*((uint16_t *)0x5D))

// 関数プロトタイプ
void task1(void); // PB0を点滅
void task2(void); // PD0を点滅

// 設定
#define TASK_MAX_NUM 2      // タスクの数
#define TASK1_SP_DEF 0x03FF // タスク1のスタックポインタ初期値
#define TASK2_SP_DEF 0x02FF // タスク2のスタックポインタ初期値

// タスク制御用構造体
typedef struct _TaskContext {
	uint8_t state;
	uint16_t sp;
	void (*task)(void);
} TaskContext;

// 制御用データ
volatile int8_t task_num;  // 実行中タスクの番号
volatile TaskContext task_contexts[TASK_MAX_NUM];

//----------------------------------------
// タイマー割り込み(タスク切り替え)
//----------------------------------------
ISR ( TIMER0_OVF_vect, ISR_NAKED )
{
	// レジスタ退避
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
	
	// スタックポインタの値保存
	if (task_num < TASK_MAX_NUM) {
		task_contexts[task_num].sp = __SP__;
	}
	
	// タスク番号変更
	task_num = (task_num + 1) % TASK_MAX_NUM;
	PORTC = task_num; // 確認用
	
	// スタックポインタ変更
	__SP__ = task_contexts[task_num].sp;
	
	// まだ起動されてないなら起動
	if (task_contexts[task_num].state == 0) {
		task_contexts[task_num].state = 1;
		sei();
		task_contexts[task_num].task();
	}
	
	// レジスタ値復元
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
// メイン(初期化処理を行う)
//----------------------------------------
int main(void)
{
	PORTC = 0x00;
	DDRC  = 0x7F; // タスク状態確認用
	
	task_contexts[0].state = 0;
	task_contexts[0].sp = TASK1_SP_DEF;
	task_contexts[0].task = task1;
	
	task_contexts[1].state = 0;
	task_contexts[1].sp = TASK2_SP_DEF;
	task_contexts[1].task = task2;
	
	task_num = -1;
	
	TCCR0B = (1<<CS02) | (1<<CS00); // プリスケーラ 1/1024
	TIMSK0 = (1<<TOIE0);            // OVF割り込み許可
	
	sei();
	
	TCNT0 = 255;
	
	while(1) {
	}
	return 0;
}

//----------------------------------------
// タスク1 (PB0点滅)
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
// タスク2 (PD0点滅)
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
