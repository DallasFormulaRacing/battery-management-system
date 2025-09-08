use chrono::Local;
use ratatui::{
    Frame,
    layout::{Alignment, Constraint, Direction, Layout, Rect},
    style::{Color, Modifier, Style},
    text::{Line, Span, Text},
    widgets::{Block, Borders, Clear, Gauge, Paragraph},
};

const STATUS_WIDTH: u16 = 30;

pub fn draw_homepage(f: &mut Frame, demo_mode: bool, selected_detail: Option<usize>) {
    let size = f.area();

    // Layout...
    let v = Layout::default()
        .direction(Direction::Vertical)
        .constraints([
            Constraint::Length(4),
            Constraint::Length(28),
            Constraint::Min(0),
            Constraint::Length(1),
        ])
        .split(size);

    // Header with datetime
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

    // Segments grid
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
            let block = Block::default()
                .title(format!("Segment {}", seg_num))
                .borders(Borders::ALL)
                .border_style(Style::default().fg(Color::DarkGray));
            f.render_widget(block, *col_rect);
        }
    }

    // Bottom: status + graph
    let bottom = Layout::default()
        .direction(Direction::Horizontal)
        .constraints([Constraint::Length(STATUS_WIDTH), Constraint::Min(0)])
        .split(v[2]);

    draw_status_panel(f, bottom[0]);

    let graph_block = Block::default()
        .title("High Voltage Current Graph")
        .borders(Borders::ALL)
        .border_style(Style::default().fg(Color::DarkGray));
    f.render_widget(graph_block, bottom[1]);

    // Footer (demo indicator + version)
    let footer_split = Layout::default()
        .direction(Direction::Horizontal)
        .constraints([
            Constraint::Length(20),
            Constraint::Min(0),
            Constraint::Length(12),
        ])
        .split(v[3]);

    let footer = Paragraph::new(
        "q to quit • 1–6 for segment details • c for enlarged current graph • esc to close popups",
    )
    .style(Style::default().fg(Color::DarkGray))
    .alignment(Alignment::Center);
    f.render_widget(footer, v[3]);

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

    let version = Paragraph::new("v0.1.0")
        .style(Style::default().fg(Color::DarkGray))
        .alignment(Alignment::Right);
    f.render_widget(version, footer_split[2]);

    if let Some(seg) = selected_detail {
        draw_detail_popup(f, size, seg);
    }
}

pub fn draw_status_panel(f: &mut Frame, area: Rect) {
    let outer = Block::default()
        .title("Status")
        .borders(Borders::ALL)
        .border_style(Style::default().fg(Color::DarkGray));
    f.render_widget(outer.clone(), area);

    let inner = outer.inner(area);
    let inner_split = Layout::default()
        .direction(Direction::Vertical)
        .constraints([
            Constraint::Min(0),
            Constraint::Length(3),
            Constraint::Length(3),
            Constraint::Length(3),
            Constraint::Length(3),
            Constraint::Min(0),
        ])
        .split(inner);

    let shutdown_ok = true;
    let error_flags = "FAULT";
    let soc_ratio = 0.02;

    let shutdown_line = Paragraph::new(vec![Line::from(vec![
        Span::styled("Shutdown Circuit: ", Style::default().fg(Color::Gray)),
        Span::styled(
            if shutdown_ok { "CLOSED" } else { "OPEN" },
            Style::default().fg(if shutdown_ok {
                Color::Green
            } else {
                Color::Red
            }),
        ),
    ])]);
    f.render_widget(shutdown_line, inner_split[1]);

    let error_line = Paragraph::new(vec![Line::from(vec![
        Span::styled("Error Flags: ", Style::default().fg(Color::Gray)),
        Span::styled(
            error_flags,
            Style::default().fg(if error_flags == "NONE" {
                Color::Green
            } else {
                Color::Red
            }),
        ),
    ])]);
    f.render_widget(error_line, inner_split[2]);

    let soc_line = Paragraph::new(format!("State of Charge: {:.0}%", soc_ratio * 100.0))
        .style(Style::default().fg(Color::Gray));
    f.render_widget(soc_line, inner_split[3]);

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
    f.render_widget(gauge, inner_split[4]);
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
