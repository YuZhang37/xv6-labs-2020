#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

//get metadata about the system
int
fill_sysinfo(uint64 addr) 
{
  struct proc *p = myproc();
  struct sysinfo si;
  if (set_freemem(&si.freemem) < 0) {
    return -1;
  }
  if (set_nproc(&si.nproc) < 0) {
    return -1;
  }
  if (copyout(p->pagetable, addr, (char*) &si, sizeof(si)) < 0) {
    return -1;
  }
  return 0;
}