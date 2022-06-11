use super::instructions::{Instruction, Register, JxxFunction, OPqFunction, CmovxxFunction};

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
        self.split_next_byte()
            .ok_or(DisassembleError::NotEnoughInput)
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
                let ra = register_from_u8_or_err(ra_bits)?;
                let rb = register_from_u8_or_err(rb_bits)?;
                match fn_part {
                    0x0 => Ok(Some(Instruction::Rrmovq { ra, rb })),
                    _ => {
                        let f = cmovxx_fn_from_u8_or_err(fn_part)?;
                        Ok(Some(Instruction::Cmovxx { f, ra, rb }))
                    }
                }
            }
            0x3 => {
                let (ra_bits, rb_bits) = self.split_next_byte_or_err()?;
                if ra_bits != 0xF {
                    return Err(DisassembleError::Expected0xFRegBits { reg_bits: ra_bits });
                }
                let rb = register_from_u8_or_err(rb_bits)?;
                let v = self.next_little_endian_u64()?;
                Ok(Some(Instruction::Irmovq { rb, v }))
            }
            0x4 => {
                let (ra_bits, rb_bits) = self.split_next_byte_or_err()?;
                let ra = register_from_u8_or_err(ra_bits)?;
                let rb = register_from_u8_or_err(rb_bits)?;
                let d = self.next_little_endian_u64()?;
                Ok(Some(Instruction::Rmmovq { ra, rb, d }))
            }
            0x5 => {
                let (ra_bits, rb_bits) = self.split_next_byte_or_err()?;
                let ra = register_from_u8_or_err(ra_bits)?;
                let rb = register_from_u8_or_err(rb_bits)?;
                let d = self.next_little_endian_u64()?;
                Ok(Some(Instruction::Mrmovq { ra, rb, d }))
            }
            0x6 => {
                let f = opq_fn_from_u8_or_err(fn_part)?;
                let (ra_bits, rb_bits) = self.split_next_byte_or_err()?;
                let ra = register_from_u8_or_err(ra_bits)?;
                let rb = register_from_u8_or_err(rb_bits)?;
                Ok(Some(Instruction::OPq { f, ra, rb }))
            }
            0x7 => {
                let f = jxx_fn_from_u8_or_err(fn_part)?;
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
                let ra = register_from_u8_or_err(ra_bits)?;

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
                let ra = register_from_u8_or_err(ra_bits)?;

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
        Err(DisassembleError::UnknownInstructionCode { code_part: 0xF })
    );

    // Halt
    assert_eq!(disassemble(&vec![0x00]), Ok(vec![Instruction::Halt]));

    // Rrmovq
    assert_eq!(
        disassemble(&vec![0x20, 0x7D]),
        Ok(vec![Instruction::Rrmovq {
            ra: Register::Rdi,
            rb: Register::R13,
        }])
    );

    // Irmovq
    assert_eq!(
        disassemble(&vec![
            0x30, 0xFD, 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01
        ]),
        Ok(vec![Instruction::Irmovq {
            rb: Register::R13,
            v: 0x0123456789ABCDEF,
        }])
    );

    // Complex (example taken from book)
    let complex_bytes = vec![
        // 30f80800000000000000 | irmovq $8,%r8
        0x30, 0xf8, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // 30f90100000000000000 | irmovq $1,%r9
        0x30, 0xf9, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // 6300                 | xorq %rax,%rax
        0x63, 0x00, // 6266                 | andq %rsi,%rsi
        0x62, 0x66, // 708700000000000000   | jmp
        0x70, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // 805600000000000000   | call
        0x80, 0x56, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // 90                   | ret
        0x90,
    ];
    let complex_expect = vec![
        Instruction::Irmovq {
            rb: Register::R8,
            v: 8,
        },
        Instruction::Irmovq {
            rb: Register::R9,
            v: 1,
        },
        Instruction::OPq {
            f: OPqFunction::Xorq,
            ra: Register::Rax,
            rb: Register::Rax,
        },
        Instruction::OPq {
            f: OPqFunction::Andq,
            ra: Register::Rsi,
            rb: Register::Rsi,
        },
        Instruction::Jxx {
            f: JxxFunction::Jmp,
            dest: 0x87,
        },
        Instruction::Call {
            dest: 0x56,
        },
        Instruction::Ret,
    ];
    assert_eq!(disassemble(&complex_bytes), Ok(complex_expect));
}

fn register_from_u8_or_err(reg_bits: u8) -> Result<Register, DisassembleError> {
    Register::from_u8(reg_bits).ok_or(DisassembleError::InvalidRegister { reg_bits })
}

fn opq_fn_from_u8_or_err(fn_part: u8) -> Result<OPqFunction, DisassembleError> {
    OPqFunction::from_u8(fn_part).ok_or(DisassembleError::InvalidOPqFnCode { fn_part })
}

fn jxx_fn_from_u8_or_err(fn_part: u8) -> Result<JxxFunction, DisassembleError> {
    JxxFunction::from_u8(fn_part).ok_or(DisassembleError::InvalidJxxFnCode { fn_part })
}

fn cmovxx_fn_from_u8_or_err(fn_part: u8) -> Result<CmovxxFunction, DisassembleError> {
    CmovxxFunction::from_u8(fn_part).ok_or(DisassembleError::InvalidCmovxxFnCode { fn_part })
}
