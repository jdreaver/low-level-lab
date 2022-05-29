use std::str::FromStr;

use super::misc::SourceLine;

/// An `AssemblyElement` is a parsed line of assembly code (no empty lines or
/// comments included).
#[derive(Debug, PartialEq, Clone)]
pub enum AssemblyElement {
    AInstruction(SourceAInstruction),
    CInstruction(SourceCInstruction),
    Label(Symbol),
}

/// An `AssemblyInstruction` is just A and C instructions (no labels).
#[derive(Debug, PartialEq, Clone)]
pub enum AssemblyInstruction {
    AInstruction(SourceAInstruction),
    CInstruction(SourceCInstruction),
}

/// A `SourceAInstruction` is a decimal value ranging from 0 to 32767 (2^15 - 1)
/// or a symbol bound to such a decimal value.
#[derive(Debug, PartialEq, Clone)]
pub enum SourceAInstruction {
    Number(u16),
    Symbol(Symbol),
}

/// A symbol can be any sequence of letters, digits, underscore (_), dot (.),
/// dollar sign ($), and colon (:) that does not begin with a digit.
#[derive(Debug, PartialEq, Eq, Hash, Clone)]
pub struct Symbol(pub String);

impl FromStr for Symbol {
    type Err = String;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        // Sanity checking
        match s.chars().next() {
            None => return Err("Empty string for symbol".to_string()),
            Some(c) => {
                if c >= '0' && c <= '9' {
                    return Err(format!("Symbol '{s}' starts with digit"));
                }
            }
        }

        // Ensure we only use allowed characters
        for c in s.chars() {
            if c.is_ascii_alphanumeric() || c == '_' || c == '.' || c == '$' || c == ':' {
                continue;
            }
            return Err(format!("found disallowed character '{c}' in symbol '{s}'"));
        }

        Ok(Symbol(s.to_string()))
    }
}

/// A `SourceCInstruction` is a representation of a C instruction in source
/// code: `dest = comp; jump`
#[derive(Debug, PartialEq, Clone)]
pub struct SourceCInstruction {
    pub dest: SourceCDest,
    pub comp: SourceCComp,
    pub jump: SourceCJump,
}

/// A `SourceCDest` is the `dest` part of a C instruction represented in
/// assembler source code.
#[derive(Debug, PartialEq, Clone)]
pub enum SourceCDest {
    Null,
    M,
    D,
    MD,
    A,
    AM,
    AD,
    AMD,
}

impl FromStr for SourceCDest {
    type Err = String;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        match s {
            "M" => Ok(SourceCDest::M),
            "D" => Ok(SourceCDest::D),
            "MD" => Ok(SourceCDest::MD),
            "A" => Ok(SourceCDest::A),
            "AM" => Ok(SourceCDest::AM),
            "AD" => Ok(SourceCDest::AD),
            "AMD" => Ok(SourceCDest::AMD),
            "" => Err("Empty dest in C instruction".to_string()),
            _ => Err(format!("Unknown C instruction dest {s}")),
        }
    }
}

/// A `SourceCComp` is the textual representation of the `a` and `cccccc` parts
/// of a C instruction.
#[derive(Debug, PartialEq, Clone)]
pub enum SourceCComp {
    /// 0
    Zero,
    /// 1
    One,
    /// -1
    NegOne,

    /// D
    D,
    /// A
    A,
    /// M
    M,

    /// !D
    NotD,
    /// !A
    NotA,
    /// !M
    NotM,

    /// -D
    NegD,
    /// -A
    NegA,
    /// -M
    NegM,

    /// D+1
    DPlusOne,
    /// A+1
    APlusOne,
    /// M+1
    MPlusOne,

    /// D-1
    DMinusOne,
    /// A-1
    AMinusOne,
    /// M-1
    MMinusOne,

    /// D+A
    DPlusA,
    /// D+M
    DPlusM,

    /// D-A
    DMinusA,
    /// D-M
    DMinusM,

    /// A-D
    AMinusD,
    /// M-D
    MMinusD,

    /// D&A
    DAndA,
    /// D&M
    DAndM,

    /// D|A
    DOrA,
    /// D|M
    DOrM,
}

impl FromStr for SourceCComp {
    type Err = String;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        match s {
            "0" => Ok(SourceCComp::Zero),
            "1" => Ok(SourceCComp::One),
            "-1" => Ok(SourceCComp::NegOne),

            "D" => Ok(SourceCComp::D),
            "A" => Ok(SourceCComp::A),
            "M" => Ok(SourceCComp::M),

            "!D" => Ok(SourceCComp::NotD),
            "!A" => Ok(SourceCComp::NotA),
            "!M" => Ok(SourceCComp::NotM),

            "-D" => Ok(SourceCComp::NegD),
            "-A" => Ok(SourceCComp::NegA),
            "-M" => Ok(SourceCComp::NegM),

            "D+1" => Ok(SourceCComp::DPlusOne),
            "A+1" => Ok(SourceCComp::APlusOne),
            "M+1" => Ok(SourceCComp::MPlusOne),

            "D-1" => Ok(SourceCComp::DMinusOne),
            "A-1" => Ok(SourceCComp::AMinusOne),
            "M-1" => Ok(SourceCComp::MMinusOne),

            "D+A" => Ok(SourceCComp::DPlusA),
            "D+M" => Ok(SourceCComp::DPlusM),

            "D-A" => Ok(SourceCComp::DMinusA),
            "D-M" => Ok(SourceCComp::DMinusM),

            "A-D" => Ok(SourceCComp::AMinusD),
            "M-D" => Ok(SourceCComp::MMinusD),

            "D&A" => Ok(SourceCComp::DAndA),
            "D&M" => Ok(SourceCComp::DAndM),

            "D|A" => Ok(SourceCComp::DOrA),
            "D|M" => Ok(SourceCComp::DOrM),

            "" => Err("Empty comp in C instruction".to_string()),
            _ => Err(format!("Unknown C instruction comp {s}")),
        }
    }
}

/// A `SourceCJump` is the `jump` part of a C instruction represented in
/// assembler source code.
#[derive(Debug, PartialEq, Clone)]
pub enum SourceCJump {
    /// No jump
    Null,
    /// if comp > 0 jump
    JGT,
    /// if comp == 0 jump
    JEQ,
    /// if comp >= 0 jump
    JGE,
    /// if comp < 0 jump
    JLT,
    /// if comp != 0 jump
    JNE,
    /// if comp <= 0 jump
    JLE,
    /// unconditional jump
    JMP,
}

impl FromStr for SourceCJump {
    type Err = String;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        match s {
            "JGT" => Ok(SourceCJump::JGT),
            "JEQ" => Ok(SourceCJump::JEQ),
            "JGE" => Ok(SourceCJump::JGE),
            "JLT" => Ok(SourceCJump::JLT),
            "JNE" => Ok(SourceCJump::JNE),
            "JLE" => Ok(SourceCJump::JLE),
            "JMP" => Ok(SourceCJump::JMP),
            "" => Err("Empty jump in C instruction".to_string()),
            _ => Err(format!("Unknown C instruction jump {s}")),
        }
    }
}

/// Parses an assembly source file.
pub fn parse_assembly_source(source: &String) -> Result<Vec<SourceLine<AssemblyElement>>, String> {
    source
        .lines()
        .enumerate()
        .map(
            |(lineno, source_line)| match parse_assembly_line(source_line) {
                Err(err) => Err(format!("error on line {lineno}: {err}")),
                Ok(Some(item)) => Ok(Some(SourceLine { lineno, item })),
                Ok(None) => Ok(None),
            },
        )
        .collect::<Result<Vec<Option<SourceLine<AssemblyElement>>>, String>>()
        .map(|p| p.into_iter().flatten().collect())
}

#[test]
fn test_parse_assembly_source() {
    let src1 = "
// Comment yo

@123 // Comment after
(LOOP)
D=A
";
    let expect1 = vec![
        SourceLine {
            lineno: 3,
            item: AssemblyElement::AInstruction(SourceAInstruction::Number(123)),
        },
        SourceLine {
            lineno: 4,
            item: AssemblyElement::Label(Symbol("LOOP".to_string())),
        },
        SourceLine {
            lineno: 5,
            item: AssemblyElement::CInstruction(SourceCInstruction {
                dest: SourceCDest::D,
                comp: SourceCComp::A,
                jump: SourceCJump::Null,
            }),
        },
    ];

    assert_eq!(parse_assembly_source(&src1.to_string()), Ok(expect1));
}

/// Parses a line of source code into a `SourceElement`. If the line is empty or
/// just a comment, return `None`.
fn parse_assembly_line(line: &str) -> Result<Option<AssemblyElement>, String> {
    // Iterate over the line and split into space-delimited words. If we see a
    // comment (`//`), ignore everything after. There should only be one element
    // left.
    let instructions: Vec<&str> = line
        .trim()
        .split_whitespace()
        .take_while(|inst| !inst.starts_with("//"))
        .collect();
    let instruction: &str = match instructions.as_slice() {
        [] => return Ok(None),
        [inst] => inst,
        _ => return Err("Found multiple instructions on line.".to_string()),
    };

    let first_char = instruction
        .chars()
        .next()
        .expect("unexpected error: couldn't extract first char of instruction");

    match first_char {
        '@' => Ok(Some(AssemblyElement::AInstruction(parse_a_instruction(
            instruction,
        )?))),
        '(' => Ok(Some(AssemblyElement::Label(parse_label(instruction)?))),
        _ => Ok(Some(AssemblyElement::CInstruction(parse_c_instruction(
            instruction,
        )?))),
    }
}

fn parse_a_instruction(source: &str) -> Result<SourceAInstruction, String> {
    // Assert starts with @, just in case
    if source
        .chars()
        .next()
        .expect("tried to parse empty A instruction")
        != '@'
    {
        panic!("A instruction started with something besides '@'")
    }

    // Remove first @ char
    let mut chars = source.chars();
    chars.next();
    let body = chars.as_str();

    // Parse as number or as label
    match body.chars().next() {
        None => Err("empty A instruction".to_string()),
        Some(x) if x.is_numeric() => {
            match u16::from_str(body) {
                Ok(i) => {
                    if i < u16::pow(2, 15) {
                        Ok(SourceAInstruction::Number(i))
                    } else {
                        Err(format!("A instruction decimal '{x}' too large. Must be less than 2 ^ 15 = 32767"))
                    }
                }
                Err(err) => Err(format!("Error parsing A instruction decimal: {err}")),
            }
        }
        Some(_) => Ok(SourceAInstruction::Symbol(Symbol::from_str(body)?)),
    }
}

#[test]
fn test_parse_a_instruction() {
    assert_eq!(
        parse_a_instruction("@123"),
        Ok(SourceAInstruction::Number(123))
    );
    assert_eq!(
        parse_a_instruction("@hello"),
        Ok(SourceAInstruction::Symbol(Symbol("hello".to_string())))
    );

    assert_eq!(
        parse_a_instruction("@12354556"),
        Err(
            "Error parsing A instruction decimal: number too large to fit in target type"
                .to_string()
        )
    );
    assert_eq!(
        parse_a_instruction("@"),
        Err("empty A instruction".to_string())
    );
    assert_eq!(
        parse_a_instruction("@1abc"),
        Err("Error parsing A instruction decimal: invalid digit found in string".to_string())
    );
}

fn parse_label(source: &str) -> Result<Symbol, String> {
    // Assert starts and ends with parens
    if source.starts_with("(") && source.ends_with(")") {
        // Remove first and last characters (parens)
        let mut chars = source.chars();
        chars.next();
        chars.next_back();

        Symbol::from_str(chars.as_str())
    } else {
        Err(format!(
            "Label '{source}' doesn't start and end with parens"
        ))
    }
}

#[test]
fn test_parse_label() {
    assert_eq!(parse_label("(hello)"), Ok(Symbol("hello".to_string())));
    assert_eq!(parse_label("(h$_:.)"), Ok(Symbol("h$_:.".to_string())));

    assert_eq!(
        parse_label("(1abc)"),
        Err("Symbol '1abc' starts with digit".to_string())
    );
    assert_eq!(
        parse_label("()"),
        Err("Empty string for symbol".to_string())
    );
}

/// Parses a line of source code into a `SourceCInstruction`. Assumes no
/// whitespace in the instruction.
fn parse_c_instruction(source: &str) -> Result<SourceCInstruction, String> {
    // C instructions look like dest=comp;jump
    let (dest, compjump) = match source.split_once("=") {
        None => (SourceCDest::Null, source),
        Some((dest_str, compjump)) => (SourceCDest::from_str(dest_str)?, compjump),
    };

    let (comp_str, jump) = match compjump.split_once(";") {
        None => (compjump, SourceCJump::Null),
        Some((comp_str, jump_str)) => (comp_str, SourceCJump::from_str(jump_str)?),
    };

    let comp = SourceCComp::from_str(comp_str)?;

    Ok(SourceCInstruction { dest, comp, jump })
}

#[test]
fn test_parse_c_instruction() {
    assert_eq!(
        parse_c_instruction("D"),
        Ok(SourceCInstruction {
            dest: SourceCDest::Null,
            comp: SourceCComp::D,
            jump: SourceCJump::Null,
        })
    );

    assert_eq!(
        parse_c_instruction("AM=D+A;JEQ"),
        Ok(SourceCInstruction {
            dest: SourceCDest::AM,
            comp: SourceCComp::DPlusA,
            jump: SourceCJump::JEQ,
        })
    );

    assert_eq!(
        parse_c_instruction("BLAH"),
        Err("Unknown C instruction comp BLAH".to_string())
    );
    assert_eq!(
        parse_c_instruction("=D"),
        Err("Empty dest in C instruction".to_string())
    );
    assert_eq!(
        parse_c_instruction("1;"),
        Err("Empty jump in C instruction".to_string())
    );
    assert_eq!(
        parse_c_instruction("AD=;JEQ"),
        Err("Empty comp in C instruction".to_string())
    );
}
