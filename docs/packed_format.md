# GameLoft J2ME Packed File Format

by PalaceSwitcher

2025-2-17

This has only been tested with Diamond Rush and Rayman Kart. These are likely applicable to other games but this has not been verified. All numbers specified are little-endian, if applicable.

## Offset Format

This format contains packed files within a packed file and doesn't keep track of file sizes, instead calculating them based on the difference between offsets.

### Offset Format Header

`int16 File Count`: Amount of packed files. The actual amount of files is often one less than this as the last offset is at the end of the file and does not represent an actual file.  
`int32[File Count] Offsets`: Start offsets of each file relative to the start of the payload.

## Alternate Offset Format

A less common variant of this format store the file count as a 32-bit integer and has a single terminator byte (usually `00`) after the header. File offsets also represent the end offset of each file rather than the start offset, so the number of files is the same as the number of offsets.

## Minimal Size Format

This is the most barebones format, featuring no header and instead having each file stored sequentially preceeded by a 16-bit integer representing their size. So far, this format has only been found nested within normal offset packs in specific versions of Diamond Rush for the "More Games" section's data.

## Offset and Size Format

This is the most commonly used format. It stores both the sizes and offsets for each file and uses one byte to represent the header.

### Offset and Size Format Header

`int8 File Count`: Amount of packed files.  
`int32[File Count][2] File Metadata`: File offset and size pairs in that order for each file. File offsets are relative to the start of the payload.

## Text Pack Format

Text packs are stored as two files packed in the [minimal pack format](#minimal-size-format), with the first file being the string table, which stores the raw string data, and the second file being the index table, which stores the indexes of each string in the table.

### String Table

The string table contains a header specifying how many bytes it takes up followed by an array of shorts representing the start index for each string. Strings are null-terminated and are encoded in ISO/IEC 8859-1, with the exception of the single quote being encoded by character 0x92. They seem to use some control characters

### String Index Table

#### String Index Table Format

`int16 String Count`: Amount of strings.  
`int16[String Count] String Indices`:Indices of each string.

The string table contains the number of strings in the string table followed by the starting index of every string. The first string is assumed to be at index 0, and the last index typically points to the end of the string table.
