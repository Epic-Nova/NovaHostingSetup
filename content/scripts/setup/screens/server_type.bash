#!/bin/bash

server_type=$(whiptail --title "Epic Nova Installer" --radiolist \ "Select an Server" 12 50 5 \
"1" "Interface Server" OFF \
"2" "Master Server" ON \
"3" "Remote Server" OFF \
"4" "VPN Server" OFF 3>&1 1>&2 2>&3)

exit $server_type