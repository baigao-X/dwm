#include <X11/XF86keysym.h>

static int showsystray                   = 1;         /* 是否显示托盘栏 */
static const int newclientathead         = 0;         /* 定义新窗口在栈顶还是栈底 */
static const unsigned int borderpx       = 2;         /* 窗口边框大小 */
static const unsigned int systraypinning = 1;         /* 托盘跟随的显示器 0代表不指定显示器 */
static const unsigned int systrayspacing = 1;         /* 托盘间距 */
static const unsigned int systrayspadding = 5;        /* 托盘和状态栏的间隙 */
static int gappi                         = 12;        /* 窗口与窗口 缝隙大小 */
static int gappo                         = 12;        /* 窗口与边缘 缝隙大小 */
static const int _gappo                  = 12;        /* 窗口与窗口 缝隙大小 不可变 用于恢复时的默认值 */
static const int _gappi                  = 12;        /* 窗口与边缘 缝隙大小 不可变 用于恢复时的默认值 */
static const int vertpad                 = 5;         /* vertical padding of bar */
static const int sidepad                 = 5;         /* horizontal padding of bar */
static const int overviewgappi           = 24;        /* overview时 窗口与边缘 缝隙大小 */
static const int overviewgappo           = 60;        /* overview时 窗口与窗口 缝隙大小 */
static const int showbar                 = 1;         /* 是否显示状态栏 */
static const int topbar                  = 1;         /* 指定状态栏位置 0底部 1顶部 */
static const float mfact                 = 0.6;       /* 主工作区 大小比例 */
static const int   nmaster               = 1;         /* 主工作区 窗口数量 */
static const unsigned int snap           = 10;        /* 边缘依附宽度 */
static const unsigned int baralpha       = 0xc0;      /* 状态栏透明度 */
static const unsigned int borderalpha    = 0xdd;      /* 边框透明度 */
static const char *fonts[]               = { "JetBrainsMono Nerd Font:style=medium:size=11", "monospace:size=11" };
static const char *colors[][3]           = {          /* 颜色设置 ColFg, ColBg, ColBorder */ 
    [SchemeNorm] = { "#bbbbbb", "#333333", "#444444" },
    [SchemeSel] = { "#ffffff", "#37474F", "#42A5F5" },
    [SchemeSelGlobal] = { "#ffffff", "#37474F", "#FFC0CB" },
    [SchemeHid] = { "#dddddd", NULL, NULL },
    [SchemeSystray] = { NULL, "#7799AA", NULL },
    [SchemeUnderline] = { "#7799AA", NULL, NULL }, 
    [SchemeNormTag] = { "#bbbbbb", "#333333", NULL },
    [SchemeSelTag] = { "#eeeeee", "#333333", NULL },
    [SchemeBarEmpty] = { NULL, "#111111", NULL },
};
static const unsigned int alphas[][3]    = {          /* 透明度设置 ColFg, ColBg, ColBorder */ 
    [SchemeNorm] = { OPAQUE, baralpha, borderalpha }, 
    [SchemeSel] = { OPAQUE, baralpha, borderalpha },
    [SchemeSelGlobal] = { OPAQUE, baralpha, borderalpha },
    [SchemeNormTag] = { OPAQUE, baralpha, borderalpha }, 
    [SchemeSelTag] = { OPAQUE, baralpha, borderalpha },
    [SchemeBarEmpty] = { 0, 0x11, 0 },
    [SchemeStatusText] = { OPAQUE, 0x88, 0},
};

/* 自定义脚本位置 */
#define SCRIPTS_PATH "$DWM/scripts"
static const char *autostartscript = SCRIPTS_PATH"/autostart.sh";
static const char *statusbarscript = SCRIPTS_PATH"/statusbar/statusbar.sh";

/* 自定义 scratchpad instance */
static const char scratchpadname[] = "scratchpad";

/* 自定义tag名称 */
/* 自定义特定实例的显示状态 */
//            ﮸  ﭮ 
static const char *tags[] = { 
    "", // tag:0  key:1  desc:terminal1
    "󰎧", // tag:1  key:2  desc:terminal2
    "󰎪", // tag:2  key:3  desc:terminal3
    "󰎭", // tag:3  key:4  desc:terminal4
    "󰎱", // tag:4  key:5  desc:terminal5
    "", // tag:5  key:b  desc:obsidian
    "", // tag:6  key:c  desc:chrome
    // "", // tag:5  key:d  desc:eduic
    "", // tag:7  key:0  desc:empty
    // "", // tag:7  key:m  desc:music
    /* "ﬄ", // tag:7  key:0  desc:qq */
    /* "﬐", // tag:8  key:w  desc:wechat */
    /* "", // tag:9  key:l  desc:wxwork */
};

/* 自定义窗口显示规则 */
/* class instance title 主要用于定位窗口适合哪个规则 */
/* tags mask 定义符合该规则的窗口的tag 0 表示当前tag */
/* isfloating 定义符合该规则的窗口是否浮动 */
/* isglobal 定义符合该规则的窗口是否全局浮动 */
/* isnoborder 定义符合该规则的窗口是否无边框 */
/* monitor 定义符合该规则的窗口显示在哪个显示器上 -1 为当前屏幕 */
/* floatposition 定义符合该规则的窗口显示的位置 0 中间，1到9分别为9宫格位置，例如1左上，9右下，3右上 */
static const Rule rules[] = {
    /* class                 instance              title             tags mask     isfloating  isglobal    isnoborder monitor floatposition */
    /** 优先级高 越在上面优先度越高 */
    { NULL,                  NULL,                "图片查看器",      0,            1,          0,          0,        -1,      0}, // qq图片查看器        浮动
    { NULL,                  NULL,                "图片查看",        0,            1,          0,          0,        -1,      0}, // 微信图片查看器      浮动

    /** 普通优先度 */
    {"obsidian",             NULL,                 NULL,             1 << 5,       0,          0,          0,        -1,      0}, // obs        tag -> 󰕧
    {"chrome",               NULL,                 NULL,             1 << 6,       0,          0,          0,        -1,      0}, // chrome     tag -> 
    {"Chromium",             NULL,                 NULL,             1 << 6,       0,          0,          0,        -1,      0}, // Chromium   tag -> 
    // {"music",                NULL,                 NULL,             1 << 5,       1,          0,          1,        -1,      0}, // music      tag ->  浮动、无边框
    { NULL,                 "qq",                  NULL,             1 << 6,       0,          0,          1,        -1,      0}, // qq         tag -> ﬄ 无边框
    { NULL,                 "wechat.exe",          NULL,             1 << 7,       0,          0,          1,        -1,      0}, // wechat     tag -> ﬐ 无边框
    { NULL,                 "wxwork.exe",          NULL,             1 << 8,       0,          0,          1,        -1,      0}, // workwechat tag ->  无边框
    {"Vncviewer",            NULL,                 NULL,             0,            1,          0,          1,        -1,      2}, // Vncviewer           浮动、无边框 屏幕顶部
    {"flameshot",            NULL,                 NULL,             0,            1,          0,          0,        -1,      0}, // 火焰截图            浮动
    {"scratchpad",          "scratchpad",         "scratchpad",      TAGMASK,      1,          1,          1,        -1,      2}, // scratchpad          浮动、全局、无边框 屏幕顶部
    {"Pcmanfm",              NULL,                 NULL,             0,            1,          0,          1,        -1,      3}, // pcmanfm             浮动、无边框 右上角
    {"wemeetapp",            NULL,                 NULL,             TAGMASK,      1,          1,          0,        -1,      0}, // !!!腾讯会议在切换tag时有诡异bug导致退出 变成global来规避该问题

    /** 部分特殊class的规则 */
    {"float",                NULL,                 NULL,             0,            1,          0,          0,        -1,      0}, // class = float       浮动
    {"global",               NULL,                 NULL,             TAGMASK,      0,          1,          0,        -1,      0}, // class = gloabl      全局
    {"noborder",             NULL,                 NULL,             0,            0,          0,          1,        -1,      0}, // class = noborder    无边框
    {"FGN",                  NULL,                 NULL,             TAGMASK,      1,          1,          1,        -1,      0}, // class = FGN         浮动、全局、无边框
    {"FG",                   NULL,                 NULL,             TAGMASK,      1,          1,          0,        -1,      0}, // class = FG          浮动、全局
    {"FN",                   NULL,                 NULL,             0,            1,          0,          1,        -1,      0}, // class = FN          浮动、无边框
    {"GN",                   NULL,                 NULL,             TAGMASK,      0,          1,          1,        -1,      0}, // CLASS = GN          全局、无边框

    /** 优先度低 越在上面优先度越低 */
    { NULL,                  NULL,                "crx_",            0,            1,          0,          0,        -1,      0}, // 错误载入时 会有crx_ 浮动
    { NULL,                  NULL,                "broken",          0,            1,          0,          0,        -1,      0}, // 错误载入时 会有broken 浮动
};
static const char *overviewtag = "OVERVIEW";
static const Layout overviewlayout = { "󰍄",  overview };

/* 自定义布局 */
static const Layout layouts[] = {
    { "󰙀",  tile },         /* 主次栈 */
    { "󰕰",  magicgrid },    /* 网格 */
    // { "󰾍",  magicgrid },    /* 网格 */
};

#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }
#define MODKEY Mod1Mask //alt
//#define MODKEY Mod4Mask //super
#define TAGKEYS(KEY, TAG, cmd, judge) \
    { MODKEY,              KEY, view,       {.ui = 1 << TAG, .v = cmd, .j = judge} }, \
    { MODKEY|ShiftMask,    KEY, tag,        {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask,  KEY, toggleview, {.ui = 1 << TAG} }, \

static Key keys[] = {
    /* modifier            key              function          argument */
    { MODKEY,              XK_BackSpace,    togglesystray,    {0} },                     /* mod backspace   |  切换 托盘栏显示状态 */

    { MODKEY,              XK_Tab,          focusstack,       {.i = +1} },               /* mod tab          |  本tag内切换聚焦窗口 */
    { MODKEY|ShiftMask,    XK_Tab,          focusstack,       {.i = -1} },               /* mod shift tab    |  本tag内切换聚焦窗口 */

    { MODKEY,              XK_comma,        viewtoleft,       {0} },                     /* mod ,            |  聚焦到左边的tag */
    { MODKEY,              XK_period,       viewtoright,      {0} },                     /* mod .            |  聚焦到右边的tag */
    { MODKEY|ShiftMask,    XK_comma,        tagtoleft,        {0} },                     /* mod shift ,      |  将本窗口移动到左边tag */
    { MODKEY|ShiftMask,    XK_period,       tagtoright,       {0} },                     /* mod shift .      |  将本窗口移动到右边tag */

    { MODKEY|ShiftMask,    XK_Return,       zoom,             {0} },                     /* mod shift enter  |  将当前聚焦窗口置为主窗口 */
    { MODKEY,              XK_a,            toggleoverview,   {0} },                     /* mod a            |  显示所有tag 或 跳转到聚焦窗口的tag */
    { MODKEY,              XK_k,            focusdir,         {.i = UP } },              /* mod k            | 二维聚焦窗口 */
    { MODKEY,              XK_j,            focusdir,         {.i = DOWN } },            /* mod j            | 二维聚焦窗口 */
    { MODKEY,              XK_h,            focusdir,         {.i = LEFT } },            /* mod h            | 二维聚焦窗口 */
    { MODKEY,              XK_l,            focusdir,         {.i = RIGHT } },           /* mod l            | 二维聚焦窗口 */
    { MODKEY|ShiftMask,    XK_k,            exchange_client,  {.i = UP } },              /* mod shift k      | 二维交换窗口 (仅平铺) */
    { MODKEY|ShiftMask,    XK_j,            exchange_client,  {.i = DOWN } },            /* mod shift j      | 二维交换窗口 (仅平铺) */
    { MODKEY|ShiftMask,    XK_h,            exchange_client,  {.i = LEFT} },             /* mod shift h      | 二维交换窗口 (仅平铺) */
    { MODKEY|ShiftMask,    XK_l,            exchange_client,  {.i = RIGHT } },           /* mod shift l      | 二维交换窗口 (仅平铺) */

    { MODKEY,              XK_minus,        setmfact,         {.f = -0.05} },            /* mod -            |  缩小主工作区 */
    { MODKEY,              XK_equal,        setmfact,         {.f = +0.05} },            /* mod =            |  放大主工作区 */

    { MODKEY,              XK_i,            hidewin,          {0} },                     /* mod i            |  隐藏 窗口 */
    { MODKEY|ShiftMask,    XK_i,            restorewin,       {0} },                     /* mod shift i      |  取消隐藏 窗口 */


    { MODKEY,              XK_t,            togglefloating,   {0} },                     /* mod t            |  开启/关闭 聚焦目标的float模式 */
    { MODKEY|ShiftMask,    XK_t,            toggleallfloating,{0} },                     /* mod shift t      |  开启/关闭 全部目标的float模式 */
    { MODKEY,              XK_f,            fullscreen,       {0} },                     /* mod f            |  开启/关闭 全屏 */
    { MODKEY|ShiftMask,    XK_f,            togglebar,        {0} },                     /* mod shift f      |  开启/关闭 状态栏 */
    { MODKEY,              XK_g,            toggleglobal,     {0} },                     /* mod g            |  开启/关闭 全局 */
    { MODKEY,              XK_u,            toggleborder,     {0} },                     /* mod u            |  开启/关闭 边框 */
    { MODKEY,              XK_e,            incnmaster,       {.i = +1} },               /* mod e            |  改变主工作区窗口数量 (1 2中切换) */

    { MODKEY,              XK_F10,          focusmon,         {.i = +1} },               /* mod b            |  光标移动到另一个显示器 */
    { MODKEY|ShiftMask,    XK_F10,          tagmon,           {.i = +1} },               /* mod shift b      |  将聚焦窗口移动到另一个显示器 */

    { MODKEY,              XK_q,            killclient,       {0} },                     /* mod q            |  关闭窗口 */
    { MODKEY|ControlMask,  XK_q,            forcekillclient,  {0} },                     /* mod ctrl q       |  强制关闭窗口(处理某些情况下无法销毁的窗口) */
    { MODKEY|ControlMask,  XK_F12,          quit,             {0} },                     /* mod ctrl f12     |  退出dwm */

	{ MODKEY|ShiftMask,    XK_space,        selectlayout,     {.v = &layouts[1]} },      /* mod shift space  |  切换到网格布局 */
	{ MODKEY,              XK_o,            showonlyorall,    {0} },                     /* mod o            |  切换 只显示一个窗口 / 全部显示 */

    { MODKEY|ControlMask,  XK_equal,        setgap,           {.i = -6} },               /* mod ctrl +       |  窗口增大 */
    { MODKEY|ControlMask,  XK_minus,        setgap,           {.i = +6} },               /* mod ctrl -       |  窗口减小 */
    { MODKEY|ControlMask,  XK_space,        setgap,           {.i = 0} },                /* mod ctrl space   |  窗口重置 */

    { MODKEY|ControlMask,  XK_k,            movewin,          {.ui = UP} },              /* mod ctrl k       |  移动窗口 */
    { MODKEY|ControlMask,  XK_j,            movewin,          {.ui = DOWN} },            /* mod ctrl j       |  移动窗口 */
    { MODKEY|ControlMask,  XK_h,            movewin,          {.ui = LEFT} },            /* mod ctrl h       |  移动窗口 */
    { MODKEY|ControlMask,  XK_l,            movewin,          {.ui = RIGHT} },           /* mod ctrl l       |  移动窗口 */

    { MODKEY|ControlMask,  XK_Up,           resizewin,        {.ui = V_REDUCE} },        /* mod ctrl up      |  调整窗口 */
    { MODKEY|ControlMask,  XK_Down,         resizewin,        {.ui = V_EXPAND} },        /* mod ctrl down    |  调整窗口 */
    { MODKEY|ControlMask,  XK_Left,         resizewin,        {.ui = H_REDUCE} },        /* mod ctrl left    |  调整窗口 */
    { MODKEY|ControlMask,  XK_Right,        resizewin,        {.ui = H_EXPAND} },        /* mod ctrl right   |  调整窗口 */


    /* spawn + SHCMD 执行对应命令(已下部分建议完全自己重新定义) */
    { MODKEY|ShiftMask,    XK_F12,    spawn, SHCMD("killall dwm") },                                            /* mod shift f12  | 强制退出dwm */
    { MODKEY,              XK_Return, spawn, SHCMD("st") },                                                     /* mod enter      | 打开st终端             */
    { MODKEY,              XK_space,  spawn, SHCMD("st -c FG") },                                               /* mod +          | 打开全局st终端         */
    // { MODKEY,              XK_space,  spawn, SHCMD("st -c float") },                                         /* mod space      | 打开浮动st终端         */ */
    // { MODKEY,              XK_s,      togglescratch, SHCMD("st -t scratchpad -c float") },                   /* mod s          | 打开scratch终端        */ */
    { MODKEY,              XK_F1,     spawn, SHCMD("killall pcmanfm || pcmanfm") },                             /* mod F1         | 打开/关闭pcmanfm       */
    { MODKEY,              XK_r,      spawn, SHCMD("rofi -show run") },                                         /* mod d          | rofi: 执行run          */
    { MODKEY,              XK_slash,  spawn, SHCMD("rofi -show recursivebrowser") },                            /* mod /          | rofi: 执行run          */
    { MODKEY,              XK_p,      spawn, SHCMD(SCRIPTS_PATH"/rofi.sh") },                                   /* mod p          | rofi: 执行自定义脚本   */
    { MODKEY,              XK_n,      spawn, SHCMD("$DWM/scripts/blurlock.sh") },                               /* mod n          | 锁定屏幕               */
    { MODKEY|ShiftMask,    XK_Up,     spawn, SHCMD(SCRIPTS_PATH"/set_vol.sh up") },                             /* mod shift up   | 音量加                 */
    { MODKEY|ShiftMask,    XK_Down,   spawn, SHCMD(SCRIPTS_PATH"/set_vol.sh down") },                           /* mod shift down | 音量减                 */
    { MODKEY|ShiftMask,    XK_a,      spawn, SHCMD("flameshot gui -c -p ~/Pictures/Screenshots") },             /* mod shift a    | 截图                   */
    { MODKEY|ShiftMask,    XK_q,      spawn, SHCMD("kill -9 $(xprop | grep _NET_WM_PID | awk '{print $3}')") }, /* mod shift q    | 选中某个窗口并强制kill */

    /* super key : 跳转到对应tag (可附加一条命令 若目标目录无窗口，则执行该命令) */
    /* super shift key : 将聚焦窗口移动到对应tag */
    /* key tag cmd */

    TAGKEYS(XK_1, 0, 0, 0)
    TAGKEYS(XK_2, 1, 0, 0)
    TAGKEYS(XK_3, 2, 0, 0)
    TAGKEYS(XK_4, 3, 0, 0)
    TAGKEYS(XK_5, 4, 0, 0)
    TAGKEYS(XK_b, 5, "obsidian", "ps -aux | grep obsidian | grep -v grep | wc -l")
    TAGKEYS(XK_c, 6, "google-chrome-stable", "ps -aux | grep google-chrome | grep -v grep | wc -l")
 // TAGKEYS(XK_d, 5, "eudic.AppImage", "ps -aux | grep eudic.AppImage | grep -v grep | wc -l")
    TAGKEYS(XK_0, 7, 0, 0)
    // TAGKEYS(XK_m, 8, SCRIPTS_PATH"/music_player.sh", 0)
    /* TAGKEYS(XK_0, 7, "linuxqq", 0) */
    /* TAGKEYS(XK_w, 8, "/opt/apps/com.qq.weixin.deepin/files/run.sh", 0) */
};

static Button buttons[] = {
    /* click               event mask       button            function       argument  */
    /* 点击窗口标题栏操作 */
    { ClkWinTitle,         0,               Button1,          hideotherwins, {0} },                                   // 左键        |  点击标题     |  隐藏其他窗口仅保留该窗口
    { ClkWinTitle,         0,               Button3,          togglewin,     {0} },                                   // 右键        |  点击标题     |  切换窗口显示状态
    /* 点击窗口操作 */
    { ClkClientWin,        MODKEY,          Button1,          movemouse,     {0} },                                   // mod + 左键  |  拖拽窗口     |  拖拽窗口
    { ClkClientWin,        MODKEY,          Button3,          resizemouse,   {0} },                                   // mod + 右键  |  拖拽窗口     |  改变窗口大小
    /* 点击tag操作 */
    { ClkTagBar,           0,               Button1,          view,          {0} },                                   // 左键        |  点击tag      |  切换tag
	{ ClkTagBar,           0,               Button3,          toggleview,    {0} },                                   // 右键        |  点击tag      |  切换是否显示tag
    { ClkTagBar,           MODKEY,          Button1,          tag,           {0} },                                   // mod + 左键  |  点击tag      |  将窗口移动到对应tag
    { ClkTagBar,           0,               Button4,          viewtoleft,    {0} },                                   // 鼠标滚轮上  |  tag          |  向前切换tag
	{ ClkTagBar,           0,               Button5,          viewtoright,   {0} },                                   // 鼠标滚轮下  |  tag          |  向后切换tag
    /* 点击状态栏操作 */
    { ClkStatusText,       0,               Button1,          clickstatusbar,{0} },                                   // 左键        |  点击状态栏   |  根据状态栏的信号执行 ~/scripts/dwmstatusbar.sh $signal L
    { ClkStatusText,       0,               Button2,          clickstatusbar,{0} },                                   // 中键        |  点击状态栏   |  根据状态栏的信号执行 ~/scripts/dwmstatusbar.sh $signal M
    { ClkStatusText,       0,               Button3,          clickstatusbar,{0} },                                   // 右键        |  点击状态栏   |  根据状态栏的信号执行 ~/scripts/dwmstatusbar.sh $signal R
    { ClkStatusText,       0,               Button4,          clickstatusbar,{0} },                                   // 鼠标滚轮上  |  状态栏       |  根据状态栏的信号执行 ~/scripts/dwmstatusbar.sh $signal U
    { ClkStatusText,       0,               Button5,          clickstatusbar,{0} },                                   // 鼠标滚轮下  |  状态栏       |  根据状态栏的信号执行 ~/scripts/dwmstatusbar.sh $signal D
                                                                                                                      //
    /* 点击bar空白处 */
    { ClkBarEmpty,         0,               Button1,          spawn, SHCMD(SCRIPTS_PATH"/call_rofi.sh window") },        // 左键        |  bar空白处    |  rofi 执行 window
    { ClkBarEmpty,         0,               Button3,          spawn, SHCMD(SCRIPTS_PATH"/call_rofi.sh drun") },          // 右键        |  bar空白处    |  rofi 执行 drun
};

