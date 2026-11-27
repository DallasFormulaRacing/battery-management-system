//! Is the app in flash one the bootloader may start?
//!
//! The BMS app carries a 32-byte header at 0x08008200 and an 8-byte end
//! marker as the last bytes of the image (bms/App/config/app_image.c, placed
//! by bms/STM32G474XX_FLASH_BL.ld). Header layout, little endian:
//!    0  u32  magic 0xFEEDFEED
//!    4  u8   header version (1; later versions only append fields)
//!    5  u8   ezfd node address, must be THIS_NODE (0x03)
//!    6  u8   board, must be BOARD (1 = controller, 2 = Nucleo, as BMS_BOARD)
//!    7  u8   0
//!    8  u32  address of the end marker
//!   12  u8   firmware major, minor, patch, 0
//!   16  16 bytes reserved
//! End marker: 0xBEEFBEEF, twice (8 bytes: one flash double word).
//!
//! The dashboard writes the image front to back, so the end marker is the
//! last thing written. Until it is there (an upload that stopped half way)
//! the app is not started.

use crate::{APP_BASE, APP_END, RAM_END, RAM_START, THIS_NODE};

pub const HEADER_ADDR: u32 = APP_BASE + 0x200;
const HEADER_MAGIC: u32 = 0xFEED_FEED;
const END_MAGIC: u32 = 0xBEEF_BEEF;

/// The board this bootloader was built for: the same numbers as BMS_BOARD in
/// bms/App/config/bms_board.h. The app reports it in its Ping reply too.
#[cfg(feature = "nucleo")]
pub const BOARD: u8 = 2; // BMS_BOARD_NUCLEO
#[cfg(not(feature = "nucleo"))]
pub const BOARD: u8 = 1; // BMS_BOARD_CONTROLLER

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u8)]
pub enum Check {
    /// Complete image for this node and board.
    Ok = 0,
    /// Erased, not an app, or an app without the header (built without the
    /// -BL preset, or older than the header).
    NoApp = 1,
    /// Header says another node or board.
    WrongBoard = 2,
    /// Header is fine but the end marker is missing: the upload didn't finish.
    Incomplete = 3,
}

fn rd(addr: u32) -> u32 {
    unsafe { core::ptr::read_volatile(addr as *const u32) }
}

/// Stack pointer in RAM and reset handler inside the app slot (Thumb bit set).
fn vectors_ok() -> bool {
    let sp = rd(APP_BASE);
    let reset = rd(APP_BASE + 4);
    let sp_ok = sp > RAM_START && sp <= RAM_END && (sp & 0x3) == 0;
    let reset_ok = (reset & 1) == 1 && (reset & !1) >= APP_BASE + 8 && (reset & !1) < APP_END;
    sp_ok && reset_ok
}

pub fn check() -> Check {
    if !vectors_ok() || rd(HEADER_ADDR) != HEADER_MAGIC {
        return Check::NoApp;
    }
    let info = rd(HEADER_ADDR + 4).to_le_bytes(); // version, node, board, 0
    if info[0] == 0 || info[0] == 0xFF {
        return Check::NoApp;
    }
    if info[1] as u16 != THIS_NODE || info[2] != BOARD {
        return Check::WrongBoard;
    }
    let end = rd(HEADER_ADDR + 8);
    if end % 8 != 0 || end < HEADER_ADDR + 32 || end > APP_END - 8 {
        return Check::Incomplete;
    }
    if rd(end) != END_MAGIC || rd(end + 4) != END_MAGIC {
        return Check::Incomplete;
    }
    Check::Ok
}
