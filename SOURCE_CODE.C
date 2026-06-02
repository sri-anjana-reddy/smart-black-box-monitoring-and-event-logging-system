/* -------- Code for LPC1768 -------- */
/* -------- Source code -------- */
#include <LPC17xx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------- LCD -------- */
#define LCD_RS (1<<10)
#define LCD_EN (1<<11)
#define LCD_DATA (0xFF<<15)

/* -------- BUZZER & LED -------- */
#define BUZZER (1<<27)
#define LED_START 19
#define LED_MASK (0xFF<<LED_START)

/* -------- I2C RTC -------- */
#define RTC_ADDR 0x68

/* -------- DELAY -------- */
void delay_ms(uint32_t ms)
{
    uint32_t i,j;
    for(i=0;i<ms;i++)
        for(j=0;j<10000;j++);
}

/* -------- LCD -------- */
void lcd_enable()
{
    LPC_GPIO0->FIOSET = LCD_EN;
    delay_ms(2);
    LPC_GPIO0->FIOCLR = LCD_EN;
}

void lcd_cmd(unsigned char cmd)
{
    LPC_GPIO0->FIOCLR = LCD_RS | LCD_DATA;
    LPC_GPIO0->FIOSET = (cmd<<15);
    lcd_enable();
}

void lcd_data(unsigned char data)
{
    LPC_GPIO0->FIOSET = LCD_RS;
    LPC_GPIO0->FIOCLR = LCD_DATA;
    LPC_GPIO0->FIOSET = (data<<15);
    lcd_enable();
}

void lcd_init()
{
    LPC_GPIO0->FIODIR |= LCD_RS | LCD_EN | LCD_DATA;

    lcd_cmd(0x38);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
}

void lcd_str(char *s)
{
    while(*s) lcd_data(*s++);
}

/* -------- UART -------- */
void uart_init()
{
    LPC_SC->PCONP |= (1<<3);

    LPC_PINCON->PINSEL0 |= (1<<4); 
    LPC_PINCON->PINSEL0 |= (1<<6);

    LPC_UART0->LCR = 0x83;
    LPC_UART0->DLL = 6;
    LPC_UART0->DLM = 0;
    LPC_UART0->FCR = 0x07;
    LPC_UART0->LCR = 0x03;
}

void uart_char(char c)
{
    while(!(LPC_UART0->LSR & (1<<5)));
    LPC_UART0->THR = c;
}

void uart_str(char *s)
{
    while(*s) uart_char(*s++);
}

/* -------- I2C -------- */
void i2c_init()
{
    LPC_SC->PCONP |= (1<<7);

    LPC_PINCON->PINSEL1 |= (1<<22);
    LPC_PINCON->PINSEL1 |= (1<<24);

    LPC_I2C1->SCLH = 60;
    LPC_I2C1->SCLL = 60;

    LPC_I2C1->I2CONSET = (1<<6);
}

void i2c_start()
{
LPC_I2C1->I2CONSET = (1<<5);
    while(!(LPC_I2C1->I2CONSET & (1<<3)));
}

void i2c_stop()
{
    LPC_I2C1->I2CONSET = (1<<4);
    LPC_I2C1->I2CONCLR = (1<<5);
}

void i2c_write(char data)
{
    LPC_I2C1->I2DAT = data;
    LPC_I2C1->I2CONCLR = (1<<3);
    while(!(LPC_I2C1->I2CONSET & (1<<3)));
}

/* -------- KEYPAD -------- */
const char keymap[4][4]={
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

char keypad_read()
{
    int r,c;

    LPC_GPIO2->FIOSET = 0x0F;

    for(r=0;r<4;r++)
    {
        LPC_GPIO2->FIOCLR = (1<<r);

        for(c=0;c<4;c++)
        {
            if(!(LPC_GPIO2->FIOPIN & (1<<(c+4))))
            {
                while(!(LPC_GPIO2->FIOPIN & (1<<(c+4))));
                LPC_GPIO2->FIOSET = (1<<r);
                return keymap[r][c];
            }
        }

        LPC_GPIO2->FIOSET = (1<<r);
    }

    return 0;
}

/* -------- PARAMETER NAME -------- */
void get_name(char key,char *name)
{
    switch(key)
    {
case '1': strcpy(name,"Temp"); break;
        case '2': strcpy(name,"Pressure"); break;
        case '3': strcpy(name,"Humidity"); break;
        case '4': strcpy(name,"Voltage"); break;
        case '5': strcpy(name,"Current"); break;
        case '6': strcpy(name,"Vibration"); break;
        case '7': strcpy(name,"RPM"); break;
        case '8': strcpy(name,"Fuel"); break;
    }
}

/* -------- RANDOM VALUE -------- */
int generate_value(char key)
{
    switch(key)
    {
        case '1': return rand()%60+20;
        case '2': return rand()%60+950;
        case '3': return rand()%80+20;
        case '4': return rand()%30+210;
        case '5': return rand()%15+3;
        case '6': return rand()%6+1;
        case '7': return rand()%4000+800;
        case '8': return rand()%100;
    }
    return 0;
}

/* -------- THRESHOLD -------- */
int check_threshold(char key,int value)
{
    switch(key)
    {
        case '1': return value>50;
        case '2': return value>990;
        case '3': return value>70;
        case '4': return value>235;
        case '5': return value>12;
        case '6': return value>4;
        case '7': return value>3500;
        case '8': return value<20;
    }
    return 0;
}

/* -------- MAIN -------- */
int main()
{
    char key;
    char pname[15];
    char buffer[100];
    int led_pin;

    LPC_GPIO2->FIODIR |= 0x0F;
    LPC_GPIO2->FIODIR &= ~(0xF0);
LPC_GPIO1->FIODIR |= LED_MASK | BUZZER;

    lcd_init();
    uart_init();
    i2c_init();

    lcd_str("SMART BLACK BOX");

    while(1)
    {
        key = keypad_read();

        if(key>='1' && key<='8')
        {
            int value = generate_value(key);
            int alert = check_threshold(key,value);

            get_name(key,pname);

            lcd_cmd(0x01);
            lcd_str(pname);
            lcd_str(":");

            sprintf(buffer,"%d",value);
            lcd_str(buffer);

            led_pin = (key-'1')+LED_START;

            LPC_GPIO1->FIOCLR = LED_MASK | BUZZER;

            lcd_cmd(0xC0);

            if(alert)
            {
                 LPC_GPIO1->FIOSET = (1<< led_pin) | BUZZER;
               lcd_str("ALERT");
             }
             else
            {
              lcd_str("NORMAL");
            }
           sprintf(buffer,"\r\n%s:%d %s\r\n",pname,value,alert?"ALERT":"OK");
           
           uart_str(buffer);

            delay_ms(500);
         }
    }
}






