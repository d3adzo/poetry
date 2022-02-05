echo "++MAKING++"
make
echo "++INSERT KO++"
insmod starter.ko
echo "++READING MODULES++"
head /proc/modules 
echo "++WAIT++"
sleep 2
echo "++KERNEL LOG++"
tail /var/log/kern.log
echo "++FILES++"
ls 
