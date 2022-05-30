use std::collections::HashMap;

use super::asm;
use super::asm::Symbol;
use super::misc::SourceLine;

pub enum HackInstruction {
    AInstruction(AInstruction),
    CInstruction(CInstruction),
}

/// A `AInstruction` is:
///     0 vvvvvvvvvvvvvvv
#[derive(Debug, PartialEq, Clone)]
pub struct AInstruction {
    v: [bool; 15],
}

/// A `CInstruction` is:
///     111 a cccccc ddd jjj
#[derive(Debug, PartialEq, Clone)]
pub struct CInstruction {
    ac: [bool; 7],
    d: [bool; 3],
    j: [bool; 3],
}

pub fn assemble(source: &String) -> Result<Vec<String>, String> {
    let mut symbols: HashMap<Symbol, u16> = HashMap::from([
        (Symbol("R0".to_string()), 0),
        (Symbol("R1".to_string()), 1),
        (Symbol("R2".to_string()), 2),
        (Symbol("R3".to_string()), 3),
        (Symbol("R4".to_string()), 4),
        (Symbol("R5".to_string()), 5),
        (Symbol("R6".to_string()), 6),
        (Symbol("R7".to_string()), 7),
        (Symbol("R8".to_string()), 8),
        (Symbol("R9".to_string()), 9),
        (Symbol("R10".to_string()), 10),
        (Symbol("R11".to_string()), 11),
        (Symbol("R12".to_string()), 12),
        (Symbol("R13".to_string()), 13),
        (Symbol("R14".to_string()), 14),
        (Symbol("R15".to_string()), 15),
        (Symbol("SP".to_string()), 0),
        (Symbol("LCL".to_string()), 1),
        (Symbol("ARG".to_string()), 2),
        (Symbol("THIS".to_string()), 3),
        (Symbol("THAT".to_string()), 4),
        (Symbol("SCREEN".to_string()), 16384),
        (Symbol("KBD".to_string()), 24576),
    ]);
    let parsed = asm::parse_assembly_source(source)?;

    // First pass: define labels and filter them out
    let mut instructions: Vec<SourceLine<asm::AssemblyInstruction>> = Vec::new();
    for inst in parsed.iter() {
        match &inst.item {
            asm::AssemblyElement::Label(sym) => {
                // TODO: Check if label already exists and fail
                symbols.insert(
                    sym.clone(),
                    instructions
                        .len()
                        .try_into()
                        .expect("couldn't downcast usize"),
                );
            }
            asm::AssemblyElement::AInstruction(a_inst) => {
                instructions.push(SourceLine {
                    lineno: inst.lineno,
                    item: asm::AssemblyInstruction::AInstruction(a_inst.clone()),
                });
            }
            asm::AssemblyElement::CInstruction(c_inst) => {
                instructions.push(SourceLine {
                    lineno: inst.lineno,
                    item: asm::AssemblyInstruction::CInstruction(c_inst.clone()),
                });
            }
        }
    }

    // Second pass: define other variables
    for inst in instructions.iter() {
        match &inst.item {
            asm::AssemblyInstruction::AInstruction(asm::SourceAInstruction::Symbol(sym)) => {
                // N.B. Symbols start at memory address 16
                let current_length: u16 = symbols
                    .len()
                    .try_into()
                    .expect("couldn't downcast to usize");
                let next_address: u16 = current_length + 16;
                symbols.entry(sym.clone()).or_insert(next_address);
            }
            _ => (),
        }
    }

    // Third pass: use defined variables
    instructions
        .iter()
        .map(|inst| {
            assembly_instruction_to_hack(inst, &symbols)
                .map(|inst| hack_instruction_to_string(&inst))
        })
        .collect()
}

fn hack_instruction_to_string(inst: &HackInstruction) -> String {
    match inst {
        HackInstruction::AInstruction(a_inst) => a_to_string(a_inst),
        HackInstruction::CInstruction(c_inst) => c_to_string(c_inst),
    }
}

fn a_to_string(inst: &AInstruction) -> String {
    bool_arrays_to_string(&[&[false], &inst.v])
}

fn c_to_string(inst: &CInstruction) -> String {
    bool_arrays_to_string(&[&[true, true, true], &inst.ac, &inst.d, &inst.j])
}

fn bool_arrays_to_string(bools: &[&[bool]]) -> String {
    let mut chars: Vec<char> = vec![];
    for &inner in bools {
        for b in inner {
            match b {
                false => chars.push('0'),
                true => chars.push('1'),
            }
        }
    }
    chars.iter().collect()
}

fn assembly_instruction_to_hack(
    inst: &SourceLine<asm::AssemblyInstruction>,
    symbols: &HashMap<Symbol, u16>,
) -> Result<HackInstruction, String> {
    let SourceLine { lineno, item } = inst;
    match item {
        asm::AssemblyInstruction::AInstruction(asm::SourceAInstruction::Number(num)) => {
            Ok(HackInstruction::AInstruction(AInstruction {
                v: a_num_to_binary(*num),
            }))
        }
        asm::AssemblyInstruction::AInstruction(asm::SourceAInstruction::Symbol(sym)) => {
            match symbols.get(sym) {
                Some(loc) => Ok(HackInstruction::AInstruction(AInstruction {
                    v: a_num_to_binary(*loc),
                })),
                None => Err(format!(
                    "line {lineno}: Don't support symbols yet ({})",
                    sym.0
                )),
            }
        }

        asm::AssemblyInstruction::CInstruction(asm::SourceCInstruction { dest, comp, jump }) => {
            Ok(HackInstruction::CInstruction(CInstruction {
                ac: c_comp_to_binary(comp),
                d: c_dest_to_binary(dest),
                j: c_jump_to_binary(jump),
            }))
        }
    }
}

/// Converts a decimal number from an A instruction into a binary array
fn a_num_to_binary(i: u16) -> [bool; 15] {
    let mut digits = [false; 15];
    let binary_str = format!("{i:15b}");

    if binary_str.len() != 15 {
        panic!("converting num {i} to binary produced result with len() != 15");
    }

    for (i, d) in binary_str.chars().enumerate() {
        digits[i] = d == '1';
    }
    digits
}

fn c_dest_to_binary(dest: &asm::SourceCDest) -> [bool; 3] {
    match dest {
        asm::SourceCDest::Null => [false, false, false],
        asm::SourceCDest::M => [false, false, true],
        asm::SourceCDest::D => [false, true, false],
        asm::SourceCDest::MD => [false, true, true],
        asm::SourceCDest::A => [true, false, false],
        asm::SourceCDest::AM => [true, false, true],
        asm::SourceCDest::AD => [true, true, false],
        asm::SourceCDest::AMD => [true, true, true],
    }
}

fn c_comp_to_binary(comp: &asm::SourceCComp) -> [bool; 7] {
    match comp {
        asm::SourceCComp::Zero => [false, true, false, true, false, true, false],
        asm::SourceCComp::One => [false, true, true, true, true, true, true],
        asm::SourceCComp::NegOne => [false, true, true, true, false, true, false],
        asm::SourceCComp::D => [false, false, false, true, true, false, false],
        asm::SourceCComp::A => [false, true, true, false, false, false, false],
        asm::SourceCComp::M => [true, true, true, false, false, false, false],
        asm::SourceCComp::NotD => [false, false, false, true, true, false, true],
        asm::SourceCComp::NotA => [false, true, true, false, false, false, true],
        asm::SourceCComp::NotM => [true, true, true, false, false, false, true],
        asm::SourceCComp::NegD => [false, false, false, true, true, true, true],
        asm::SourceCComp::NegA => [false, true, true, false, false, true, true],
        asm::SourceCComp::NegM => [true, true, true, false, false, true, true],
        asm::SourceCComp::DPlusOne => [false, false, true, true, true, true, true],
        asm::SourceCComp::APlusOne => [false, true, true, false, true, true, true],
        asm::SourceCComp::MPlusOne => [true, true, true, false, true, true, true],
        asm::SourceCComp::DMinusOne => [false, false, false, true, true, true, false],
        asm::SourceCComp::AMinusOne => [false, true, true, false, false, true, false],
        asm::SourceCComp::MMinusOne => [true, true, true, false, false, true, false],
        asm::SourceCComp::DPlusA => [false, false, false, false, false, true, false],
        asm::SourceCComp::DPlusM => [true, false, false, false, false, true, false],
        asm::SourceCComp::DMinusA => [false, false, true, false, false, true, true],
        asm::SourceCComp::DMinusM => [true, false, true, false, false, true, true],
        asm::SourceCComp::AMinusD => [false, false, false, false, true, true, true],
        asm::SourceCComp::MMinusD => [true, false, false, false, true, true, true],
        asm::SourceCComp::DAndA => [false, false, false, false, false, false, false],
        asm::SourceCComp::DAndM => [true, false, false, false, false, false, false],
        asm::SourceCComp::DOrA => [false, false, true, false, true, false, true],
        asm::SourceCComp::DOrM => [true, false, true, false, true, false, true],
    }
}

fn c_jump_to_binary(jump: &asm::SourceCJump) -> [bool; 3] {
    match jump {
        asm::SourceCJump::Null => [false, false, false],
        asm::SourceCJump::JGT => [false, false, true],
        asm::SourceCJump::JEQ => [false, true, false],
        asm::SourceCJump::JGE => [false, true, true],
        asm::SourceCJump::JLT => [true, false, false],
        asm::SourceCJump::JNE => [true, false, true],
        asm::SourceCJump::JLE => [true, true, false],
        asm::SourceCJump::JMP => [true, true, true],
    }
}
