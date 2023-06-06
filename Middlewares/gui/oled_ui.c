#include "oled_ui.h"
#include "u8g2.h"
#include "delay.h"
#include "i2c.h"

uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg,
                                  U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    //Function which implements a delay, arg_int contains the amount of ms
    case U8X8_MSG_DELAY_MILLI:
        delay_ms(arg_int);
        break;
    //Function which delays 10us
    case U8X8_MSG_DELAY_10MICRO:
        delay_us(10);
        break;
    //Function which delays 100ns
    case U8X8_MSG_DELAY_100NANO:
        __NOP();
        break;
    case U8X8_MSG_GPIO_I2C_CLOCK:
        if (arg_int) I2C_OLED_SCL_Level(True);
        else I2C_OLED_SCL_Level(False);
        break;
    case U8X8_MSG_GPIO_I2C_DATA:
        if (arg_int) I2C_OLED_SDA_Level(True);
        else I2C_OLED_SDA_Level(False);
        break;
    default:
        return 0; //A message was received which is not implemented, return 0 to indicate an error
    }
    return 1; // command processed successfully.
}

void ui_draw_image(void)
{
	u8g2_t u8g2;
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8g2_gpio_and_delay_stm32);
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    delay_ms(1000);
    u8g2_ClearBuffer(&u8g2);
//    u8g2_DrawLine(&u8g2, 0, 0, 127, 63);
//    u8g2_DrawLine(&u8g2, 0, 63, 127, 0);
	u8g2_DrawCircle(&u8g2, 64, 32, 16, U8G2_DRAW_UPPER_RIGHT);
	u8g2_DrawCircle(&u8g2, 64, 32, 16, U8G2_DRAW_UPPER_LEFT);
	u8g2_DrawCircle(&u8g2, 64, 32, 16, U8G2_DRAW_LOWER_RIGHT);
	u8g2_DrawCircle(&u8g2, 64, 32, 16, U8G2_DRAW_LOWER_LEFT);
    u8g2_SendBuffer(&u8g2);
    u8g2_ClearBuffer(&u8g2);
}

