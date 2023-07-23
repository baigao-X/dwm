# 打印菜单
#
dwmscriptsdir=$(cd $(dirname $0); cd .. ;pwd)

call_menu() {
    echo ' set wallpaper'
    echo '艹 update statusbar'
    # [ "$(sudo docker ps | grep v2raya)" ] && echo ' close v2raya' || echo ' open v2raya'
    # [ "$(ps aux | grep varaya | grep -v 'grep\|rofi\|nvim')" ] && echo ' close picom' || echo ' open picom'
    # [ "$(ps aux | grep picom | grep -v 'grep\|rofi\|nvim')" ] && echo ' close picom' || echo ' open picom'
}

# 执行菜单
execute_menu() {
    case $1 in
        ' set wallpaper')
            feh --randomize --bg-fill ~/.local/wallpaper/*.png
            ;;
        '艹 update statusbar')
            coproc ($dwmscriptsdir/statusbar/statusbar.sh updateall > /dev/null 2>&1)
            ;;
        # ' open v2raya')
        #     # coproc (sudo docker restart v2raya > /dev/null && $dwmscriptsdir/statusbar/statusbar.sh updateall > /dev/null)
        #     coproc (sudo systemctl start v2raya > /dev/null && $dwmscriptsdir/statusbar/statusbar.sh updateall > /dev/null)
        #     ;;
        # ' close v2raya')
        #     # coproc (sudo docker stop v2raya > /dev/null && $dwmscriptsdir/statusbar/statusbar.sh updateall > /dev/null)
        #     coproc (sudo systemctl stop v2raya > /dev/null && $dwmscriptsdir/statusbar/statusbar.sh updateall > /dev/null)
        #     ;;
        ' open picom')
            coproc (picom --experimental-backends --config ~/.config/picom/picom.conf > /dev/null 2>&1)
            ;;
        ' close picom')
            killall picom
            ;;
    esac
}

execute_menu "$(call_menu | rofi -dmenu -p "")"
