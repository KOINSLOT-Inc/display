# SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
#
# SPDX-License-Identifier: GPL-3.0-or-later

from __future__ import annotations

from collections import OrderedDict

import os
import sys
import yaml

# This file is used to convert TTF fonts to compressed and encoded byte strings
# that can be used by the Display component.


# Hex encodes an unsigned integer, filling num_bytes bytes. E.g.
#   - unsigned(1, 1) == "01"
#   - unsigned(1, 2) == "0001"
#   - unsigned(178, 1) == "B2"
#   - unsigned(1788, 2) == "06FC"
def unsigned(data: int, num_bytes) -> int:
    return f"{data:0{num_bytes * 2}x}"


# Hex encodes a signed integer, filling num_bytes. E.g.
#   - X > 0 => signed(X, N) == unsigned(X, N)
#   - signed(-1, 1) == "FF"
#   - signed(-1, 2) == "FFFF"
#   - signed(-16, 2) == "FFF0"
#   - signed(-1788, 2) == "F904"
def signed(data: int, num_bytes) -> int:
    if data < 0:
        return f"{(2 ** (8 * num_bytes)) + data:0{num_bytes * 2}x}"
    return unsigned(data, num_bytes)


# iterate through a string of hex digits two at a time, i.e. 1 byte at a time
def iter_bytes(hex_data: str) -> str:
    nibble = iter(hex_data)
    while True:
        try:
            yield next(nibble) + next(nibble)
        except StopIteration:
            return ""


# Holds encoding information
class Encoding:
    offsets: OrderedDict = OrderedDict()


# Represents a character in a BDF font file
class Char:
    def __init__(self, encoding):
        self.encoding = encoding

        self.code: str  # char identifier

        self.device_width: tuple[
            int,  # pixel width
            int,  # pixel height
        ]

        self.bbx: tuple[  # bounding box around black pixels
            int,  # width
            int,  # height
            int,  # x offset from bottom left of bitmap
            int,  # y offset from bottom left of bitmap
        ]

        self.bitmap: list[list[int]]  # list of rows in binary

    # compresses and encodes the character into a hex string
    def encode(self) -> str:
        output = ""
        total_offset = 0

        # character code
        self.encoding.offsets["CHARACTER_CODE"] = total_offset
        self.encoding.offsets["CHARACTER_CODE_TYPE"] = "uint16_t"
        total_offset += 2
        output += self.code

        # bytes in encoded character
        self.encoding.offsets["CHARACTER_BYTES"] = total_offset
        self.encoding.offsets["CHARACTER_BYTES_TYPE"] = "uint16_t"
        total_offset += 2
        output += "0000"  # placeholder to be overwritten later

        # device width
        self.encoding.offsets["CHARACTER_DEVICE_WIDTH_X"] = total_offset
        self.encoding.offsets["CHARACTER_DEVICE_WIDTH_X_TYPE"] = "uint8_t"
        total_offset += 1
        output += unsigned(self.device_width[0], 1)

        self.encoding.offsets["CHARACTER_DEVICE_WIDTH_Y"] = total_offset
        self.encoding.offsets["CHARACTER_DEVICE_WIDTH_Y_TYPE"] = "uint8_t"
        total_offset += 1
        output += unsigned(self.device_width[1], 1)

        # bbx
        self.encoding.offsets["CHARACTER_BBX_WIDTH"] = total_offset
        self.encoding.offsets["CHARACTER_BBX_WIDTH_TYPE"] = "uint8_t"
        total_offset += 1
        output += unsigned(self.bbx[0], 1)

        self.encoding.offsets["CHARACTER_BBX_HEIGHT"] = total_offset
        self.encoding.offsets["CHARACTER_BBX_HEIGHT_TYPE"] = "uint8_t"
        total_offset += 1
        output += unsigned(self.bbx[1], 1)

        self.encoding.offsets["CHARACTER_BBX_X_OFFSET"] = total_offset
        self.encoding.offsets["CHARACTER_BBX_X_OFFSET_TYPE"] = "int8_t"
        total_offset += 1
        output += signed(self.bbx[2], 1)

        self.encoding.offsets["CHARACTER_BBX_Y_OFFSET"] = total_offset
        self.encoding.offsets["CHARACTER_BBX_Y_OFFSET_TYPE"] = "int8_t"
        total_offset += 1
        output += signed(self.bbx[3], 1)

        # bitmap
        self.encoding.offsets["CHARACTER_BITMAP"] = total_offset

        binary_bitmap = [pixel for row in self.bitmap for pixel in row]

        head = 0
        bits = binary_bitmap[head : head + 8]  # grab 8 bits at a time to encode
        while len(bits) > 0:
            bits = [str(bit) for bit in bits]

            if len(bits) < 8:  # right pad last byte with zeros if necessary
                bits += ["0"] * (8 - len(bits))

            byte = int("".join(bits), 2)
            output += f"{byte:02x}"
            head += 8
            bits = binary_bitmap[head : head + 8]

        # overwrite character bytes now that we have the full size
        output = output[0:4] + unsigned(int(len(output) / 2), 2) + output[8:]

        return output


# Represents a BDF font file
class Font:
    def __init__(self, encoding, file, name):
        self.name = name
        self.encoding = encoding

        self.size: int  # font size in pixels

        self.num_chars: int  # number of chars

        self.bounding_box: tuple[
            int,  # default font bounding box width
            int,  # default font bounding box height
            # offset from bottom left corner of bounding box as (0, 0)
            int,  # default x offset of the bottom left corner
            int,  # default y offset of the bottom left corner
        ]

        self.ascent: int  # pixels above the baseline
        self.descent: int  # pixels below the baseline

        self.copyright: str  # copyright for the font

        self.chars: list[Char] = []  # all characters in the font

        self.parse_bdf_file(file)

        # create a custom character to use when a character isn't available in the font
        missing_char_replacement = Char(encoding)
        missing_char_replacement.code = "0000"

        width = int(self.size / 2)
        height = int(self.size)

        missing_char_replacement.device_width = (width + 1, height)
        missing_char_replacement.bbx = (width, height, 0, 0)

        missing_char_replacement.bitmap = []
        for y in range(0, height):
            row = []

            for x in range(0, width):
                pixel = 0

                # edges
                if x == 0 or x == (width - 1) or y == 0 or y == (height - 1):
                    pixel = 1

                row.append(pixel)

            missing_char_replacement.bitmap.append(row)

        self.chars.insert(0, missing_char_replacement)

    # compresses and encodes the font into a hex string
    def encode(self) -> str:
        output = ""
        total_offset = 0

        # size
        self.encoding.offsets["FONT_SIZE"] = total_offset
        self.encoding.offsets["FONT_SIZE_TYPE"] = "uint8_t"
        total_offset += 1
        output += unsigned(self.size, 1)

        # characters
        self.encoding.offsets["FONT_CHARACTERS"] = total_offset
        self.encoding.offsets["FONT_CHARACTERS_TYPE"] = "uint16_t"
        total_offset += 2
        output += unsigned(self.num_chars, 2)

        # bounding box
        self.encoding.offsets["FONT_BOUNDING_BOX_WIDTH"] = total_offset
        self.encoding.offsets["FONT_BOUNDING_BOX_WIDTH_TYPE"] = "uint8_t"
        total_offset += 1
        output += unsigned(self.bounding_box[0], 1)

        self.encoding.offsets["FONT_BOUNDING_BOX_HEIGHT"] = total_offset
        self.encoding.offsets["FONT_BOUNDING_BOX_HEIGHT_TYPE"] = "uint8_t"
        total_offset += 1
        output += unsigned(self.bounding_box[1], 1)

        self.encoding.offsets["FONT_BOUNDING_BOX_X_OFFSET"] = total_offset
        self.encoding.offsets["FONT_BOUNDING_BOX_X_OFFSET_TYPE"] = "int8_t"
        total_offset += 1
        output += signed(self.bounding_box[2], 1)

        self.encoding.offsets["FONT_BOUNDING_BOX_Y_OFFSET"] = total_offset
        self.encoding.offsets["FONT_BOUNDING_BOX_Y_OFFSET_TYPE"] = "int8_t"
        total_offset += 1
        output += signed(self.bounding_box[3], 1)

        # ascent
        self.encoding.offsets["FONT_ASCENT"] = total_offset
        self.encoding.offsets["FONT_ASCENT_TYPE"] = "uint8_t"
        total_offset += 1
        output += unsigned(self.ascent, 1)

        # descent
        self.encoding.offsets["FONT_DESCENT"] = total_offset
        self.encoding.offsets["FONT_DESCENT_TYPE"] = "uint8_t"
        total_offset += 1
        output += unsigned(self.descent, 1)

        # characters
        self.encoding.offsets["FONT_FIRST_CHARACTER"] = total_offset
        for char in self.chars:
            output += char.encode()

        return output

    # parses a BDF font file to populate the fields in this class
    def parse_bdf_file(self, file: str):
        current_char = Char(self.encoding)
        current_bitmap = []
        reading_bitmap = False

        with open(file) as lines:
            for line in lines:
                if line.upper().startswith("ENDCHAR"):
                    reading_bitmap = False
                    current_char.bitmap = current_bitmap
                    self.chars.append(current_char)
                    continue

                if reading_bitmap:
                    current_bitmap.append(
                        list(
                            map(
                                int,
                                bin(int(line, 16))[2:].zfill(4 * (len(line) - 1)),
                            )
                        )[: current_char.bbx[0]]
                    )
                    continue

                if line.upper().startswith("SIZE "):
                    self.size = int(line.split()[1])
                elif line.upper().startswith("FONTBOUNDINGBOX "):
                    self.bounding_box = list(map(int, line.split()[1:]))
                elif line.upper().startswith("FONT_ASCENT "):
                    self.ascent = int(line.split()[1])
                elif line.upper().startswith("FONT_DESCENT "):
                    self.descent = int(line.split()[1])
                elif line.upper().startswith("COPYRIGHT "):
                    self.copyright = line[11:-1]
                elif line.upper().startswith("CHARS "):
                    self.num_chars = int(line.split()[1])
                elif line.upper().startswith("STARTCHAR "):
                    current_char = Char(self.encoding)
                    current_char.code = line.split()[1]
                elif line.upper().startswith("DWIDTH "):
                    current_char.device_width = tuple(map(int, line.split()[1:]))
                elif line.upper().startswith("BBX "):
                    current_char.bbx = tuple(map(int, line.split()[1:]))
                elif line.upper().startswith("BITMAP"):
                    current_bitmap = []
                    reading_bitmap = True


# creates the header and source file code for a font
def generate_encoded_font_code(font: Font) -> (str, str):
    hex_data = font.encode()

    font_variable_name = f"{font.name}_{font.size}_pt"
    font_array_size = int(len(hex_data) / 2) + 1

    docstring = (
        f"// {font.name}\n"
        f"//   - {font.copyright}\n"
        f"//   - Size: {font.size}\n"
        f"//   - Characters: {font.num_chars}\n"
    )

    header = docstring + f"extern uint8_t {font_variable_name}[{font_array_size}];\n"

    source = docstring + f"uint8_t {font_variable_name}[{font_array_size}] ="

    # add `\x` prefix to all hex nibble pairs
    formatted_bytes = ""
    for byte in iter_bytes(font.encode()):
        formatted_bytes += "\\x" + byte

    # split hex data into lines
    line_length = 20
    lines = [
        formatted_bytes[i : i + (line_length * 4)]
        for i in range(0, len(formatted_bytes), line_length * 4)
    ]

    for line in lines:
        source += f'\n  "{line}"'

    source += ";\n"

    return header, source


if __name__ == "__main__":
    with open("fonts.yaml") as f:
        font_configs = yaml.safe_load(f)["fonts"]

    encoding = Encoding()
    fonts = []

    for font_config in font_configs:
        for size in font_config["size"]:
            bdf_file = f".make-cache/bdf-fonts/{font_config['name']}/{font_config['name']}.{size}.bdf"
            fonts.append(Font(encoding, bdf_file, font_config["name"]))

    header_blocks = []
    source_blocks = []
    for font in fonts:
        font_header, font_source = generate_encoded_font_code(font)
        header_blocks += ["\n" + font_header]
        source_blocks += ["\n" + font_source]

    header = (
        "// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.\n"
        "//\n"
        "// SPDX-License-Identifier: GPL-3.0-or-later\n"
        "\n"
        "#pragma once\n"
        "\n"
        '#include "esp_types.h"\n'
        "\n"
        "namespace Display::Font {\n"
    )

    for offset, value in encoding.offsets.items():
        header += f"\n#define {offset} {value}"
    header += "\n"
    header += "".join(header_blocks)

    source = (
        "// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.\n"
        "//\n"
        "// SPDX-License-Identifier: GPL-3.0-or-later\n"
        "\n"
        '#include "Fonts.hpp"\n'
        "\n"
        "namespace Display::Font {\n"
    )
    source += "".join(source_blocks)

    header += "\n" + "} // namespace Display::Font"
    source += "\n" + "} // namespace Display::Font"

    if not os.path.isdir("include/generated"):
        os.mkdir("include/generated")

    with open("include/generated/Fonts.hpp", "w+") as f:
        f.write(header)

    if not os.path.isdir("src/generated"):
        os.mkdir("src/generated")

    with open("src/generated/Fonts.cpp", "w+") as f:
        f.write(source)
