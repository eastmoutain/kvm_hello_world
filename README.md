
# kvm_hello_world

## description
kvm_hello_world refers to https://github.com/dpw/kvm-hello-world.

## Changes
1. add guest VM code, which brings the guest vm from real mode to
long mode.
2. set up page table for guest VM.
3. Guest VM can print its debug message to console with the help of host.

## Build
make Makefile

## Run
./sudo kvm_hello_world

## Expected results
alloc 0x40000000 physical address space for guest  
copy [0x603000~0xa0c918], size 0x409918 to kvm space  
[vm] hello, world!  
[vm] try to add your own code, bye!  
vm exit successfully!  
