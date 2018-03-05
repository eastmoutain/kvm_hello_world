
CC = gcc

.PHONY: kvm_hello_world guest


linker := linker.ld

GUEST_COBJ := guest.o \
			 gprintf.o

GUEST_ASM_OBJ := start.o \

KVM_OBJ := guest \
		   kvm_hello_world.o \
		   dump_register.o

kvm_hello_world: $(KVM_OBJ)
	@$(CC) -Wall -Wextra -Werror -fno-stack-protector $^ -o $@

guest: $(linker) $(GUEST_COBJ) $(GUEST_ASM_OBJ)
	@$(LD) -T $^ -o $@

%.o:%.c
	$(CC) -Wall -Wextra -fno-stack-protector -c $< -o $@

%.o:%.s
	@$(CC) -c $< -o $@

.PHONY: clean

clean:
	@echo "clean..."
	@-rm $(KVM_OBJ) $(GUEST_COBJ) $(GUEST_ASM_OBJ) kvm_hello_world guest
	@echo "clean done"
