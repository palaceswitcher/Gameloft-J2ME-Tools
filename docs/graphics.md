# Diamond Rush Graphics Format

by PalaceSwitcher (2025-1-23)

Graphics are typically represented as uncompressed, indexed bitmaps with palettes that can be one of 4 color formats: ARGB8888, ARGB4444, ARGB1555, or RGB565. The bitmaps themselves can be either 1-bit, 2-bits, 4-bits or 8-bits in depth. The size of the palette is dependent on the bit depth of the bitmap, with the amount of colors per palette being 2 to the power of the bit width.

## Header

`0x0` - `0x5`: Signature? Seems to be ignored.  
`int16 Sizes`: How many image resolutions  
`int8[size][2] Dimensions`: Width and height pairs for each encoded image.
