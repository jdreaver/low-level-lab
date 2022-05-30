use std::env;
use std::fs;
use std::path::Path;
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

    let file_basename: &str = Path::new(&source_path)
        .file_stem()
        .expect("couldn't get file basename")
        .to_str()
        .expect("couldn't convert file basename to &str");

    let source_string = fs::read_to_string(source_path.clone())
        .expect("Something went wrong reading the source file");

    let parsed = vm::parse_vm_source(&source_string).expect("failed to parse source");
    // println!("{:#?}", parsed);

    let asm =
        vm::vm_to_asm(&file_basename.to_string(), parsed).expect("failed to compile source to ASM");
    for line in &asm {
        println!("{}", line)
    }
}
