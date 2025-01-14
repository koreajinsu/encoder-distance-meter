#define F_CPU 16000000UL // CPU 클럭 속도
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

// 핀 정의
#define BUTTON_A PD2 // INT0
#define BUTTON_B PD3 // INT1
#define BUTTON_C PD4 // 일반 버튼 (폴링 방식)

// I2C 및 LCD 관련 함수
void i2c_init() {
	TWSR = 0x00;             // 프리스케일러 설정 (1)
	TWBR = 0x47;             // 비트레이트 설정 (100kHz)
	TWCR = (1 << TWEN);      // I2C 활성화
}

void i2c_start() {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // 시작 조건 완료 대기
}

void i2c_stop() {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	_delay_us(10); // 짧은 대기
}

void i2c_write(uint8_t data) {
	TWDR = data;             // 데이터 레지스터에 데이터 쓰기
	TWCR = (1 << TWINT) | (1 << TWEN); // 데이터 전송
	while (!(TWCR & (1 << TWINT)));    // 전송 완료 대기
}

void lcd_send(uint8_t value, uint8_t mode) {
	uint8_t data = value & 0xF0; // 상위 4비트 전송
	data |= mode;                // RS 설정 (명령어 또는 데이터 모드)
	data |= 0x08;                // 백라이트 켜기 (0x08)

	i2c_start();
	i2c_write(0x27 << 1);  // LCD 주소 (I2C SLA+W)
	i2c_write(data | 0x04); // 데이터 전송 후 Enable 비트 설정
	_delay_us(1);
	i2c_write(data & ~0x04); // Enable 비트 클리어
	_delay_us(50);
	i2c_stop();
}

void lcd_command(uint8_t command) {
	lcd_send(command & 0xF0, 0x00); // 상위 4비트 명령어 전송
	lcd_send((command << 4), 0x00); // 하위 4비트 명령어 전송
}

void lcd_data(uint8_t data) {
	lcd_send(data & 0xF0, 0x01);   // 상위 4비트 데이터 전송 (RS = 1)
	lcd_send((data << 4), 0x01);   // 하위 4비트 데이터 전송 (RS = 1)
}

void lcd_init() {
	_delay_ms(50);                // LCD 초기화 대기
	lcd_command(0x30);            // 초기화 명령
	_delay_ms(5);
	lcd_command(0x30);            // 초기화 명령 반복
	_delay_us(150);
	lcd_command(0x20);            // 4비트 모드 설정
	_delay_ms(5);

	// Function set: 4비트 모드, 2라인 디스플레이, 5x7 도트
	lcd_command(0x28);
	// Display ON, Cursor OFF
	lcd_command(0x0C);
	// Clear display
	lcd_command(0x01);
	_delay_ms(2);
}

void lcd_clear() {
	lcd_command(0x01); // Clear display
	_delay_ms(2);
}

void lcd_write_string(const char *str) {
	while (*str) {
		lcd_data(*str++); // 문자열의 각 문자를 LCD에 전송
	}
}

// 글로벌 변수
volatile uint8_t lastButtonState = 0;

// 외부 인터럽트 초기화
void interrupts_init() {
	// INT0 (PD2), INT1 (PD3)를 상승 에지에서 인터럽트 발생하도록 설정
	EICRA |= (1 << ISC01) | (1 << ISC00) | (1 << ISC11) | (1 << ISC10);
	EIMSK |= (1 << INT0) | (1 << INT1); // INT0, INT1 활성화

	sei(); // 전역 인터럽트 활성화
}

// INT0 인터럽트 서비스 루틴
ISR(INT0_vect) {
	if (lastButtonState != 1) {
		lastButtonState = 1;
		lcd_clear();
		lcd_write_string("A");
		} else {
		lastButtonState = 0;
		lcd_clear();
		lcd_write_string("Hello, World!");
	}
}

// INT1 인터럽트 서비스 루틴
ISR(INT1_vect) {
	if (lastButtonState != 2) {
		lastButtonState = 2;
		lcd_clear();
		lcd_write_string("B");
		} else {
		lastButtonState = 0;
		lcd_clear();
		lcd_write_string("Hello, World!");
	}
}

// PD4 버튼 감지 함수 (폴링 방식)
void check_button_c() {
	if (!(PIND & (1 << BUTTON_C))) { // 버튼 C가 눌렸을 때
		_delay_ms(50); // 디바운싱 처리
		if (!(PIND & (1 << BUTTON_C))) {
			if (lastButtonState != 3) {
				lastButtonState = 3;
				lcd_clear();
				lcd_write_string("C");
				} else {
				lastButtonState = 0;
				lcd_clear();
				lcd_write_string("Hello, World!");
			}
			while (!(PIND & (1 << BUTTON_C))); // 버튼이 눌린 상태에서 떼어질 때까지 대기 (재입력 방지)
		}
	}
}

int main() {
	// 초기화
	i2c_init();         // I2C 초기화
	lcd_init();         // LCD 초기화
	interrupts_init();  // 외부 인터럽트 초기화

	// PD4 버튼 입력 설정 및 풀업 저항 활성화
	DDRD &= ~(1 << BUTTON_C);
	PORTD |= (1 << BUTTON_C);

	lcd_clear();        // 화면 지우기
	lcd_write_string("Hello, World!"); // 기본 메시지 출력

	while (1) {
		// 폴링 방식으로 PD4 버튼 확인
		check_button_c();
	}
}
