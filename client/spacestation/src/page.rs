use crate::ui::{draw_detail_popup, draw_homepage};
use crossterm::event::{self, Event, KeyCode};
use ratatui::{Terminal, backend::CrosstermBackend};
use std::io;

pub fn run_homepage(
    terminal: &mut Terminal<CrosstermBackend<std::io::Stdout>>,
    demo_mode: bool,
) -> io::Result<()> {
    let mut selected_detail: Option<usize> = None;

    loop {
        terminal.draw(|f| {
            draw_homepage(f, demo_mode, selected_detail);
        })?;

        if event::poll(std::time::Duration::from_millis(200))? {
            if let Event::Key(key) = event::read()? {
                match key.code {
                    KeyCode::Char('q') => break,
                    KeyCode::Esc => selected_detail = None,
                    KeyCode::Char(c @ '1'..='6') => {
                        selected_detail = Some((c as u8 - b'0') as usize);
                    }
                    _ => {}
                }
            }
        }
    }
    Ok(())
}
