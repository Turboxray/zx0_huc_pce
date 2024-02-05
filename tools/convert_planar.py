
import argparse
import sys
from PIL import Image   # 'pip install pillow' needed to install PIL library

class ConvertImage():

    def __init__(self, args):
        self.args = args
        self.switch = { 'Sprite'     : self.processSPR,
                        'tile_8x8'   : self.processTLE8,
                        'tile_16x16' : self.processTLE16
                      }
        self.extName = { 'Sprite'     : '_spr16',
                         'tile_8x8'   : '_tle8',
                         'tile_16x16' : '_tle16'
                       }

        self.planarName  = ('planar',args.fileout)[args.fileout != ''] + self.extName[args.type] + '.bin'
        self.linearName  = ('linear',args.fileout)[args.fileout != ''] + self.extName[args.type] + '.bin'


    def process(self):

        try:
            self.img = Image.open(self.args.filein)
            self.img_arr = self.img.load()

            self.printStats(f'Source Image: {self.args.filein}')

            if self.img.mode != "P":
                return False

            self.printStats(f'Image type: {self.img.mode}')
            self.printStats(f'Image width: {self.img.width}')
            self.printStats(f'Image height: {self.img.height}')

            planar, linear = self.switch[args.type]()

            self.printStats(f'Saving planar file: {self.planarName}. ')
            with open(self.planarName,'wb') as fout:
                fout.write(bytearray(planar))

            if self.args.linear:
                self.printStats(f'Saving linear file: {self.linearName}. ')
                with open(self.linearName,'wb') as fout:
                    fout.write(bytearray(linear))
    
            self.printStats(f'\nDone.\n\n')

        except Exception as e:
            print(e)
            return False

        return True        


    def printStats(self, msg):
        if self.args.silence:
            return
        print(msg)

    def processSPR(self):
        spr_planar_array = []
        spr_linear_array = []

        self.printStats(f'Processing for sprites. ')

        spr_height = (self.img.height // 16) * 16
        spr_width  = (self.img.width // 16) * 16

        for cell_row in range(0, spr_height, 16):
            for cell_col in range(0, spr_width, 16):
                spr_planar, spr_linear = self.getSprite(cell_row, cell_col)
                spr_planar_array += spr_planar        
                spr_linear_array += spr_linear        

        return spr_planar_array, spr_linear_array

    def processTLE8(self):
        tile_planar_array = []
        tile_linear_array = []

        self.printStats(f'Processing for tile size 8x8. ')

        tiles_height = (self.img.height // 8) * 8
        tiles_width  = (self.img.width // 8) * 8

        for cell_row in range(0, tiles_height, 8):
            for cell_col in range(0, tiles_width, 8):
                tile_planar, tile_linear = self.getMetaTile_8x8(cell_row, cell_col)
                tile_planar_array += tile_planar        
                tile_linear_array += tile_linear        

        return tile_planar_array, tile_linear_array

    def processTLE16(self):

        tile_planar_array = []
        tile_linear_array = []

        self.printStats(f'Processing for tile size 16x16. ')

        tiles_height = (self.img.height // 16) * 16
        tiles_width  = (self.img.width // 16) * 16

        for cell_row in range(0, tiles_height, 16):
            for cell_col in range(0, tiles_width, 16):
                tile_planar, tile_linear = self.getMetaTile_16x16(cell_row, cell_col)
                tile_planar_array += tile_planar        
                tile_linear_array += tile_linear        

        return tile_planar_array, tile_linear_array


    def getSprite(self, cell_row, cell_col):

        metaListPlanar  = []
        metaListPLinear = []
        planar, linear = self.getSpriteCellData(cell_col, cell_row)
        metaListPlanar = metaListPlanar + planar
        metaListPLinear = metaListPLinear + linear

        return metaListPlanar, metaListPLinear


    def getMetaTile_8x8(self, cell_row, cell_col):

        metaListPlanar  = []
        metaListPLinear = []
        planar, linear = self.getTileCellData_8x8(cell_col, cell_row)
        metaListPlanar = metaListPlanar + planar
        metaListPLinear = metaListPLinear + linear

        return metaListPlanar, metaListPLinear


    def getMetaTile_16x16(self, cell_row, cell_col):

        metaListPlanar  = []
        metaListPLinear = []
        planar, linear = self.getTileCellData_8x8(cell_col, cell_row)
        metaListPlanar = metaListPlanar + planar
        metaListPLinear = metaListPLinear + linear

        planar, linear = self.getTileCellData_8x8(cell_col + 8, cell_row)
        metaListPlanar = metaListPlanar + planar
        metaListPLinear = metaListPLinear + linear

        planar, linear = self.getTileCellData_8x8(cell_col, cell_row + 8)
        metaListPlanar = metaListPlanar + planar
        metaListPLinear = metaListPLinear + linear

        planar, linear = self.getTileCellData_8x8(cell_col + 8, cell_row + 8)
        metaListPlanar = metaListPlanar + planar
        metaListPLinear = metaListPLinear + linear

        return metaListPlanar, metaListPLinear


    def getTileCellData_8x8(self, col_offset, row_offset):

        pixel_arr = []
        planar_arr = [0] * 32
        linear_arr = []

        # get individual 8x8 cell
        for row in range(8):
            for col in range(8):
                y_offset = row_offset + row
                x_offset = col_offset + col
                pixel_arr.append(self.img_arr[x_offset,y_offset])

        # # convert cell into planar format
        for row in range(8):
            for col in range(8):
                offset = (row*8) + col
                p = pixel_arr[offset]

                planar_arr[(row * 2) + 0 ] <<= 1
                planar_arr[(row * 2) + 0 ] |= p & 0x01
                p >>= 1
                planar_arr[(row * 2) + 1 ] <<= 1
                planar_arr[(row * 2) + 1 ] |= p & 0x01
                p >>= 1
                planar_arr[(row * 2) + 16 ] <<= 1
                planar_arr[(row * 2) + 16 ] |= p & 0x01
                p >>= 1
                planar_arr[(row * 2) + 17 ] <<= 1
                planar_arr[(row * 2) + 17 ] |= p & 0x01
                p >>= 1

        # # convert cell into linear format
        for row in range(8):
            for col in range(0,8,2):
                offset = (row*8) + col
                p0 = pixel_arr[offset+0] & 0x0f
                p1 = pixel_arr[offset+1] & 0x0f
                linear_arr.append((p0<<4) | p1)

        return planar_arr, linear_arr


    def getSpriteCellData(self, col_offset, row_offset):

        pixel_arr = []
        planar_arr = [0] * 256
        linear_arr = []

        # get individual 8x8 cell
        for row in range(16):
            for col in range(16):
                y_offset = row_offset + row
                x_offset = col_offset + col
                pixel_arr.append(self.img_arr[x_offset,y_offset])

        # # convert cell into planar format
        for row in range(16):
            for col in range(16):
                offset = (row*16) + col
                p = pixel_arr[offset]

                planar_arr[(row) + 0 ] <<= 1
                planar_arr[(row) + 0 ] |= p & 0x01
                p >>= 1
                planar_arr[(row) + 16 ] <<= 1
                planar_arr[(row) + 16 ] |= p & 0x01
                p >>= 1
                planar_arr[(row) + 32 ] <<= 1
                planar_arr[(row) + 32 ] |= p & 0x01
                p >>= 1
                planar_arr[(row) + 48 ] <<= 1
                planar_arr[(row) + 48 ] |= p & 0x01
                p >>= 1

        planar_arr = [[item & 0xff, (item>>8) & 0xff] for item in planar_arr]
        planar_arr = [j for sub in planar_arr for j in sub][0:128]

        # # convert cell into linear format
        for row in range(16):
            for col in range(0,16,2):
                offset = (row*16) + col
                p0 = pixel_arr[offset+0] & 0x0f
                p1 = pixel_arr[offset+1] & 0x0f
                linear_arr.append((p0<<4) | p1)

        return planar_arr, linear_arr

#.....................................
# END CLASS


def auto_int(val):
    val = int(val, (10,16)['0x' in val])
    return val

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='8bit Bitmap to PCE planar converter',
                                      formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    runOptionsGroup = parser.add_argument_group('Run options', 'Choose a type.')

    runOptionsGroup.add_argument('--filein',
                                 '-in',
                                 required=True,
                                 help='The source image. Must be an 8bit palettized image.')

    runOptionsGroup.add_argument('--fileout',
                                 '-out',
                                 default='',
                                 help='Override the default output name.')

    runOptionsGroup.add_argument('--linear',
                                 '-lin',
                                 action='store_true',
                                 help='Also output a linear 4bit binary format.')

    runOptionsGroup.add_argument('--silence',
                                 '-s',
                                 action='store_true',
                                 help='Silent mode.')

    runOptionsGroup.add_argument('--type',
                                 '-t',
                                 choices=['Sprite', 'tile_8x8', 'tile_16x16'],
                                 required=True,
                                 help='The type of conversion. Note: tile_16x16 is HuC metatile format.')


    args = parser.parse_args()

    sys.exit( ConvertImage(args).process() == False)
