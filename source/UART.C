/******************************************************************************/
/*                        -- 深圳安信可科技有限公司 --                        */
/*
//  文件名：  uart.c                                                              
//  说明：    串口底层驱动函数，使用16位的定时器作为波特率发生器，自动重载模式                                                              
//  编写人员：Wu                                                                   
//  编写日期：2012年11月12日                                                              
//  程序维护：
//	维护记录：
//
//                                                          
*/ 
// 免责声明：该程序仅用于学习与交流 
// (c) Anxinke Corporation. All rights reserved.                                                               
/******************************************************************************/
#include "typedef.h"
#include "gpio.h"
#include "uart.h"

static bit bUartFlag;


/******************************************************************************/
// 函数名称：uartInit 
// 输入参数：无 
// 输出参数：无 
// 函数功能：设置好定时器0的工作模式 
/******************************************************************************/
void uartInit(void)
{
    /*
     * 设置定时器0为16位自动重载定时器
    */
    AUXR |= 0x80; //定时器0为1T模式
    TMOD &= 0xF0; //设置定时器为模式0(16位自动重装载)
	TL0 = (0xFFFF - MCU_FREQ / UART_BUAD) & 0xFF; //设置定时初值
	TH0 = ((0xFFFF - MCU_FREQ / UART_BUAD) >> 8) & 0xFF; //设置定时初值
    TR0 = 0; //定时器0开始计时
    ET0 = 0; //使能定时器0中断
    TI=1;
    EA = 1;
}

/******************************************************************************/
// 函数名称：uartSendData 
// 输入参数：ucData: 发送字节 
// 输出参数：无 
// 函数功能：使用串口发送一个字节数据 
/******************************************************************************/
void uartSendData(u8 ucData)
{
    u8 ucCnt;
    UART_TX_LOW(); //串口起始位开始
    TR0 = 1; //定时器0开始计时
    ET0 = 1; //使能定时器0中断
    bUartFlag = ON; 
    while(bUartFlag == ON);
	/*
	 * 由低位开始，将数据通过串口输出
	*/
    for (ucCnt = 0; ucCnt < 8; ucCnt++) {
        UART_TX_SET(ucData & 0x01);
        ucData >>= 1;
        bUartFlag = ON; 
        while(bUartFlag == ON);
    }
    UART_TX_HIGH(); // 发送串口停止位  
    bUartFlag = ON; 
    while(bUartFlag == ON);
    TR0 = 0; //定时器0结束计时
    ET0 = 0; //禁能定时器0中断    
}
/******************************************************************************/
// 函数名称：uartSendString 
// 输入参数：pS: 字符串首地址 
// 输出参数：无 
// 函数功能：发送字符串通过串口输出 
/******************************************************************************/
void uartSendString(char *pS)
{
    while (*pS)                      //检测字符串结束标志
    {
        uartSendData(*pS++);         //发送当前字符
    }
}

/******************************************************************************/
// 函数名称：time0ISR 
// 输入参数：无 
// 输出参数：无 
// 函数功能：串口0服务函数 
/******************************************************************************/
void time0ISR(void) interrupt 1 using 1
{
    EA = 0;
    bUartFlag = OFF;
    EA = 1;
}






