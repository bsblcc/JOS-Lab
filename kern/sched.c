#include <inc/assert.h>

#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/monitor.h>


// Choose a user environment to run and run it.

void
round_robin(void)
{
	struct Env *idle;
	int i;

	// Implement simple round-robin scheduling.
	//
	// Search through 'envs' for an ENV_RUNNABLE environment in
	// circular fashion starting just after the env this CPU was
	// last running.  Switch to the first such environment found.
	//
	// If no envs are runnable, but the environment previously
	// running on this CPU is still ENV_RUNNING, it's okay to
	// choose that environment.
	//
	// Never choose an environment that's currently running on
	// another CPU (env_status == ENV_RUNNING) and never choose an
	// idle environment (env_type == ENV_TYPE_IDLE).  If there are
	// no runnable environments, simply drop through to the code
	// below to switch to this CPU's idle environment.

	// LAB 4: Your code here.




	if(curenv == NULL)
	{
		//cprintf("first sched\n");

		for (i = 0 ; i < NENV; i++)
		{

			if(envs[i].env_status == ENV_RUNNABLE && envs[i].env_type == ENV_TYPE_USER)
			{
				//cprintf("one env found:%d\n" , i);
				env_run(&envs[i]);
			}
		}
	}
	else
	{
		//cprintf("not first\n");
		//cprintf("curenv is %d\n" ,  ENVX(curenv->env_id));
		for (i = ENVX(curenv->env_id) + 1 ; i < NENV ; i++)
		{
			//scprintf("this is env#%d , and its status:%d , its type:%d\n", i,envs[i].env_status,envs[i].env_type);
			if((envs[i].env_status == ENV_RUNNABLE) && (envs[i].env_type == ENV_TYPE_USER))
			{
				//cprintf("one env found:%d\n" , i);
				env_run(&envs[i]);
			}
		}
		for (i = 0 ; i <= ENVX(curenv->env_id) ; i++)
		{
			if((envs[i].env_status == ENV_RUNNABLE) && (envs[i].env_type == ENV_TYPE_USER))
			{
				//cprintf("one env found:%d\n" , i);
				env_run(&envs[i]);
			}
		}
	}

	if (curenv != NULL)
	if (curenv->env_status == ENV_RUNNING)
	{
		//cprintf("still choose curenv\n");
		env_run(curenv);
	}
	//env_run(&envs[cpunum()]);

	// For debugging and testing purposes, if there are no
	// runnable environments other than the idle environments,
	// drop into the kernel monitor.
	for (i = 0; i < NENV; i++) {
		if (envs[i].env_type != ENV_TYPE_IDLE &&
		    (envs[i].env_status == ENV_RUNNABLE ||
		     envs[i].env_status == ENV_RUNNING))
			break;
	}

	if (i == NENV) {
		cprintf("No more runnable environments!\n");
		while (1)
			monitor(NULL);
	}

	// Run this CPU's idle environment when nothing else is runnable.
	idle = &envs[cpunum()];
	if (!(idle->env_status == ENV_RUNNABLE || idle->env_status == ENV_RUNNING))
		panic("CPU %d: No idle environment!", cpunum());
	env_run(idle);





}







void
my_sched_pvl(void)
{
	int i;
	struct Env* low_env = NULL;

	if (curenv == NULL)
	{
		for (i = 0 ; i < NENV ; i++)
		{
			if (envs[i].env_status == ENV_RUNNABLE && envs[i].env_type == ENV_TYPE_USER)
			{
				if (envs[i].env_pvl == 0)
				{
					env_run(&envs[i]);
				}
				else
				{
					if (low_env == NULL)
					{
						low_env = &envs[i];
					}
				}
			}
		}
	}
	else
	{
		for (i = ENVX(curenv->env_id) + 1 ; i < NENV ; i++)
		{

			if (envs[i].env_status == ENV_RUNNABLE && envs[i].env_type == ENV_TYPE_USER)
			{
				if (envs[i].env_pvl == 0)
				{
					env_run(&envs[i]);
				}
				else
				{
					if (low_env == NULL)
					{
						low_env = &envs[i];
					}
				}
			}
		}
		for (i = 0 ; i <= ENVX(curenv->env_id) ; i++)
		{

			if (envs[i].env_status == ENV_RUNNABLE && envs[i].env_type == ENV_TYPE_USER)
			{
				if (envs[i].env_pvl == 0)
				{
					env_run(&envs[i]);
				}
				else
				{
					if (low_env == NULL)
					{
						low_env = &envs[i];
					}
				}
			}
		}
	}

	if (curenv == NULL)
	{
		if(low_env != NULL)
		{
			env_run(low_env);
		}
	}
	else
	{
		if (curenv->env_pvl == 0 && curenv->env_status == ENV_RUNNING)
		{
			env_run(curenv);
		}
		else
		{
			if (low_env != NULL)
			{
				env_run(low_env);
			}
			else
			{
				if (curenv->env_status == ENV_RUNNING)
				{
					env_run(curenv);
				}
			}
		}
	}

/*
	if (curenv != NULL)
	if (curenv->env_pvl == 0 && curenv->env_status == ENV_RUNNING)
	{
		env_run(curenv);
	}

	if (low_env != NULL)
	{
		env_run(low_env);
	}
	else
	{
		if (curenv != NULL)
		if (curenv->env_status == ENV_RUNNING)
		{
			env_run(curenv);
		}
	}
*/
	#define idle low_env


	for (i = 0; i < NENV; i++) {
		if (envs[i].env_type != ENV_TYPE_IDLE &&
		    (envs[i].env_status == ENV_RUNNABLE ||
		     envs[i].env_status == ENV_RUNNING))
			break;
	}

	if (i == NENV) {
		cprintf("No more runnable environments!\n");
		while (1)
			monitor(NULL);
	}

	idle = &envs[cpunum()];
	if (!(idle->env_status == ENV_RUNNABLE || idle->env_status == ENV_RUNNING))
		panic("CPU %d: No idle environment!", cpunum());
	env_run(idle);


}

void 
stride_sched()
{
	// stride ...
	// I didnt user a priority queue like heap because it will
	// make the inter-relation much complex and is not adapted well for lab5.6.7....
	// tonikaku , mandao desu...
	// jiu shi lan...
	// and I really love to impelement a lottery-schedule but 
	// I have to leave it after device-drivers (maybe lab6?)
	// because I cant make a random number...
	// and I hate the fake ones using math formula...

	int min_stride = -1;
	struct Env* e = NULL;
	int i;


	for (i = 0; i < NENV; i++)
	{	
		if (envs[i].env_status == ENV_RUNNABLE && envs[i].env_type == ENV_TYPE_USER)
		{
			min_stride = envs[i].env_stride;
			e = &envs[i];
			break;
		}
	}


	for (; i < NENV; i++)
	{	
		if (envs[i].env_status == ENV_RUNNABLE && envs[i].env_type == ENV_TYPE_USER)
		if (envs[i].env_stride < min_stride)
		{
			min_stride = envs[i].env_stride;
			e = &envs[i];
		}
	}

	if (e == NULL)
	{
		if (curenv != NULL)
		if (curenv->env_status == ENV_RUNNING)
		{
			cprintf("########### choose %d , its stride: %d\n" , ENVX(curenv->env_id), curenv->env_stride);
			curenv->env_stride += curenv->env_step;
			
			env_run(curenv);
		}
	}
	else
	{
		if (curenv != NULL)
		if (curenv->env_status == ENV_RUNNING)
		{
			if (curenv->env_stride < min_stride)
			{
				e = &envs[i];
			}
		}
		cprintf("########### choose %d , its stride: %d\n" , ENVX(e->env_id), e->env_stride);
		e->env_stride += e->env_step;
		env_run(e);

	}

	struct Env* idle;
	for (i = 0; i < NENV; i++) {
		if (envs[i].env_type != ENV_TYPE_IDLE &&
		    (envs[i].env_status == ENV_RUNNABLE ||
		     envs[i].env_status == ENV_RUNNING))
			break;
	}

	if (i == NENV) {
		cprintf("No more runnable environments!\n");
		while (1)
			monitor(NULL);
	}

	idle = &envs[cpunum()];
	if (!(idle->env_status == ENV_RUNNABLE || idle->env_status == ENV_RUNNING))
		panic("CPU %d: No idle environment!", cpunum());
	env_run(idle);

}



void
sched_yield(void)
{
	

	round_robin();

	//stride_sched();


	//my_sched_pvl();

}

