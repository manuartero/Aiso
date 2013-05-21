#include "clipthread.h"

int activo;
int periodo;
struct clipstruct *nodo_actual;

int funcion_thread(void *data) {

	printk(KERN_INFO "iniciando ejecuacion kernel thread.\n");
	activo = 1;	  
	
	for(;;){
		if (periodo==0){
			set_current_state(TASK_INTERRUPTIBLE);
			schedule();
		}
		else{
			msleep(10000);
		}
	
		if (signal_pending(current)){
			activo = 0; 
			break;
		}
		
		if (kthread_should_stop()) {
			activo = 0;
			break;
		}	
		printk(KERN_INFO "Estamos en el clipboard:%d\n",nodo_actual->id);
		printk(KERN_INFO "El buffer contiene %d elementos\n",nodo_actual->num_elem);
	}

	printk(KERN_INFO "finalizando ejecucion kernel thread.\n");
	return 0;
}
