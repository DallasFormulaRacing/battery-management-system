import os
import json

include_dirs = [
    "-I./bms/Common/Inc",
    "-I./bms/Common/Src",
    "-I./bms/App/Inc",
    "-I./bms/Core/Inc",
    "-I./bms/Drivers/STM32G4xx_HAL_Driver/Inc",  
    "-I./bms/Drivers/CMSIS/Device/ST/STM32G4xx/Include",
    "-I./bms/Drivers/CMSIS/Include",
    "-I/Applications/ArmGNUToolchain/13.3.rel1/arm-none-eabi/lib/gcc/arm-none-eabi/13.3.1/include",
    "-I/Applications/ArmGNUToolchain/13.3.rel1/arm-none-eabi/lib/gcc/arm-none-eabi/13.3.1/include-fixed",
    "-I/Applications/ArmGNUToolchain/13.3.rel1/arm-none-eabi/arm-none-eabi/include"
]

defines = [
    "-DSTM32G474xx",
    "-DUSE_HAL_DRIVER"
]
flags = [
    "-std=c11",
    "-x", "c",
    "-c"
]
compiler = "arm-none-eabi-gcc"

compile_db = []

root = os.getcwd()
for dirpath, _, filenames in os.walk("./bms"):
    for filename in filenames:
        if filename.endswith(".c"):
            rel_path = os.path.join(dirpath, filename)
            command = " ".join(
                [compiler]
                + include_dirs
                + defines
                + flags
                + [rel_path, "-o", "null.o"]
            )
            compile_db.append({
                "directory": root,
                "command": command,
                "file": rel_path
            })

with open("compile_commands.json", "w") as f:
    json.dump(compile_db, f, indent=2)

print(f"Generated compile_commands.json with {len(compile_db)} entries.")
