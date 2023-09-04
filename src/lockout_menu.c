/**
 * Filename: menu.c
 * Description: whatup dawg menusssss poggers!
*/

#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>
#include <ti/getcsc.h>
#include <debug.h>
#include "lockout_menu.h"

static void draw_button(char* text, bool filled, uint24_t x, uint24_t y) {
    int height = 20;
    int width = 60;

    int text_length = strlen(text);

    int text_spacing = 8;
    int text_height = 8;
    int text_x = x + width / 2 - text_spacing * text_length / 2;
    int text_y = y + height / 2 - text_height / 2;

    if (filled) {
        gfx_SetColor(3);
        gfx_FillRectangle(x, y, width, height);
        gfx_SetTextFGColor(1);
        gfx_PrintStringXY(text, text_x, text_y);
    } else {
        gfx_SetTextFGColor(3);
        gfx_PrintStringXY(text, text_x, text_y);
    }
}

menu_result_t lockout_menu_yes_no(uint24_t width, uint24_t height, char **message, size_t message_len) {
    // we're gonna directly overwrite the screen
    gfx_SetDrawScreen();
    gfx_SetMonospaceFont(8);

    uint24_t xpos = (GFX_LCD_WIDTH - width) / 2;
    uint24_t ypos = (GFX_LCD_HEIGHT - height) / 2;

    int middle_y = ypos + (height - 80) / 2;
    int text_y = middle_y - 4;

    int current_choice = 0;
    bool went_left;
    bool went_right;

    do {
        kb_Scan();
        if (kb_Data[7] & kb_Left) {
            if (!went_left) {
                current_choice -= 1;
                if (current_choice < 0)
                    current_choice = 2;
                
                dbg_printf("went left\n");
            }
            went_left = true;
        } else {
            went_left = false;
        }

        if (kb_Data[7] & kb_Right) {
            if (!went_right) {
                current_choice += 1;
                if (current_choice > 2)
                    current_choice = 0;
                dbg_printf("went right\n");
            }
            went_right = true;
        } else {
            went_right = false;
        }

        dbg_printf("current_choice: %i\n", current_choice);

        // draw menu background
        gfx_SetColor(1);
        gfx_FillRectangle(xpos, ypos, width, height);
        gfx_SetColor(3);
        gfx_Rectangle(xpos, ypos, width, height);

        // draw text
        gfx_SetTextFGColor(3);
        for (size_t i = 0; i < message_len; i++) {
            int text_length = strlen(message[i]);
            int middle_x = xpos + width / 2;
            int text_x = middle_x - (text_length * 8 / 2);

            gfx_PrintStringXY(message[i], text_x, text_y + 10 * i);
        }

        // draw buttons
        int half_button = 30;
        int third_width = width / 4;
        int button_y = ypos + height - 20 - 20;
        int button_1_x = third_width - half_button - 15;
        int button_2_x = third_width * 2 - half_button;
        int button_3_x = third_width * 3 - half_button + 15;

        draw_button("Yes", 0 == current_choice, xpos + button_1_x, button_y);
        draw_button("No", 1 == current_choice, xpos + button_2_x, button_y);
        draw_button("Cancel", 2 == current_choice, xpos + button_3_x, button_y);

        if (!os_GetCSC()) {
            went_left = false;
            went_right = false;
        }

        while (!os_GetCSC());
    } while (!(kb_Data[6] & kb_Enter));

    // set it back to the buffer when we're done
    gfx_SetMonospaceFont(0);
    gfx_SetDrawBuffer();

    switch (current_choice) {
        case 0: return MENU_RESULT_YES;
        case 1: return MENU_RESULT_NO;
        case 2: return MENU_RESULT_CANCEL;
    }

    return MENU_RESULT_NO;
}
