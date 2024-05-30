/*
 * GccApplication1.c
 *
 * Created: 17-05-2024 00:26:07
 * Author : ADMIN
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#define F_CPU 16000000UL

#define LCD_DATA PORTA
#define ctrl PORTB
#define rs PB0
#define rw PB1
#define en PB2

void LCD_cmd(unsigned char cmd);
void init_LCD(void);
void LCD_write(unsigned char data);
void LCD_write_string(char *str);

void i2c_init()
{
	TWSR = 0x00;
	TWBR = ((F_CPU/100000)-16)/(2*pow(4,(TWSR & ((1<<TWPS1) | (1<<TWPS0)))));
	
	TWCR = (1<<TWEN);
}

void i2c_start()
{
	TWCR = (1<<TWEN) | (1<<TWSTA) | (1<<TWINT);
	
	while((TWCR & (1<<TWINT))==0);
}

void i2c_write(unsigned char data)
{
	TWDR=data;
	TWCR = (1<<TWEN) | (1<<TWINT);
	while((TWCR & (1<<TWINT))==0);
}

unsigned char i2c_read_ack()
{
	TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWEA);
	while((TWCR & (1<<TWINT))==0);
	return TWDR;
}

unsigned char i2c_read_nack()
{
	TWCR = (1<<TWEN) | (1<<TWINT) ;
	while((TWCR & (1<<TWINT))==0);
	return TWDR;
}

void i2c_stop()
{
	TWCR = (1<<TWEN) | (1<<TWSTO) | (1<<TWINT);
	for(int i=0;i<=200;i++);
}

void rtc_int()
{
	i2c_init();
	i2c_start();
	i2c_write(0xD0); //default address to write
	i2c_write(0x07); //to access control register
	i2c_write(0x00); //value assigned to register
	i2c_stop();
}

void rtc_set_time (unsigned char h, unsigned char m, unsigned char s, unsigned char date, unsigned char month, unsigned char year)
{
	i2c_start();
	i2c_write(0xD0);
	i2c_write(0x00);
	i2c_write(s);
	i2c_write(m);
	i2c_write(h);
	i2c_stop();
	
	i2c_start();
	i2c_write(0xD0);
	i2c_write(0x04);
	i2c_write(date);
	i2c_write(month);
	i2c_write(year);
	i2c_stop();
}



void rtc_get_time (unsigned char *h, unsigned char *m, unsigned char *s, unsigned char *date, unsigned char *month, unsigned char *year)
{
	i2c_start();
	i2c_write(0xD0);
	i2c_write(0x00);
	i2c_stop();
	i2c_start();
	i2c_write(0xD1); //address to read
	*s=i2c_read_ack();
	*m=i2c_read_ack();
	*h=i2c_read_nack();
	i2c_stop();
	
	i2c_start();
	i2c_write(0xD0);
	i2c_write(0x04);
	i2c_stop();
	i2c_start();
	i2c_write(0xD1); //address to read
	*date=i2c_read_ack();
	*month=i2c_read_ack();
	*year = i2c_read_nack();
	i2c_stop();
}


void LCD_init(void)
{
	LCD_cmd(0x38);
	_delay_ms(1);
	LCD_cmd(0x01);
	_delay_ms(1);
	LCD_cmd(0x0C);
	_delay_ms(1);
	LCD_cmd(0x80);
	_delay_ms(1);
	return ;
}

void LCD_cmd(unsigned char cmd)
{
	LCD_DATA=cmd;
	ctrl = (0<<rs) | (0<<rw) | (1<<en);
	_delay_ms(1);
	ctrl = (0<<rs) | (0<<rw) | (0<<en);
	_delay_ms(1);
	return;
}

void LCD_write(unsigned char data)
{
	LCD_DATA=data;
	ctrl = (1<<rs) | (0<<rw) | (1<<en);
	_delay_ms(1);
	ctrl = (1<<rs) | (0<<rw) | (0<<en);
	_delay_ms(1);
	return;
}

void LCD_write_string(char *str)
{
	int i=0;
	while (str[i]!='\0')
	{
		LCD_write(str[i]);
		i++;
	}
	return;
}

int main(void)
{
    DDRA=0xFF;
	DDRB=0x07;
	LCD_init();
	LCD_write_string("TIME :");
	LCD_cmd(0xC0);
	LCD_write_string("DATE :");
	
	rtc_int();
	rtc_set_time(0x11, 0x23, 0x15,0x17,0x05,0x24);
	
	unsigned char i,j,k,a,b,c;
    while (1) 
    {
		LCD_cmd(0x87);
		rtc_get_time(&i,&j,&k,&a,&b,&c);
		LCD_write('0' + (i>>4));
		LCD_write('0'+(i & 0x0F));
		LCD_write(':');
		LCD_write('0'+(j>>4));
		LCD_write('0'+(j & 0x0F));
		LCD_write(':');
		LCD_write('0'+(k>>4));
		LCD_write('0'+(k & 0x0F));
		LCD_cmd(0xC7);
		LCD_write('0' + (a>>4));
		LCD_write('0'+(a & 0x0F));
		LCD_write(':');
		LCD_write('0'+(b>>4));
		LCD_write('0'+(b & 0x0F));
		LCD_write(':');
		LCD_write('0'+(c>>4));
		LCD_write('0'+(c & 0x0F));
		
		_delay_ms(500);
	}
	
	return 0;
}


	



