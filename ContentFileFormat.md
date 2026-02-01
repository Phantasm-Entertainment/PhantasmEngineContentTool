Header (8 bytes)
File version (4 bytes)

Atlas pages (2 bytes)
data for each atlas page:
Atlas width (2 bytes)
Atlas height (2 bytes)

Texture list size (4 bytes uint32)
data for each texture:
Name length (1 byte)
Width (2 bytes)
Height (2 bytes)
Page index (2 bytes)
Page X (2 bytes)
Page Y (2 bytes)
Name (variable length)
Image data (variable length, is Width * Height * 4)

Font list size (4 bytes uint32)
data for each font:
Name length (1 byte)
Name (variable length)
Character count (2 bytes uint16)
data for each character: