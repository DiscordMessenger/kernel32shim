#define EXPORT_MANDATORY(n) EXPORT(n)
#define EXPORT_OPTIONAL(n) EXPORT(n)
#define EXPORT_MANDATORY_DEBUG(n) EXPORT_DEBUG(n)

#define EXPORT(name) extern void* P_ ## name
#define EXPORT_DEBUG(name) extern void* P_ ## name
#include "forward.h"
#undef EXPORT
#undef EXPORT_DEBUG

#define EXPORT(name) __attribute__((dllexport)) void name()
#define EXPORT_DEBUG(name) __attribute__((dllexport)) void name()
#include "forward.h"
#undef EXPORT
#undef EXPORT_DEBUG

// note: need to prefix names with _ because cringe
#define EXPORT(name)              \
	__asm__(                      \
		".global _" #name "\n"    \
		".extern _P_" #name "\n"  \
		".section .text\n"        \
		"_" #name ":\n"           \
		"\tjmp *(_P_" #name ")\n" \
	)

// special debugging exports for fun
#define EXPORT_DEBUG(name)                       \
__asm__(                                         \
    ".global _" #name "              \n"  \
    ".extern _P_" #name "            \n"  \
    ".extern _OutputDebugStringA     \n"  \
    ".extern _PrintIntsDebug         \n"  \
    ".section .text                  \n"  \
    "_" #name ":                     \n"  \
    "    pushal                      \n"  \
    "    push $_" #name "_str        \n"  \
    "    call _OutputDebugStringA    \n"  \
	"    mov 36(%esp), %eax          \n"  \
	"    mov 40(%esp), %ebx          \n"  \
	"    mov 44(%esp), %ecx          \n"  \
	"    mov 48(%esp), %edx          \n"  \
	"    push %edx                   \n"  \
	"    push %ecx                   \n"  \
	"    push %ebx                   \n"  \
	"    push %eax                   \n"  \
	"    call _PrintIntsDebug        \n"  \
    "    popal                       \n"  \
    "    jmp *(_P_" #name ")         \n"  \
    ".section .rodata                \n"  \
    "_" #name "_str:                 \n"  \
    "    .asciz \"DBG: " #name " \"  \n"  \
    ".section .text                  \n"  \
);

#include "forward.h"
#undef EXPORT
#undef EXPORT_DEBUG

#undef EXPORT_MANDATORY
#undef EXPORT_OPTIONAL
