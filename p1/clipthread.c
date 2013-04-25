#include "clipthread.h"

extern int activo;

int funcion_thread(void *data)
{
	return 0;
}
/*
int _funcion_thread(void *data) 
{
	struct sigaction sa;

	allow_signal(SIGKILL);
	printk(KERN_INFO "iniciando ejecuacion kernel thread.\n");
	activo = 1;	  
	
	/* Capturar la señal SIGUSR1 y asociarle la funcion tratamiento_signal() 
   /* sa.sa_handler = tratamiento_signal; 
    sigemptyset(&sa.sa_mask);  
    sa.sa_flags = 0; 
    sigaction(SIGUSR1, &sa, 0); //FIXME
	
	for(;;){
		// sleep()
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
	
		if (kthread_should_stop()) {
			activo = 0;
			break;
		}
		
		/*
		if (signal_pending(current)){
			if (ticket == ESCRITURA_CLIPBOARD) {
				printk(KERN_INFO "THREAD : ESCRITURA_CLIPBOARD.\n");
			} else if (ticket == CAMBIO_CLIPBOARD) {
				printk(KERN_INFO "THREAD: CAMBIO_CLIPBOARD.\n");
			} else {
				 // señal desconocida
				 activo = 0;
				 break;
			}
		}
		
		
	}
	
	printk(KERN_INFO "finalizando ejecucion kernel thread.\n");
	return 0;
}
*/
void tratamiento_signal(int signal)
{
	printk(KERN_INFO "THREAD : ESCRITURA_CLIPBOARD.. o lo otro.\n");
}

pid_t mi_pid(void){
	return current->pid;
}
