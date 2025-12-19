
= UEFI barebone実行

== へにゃぺんて氏の本がベース

以下のURLの本が底本

https://techbookfest.org/product/6004413558685696?productVariantID=5950397508222976

== Apple Siliconでのツール

テストしたhomebrewのversionは以下です。

//cmd{
% brew --version
Homebrew 5.0.6
//}

PE32+のビルド用に、homebrewからclangとlld-linkをインストール

//cmd{
% brew install llvm lld
//}

この環境だけ、clangをhomebrewのものを優先する。lld-linkは衝突しないので、問題ない

//list[環境変数設定][x86env][sh]{
#!/bin/zsh
HOMEBREW_LLVM_BIN="/opt/homebrew/opt/llvm/bin"
if [ -d "$HOMEBREW_LLVM_BIN" ]; then
  export PATH="$HOMEBREW_LLVM_BIN:$PATH"
  echo "PATH updated (session only): $HOMEBREW_LLVM_BIN"
else
  echo "Error: $HOMEBREW_LLVM_BIN not found"
  exit 1
fi%
//}

以下になればOK

//cmd{
% which clang
/opt/homebrew/opt/llvm/bin/clang
% which lld-link
/opt/homebrew/bin/lld-link
//}

実行ファイルのコンパイル

//cmd{
% clang \
  -target x86_64-unknown-windows \
  -ffreestanding -fno-stack-protector -fno-pic \
  -mno-red-zone \
  -c main.c -o main.o
% lld-link \
  /subsystem:efi_application \
  /entry:efi_main \
  /nodefaultlib \
  main.o \
  /out:BOOTX64.EFI
//}


qemuのインストール

//cmd{
% brew install qemu
//}

OVMF（EDK IIのUEFIファームウェア）のインストール

//cmd{
% cp /opt/homebrew/share/qemu/edk2-x86_64-code.fd ./OVMF_CODE.fd
% cp /opt/homebrew/share/qemu/edk2-i386-vars.fd ./OVMF_VARS.fd
//}

disk.imgの作成

//cmd{
% brew install dosfstools mtools
% dd if=/dev/zero of=disk.img bs=1m count=64 
% mkfs.fat -F 32 disk.img
% mmd   -i disk.img ::/EFI
% mmd   -i disk.img ::/EFI/BOOT
% mcopy -i disk.img BOOTX64.EFI ::/EFI/BOOT/BOOTX64.EFI
//}

disk.imgの確認
//cmd{
% mdir -i disk.img ::/EFI/BOOT

 Volume in drive : has no label
 Volume Serial Number is 62E8-6B5C
Directory for ::/EFI/BOOT

.            <DIR>     2025-12-19  15:53
..           <DIR>     2025-12-19  15:53
BOOTX64  EFI      2048 2025-12-19  15:53
        3 files               2 048 bytes
                         66 055 680 bytes free
//}


qemu実行

//cmd{
% qemu-system-x86_64 \
  -machine q35 \
  -m 512M \
  -drive if=pflash,format=raw,readonly=on,file=OVMF_CODE.fd \
  -drive if=pflash,format=raw,file=OVMF_VARS.fd \
  -drive file=disk.img,format=raw \
  -serial stdio
//}

実行結果

//image[hello_uefi][実行結果]{
//}