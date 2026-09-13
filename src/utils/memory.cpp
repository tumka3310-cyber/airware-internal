#include "memory.h"

namespace mem {
void* read_ptr(uintptr_t addr) { return read<void*>(addr); }
}
