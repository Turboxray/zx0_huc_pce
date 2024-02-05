

void huc_decomp_zx0_vram(void)
{

#asm
;// ***************************************************************************
;// ***************************************************************************
;//
;// unpack-zx0.asm
;//
;// HuC6280 decompressor for Einar Saukas's "classic" ZX0 format.
;//
;// The code length is 200 bytes for RAM, 250 bytes for direct-to-VRAM, plus
;// some generic utility code.
;//
;// Copyright John Brandwood 2021-2024.
;// 
;// Distributed under the Boost Software License, Version 1.0.
;// (See accompanying file LICENSE_1_0.txt or copy at
;//  http://www.boost.org/LICENSE_1_0.txt-)
;//
;// ***************************************************************************
;// ***************************************************************************
;//
;// ZX0 "modern" format is not supported, because it costs an extra 4 bytes of
;// code in this decompressor, and it runs slower.
;//
;// Use Emmanuel Marty's SALVADOR ZX0 compressor which can be found here ...
;//  https://github.com/emmanuel-marty/salvador
;//
;// To create a ZX0 file to decompress to RAM
;//
;//  salvador -classic <infile> <outfile>
;//
;// To create a ZX0 file to decompress to VRAM, using a 2KB ring-buffer in RAM
;//
;//  salvador -classic -w 2048 <infile> <outfile>
;//
;// ***************************************************************************
;// ***************************************************************************

	.ifndef	ZX0_WINBUF

ZX0_WINBUF	=	($3800) >> 8		; Default to a 2KB window in
ZX0_WINMSK	=	($0800 - 1) >> 8	; RAM, located at $3800.

	.endif



; ***************************************************************************
; ***************************************************************************
;
; Data usage is 11 bytes of zero-page, using aliases for clarity.
;

zx0_srcptr	=	_bp			; 1 word.
zx0_dstptr	=	_di			; 1 word.

zx0_length	=	_ax			; 1 word.
zx0_offset	=	_bx			; 1 word.
zx0_winptr	=	_cx			; 1 word.
zx0_bitbuf	=	_dl			; 1 byte.

zx0_to_vdc2

		clx				; Offset to PCE VDC.

		tma3				; Preserve MPR3.
		pha

		jsr	set_bp_to_mpr3		; Map zx0_srcptr to MPR3.

		jsr	set_di_to_mawr		; Map zx0_dstptr to VRAM.

		lda	#$40			; Initialize bit-buffer.
		sta	<zx0_bitbuf

		ldy	#$FF			; Initialize offset to $FFFF.
		sty	<zx0_offset + 0
		sty	<zx0_offset + 1

		iny				; Initialize hi-byte of length
		sty	<zx0_length + 1		; to zero.

		lda	#ZX0_WINBUF		; Initialize window ring-buffer
		sta	<zx0_dstptr + 1		; location in RAM.
		sty	<zx0_dstptr + 0

.lz_finished:	iny				; Initialize length back to 1.
		sty	<zx0_length + 0

		lda	<zx0_bitbuf		; Restore bit-buffer.

		asl	a			; Copy from literals or new offset?
		bcc	.cp_literals

		;
		; Copy bytes from new offset.
		;

.new_offset:	jsr	.get_gamma_flag		; Get offset MSB, returns CS.

		cla				; Negate offset MSB and check
		sbc	<zx0_length + 0		; for zero (EOF marker).
		beq	.got_eof

		sec
		ror	a
		sta	<zx0_offset + 1		; Save offset MSB.

		lda	[zx0_srcptr]		; Get offset LSB.
		inc	<zx0_srcptr + 0
		beq	.inc_off_src

.off_skip1:	ror	a			; Last offset bit starts gamma.
		sta	<zx0_offset + 0		; Save offset LSB.

		bcs	.got_lz_two		; Minimum length of 2?

		sty	<zx0_length + 0		; Initialize length back to 1.

		lda	<zx0_bitbuf		; Restore bit-buffer.

		jsr	.get_gamma_data		; Get length, returns CS.

		ldy	<zx0_length + 0		; Get lo-byte of (length+1).
.got_lz_two:	iny

;		bne	.get_lz_win		; N.B. Optimized to do nothing!
;
;		inc	<zx0_length + 1		; Increment from (length+1).
;		dec	<zx0_length + 1		; Decrement because lo-byte=0.

.get_lz_win:	clc				; Calc address of match.
		lda	<zx0_dstptr + 0		; N.B. Offset is negative!
		adc	<zx0_offset + 0
		sta	<zx0_winptr + 0
		lda	<zx0_dstptr + 1
		adc	<zx0_offset + 1
		and	#ZX0_WINMSK
		ora	#ZX0_WINBUF
		sta	<zx0_winptr + 1

.lz_byte:	lda	[zx0_winptr]		; Copy bytes from window into
		sta	[zx0_dstptr]		; decompressed data.
		sta	VDC_DL, x
		txa
		eor	#1
		tax

		inc	<zx0_winptr + 0
		beq	.inc_lz_win
.lz_skip1:	inc	<zx0_dstptr + 0
		beq	.inc_lz_dst

.lz_skip2:	dey				; Any bytes left to copy?
		bne	.lz_byte

		lda	<zx0_length + 1		; Any pages left to copy?
		beq	.lz_finished		; Optimized for branch-likely.

		dec	<zx0_length + 1		; This is rare, so slower.
		bra	.lz_byte

		;
		; Copy bytes from compressed source.
		;

.cp_literals:	bsr	.get_gamma_flag		; Get length, returns CS.

		ldy	<zx0_length + 0		; Check the lo-byte of length
		bne	.cp_byte		; without effecting CS.

.cp_page:	dec	<zx0_length + 1

.cp_byte:	lda	[zx0_srcptr]		; Copy bytes from compressed
		sta	[zx0_dstptr]		; data to decompressed data.
		sta	VDC_DL, x
		txa
		eor	#1
		tax

		inc	<zx0_srcptr + 0
		beq	.inc_cp_src
.cp_skip1:	inc	<zx0_dstptr + 0
		beq	.inc_cp_dst

.cp_skip2:	dey				; Any bytes left to copy?
		bne	.cp_byte

		lda	<zx0_length + 1		; Any pages left to copy?
		bne	.cp_page		; Optimized for branch-unlikely.

		iny				; Initialize length back to 1.
		sty	<zx0_length + 0

		lda	<zx0_bitbuf		; Restore bit-buffer.

		asl	a			; Copy from last offset or new offset?
		bcs	.new_offset

		;
		; Copy bytes from last offset (rare so slower).
		;

.old_offset:	bsr	.get_gamma_flag		; Get length, returns CS.

		ldy	<zx0_length + 0		; Check the lo-byte of length.
		bne	.get_lz_win

		dec	<zx0_length + 1		; Decrement because lo-byte=0.
		bra	.get_lz_win

		;
		; All done!
		;

.got_eof:	tma3				; Return final MPR3 in Y reg.
		tay

		pla				; Restore MPR3.
		tam3

		leave				; Finished decompression!

		;
		; Optimized handling of pointers crossing page-boundaries.
		;

.inc_off_src:	jsr	inc.h_bp_mpr3
		bra	.off_skip1

.inc_lz_dst:	bsr	.next_dstpage
		bra	.lz_skip2

.inc_cp_src:	jsr	inc.h_bp_mpr3
		bra	.cp_skip1

.inc_cp_dst:	bsr	.next_dstpage
		bra	.cp_skip2

.inc_lz_win:	lda	<zx0_winptr + 1
		inc	a
		and	#ZX0_WINMSK
		ora	#ZX0_WINBUF
		sta	<zx0_winptr + 1
		bra	.lz_skip1

.next_dstpage:	lda	<zx0_dstptr + 1
		inc	a
		and	#ZX0_WINMSK
		ora	#ZX0_WINBUF
		sta	<zx0_dstptr + 1
		rts

.gamma_page:	jsr	inc.h_bp_mpr3
		bra	.gamma_skip1

		;
		; Get 16-bit interlaced Elias gamma value.
		;

.get_gamma_data:asl	a			; Get next bit.
		rol	<zx0_length + 0
.get_gamma_flag:asl	a
		bcc	.get_gamma_data		; Loop until finished or empty.
		bne	.gamma_done		; Bit-buffer empty?

.gamma_reload:	lda	[zx0_srcptr]		; Reload the empty bit-buffer
		inc	<zx0_srcptr + 0		; from the compressed source.
		beq	.gamma_page
.gamma_skip1:	rol	a
		bcs	.gamma_done		; Finished?

.get_gamma_loop:asl	a			; Get next bit.
		rol	<zx0_length + 0
		rol	<zx0_length + 1
		asl	a
		bcc	.get_gamma_loop		; Loop until finished or empty.
		beq	.gamma_reload		; Bit-buffer empty?

.gamma_done:	sta	<zx0_bitbuf		; Preserve bit-buffer.
		rts


set_bp_to_mpr3:
		lda.h	<_bp			; Do not remap a ptr to RAM,
		cmp	#$60			; which is $2000-$5FFF.
		bcc	!+
		and	#$1F			; Remap ptr to MPR3.
		ora	#$60
		sta.h	<_bp
		tya				; Put bank into MPR3.
		tam3
!:		rts

set_bp_to_mpr34:
		lda.h	<_bp			; Do not remap a ptr to RAM,
		cmp	#$60			; which is $2000-$5FFF.
		bcc	!+
		and	#$1F			; Remap ptr to MPR3.
		ora	#$60
		sta.h	<_bp
		tya				; Put bank into MPR3.
		tam3
		inc	a			; Put next into MPR4.
		tam4
!:		rts

inc.h_bp_mpr3:	
		inc.h	<_bp			; Increment hi-byte of _bp.
		bpl	!+			; OK if within MPR0-MPR3.
		pha				; Increment the bank in MPR3,
		tma3				; usually when pointer moves
		inc	a			; from $7FFF -> $8000.
		tam3
		lda	#$60
		sta.h	<_bp
		pla
!:		rts

set_di_to_mawr
		;lda	#VDC_MAWR		; Set VDC or SGX destination
		stz	<vdc_reg, x		; address.
		stz	VDC_AR, x

!:		lda	<_di + 0
		sta	VDC_DL, x
		lda	<_di + 1
		sta	VDC_DH, x

		lda	#VDC_VWR		; Select the VRR/VWR data
		sta	<vdc_reg, x		; register.
		sta	VDC_AR, x
		rts

#endasm
}

