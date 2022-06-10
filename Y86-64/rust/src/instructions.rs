/// All Y86-64 instructions.
#[derive(Debug, PartialEq)]
pub enum Instruction {
    Halt,
    Nop,
    // rrmovq rA, rB
    Rrmovq {
        ra: Register,
        rb: Register,
    },
    // irmovq V, rB
    Irmovq {
        rb: Register,
        v: u64,
    },
    // rmmovq rA, D(rB)
    Rmmovq {
        ra: Register,
        rb: Register,
        d: u64,
    },
    // mrmovq D(rB), rA
    Mrmovq {
        ra: Register,
        rb: Register,
        d: u64,
    },
    // OPq rA, rB
    OPq {
        f: OPqFunction,
        ra: Register,
        rb: Register,
    },
    // jXX Dest
    Jxx {
        f: JxxFunction,
        dest: u64,
    },
    // cmovXX rA, rB
    Cmovxx {
        f: CmovxxFunction,
        ra: Register,
        rb: Register,
    },
    // call Dest
    Call {
        dest: u64,
    },
    Ret,
    // pushq rA
    Pushq {
        ra: Register,
    },
    // popoq rA
    Popoq {
        ra: Register,
    },
}

/// 15 registers (in order, starting with index 0)
#[derive(Debug, PartialEq)]
pub enum Register {
    Rax,
    Rcx,
    Rdx,
    Rbx,
    Rsp,
    Rbp,
    Rsi,
    Rdi,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
}

impl Register {
    fn from_u8(value: u8) -> Option<Self> {
        match value {
            0x0 => Some(Self::Rax),
            0x1 => Some(Self::Rcx),
            0x2 => Some(Self::Rdx),
            0x3 => Some(Self::Rbx),
            0x4 => Some(Self::Rsp),
            0x5 => Some(Self::Rbp),
            0x6 => Some(Self::Rsi),
            0x7 => Some(Self::Rdi),
            0x8 => Some(Self::R8),
            0x9 => Some(Self::R9),
            0xA => Some(Self::R10),
            0xB => Some(Self::R11),
            0xC => Some(Self::R12),
            0xD => Some(Self::R13),
            0xE => Some(Self::R14),
            _ => None,
        }
    }

    fn from_last_4_bits(value: u8) -> Option<Self> {
        Self::from_u8(value & 0xF)
    }

    fn from_first_4_bits(value: u8) -> Option<Self> {
        Self::from_u8(value >> 4)
    }
}

#[derive(Debug, PartialEq)]
pub enum OPqFunction {
    Addq,
    Subq,
    Andq,
    Xorq,
}

#[derive(Debug, PartialEq)]
pub enum JxxFunction {
    Jmp,
    Jle,
    Jl,
    Je,
    Jne,
    Jge,
    Jg,
}

#[derive(Debug, PartialEq)]
pub enum CmovxxFunction {
    Cmovle,
    Cmovl,
    Cmove,
    Cmovne,
    Cmovge,
    Cmovg,
}

#[derive(Debug, PartialEq, Clone)]
pub enum DisassembleError {
    UnknownInstructionSequence { next_9_bytes: Vec<u8> },
}

/// Disassamble a stream of bytes into instructions.
pub fn disassemble(mut input: &[u8]) -> Result<Vec<Instruction>, DisassembleError> {
    let mut instructions = vec![];

    while let Some((rest, instruction)) = disassemble_next_instruction(input)? {
        input = rest;
        instructions.push(instruction);
    }

    Ok(instructions)
}

pub fn disassemble_next_instruction(
    input: &[u8],
) -> Result<Option<(&[u8], Instruction)>, DisassembleError> {
    let err = DisassembleError::UnknownInstructionSequence {
        next_9_bytes: input.iter().take(9).cloned().collect(),
    };

    match input {
        [] => Ok(None),
        [0x00, rest @ ..] => Ok(Some((rest, Instruction::Halt))),
        [0x10, rest @ ..] => Ok(Some((rest, Instruction::Nop))),
        [0x20, reg_byte, rest @ ..] => {
            let ra = Register::from_first_4_bits(*reg_byte).ok_or(err.clone())?;
            let rb = Register::from_last_4_bits(*reg_byte).ok_or(err)?;
            Ok(Some((rest, Instruction::Rrmovq { ra, rb })))
        }
        [0x30, reg_byte, v8, v7, v6, v5, v4, v3, v2, v1, rest @ ..] => {
            if reg_byte >> 4 != 0xF {
                return Err(err);
            }
            let rb = Register::from_last_4_bits(*reg_byte).ok_or(err)?;
            let v = u64::from_le_bytes([*v8, *v7, *v6, *v5, *v4, *v3, *v2, *v1]);
            Ok(Some((rest, Instruction::Irmovq { rb, v })))
        }
        [0x40, reg_byte, d8, d7, d6, d5, d4, d3, d2, d1, rest @ ..] => {
            let ra = Register::from_first_4_bits(*reg_byte).ok_or(err.clone())?;
            let rb = Register::from_last_4_bits(*reg_byte).ok_or(err)?;
            let d = u64::from_le_bytes([*d8, *d7, *d6, *d5, *d4, *d3, *d2, *d1]);
            Ok(Some((rest, Instruction::Rmmovq { ra, rb, d })))
        }
        [0x50, reg_byte, d8, d7, d6, d5, d4, d3, d2, d1, rest @ ..] => {
            let ra = Register::from_first_4_bits(*reg_byte).ok_or(err.clone())?;
            let rb = Register::from_last_4_bits(*reg_byte).ok_or(err)?;
            let d = u64::from_le_bytes([*d8, *d7, *d6, *d5, *d4, *d3, *d2, *d1]);
            Ok(Some((rest, Instruction::Mrmovq { ra, rb, d })))
        }
        _ => Err(err),
    }
}

#[test]
fn test_disassemble_next_instruction() {
    // Empty
    assert_eq!(disassemble_next_instruction(&vec![]), Ok(None));

    // Unknown
    assert_eq!(
        disassemble_next_instruction(&vec![0xF, 0x0, 0xA]),
        Err(DisassembleError::UnknownInstructionSequence {
            next_9_bytes: vec![0xF, 0x0, 0xA],
        })
    );

    // Halt (with extra)
    assert_eq!(
        disassemble_next_instruction(&vec![0x00, 0xF]),
        Ok(Some((&[0xF][..], Instruction::Halt)))
    );

    // Rrmovq
    assert_eq!(
        disassemble_next_instruction(&vec![0x20, 0x7D]),
        Ok(Some((
            &[][..],
            Instruction::Rrmovq {
                ra: Register::Rdi,
                rb: Register::R13,
            }
        )))
    );

    // Irmovq
    assert_eq!(
        disassemble_next_instruction(&vec![0x30, 0xFD, 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01]),
        Ok(Some((
            &[][..],
            Instruction::Irmovq {
                rb: Register::R13,
                v: 0x0123456789ABCDEF,
            }
        )))
    );
}
