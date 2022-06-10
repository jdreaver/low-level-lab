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
    // popq rA
    Popq {
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

    fn from_u8_or_err(reg_bits: u8) -> Result<Self, DisassembleError> {
        Self::from_u8(reg_bits).ok_or(DisassembleError::InvalidRegister { reg_bits })
    }
}

#[derive(Debug, PartialEq)]
pub enum OPqFunction {
    Addq,
    Subq,
    Andq,
    Xorq,
}

impl OPqFunction {
    fn from_u8(value: u8) -> Option<Self> {
        match value {
            0x0 => Some(Self::Addq),
            0x1 => Some(Self::Subq),
            0x2 => Some(Self::Andq),
            0x3 => Some(Self::Xorq),
            _ => None,
        }
    }

    fn from_u8_or_err(fn_part: u8) -> Result<Self, DisassembleError> {
        Self::from_u8(fn_part).ok_or(DisassembleError::InvalidOPqFnCode { fn_part })
    }
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

impl JxxFunction {
    fn from_u8(value: u8) -> Option<Self> {
        match value {
            0x0 => Some(Self::Jmp),
            0x1 => Some(Self::Jle),
            0x2 => Some(Self::Jl),
            0x3 => Some(Self::Je),
            0x4 => Some(Self::Jne),
            0x5 => Some(Self::Jge),
            0x6 => Some(Self::Jg),
            _ => None,
        }
    }

    fn from_u8_or_err(fn_part: u8) -> Result<Self, DisassembleError> {
        Self::from_u8(fn_part).ok_or(DisassembleError::InvalidJxxFnCode { fn_part })
    }
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

impl CmovxxFunction {
    fn from_u8(value: u8) -> Option<Self> {
        match value {
            0x1 => Some(Self::Cmovle),
            0x2 => Some(Self::Cmovl),
            0x3 => Some(Self::Cmove),
            0x4 => Some(Self::Cmovne),
            0x5 => Some(Self::Cmovge),
            0x6 => Some(Self::Cmovg),
            _ => None,
        }
    }

    fn from_u8_or_err(fn_part: u8) -> Result<Self, DisassembleError> {
        Self::from_u8(fn_part).ok_or(DisassembleError::InvalidCmovxxFnCode { fn_part })
    }
}

#[derive(Debug, PartialEq, Clone)]
pub enum DisassembleError {
    NotEnoughInput,
    InvalidInstruction,
    UnknownInstructionCode { code_part: u8 },
    InvalidRegister { reg_bits: u8 },
    Expected0xFRegBits { reg_bits: u8 },
    Expected0x0FnBits { fn_part: u8 },
    InvalidOPqFnCode { fn_part: u8 },
    InvalidJxxFnCode { fn_part: u8 },
    InvalidCmovxxFnCode { fn_part: u8 },
}

/// Disassamble a stream of bytes into instructions.
pub fn disassemble(input: &[u8]) -> Result<Vec<Instruction>, DisassembleError> {
    let mut disassembler = Disassembler::from_bytes(input);
    let mut instructions = vec![];

    while let Some(instruction) = disassembler.next_instruction()? {
        instructions.push(instruction);
    }

    Ok(instructions)
}

struct Disassembler {
    input: Vec<u8>,
    i: usize,
}

impl Disassembler {
    fn from_bytes(bytes: &[u8]) -> Self {
        let input = bytes.iter().cloned().collect();
        Disassembler { input, i: 0 }
    }

    fn next_byte(&mut self) -> Option<u8> {
        let x = self.input.get(self.i);
        self.i += 1;
        x.map(|x| *x)
    }

    fn next_byte_or_err(&mut self) -> Result<u8, DisassembleError> {
        self.next_byte().ok_or(DisassembleError::NotEnoughInput)
    }

    fn split_next_byte(&mut self) -> Option<(u8, u8)> {
        self.next_byte().map(|b| split_byte(b))
    }

    fn split_next_byte_or_err(&mut self) -> Result<(u8, u8), DisassembleError> {
        self.split_next_byte().ok_or(DisassembleError::NotEnoughInput)
    }

    fn next_little_endian_u64(&mut self) -> Result<u64, DisassembleError> {
        let x8 = self.next_byte_or_err()?;
        let x7 = self.next_byte_or_err()?;
        let x6 = self.next_byte_or_err()?;
        let x5 = self.next_byte_or_err()?;
        let x4 = self.next_byte_or_err()?;
        let x3 = self.next_byte_or_err()?;
        let x2 = self.next_byte_or_err()?;
        let x1 = self.next_byte_or_err()?;
        Ok(u64::from_le_bytes([x8, x7, x6, x5, x4, x3, x2, x1]))
    }

    fn next_instruction(&mut self) -> Result<Option<Instruction>, DisassembleError> {
        let (code_part, fn_part) = match self.split_next_byte() {
            None => return Ok(None),
            Some(x) => x,
        };

        match code_part {
            0x0 => Ok(Some(Instruction::Halt)),
            0x1 => Ok(Some(Instruction::Nop)),
            0x2 => {
                let (ra_bits, rb_bits) = self.split_next_byte_or_err()?;
                let ra = Register::from_u8_or_err(ra_bits)?;
                let rb = Register::from_u8_or_err(rb_bits)?;
                match fn_part {
                    0x0 => Ok(Some(Instruction::Rrmovq { ra, rb })),
                    _ => {
                        let f = CmovxxFunction::from_u8_or_err(fn_part)?;
                        Ok(Some(Instruction::Cmovxx { f, ra, rb }))
                    }
                }
            }
            0x3 => {
                let (ra_bits, rb_bits) = self.split_next_byte_or_err()?;
                if ra_bits != 0xF {
                    return Err(DisassembleError::Expected0xFRegBits { reg_bits: ra_bits });
                }
                let rb = Register::from_u8_or_err(rb_bits)?;
                let v = self.next_little_endian_u64()?;
                Ok(Some(Instruction::Irmovq { rb, v }))
            }
            0x4 => {
                let (ra_bits, rb_bits) = self.split_next_byte_or_err()?;
                let ra = Register::from_u8_or_err(ra_bits)?;
                let rb = Register::from_u8_or_err(rb_bits)?;
                let d = self.next_little_endian_u64()?;
                Ok(Some(Instruction::Rmmovq { ra, rb, d }))
            }
            0x5 => {
                let (ra_bits, rb_bits) = self.split_next_byte_or_err()?;
                let ra = Register::from_u8_or_err(ra_bits)?;
                let rb = Register::from_u8_or_err(rb_bits)?;
                let d = self.next_little_endian_u64()?;
                Ok(Some(Instruction::Mrmovq { ra, rb, d }))
            }
            0x6 => {
                let f = OPqFunction::from_u8_or_err(fn_part)?;
                let (ra_bits, rb_bits) = self.split_next_byte_or_err()?;
                let ra = Register::from_u8_or_err(ra_bits)?;
                let rb = Register::from_u8_or_err(rb_bits)?;
                Ok(Some(Instruction::OPq { f, ra, rb }))
            }
            0x7 => {
                let f = JxxFunction::from_u8_or_err(fn_part)?;
                let dest = self.next_little_endian_u64()?;
                Ok(Some(Instruction::Jxx { f, dest }))
            }
            0x8 => {
                if fn_part != 0x0 {
                    return Err(DisassembleError::Expected0x0FnBits { fn_part });
                }
                let dest = self.next_little_endian_u64()?;
                Ok(Some(Instruction::Call { dest }))
            }
            0x9 => {
                if fn_part != 0x0 {
                    return Err(DisassembleError::Expected0x0FnBits { fn_part });
                }
                Ok(Some(Instruction::Ret))
            }
            0xA => {
                if fn_part != 0x0 {
                    return Err(DisassembleError::Expected0x0FnBits { fn_part });
                }

                let (ra_bits, rb_bits) = self.split_next_byte_or_err()?;
                if rb_bits != 0xF {
                    return Err(DisassembleError::Expected0xFRegBits { reg_bits: rb_bits });
                }
                let ra = Register::from_u8_or_err(ra_bits)?;

                Ok(Some(Instruction::Pushq { ra }))
            }
            0xB => {
                if fn_part != 0x0 {
                    return Err(DisassembleError::Expected0x0FnBits { fn_part });
                }

                let (ra_bits, rb_bits) = self.split_next_byte_or_err()?;
                if rb_bits != 0xF {
                    return Err(DisassembleError::Expected0xFRegBits { reg_bits: rb_bits });
                }
                let ra = Register::from_u8_or_err(ra_bits)?;

                Ok(Some(Instruction::Popq { ra }))
            }
            _ => Err(DisassembleError::UnknownInstructionCode { code_part }),
        }
    }
}

// Splits a byte into the upper 4 and lower 4 bits
pub fn split_byte(x: u8) -> (u8, u8) {
    (x >> 4, x & 0xF)
}

#[test]
fn test_disassemble() {
    // Empty
    assert_eq!(disassemble(&vec![]), Ok(vec![]));

    // Unknown
    assert_eq!(
        disassemble(&vec![0xFA]),
        Err(DisassembleError::UnknownInstructionCode {
            code_part: 0xF,
        })
    );

    // Halt
    assert_eq!(
        disassemble(&vec![0x00]),
        Ok(vec![Instruction::Halt])
    );

    // Rrmovq
    assert_eq!(
        disassemble(&vec![0x20, 0x7D]),
        Ok(vec![
            Instruction::Rrmovq {
                ra: Register::Rdi,
                rb: Register::R13,
            }
        ])
    );

    // Irmovq
    assert_eq!(
        disassemble(&vec![
            0x30, 0xFD, 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01
        ]),
        Ok(vec![
            Instruction::Irmovq {
                rb: Register::R13,
                v: 0x0123456789ABCDEF,
            }
        ])
    );
}
