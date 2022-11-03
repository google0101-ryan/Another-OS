#if ARCH == 1
#include <arch/x86/kernel/kernel.h>
#else
#error "Unsupported arch"
#endif

#include <multiboot.h>

ArchKernel kernel;

void* __dso_handle;
extern "C" int __cxa_atexit(void (*) (void *), void *, void *)
{return 0;}

extern "C" int kmain(multiboot_info_t* info, uint32_t) 
{
	kernel.Init(info);
	return 0;
}