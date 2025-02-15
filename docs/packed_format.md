# GameLoft J2ME Packed File Format

by PalaceSwitcher

2025-2-15

This has only been tested with Diamond Rush and Rayman Kart. These are likely applicable to other games but this has not been verified. All numbers specified are little-endian, if applicable.

## Offset Format

This format contains packed files within a packed file and doesn't keep track of file sizes, instead calculating them based on the difference between offsets.

### Offset Format Header

`int16 File Count`: Amount of packed files. The actual amount of files is often one less than this as the last offset is at the end of the file and does not represent an actual file.  
`int32[File Count] Offsets`: Start offsets of each file relative to the start of the payload.

## Alternate Offset Format

A less common variant of this format store the file count as a 32-bit integer and has a single terminator byte (usually `00`) after the header. File offsets also represent the end offset of each file rather than the start offset, so the number of files is the same as the number of offsets.

## Minimal Size Format

This is the most barebones format, featuring no header and instead having each file stored sequentially preceeded by a 16-bit integer representing their size. So far, this format has only been found nested within the normal offset in specific versions of Diamond Rush for the "More Games" section's data.

## Offset and Size Format

This is the most commonly used format. It stores both the sizes and offsets for each file and uses one byte to represent the header.

### Offset and Size Format Header

`int8 File Count`: Amount of packed files.  
`int32[File Count][2] File Metadata`: File offset and size pairs in that order for each file. File offsets are relative to the start of the payload.

## Text Pack Format

Text packs start with a short representing the length of the text data itself, and the rest of the file is used to store the string index table. Strings are defined by the string index table, which has the offset and size of each string in the string table.

### Text Pack Header

`int16 String Table Size`: Size of the string table in bytes.  
`char[String Table Size] String Table`: The table of null-terminated strings.  
`int16 String Index Table Size`: Size of the string index table in bytes.  
`int16[] String Index Table`: The indices for each string in the table.

#### String Table

The string table contains a header specifying how many bytes it takes up followed by an array of shorts representing the start index for each string. Strings are null-terminated and are encoded in ISO/IEC 8859-1, with the exception of the single quote being encoded by character 0x92. They seem to use some control characters

#### String Index Table

The first half of the table has the sizes of each string as a short, and the other half has the index of each string excluding the header.
