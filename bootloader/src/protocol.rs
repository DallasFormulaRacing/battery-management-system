#![allow(dead_code)]

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct DfrCanId {
    pub priority: u16,
    pub target: u16,
    pub command: u16,
    pub source: u16,
}

impl DfrCanId {
    pub fn new(priority: u16, target: u16, command: u16, source: u16) -> Result<Self, &'static str> {
        if priority > 0x07 {
            return Err("Priority is out of range (max 7)");
        }
        if target > 0x1F {
            return Err("Target ID is out of range (max 31)");
        }
        if source > 0x1F {
            return Err("Source is out of range (max 31)");
        }

        Ok(Self {
            priority,
            target,
            command,
            source,
        })
    }

    /// Packs the struct back into a 29-bit raw CAN identifier
    pub fn to_raw_id(&self) -> u32 {
        ((self.priority as u32) << 26)
            | ((self.target as u32) << 21)
            | ((self.command as u32) << 5)
            | (self.source as u32)
    }
}


#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u16)]
pub enum BootloaderCommand {
    Ping = 0x40,
    Erase = 0x45,
    EraseOk = 0x46,
    Write = 0x47,
    WriteOk = 0x48,
    AddressAndSize = 0x4A,
    FirmwareUpdateQuery = 0x4B,
    FirmwareUpdateResponse = 0x4C,
    Reboot = 0x4D,
    /// G474 BMS bootloader only: [length u32 BE, crc32 u32 BE] of the image
    /// from APP_BASE. Answered with VerifyResult.
    Verify = 0x4E,
    /// [result u8 (0 ok, 1 CRC mismatch, 2 bad length, 3 image check failed),
    ///  crc32 of the flash u32 BE, image::Check u8]
    VerifyResult = 0x4F,
    Jump = 0xAAAA,
}

/// Node addresses from ev27-can can/devices.yaml (generated/c/ev27_devices.h,
/// EV27_ADDR_*). The bootloader uses the same address as the device's Status
/// frame (0x700 + addr). Only the two this bootloader talks to; add others
/// from devices.yaml when needed.
pub const EV27_ADDR_CM5: u16 = 0x01;
pub const EV27_ADDR_BMS: u16 = 0x03;

pub fn parse_can_id(raw_id: u32) -> DfrCanId {
    // raw_id (29 bits) = [priority 3b][target 5b][command 16b][source 5b]
    let priority = ((raw_id >> 26) & 0x07) as u16;
    let target = ((raw_id >> 21) & 0x1F) as u16;
    let command = ((raw_id >> 5) & 0xFFFF) as u16;
    let source = (raw_id & 0x1F) as u16;

    DfrCanId {
        priority,
        target,
        command,
        source,
    }
}

impl TryFrom<u16> for BootloaderCommand {
    type Error = ();
    fn try_from(v: u16) -> Result<Self, Self::Error> {
        match v {
            x if x == BootloaderCommand::Ping as u16 => Ok(BootloaderCommand::Ping),
            x if x == BootloaderCommand::Erase as u16 => Ok(BootloaderCommand::Erase),
            x if x == BootloaderCommand::EraseOk as u16 => Ok(BootloaderCommand::EraseOk),
            x if x == BootloaderCommand::AddressAndSize as u16 => Ok(BootloaderCommand::AddressAndSize),
            x if x == BootloaderCommand::Write as u16 => Ok(BootloaderCommand::Write),
            x if x == BootloaderCommand::WriteOk as u16 => Ok(BootloaderCommand::WriteOk),
            x if x == BootloaderCommand::FirmwareUpdateQuery as u16 => Ok(BootloaderCommand::FirmwareUpdateQuery),
            x if x == BootloaderCommand::FirmwareUpdateResponse as u16 => Ok(BootloaderCommand::FirmwareUpdateResponse),
            x if x == BootloaderCommand::Reboot as u16 => Ok(BootloaderCommand::Reboot),
            x if x == BootloaderCommand::Verify as u16 => Ok(BootloaderCommand::Verify),
            x if x == BootloaderCommand::VerifyResult as u16 => Ok(BootloaderCommand::VerifyResult),
            x if x == BootloaderCommand::Jump as u16 => Ok(BootloaderCommand::Jump),
            _ => Err(()),
        }
    }
}

impl From<BootloaderCommand> for u16 {
    fn from(cmd: BootloaderCommand) -> Self {
        cmd as u16
    }
}
