//! ev27-can `Status` (ID 0x700 + node address, 48 bytes, every 500 ms) as the
//! bootloader sends it: Mode = Bootloader, so the CM5 and the dashboard see
//! that the BMS is up but not running its app.
//!
//! Layout from ev27-can generated/dbc/ev27_main.dbc (BMS_Status, little endian):
//!   byte 0     bits 0-3 Mode, bit 4 Muted, bit 5 ConfigValid, bit 6 TimeSynced
//!   byte 1     Role
//!   bytes 2-4  FwMajor, FwMinor, FwPatch
//!   bytes 5-8  GitHash
//!   bytes 9-12 Uptime (s)
//!   13-14 SensorsEnabled, 15-16 SensorsPresent, 17-18 SensorsHealthy,
//!   19-20 ErrorFlags, 21 CanTxErrors, 22 CanRxErrors,
//!   23-26 Uid0, 27-30 Uid1, 31-34 Uid2, 35-47 unused

pub const STATUS_BASE_ID: u16 = 0x700;
pub const STATUS_LEN: usize = 48;

pub const MODE_BOOTLOADER: u8 = 1;

/// Bootloader version, reported as FwMajor.FwMinor.FwPatch while in the bootloader.
pub const BL_VERSION: (u8, u8, u8) = (0, 1, 0);

/// ErrorFlags bits the bootloader sets.
pub const ERR_NO_VALID_APP: u16 = 1 << 0;

pub struct Status {
    pub uptime_s: u32,
    pub error_flags: u16,
    pub tec: u8,
    pub rec: u8,
    pub uid: [u32; 3],
}

pub fn pack(s: &Status) -> [u8; STATUS_LEN] {
    let mut b = [0u8; STATUS_LEN];
    b[0] = MODE_BOOTLOADER | (1 << 5); // ConfigValid: the BMS has no config record
    b[2] = BL_VERSION.0;
    b[3] = BL_VERSION.1;
    b[4] = BL_VERSION.2;
    b[9..13].copy_from_slice(&s.uptime_s.to_le_bytes());
    b[19..21].copy_from_slice(&s.error_flags.to_le_bytes());
    b[21] = s.tec;
    b[22] = s.rec;
    b[23..27].copy_from_slice(&s.uid[0].to_le_bytes());
    b[27..31].copy_from_slice(&s.uid[1].to_le_bytes());
    b[31..35].copy_from_slice(&s.uid[2].to_le_bytes());
    b
}
