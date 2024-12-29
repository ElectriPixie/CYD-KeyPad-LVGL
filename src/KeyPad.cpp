#include <Arduino.h>
#include <TFT_eSPI.h>
#include <esp32_smartdisplay.h>
#include <lvgl.h>
#include <widgets/label/lv_label.h>
#include <widgets/button/lv_button.h>
#include <string>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

char* numStr(int number) {
    std::string strNumber = std::to_string(number);
    char* ret = new char[strNumber.length() + 1];
    strcpy(ret, strNumber.c_str());
   return ret;
}

//static lv_obj_t *label;
void lv_button(lv_obj_t *button, int x, int y, int w, int h, const char * label_txt);

struct Button {
    int x, y;       
    int w, h; 
    int number;
    int color;
    lv_obj_t *button;
};

struct KeyPad {
  int x, y;
  int w, h;
  int offset;
  struct Button Buttons[9];
};

struct KeyPad KeyPad;

void drawButton(struct KeyPad* KeyPad, int i, int color)
{
  int x, y;
  int w, h;
  x = KeyPad->Buttons[i].x;
  y = KeyPad->Buttons[i].y;
  w = KeyPad->Buttons[i].w;
  h = KeyPad->Buttons[i].h;
  const char * number = numStr(KeyPad->Buttons[i].number);
  if(color == 0)
  {
    color = KeyPad->Buttons[i].color;
  }
  lv_button(KeyPad->Buttons->button, x, y, w, h, number);
}

void drawKeyPad(struct KeyPad* KeyPad)
{
  for(int i = 0; i < 9; i++)
  {
    drawButton(KeyPad, i, 0);
  }
}

void addButton(struct KeyPad* KeyPad, int i, int x, int y, int w, int h, int color)
{
  KeyPad->Buttons[i].x = x;
  KeyPad->Buttons[i].y = y;
  KeyPad->Buttons[i].w = w;
  KeyPad->Buttons[i].h = h;
  KeyPad->Buttons[i].number = i+1;
  KeyPad->Buttons[i].color = color;
}

void initKeyPad(struct KeyPad* KeyPad, int offset, int w, int h, int color)
{
  int x = 0;
  int y = 0;
  int xw;
  int yh;
  xw = (w - offset*2 )/ 3; 
  yh = (h - offset*2 )/ 3; 
  KeyPad->w = w;
  KeyPad->h = h;
  KeyPad->offset = offset;

  for(int i = 0; i < 3; i++)
  {
    addButton(KeyPad, i, x, y, xw, yh, color);
    x = x+xw+KeyPad->offset;
  }
  x = 0;
  y = y+yh+KeyPad->offset;
  for(int i = 3; i < 6; i++)
  {
    addButton(KeyPad, i, x, y, xw, yh, color);
    x = x+xw+KeyPad->offset;
  }
  x = 0;  
  y = y+yh+KeyPad->offset;
  for(int i = 6; i < 9; i++)
  {
    addButton(KeyPad, i, x, y, xw, yh, color);
    x = x+xw+KeyPad->offset;
  }
}

//int checkKeyPad(struct KeyPad* KeyPad, int x, int y)
//{
//  for(int i = 0; i < 9; i++)
//  {
//    if(x > KeyPad->Buttons[i].x && x < KeyPad->Buttons[i].x + KeyPad->Buttons[i].w)
//    {
//      if(y > KeyPad->Buttons[i].y && y < KeyPad->Buttons[i].y + KeyPad->Buttons[i].h)
//      {
//        return i;
//      }
//    }
//  }
//  return -1;
//}

void lv_button(lv_obj_t *button, int x, int y, int w, int h, const char * label_txt)
{
    /*Init the style for the default state*/
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_radius(&style, 3);

    lv_style_set_bg_opa(&style, LV_OPA_100);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_color(&style, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);

    lv_style_set_border_opa(&style, LV_OPA_40);
    lv_style_set_border_width(&style, 2);
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_GREY));

    lv_style_set_shadow_width(&style, 8);
    lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_offset_y(&style, 8);

    lv_style_set_outline_opa(&style, LV_OPA_COVER);
    lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_BLUE));

    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_pad_all(&style, 10);

    /*Init the pressed style*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);

    lv_style_set_translate_y(&style_pr, 5);
    lv_style_set_shadow_offset_y(&style_pr, 3);
    lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 4));

    /*Add a transition to the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);

    lv_style_set_transition(&style_pr, &trans);

    button = lv_button_create(lv_scr_act());
    lv_obj_remove_style_all(button);                          /*Remove the style coming from the theme*/
    lv_obj_add_style(button, &style, 0);
    lv_obj_add_style(button, &style_pr, LV_STATE_PRESSED);
    lv_obj_set_size(button, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    //lv_obj_center(button);

    lv_obj_t * label = lv_label_create(button);
    lv_label_set_text(label, label_txt);
    lv_obj_center(label);
    lv_obj_set_x(button, x);
    lv_obj_set_y(button, y);
    lv_obj_set_width(button, w);
    lv_obj_set_height(button, h);
}

void setup()
{
  smartdisplay_init();
  auto display = lv_display_get_default();
  lv_display_set_rotation(display, LV_DISPLAY_ROTATION_270);
  initKeyPad(&KeyPad, 5, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_GREEN);
  drawKeyPad(&KeyPad);
}

auto lv_last_tick = millis();

void loop()
{
    auto const now = millis();
    // Update the ticker
    lv_tick_inc(now - lv_last_tick);
    lv_last_tick = now;
    // Update the UI
    lv_timer_handler();
}