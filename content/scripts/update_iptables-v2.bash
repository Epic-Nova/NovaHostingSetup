#!/bin/bash

POSITIONAL=()
while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
    -e|--subnet)
      SUBNET="$2"
      shift # past argument
      shift # past value
      ;;
    -s|--wan-interface)
      WAN_INTERFACE="$2"
      shift # past argument
      shift # past value
      ;;
    -l|--lan-interface)
      LAN_INTERFACE="$2"
      shift # past argument
      shift # past value
      ;;
    -l|--port)
      PORT="$2"
      shift # past argument
      shift # past value
      ;;
    -l|--protocol)
      PROTOCOL="$2"
      shift # past argument
      shift # past value
      ;;
    -l|--port-state)
      PORT_STATE="$2"
      shift # past argument
      shift # past value
      ;;
    -l|--port-Forwarding)
      PORT_FORWARDING="$2"
      shift # past argument
      shift # past value
      ;;
    *) 
    # unknown option
      POSITIONAL+=("$1") # save it in an array for later
      shift # past argument
      ;;
  esac
done

LOGFILE=access_table

if [ -z "$SUBNET" ] || [ -z "$WAN_INTERFACE" ] || [ -z "$LAN_INTERFACE" ] || [ -z "$TUN_INTERFACE" ] || [ -z "$PORT" ] || [ -z "$PROTOCOL" ] 
then exit "one or more required variables are not defined"
fi

iptables -I (INPUT/OUTPUT/FORWARD) -s (eg. 10.0.0.0/8) -i (Interface) -p (tcp/udp) -dport (PORT) -j (ACCEPT/DROP)

while IFS= read -r line; do
iptables -D INPUT -p $2 -s 0.0.0.0/0 --dport $1 -j DROP
iptables -D INPUT -p $2 -s localhost --dport $1 -j ACCEPT
iptables -D INPUT -p 2 -s $line --dport $1 -j ACCEPT

sed -i "/$line/d" $LOGFILE
done <$LOGFILE


new_addresses=($(host $3 | awk '/address/ {print $NF}'))
for i in "${new_addresses[@]}"
do
iptables -I INPUT -p 2 -s $i --dport $1 -j ACCEPT
echo "Added $i to the trusted ip list"
echo "$i" >> $LOGFILE
done

iptables -A INPUT -p $2 -s 0.0.0.0/0 --dport $1 -j DROP
iptables -A INPUT -p $2 -s localhost --dport $1 -j ACCEPT
iptables-save
echo "Updated SSH Access Table"