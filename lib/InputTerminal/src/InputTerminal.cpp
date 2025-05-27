#include "InputTerminal.h"

//#include <memory.h>
#define OK 1
#define NG 0

InputTerminal::InputTerminal(unsigned char *input_terminal, unsigned char terminal_num)
{
	unsigned char i;

	err = ITERR_NON;						// �G���[���N���A
	
	if (terminal_num < 16){
		Tm_num = terminal_num;
#ifndef TM_PIN_NUM
		Tm_Pin = new unsigned char[terminal_num];
#endif

		for (i = 0; i<Tm_num; i++){
			Tm_Pin[i] = input_terminal[i];
//			pinMode(Tm_Pin[i], INPUT);	// ���̓v���A�b�v�ݒ�
			pinMode(Tm_Pin[i], INPUT_PULLUP);	// ���̓v���A�b�v�ݒ�
			//		printf("pin %d:%d:%d\n", i, Tm_Pin[i],input_terminal[i]);

		}
		//	memcopy(Tm_Pin,input_terminal,sizeof(input_terminal));
	}
	else{
		Tm_num = 0;
		err = ITERR_KEYNUM;						// �L�[�ݒ萔�I�[�o�[
	}


	keychktim_shortw = IT_KEYCHKTIM_SHORT;
	keychktim_longw = IT_KEYCHKTIM_LONG;
	
	shorton_keydataw = 0;
	longon_keydataw = 0;
	keydat = 0;
	
	return;
}

InputTerminal::~InputTerminal(void)
{

#ifndef TM_PIN_NUM
	delete[] Tm_Pin;
#endif

	return;
}


void InputTerminal::view_pin(void)
{
	unsigned char i;

	Serial.print("pin_num:");
	Serial.println(Tm_num);

	for (i = 0; i < Tm_num; i++){
		Serial.print("pin ");
		Serial.print(i);
		Serial.print(":");
		Serial.println(Tm_Pin[i]);
	}

	return;
}

// �L�[���̓|�[�g��ԁi�u���j�쐬
unsigned int InputTerminal::makekeydata(void)
{
	unsigned char i;

	keydat = 0;
	
	for (i = 0; i<Tm_num; i++){
		keydat |= ((!(unsigned int)digitalRead(Tm_Pin[i])) << i);
	}
//	Serial.println(keydat);

	return (keydat);
}

void InputTerminal::scan(void)
{

	unsigned int keydatw;              // �L�[���̓f�[�^�i�u���l�j
	static unsigned int keydat_lastw;  // �L�[���̓f�[�^�i�O��l�j
	static unsigned char keychkw;       // �L�[���͔��苖�s���t���O

	keydatw = makekeydata();        // �L�[�̏u����ԓǂݍ���

	if (((shorton_keydataw == 0) && (longon_keydataw == 0) // �O��L�[���͏��������ς�
		&& (keydatw == 0) && (keydat_lastw == 0))        // �L�[���̓f�[�^�Ȃ�
//		|| (keymode == KEY_RESET)
		) {                       // ���Z�b�g���Ăяo������
		keychkw = OK;                                          // �L�[���͔��苖��
		itm_starttiml = millis();                              // �L�[���͎��ԏ�񏉊���
		// �L�[���͂��s����܂Ŗ��񂱂��ŏ���������B
	}
	else {}

	if (keychkw == OK) {
		if (keydatw == keydat_lastw) {               // �L�[�����ꒆ
			if ((millis() - itm_starttiml) >= (unsigned long)keychktim_longw) {   // ���������莞�ԁ@��������ON���ɔ���
				longon_keydataw = keydat_lastw;                    // �������L�[���͏��m��
				keychkw = NG;                                      // �L�[���͕s�Ƃ���
			}
		}
		else if (keydatw < keydat_lastw) {           // �O�񂩂牟����Ă���L�[�������� = OFF�G�b�W����
			// �����ZON����́A������Ă���L�[�����������_�Ŕ��肷��B�iOFF�^�C�~���O�����ꂽ�����Ɖ��߁j
			if ((millis() - itm_starttiml) >= (unsigned long)keychktim_shortw) {  // �Z�������莞�ԁB�@�Z������OFF�G�b�W�Ŕ���
				shorton_keydataw = keydat_lastw;                   // �Z�����L�[���͏��m��
				keychkw = NG;                                      // �L�[���͕s�Ƃ���
				// ���������������ɃL�[�������OFF�ƂȂ����ꍇ�A�c�����L�[�݂̂�
				// �Ĕ��肵�Ȃ��悤�ɁA�SOFF����܂ŃL�[���͔����s�Ƃ���B
			}
			else {}

			itm_starttiml = millis();                              // �L�[���͎��ԏ�񏉊���
		}
		else { // �O�񂩂牟����Ă���L�[��������
			// ����ON���́A�����ꂽ�L�[�����������_����ăJ�E���g
			itm_starttiml = millis();                              // �L�[���͎��ԏ�񏉊���
		}

	}
	else {
		itm_starttiml = millis();                              // �L�[���͎��ԏ�񏉊���
	}
			
	keydat_lastw = keydatw;          // �O��l�ێ�
	return;

	return;
}


unsigned int InputTerminal::read(void)
{
	unsigned int ret;
	
	ret = read_s() | read_l();
	
	return(ret);

}


unsigned int InputTerminal::read_s(void)
{
	unsigned int ret;
	
	ret = shorton_keydataw;
	shorton_keydataw = 0;
	
	return(ret);

}

unsigned int InputTerminal::read_l(void)
{
	unsigned int ret;
	
	ret = longon_keydataw;
	longon_keydataw = 0;
	
	return(ret);

}

unsigned char InputTerminal::read_tmnum(void)
{
	return(Tm_num);
}

unsigned char InputTerminal::read_err(void)
{
	return(err);
}


