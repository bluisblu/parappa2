#! /usr/bin/env python3

# SPDX-FileCopyrightText: Copyright 2025 polyproxy
# SPDX-License-Identifier: MIT

import argparse
import os
import shutil
import subprocess
import sys
import re
import json

from pathlib import Path
from typing import Dict, List, Set, Union, Any, Literal, cast

import ninja_syntax

import splat
import splat.scripts.split as split
from splat.segtypes.linker_entry import LinkerEntry

ROOT = Path(__file__).parent
TOOLS_DIR = ROOT / "tools"

YAML_FILE = "config/pr2.proto.yaml"
BASENAME = "SCPS_150.17"
LD_PATH = f"{BASENAME}.ld"
ELF_PATH = f"build/{BASENAME}"
MAP_PATH = f"build/{BASENAME}.map"
PRE_ELF_PATH = f"build/{BASENAME}.elf"

COMMON_INCLUDES = "-Iinclude -Isrc -isystem include/sdk/ee -isystem include/sdk -isystem include/gcc -isystem include/gcc/gcc-lib"
COMPILER_DIR = f"{TOOLS_DIR}/cc/ee-gcc2.96/bin"

COMPILER_FLAGS = "-O2 -G8 -gstabs"
COMPILER_FLAGS_CPP = "-O2 -G8 -gstabs -x c++ -fno-exceptions -fno-strict-aliasing"

COMPILE_CMD = f"{COMPILER_DIR}/ee-gcc -c {COMMON_INCLUDES} {COMPILER_FLAGS}"
COMPILE_CMD_CPP = f"{COMPILER_DIR}/ee-gcc -c {COMMON_INCLUDES} {COMPILER_FLAGS_CPP}"

def exec_shell(command: List[str], stdout=subprocess.PIPE) -> str:
    ret = subprocess.run(command, stdout=stdout, stderr=subprocess.PIPE, text=True)
    return ret.stdout

def clean():
    if os.path.exists(".splache"):
        os.remove(".splache")
    if os.path.exists(".ninja_log"):
        os.remove(".ninja_log")
    if os.path.exists("build.ninja"):
        os.remove("build.ninja")
    if os.path.exists("SCPS_150.17.ld"):
        os.remove("SCPS_150.17.ld")
    shutil.rmtree("asm", ignore_errors=True)
    shutil.rmtree("assets", ignore_errors=True)
    shutil.rmtree("build", ignore_errors=True)

# https://github.com/Fantaskink/SOTC/blob/44d5744b0a1725da85c980ea1389e544d1802f23/configure.py#L177-L214
# Pattern to workaround unintended nops around loops
COMMENT_PART = r"\/\* (.+) ([0-9A-Z]{2})([0-9A-Z]{2})([0-9A-Z]{2})([0-9A-Z]{2}) \*\/"
INSTRUCTION_PART = r"(\b(bc1t|bne|bnel|beq|beql|bqez|bnez|bnezl|beqzl|bgez|bgezl|bgtz|bgtzl|blez|blezl|bltz|bltzl|b|slti)\b.*)"
OPCODE_PATTERN = re.compile(f"{COMMENT_PART}  {INSTRUCTION_PART}")

SLOOP_PROBLEMATIC_FUNCS = [
    "TsRestoreSaveData.s",
    "TsGetRankingList.s",
    "TsOption_Draw.s",
    "TsNAMEINBox_Flow.s",
    "_TsCELBackObjDraw.s",
]

def patch_branch_instructions(folder: str, func: str = None) -> None:
    print(f'(HACK) Applying short loop fix on "{folder}"')
    for root, dirs, files in os.walk(folder):
        for filename in files:
            filepath = os.path.join(root, filename)

            # Only patch branches on a specific function.
            if func and os.path.splitext(filename)[0] != func:
                continue

            # Uncomment to only apply the patch on specific functions
            #
            # if filename not in SLOOP_PROBLEMATIC_FUNCS:
            #     continue

            with open(filepath, "r") as file:
                content = file.read()

            if re.search(OPCODE_PATTERN, content):
                # print(f"(HACK) Applying short loop fix on file \"{filename}\"")

                # Reference found
                # Embed the opcode, we have to swap byte order for correct endianness
                content = re.sub(
                    OPCODE_PATTERN,
                    r"/* \1 \2\3\4\5 */  .word      0x\5\4\3\2 /* \6 */",
                    content,
                )

                with open(filepath, "w") as file:
                    file.write(content)

def apply_short_loop_fix():
    # patch_branch_instructions("asm/nonmatchings/main/scrctrl")
    patch_branch_instructions("asm/nonmatchings/menu/menusub")
    patch_branch_instructions("asm/nonmatchings/prlib/render")
    patch_branch_instructions("asm/nonmatchings/prlib/shape")
    patch_branch_instructions("asm/nonmatchings/prlib/spadata")
    patch_branch_instructions("asm/nonmatchings/prlib/menderer")

EUC_HACK_FILENAME_TABLE = ["TsDrawUPacket.s", "_P3MC_SetBrowsInfo.s"]
def eucjp_convert():
    for root, dirs, files in os.walk("asm/nonmatchings/"):
        for filename in files:
            if filename.startswith("D_") or filename in EUC_HACK_FILENAME_TABLE:
                filepath = os.path.join(root, filename)

                if filename in EUC_HACK_FILENAME_TABLE:
                    print(f"(HACK) Converting {filename} to EUC-JP")

                with open(filepath, "r", encoding="utf-8") as file:
                    content = file.read()

                eucjp_content = content.encode("euc-jp").decode("euc-jp")

                with open(filepath, "w", encoding="euc-jp") as file:
                    file.write(eucjp_content)

def write_permuter_settings():
    with open("permuter_settings.toml", "w") as f:
        f.write(
            """compiler_command = "tools/cc/ee-gcc2.96/bin/ee-gcc -c -Iinclude -Iinclude/sdk/ee -Iinclude/gcc -Iinclude/gcc/gcc-lib -O2 -G8 -gstabs -D__GNUC__"
assembler_command = "mips-linux-gnu-as -march=r5900 -mabi=eabi -Iinclude"
compiler_type = "gcc"

[preserve_macros]

[decompme.compilers]
"tools/cc/ee-gcc2.96/bin/ee-gcc" = "ee-gcc2.9-991111-01"
"""
        )

def build_stuff(linker_entries: List[LinkerEntry]):
    built_objects: Set[Path] = set()

    def build(
        object_paths: Union[Path, List[Path]],
        src_paths: List[Path],
        task: str,
        variables: Dict[str, str] = {},
        implicit_outputs: List[str] = [],
    ):
        if not isinstance(object_paths, list):
            object_paths = [object_paths]

        object_strs = [str(obj) for obj in object_paths]

        for object_path in object_paths:
            if object_path.suffix == ".o":
                built_objects.add(object_path)
            ninja.build(
                outputs=object_strs,
                rule=task,
                inputs=[str(s) for s in src_paths],
                variables=variables,
                implicit_outputs=implicit_outputs,
            )

    ninja = ninja_syntax.Writer(open(str(ROOT / "build.ninja"), "w"), width=9999)

    # Rules
    cross = "mips-linux-gnu-"
    ld_args = "-EL -T config/undefined_syms_auto.txt -T config/undefined_funcs_auto.txt -T config/undefined_syms.txt -Map $mapfile -T $in -o $out"

    ninja.rule(
        "as",
        description="as $in",
        command=f"cpp {COMMON_INCLUDES} $in -o - | iconv -f=UTF-8 -t=EUC-JP $in | {cross}as -no-pad-sections -EL -march=5900 -mabi=eabi -Iinclude -o $out && python3 tools/buildtools/elf_patcher.py $out gas",
    )

    ninja.rule(
        "cpp",
        description="cpp $in",
        command=f"{COMPILE_CMD_CPP} $in -o $out && {cross}strip $out -N dummy-symbol-name",
    )

    ninja.rule(
        "cc",
        description="cc $in",
        command=f"{COMPILE_CMD} $in -o $out && {cross}strip $out -N dummy-symbol-name",
    )

    ninja.rule(
        "ld",
        description="link $out",
        command=f"{cross}ld {ld_args}",
    )

    ninja.rule(
        "sha1sum",
        description="sha1sum $in",
        command="sha1sum -c $in && touch $out",
    )

    ninja.rule(
        "elf",
        description="elf $out",
        command=f"{cross}objcopy $in $out -O binary",
    )

    for entry in linker_entries:
        seg = entry.segment

        if seg.type[0] == ".":
            continue

        if entry.object_path is None:
            continue

        if isinstance(seg, splat.segtypes.common.asm.CommonSegAsm) or isinstance(
            seg, splat.segtypes.common.data.CommonSegData
        ):
            build(entry.object_path, entry.src_paths, "as")
        elif isinstance(seg, splat.segtypes.common.cpp.CommonSegCpp):
            build(entry.object_path, entry.src_paths, "cpp")
        elif isinstance(seg, splat.segtypes.common.c.CommonSegC):
            build(entry.object_path, entry.src_paths, "cc")
        elif isinstance(seg, splat.segtypes.common.databin.CommonSegDatabin) or isinstance(seg, splat.segtypes.common.rodatabin.CommonSegRodatabin):
            build(entry.object_path, entry.src_paths, "as")
        else:
            print(f"ERROR: Unsupported build segment type {seg.type}")
            sys.exit(1)

    ninja.build(
        PRE_ELF_PATH,
        "ld",
        LD_PATH,
        implicit=[str(obj) for obj in built_objects],
        variables={"mapfile": MAP_PATH},
    )

    ninja.build(
        ELF_PATH,
        "elf",
        PRE_ELF_PATH,
    )

    ninja.build(
        ELF_PATH + ".ok",
        "sha1sum",
        "checksum.sha1",
        implicit=[ELF_PATH],
    )

def generate_objdiff_configuration(config: dict[str, Any]):
    """
    Generate `objdiff.json` configuration from splat YAML config.

    Parse splat YAML config to get a list of the TUs that need to
    be diffed and create appropriate `units` for objdiff to process.

    Target objects need to be extracted separately (see the
    `make <lang>-make-asm` command) in order for objdiff to find the
    target files.
    """
    segments: list[Any] = config["segments"]

    tu_to_diff: list[tuple[Literal["asm", "c"], str]] = []

    for segment in segments:
        if not (isinstance(segment, dict) and segment["name"] == "main"):
            # we are looking for the main segment
            continue

        subsegments = cast(list[Any], segment["subsegments"])

        for subsegment in subsegments:
            if isinstance(subsegment, list):
                if len(subsegment) != 3:
                    # entry is not complete => skip it
                    continue

                _, subs_type, subs_name = cast(tuple[int, str, str], subsegment)

            elif isinstance(subsegment, dict):
                subs_type = cast(int, subsegment["type"])
                subs_name = cast(str, subsegment["name"])

            else:
                raise RuntimeError("invalid subsegment type")

            if subs_type in ("asm", "c", "cpp"):
                # if subs_name in ("os/tim2",) or subs_name.startswith("sdk/"):
                if subs_name.startswith("sdk/"):
                    # -> skip SDK as it's not part of the game files
                    continue

                tu_to_diff.append((subs_type, subs_name))

    units: list[dict[str, Any]] = []

    decomp_tu_count: int = 0

    for tu_type, tu_name in tu_to_diff:
        tu_obj_suffix = f".{tu_type}.o" # .c.o or .cpp.o

        target_path = Path("obj", tu_name).with_suffix(tu_obj_suffix)

        # since we only compile fully decompiled TUs, the
        # "c" type implies that the TU is complete
        is_decompiled = tu_type in ("c", "cpp")

        category = Path(tu_name).parts[0]

        if is_decompiled:
            # compose the build path as "build/src/path/of/tu.{c,cpp}.o"
            base_path = Path("build", "src", tu_name).with_suffix(tu_obj_suffix)
            decomp_tu_count += 1
        else:
            # use dummy object for incomplete (not yet decompiled) TUs:
            # expected/obj/dummy.c.o
            base_path = Path("obj", "dummy").with_suffix(".c.o")

        units.append(
            {
                "name": tu_name,
                "target_path": str(target_path),
                "base_path": str(base_path),
                "scratch": {
                    "platform": "ps2",
                    "compiler": "ee-gcc2.9-991111-01",
                    "c_flags": "-O2 -G8 -gstabs",
                    "preset_id": 118 # Preset ID for PaRappa the Rapper 2
                },
                "metadata": {"progress_categories": [category]},
            }
        )

    objdiff_json: dict[str, Any] = {
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "custom_make": "ninja",
        "custom_args": [],
        "build_target": False,
        "build_base": True,
        "watch_patterns": [
            # "src/**/*.h",
            # "src/**/*.c",
            # "src/**/*.cpp"
        ],
        "units": units,
    }

    objdiff_path = Path("objdiff.json")

    with objdiff_path.open(mode="w") as fw:
        json.dump(objdiff_json, fw, indent=2)

    print(
        f"Wrote objdiff configuration ({len(units)} units) to {objdiff_path} ({decomp_tu_count} decompiled)"
    )

def build_objdiff_objects():
    # obj_path = Path("obj")
    objdiff_path = Path("objdiff.json")

    dummy_path = Path("obj", "dummy").with_suffix(".c.o")

    objdiff_conf = json.loads(objdiff_path.read_text())

    units = objdiff_conf["units"]

    build_jobs: list[tuple[Path, Path]] = []

    for unit in units:
        # name: str = unit["name"]
        target_path: Path = Path(unit["target_path"])
        base_path: Path = Path(unit["base_path"])

        if base_path == dummy_path:
            continue

        asm_path = Path("asm", *target_path.parts[1:]).with_suffix("").with_suffix(".s")

        assert asm_path.exists()

        build_jobs.append((asm_path, target_path))

    # compile objects

    cross = "mips-linux-gnu-"

    dummy_c_path = Path("obj", "dummy.c")
    dummy_o_path = Path("obj", "dummy.c.o")

    # create the empty source (touch)
    dummy_c_path.parent.mkdir(parents=True, exist_ok=True)
    dummy_c_path.open(mode="a").close()
    command = f"{COMPILE_CMD} {dummy_c_path} -o {dummy_o_path} && {cross}strip {dummy_o_path} -N dummy-symbol-name"
    subprocess.run(command, shell=True)
    dummy_c_path.unlink()

    for asm_path, target_path in build_jobs:
        command = (
            f"cpp {COMMON_INCLUDES} {asm_path} -o - | "
            f"iconv -f=UTF-8 -t=EUC-JP {asm_path} | "
            f"{cross}as -no-pad-sections -EL -march=5900 -mabi=eabi -Iinclude -o {target_path} && "
            f"python3 tools/buildtools/elf_patcher.py {target_path} gas"
        )

        asm_text = asm_path.read_text()
        asm_text = re.sub(r" ACC,", " $ACC,", asm_text)
        asm_text = re.sub(r" Q, ", " $Q, ", asm_text)
        asm_text = re.sub(r", Q\n", ", $Q\n", asm_text)
        asm_text = re.sub(r" R, ", " $R, ", asm_text)
        asm_text = re.sub(r", R\n", ", $R\n", asm_text)
        asm_path.write_text(asm_text)

        target_path.parent.mkdir(parents=True, exist_ok=True)

        subprocess.run(command, shell=True)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Configure the project")
    parser.add_argument(
        "-c",
        "--clean",
        help="Clean extraction and build artifacts",
        action="store_true",
    )
    parser.add_argument(
        "-csrc",
        "--cleansrc",
        help="Clean the 'src' folder",
        action="store_true",
    )
    parser.add_argument(
        "-noeuc",
        "--no-eucjp-converting",
        help="Do not convert to EUC-JP the disassembly strings",
        action="store_true",
    )
    parser.add_argument(
        "-nosloopfix",
        "--no-short-loop-fix",
        help="Do not patch branch instructions on specific functions to combat an assembler bug",
        action="store_true",
    )
    parser.add_argument(
        "-objdiff",
        "--objdiff",
        help="Output objdiff JSONs (requires existing build)",
        action="store_true"
    )
    args = parser.parse_args()

    if args.clean:
        if args.objdiff:
            print("warning: --objdiff enabled with --clean. Clean will be skipped.")
        else:
            clean()

    if args.cleansrc:
        shutil.rmtree("src", ignore_errors=True)

    split.main([Path(YAML_FILE)], modes="all", verbose=False)

    linker_entries = split.linker_writer.entries

    build_stuff(linker_entries)

    write_permuter_settings()

    if not args.no_short_loop_fix:
        if args.clean:
            apply_short_loop_fix()
        else:
            print("warning: Not a clean build. Skipping short loop fix.")

    if not args.no_eucjp_converting:
        if args.clean:
            eucjp_convert()
        else:
            print("warning: Not a clean build. Skipping EUC-JP conversion.")

    exec_shell(["ninja", "-t", "compdb"], open("compile_commands.json", "w"))

    if args.objdiff:
        generate_objdiff_configuration(split.config)
        build_objdiff_objects()
