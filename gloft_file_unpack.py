#!/usr/bin/env python3
# Gameloft J2ME File Unpacker by PalaceSwitcher
import argparse
import sys
import io
from pathlib import Path
import typing

VERSION="v1.1 (2025-1-22)"

# Handle command line arguments
parser = argparse.ArgumentParser()
parser.add_argument("-d", "--newdir",
					help="Put unpacked files in separate directory",
                    action="store_true")
parser.add_argument("-f", "--format", metavar="FORMAT",
					help="Pack format (standard by default). Formats: s (Standard), alt (Demo), ad (More Games), txt (Text Pack)",
                    action="store", default="s", choices=["s","alt","ad","txt"])
parser.add_argument("-n", "--filenum", metavar="NUMBER",
					help="Which file or block from the pack to get. -1 retrieves all files/blocks.",
                    action="store", default=0)
parser.add_argument("-e", "--file-ext",
					help="File extension automatically given to each output file. Blank by default",
                    action="store", default="")
parser.add_argument("filename", help="Binary file to unpack")
args = parser.parse_args()

# Misc functions
def get_int_from_bytes(bytes: bytearray, index: int = 0) -> int:
	return bytes[index] & 255 | (bytes[index + 1] & 255) << 8 | (bytes[index + 2] & 255) << 16 | (bytes[index + 3] & 255) << 24
def get_short_from_bytes(bytes: bytearray, index: int = 0, ) -> int:
	return bytes[index] & 255 | (bytes[index + 1] & 255) << 8
def str_from_bytes(bytes):
	bytes = bytearray(bytes)
	end = bytes.find(0, 1)
	quot = bytes.find(0x92, 1) #Quotation mark index
	if quot != -1:
		bytes[quot] = 0x27 #Replace with ASCII quotation mark
	if end != -1:
		return bytes[0:end].decode("latin-1")
	else:
		return bytes[0:].decode("latin-1")

# Drag and drop functionality
try:
	filename: str = sys.argv[1]
except IndexError:
	#filename: str = input("Filename: ")
	parser.print_help()
	sys.exit(1)

# Get format
format = args.format

# File error handling
while True:
	try:
		f = open(filename, "rb") #Get file
		break
	except FileNotFoundError as not_found:
		print("File \""+not_found.filename+"\" not found!")
		filename: str = input("Filename: ")

data: bytearray = f.read() #Read file
file_size = len(data)
file_stream = io.BytesIO(data)

# Get correct offset for mode and get file count
if format == "s":
	file_count = int.from_bytes(file_stream.read(1))
elif format == "alt":
	file_count = get_int_from_bytes(file_stream.read(4))
elif format == "ad":
	file_count = get_short_from_bytes(file_stream.read(2))
header = bytearray() #Create header
file_offsets: list[int] = [] #File offsets
file_sizes: list[int] = [] #File sizes

files_data: list[bytearray] = [] #Byte arrays for each file

# Get file offsets and sizes
if format == "s":
	for i in range(file_count):
		file_offsets.append(get_int_from_bytes(file_stream.read(4)))
		file_sizes.append(get_int_from_bytes(file_stream.read(4)))
elif format == "ad":
	for i in range(file_count):
		file_offsets.append(get_int_from_bytes(file_stream.read(4)))
elif format == "alt":
	file_offsets.append(0) #The first offset is assumed to be zero
	for i in range(file_count):
		file_offsets.append(get_int_from_bytes(file_stream.read(4)))
	for i in range(file_count):
		file_sizes.append(file_offsets[i+1] - file_offsets[i])

# Load and separate files in payload
if format == "s":
	for i in range(file_count):
		files_data.append(file_stream.read(file_sizes[i]))
elif format == "ad":
	for i in range(file_count):
		file_size = get_short_from_bytes(file_stream.read(2))
		files_data.append(file_stream.read(file_size))
if format == "alt":
	file_stream.read(1) #Skip initial byte
	for i in range(file_count):
		files_data.append(file_stream.read(file_sizes[i]))
elif format == "txt":
	txt_string_sizes: list[int] = []
	txt_string_indices: list[int] = []
	txt_string_size = get_short_from_bytes(file_stream.read(2))
	txt_string_block: bytearray = file_stream.read(txt_string_size) #String data
	txt_table_size = file_size - file_stream.tell() #Size of text index table
	for i in range(0, txt_table_size//2, 2):
		txt_string_sizes.append(get_short_from_bytes(file_stream.read(2)))
	for i in range(0, txt_table_size//2, 2):
		txt_string_indices.append(get_short_from_bytes(file_stream.read(2)))

	# Copy strings to files
	file_count = len(txt_string_sizes) - 1
	for i in range(file_count):
		files_data.append(str_from_bytes(txt_string_block[txt_string_indices[i]:txt_string_indices[i]+txt_string_sizes[i]]))

# Output files
for i in range(file_count):
	filename_path = Path(filename) #Filename as a Pack object
	file_ext = ""
	if args.file_ext != "":
		file_ext = "."+args.file_ext
	o = open(f"{filename_path.with_suffix("")}_{str(i)}{file_ext}", "wb") #Create output filename
	o.write(files_data[i]) #Output file

print("Success! "+str(file_count)+" file(s) unpacked.")
