
#include "huc.h"
#include "HuC_zx0/zx0_support.h"
#include "getFarPointer.h"

#asm
.bank HUC_USER_RESERVED
#endasm

#incbin(test, "__vram_zx0_2048w.bin");

// Note: We still need the original PNG file, but not for tiles. For map and palette info.
#incpal(testpal,"test_set_1.png");
#inctilepal(test_pmap,"test_set_1.png");
#incbin(testmap,"test_set_1.stm");

// This is a pointer "container". It doesn't matter the name or any other fields,
// but it needs a .bank char array and a .addr int array.
typedef struct {
    char idx;       //current open slot
    char bank[20];
    int  addr[20];
} PointerContainer;
PointerContainer c_Pointers;

//###############################################################################
//###############################################################################
// Main                                                                         #
//###############################################################################

int main()
{
    // var setup and init start 
    int i,j,k;

    set_xres(344);
    cls();
    load_default_font();
    disp_on();

    loadPointers();

    // Three examples below. One takes a label, one takes bank and an address, and one takes a pointer container.

    // E.g 1)
    // Supply a bank and ptr manually
    // decomp_zx0_vram_base(c_Pointers.bank[0],c_Pointers.addr[0], 0x2000);

    // E.g 2)
    // Supply a direct label
    // decomp_zx0_vram_direct(test, 0x2000);

    // E.g 3)
    // Supply a far container at entry 0
    vsync();
    decomp_zx0_vram(c_Pointers, 0, 0x2000);
    i = vsync()-1;    // Benchmark how long it took to decompress in "vblank" frames
    j= (i * 1000) / 60;


    // associate the tilemap attributes
	set_map_data(testmap, 20, 8);
    set_map_pals(test_pmap);
    set_map_tile_type(16);

	load_palette(0, testpal, 1);

    // set the tilemap to point to the correct tileset in vram
    set_map_tile_base(0x2000);
    // display the original PNG image
    load_map(0,0,0,0,20,8);


    put_string("ZX0 test.",4,17);
    put_string("Time to decompress..", 4, 19);
    put_string("number of frames:", 9, 21);
    put_number(i, 3,26,21);
    put_string("seconds: 0.", 9, 22);
    put_number(j, 3,20,22);

    for(;;) { vsync(); }

    return 0;
}

//###############################################################################
//###############################################################################
// Subs                                                                         #
//###############################################################################



//...............................................................................
//...............................................................................
//
void loadPointers() {
    c_Pointers.idx = 0;
    getFarPointer(test, c_Pointers, c_Pointers.idx);
    c_Pointers.idx++;
}

//...............................................................................
//...............................................................................
//

