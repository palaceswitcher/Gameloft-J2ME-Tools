# Diamond Rush Graphics Format

by PalaceSwitcher (2025-2-21)

Graphics are typically represented as optionally compressed, indexed bitmaps with palettes that can be one of 4 color formats: ARGB8888, ARGB4444, ARGB1555, or RGB565. The bitmaps themselves can be either 1-bit, 2-bits, 4-bits or 8-bits in depth, and can either be stored raw or compressed via RLE (7-bit or 8-bit only). The size of the palette is dependent on the bit depth of the bitmap, with the amount of colors per palette being 2 to the power of the bit width.

## Header

`0x0` - `0x5`: Signature? Seems to be ignored. Is always `DF 03 01 01 01 01`.  
`int16 Sprite Dimension Count`: Amount of sprite resolutions.  
`int8[Sprite Dimension Count][2] Dimensions`: List of width and height pairs in that order.  
`int16 Sprite Count`: Amount of sprites defined  
`int8[Sprite Count][4] Sprite Definitions`: Sprite definitions.

### Sprite Definition

- `0x0`: Dimensions Index
- `0x1`: X Position in image
- `0x2`: Y Position in image
- `0x3`: Transformation flags. Bit 0 is for vertical mirroring and bit 1 is for horizontal mirroring

`int16 Unknown Count`:  
`int16[Unknown Count][2]`: Unknown, first member of struct is treated as a padded int8  
`int16 Unknown Count 2`:  
`int8[Unknown Count 2][5]`:  
`int16 Unknown Count 3`:  
`int16[Unknown Count 3][2]`: Unknown, first member of struct is treated as a padded  
`int16 Palette Format`: Color format of the palette  

### Palette Format Codes

`0x8888`: ARGB8888  
`0x4444`: ARGB4444  
`0x5515`: ARGB1555  
`0x6505`: RGB565  

`int8 Palette Count`: Number of palettes, can be up to 16.  
`int8 Palette Size`: Palette size.  
`int16 or int32[16][Palette Size] Palette Data`: Palette data, stores 16 blank palettes by default. Is only 32-bit if the palette is ARGB8888  
`int8[16][Palette Count] Palette Data`: .

`int16 Bit Depth`: Sprite(?) bitmap format.

### Bit Format Codes

`0x1600`: 4-bit (Uncompressed)  
`0x0400`: 2-bit (Uncompressed)  
`0x0200`: 1-bit (Uncompressed)  
`0x5602`: 8-bit (Uncompressed)  
`0x56F2`: 8-bit (RLE Compressed)
`0x27F1`: 7-bit (RLE Compressed)
.


## Bitmap Format

Bitmaps are palette-indexed, and can be 1-bit, 2-bit, 4-bit, or 8-bit in depth. They can be either raw or compressed via RLE, with RLE compression being exclusive to the 8-bit format.

## RLE Format 1

RLE consists of sets of repeat tags occasionally interrupted by "stop" tags that have a specified length of uncompressed bytes. The only distinction between the two is that "stop" tags have their 7th bit set, so a certain value can only repeat with one tag 127 times and only 127 uncompressed bytes can repeat after a tag. The count values for both is equal to the tag value itself, minus the 7th bit.

### RLE Format 1 Example

Compressed: `04 1C 82 23 22`  
Uncompressed: `1C 1C 1C 1C 23 22`

## RLE Format 2

This is essentially a more limited, inverted version of the previous format. All bytes are assumed to be uncompressed, and are interrupted by occasional RLE tags that have their 7th bit set. This does not allow values over 128 for bitmap data, since they are treated as RLE tags, so this format effectively makes bitmaps 7-bit.

### RLE Format 2 Example

Compressed: `14 5A 28 85 34 06`  
Compressed: `14 5A 28 34 34 34 34 34 06`
