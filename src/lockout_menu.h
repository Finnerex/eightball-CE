/**
 * Filename: menu.h
 * Description: whatup dawg menusssss poggers!
*/

#ifndef LOCKOUT_MENU_H
#define LOCKOUT_MENU_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MENU_RESULT_CANCEL = 0,
    MENU_RESULT_YES = 1,
    MENU_RESULT_NO = 2
} menu_result_t;

menu_result_t lockout_menu_yes_no(uint24_t width, uint24_t height, char **message, size_t message_len);

#ifdef __cplusplus
}
#endif

#endif