/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_IODef.h
** 创 建 人: 文小兵
** 描    述: 板载IO数据对象位定义
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0
** 创建日期: 2024年5月9日
**--------------------------------------------------------------------------------------
***************************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __drv_IODef_h__
#define __drv_IODef_h__

/*输入状态定义*/
#define		R_BIT_XIN_STATUS								0 	//XIN8-XIN51,占用44bit,给3个DHR...DHR[0]-DHR[2]
#define		XIN_DHR_NUM										3
#define		B0												0	 //XIN8
#define		B1												1	 //XIN9
#define		B2												2	 //XIN10
#define		B3												3	 //XIN11
#define		B4												4	 //XIN12
#define		B5												5	 //XIN13
#define		B6												6	 //XIN14
#define		B7												7	 //XIN15
#define		B8												8    //XIN16
#define		B9												9    //XIN17
#define		B10												10	 //XIN18
#define		B11												11	 //XIN19
#define		B12												12	 //XIN20
#define		B13												13	 //XIN21
#define		B14												14	 //XIN22
#define		B15												15	 //XIN23
#define		B16												16	 //XIN24
#define		B17												17	 //XIN25
#define		B18												18	 //XIN26
#define		B19												19	 //XIN27
#define		B20												20	 //XIN28
#define		B21												21	 //XIN29
#define		B22												22	 //XIN30
#define		B23												23	 //XIN31
#define		B24												24	 //XIN32
#define		B25												25	 //XIN33
#define		B26												26	 //XIN34
#define		B27												27	 //XIN35
#define		B28												28	 //XIN36
#define		B29												29	 //XIN37
#define		B30												30	 //XIN38
#define		B31												31	 //XIN39
#define		B32												32	 //XIN40
#define		B33												33	 //XIN41
#define		B34												34	 //XIN42
#define		B35												35	 //XIN43
#define		B36												36	 //XIN44
#define		B37												37	 //XIN45
#define		B38												38	 //XIN46
#define		B39												39	 //XIN47
#define		B40												40	 //XIN48
#define		B41												41	 //XIN49
#define		B42												42	 //XIN50
#define		B43												43	 //XIN51

/*输出状态定义*/
#define		RW_BIT_YOUT_STATUS				                3 	//Y4-Y71,占用68bit,给5个DHR...DHR[3]-DHR[7]
#define		YOUT_LS_BIT_NUM						            68
#define		YOUT_DHR_NUM_LS						            5
//高速输出(可配置位PWM脉冲或数字频率输出)
#define		B48												48	 //Y0
#define		B49												49	 //Y1
#define		B50												50	 //Y2
#define		B51												51	 //Y3	
//驱动电机使用专用位(Y4-Y23)
#define		B52												52	 //Y4  //使能信号1:启动/停止输入-0:启动,1:停止
#define		B53												53	 //Y5
#define		B54												54	 //Y6  //制动刹车信号1:运行/急停brake输入-0:运行，1:瞬时停止
#define		B55												55	 //Y7  //旋转方向切信号1-1:顺CW,0:逆CCW
#define		B56												56	 //Y8
#define		B57												57	 //Y9  //使能信号2:启动/停止输入-0:启动,1:停止
#define		B58												58	 //Y10
#define		B59												59	 //Y11 //制动刹车信号2:运行/急停brake输入-0:运行，1:瞬时停止
#define		B60												60	 //Y12 //旋转方向切信号2-1:顺CW,0:逆CCW
#define		B61												61	 //Y13
#define		B62												62	 //Y14 //使能信号3:启动/停止输入-0:启动,1:停止
#define		B63												63	 //Y15
#define		B64												64	 //Y16 //制动刹车信号3:运行/急停brake输入-0:运行，1:瞬时停止
#define		B65												65	 //Y17 //旋转方向切信号3-1:顺CW,0:逆CCW
#define		B66												66	 //Y18
#define		B67												67	 //Y19 //使能信号4:启动/停止输入-0:启动,1:停止
#define		B68												68	 //Y20
#define		B69												69	 //Y21 //制动刹车信号4:运行/急停brake输入-0:运行，1:瞬时停止
#define		B70												70	 //Y22 //旋转方向切信号4-1:顺CW,0:逆CCW
#define		B71												71	 //Y23
//普通Yx输出位
#define		B72												72	 //Y24
#define		B73												73	 //Y25
#define		B74												74	 //Y26
#define		B75												75	 //Y27
#define		B76												76	 //Y28
#define		B77												77	 //Y29
#define		B78												78	 //Y30
#define		B79												79	 //Y31
#define		B80												80	 //Y32		----595
#define		B81												81	 //Y33
#define		B82												82	 //Y34
#define		B83												83	 //Y35
#define		B84												84	 //Y36
#define		B85												85	 //Y37
#define		B86												86	 //Y38
#define		B87												87	 //Y39
#define		B88												88	 //Y40
#define		B89												89	 //Y41
#define		B90												90	 //Y42
#define		B91												91	 //Y43
#define		B92												92	 //Y44
#define		B93												93	 //Y45
#define		B94												94	 //Y46
#define		B95												95	 //Y47
#define		B96												96	 //Y48
#define		B97												97	 //Y49
#define		B98												98	 //Y50
#define		B99												99	 //Y51
#define		B100											100	 //Y52
#define		B101											101	 //Y53
#define		B102											102	 //Y54
#define		B103											103	 //Y55
#define		B104											104	 //Y56
#define		B105											105	 //Y57
#define		B106											106	 //Y58
#define		B107											107	 //Y59
#define		B108											108	 //Y60
#define		B109											109	 //Y61
#define		B110											110	 //Y62
#define		B111											111	 //Y63
#define		B112											112	 //Y64
#define		B113											113	 //Y65
#define		B114											114	 //Y66
#define		B115											115	 //Y67
#define		B116											116	 //Y68
#define		B117											117	 //Y69
#define		B118											118	 //Y70
#define		B119											119	 //Y71
#define		B120											120	 //全开
#define		B121											121	 //全关

/*IO 位定义*/
typedef enum
{
    IOBit_XIN8 = 0,
    IOBit_XIN9,//1
    IOBit_XIN10,//2
    IOBit_XIN11,//3
    IOBit_XIN12,//4
    IOBit_XIN13,//5
    IOBit_XIN14,//6
    IOBit_XIN15,//7
    IOBit_XIN16,//8
    IOBit_XIN17,//9
    IOBit_XIN18,//10
    IOBit_XIN19,//11
    IOBit_XIN20,//12
    IOBit_XIN21,//13
    IOBit_XIN22,//14
    IOBit_XIN23,//15
    IOBit_XIN24,//16
    IOBit_XIN25,//17
    IOBit_XIN26,//18
    IOBit_XIN27,//19
    IOBit_XIN28,//20
    IOBit_XIN29,//21
    IOBit_XIN30,//22
    IOBit_XIN31,//23
    IOBit_XIN32,//24
    IOBit_XIN33,//25
    IOBit_XIN34,//26
    IOBit_XIN35,//27
    IOBit_XIN36,//28
    IOBit_XIN37,//29
    IOBit_XIN38,//30
    IOBit_XIN39,//31
    IOBit_XIN40,//32
    IOBit_XIN41,//33
    IOBit_XIN42,//34
    IOBit_XIN43,//35
    IOBit_XIN44,//36
    IOBit_XIN45,//37
    IOBit_XIN46,//38
    IOBit_XIN47,//39
    IOBit_XIN48,//40
    IOBit_XIN49,//41
    IOBit_XIN50,//42
    IOBit_XIN51,//43
    //...

    IOBit_YOUT0 = 48,
    IOBit_YOUT1, IOBit_YOUT2, IOBit_YOUT3, IOBit_YOUT4, IOBit_YOUT5, IOBit_YOUT6, IOBit_YOUT7,//55
    IOBit_YOUT8, IOBit_YOUT9, IOBit_YOUT10, IOBit_YOUT11, IOBit_YOUT12, IOBit_YOUT13, IOBit_YOUT14, IOBit_YOUT15,//63
	IOBit_YOUT16, IOBit_YOUT17, IOBit_YOUT18, IOBit_YOUT19, IOBit_YOUT20, IOBit_YOUT21, IOBit_YOUT22, IOBit_YOUT23,//71
	IOBit_YOUT24, IOBit_YOUT25, IOBit_YOUT26, IOBit_YOUT27, IOBit_YOUT28, IOBit_YOUT29, IOBit_YOUT30, IOBit_YOUT31,//79
	IOBit_YOUT32, IOBit_YOUT33, IOBit_YOUT34, IOBit_YOUT35, IOBit_YOUT36, IOBit_YOUT37, IOBit_YOUT38, IOBit_YOUT39,//87
	IOBit_YOUT40, IOBit_YOUT41, IOBit_YOUT42, IOBit_YOUT43, IOBit_YOUT44, IOBit_YOUT45, IOBit_YOUT46, IOBit_YOUT47,//95
	IOBit_YOUT48, IOBit_YOUT49, IOBit_YOUT50, IOBit_YOUT51, IOBit_YOUT52, IOBit_YOUT53, IOBit_YOUT54, IOBit_YOUT55,//103
	IOBit_YOUT56, IOBit_YOUT57, IOBit_YOUT58, IOBit_YOUT59, IOBit_YOUT60, IOBit_YOUT61, IOBit_YOUT62, IOBit_YOUT63,//111
	IOBit_YOUT64, IOBit_YOUT65, IOBit_YOUT66, IOBit_YOUT67, IOBit_YOUT68, IOBit_YOUT69, IOBit_YOUT70, IOBit_YOUT71,//119
	IOBit_YOU_ALL_ON, IOBit_YOU_ALL_OFF,//121
}IO_Bit_EnumDef;

/*输入CN 位定义*/
typedef enum
{
	CN24_9 = IOBit_XIN11,
	CN24_10 = IOBit_XIN10,
	CN24_11 = IOBit_XIN9,
	CN24_12 = IOBit_XIN8,
    CN40_9 = IOBit_XIN12,
	CN40_10 = IOBit_XIN13,
	CN40_11 = IOBit_XIN14,
	CN40_12 = IOBit_XIN15,
    /*CN55
	CN55_3 = IOBit_XIN0,
    CN55_4 = IOBit_XIN1,
    CN55_5 = IOBit_XIN2,
    CN55_6 = IOBit_XIN3,
    */
    ICN56 = IOBit_XIN16,
    ICN57 = IOBit_XIN18,
    ICN58 = IOBit_XIN28,
    ICN59_3 = IOBit_XIN20,
    ICN59_4 = IOBit_XIN21,
    ICN60_3 = IOBit_XIN24,
    ICN60_4 = IOBit_XIN25,
    ICN61 = IOBit_XIN30,
    ICN62 = IOBit_XIN32,
    ICN63 = IOBit_XIN34,
    ICN64 = IOBit_XIN36,
    ICN65 = IOBit_XIN38,
    ICN66 = IOBit_XIN40,
    ICN67 = IOBit_XIN41,
    ICN68 = IOBit_XIN42,
    ICN69 = IOBit_XIN43,
    ICN70_10 = IOBit_XIN44,
    ICN70_11 = IOBit_XIN45,
    /*CN71
    CN71_3 = IOBit_XIN4,
    CN71_4 = IOBit_XIN5,
    CN71_5 = IOBit_XIN6,
    CN71_6 = IOBit_XIN7,
    */
    ICN72 = IOBit_XIN17,
    ICN73 = IOBit_XIN19,
    ICN74 = IOBit_XIN29,
    ICN75_3 = IOBit_XIN22,
    ICN75_4 = IOBit_XIN23,
    ICN76_3 = IOBit_XIN26,
    ICN76_4 = IOBit_XIN27,
    ICN77 = IOBit_XIN31,
    ICN78 = IOBit_XIN33,
    ICN79 = IOBit_XIN35,
    ICN80 = IOBit_XIN37,
    ICN81 = IOBit_XIN39,
    ICN82_10 = IOBit_XIN46,
    ICN82_11 = IOBit_XIN47,
    ICN83_10 = IOBit_XIN48,
    ICN83_11 = IOBit_XIN49,
    ICN84_10 = IOBit_XIN50,
    ICN84_11 = IOBit_XIN51,
    ICN_Disable = 127,
}IO_iCNxBit_EnumDef;

/*输出CN 位定义*/
typedef enum
{
	OCN24_3 = IOBit_YOUT27,
	OCN24_4 = IOBit_YOUT26,
	OCN24_5 = IOBit_YOUT25,
	OCN24_6 = IOBit_YOUT24,
	OCN24_7 = IOBit_YOUT28,
    OCN25 = IOBit_YOUT44,
    OCN26 = IOBit_YOUT36,
    OCN27 = IOBit_YOUT38,
    OCN28 = IOBit_YOUT40,
    OCN29 = IOBit_YOUT65,
    OCN30_3 = IOBit_YOUT68,
    OCN30_4 = IOBit_YOUT69,
    OCN30_5 = IOBit_YOUT70,
    OCN30_6 = IOBit_YOUT71,
    OCN31_2 = IOBit_YOUT34,
    OCN31_3 = IOBit_YOUT35,
    OCN31_4 = IOBit_YOUT63,
    OCN32 = IOBit_YOUT42,
    OCN33 = IOBit_YOUT46,
    OCN34 = IOBit_YOUT48,
    OCN35 = IOBit_YOUT50,
    OCN36 = IOBit_YOUT52,
    OCN37 = IOBit_YOUT54,
    OCN38 = IOBit_YOUT56,
    OCN87 = IOBit_YOUT58,
    OCN40_3 = IOBit_YOUT33,
    OCN40_4 = IOBit_YOUT32,
    OCN40_5 = IOBit_YOUT31,
    OCN40_6 = IOBit_YOUT30,
    OCN40_7 = IOBit_YOUT29,
    OCN41 = IOBit_YOUT45,
    OCN42 = IOBit_YOUT37,
    OCN43 = IOBit_YOUT39,
    OCN44 = IOBit_YOUT41,
    OCN45 = IOBit_YOUT64,
    OCN46_2 = IOBit_YOUT60,
    OCN46_3 = IOBit_YOUT61,
    OCN46_4 = IOBit_YOUT62,
    OCN47 = IOBit_YOUT43,
    OCN48 = IOBit_YOUT47,
    OCN49 = IOBit_YOUT49,
    OCN50 = IOBit_YOUT51,
    OCN51 = IOBit_YOUT53,
    OCN52 = IOBit_YOUT55,
    OCN53 = IOBit_YOUT57,
    OCN70_3 = IOBit_YOUT0,
	OCN70_5 = IOBit_YOUT4,
	OCN70_6 = IOBit_YOUT5,
	OCN70_7 = IOBit_YOUT6,
	OCN70_8 = IOBit_YOUT7,
	OCN70_9 = IOBit_YOUT8,
    OCN82_3 = IOBit_YOUT1,
    OCN82_5 = IOBit_YOUT9,
    OCN82_6 = IOBit_YOUT10,
    OCN82_7 = IOBit_YOUT11,
    OCN82_8 = IOBit_YOUT12,
    OCN82_9 = IOBit_YOUT13,
    OCN83_3 = IOBit_YOUT2,
    OCN83_5 = IOBit_YOUT14,
    OCN83_6 = IOBit_YOUT15,
    OCN83_7 = IOBit_YOUT16,
    OCN83_8 = IOBit_YOUT17,
    OCN83_9 = IOBit_YOUT18,
	OCN84_3 = IOBit_YOUT3,
	OCN84_5 = IOBit_YOUT19,
	OCN84_6 = IOBit_YOUT20,
	OCN84_7 = IOBit_YOUT21,
	OCN84_8 = IOBit_YOUT22,
	OCN84_9 = IOBit_YOUT23,
    OCN89 = IOBit_YOUT59,
    OCN88 = IOBit_YOUT67,
    OCN90 = IOBit_YOUT66,
    OCN_Disable = 127,
}IO_oCNxBit_EnumDef;

/*IO共用体 位定义*/
typedef struct
{
    unsigned short IOIN8 : 1;//InCN 0
    unsigned short IOIN9 : 1;//1
    unsigned short IOIN10 : 1;//2
    unsigned short IOIN11 : 1;//3
    unsigned short IOIN12 : 1;//4
    unsigned short IOIN13 : 1;//5
    unsigned short IOIN14 : 1;//6
    unsigned short IOIN15 : 1;//7
    unsigned short IOIN16 : 1;//InCN56 8
    unsigned short IOIN17 : 1;//InCN72 9
    unsigned short IOIN18 : 1;//InCN57 10
    unsigned short IOIN19 : 1;//InCN73 11
    unsigned short IOIN20 : 1;//InCN59_3 12
    unsigned short IOIN21 : 1;//InCN59_4 13
    unsigned short IOIN22 : 1;//InCN75_3 14
    unsigned short IOIN23 : 1;//InCN75_4 15
    unsigned short IOIN24 : 1;//InCN60_3 16
    unsigned short IOIN25 : 1;//InCN60_4 17
    unsigned short IOIN26 : 1;//InCN76_3 18
    unsigned short IOIN27 : 1;//InCN76_4 19
    unsigned short IOIN28 : 1;//InCN58 20
    unsigned short IOIN29 : 1;//InCN74 21
    unsigned short IOIN30 : 1;//InCN61 22
    unsigned short IOIN31 : 1;//InCN77 23
    unsigned short IOIN32 : 1;//InCN62 24
    unsigned short IOIN33 : 1;//InCN78 25
    unsigned short IOIN34 : 1;//InCN63 26
    unsigned short IOIN35 : 1;//InCN79 27
    unsigned short IOIN36 : 1;//InCN64 28
    unsigned short IOIN37 : 1;//InCN80 29
    unsigned short IOIN38 : 1;//InCN65 30
    unsigned short IOIN39 : 1;//InCN81 31
    unsigned short IOIN40 : 1;//InCN66 32
    unsigned short IOIN41 : 1;//InCN67 33
    unsigned short IOIN42 : 1;//InCN68 34
    unsigned short IOIN43 : 1;//InCN69 35
    unsigned short IOIN44 : 1;//InCN70_10 36
    unsigned short IOIN45 : 1;//InCN70_11 37
    unsigned short IOIN46 : 1;//InCN82_10 38
    unsigned short IOIN47 : 1;//InCN82_11 39
    unsigned short IOIN48 : 1;//InCN83_10 40
    unsigned short IOIN49 : 1;//InCN83_11 41
    unsigned short IOIN50 : 1;//InCN84_10 42
    unsigned short IOIN51 : 1;//InCN84_11 bit43

    unsigned short IONUll : 4;//空白占位

    unsigned short IOOUT0 : 1;//OutCN bit48
    unsigned short IOOUT1 : 1;//49
    unsigned short IOOUT2 : 1;//50
    unsigned short IOOUT3 : 1;//51
    unsigned short IOOUT4 : 1;//52
    unsigned short IOOUT5 : 1;//53
    unsigned short IOOUT6 : 1;//54
    unsigned short IOOUT7 : 1;//55
    unsigned short IOOUT8 : 1;//56
    unsigned short IOOUT9 : 1;//57
    unsigned short IOOUT10 : 1;//58
    unsigned short IOOUT11 : 1;//59
    unsigned short IOOUT12 : 1;//60
    unsigned short IOOUT13 : 1;//61
    unsigned short IOOUT14 : 1;//62
    unsigned short IOOUT15 : 1;//63
    unsigned short IOOUT16 : 1;
    unsigned short IOOUT17 : 1;
    unsigned short IOOUT18 : 1;
    unsigned short IOOUT19 : 1;
    unsigned short IOOUT20 : 1;
    unsigned short IOOUT21 : 1;
    unsigned short IOOUT22 : 1;
    unsigned short IOOUT23 : 1;
    unsigned short IOOUT24 : 1;
    unsigned short IOOUT25 : 1;
    unsigned short IOOUT26 : 1;
    unsigned short IOOUT27 : 1;
    unsigned short IOOUT28 : 1;
    unsigned short IOOUT29 : 1;
    unsigned short IOOUT30 : 1;
    unsigned short IOOUT31 : 1;
    unsigned short IOOUT32 : 1;
    unsigned short IOOUT33 : 1;//OutCN40_3
    unsigned short IOOUT34 : 1;//OutCN31_3
    unsigned short IOOUT35 : 1;//OutCN31_2
    unsigned short IOOUT36 : 1;//OutCN26
    unsigned short IOOUT37 : 1;//OutCN42
    unsigned short IOOUT38 : 1;//OutCN27
    unsigned short IOOUT39 : 1;//OutCN43
    unsigned short IOOUT40 : 1;//OutCN28
    unsigned short IOOUT41 : 1;//OutCN44
    unsigned short IOOUT42 : 1;//OutCN32
    unsigned short IOOUT43 : 1;//OutCN47
    unsigned short IOOUT44 : 1;//OutCN25
    unsigned short IOOUT45 : 1;//OutCN41
    unsigned short IOOUT46 : 1;//OutCN33
    unsigned short IOOUT47 : 1;//OutCN48
    unsigned short IOOUT48 : 1;//OutCN34
    unsigned short IOOUT49 : 1;//OutCN49
    unsigned short IOOUT50 : 1;//OutCN35
    unsigned short IOOUT51 : 1;//OutCN50
    unsigned short IOOUT52 : 1;//OutCN36
    unsigned short IOOUT53 : 1;//OutCN51
    unsigned short IOOUT54 : 1;//OutCN37-----
    unsigned short IOOUT55 : 1;//OutCN52
    unsigned short IOOUT56 : 1;//OutCN30
    unsigned short IOOUT57 : 1;//OutCN53
    unsigned short IOOUT58 : 1;//OutCN31
    unsigned short IOOUT59 : 1;//OutCN54
    unsigned short IOOUT60 : 1;//OutCN46_2
    unsigned short IOOUT61 : 1;//OutCN46_3
    unsigned short IOOUT62 : 1;//OutCN46_4
    unsigned short IOOUT63 : 1;//OutCN31_4
    unsigned short IOOUT64 : 1;//OutCN45
    unsigned short IOOUT65 : 1;//OutCN29
    unsigned short IOOUT66 : 1;//OutCN90
    unsigned short IOOUT67 : 1;//OutCN88-----
    unsigned short IOOUT68 : 1;//OutCN30_3
    unsigned short IOOUT69 : 1;//OutCN30_4
    unsigned short IOOUT70 : 1;//OutCN30_5
    unsigned short IOOUT71 : 1;//OutCN30_6
    unsigned short IOOU_ALL_ON : 1;
    unsigned short IOOU_ALL_OFF : 1;
}IO_IO2DHRBit_StDef;

/*IO共用体 CN位定义*/
typedef struct
{
    unsigned short IOIN8_CN24_12 : 1;//InCN
    unsigned short IOIN9_CN24_11 : 1;
    unsigned short IOIN10_CN24_10 : 1;
    unsigned short IOIN11_CN24_9 : 1;
    unsigned short IOIN12_CN40_9 : 1;
    unsigned short IOIN13_CN40_10 : 1;
    unsigned short IOIN14_CN40_11 : 1;
    unsigned short IOIN15_CN40_12 : 1;
    unsigned short InCN56 : 1;
    unsigned short InCN72 : 1;
    unsigned short InCN57 : 1;
    unsigned short InCN73 : 1;
    unsigned short InCN59_3 : 1;
    unsigned short InCN59_4 : 1;
    unsigned short InCN75_3 : 1;
    unsigned short InCN75_4 : 1;
    unsigned short InCN60_3 : 1;
    unsigned short InCN60_4 : 1;
    unsigned short InCN76_3 : 1;
    unsigned short InCN76_4 : 1;
    unsigned short InCN58 : 1;
    unsigned short InCN74 : 1;
    unsigned short InCN61 : 1;
    unsigned short InCN77 : 1;
    unsigned short InCN62 : 1;
    unsigned short InCN78 : 1;
    unsigned short InCN63 : 1;
    unsigned short InCN79 : 1;
    unsigned short InCN64 : 1;
    unsigned short InCN80 : 1;
    unsigned short InCN65 : 1;
    unsigned short InCN81 : 1;
    unsigned short InCN66 : 1;
    unsigned short InCN67 : 1;
    unsigned short InCN68 : 1;
    unsigned short InCN69 : 1;
    unsigned short InCN70_10 : 1;
    unsigned short InCN70_11 : 1;
    unsigned short InCN82_10 : 1;
    unsigned short InCN82_11 : 1;
    unsigned short InCN83_10 : 1;
    unsigned short InCN83_11 : 1;
    unsigned short InCN84_10 : 1;
    unsigned short InCN84_11 : 1;//bit43

    unsigned short IONUll : 4;//空白占位

    unsigned short IOOUT0_CN70_3 : 1;//OutCN //bit48
    unsigned short IOOUT1_CN82_3 : 1;
    unsigned short IOOUT2_CN83_3 : 1;
    unsigned short IOOUT3_CN84_3 : 1;
    unsigned short IOOUT4_CN70_5 : 1;
    unsigned short IOOUT5_CN70_6 : 1;
    unsigned short IOOUT6_CN70_7 : 1;
    unsigned short IOOUT7_CN70_8 : 1;
    unsigned short IOOUT8_CN70_9 : 1;
    unsigned short IOOUT9_CN82_5 : 1;
    unsigned short IOOUT10_CN82_6 : 1;
    unsigned short IOOUT11_CN82_7 : 1;
    unsigned short IOOUT12_CN82_8 : 1;
    unsigned short IOOUT13_CN82_9 : 1;
    unsigned short IOOUT14_CN83_5 : 1;
    unsigned short IOOUT15_CN83_6 : 1;
    unsigned short IOOUT16_CN83_7 : 1;
    unsigned short IOOUT17_CN83_8 : 1;
    unsigned short IOOUT18_CN83_9 : 1;
    unsigned short IOOUT19_CN84_5 : 1;
    unsigned short IOOUT20_CN84_6 : 1;
    unsigned short IOOUT21_CN84_7 : 1;
    unsigned short IOOUT22_CN84_8 : 1;
    unsigned short IOOUT23_CN84_9 : 1;
    unsigned short IOOUT24_CN24_6 : 1;
    unsigned short IOOUT25_CN24_5 : 1;
    unsigned short IOOUT26_CN24_4 : 1;
    unsigned short IOOUT27_CN24_3 : 1;
    unsigned short IOOUT28_CN24_7 : 1;
    unsigned short IOOUT29_CN40_7 : 1;
    unsigned short IOOUT30_CN40_6 : 1;
    unsigned short IOOUT31_CN40_5 : 1;
    unsigned short IOOUT32_CN40_4 : 1;
    unsigned short OutCN40_3 : 1;
    unsigned short OutCN31_3 : 1;
    unsigned short OutCN31_2 : 1;
    unsigned short OutCN26 : 1;
    unsigned short OutCN42 : 1;
    unsigned short OutCN27 : 1;
    unsigned short OutCN43 : 1;
    unsigned short OutCN28 : 1;
    unsigned short OutCN44 : 1;
    unsigned short OutCN32 : 1;
    unsigned short OutCN47 : 1;
    unsigned short OutCN25 : 1;
    unsigned short OutCN41 : 1;
    unsigned short OutCN33 : 1;
    unsigned short OutCN48 : 1;
    unsigned short OutCN34 : 1;//Y48
    unsigned short OutCN49 : 1;
    unsigned short OutCN35 : 1;//Y50
    unsigned short OutCN50 : 1;
    unsigned short OutCN36 : 1;
    unsigned short OutCN51 : 1;
    unsigned short OutCN37 : 1;//Y54
    unsigned short OutCN52 : 1;
    unsigned short OutCN30 : 1;
    unsigned short OutCN53 : 1;
    unsigned short OutCN31 : 1;
    unsigned short OutCN54 : 1;
    unsigned short OutCN46_2 : 1;
    unsigned short OutCN46_3 : 1;
    unsigned short OutCN46_4 : 1;
    unsigned short OutCN31_4 : 1;
    unsigned short OutCN45 : 1;
    unsigned short OutCN29 : 1;
    unsigned short OutCN90 : 1;
    unsigned short OutCN88 : 1;//Y67
    unsigned short OutCN30_3 : 1;
    unsigned short OutCN30_4 : 1;
    unsigned short OutCN30_5 : 1;
    unsigned short OutCN30_6 : 1;
    unsigned short IOOU_ALL_ON : 1;
    unsigned short IOOU_ALL_OFF : 1;
}IO_CN2DHRBit_StDef;

#endif

/************************************END OF FILE************************************/
