#include <linux/string.h>
#ifndef BUILD_UBOOT
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (540)
#define FRAME_HEIGHT (960)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))
#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0x00   // END OF REGISTERS MARKER


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)									lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)				lcm_util.dsi_write_regs(addr, pdata, byte_nums)
//#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

       

#define LCM_DSI_CMD_MODE

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/

	
	{0xff,4,{0xAA,0x55,0x25,0x01}},
	{0xf3,4,{0x02,0x03,0x07,0x15}},
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x00}},
	{0xB8,4,{0x01,0x02,0x02,0x02}},
	{0xBC,3,{0x05,0x05,0x05}},
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x01}},
	{0xB0,3,{0x05,0x05,0x05}},
	{0xB1,3,{0x05,0x05,0x05}},
	{0xB6,3,{0x44,0x44,0x44}},
	{0xB7,3,{0x34,0x34,0x34}},
	{0xB3,3,{0x10,0x10,0x10}},
	{0xB9,3,{0x34,0x34,0x34}},
	{0xB4,3,{0x0A,0x0A,0x0A}},
	{0xBA,3,{0x14,0x14,0x14}},
	{0xBC,3,{0x00,0xA0,0x00}},
	{0xBD,3,{0x00,0xA0,0x00}},
	{0xBE,1,{0x4E}},
	 {0xD1,16,{0x00,0x32,0x00,0x41,0x00,0x54,0x00,0x67,0x00,0x7A,0x00,0x98,0x00,0xB0,0x00,0xDB}} ,
	  {0xD2,16,{0x01,0x01,0x01,0x3F,0x01,0x70,0x01,0xB4,0x01,0xEC,0x01,0xED,0x02,0x1E,0x02,0x51}},
	   {0xD3,16,{0x02,0x6C,0x02,0x8D,0x02,0xA5,0x02,0xC9,0x02,0xEA,0x03,0x19,0x03,0x45,0x03,0x7A}},
	    {0xD4,4,{0x03,0xB0,0x03,0xF4}}  ,
	    {0xD5,16,{0x00,0x32,0x00,0x41,0x00,0x54,0x00,0x67,0x00,0x7A,0x00,0x98,0x00,0xB0,0x00,0xDB}} ,
	    {0xD6,16,{0x01,0x01,0x01,0x3F,0x01,0x70,0x01,0xB4,0x01,0xEC,0x01,0xED,0x02,0x1E,0x02,0x51}},
	    {0xD7,16,{0x02,0x6C,0x02,0x8D,0x02,0xA5,0x02,0xC9,0x02,0xEA,0x03,0x19,0x03,0x45,0x03,0x7A}} ,
	    {0xD8,4,{0x03,0xB0,0x03,0xF4}} ,
	     {0xD9,16,{0x00,0x32,0x00,0x41,0x00,0x54,0x00,0x67,0x00,0x7A,0x00,0x98,0x00,0xB0,0x00,0xDB}},
	      {0xDD,16,{0x01,0x01,0x01,0x3F,0x01,0x70,0x01,0xB4,0x01,0xEC,0x01,0xED,0x02,0x1E,0x02,0x51}},
	       {0xDE,16,{0x02,0x6C,0x02,0x8D,0x02,0xA5,0x02,0xC9,0x02,0xEA,0x03,0x19,0x03,0x45,0x03,0x7A}} ,       
	{0xDF,4,{0x03,0xB0,0x03,0xF4}} ,
	{0xE0,16,{0x00,0x32,0x00,0x41,0x00,0x54,0x00,0x67,0x00,0x7A,0x00,0x98,0x00,0xB0,0x00,0xDB}},
	{0xE1,16,{0x01,0x01,0x01,0x3F,0x01,0x70,0x01,0xB4,0x01,0xEC,0x01,0xED,0x02,0x1E,0x02,0x51}} ,
	{0xE2,16,{0x02,0x6C,0x02,0x8D,0x02,0xA5,0x02,0xC9,0x02,0xEA,0x03,0x19,0x03,0x45,0x03,0x7A}} ,
	
	{0xE3,4,{0x03,0xB0,0x03,0xF4}}	,
	 {0xE4,16,{0x00,0x32,0x00,0x41,0x00,0x54,0x00,0x67,0x00,0x7A,0x00,0x98,0x00,0xB0,0x00,0xDB}},
	  {0xE5,16,{0x01,0x01,0x01,0x3F,0x01,0x70,0x01,0xB4,0x01,0xEC,0x01,0xED,0x02,0x1E,0x02,0x51}},
	  {0xEA,16,{0x02,0x6C,0x02,0x8D,0x02,0xA5,0x02,0xC9,0x02,0xEA,0x03,0x19,0x03,0x45,0x03,0x7A}}  ,
	  {0xEB,4,{0x03,0xB0,0x03,0xF4}} ,
	 
	  {0x3A,1,{0x07}},
	  {0x35,1,{0x00}},
		{0x55,1,{2}},
	  {0x4C,1,{0x11}},

	// Note
	// Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.


	// Setting ending by predefined flag
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}},

    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 200, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
static struct LCM_setting_table lcm_compare_id_setting[] = {
	// Display off sequence
	{0xF0,	5,	{0x55, 0xaa, 0x52,0x08,0x00}},
	{REGFLAG_DELAY, 10, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
				MDELAY(2);
       	}
    }
	
}


static void init_lcm_registers(void)
{
	unsigned int data_array[16];
}
// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if defined(LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
	
		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573

			params->dsi.DSI_WMEM_CONTI=0x3C; 
			params->dsi.DSI_RMEM_CONTI=0x3E; 

		
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 2;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				= 3;
		params->dsi.vertical_backporch					= 12;
		params->dsi.vertical_frontporch					= 2;
		params->dsi.vertical_active_line				= FRAME_HEIGHT;

		params->dsi.horizontal_sync_active				= 10;
		params->dsi.horizontal_backporch				= 50;
		params->dsi.horizontal_frontporch				= 50;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		
		params->dsi.LPX =12;

		// Bit rate calculation
		params->dsi.pll_div1=38;		// fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)
		params->dsi.pll_div2=1;			// div2=0~15: fout=fvo/(2*div2)

}


static void lcm_init(void)
{

	SET_RESET_PIN(0);
    MDELAY(200);
    SET_RESET_PIN(1);
    MDELAY(50);

    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	
}


static void lcm_suspend(void)
{
		//push_table(lcm_sleep_mode_in_setting, sizeof(lcm_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
		//SET_RESET_PIN(0);
		//MDELAY(1);
		//SET_RESET_PIN(1);
			unsigned int data_array[2];
#if 1
		//data_array[0] = 0x00000504; // Display Off
		//dsi_set_cmdq(&data_array, 1, 1);
		//MDELAY(100); 
		data_array[0] = 0x00280500; // Display Off
		dsi_set_cmdq(&data_array, 1, 1);
		MDELAY(10); 
		data_array[0] = 0x00100500; // Sleep In
		dsi_set_cmdq(&data_array, 1, 1);
		MDELAY(100);
#endif
#ifdef BUILD_UBOOT
		printf("zhibin uboot %s\n", __func__);
#else
		printk("zhibin kernel %s\n", __func__);
#endif

}


static void lcm_resume(void)
{
		//lcm_init();	
#ifdef BUILD_UBOOT
		printf("zhibin uboot %s\n", __func__);
#else
		printk("zhibin kernel %s\n", __func__);
	
#endif
		push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);

}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];
	
#ifdef BUILD_UBOOT
		printf("zhibin uboot %s\n", __func__);
#else
		printk("zhibin kernel %s\n", __func__);	
#endif

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(&data_array, 7, 0);

}
static unsigned int lcm_compare_id(void)
{

		int   array[4];
		char  buffer[3];
		char  id0=0;
		char  id1=0;
		char  id2=0;


		SET_RESET_PIN(0);
		MDELAY(200);
		SET_RESET_PIN(1);
		MDELAY(200);
		
	array[0] = 0x00083700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x04,buffer, 3);
	
	id0 = buffer[0]; //should be 0x00
	id1 = buffer[1];//should be 0x80
	id2 = buffer[2];//should be 0x00
	#ifdef BUILD_UBOOT
		printf("zhibin uboot %s\n", __func__);
		printf("%s, id0 = 0x%08x\n", __func__, id0);//should be 0x00
		printf("%s, id1 = 0x%08x\n", __func__, id1);//should be 0x80
		printf("%s, id2 = 0x%08x\n", __func__, id2);//should be 0x00
	#else
		//printk("zhibin kernel %s\n", __func__);	
	#endif
	
	return 0;


}


LCM_DRIVER nt35516_qhd_dsi_cmd_tft9k1342_drv = 
{
    .name			= "nt35516_dsi_tf",
	.set_util_funcs = lcm_set_util_funcs,
	.compare_id     = lcm_compare_id,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
#if defined(LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };
