use crate::ui::{draw_detail_popup, draw_homepage};
use crossterm::event::{self, Event, KeyCode};
use ratatui::{Terminal, backend::CrosstermBackend};
use std::io;
use std::time::{Duration, Instant};

pub struct AppState {
    pub t: f64,
    pub current_data: Vec<(f64, f64)>,
    pub start_time: Instant,
}

impl AppState {
    pub fn new() -> Self {
        Self {
            t: 0.0,
            current_data: Vec::new(),
            start_time: Instant::now(),
        }
    }

    /// Push a fake current value
    pub fn push_fake_current(&mut self) {
        // Update time from start
        self.t = self.start_time.elapsed().as_secs_f64();

        // Fake waveform
        let value = 200.0 + (self.t * 5.0).sin() * 100.0;

        // Append to dataset
        self.current_data.push((self.t, value));

        // Keep only last 60s
        let window = 60.0;
        self.current_data
            .retain(|(time, _)| *time >= self.t - window);
    }
}

pub fn run_homepage(
    terminal: &mut Terminal<CrosstermBackend<std::io::Stdout>>,
    demo_mode: bool,
) -> io::Result<()> {
    let mut selected_detail: Option<usize> = None;
    let mut app = AppState::new();

    let mut last_render = Instant::now();
    let render_interval = Duration::from_secs(1); // 1 FPS

    loop {
        // Always check for input (every ~100 ms)
        if event::poll(Duration::from_millis(100))? {
            if let Event::Key(key) = event::read()? {
                match key.code {
                    KeyCode::Char('q') => break,
                    KeyCode::Esc => selected_detail = None,
                    KeyCode::Char(c @ '1'..='6') => {
                        selected_detail = Some((c as u8 - b'0') as usize);
                    }
                    _ => {}
                }

                terminal.draw(|f| {
                    draw_homepage(f, app.t, &app.current_data, demo_mode, selected_detail);
                })?;

                last_render = Instant::now();
            }
        }

        // Only update + render if enough time has passed
        if last_render.elapsed() >= render_interval {
            app.push_fake_current(); // new fake data point

            terminal.draw(|f| {
                draw_homepage(f, app.t, &app.current_data, demo_mode, selected_detail);
            })?;

            last_render = Instant::now();
        }
    }

    Ok(())
}
