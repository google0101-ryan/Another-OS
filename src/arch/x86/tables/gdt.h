#pragma once

#include <stdint.h>
#include <stddef.h>

class GDT
{
private:
	void CreateEntry(int idx, uint32_t base, uint32_t limit, int flags);
	void CommitGdt();
public:
	void Init();

	void SetTSS(uint32_t ss0, uint32_t esp0);
};