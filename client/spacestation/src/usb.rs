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
    cell_voltages: Vec<f32>,
    cell_temps: Vec<f32>,
    balancing_status: Vec<bool>,
    general: GeneralSegment,
}
