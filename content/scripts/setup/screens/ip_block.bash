#!/bin/bash
function jumpto
{
    label=$1
    cmd=$(sed -n "/$label:/{:a;n;p;ba};" $0 | grep -v ':$')
    eval "$cmd"
    exit
}

subnet=$(whiptail --inputbox "Enter the Local Epic Nova Subnet eg. 10.0.0.0/8" 8 39 22 --title "Epic Nova Installer" 3>&1 1>&2 2>&3)
wan_interface=$(whiptail --inputbox "Enter the WAN Interface name to use" 8 39 22 --title "Epic Nova Installer" 3>&1 1>&2 2>&3)
lan_interface=$(whiptail --inputbox "Enter the LAN Interface name to use" 8 39 22 --title "Epic Nova Installer" 3>&1 1>&2 2>&3)

start=${1:-"start"}

jumpto $start

start:

port=$(whiptail --inputbox "Enter an Port" 8 39 22 --title "Epic Nova Installer" 3>&1 1>&2 2>&3)
protocol=$(whiptail --inputbox "Enter the protocol for the entered port" 8 39 tcp --title "Epic Nova Installer" 3>&1 1>&2 2>&3)
port_state=$(whiptail --title "Epic Nova Installer" --radiolist \
"Select the state for the port" 10 57 4 \
"ACCEPT" "Accepts the Connections to this port" ON \
"DROP" "Blocks the Connections to this port" OFF  3>&1 1>&2 2>&3)
port_forwarding=$(whiptail --title "Epic Nova Installer" --radiolist \
"Select the state for the port" 10 70 4 \
"INPUT" "Controls the behavior for incoming connections" ON \
"OUTPUT" "Controls the behavior for outgoing connections" OFF \
"FORWARD" "Forwards an port" OFF 3>&1 1>&2 2>&3)

bash /etc/nova/scripts/setup/update_iptables-v2.bash --subnet $subnet --wan-interface $wan_interface --lan-interface $lan_interface --port $port --protocol $protocol --port-state $port_state --port-Forwarding $port_forwarding

#bash /etc/nova/scripts/setup/register_crontabs.bash $port $protocol vpn.epicnova.net
#bash /etc/nova/scripts/setup/register_crontabs.bash $port $protocol admin.epicnova.net

if (whiptail --title "Epic Nova Installer" --yesno "Do you want to add more rules?" 8 78); then
    jumpto $start
else
    exit
fi