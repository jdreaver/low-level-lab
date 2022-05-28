use std::env;
use std::fs;
use std::process;

// use nand2tetris::asm;
use nand2tetris::hack;

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

    // let parsed = asm::parse_assembly_source(&source_string).expect("failed to parse source");
    // println!("{:#?}", parsed);

    let assembled = hack::assemble(&source_string).expect("failed to assemble source");
    // println!("{:#?}", assembled);
    for line in &assembled {
        println!("{}", line)
    }
}
