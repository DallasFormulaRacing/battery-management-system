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
    Jump = 0xAAAA,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u32)]
pub enum CanDevices {
    RaspberryPi = 0x01,
    Nuc1 = 0x06,
    Nuc2 = 0x07,
    UNKNOWN = 0x1F,
}

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
