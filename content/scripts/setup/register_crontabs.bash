#!/bin/bash

cat <(fgrep -i -v "bash /etc/nova/scripts/update_iptables.bash $1 $2 $3" <(crontab -l)) <(echo "0 * * * * bash /etc/nova/scripts/update_iptables.bash $1 $2 $3") | crontab -
bash /etc/nova/scripts/update_iptables.bash $1 $2 $3