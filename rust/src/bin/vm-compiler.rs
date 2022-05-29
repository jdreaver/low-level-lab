use std::env;
use std::fs;
use std::process;

use nand2tetris::vm;

fn main() {
    let source_path: String = match env::args().collect::<Vec<String>>().as_slice() {
        [_, path] => path.to_string(),
        _ => {
            eprintln!("Usage: assembler <filename>");
            process::exit(1);
        }
    };

    let source_string =
        fs::read_to_string(source_path).expect("Something went wrong reading the source file");

    let parsed = vm::parse_vm_source(&source_string).expect("failed to parse source");
    // println!("{:#?}", parsed);

    let asm = vm::vm_to_asm(parsed).expect("failed to compile source to ASM");
    for line in &asm {
        println!("{}", line)
    }
}
