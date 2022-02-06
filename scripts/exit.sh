echo "++RM MOD++"
rmmod poet.ko
echo "++MAKE++"
make clean
echo "++LOG++"
tail /var/log/kern.log
echo '' > /var/log/kern.log
echo "++FILES++"
ls 
