struct GeneralSegment {
    avg_voltage: f32,
    max_voltage: f32,
    min_voltage: f32,
    avg_temp: f32,
    max_temp: f32,
    min_temp: f32,
    over_voltage_faults: bool,
    under_voltage_faults: bool,
    over_temp_faults: bool,
}

struct SegmentDetail {
    cell_voltages: [f32; 23],
    cell_temps: [f32; 23],
    balancing_status: [u16; 23],
    general: GeneralSegment,
}
