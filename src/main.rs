use std::collections::hash_map::DefaultHasher;
use std::hash::{Hash, Hasher};
use rand::random;

#[derive(Clone, PartialEq, Eq, Hash, Debug)]
struct Grid {
    data: Vec<Vec<bool>>,
}

impl Grid {
    fn new(width: usize, height: usize) -> Self {
        Grid {
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
        for row in &self.data {
            for &cell in row {
                print!("{}", if cell { "█" } else { " " });
            }
            println!();
        }
        println!();
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

                let new_x = (x as isize + i).rem_euclid(height as isize) as usize;
                let new_y = (y as isize + j).rem_euclid(width as isize) as usize;

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
    fn new(generation: usize, grid: Grid) -> Self {
        State { generation, grid }
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
        Game {
            previous_states: Vec::new(),
            width,
            height,
            current_state: State::new(0, initial_grid),
        }
    }

    fn new_random(width: usize, height: usize) -> Self {
        let mut game = Game::new(width, height);
        game.randomize();
        game
    }

    fn randomize(&mut self) {
        self.current_state.grid.randomize();
    }

    fn reset(&mut self) {
        self.previous_states.clear();
        self.current_state = State::new(0, Grid::new(self.width, self.height));
    }

    fn is_empty(&self) -> bool {
        self.current_state.grid.is_empty()
    }
}

fn main() {
}
