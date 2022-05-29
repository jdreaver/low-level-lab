use super::asm;

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
    asm::parse_assembly_source(source)?
        .iter()
        .map(|inst| {
            assembly_instruction_to_hack(inst).map(|inst| hack_instruction_to_string(&inst))
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

fn assembly_instruction_to_hack(inst: &asm::SourceLine) -> Result<HackInstruction, String> {
    let asm::SourceLine { lineno, element } = inst;
    match element {
        asm::AssemblyElement::AInstruction(asm::SourceAInstruction::Number(num)) => {
            Ok(HackInstruction::AInstruction(AInstruction {
                v: a_num_to_binary(*num),
            }))
        }
        asm::AssemblyElement::AInstruction(asm::SourceAInstruction::Symbol(asm::Symbol(sym))) => {
            Err(format!("line {lineno}: Don't support symbols yet ({sym})"))
        }

        asm::AssemblyElement::CInstruction(asm::SourceCInstruction { dest, comp, jump }) => {
            Ok(HackInstruction::CInstruction(CInstruction {
                ac: c_comp_to_binary(comp),
                d: c_dest_to_binary(dest),
                j: c_jump_to_binary(jump),
            }))
        }

        _ => Err("line {lineno}: Unsupported instruction {element:#?}".to_string()),
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
