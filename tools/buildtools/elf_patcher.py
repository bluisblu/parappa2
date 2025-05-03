#!/usr/bin/env python3

# SPDX-FileCopyrightText: © 2024 AngheloAlf
# SPDX-License-Identifier: MIT

from __future__ import annotations

import argparse
import spimdisasm
from pathlib import Path
import struct

def align_up(x: int, a: int) -> int:
    return (x + (a-1)) & ~(a-1)

SECTIONS_TO_REALIGN_PER_TOOL: dict[str, dict[str, int]] = {
    "gas": {
        ".text": 0x8,
        ".data": 0x8,
        ".rodata": 0x8,
        ".sdata": 0x8,
        ".sbss": 0x4,
        ".bss": 0x10,
    },
}

SPECIFIC_ALIGNMENTS: dict[str, dict[str, int]] = {
    # .bss
    "pack.bss.s.o": {
        ".bss": 0x40,
    },
    "etc.bss.s.o": {
        ".bss": 0x8,
    },
    "mcctrl.bss.s.o": {
        ".bss": 0x8,
    },
    "menu.bss.s.o": {
        ".bss": 0x8,
    },
}

spimdisasm.common.GlobalConfig.VERBOSE = False
spimdisasm.common.GlobalConfig.QUIET = True

parser = argparse.ArgumentParser(description="Patches elf objects to bypass alignment restrictions of other tools")

parser.add_argument("elf_path")
parser.add_argument("mode", choices=["gas"])
parser.add_argument("--section-align", nargs="+")

args = parser.parse_args()

elf_path = Path(args.elf_path)
section_align_override: dict[str, int] = {}
if args.section_align is not None:
    for entry in args.section_align:
        sect, align_str = entry.split(":")
        align = int(align_str, 0)
        section_align_override[sect] = align

if elf_path.parent.name == "sdk":
    # SDK seems to have 0x4 alignment on .data
    section_align_override.setdefault(".data", 0x4)

# Override specific alignments
specific_alignments = SPECIFIC_ALIGNMENTS.get(elf_path.name)
if specific_alignments:
    print(f"Applying overriden alignment on: \"{elf_path.name}\"")
    section_align_override.update(specific_alignments)

elf_bytes = bytearray(elf_path.read_bytes())

elf_file = spimdisasm.elf32.Elf32File(elf_bytes)

sections_to_realign = SECTIONS_TO_REALIGN_PER_TOOL[args.mode]

for i, sect in enumerate(elf_file.sectionHeaders):
    name = elf_file.shstrtab[sect.name]
    # print(name, sect)

    # Check the cmd line override first
    new_alignment = section_align_override.get(name)

    if new_alignment is None:
        # If no override is present then use the built-in ones
        new_alignment = sections_to_realign.get(name)

    if new_alignment is not None:
        section_offset = elf_file.header.shoff + i * 0x28
        addralign_pointer = section_offset + 0x20
        size_pointer = section_offset + 0x14

        # Patch the alignment of the section
        fmt = spimdisasm.common.GlobalConfig.ENDIAN.toFormatString() + "I"
        struct.pack_into(fmt, elf_bytes, addralign_pointer, new_alignment)

elf_path.write_bytes(elf_bytes)
