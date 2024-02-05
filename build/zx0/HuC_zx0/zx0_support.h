void __fastcall __macro decomp_zx0_vram_base_m( unsigned int bank_p<__ax>, unsigned int addr_p<__bp>,  unsigned int vram_p<__di>);
void __fastcall __macro decomp_zx0_vram_direct_m( char far *obj<__fbank:__bp>,  unsigned int vram_p<__di>);

#define decomp_zx0_vram(container, index, vram_addr) decomp_zx0_vram_base_m(container.bank[index], container.addr[index], vram_addr); huc_decomp_zx0_vram()
#define decomp_zx0_vram_direct(label, vram_addr) decomp_zx0_vram_direct_m(label, vram_addr); huc_decomp_zx0_vram()
#define decomp_zx0_vram_base(bank, addr, vram_addr) decomp_zx0_vram_base_m(bank, addr, vram_addr); huc_decomp_zx0_vram()


#asm
 macro _decomp_zx0_vram_base_m.3
    ldy <__ax
    lda <__bp + 1
    and #$1f
    ora #$60
    sta <__bp + 1
       ;call _huc_decomp_zx0_vram
 .endm

 macro _decomp_zx0_vram_direct_m.2
    ldy <__fbank
    lda <__fptr + 1
    and #$1f
    ora #$60
    sta <__bp + 1
       ;call _huc_decomp_zx0_vram
 .endm

 #endasm

#include "HuC_zx0/huc_zx0_lib.c"

