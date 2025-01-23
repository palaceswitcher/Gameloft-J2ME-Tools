# Diamond Rush Graphics Format

by PalaceSwitcher (2025-1-20)

## Header

`0x0` - `0x5`: Signature? Seems to be ignored.  
`int16 Sizes`: How many image resolutions  
`int8[size][2] Dimensions`: Width and height pairs for each encoded image.
