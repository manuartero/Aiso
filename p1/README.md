Practica 1
==========

Clipboard multiple con la siguiente funcionalidad:

1) interactuar mediante ficheros virtuales en el directorio /proc/aisoclip 
 - tendremos NCLIP clipboards a los que se accedera via /proc/aisoclip/clipboard.
 - se eligira el clipboard mediante /proc/aisoclip/selection.
 - NCLIP se fijara mediante un parametro. 
 
> el clipboard multiple se implementara con una lista doblemente enlazada de estructuras clipstructs


Uso
---

```
echo "texto" > /proc/aisoclip/clipboard
cat /proc/aisoclip/clipboard
```
