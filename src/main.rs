use clap::Command;
use clap::arg;
use rand::random;
use std::hash::Hash;
use std::io::Write;

#[derive(Clone, PartialEq, Eq, Hash, Debug)]
struct Grid {
    data: Vec<Vec<bool>>,
}

impl Grid {
    fn new(width: usize, height: usize) -> Self {
        Self {
            data: vec![vec![false; width]; height],
        }
    }

    fn randomize(&mut self) {
        for row in &mut self.data {
            for cell in row {
                *cell = random();
            }
        }
    }

    fn print(&self) {
        let rows = self.data.len();
        let cols = if rows > 0 { self.data[0].len() } else { 0 };

        print!("┌");
        for _ in 0..cols {
            print!("─");
        }
        println!("┐");

        for row in &self.data {
            print!("│");
            for &cell in row {
                print!("{}", if cell { "█" } else { " " });
            }
            println!("│");
        }

        print!("└");
        for _ in 0..cols {
            print!("─");
        }
        println!("┘");
        println!();
    }

    // TODO: Rework casting in this function
    fn compute_index(index: usize, offset: isize, max: usize) -> Option<usize> {
        let Ok(idx): Result<isize, _> = index.try_into() else {
            return None;
        };
        let Ok(m): Result<isize, _> = max.try_into() else {
            return None;
        };

        let new_index = (idx + offset).rem_euclid(m);
        if new_index < 0 {
            None
        } else {
            let Ok(new_index_usize): Result<usize, _> = new_index.try_into() else {
                return None;
            };
            Some(new_index_usize)
        }
    }

    fn count_alive_neighbors(&self, x: usize, y: usize) -> usize {
        let height = self.data.len();
        let width = self.data[0].len();
        let mut count = 0;

        for i in -1..=1 {
            for j in -1..=1 {
                if i == 0 && j == 0 {
                    continue;
                }

                let Some(new_x) = Self::compute_index(x, i, height) else {
                    continue;
                };
                let Some(new_y) = Self::compute_index(y, j, width) else {
                    continue;
                };

                if self.data[new_x][new_y] {
                    count += 1;
                }
            }
        }
        count
    }

    fn is_empty(&self) -> bool {
        self.data.iter().all(|row| row.iter().all(|&cell| !cell))
    }
}

#[derive(Clone, Debug)]
struct State {
    generation: usize,
    grid: Grid,
}

impl State {
    const fn new(generation: usize, grid: Grid) -> Self {
        Self { generation, grid }
    }

    fn print(&self) {
        println!("Generation: {}", self.generation);
        self.grid.print();
    }
}

struct Game {
    previous_states: Vec<State>,
    width: usize,
    height: usize,
    current_state: State,
}

impl Game {
    fn new(width: usize, height: usize) -> Self {
        let initial_grid = Grid::new(width, height);
        Self {
            previous_states: Vec::new(),
            width,
            height,
            current_state: State::new(0, initial_grid),
        }
    }

    fn randomize(&mut self) {
        self.current_state.grid.randomize();
    }

    fn reset(&mut self) {
        self.previous_states.clear();
        self.current_state = State::new(0, Grid::new(self.width, self.height));
    }

    fn update(&mut self) {
        let mut new_grid_data = self.current_state.grid.data.clone();

        for (i, row) in self.current_state.grid.data.iter().enumerate() {
            for (j, &cell) in row.iter().enumerate() {
                let alive_neighbors = self.current_state.grid.count_alive_neighbors(i, j);
                if cell {
                    new_grid_data[i][j] = alive_neighbors == 2 || alive_neighbors == 3;
                } else {
                    new_grid_data[i][j] = alive_neighbors == 3;
                }
            }
        }

        self.current_state.grid.data = new_grid_data;
        self.current_state.generation += 1;
    }

    fn tick(&mut self) -> Option<usize> {
        self.update();

        for state in &self.previous_states {
            if state.grid == self.current_state.grid {
                return Some(state.generation);
            }
        }

        self.previous_states.push(self.current_state.clone());

        None
    }

    fn display(&self) {
        println!("\x1B[2J\x1B[1;1H");
        self.current_state.print();
    }

    fn revert_to(&mut self, generation: usize) {
        if let Some(state) = self
            .previous_states
            .iter()
            .find(|s| s.generation == generation)
            .cloned()
        {
            self.current_state = state;
        }
    }

    fn revert_to_initial(&mut self) {
        if let Some(initial_state) = self.previous_states.first() {
            self.current_state = initial_state.clone();
        }
    }

    fn is_empty(&self) -> bool {
        self.current_state.grid.is_empty()
    }

    fn run_until_loop(&mut self, max_generation: usize) -> Option<usize> {
        for _ in 0..max_generation {
            if let Some(cycle_start_generation) = self.tick() {
                return Some(cycle_start_generation);
            }
        }
        None
    }
}

fn sleep_millis(n: u64) {
    std::thread::sleep(std::time::Duration::from_millis(n));
}

fn flush() {
    std::io::stdout().flush().expect("Failed to flush stdout");
}

fn alternate_screen() {
    print!("\x1B[?1049h");
    flush();
}

fn normal_screen() {
    print!("\x1B[?1049l");
    flush();
}

fn main() {
    let matches = Command::new("Game of Life")
        .version("0.1")
        .author("Sam Christy")
        .about("Search for loops in Conway's Game of Life")
        .arg(arg!(--width <WIDTH> "Width of the grid").default_value("10"))
        .arg(arg!(--height <HEIGHT> "Height of the grid").default_value("10"))
        .arg(arg!(--max <MAX> "Maximum number of generations to run").default_value("1000"))
        .arg(arg!(--min <MIN_LOOP_LENGTH> "Minimum loop length to consider").default_value("5"))
        .arg(arg!(--delay <DELAY> "Delay between generations in milliseconds").default_value("100"))
        .get_matches();

    let w = matches
        .get_one::<String>("width")
        .map(|s| s.parse::<usize>().unwrap())
        .unwrap_or(10);
    let h = matches
        .get_one::<String>("height")
        .map(|s| s.parse::<usize>().unwrap())
        .unwrap_or(10);
    let max_generation = matches
        .get_one::<String>("max")
        .map(|s| s.parse::<usize>().unwrap())
        .unwrap_or(1000);
    let min_loop_length = matches
        .get_one::<String>("min")
        .map(|s| s.parse::<usize>().unwrap())
        .unwrap_or(5);
    let delay = matches
        .get_one::<String>("delay")
        .map(|s| s.parse::<u64>().unwrap())
        .unwrap_or(100);

    let mut game_state = Game::new(w, h);
    let mut attempts = 0;

    let _ = ctrlc::set_handler(move || {
        normal_screen();
        println!("Exiting...");
        std::process::exit(0);
    });

    println!("Initialized game state with grid size {w}x{h}");
    loop {
        game_state.reset();
        game_state.randomize();
        print!("\rAttempt: {attempts}");
        flush();
        attempts += 1;
        if let Some(cycle_start_generation) = game_state.run_until_loop(max_generation) {
            let loop_length = game_state.current_state.generation - cycle_start_generation;
            if !game_state.is_empty() && loop_length > min_loop_length {
                game_state.revert_to_initial();
                println!("\nCondition met on attempt {attempts} with loop length {loop_length}");
                alternate_screen();
                loop {
                    game_state.display();
                    println!("Loop length: {loop_length}");
                    println!("Press Ctrl+C to exit.");
                    sleep_millis(delay);
                    game_state.tick();
                }
            }
        }
    }
}
