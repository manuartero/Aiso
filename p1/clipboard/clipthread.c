#include "clipthread.h"

int activo;
int periodo;

int funcion_thread(void *data) {

	printk(KERN_INFO "iniciando ejecuacion kernel thread.\n");
	activo = 1;	  
	
	for(;;){
		if (periodo==0){
			set_current_state(TASK_INTERRUPTIBLE);
			printk(KERN_INFO "entramos en el planificador\n");
			schedule();
			printk(KERN_INFO "salimos de dormir\n");
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
		printk(KERN_INFO "las cosas");
	}

	printk(KERN_INFO "finalizando ejecucion kernel thread.\n");
	return 0;
}
