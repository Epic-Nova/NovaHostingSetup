#!/bin/bash
function jumpto
{
    label=$1
    cmd=$(sed -n "/$label:/{:a;n;p;ba};" $0 | grep -v ':$')
    eval "$cmd"
    exit
}

start=${1:-"start"}

jumpto $start

start:

port=$(whiptail --inputbox "Enter an Port you want to Block (Comma separated and without spaces)" 8 39 21 --title "Epic Nova Installer" 3>&1 1>&2 2>&3)

protocol=$(whiptail --inputbox "Enter the protocol on which you want to block this port" 8 39 tcp --title "Epic Nova Installer" 3>&1 1>&2 2>&3)

bash /etc/nova/scripts/setup/register_crontabs.bash $port $protocol vpn.epicnova.net
bash /etc/nova/scripts/setup/register_crontabs.bash $port $protocol admin.epicnova.net

if (whiptail --title "Epic Nova Installer" --yesno "Do you want to add more ports?" 8 78); then
    jumpto $start
else
    exit
fi