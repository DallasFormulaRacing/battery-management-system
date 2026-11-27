# ev27-can: the shared EV-27 CAN database (DallasFormulaRacing/ev27-can),
# checked out as a git submodule at bms/submodules/ev27-can and pinned to a release tag.
#
# Builds the committed generated C as-is (no code generation at build time):
#   generated/c/ev27_main.c   main bus (CAN FD, FDCAN2) pack/unpack
#   generated/c/ev27_tb.c     tb bus (500k, FDCAN1) pack/unpack: CAB500, IMD, Elcon
#   node/ev27_node.c          Status / Mute / commands / car time; the BMS provides ev27_port_*
#
# To move to a new database version:
#   cd bms/submodules/ev27-can && git fetch --tags && git checkout vX.Y.Z
#   cd ../../.. && git add bms/submodules/ev27-can   (then commit)

set(EV27_CAN_DIR "${CMAKE_SOURCE_DIR}/submodules/ev27-can")

if(NOT EXISTS "${EV27_CAN_DIR}/generated/c/ev27_main.c")
  message(FATAL_ERROR
    "The ev27-can submodule is missing (${EV27_CAN_DIR}).\n"
    "From the repo root run: git submodule update --init bms/submodules/ev27-can")
endif()

add_library(ev27can STATIC
  "${EV27_CAN_DIR}/generated/c/ev27_main.c"
  "${EV27_CAN_DIR}/generated/c/ev27_tb.c"
  "${EV27_CAN_DIR}/node/ev27_node.c"
)

target_include_directories(ev27can PUBLIC
  "${EV27_CAN_DIR}/generated/c"
  "${EV27_CAN_DIR}/node"
)
