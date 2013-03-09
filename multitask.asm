;
; マルチタスク練習
;
; MCU = atmega168p
; F_CPU = 8MHz
;
; H.25 3/9
;
.INCLUDE "m168pdef.inc"

;----- 割り込みベクタ定義 -----
.CSEG
	RJMP	MAIN
.ORG OVF0addr
	RJMP	IOVF0


;----- タスク切り替え -----
IOVF0:
	; 汎用レジスタ退避
	PUSH	R16
	PUSH	R17
	PUSH	R18
	PUSH	XH
	PUSH	XL
	; ステータスレジスタ退避
	IN		R16,	SREG
	PUSH	R16
SW_TSK:
	; タスク番号取得
	LDS		R16,	0x0100
	; スタックポインタTOP値保存
	LDI		XH,		0x01
	LDI		XL,		0x01
	ADD		XL,		R16
	IN		R17,	SPL
	ST		X,		R17
	
	; タスク番号変更
	LDI		R17,	0x01
	EOR		R16,	R17
	STS		0x0100,	R16
	OUT		PORTC,	R16
	; スタックポインタTOP値変更
	LDI		XL,		0x01
	ADD		XL,		R16
	LD		R17,	X
	OUT		SPL,	R17
	; ステータスレジスタ復元
	POP		R16
	OUT		SREG,	R16
	; 汎用レジスタ復元
	POP		XL
	POP		XH
	POP		R18
	POP		R17
	POP		R16
	; 割り込み終了
	reti

;----- 初期化処理 -----
MAIN:
	CLI
	
	; 実行中タスク確認用
	LDI		R16,	0x7F
	OUT		DDRC,	R16
	LDI		R16,	0x00
	OUT		PORTC,	R16

	; タスク1の設定
	LDI		R16,	0xDF
	OUT		SPL,	R16
	LDI		R16,	Low(TASK1)
	LDI		R17,	High(TASK1)
	PUSH	R16
	PUSH	R17
	LDI		R16,	0x00 ; ダミー
	PUSH	R16
	PUSH	R16
	PUSH	R16
	PUSH	R16
	PUSH	R16
	PUSH	R16
	IN		R16,	SPL
	STS		0x0101,	R16
	
	; タスク2の設定
	LDI		R16,	0xBF
	OUT		SPL,	R16
	LDI		R16,	Low(TASK2)
	LDI		R17,	High(TASK2)
	PUSH	R16
	PUSH	R17
	LDI		R16,	0x00
	PUSH	R16
	PUSH	R16
	PUSH	R16
	PUSH	R16
	PUSH	R16
	PUSH	R16
	IN		R16,	SPL
	STS		0x0102,	R16
	
	; タイマー0の設定
	LDI		R16,	((1<<CS02)+(1<<CS00))
;	LDI		R16,	(1<<CS00)
	OUT		TCCR0B,	R16
	LDI		R16,	(1<<TOIE0)
	STS		TIMSK0,	R16
	
	; 実行中タスクをタスク2に
	LDI		R16,	0x01
	STS		0x0100,	R16
	
	RJMP	SW_TSK

;----- タスク1 -----
TASK1:
	LDI		R16,		0xFF
	OUT		DDRB,		R16
	LDI		R16,		0x00
	OUT		PORTB,		R16

LOOP1:
	RCALL	DELAY
	SBI		PINB,		PB0
	RJMP	LOOP1


;----- タスク2 -----
TASK2:
	LDI		R16,		0xFF
	OUT		DDRD,		R16
	LDI		R16,		0x00
	OUT		PORTD,		R16

LOOP2:
	RCALL	DELAY
	RCALL	DELAY
	RCALL	DELAY
	RCALL	DELAY
	SBI		PIND,		PD0
	RJMP	LOOP2


;----- 待機 -----
DELAY:
	PUSH	R17
	PUSH	R18
	LDI		R17,		0xFF
L1:
	LDI		R18,		0xFF
L2:
	NOP
	DEC		R18
	BRNE	L2

	DEC		R17
	BRNE	L1
	POP		R18
	POP		R17
	RET
