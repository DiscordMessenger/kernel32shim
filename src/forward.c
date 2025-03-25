#define EXPORT_MANDATORY(n) EXPORT(n)
#define EXPORT_OPTIONAL(n) EXPORT(n)

#define EXPORT(name) extern void* P_ ## name
#include "forward.h"
#undef EXPORT

#define EXPORT(name) __attribute__((dllexport)) void name()
#include "forward.h"
#undef EXPORT

// note: need to prefix names with _ because cringe
#define EXPORT(name)              \
	__asm__(                      \
		".global _" #name "\n"    \
		".extern _P_" #name "\n"  \
		".section .text\n"        \
		"_" #name ":\n"           \
		"\tjmp (_P_" #name ")\n"  \
	)
#include "forward.h"
#undef EXPORT

#undef EXPORT_MANDATORY
#undef EXPORT_OPTIONAL
