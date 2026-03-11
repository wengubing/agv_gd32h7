


/* Includes ------------------------------------------------------------------*/

#define  MB_GLOBALS

/* Includes ------------------------------------------------------------------*/

#include "..\..\Drivers\ModbusRtuPoll\ModbusRtuPoll.h"


#define		MS_To_50us_CNT(ms)		(ms * 20)			//1000/50

u16 timeout=0;

/* Private typedef -----------------------------------------------------------*/


//MASTER单命令结构体
typedef struct
{
 u32 addr;   //地址[低16位为操作地址,高16位的-低8位为站台号_高8位为区分线圈和寄存器
        //当使用RW功能码时[备注:RW功能码为读取数据的地址]
 u16 func2,  //命令
   n,    //数量,当使用RW功能码时[备注:RW功能码为读取数据的数量]
   wr_addr, //当使用RW功能码时[备注:RW功能码:为写从机数据的地址]
   wr_n1,  //当使用RW功能码时[备注:RW功能码为写从机的字节数]
   *phr_rd1, //读取过来的数据缓冲区
   *phr_wt1; //写入从机的数据缓冲区(仅RW指令使用)
}LINK_CMD;


/* Private define ------------------------------------------------------------*/





/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/





/***************************************************************************************
** 函数名称: modbus_init
** 功能描述: modbus初始化
** 参    数: None
** 返 回 值: None       
****************************************************************************************/
void modbus_init(unsigned short hr_nb2, unsigned char slv_adr2)
{
 modbus_slv_const.hr_nb1  = hr_nb2;
 modbus_slv_const.slv_adr1 = slv_adr2;
}

/***************************************************************************************
** 函数名称: link_para_init
** 功能描述: modbus_master通讯参数初始化
** 参    数: None
** 返 回 值: None       
****************************************************************************************/
unsigned char link_para_init(LINK_COMM_PARA *pLpara, u32 timeout, u32 send_dlay, u8 retry, ProtocolType prid)
{
 unsigned char res=0;
 
 if(prid == MODBUS_NULL)
  res=1;
 
 if(send_dlay<10)
  send_dlay=10;
 else if(send_dlay>1000)
  send_dlay=1000;
 
 if(timeout<10)
  timeout=10;
 else if(timeout>3000)
  timeout=3000;
 
 if(retry<1)
  retry=1;
 else if(retry>5)
  retry=5;

 //modbus_master通讯参数初始化
 pLpara->comm_timeout = timeout;
 pLpara->comm_retry = retry;
 pLpara->comm_Send_delay = send_dlay;
 
 pLpara->master_send_cnt=0;//清重发次数
 pLpara->link_comm_ok_cnt=0;
 pLpara->wait_timeout_cnt=0;
 pLpara->link_comm_err_cnt=0;
 pLpara->Protocol_id=prid;
 
 return res;
}

/***************************************************************************************
** 函数名称: Rs485_usart_tm_50us
** 功能描述: 3.5T时间判断函数
** 参    数: ST485_USART结构体指针
** 返 回 值: None
****************************************************************************************/
u8 Rs485_usart_tm_50us(ST485_USART *su)
{
	u8 res=0;

  if(su->jfc --) /* 3.5T判断 */
  {//启动3.5T
		su->Slave_timeout=0;		
		if(!(su -> jfc))
		{// 3.5T时间到
			Rs485_usart_tx_rx_rde(su->Ini_par.usartx, OFF);//接收一数据包后,舍弃usartx来的数据
			M_K_CLR_BIT(su->sta, B_EV_USART_TX);//清发送标志  		 
			su->sta = (0x01 << B_EV_USART_RX);//置收到一包数据事件
			res = B_EV_USART_RX;
		}
  }
  else
  {//usart无数据...
		su->jfc= 0;
		res = B_EV_USART_None;	
		if(su->Ini_par.workmode==RS485_Slave)
		{
			if(++su->Slave_timeout>=MS_To_50us_CNT(200)) 
			{
				su->Slave_timeout=0;
				M_K_CLR_BIT(su->sta, B_EV_USART_TX);//清发送标志
				Rs485_usart_tx_rx_rde(su->Ini_par.usartx, ON);//重新使能接收!!!
				res = B_EV_Rx_TimeOut;	
			}		
		}
  }
	
	return res;
}

/***************************************************************************************
** 函数名称: char_hl_short
** 功能描述: 两个字节合成一个整数
** 参    数: None
** 返 回 值: None       
****************************************************************************************/
unsigned short char_hl_short(unsigned char hi,unsigned lo)
{
 union{
 unsigned char ch[2];
 unsigned short s;
 }uu;
 uu.ch[0]=lo;
 uu.ch[1]=hi;
 return(uu.s);
}

/***************************************************************************************
** 函数名称: short_xch_hl
** 功能描述: 交换一个字的高、低字节
** 参    数: None
** 返 回 值: None       
****************************************************************************************/
unsigned short short_xch_hl(unsigned short i)
{
 union{
 unsigned char ch[2];
 unsigned short s;
 }u;
 u.ch[0]=(i>>8) & 0xff;
 u.ch[1]=i & 0xff;
 return(u.s);
}

/***************************************************************************************
** 函数名称: uint_xch_hl
** 功能描述: 依次交换4字节数据的位置
** 参    数: None
** 返 回 值: None       
****************************************************************************************/
unsigned int uint_xch_hl(unsigned int i)
{
 union{
 unsigned char ch[4];
 unsigned int  s;
 }u;
 
 u.ch[0]=(i>>24) & 0xff;
 u.ch[1]=(i>>16) & 0xff;
 u.ch[2]=(i>>8) & 0xff;
 u.ch[3]=i & 0xff;
 
 return(u.s);
}

/***************************************************************************************
** 函数名称: short_copy_xch
** 功能描述: short数据交换和复制
** 参    数: 
      t:指向hr的目标区域的指针;
      s:指向接收数据源缓冲区rb的指针;
      n:操作coil的个数
      b_sch:高低字节交换控制位
** 返 回 值: None       
****************************************************************************************/
void short_copy_xch(void *t, void *s, int n, bool b_xch)
{
 int i;
 unsigned short *pt,*ps;
 
 pt=(unsigned short *)t;
 ps=(unsigned short *)s;
 
 if(n>0)
 {
  if(!b_xch)//b_xch = 0
  {//不进行交换,直接赋值
   for(i=0;i<n;i++)
   {
    *pt=*ps;
    pt++;
    ps++;
   }
  }
  else//b_xch = 1
  {//高低字节交换
   for(i=0;i<n;i++)
   {
    *pt=short_xch_hl(*ps);
    pt++;
    ps++;
   }          
  }
 }
}

/***************************************************************************************
** 函数名称: modbus_crc_org
** 功能描述: crc计算
** 参    数: None
** 返 回 值: None       
****************************************************************************************/
unsigned short modbus_crc_org(unsigned short CRC_ORG,unsigned char *p,unsigned short length)
{
 unsigned char j;
 unsigned short reg_crc;

 reg_crc=CRC_ORG;
 
 do {
  reg_crc^=*p++;
  for(j=0;j<8;j++)
  {
   if(reg_crc&0x01)
   {
    reg_crc=(reg_crc>>1)^0xa001;
   }
   else
   {
    reg_crc=reg_crc>>1;
   }
  }
 }while(--length!=0);
  
 return(reg_crc);
}

/***************************************************************************************
** 函数名称: modbus_crc
** 功能描述: 带值算crc
** 参    数: None
** 返 回 值: None       
****************************************************************************************/
unsigned short modbus_crc(void *puchMsg, unsigned short usDataLen)
{
 return(modbus_crc_org(0xFFFF,puchMsg,usDataLen)); 
}

/***************************************************************************************
** 函数名称: modbus_chk_format
** 功能描述: 检测一包数据的合法性,不检查SLV，命令
** 参    数: rb2:原始数据缓冲区,n通讯读取的数据数量
** 返 回 值: None       
****************************************************************************************/
MODBUS_MASTER_ERR_INFO modbus_chk_format(unsigned char *rb2, int n, unsigned char addr)
{
 unsigned char slv;
 union{
    unsigned short crc_val;
    unsigned char ch[2];
    }crc;
 unsigned short crc_buf;

	slv = rb2[0];  //从机地址
	if(n<5)
	{
	return(MODBUS_ERR_BYTELESS);  //字节数太少
	}
 
	if(slv != addr)
	{
	 return(MODBUS_ERR_SLV);  //从机地址错
	}

	crc.ch[0]=rb2[n - 2];//低8位
	crc.ch[1]=rb2[n - 1];//高8位
	crc_buf = modbus_crc(rb2, (n - 2));
	if(crc.crc_val != crc_buf)
	{
		return(MODBUS_ERR_CRC);  //crc错
	}

	return(MODBUS_FORMAT_OK);
}

/***************************************************************************************
** 函数名称: read_coil_buf
** 功能描述: 根据给定参数读取Coil至缓冲区，返回需要传递的字节数
** 参    数: 
** 返 回 值: 字节数       
****************************************************************************************/
unsigned short read_coil_buf(
               unsigned short *pt16, //目标数据区指针
               unsigned short *phr, //缓冲区地址，HR首地址
               unsigned short da_adr,//COIL地址
               unsigned short da_n  //COIL个数
              )
{
 union{
  u32 *p32;
  u16 *p16;
  }ps;
 unsigned short i,j,k;

 //pt16=(u16 *)&rb[3];
 i=da_adr % 16; //确定移位值
 j=(da_n+15)/16; //HR数量
 
 ps.p16=phr+(da_adr/16);
 for(k=0;k<j;k++)
 {
  *pt16=(*ps.p32 >> i);
  ps.p16++;
  pt16++;
 }
 i=da_n%16;
 if(i!=0)
 {
  pt16--;
  *pt16 &=((1<<i) -1);
 }

 return((da_n+7)/8);
}

/***************************************************************************************
** 函数名称: set_hr_coil
** 功能描述: 根据参数设定COIL
** 参    数: 
** 返 回 值: None       
****************************************************************************************/
void set_hr_coil(
         unsigned short *phr,
         unsigned short *buf, //HR首地址,缓冲区地址
         unsigned short da_adr,//COIL地址
         unsigned short da_n  //COIL个数
        )
{
 union{
  u32 *p32;
  u16 *p16;
  }ps;
 unsigned short i,j,k,s_mask,e_mask;
 unsigned short *pt16;

 //ps.p16=(u16 *)&rb[7];
 ps.p16=buf;
 //pt16=&HR[da_adr/16];
 pt16=phr+da_adr/16;

 i=da_adr % 16;  //确定移位值
 j=(i + da_n + 15) / 16; //HR数量
 s_mask = 0 - (1 << i); //有效位=1
 e_mask = (da_adr + da_n) % 16;
 if(e_mask !=0)
 {
  e_mask=(1<<(e_mask))-1;  //有效位=1
 }
 else
 {
  e_mask=0xffff;
 }

 
 if(j==1)
 { //HR首尾一体
  j=s_mask & e_mask;
  i=*ps.p16 << i;
  i &= j;
  *pt16 &= ~j;
  *pt16 |= i;
 }
 else
 {
  k=*ps.p16 << i;  //设定第一个HR
  //k &= s_mask;
  *pt16 &= ~s_mask;
  *pt16 |= k;

  pt16++;
  //ps.p16++;
  i=16-i;

  j--;
  for(k=1;k<j;k++)
  {
   *pt16=*ps.p32 >>i;
   pt16++;
   ps.p16++;
  }
  k= *ps.p32 >>i;
  k &= e_mask;
  *pt16 &= ~e_mask;
  *pt16 |= k;
 } 
}

/***************************************************************************************
** 函数名称: task_modbus_slv_receive
** 功能描述: modbus从机处理任务
** 参    数: p:指向串行端口结构体的指针，phr：指向对应的HR首指针,prsp_n:指向回应字节数的指针
** 返 回 值: 返回1：表示接收一包数据成功       
****************************************************************************************/
MODBUS_SLV_INFO task_modbus_slv_receive(
 ST485_USART   *p,
 unsigned short *phr,
 unsigned int  *prsp_n,
 unsigned int  option)
{
 MODBUS_SLV_INFO r_dat;
 u8 func,byte_count,b_broadcast,slv,b_f0,b_rw,b_hr_ok,b_coil_ok,b_hr_bc_ok,b_coil_bc_ok,
  *rb;//接收数据缓冲区
 u16 da_adr,da_n,wr_adr,wr_n,n,i,rsp_n,hr_nb,adr_end1,
  *pt16;//指向数据池HR的指针

 r_dat = MODBUS_SLV_NULL;

 if(M_TST_BIT(p -> sta, B_EV_USART_RX) != 0) //判断是否有接收标志
 {
  M_K_CLR_BIT(p -> sta, B_EV_USART_RX);//清接收标志
  n = p -> rx_nb;
  p -> rx_nb = 0;//接收数据包长度清0
  rb = &p -> tr_buf[0];
  rsp_n = 0;
  b_f0 = 0;
  b_rw = !(option & MODBUS_OT_RO);

  if(modbus_chk_format(rb, n, modbus_slv_const.slv_adr1) == MODBUS_FORMAT_OK)
  {//数据包格式OK
   r_dat = MODBUS_SLV_FORMAT_OK;
   slv = rb[0];
   if(slv == 0)
   {//广播地址
    b_broadcast = 1;
    b_f0 = 1;
   }
   else
   {
    b_broadcast = 0;
    if(slv == modbus_slv_const.slv_adr1)
    {
     b_f0=1;
    }
   }
  }
  else
  {
   r_dat = MODBUS_SLV_FORMAT_ERR;
  }

  if(b_f0)
  {//开始数据包解析
   func = rb[1];  //命令
   da_adr = char_hl_short(rb[2],rb[3]);//读取HR起始地址
   da_n = char_hl_short(rb[4],rb[5]);//读取HRreg数量
   byte_count = rb[6]; // 得到寄存器数量对应的字节数(2~240)

   adr_end1 = da_adr + da_n;//结束地址
   hr_nb = modbus_slv_const.hr_nb1;//数据池个数
   b_hr_ok = ((da_n < 128) && (adr_end1 <= hr_nb));//判断coil数和起始地址是否合法
   b_coil_ok = ((da_n < (256 * 8)) && (adr_end1 <= hr_nb * 16));//最大coil限制在2048个以内
   b_hr_bc_ok = (byte_count == da_n * 2);//字节数=寄存器数*2
   b_coil_bc_ok = (byte_count == (da_n + 7) / 8);
     
   switch(func)
   {   
    case MD_RD_COIL: //读取线圈---01
            {
             if(b_coil_ok)
             {
              rb[2] = read_coil_buf((u16 *)&rb[3], phr, da_adr, da_n);
              rsp_n = rb[2]+3;
             }
            }
    break;
    case MD_FR_MCOIL: //强制多个线圈---15
            {
             if(b_coil_ok && b_coil_bc_ok && b_rw)
             {
              set_hr_coil(phr,(u16 *)&rb[7],da_adr,da_n);
              rsp_n=6;
             }
            }
    break;
     case MD_FR_MHR_RDHR: //强制并读取多HR---23
            {
             if(b_hr_ok && b_rw)
             {
              wr_adr=char_hl_short(rb[6],rb[7]);//写HR起始地址
              wr_n=char_hl_short(rb[8],rb[9]);//写HR个数
              byte_count=rb[10]; //字节计数;
              if(wr_n<128 && (wr_adr+wr_n)<=hr_nb && byte_count==wr_n*2)
              {
               short_copy_xch((void *)(phr+wr_adr),(void *)(&rb[11]),wr_n,true);//写多个HR
               rb[2]=da_n*2;
               short_copy_xch((void *)&rb[3],(void *)(phr+da_adr),da_n,true);//读取多个HR
               rsp_n=rb[2]+3;
              }
             }
           }
    break;
     case MD_RD_HR:  //读取HR---03
            {
             if(b_hr_ok)
             { //正常读取保持寄存器
              rb[2]=da_n*2;
              short_copy_xch((void *)&rb[3],(void *)(phr+da_adr),da_n,true);
              rsp_n=rb[2]+3;
             }
            }
     break;
     case MD_RD_IR:  //读取IR---04
            {
             if(b_hr_ok)
             { //正常读取保持寄存器
              rb[2]=da_n*2;
              short_copy_xch((void *)&rb[3],(void *)(phr+da_adr),da_n,true);
              rsp_n=rb[2]+3;
             }
            }
     break;						
     case MD_FR_SCOIL: //强制单个线圈---05
            {
             if((da_adr<(hr_nb*16)) && b_rw)
             {
              pt16=phr+(da_adr >> 4);//hr寄存器地址---da_adr/16
              i=1<<(da_adr & 15);//地址偏移量---da_adr %16
              rsp_n=6;
              if(da_n==0xff00)
              {
               *pt16 |= i;//coil置"1"
              }
              else if(da_n==0x0000)
              {
               *pt16 &= ~i;//coil置"0"
              }
              else
              {
               rsp_n=0;
              }
             }
            }
      break;
      case MD_FR_SHR:  //强制单个HR---06
            {
             if((n==8) && (da_adr<hr_nb) && b_rw)
             {
              *(phr+da_adr)=da_n;
              rsp_n=6;
             }
            }
      break;
      case MD_FR_MHR:  //强制多个HR---16
            {
             if(b_rw)
             {
              if(b_hr_ok && b_hr_bc_ok)
              {
               short_copy_xch((void *)(phr + da_adr), (void *)(&rb[7]), da_n, true);
               rsp_n = 6;
              }
             }
            }
       break;
       default:
          //命令格式错
      break;
   }
   if(rsp_n != 0)
   {
    r_dat = MODBUS_SLV_OK;
    if(b_broadcast)
    {//主机广播从机,不回应
     rsp_n = 0;
    }
   }
   else
   {
    r_dat = MODBUS_SLV_FUNC_ERR; //命令格式错误
   }
  }
  *prsp_n = rsp_n;
 }

 return(r_dat);
}
/***************************************************************************************
** 函数名称: task_modbus_send
** 功能描述: modbus发送处理函数
** 参    数: None
** 返 回 值: None       
****************************************************************************************/
void task_modbus_send(ST485_USART *p6, u16 rsp_nb2)
{
 //WorkModeType sendsel=(WorkModeType)Sel;
 unsigned char *tx_buf;
 union{
    unsigned char  ch[2];
    unsigned short data;
  }crc;

 if(rsp_nb2 != 0)
 {
  tx_buf = &p6 -> tr_buf[0];
  crc.data = modbus_crc(tx_buf, rsp_nb2);//计算发送数据包的CRC值
  tx_buf[rsp_nb2 ++] = crc.ch[0];
  tx_buf[rsp_nb2 ++] = crc.ch[1];
	 
  Rs485_send_msg(p6, rsp_nb2);
 }
}

/***************************************************************************************
** 函数名称: task_modbus_rtu_slv
** 功能描述: 接收数据包解析,Modbus_RTU协议,实现从机一次成功的通讯(主机无数据包时,自动开启接收中断)
** 参    数: Modbus从模式函数;
       p:串口结构体;
       phr:HR的指针,
       option:bit0=1只读,=0读写
       usart_port:usart端口号:usart1,usart2,usart3
** 返 回 值: rsta:
       #define MODBUS_SLV_NULL     0 //无操作
       #define MODBUS_SLV_OK      1 //操作正确
       #define MODBUS_SLV_FORMAT_OK  2 //格式正确,但不是自己的数据包
       #define MODBUS_SLV_FORMAT_ERR  3 //格式错误
       #define MODBUS_SLV_FUNC_ERR   4 //命令错误
****************************************************************************************/
MODBUS_SLV_INFO task_modbus_rtu_slv(ST485_USART *p, unsigned short *phr, u16 option)
{
 MODBUS_SLV_INFO  rsta = MODBUS_SLV_NULL; //接收数据状态:1表示成功接收一包数据
 unsigned int  rsp_nb = 0;//应答字节数

 rsta = task_modbus_slv_receive(p, phr, &rsp_nb, option);
 if((rsta == MODBUS_SLV_OK) && (rsp_nb != 0))
 {
  task_modbus_send(p, rsp_nb);
 }

 return rsta;
}

/***************************************************************************************
** 函数名称: modbus_tcp_chk_format
** 功能描述: 检测一包数据的合法性,不检查SLV，命令
** 参    数: rb2:原始数据缓冲区,n通讯读取的数据数量
** 返 回 值: None       
****************************************************************************************/
MODBUS_MASTER_ERR_INFO modbus_tcp_chk_format(unsigned char *rb2, int n, unsigned char addr)
{
    unsigned char slv;

    slv = rb2[6];  //从机地址
    
    //8个字节取值原因：6个tcp协议头 + 1 从机ID + 1个功能号
    if(n<8)
    {
        return(MODBUS_ERR_BYTELESS);  //字节数太少
    }

    if(slv != addr)
    {
        return(MODBUS_ERR_SLV);  //从机地址错
    }

    return(MODBUS_FORMAT_OK);
}

/***************************************************************************************
** 函数名称: task_modbus_tcp_slv_receive
** 功能描述: modbus tcp从机处理任务
** 参    数: p:指向串行端口结构体的指针，phr：指向对应的HR首指针,prsp_n:指向回应字节数的指针
** 返 回 值: 返回1：表示接收一包数据成功       
****************************************************************************************/
MODBUS_SLV_INFO task_modbus_tcp_slv_receive(
 ST485_USART   *p,
 unsigned short *phr,
 unsigned int  *prsp_n,
 unsigned int  option)
{
    
    u8 modbus_tcp_frameheand = 6;   //modbustcp 帧头为 01 02 00 00 00 06
                                    //01 02 心跳值    
                                    //00 00 固定码
                                    //00 06 数据长度
    
    MODBUS_SLV_INFO r_dat;
    u8 func,byte_count,b_broadcast,slv,b_f0,b_rw,b_hr_ok,b_coil_ok,b_hr_bc_ok,b_coil_bc_ok,
    *rb;//接收数据缓冲区
    u16 da_adr,da_n,wr_adr,wr_n,n,i,rsp_n,hr_nb,adr_end1,
    *pt16;//指向数据池HR的指针

    r_dat = MODBUS_SLV_NULL;

    if(M_TST_BIT(p -> sta, B_EV_USART_RX) != 0) //判断是否有接收标志
    {
        M_K_CLR_BIT(p -> sta, B_EV_USART_RX);//清接收标志
        n = p -> rx_nb;
        p -> rx_nb = 0;//接收数据包长度清0
        rb = &p -> tr_buf[0];
        rsp_n = 0;
        b_f0 = 0;
        b_rw = !(option & MODBUS_OT_RO);

        if(modbus_tcp_chk_format(rb, n, modbus_slv_const.slv_adr1) == MODBUS_FORMAT_OK)
        {//数据包格式OK
            r_dat = MODBUS_SLV_FORMAT_OK;
            slv = rb[0 + modbus_tcp_frameheand];
            if(slv == 0)
            {//广播地址
                b_broadcast = 1;
                b_f0 = 1;
            }
            else
            {
                b_broadcast = 0;
                if(slv == modbus_slv_const.slv_adr1)
                {
                    b_f0=1;
                }
            }
        }
        else
        {
            r_dat = MODBUS_SLV_FORMAT_ERR;
        }

        if(b_f0)
        {//开始数据包解析
           func = rb[1 + modbus_tcp_frameheand];  //命令
           da_adr = char_hl_short(rb[2 + modbus_tcp_frameheand],rb[3 + modbus_tcp_frameheand]);//读取HR起始地址
           da_n = char_hl_short(rb[4 + modbus_tcp_frameheand],rb[5 + modbus_tcp_frameheand]);//读取HRreg数量
           byte_count = rb[6 + modbus_tcp_frameheand]; // 得到寄存器数量对应的字节数(2~240)

           adr_end1 = da_adr + da_n;//结束地址
           hr_nb = modbus_slv_const.hr_nb1;//数据池个数
           b_hr_ok = ((da_n < 128) && (adr_end1 <= hr_nb));//判断coil数和起始地址是否合法
           b_coil_ok = ((da_n < (256 * 8)) && (adr_end1 <= hr_nb * 16));//最大coil限制在2048个以内
           b_hr_bc_ok = (byte_count == da_n * 2);//字节数=寄存器数*2
           b_coil_bc_ok = (byte_count == (da_n + 7) / 8);
             
           switch(func)
           {   
            case MD_RD_COIL: //读取线圈---01
                    {
                     if(b_coil_ok)
                     {
                      rb[2 + modbus_tcp_frameheand] = read_coil_buf((u16 *)&rb[3 + modbus_tcp_frameheand], phr, da_adr, da_n);
                      rsp_n = rb[2 + modbus_tcp_frameheand]+3;
                     }
                    }
            break;
            case MD_FR_MCOIL: //强制多个线圈---15
                    {
                     if(b_coil_ok && b_coil_bc_ok && b_rw)
                     {
                      set_hr_coil(phr,(u16 *)&rb[7 + modbus_tcp_frameheand],da_adr,da_n);
                      rsp_n=6;
                     }
                    }            
            break;
             case MD_FR_MHR_RDHR: //强制并读取多HR---23
//                    {
//                     if(b_hr_ok && b_rw)
//                     {
//                      wr_adr=char_hl_short(rb[6],rb[7]);//写HR起始地址
//                      wr_n=char_hl_short(rb[8],rb[9]);//写HR个数
//                      byte_count=rb[10]; //字节计数;
//                      if(wr_n<128 && (wr_adr+wr_n)<=hr_nb && byte_count==wr_n*2)
//                      {
//                       short_copy_xch((void *)(phr+wr_adr),(void *)(&rb[11]),wr_n,true);//写多个HR
//                       rb[2]=da_n*2;
//                       short_copy_xch((void *)&rb[3],(void *)(phr+da_adr),da_n,true);//读取多个HR
//                       rsp_n=rb[2]+3;
//                      }
//                     }
//                   }
                                 {
                     if(b_hr_ok && b_rw)
                     {
                      wr_adr=char_hl_short(rb[6 + modbus_tcp_frameheand],rb[7 + modbus_tcp_frameheand]);//写HR起始地址
                      wr_n=char_hl_short(rb[8 + modbus_tcp_frameheand],rb[9 + modbus_tcp_frameheand]);//写HR个数
                      byte_count=rb[10 + modbus_tcp_frameheand]; //字节计数;
                      if(wr_n<128 && (wr_adr+wr_n)<=hr_nb && byte_count==wr_n*2)
                      {
                       short_copy_xch((void *)(phr+wr_adr),(void *)(&rb[11 + modbus_tcp_frameheand]),wr_n,true);//写多个HR
                       rb[2 + modbus_tcp_frameheand]=da_n*2;
                       short_copy_xch((void *)&rb[3 + modbus_tcp_frameheand],(void *)(phr+da_adr),da_n,true);//读取多个HR
                       rsp_n=rb[2 + modbus_tcp_frameheand]+3;
                       rb[5]=rsp_n;   
                      }
                     }
                   }             
            break;
             case MD_RD_HR:  //读取HR---03
                    {
//                     if(b_hr_ok)
//                     { //正常读取保持寄存器
//                      rb[2]=da_n*2;
//                      short_copy_xch((void *)&rb[3],(void *)(phr+da_adr),da_n,true);
//                      rsp_n=rb[2]+3;
//                     }
                     if(b_hr_ok)
                     { //正常读取保持寄存器
                      rb[8]=da_n*2;
                      short_copy_xch((void *)&rb[9],(void *)(phr+da_adr),da_n,true);
                      rsp_n=rb[8]+3;
                      rb[5]=rsp_n;
                     }                        
                    }
             break;
             case MD_FR_SCOIL: //强制单个线圈---05
                    {
                     if((da_adr<(hr_nb*16)) && b_rw)
                     {
                      pt16=phr+(da_adr >> 4);//hr寄存器地址---da_adr/16
                      i=1<<(da_adr & 15);//地址偏移量---da_adr %16
                      rsp_n=6;
                      if(da_n==0xff00)
                      {
                       *pt16 |= i;//coil置"1"
                      }
                      else if(da_n==0x0000)
                      {
                       *pt16 &= ~i;//coil置"0"
                      }
                      else
                      {
                       rsp_n=0;
                      }
                     }
                    }
              break;
              case MD_FR_SHR:  //强制单个HR---06
//                    {
//                     if((n==8) && (da_adr<hr_nb) && b_rw)
//                     {
//                      *(phr+da_adr)=da_n;
//                      rsp_n=6;
//                     }
//                    }
                    {
                     //tcp协议头为6个字节，数据域为6个字节， 6 + 6  
                     if((n==12) && (da_adr<hr_nb) && b_rw)
                     {
                      *(phr+da_adr)=da_n;
                      rsp_n=6;
                     }
                    }              
              break;
              case MD_FR_MHR:  //强制多个HR---16
//                    {
//                     if(b_rw)
//                     {
//                      if(b_hr_ok && b_hr_bc_ok)
//                      {
//                       short_copy_xch((void *)(phr + da_adr), (void *)(&rb[7]), da_n, true);
//                       rsp_n = 6;
//                      }
//                     }
//                    }
                    {
                     if(b_rw)
                     {
                      if(b_hr_ok && b_hr_bc_ok)
                      {
                       short_copy_xch((void *)(phr + da_adr), (void *)(&rb[7 + modbus_tcp_frameheand]), da_n, true);
                       rsp_n = 6;
                       rb[5]=rsp_n;   
                      }
                     }
                    }              
               break;
               default:
                  //命令格式错
              break;
           }
           if(rsp_n != 0)
           {
            r_dat = MODBUS_SLV_OK;
            if(b_broadcast)
            {//主机广播从机,不回应
             rsp_n = 0;
            }
           }
           else
           {
            r_dat = MODBUS_SLV_FUNC_ERR; //命令格式错误
           }
        }
        *prsp_n = (rsp_n + modbus_tcp_frameheand);
    }

    return(r_dat);
}

/***************************************************************************************
** 函数名称: task_modbus_tcp_send
** 功能描述: modbus tcp发送处理函数
** 参    数: None
** 返 回 值: None       
****************************************************************************************/
void task_modbus_tcp_send(ST485_USART *p6, u16 rsp_nb2)
{ 
    Rs485_send_msg(p6, rsp_nb2);
}

/***************************************************************************************
** 函数名称: task_modbus_tcp_slv
** 功能描述: 接收数据包解析,Modbus_TCP协议,实现从机一次成功的通讯(主机无数据包时,自动开启接收中断)
** 参    数: Modbus从模式函数;
       p:串口结构体;
       phr:HR的指针,
       option:bit0=1只读,=0读写
       usart_port:usart端口号:usart1,usart2,usart3
** 返 回 值: rsta:
       #define MODBUS_SLV_NULL     0 //无操作
       #define MODBUS_SLV_OK      1 //操作正确
       #define MODBUS_SLV_FORMAT_OK  2 //格式正确,但不是自己的数据包
       #define MODBUS_SLV_FORMAT_ERR  3 //格式错误
       #define MODBUS_SLV_FUNC_ERR   4 //命令错误
****************************************************************************************/
MODBUS_SLV_INFO task_modbus_tcp_slv(ST485_USART *p, unsigned short *phr, u16 option)
{
    MODBUS_SLV_INFO  rsta = MODBUS_SLV_NULL; //接收数据状态:1表示成功接收一包数据
    unsigned int  rsp_nb = 0;//应答字节数

    rsta = task_modbus_tcp_slv_receive(p, phr, &rsp_nb, option);
    
    if((rsta == MODBUS_SLV_OK) && (rsp_nb != 0))
    {
        task_modbus_tcp_send(p, rsp_nb);
    }

    return rsta;
}

/***************************************************************************************
** 函数名称: task_modbus_rtu_master_send
** 功能描述: modbus_rtu主机打包和发送任务,主机把数据包发送完成后,立即开启usart接收使能
** 参    数: 
      p:指向串行端口结构体的指针
      pm:指向对应主模式命令结构体指针
      b_dir:直接写入
** 返 回 值: none
****************************************************************************************/
void task_modbus_rtu_master_send(ST485_USART *p, ST_MODBUS_MASTER *pm, u8 b_dir)
{
 u8 func,
   b_hr_ok,
   b_coil_ok,
   *send_buf;//发送缓冲区指针
 u16 dat_adr_md,
   dat_num_md,//字节数
   wr_adr,
   wr_n,
   i,
   send_num,
   hr_nb,
   adr_end1,
   *p_phr_write;//(从)读/(向)写从机HR的指针buf

 p_phr_write = pm -> pwr_write;

 if(pm -> mb_sta == MD_MST_STA_READY)
 {//上一帧数据发送完成,启动下一帧数据发送
  func = pm -> func;
  dat_adr_md = pm -> da_adr;
  dat_num_md = pm -> dat_num;
  
  /* 仅23功能码 */
  if(func == MD_FR_MHR_RDHR)
  {
   wr_adr = pm -> wr_adr;
   wr_n = pm -> wr_n; 
  }
  
  send_buf = &(p->tr_buf[0]);
  
  /* 发送数据准备 */
  send_buf[0] = pm -> slv;//站号
  send_buf[1] = func;//功能码
  send_buf[2] = dat_adr_md >> 8;//地址
  send_buf[3] = dat_adr_md & 0xff;
  send_buf[4] = dat_num_md >> 8;//数量
  send_buf[5] = dat_num_md & 0xff;

  adr_end1 = dat_adr_md + dat_num_md;
  hr_nb = modbus_slv_const.hr_nb1;//获取HR数据池大小

  if(b_dir)
  {
   b_hr_ok = 1;
   b_coil_ok = 1;
  }
  else
  {
   b_hr_ok = (dat_num_md < 128 && adr_end1 <= hr_nb);
   b_coil_ok = ((dat_num_md < (256 * 8)) && (adr_end1 <= hr_nb * 16));
  }
  
  if(b_dir)
  {
   dat_adr_md = 0;
   wr_adr = 0;
  }
  
  send_num = 0;
  
  switch(func)
  {
   case MD_RD_COIL://读单个线圈
   case MD_RD_DISCRETE://读离散输入状态
    if(b_coil_ok)
    {
     send_num = 6;
    }
    break;
   case MD_FR_SCOIL://强制单个线圈
    if(dat_adr_md < hr_nb * 16)
    {
     send_buf[5] = 0x00;
     i=*(p_phr_write + dat_adr_md / 16);
     if(i & (1 << dat_adr_md))
     {
      send_buf[4] = 0xFF;
      pm -> dat_num = 0xff00;
     }
     else
     {
      send_buf[4] = 0x00;
      pm -> dat_num = 0x0000;
     }
     send_num = 6;
    }
    break;
   case MD_FR_MCOIL://强制多个线圈
    if(b_coil_ok)
    {
     send_buf[6] = read_coil_buf((u16 *)&send_buf[7], p_phr_write, dat_adr_md, dat_num_md); // COIL地址，COIL个数
     send_num = send_buf[6] + 7;
    }
    break;
   case MD_RD_HR://读取存器
    if(b_hr_ok)
    {
     send_num = 6;
    }
    break;
   case MD_RD_IR://读取存器
    if(b_hr_ok)
    {
     send_num = 6;
    }
    break;		
   case MD_FR_SHR://强制单个HR
    if(dat_adr_md < hr_nb)
    {
     dat_num_md = *(p_phr_write + dat_adr_md);//获取当前需要的操作数据
     send_buf[4] = dat_num_md >> 8;//数据的高8位
     send_buf[5] = dat_num_md & 0xff;//数据的低8位
     pm -> dat_num = dat_num_md; //用于master接收时判断与发出的数据是否一致
     send_num = 6;
    }
    break;
   case MD_FR_MHR://强制多个HR
    if(b_hr_ok)
    {
     short_copy_xch(&send_buf[7], (p_phr_write + dat_adr_md), dat_num_md, true);
     send_buf[6] = dat_num_md * 2;
     send_num = send_buf[6] + 7;
    }
    break;
   case MD_FR_MHR_RDHR://读写多个寄存器
    if(b_hr_ok)
    {
     if(wr_n < 128)
     {
      //写从机地址
      send_buf[6] = (pm -> wr_adr) >> 8;
      send_buf[7] = (pm -> wr_adr) & 0xff;
      //写从机数量
      send_buf[8] = 0;
      send_buf[9] = wr_n;
      //写入字节计数
      send_buf[10] = wr_n * 2; 
      short_copy_xch(&send_buf[11], p_phr_write + wr_adr, wr_n, true);
      send_num = 11 + send_buf[10];
     }
    }
    break;
   default:
    {
     send_num=0;
    }
    break;
  }
  
  if(send_num>0)
  {
   task_modbus_send(p, send_num);//将一帧数据发出
   pm -> mb_sta = MD_MST_STA_INPROCESS;//置主机状态
  }
  else
  {
   pm -> mb_sta = MD_MST_STA_SEND_ERR;
  }
 }
}

/***************************************************************************************
** 函数名称: task_modbus_rtu_master_receive
** 功能描述: modbus_rtu主机接收任务
** 参    数: 
      p:指向串行端口结构体的指针
      pm:指向对应主模式命令结构体指针
      b_wr_dir:直接写入
** 返 回 值: none
****************************************************************************************/
MD_MST_STA task_modbus_rtu_master_receive(ST485_USART *p, ST_MODBUS_MASTER *pm,  u8 b_dir)
{
 u8 func=0,
   byte_count=0,
   b_f0=0,
   *re_buf=0;//接收缓冲区指针
 u16 dat_adr_md_r=0,//数据地址
   dat_num_r=0,
   rsp_adr=0,
   n=0,
   *p_hr_read=0;//(从)读/(向)写从机HR的指针buf
 MODBUS_MASTER_ERR_INFO b_format;
 MD_MST_STA mb_sta;
	
 if(M_TST_BIT(p -> sta, B_EV_USART_RX) != 0) // 判断是否有接收标志
 {
  M_K_CLR_BIT(p -> sta, B_EV_USART_RX); // 清接收标志 
  
  p_hr_read = pm -> phr_read;
  n = p -> rx_nb;
  p -> rx_nb = 0;
  re_buf = &(p->tr_buf[0]);//获取接收区起始地址
  b_f0 = 2;

  b_format = modbus_chk_format(re_buf, n, pm->slv);//格式检查
  
  if(b_format == MODBUS_FORMAT_OK)
  {
   func = re_buf[1];  //命令
   dat_adr_md_r = char_hl_short(re_buf[2], re_buf[3]);
   dat_num_r = char_hl_short(re_buf[4], re_buf[5]);
   byte_count = re_buf[2]; //字节计数
   if(b_dir)
   {
    rsp_adr = 0;
   }
   else
   {
    rsp_adr = pm -> da_adr;
   }

   if((re_buf[0] == pm -> slv) && (func == pm -> func))
   {
    switch(func)
    {
     case MD_FR_SCOIL: //强制单个线圈
     case MD_FR_MCOIL: //强制多个线圈
     case MD_FR_SHR:  //强制单个HR
     case MD_FR_MHR:  //强制多个HR
      if(dat_adr_md_r == pm->da_adr && dat_num_r == pm->dat_num)
      {
       b_f0=1;
      }
      break;
     case MD_RD_COIL: //读取线圈
	 case MD_RD_DISCRETE: //读取离散量
      if(byte_count == (pm -> dat_num + 7) / 8)
      {
       set_hr_coil(p_hr_read, (u16 *) & re_buf[3], rsp_adr, pm -> dat_num);
       b_f0 = 1;
      }
     break;
     case MD_RD_HR:    //读取HR
     case MD_RD_IR:    //读取IR			 
     case MD_FR_MHR_RDHR: //强制并读取
      if(byte_count == pm -> dat_num * 2)
      {
       short_copy_xch(p_hr_read + rsp_adr, &re_buf[3], pm -> dat_num, true);
       b_f0 = 1;
      }
     break;
     default://命令格式错
     break;
    }
   }
  }
 }
 
 if(b_f0==0)
 {
  mb_sta = MD_MST_STA_NO_REC;//置未收到数据包
 } 
 else if(b_f0==1)
 {
  mb_sta = MD_MST_STA_OK;//置OK标志
 }
 else if(b_f0==2)
 {
  mb_sta = MD_MST_STA_REC_ERR;//置错误标志
 }
 
 return mb_sta;
}

/***************************************************************************************
** 函数名称: master_send_process
** 功能描述: 主机数据包解析及发送
***************************************************************************************/
void master_send_process(ST485_USART *s, LINK_QUE *k, ST_MODBUS_MASTER *g)
{
  LINK_CMD  cmd_new;
 
  /* 解析命令队列 */
  cmd_new.addr = (((u32)k->sel_coil_reg << 24) 
         | ((u32)k->slv_addr << 16)//获取连接从机的站台号
         | k->rw_slv_first_hr_addr);//获取单指令r/w地址
  cmd_new.func2 = k->func;//获取当前功能码
  cmd_new.n = k->rw_num;//获取读/写从机数量
  if(cmd_new.func2 == MD_FR_MHR_RDHR)
  {//如果是RW指令
   cmd_new.wr_addr = k->wr_slv_first_hr_addr;//获取写从机地址
   cmd_new.wr_n1  = k->write_num;//获取写从机数量
  }

//    coil_bit = (cmd_new.addr >> 24) & 0xff;//区分线圈[0]和寄存器[1]
  g->phr_read  = k->phr_read;
  g->pwr_write = k->phr_write;
  g->slv = (cmd_new.addr >> 16) & 0xff; //站台号
  g->func = (MD_FUNC)cmd_new.func2;   //功能码
  g->da_adr = cmd_new.addr & 0xffff;   //操作地址
  //if(coil_bit)//判断是否是coil操作
  g->dat_num = cmd_new.n;//操作数量
//    else
//     g->dat_num = cmd_new.n/2;
  
  /* 仅23功能码 */
  if(g->func == MD_FR_MHR_RDHR)
  {
   g->wr_adr = cmd_new.wr_addr;
   g->wr_n = cmd_new.wr_n1;   
  }
  
  g->mb_sta = MD_MST_STA_READY;//可以开始发送数据
  
  task_modbus_rtu_master_send(s, g, 1);//发送数据...
  
  if(g->mb_sta == MD_MST_STA_INPROCESS)
  {//进行中...
   g->Master_link_Sta = STA_RX;//置等待数据回应 
  }
  else
  {//如果发送数据准备有错
   g->Master_link_Sta = STA_TX;//重新启动发送
  }
}

/***************************************************************************************
** 函数名称: master_send_ready
** 功能描述: 主机发送准备
***************************************************************************************/
MD_MST_STA master_send_ready(LINK_COMM_PARA *para, ST485_USART *st, ST_MODBUS_MASTER *psm)
{
 switch (psm->mb_sta)
 {
  //更换命令队列
  case MD_MST_STA_OK://1次连接通讯是否OK
  case MD_MST_STA_RETRYED://link次数超过设定值
  case MD_MST_STA_INVALID_FUNC://无效指令
  {
    st->rx_nb = 0;//将接收数据长度清0
		if(psm->mb_sta==MD_MST_STA_OK || psm->mb_sta==MD_MST_STA_RETRYED)
		{
			if(psm->mb_sta==MD_MST_STA_OK)
			{
			 para->link_comm_ok_cnt++;
			}			
			para->master_send_cnt = 0;
			para->wait_timeout_cnt=0;			
		}

    psm->Master_link_Sta=STA_TX;
  }
  break;
  
  //数据包重发
  case MD_MST_STA_TIMEOUT:
  case MD_MST_STA_REC_ERR:
  {
    psm->Master_link_Sta=STA_TX; 
    para->wait_timeout_cnt=0;
    para->link_comm_err_cnt++;//错误计数
  }
  break;
  
  default:
  break;
 }
 
 para->MasterSend_dealy_cnt = Dylinkcomm_1ms_cnt;//置发送延时计数器
 
 return psm->mb_sta;
}
/***************************************************************************************
** 函数名称: link_TimeoutProcess
** 功能描述: link超时处理
***************************************************************************************/
void link_TimeoutProcess(LINK_COMM_PARA *plinkpara, ST485_USART *pSTu, ST_MODBUS_MASTER *pSTpm)
{
    if(pSTpm->mb_sta == MD_MST_STA_OK)//接收数据处理
    {//收到正确的回应,表示一次完整的通讯完成
      /*初始化标志*/
      pSTpm->mb_sta=master_send_ready(plinkpara, pSTu, pSTpm); //---------------------------->>>
    }
    else if(pSTpm->mb_sta == MD_MST_STA_REC_ERR)
    {//收到错误数据包,此时,master重发！
     if(plinkpara->master_send_cnt>=plinkpara->comm_retry)
     {//到达重发指定次数(收到错误数据包)
      pSTpm->mb_sta=MD_MST_STA_RETRYED;
     }
     pSTpm->mb_sta=master_send_ready(plinkpara, pSTu, pSTpm); //---------------------------->>>     
    }
    else if(Delay_Fuction(Dylinkcomm_1ms_cnt, plinkpara->wait_recive_timeout_cnt, 1) && pSTpm->mb_sta == MD_MST_STA_NO_REC)
    {//未收到数据包
     /*超时判断*/
        u32 timeoutcnt=0;
        plinkpara->wait_timeout_cnt++;
        
//        if(plinkpara->TimeoutInterval_flag)
//            timeoutcnt=3*plinkpara->comm_timeout;		
//        timeoutcnt=plinkpara->comm_timeout;
//        else																	
        timeoutcnt=plinkpara->comm_timeout;
        
     if(plinkpara->wait_timeout_cnt >= timeoutcnt)
     {//超时...
      if(plinkpara->master_send_cnt<plinkpara->comm_retry)
      {//需要重发(从机无响应)
       pSTpm->mb_sta=MD_MST_STA_TIMEOUT;
//			 plinkpara->TimeoutInterval_flag=0;
      }
      else 
      {//达到重发指定次数(从机无响应)
       pSTpm->mb_sta=MD_MST_STA_RETRYED;
//			 plinkpara->TimeoutInterval_flag++;
      }
      pSTpm->mb_sta=master_send_ready(plinkpara, pSTu, pSTpm); //---------------------------->>>   
     }
     plinkpara->wait_recive_timeout_cnt=Dylinkcomm_1ms_cnt;    
    }
}

/***************************************************************************************
** 函数名称: link_ptl_modbus_rtu
** 功能描述: 
1.通讯连接任务,实现数据包的发送和接收一体，并返回执行的状态
2.本函数需要在大循环中非阻塞调用;
***************************************************************************************/
MD_MST_STA link_ptl_modbus_rtu(LINK_COMM_PARA *plinkpara, LINK_QUE *plinkque)
{
  ST485_USART      *pSTu=plinkpara->pST;
	ST_MODBUS_MASTER *pSTpm = &plinkpara->mb_master;

	pSTpm->mb_sta=MD_MST_STA_NULL;
	
  switch (pSTpm->Master_link_Sta)
  {
   case STA_TX://master发送
   {
    if(Delay_Fuction(Dylinkcomm_1ms_cnt, plinkpara->MasterSend_dealy_cnt, plinkpara->comm_Send_delay))
    {//发送间隔时间到....
     if(MD_FUNC_NULL==(MD_FUNC)plinkque->func)
     {
      pSTpm->mb_sta=MD_MST_STA_INVALID_FUNC;
     }
     else
     {
      master_send_process(pSTu, plinkque, pSTpm);//发送处理
      if(pSTpm->Master_link_Sta==STA_RX)
      {//成功发送
       plinkpara->master_send_cnt++;//发送次数
       plinkpara->wait_recive_timeout_cnt=Dylinkcomm_1ms_cnt;      
      }
      else if(pSTpm->Master_link_Sta==STA_TX)
      {
       plinkpara->master_send_cnt = 0;
       pSTpm->mb_sta=MD_MST_STA_NULL;
      }
      
      pSTpm->mb_sta=master_send_ready(plinkpara, pSTu, pSTpm); //---------------------------->>>      
     }
    }
   }
   break;
   case STA_RX://master接收
   {
    pSTpm->mb_sta = task_modbus_rtu_master_receive(pSTu, pSTpm, 1);
		link_TimeoutProcess(plinkpara, pSTu, pSTpm);
   }
   break;
   default:
   break;
  }

 return pSTpm->mb_sta;
}

/************************************END OF FILE*****************************************/
