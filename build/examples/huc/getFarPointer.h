void __fastcall __macro getFarPointerMacro_base( char far *obj<__fbank:__fptr>, unsigned int bank_p<__ax>, unsigned int addr_p<__bx> );

#define getFarPointer(label, p_container, index) getFarPointerMacro_base(label, &(p_container.bank[index]), &(p_container.addr[index]) )

#asm
 macro _getFarPointerMacro_base.3
        lda __fbank
        sta [__ax]
        cly
        lda __fptr
        sta [__bx],y
        iny
        lda __fptr+1
        sta [__bx],y
        iny
 .endm
#endasm
