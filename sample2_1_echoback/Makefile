all: BOOTX64.EFI OVMF_CODE.fd OVMF_VARS.fd disk.img

BOOTX64.EFI: main.c
	clang \
	-target x86_64-unknown-windows \
	-ffreestanding -fno-stack-protector -fno-pic \
	-mno-red-zone \
	-c $< -o $@
	lld-link \
	/subsystem:efi_application \
	/entry:efi_main \
	/nodefaultlib \
	$@ \
	/out:BOOTX64.EFI

run: BOOTX64.EFI OVMF_CODE.fd OVMF_VARS.fd disk.img
	qemu-system-x86_64 \
	-machine q35 \
	-m 512M \
	-drive if=pflash,format=raw,readonly=on,file=OVMF_CODE.fd \
	-drive if=pflash,format=raw,file=OVMF_VARS.fd \
	-drive file=disk.img,format=raw \
	-serial stdio

OVMF_CODE.fd:
	cp /opt/homebrew/share/qemu/edk2-x86_64-code.fd ./OVMF_CODE.fd

OVMF_VARS.fd:
	cp /opt/homebrew/share/qemu/edk2-i386-vars.fd ./OVMF_VARS.fd

disk.img:
	dd if=/dev/zero of=disk.img bs=1m count=64 
	mkfs.fat -F 32 disk.img
	mmd   -i disk.img ::/EFI
	mmd   -i disk.img ::/EFI/BOOT
	mcopy -i disk.img BOOTX64.EFI ::/EFI/BOOT/BOOTX64.EFI

clean:
	rm BOOTX64.EFI disk.img

.PHONY: clean
