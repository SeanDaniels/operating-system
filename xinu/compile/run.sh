vboxmanage controlvm backend poweroff
make clean
make
sleep 2
vboxmanage startvm backend
sleep 5
minicom -s 
