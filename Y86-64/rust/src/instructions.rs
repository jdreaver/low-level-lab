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
    pub fn from_u8(value: u8) -> Option<Self> {
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
}

#[derive(Debug, PartialEq)]
pub enum OPqFunction {
    Addq,
    Subq,
    Andq,
    Xorq,
}

impl OPqFunction {
    pub fn from_u8(value: u8) -> Option<Self> {
        match value {
            0x0 => Some(Self::Addq),
            0x1 => Some(Self::Subq),
            0x2 => Some(Self::Andq),
            0x3 => Some(Self::Xorq),
            _ => None,
        }
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
    pub fn from_u8(value: u8) -> Option<Self> {
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
   pub fn from_u8(value: u8) -> Option<Self> {
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
}
