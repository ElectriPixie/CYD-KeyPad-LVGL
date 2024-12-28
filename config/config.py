Import("env")
import os
import shutil

board_name = env.get("BOARD")
lv_conf = env.get("PROJECT_INCLUDE_DIR")+"/lv_conf.h"
lv_conf_link = env.get("PROJECT_DIR")+"/.pio/libdeps/"+board_name+"/lvgl/lv_conf.h"
if not os.path.islink(lv_conf_link) and not os.path.isfile(lv_conf_link):
    shutil.copy(lv_conf, lv_conf_link)

User_Setup = env.get("PROJECT_INCLUDE_DIR")+"/User_Setup.h"
User_Setup_Link = env.get("PROJECT_DIR")+"/.pio/libdeps/"+board_name+"/TFT_eSPI/User_Setup.h"
if not os.path.islink(User_Setup_Link) and not os.path.isfile(User_Setup_Link):
    os.symlink(User_Setup, User_Setup_Link)