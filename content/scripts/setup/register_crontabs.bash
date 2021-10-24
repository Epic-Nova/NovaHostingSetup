#!/bin/bash

cat <(fgrep -i -v 'bash /etc/nova/scripts/update_iptables.bash' <(crontab -l)) <(echo '0 * * * * bash /etc/nova/scripts/update_iptables.bash') | crontab -
bash /etc/nova/scripts/update_iptables.bash