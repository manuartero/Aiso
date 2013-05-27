#### Instalacion

 - mknod /dev/aisodrv c 700 0
 - chmod 664 /dev/aisodrv
 - insmod p4.ko


#### Uso

 - echo holahola > /dev/aisodrv 
 - cat /dev/aisodrv 
