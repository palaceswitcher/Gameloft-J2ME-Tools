# Diamond Rush Graphics Format

by PalaceSwitcher (2025-2-18)

Graphics are typically represented as uncompressed, indexed bitmaps with palettes that can be one of 4 color formats: ARGB8888, ARGB4444, ARGB1555, or RGB565. The bitmaps themselves can be either 1-bit, 2-bits, 4-bits or 8-bits in depth. The size of the palette is dependent on the bit depth of the bitmap, with the amount of colors per palette being 2 to the power of the bit width.

## Header

`0x0` - `0x5`: Signature? Seems to be ignored. Is always `DF 03 01 01 01 01`.  
`int16 Sprite Dimension Count`: Amount of sprite resolutions.  
`int8[Sprite Dimension Count][2] Dimensions`: List of width and height pairs in that order.  
`int16 Sprite Count`: Amount of sprites defined  
`int8[Sprite Count][4]`: Sprite definitions.

### Sprite Definition

- `0x0`: Dimensions
- `0x1`: X Position in image
- `0x2`: Y Position in image
- `0x3`: Transformation flags. Bit 0 is for vertical mirroring and bit 1 is for horizontal mirroring