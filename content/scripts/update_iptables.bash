#!/bin/bash

HOSTNAME=vpn.epicnova.net
LOGFILE=access_table

while IFS= read -r line; do
iptables -D INPUT -p tcp -s 0.0.0.0/0 --dport 22 -j DROP
iptables -D INPUT -p tcp -s $line --dport 22 -j ACCEPT

sed -i "/$line/d" $LOGFILE
done <$LOGFILE


new_addresses=($(host $HOSTNAME | awk '/address/ {print $NF}'))
for i in "${new_addresses[@]}"
do
iptables -I INPUT -p tcp -s $i --dport 22 -j ACCEPT
echo "Added $i to the trusted ip list"
echo "$i" >> $LOGFILE
done

iptables -A INPUT -p tcp -s 0.0.0.0/0 --dport 22 -j DROP
iptables-save
echo "Updated SSH Access Table"