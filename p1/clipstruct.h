 #ifndef CLIPSTRUCT_H
 #define CLIPSTRUCT_H

#include <linux/list.h>

struct clipstruct {
    unsigned int id;
    char* buffer;
    struct list_head lista;
};
 
#endif /* CLIPSTRUCT_H */

