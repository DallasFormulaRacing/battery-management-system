#![allow(unused_imports)]

use chrono::Local;
use ratatui::{
    Frame,
    layout::{Alignment, Constraint, Direction, Layout, Rect},
    style::{Color, Modifier, Style},
    symbols::Marker,
    text::{Line, Span, Text},
    widgets::{Axis, Block, Borders, Chart, Clear, Dataset, Gauge, GraphType, Paragraph},
};
use std::time::Instant;

const STATUS_WIDTH: u16 = 30;

/// Homepage renderer
///
/// Pass `t` (your running time counter) and `current_data` (slice of (time,current) points)
/// that you update elsewhere.
pub fn draw_homepage(
    f: &mut Frame,
    t: f64,
    current_data: &[(f64, f64)],
    demo_mode: bool,
    selected_detail: Option<usize>,
) {
    let size = f.area();

    // Layout: [header | segments grid | bottom (status+graph) | footer]
    let v = Layout::default()
        .direction(Direction::Vertical)
        .constraints([
            Constraint::Length(4),  // header
            Constraint::Length(28), // segments grid
            Constraint::Min(0),     // bottom row (status + graph)
            Constraint::Length(1),  // footer
        ])
        .split(size);

    // ── Header with datetime ───────────────────────────────────────────────────
    let now = Local::now().format("%a, %b %e, %Y %I:%M %p").to_string();
    let header_text = Text::from(vec![
        Line::from(Span::styled(
            "Battery Management System -- Graphical Monitoring Interface",
            Style::default().fg(Color::Gray),
        )),
        Line::from(Span::styled(now, Style::default().fg(Color::DarkGray))),
    ]);
    let header = Paragraph::new(header_text)
        .alignment(Alignment::Center)
        .block(
            Block::default()
                .borders(Borders::ALL)
                .border_style(Style::default().fg(Color::DarkGray)),
        );
    f.render_widget(header, v[0]);

    // ── Segments grid: 2 rows × 3 columns ─────────────────────────────────────
    let rows = Layout::default()
        .direction(Direction::Vertical)
        .constraints([Constraint::Length(14), Constraint::Length(14)])
        .split(v[1]);

    for (row_idx, row_rect) in rows.iter().enumerate() {
        let cols = Layout::default()
            .direction(Direction::Horizontal)
            .constraints([
                Constraint::Percentage(33),
                Constraint::Percentage(33),
                Constraint::Percentage(34),
            ])
            .split(*row_rect);

        for (col_idx, col_rect) in cols.iter().enumerate() {
            let seg_num = row_idx * 3 + col_idx + 1;
            draw_segment(f, *col_rect, seg_num);
        }
    }

    // ── Bottom row: [Status | HV Current Graph] ───────────────────────────────
    let bottom = Layout::default()
        .direction(Direction::Horizontal)
        .constraints([Constraint::Length(STATUS_WIDTH), Constraint::Min(0)])
        .split(v[2]);

    draw_status_panel(f, bottom[0]);

    // Chart dataset
    let datasets = vec![
        Dataset::default()
            .name("Current (A)")
            .marker(Marker::Braille)
            .style(Style::default().fg(Color::Green))
            .graph_type(GraphType::Line)
            .data(current_data),
    ];

    // X axis: show a 60s sliding window following t
    let window = 15.0; // width of visible window in seconds

    let (x_start, x_end) = if t < window {
        (0.0, window) // before threshold, axis grows to 60s
    } else {
        (t - window, t) // after threshold, scroll forward
    };

    let chart = Chart::new(datasets)
        .block(
            Block::default()
                .title("High Voltage Current Graph")
                .borders(Borders::ALL)
                .border_style(Style::default().fg(Color::DarkGray)),
        )
        .x_axis(
            Axis::default()
                .title("time (s)")
                .style(Style::default().fg(Color::Gray))
                .bounds([x_start, x_end])
                .labels(vec![
                    Span::raw(format!("{:.0}", x_start)),
                    Span::raw(format!("{:.0}", (x_start + x_end) / 2.0)),
                    Span::raw(format!("{:.0}", x_end)),
                ]),
        )
        .y_axis(
            Axis::default()
                .title("current (A)")
                .style(Style::default().fg(Color::Gray))
                .bounds([0.0, 400.0])
                .labels(vec![
                    Span::raw("0"),
                    Span::raw("100"),
                    Span::raw("200"),
                    Span::raw("300"),
                    Span::raw("400"),
                ]),
        );
    f.render_widget(chart, bottom[1]);

    // ── Footer: Demo indicator (left), help (center), version (right) ─────────
    let footer_split = Layout::default()
        .direction(Direction::Horizontal)
        .constraints([
            Constraint::Length(20),
            Constraint::Min(0),
            Constraint::Length(12),
        ])
        .split(v[3]);

    // Left: DEMO ON/OFF
    let demo_indicator = if demo_mode {
        Paragraph::new("DEMO ON")
            .style(Style::default().fg(Color::Green).bg(Color::DarkGray))
            .alignment(Alignment::Left)
    } else {
        Paragraph::new("DEMO OFF")
            .style(Style::default().fg(Color::DarkGray))
            .alignment(Alignment::Left)
    };
    f.render_widget(demo_indicator, footer_split[0]);

    // Center: help
    let footer_center =
        Paragraph::new("q to quit • 1–6 for segment details • c enlarge current graph • esc close")
            .style(Style::default().fg(Color::DarkGray))
            .alignment(Alignment::Center);
    f.render_widget(footer_center, footer_split[1]);

    // Right: version
    let version = Paragraph::new("v0.1.0")
        .style(Style::default().fg(Color::DarkGray))
        .alignment(Alignment::Right);
    f.render_widget(version, footer_split[2]);

    // Optional detail popup
    if let Some(seg) = selected_detail {
        draw_detail_popup(f, size, seg);
    }
}

// ───────────────────────── helpers ─────────────────────────

pub fn draw_status_panel(f: &mut Frame, area: Rect) {
    let outer = Block::default()
        .title("Status")
        .borders(Borders::ALL)
        .border_style(Style::default().fg(Color::DarkGray));
    f.render_widget(outer.clone(), area);

    let soc_ratio = 0.22f64; // example SOC

    let inner = outer.inner(area);
    let inner_split = Layout::default()
        .direction(Direction::Vertical)
        .constraints([
            Constraint::Length(2), // OVC
            Constraint::Length(2), // PWR
            Constraint::Length(2), // COM
            Constraint::Length(2), // MEM
            Constraint::Length(1), // SOC text
            Constraint::Length(1), // Gauge
            Constraint::Min(0),
            Constraint::Length(2), // Summary
        ])
        .split(inner);

    let oc = false;
    let rail_fault = false;
    let comms_fault = false;
    let mem_fault = false;

    // OVC
    f.render_widget(
        Paragraph::new(vec![Line::from(vec![
            Span::styled("OVC: ", Style::default().fg(Color::Gray)),
            Span::styled(
                if oc { "FAULT" } else { "OK" },
                Style::default()
                    .fg(if oc { Color::Red } else { Color::Green })
                    .add_modifier(Modifier::BOLD),
            ),
        ])]),
        inner_split[0],
    );

    // PWR
    f.render_widget(
        Paragraph::new(vec![Line::from(vec![
            Span::styled("PWR: ", Style::default().fg(Color::Gray)),
            Span::styled(
                if rail_fault { "FAULT" } else { "OK" },
                Style::default()
                    .fg(if rail_fault { Color::Red } else { Color::Green })
                    .add_modifier(Modifier::BOLD),
            ),
        ])]),
        inner_split[1],
    );

    // COM
    f.render_widget(
        Paragraph::new(vec![Line::from(vec![
            Span::styled("COM: ", Style::default().fg(Color::Gray)),
            Span::styled(
                if comms_fault { "FAULT" } else { "OK" },
                Style::default()
                    .fg(if comms_fault {
                        Color::Red
                    } else {
                        Color::Green
                    })
                    .add_modifier(Modifier::BOLD),
            ),
        ])]),
        inner_split[2],
    );

    // MEM
    f.render_widget(
        Paragraph::new(vec![Line::from(vec![
            Span::styled("MEM: ", Style::default().fg(Color::Gray)),
            Span::styled(
                if mem_fault { "FAULT" } else { "OK" },
                Style::default()
                    .fg(if mem_fault { Color::Red } else { Color::Green })
                    .add_modifier(Modifier::BOLD),
            ),
        ])]),
        inner_split[3],
    );

    // SOC label
    f.render_widget(
        Paragraph::new("SOC:").style(Style::default().fg(Color::Gray)),
        inner_split[4],
    );

    // SOC gauge
    let gauge_color = if soc_ratio > 0.6 {
        Color::Green
    } else if soc_ratio > 0.3 {
        Color::Yellow
    } else {
        Color::Red
    };
    let mut gauge_style = Style::default()
        .fg(gauge_color)
        .bg(Color::Black)
        .add_modifier(Modifier::BOLD);
    if soc_ratio < 0.1 {
        gauge_style = gauge_style.add_modifier(Modifier::SLOW_BLINK);
    }
    let gauge = Gauge::default()
        .ratio(soc_ratio)
        .label(format!("{:.0}%", soc_ratio * 100.0))
        .block(Block::default().borders(Borders::NONE))
        .gauge_style(gauge_style);
    f.render_widget(gauge, inner_split[5]);

    // Summary line
    let fault_count = 0;
    let summary_text = if fault_count == 0 {
        "All systems OK"
    } else {
        "System fault detected"
    };
    let summary_line = Paragraph::new(summary_text)
        .style(Style::default().fg(if fault_count == 0 {
            Color::Green
        } else {
            Color::Red
        }))
        .alignment(Alignment::Center);
    f.render_widget(summary_line, inner_split[7]);
}

pub fn draw_detail_popup(f: &mut Frame, area: Rect, seg: usize) {
    f.render_widget(Clear, area);
    let title = format!("Segment {} — Detail View", seg);
    let body = Paragraph::new("Placeholder details...\n(Press ESC to close)")
        .alignment(Alignment::Center)
        .block(
            Block::default()
                .title(title)
                .borders(Borders::ALL)
                .border_style(Style::default().fg(Color::DarkGray)),
        );
    f.render_widget(body, area);
}

/// One segment with 2×3 grid inside (Voltage row + Temp row),
/// labels and values centered vertically + horizontally.
pub fn draw_segment(f: &mut Frame, area: Rect, seg_num: usize) {
    let block = Block::default()
        .title(format!("Segment {}", seg_num))
        .borders(Borders::ALL)
        .border_style(Style::default().fg(Color::DarkGray));
    f.render_widget(block.clone(), area);

    let inner = block.inner(area);
    let seg_rows = Layout::default()
        .direction(Direction::Vertical)
        .constraints([Constraint::Percentage(50), Constraint::Percentage(50)])
        .split(inner);

    // Placeholder values – replace with real data
    let vmax = 3.95;
    let vavg = 3.90;
    let vmin = 3.85;
    let tmax = 42.0;
    let tavg = 40.0;
    let tmin = 38.0;

    // Voltage row
    let voltages = [
        ("MAX VOLT", format!("{:.2}V", vmax)),
        ("AVG VOLT", format!("{:.2}V", vavg)),
        ("MIN VOLT", format!("{:.2}V", vmin)),
    ];
    let vcols = Layout::default()
        .direction(Direction::Horizontal)
        .constraints([
            Constraint::Percentage(33),
            Constraint::Percentage(34),
            Constraint::Percentage(33),
        ])
        .split(seg_rows[0]);

    for (i, (label, value)) in voltages.iter().enumerate() {
        // Mini vertical layout for centering
        let cell = vcols[i];
        let cell_rows = Layout::default()
            .direction(Direction::Vertical)
            .constraints([
                Constraint::Percentage(25),
                Constraint::Length(2), // label
                Constraint::Length(1), // spacing
                Constraint::Length(2), // value
                Constraint::Percentage(25),
            ])
            .split(cell);

        f.render_widget(
            Paragraph::new(*label)
                .alignment(Alignment::Center)
                .style(Style::default().fg(Color::Gray)),
            cell_rows[1],
        );

        f.render_widget(
            Paragraph::new(value.clone())
                .alignment(Alignment::Center)
                .style(Style::default().fg(Color::Green)),
            cell_rows[3],
        );
    }

    // Temp row
    let temps = [
        ("MAX TEMP", format!("{:.1}C", tmax)),
        ("AVG TEMP", format!("{:.1}C", tavg)),
        ("MIN TEMP", format!("{:.1}C", tmin)),
    ];
    let tcols = Layout::default()
        .direction(Direction::Horizontal)
        .constraints([
            Constraint::Percentage(33),
            Constraint::Percentage(34),
            Constraint::Percentage(33),
        ])
        .split(seg_rows[1]);

    for (i, (label, value)) in temps.iter().enumerate() {
        let cell = tcols[i];
        let cell_rows = Layout::default()
            .direction(Direction::Vertical)
            .constraints([
                Constraint::Percentage(25),
                Constraint::Length(2), // label
                Constraint::Length(1), // spacing
                Constraint::Length(2), // value
                Constraint::Percentage(25),
            ])
            .split(cell);

        f.render_widget(
            Paragraph::new(*label)
                .alignment(Alignment::Center)
                .style(Style::default().fg(Color::Gray)),
            cell_rows[1],
        );

        f.render_widget(
            Paragraph::new(value.clone())
                .alignment(Alignment::Center)
                .style(Style::default().fg(Color::Yellow)),
            cell_rows[3],
        );
    }
}
