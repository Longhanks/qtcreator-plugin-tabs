SCRIPT_PATH=$(dirname $(realpath -s $0))
/mnt/c/Windows/System32/cmd.exe /C $(wslpath -aw $SCRIPT_PATH)\\build_win.bat
