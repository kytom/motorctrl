//#include <reg52.h>
#include "12864.h"
#include <stdio.h>
#define uchar unsigned char
#define uint unsigned int
#define key_port P0	//键盘接口
sbit DIR = P1^0;
sbit EN = P1^1;
sbit PWM = P1^2;
sbit BEEP = P2^3;

 

uchar keyscan();	//按键判断程序

uint ms = 10;
int angle = 1;
uchar code dis1[] = {"转角:0.5圈"};
uchar code dis2[] = {"转速:30r/min"};
uchar code dis3[] = {"方向:+"};
uint count = 0;
uint t = 0;
uchar buf;
uchar num,temp;


void print_angle(void);
void adjust_parm_key(uchar k);
void adjust_parm_temp(void);

void delay_ms(uint z) //delay0.01ms
{
	uint x;
	for(x=z;x>0;x--);
//		for(y=/*56 0.5ms*/ 27/*0.25ms*/;y>0;y--);
}

void delay_1ms(uint m)
{
	uint i, j, k;
	for(i=0; i<m; i++)
	{
		for(j=0; j<10; j++)
			for(k=0; k<33; k++);
	}
}

void delay10ms(void) //延时程序
{
	unsigned char i,j;
	for(i=20;i>0;i--)
	for(j=248;j>0;j--);
}



void Motor_Go(uint ms)
{
	uint ms1;
	switch(ms){
		case 10: ms1 = 210;break;
		case 9: ms1 = 105;break;
		case 8: ms1 = 70;break;
		case 7: ms1 = 51;break;
		case 6: ms1 = 42;break;
		case 5: ms1 = 36;break;
		case 4: ms1 = 32;break;
		case 3: ms1 = 26;break;
		case 2: ms1 = 22;break;
		case 1: ms1 = 20;break;
	}
	PWM = 1;
	delay_ms(ms1);
	PWM = 0;
	delay_ms(ms1);
}

void print(const char *s, int pos)
{
	uchar i;
	 lcd_pos(pos,0);             //设置显示位置为第一行的第1个字符
     i = 0;
    while(s[i] != '\0')
     {                         //显示字符
       lcd_wdat(s[i]);
       i++;
     }
}

void LCDinit(void)
{
    delay(10);                 //延时
    wela=0;
	dula=0; 
    lcd_init();                //初始化LCD             
  
   print(dis1, 0);
	 print(dis2, 1);
 	print(dis3, 2);
}




void print_rate(void)
{
	switch(ms){
		case 1: print("转速:300r/min", 1); break;
		case 2: print("转速:270r/min", 1); break;
		case 3: print("转速:240r/min", 1); break;
		case 4: print("转速:210r/min", 1); break;
		case 5: print("转速:180r/min", 1); break;
		case 6: print("转速:150r/min", 1); break;
		case 7: print("转速:120r/min", 1); break;
		case 8: print("转速:90r/min", 1); break;
		case 9: print("转速:60r/min", 1); break;
		case 10: print("转速:30r/min", 1); break;
	}
}

void Time0_Init()          //定时器初始化
{
	TMOD = 0x01;           //定时器0工作在方式1    
	IE   = 0x82;
	TH0  = 0xee;
	TL0  = 0x00;     //11.0592MZ晶振，5ms
//	    TR0=1;                 //定时器开始
}
void Time0_Int() interrupt 1 //中断程序
{
	count=(count+1); 
	TH0  = 0xee;             //重新赋值
	TL0  = 0x00;
	if(count > t){              //判断0.5ms次数是否小于角度标识
	   EN = 0;                  //确实小于，PWM输出高电平
	   count = 0;
	 }
             //0.5ms次数加1
 //   count=count%40;     //次数始终保持为40 即保持周期为20ms
}
	

void com_init()
{
	SCON = 0x50;
	PCON = 0x00;
	
	TMOD |= 0x20;
	TH1 = 0XFD;
	TL1 = 0XFD;
	TR1 = 1;
	ES = 1;
}

void serial() interrupt 4
{
	EA = 0;
	ES = 0;
	RI = 0;
	buf = SBUF;
	switch (buf) {
		case 'A':
			++ms;
			if(ms > 10)
				ms = 10;
			print_rate();	
			break;
		case 'B':
			--ms;
			if(ms < 1)
				ms = 1;
			print_rate();
			break;
			
			case 'C':
			{
				DIR = ~DIR;
			if(DIR)
				print("方向:+", 2);
			else
				print("方向:-", 2);
		}
			break;
			
		case 'D':
			++angle;
			if(angle > 200)
				angle = 200;
			print_angle();
			break;
		case 'E':
			--angle;
			if(angle <= 0)
				angle = 0;
			print_angle();
			break;
		case 'F':
			EN = ~EN;
			break;
		default:
//			BEEP = 0;
			break;
			
/*			 case 0x31:  P1=0xfe;BEEP=1;break;  //???1,???LED?         
      case 0x32:  P1=0xfd;BEEP=1;break;  //???2,???LED?        
      case 0x33:  P1=0xfb;BEEP=1;break;  //???3,???LED?        
      case 0x34:  P1=0xf7;BEEP=1;break;  //???4,???LED?       
      case 0x35:  P1=0xef;BEEP=1;break;  //???5,???LED?            
      case 0x36:  P1=0xdf;BEEP=1;break;  //???5,???LED?                   
      case 0x37:  P1=0xbf;BEEP=1;break;  //???5,???LED?
	  case 0x38:  P1=0x7f;BEEP=1;break;  //???5,???LED?
	  default:    BEEP=0;P1=0xff;break;  //???????,????   */     

	}
	ES = 1;
	EA = 1;
}


int main()
{
	uchar k;
//	char string[100] = "";
	EN = 0;
	DIR = 1;
	PWM = 0;
	RD = 0;
	LCDinit();
	Time0_Init();
	com_init();

//	sprintf(string, "hello world");
	while(1){
		EA = 1;
		if (ENABLE_TEMP_CTRL == 0) {
			k = keyscan();
			adjust_parm_key(k);
		} else {
			adjust_parm_temp();
		}
		while(EN){
			EA = 1;
			Motor_Go(ms);

			k = keyscan();
			if(k == 2){
				EN = ~EN;
				TR0 = 0;
				count = 0;
				}
		/*	
			if(k == 1){
				DIR = ~DIR;
				if(DIR)
					print("方向:+", 2);
				else
					print("方向:-", 2);
			}
		
			if(k == 4){
				++ms;
				if(ms > 10)
					ms = 10;
				print_rate();
			}
			if(k == 3){
				--ms;
				if(ms < 1)
					ms = 1;
				print_rate();
			}*/
			EA = 0;
		}
		
	}
}

void adjust_parm_temp(void)
{
	int temp = 0;

	temp = read_temp();
	TR0 = 0;
	EN = 1;
	ms = 11 - (temp / 10);
	if (ms > 10)
		ms = 10;
	else if (ms < 1)
		ms = 1;
}

void adjust_parm_key(uchar k)
{
	float t1 = 0.0;
		if(k == 1){
			DIR = ~DIR;
			if(DIR)
				print("方向:+", 2);
			else
				print("方向:-", 2);
			}
	  	if(k == 4){
			++ms;
			if(ms > 10)
				ms = 10;
			print_rate();
		}
		if(k == 3){
			--ms;
			if(ms < 1)
				ms = 1;
			print_rate();
		}
		if(k == 5){
				++angle;
				if(angle > 200)
					angle = 200;
				print_angle();
		}
		if(k == 6){
				--angle;
				if(angle <= 0)
					angle = 0;
				print_angle();
		}

		if(k == 10){
				angle += 20;
				if(angle > 200)
					angle = 200;
				print_angle();
		}
		if(k == 11){
				angle -= 20;
				if(angle <= 0)
					angle = 0;
				print_angle();
		}

		if(k == 2)
			EN = ~EN;

		if(angle != 0) {
			TR0 = 1;
			t1 = (angle * 0.5) / (5.5 - 0.5 * ms) ;
			t = (t1 / 0.5) * 100;
		} else {
			TR0 = 0;			//关闭中断 无穷次循环
		}
}

uchar keyscan()
{
			num = 16;
			key_port=0xfe;
			temp=key_port;
			temp=temp&0xf0;
			while(temp!=0xf0)
				{
					delay_1ms(10);
					temp=key_port;
					temp=temp&0xf0;
					while(temp!=0xf0)
					{
						temp=key_port;
					switch(temp)
						{
							case 0xee:num=1;
								break;
							case 0xde:num=2;
								break;
							case 0xbe:num=3;
								break;
							case 0x7e:num=10;
								break;
						}
					while(temp!=0xf0)
						{
							temp=key_port;
							temp=temp&0xf0;
						}
					}
				}

			key_port=0xfd;
			temp=key_port;
			temp=temp&0xf0;
			while(temp!=0xf0)
				{
					delay_1ms(10);
					temp=key_port;
					temp=temp&0xf0;
					while(temp!=0xf0)
					{
						temp=key_port;
					switch(temp)
						{
							case 0xed:num=4;
								break;
							case 0xdd:num=5;
								break;
							case 0xbd:num=6;
								break;
							case 0x7d:num=11;
								break;
						}
					while(temp!=0xf0)
						{
							temp=key_port;
							temp=temp&0xf0;
						}
					}
				}


			key_port=0xfb;
			temp=key_port;
			temp=temp&0xf0;
			while(temp!=0xf0)
				{
					delay_1ms(10);
					temp=key_port;
					temp=temp&0xf0;
					while(temp!=0xf0)
					{
						temp=key_port;
					switch(temp)
						{
							case 0xeb:num=7;
								break;
							case 0xdb:num=8;
								break;
							case 0xbb:num=9;
								break;
							case 0x7b:num=12;
								break;
						}
					while(temp!=0xf0)
						{
							temp=key_port;
							temp=temp&0xf0;
						}
					}
				}


			key_port=0xf7;
			temp=key_port;
			temp=temp&0xf0;
			while(temp!=0xf0)
				{
					delay_1ms(10);
					temp=key_port;
					temp=temp&0xf0;
					while(temp!=0xf0)
					{
						temp=key_port;
					switch(temp)
						{
							case 0xe7:num=14;
								break;
							case 0xd7:num=0;
								break;
							case 0xb7:num=15;
								break;
							case 0x77:num=13;
								break;
						}
					while(temp!=0xf0)
						{
							temp=key_port;
							temp=temp&0xf0;
						}
					}
				}										

		return num;
}

void print_angle(void)
{
/*	int v;
	float ang;
	v = 330 - 30 * ms;
	sprintf(rate, "转速:%dr/min", v);
	print(rate, 1);
	ang = 5.5 - 0.5 * ms;
	sprintf(angle, "转角:%f圈", ang);
	print(angle, 0);*/
	switch(angle){
		case 0:	print("转角:+00 ", 0);	break;
		case 1: print("转角:0.5 圈", 0); break;
		case 2: print("转角:1.0 圈", 0); break;
		case 3: print("转角:1.5 圈", 0); break;
		case 4: print("转角:2.0 圈", 0); break;
		case 5: print("转角:2.5 圈", 0); break;
		case 6: print("转角:3.0 圈", 0); break;
		case 7: print("转角:3.5 圈", 0); break;
		case 8: print("转角:4.0 圈", 0); break;
		case 9: print("转角:4.5 圈", 0); break;
		case 10: print("转角:5.0 圈", 0); break;

		case 11: print("转角:5.5 圈", 0); break;
		case 12: print("转角:6.0 圈", 0); break;
		case 13: print("转角:6.5 圈", 0); break;
		case 14: print("转角:7.0 圈", 0); break;
		case 15: print("转角:7.5 圈", 0); break;
		case 16: print("转角:8.0 圈", 0); break;
		case 17: print("转角:8.5 圈", 0); break;
		case 18: print("转角:9.0 圈", 0); break;
		case 19: print("转角:9.5 圈", 0); break;
		case 20: print("转角:10.0圈", 0); break;
		
		default : print("wrong input", 0); break;

	}
}

