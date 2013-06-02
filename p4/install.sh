#!/bin/sh

# Se espera $module.ko en la misma carpeta: compilar el driver
# make clean
# make

module="p4"
device="aiso_device"
mode="766"

# desistalar modulo previo
if lsmod | grep -q $module; then
    /sbin/rmmod $module
fi 

# instalar el modulo
/sbin/insmod ./$module.ko

# reserva el numero mayor
major=$(awk "\$2==\"$device\" {print \$1}" /proc/devices)

# crear dispositvo en /dev
rm -f /dev/$device
mknod /dev/$device c $major 0

# cambiar los permisos
chmod $mode /dev/$device

