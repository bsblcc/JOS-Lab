// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at vpt
	//   (see <inc/memlayout.h>).
	// LAB 4: Your code here.


	if ( ((err & FEC_WR) == 0) || ((vpd[PDX(addr)] & PTE_P) == 0) || ((vpt[PGNUM(addr)] & PTE_COW) == 0))
	{
		panic("not a COW page fault\n");
	}


	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.
	//   No need to explicitly delete the old page's mapping.

	// LAB 4: Your code here.

	r = sys_page_alloc(0 , PFTEMP, PTE_U | PTE_W | PTE_P);
	if (r < 0)
	{
		panic("%e\n" , r);
	}

	memmove(PFTEMP , ROUNDDOWN(addr , PGSIZE) , PGSIZE);

	r = sys_page_map(0 , PFTEMP , 0, ROUNDDOWN(addr, PGSIZE), PTE_W | PTE_U | PTE_P);
	if (r < 0)
	{
		panic("%e\n" , r);
	}



	return;
	panic("pgfault not implemented");
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;


	// LAB 4: Your code here.
	void* addr = (void*) (pn * PGSIZE);
	pte_t pte = vpt[PGNUM(addr)];

	if ((pte & PTE_W) != 0 || (pte & PTE_COW) != 0)
	{

		r = sys_page_map(0 , addr , envid , addr , PTE_U | PTE_COW | PTE_P);
		if (r < 0)
		{
			panic("%e\n" , r);
		}

		if ((pte & PTE_W) != 0)
			r = sys_page_map(0 , addr, 0 , addr, PTE_U | PTE_COW | PTE_P);


		if (r < 0)
		{
			panic("%e\n" , r);
		}
	}
	else
	{
		r = sys_page_map(0, addr ,envid , addr, PTE_U | PTE_P);
		if (r < 0)
		{
			panic("%e\n" , r);
		}
	}


	return 0;
}


//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use vpd, vpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	set_pgfault_handler(pgfault);

	envid_t envid = sys_exofork();
	if (envid < 0)
	{
		panic("%e\n" , envid);
	}
	if (envid == 0)
	{
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}
	uint32_t addr;
	for (addr = UTEXT ; addr < UXSTACKTOP - PGSIZE ; addr += PGSIZE)
	{
		if (((vpd[PDX(addr)] & PTE_P) != 0) && ((vpt[PGNUM(addr)] & PTE_P) != 0)) 
		{
			duppage(envid, addr / PGSIZE);
		}
	}
	int result = sys_page_alloc(envid , (void*)UXSTACKTOP - PGSIZE , PTE_U | PTE_W | PTE_P);
	if (result < 0)
	{
		panic("%e\n" , result);
	}
	extern void _pgfault_upcall();



	result = sys_env_set_pgfault_upcall(envid , _pgfault_upcall);
	if (result < 0)
	{
		panic("%e\n" , result);
	}
	result = sys_env_set_status(envid , ENV_RUNNABLE);

	if (result < 0)
	{
		panic("%e\n" , result);
	}
	//cprintf("ive already set env#%d to runnable\n" , ENVX(envid));
	return envid;

	panic("fork not implemented");
}



static int
sduppage(envid_t envid , unsigned pn)
{
	int r;
	void* addr = (void*) (pn * PGSIZE);
	pte_t pte = vpt[PGNUM(addr)];


	if ((pte & PTE_COW) != 0)
	{

		r = sys_page_map(0 , addr , envid , addr , PTE_U | PTE_COW | PTE_P);
		if (r < 0)
		{
			panic("%e\n" , r);
		}
	}
	else
	{
		if ((pte & PTE_W) != 0)
		{
			r = sys_page_map(0, addr ,envid , addr,PTE_U | PTE_P | PTE_W);
			if (r < 0)
			{
				panic("%e\n" , r);
			}
		}
		else
		{
			r = sys_page_map(0, addr ,envid , addr, PTE_U | PTE_P);
			if (r < 0)
			{
				panic("%e\n" , r);
			}
		}
	}


	return 0;
}

// Challenge!
int
sfork(void)
{
		// it seems OK but I cant handle the global "thisenv"
		// so user program "pingppongs" cant work well for child and parent 
		// cant recgonize themselves by "thisenv"

	set_pgfault_handler(pgfault);

	uint32_t addr;

	envid_t envid = sys_exofork();
	if (envid < 0)
	{
		cprintf("exofork failed\n");
		return envid;
	}
	if (envid == 0)
	{
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}


	if (((vpd[PDX(USTACKTOP - PGSIZE)] & PTE_P) != 0) && ((vpt[PGNUM(USTACKTOP - PGSIZE)] & PTE_P) != 0)) 
	{
		duppage(envid, (USTACKTOP - PGSIZE) / PGSIZE);
	}

	for (addr = UTEXT ; addr < USTACKTOP - PGSIZE ; addr += PGSIZE)
	{
		if (((vpd[PDX(addr)] & PTE_P) != 0) && ((vpt[PGNUM(addr)] & PTE_P) != 0)) 
		{
			sduppage(envid, addr / PGSIZE);
		}
	}

	int result = sys_page_alloc(envid , (void*)UXSTACKTOP - PGSIZE , PTE_U | PTE_W | PTE_P);
	if (result < 0)
	{
		cprintf("no mem for UXSTACK\n");
		return -E_INVAL;
	}


	extern void _pgfault_upcall();



	result = sys_env_set_pgfault_upcall(envid , _pgfault_upcall);
	if (result < 0)
	{
		cprintf("set upcall failed\n");
		return -E_INVAL;
	}
	result = sys_env_set_status(envid , ENV_RUNNABLE);

	if (result < 0)
	{
		cprintf("set status failed\n");
		return -E_INVAL;
	}
	//cprintf("ive already set env#%d to runnable\n" , ENVX(envid));
	return envid;



	
}
