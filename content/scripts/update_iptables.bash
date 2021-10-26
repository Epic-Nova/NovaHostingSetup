#!/bin/bash

LOGFILE=access_table

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