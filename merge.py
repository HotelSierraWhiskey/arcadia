from intelhex import IntelHex


OUTFILE = "build/unified.hex"

if __name__ == "__main__":
	ih1 = IntelHex("bootloader/build/bootloader.hex")
	ih2 = IntelHex("app/build/app.hex")

	ih1.merge(ih2, overlap='error')

	ih1.write_hex_file(OUTFILE)