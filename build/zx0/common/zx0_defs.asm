
;......................
ifndef leave
 macro leave
    rts
 .endm
endif

;......................
ifndef VDC_AR
VDC_AR      = $FF:0000  ; Status Register
endif

;......................
ifndef VDC_SR
VDC_SR      = $FF:0000  ; Address Register
endif

;......................
ifndef VDC_DL
VDC_DL      = $FF:0002  ; Data (Read/Write) Low
endif

;......................
ifndef VDC_DH
VDC_DH      = $FF:0003  ; Data (Read/Write) High
endif

;......................
ifndef VDC_VWR
VDC_VWR     = 2     ; VRAM Data Write
endif

;......................
ifndef _bp
_bp     = $F8:20EC  ; base pointer
endif

;......................
ifndef _si
_si     = $F8:20EE  ; source address
endif

;......................
ifndef _di
_di     = $F8:20F0  ; destination address
endif

;......................
ifndef _ax
_ax     = $F8:20F8
endif

;......................
ifndef _al
_al     = $F8:20F8
endif

;......................
ifndef _ah
_ah     = $F8:20F9

endif

;......................
ifndef _bx
_bx     = $F8:20FA
endif

;......................
ifndef _bl
_bl     = $F8:20FA
endif

;......................
ifndef _bh
_bh     = $F8:20FB

endif

;......................
ifndef _cx
_cx     = $F8:20FC
endif

;......................
ifndef _cl
_cl     = $F8:20FC
endif

;......................
ifndef _ch
_ch     = $F8:20FD

endif

;......................
ifndef _dx
_dx     = $F8:20FE
endif

;......................
ifndef _dl
_dl     = $F8:20FE
endif

;......................
ifndef _dh
_dh     = $F8:20FF
endif

