FOENIX = module/Calypsi-m68k-Foenix

BUILD_VER = 0.2.0 ($(shell git branch --show-current), $(shell date +"%b %d %Y %H:%M"))

# Common source files
ASM_SRCS =
C_SRCS = chunk.c debug.c memory.c scanner.c value.c native.c \
         compiler.c main.c object.c table.c vm.c line_edit.c \
		 string_output.c
 
MODEL = --code-model=large --data-model=large
LIB_MODEL = lc-ld
C_FLAGS = -Iinclude -DA2560=1 -DUSE_DL=0 -DNO_WCHAR=1 -DBUILD_VER="\"$(BUILD_VER)\"" -DTARGET_MCP

FOENIX_LIB = lib/foenix-$(LIB_MODEL).a
A2560K_RULES = lib/a2560k-simplified.scm

# Object files
OBJS = $(ASM_SRCS:%.s=obj/%.o) $(C_SRCS:%.c=obj/%.o)
OBJS_DEBUG = $(ASM_SRCS:%.s=obj/%-debug.o) $(C_SRCS:%.c=obj/%-debug.o)

all: lox.pgz

obj/%.o: %.s
	as68k --core=68000 $(MODEL) --debug --list-file=$(@:%.o=%.lst) -o $@ $<

obj/%.o: %.c
	cc68k $(C_FLAGS) --core=68000 $(MODEL) --debug --list-file=$(@:%.o=%.lst) -o $@ $<

obj/%-debug.o: %.s
	as68k --core=68000 $(MODEL) --debug --list-file=$(@:%.o=%.lst) -o $@ $<

obj/%-debug.o: %.c
	cc68k $(C_FLAGS) --core=68000 $(MODEL) --debug --list-file=$(@:%.o=%.lst) -o $@ $<

lox.pgz:  $(OBJS)
	ln68k -o $@ $^ $(A2560K_RULES) clib-68000-$(LIB_MODEL).a $(FOENIX_LIB) --output-format=pgz --list-file=lox.lst --cross-reference --rtattr printf=float --rtattr scanf=float --rtattr cstartup=Foenix_user --stack-size=65536 --heap-size=262144

lox.elf:  $(OBJS)
	ln68k -o $@ $^ $(A2560K_RULES) --debug clib-68000-$(LIB_MODEL).a $(FOENIX_LIB) --list-file=lox.lst --cross-reference --rtattr printf=float --rtattr scanf=float --rtattr cstartup=Foenix_user --stack-size=65536 --heap-size=262144

clean:
	-rm $(OBJS) $(OBJS:%.o=%.lst) $(OBJS_DEBUG) $(OBJS_DEBUG:%.o=%.lst)
	-rm lox.pgz lox.elf lox.lst 
