# Worm for flooding attack

## The things that worm do 
* premise of the victim host : we know the victim name, password and ip
* generate ssh key
* create secret directory
* put the two executable attack file in the secret directory
* execute the one of the file
* if the worm was detected by the victim and be deleted the worm will execute other attack
* modify the sudoers in order to execute the file by unknown password

## Run the program in attacker host
```bash
$ ./worm
```
