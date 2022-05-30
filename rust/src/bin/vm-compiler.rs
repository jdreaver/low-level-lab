use std::env;
use std::fs;
use std::path::Path;
use std::process;

use nand2tetris::vm;

fn main() {
    if env::args().len() <= 1 {
        eprintln!("Usage: assembler <filename>");
        process::exit(1);
    }

    for path_str in env::args().skip(1) {
        let path = Path::new(&path_str);
        let file_basename: &str = path
            .file_stem()
            .expect("couldn't get file basename")
            .to_str()
            .expect("couldn't convert file basename to &str");

        let source_string =
            fs::read_to_string(path.clone()).expect("Something went wrong reading the source file");

        let parsed = vm::parse_vm_source(&source_string).expect("failed to parse source");
        // println!("{:#?}", parsed);

        let asm = vm::vm_to_asm(&file_basename.to_string(), parsed)
            .expect("failed to compile source to ASM")
            .join("\n");

        let output_file_path = path.with_extension("asm");
        fs::write(output_file_path, format!("{}\n", asm)).expect("failed to write to output file");
    }
}
