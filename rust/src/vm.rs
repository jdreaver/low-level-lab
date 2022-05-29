use std::str::FromStr;
use std::string::ToString;

use super::misc::SourceLine;

/// All possible commands that can be used in a VM line of source code.
#[derive(Debug, PartialEq, Clone)]
pub enum VMCommand {
    Push(PushCommand),
    Pop(PopCommand),

    Add,
    Subtract,
    Negate,

    Equal,
    GreaterThan,
    LessThan,

    And,
    Or,
    Not,
}

/// Push a value of segment[index] onto the stack.
#[derive(Debug, PartialEq, Clone)]
pub struct PushCommand {
    segment: Segment,
    index: u16,
}

/// Pop the top stack value and store in segment[index].
#[derive(Debug, PartialEq, Clone)]
pub struct PopCommand {
    segment: Segment,
    index: u16,
}

#[derive(Debug, PartialEq, Clone)]
pub enum Segment {
    Argument,
    Local,
    Static,
    Constant,
    This,
    That,
    Pointer,
    Temp,
}

impl FromStr for Segment {
    type Err = String;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        match s {
            "argument" => Ok(Segment::Argument),
            "local" => Ok(Segment::Local),
            "static" => Ok(Segment::Static),
            "constant" => Ok(Segment::Constant),
            "this" => Ok(Segment::This),
            "that" => Ok(Segment::That),
            "pointer" => Ok(Segment::Pointer),
            "temp" => Ok(Segment::Temp),
            "" => Err("Empty string for segment".to_string()),
            _ => Err(format!("Unknown segment {s}")),
        }
    }
}

impl ToString for Segment {
    fn to_string(&self) -> String {
        match self {
            Segment::Argument => "argument".to_string(),
            Segment::Local => "local".to_string(),
            Segment::Static => "static".to_string(),
            Segment::Constant => "constant".to_string(),
            Segment::This => "this".to_string(),
            Segment::That => "that".to_string(),
            Segment::Pointer => "pointer".to_string(),
            Segment::Temp => "temp".to_string(),
        }
    }
}

/// Parses a VM source file.
pub fn parse_vm_source(source: &String) -> Result<Vec<SourceLine<VMCommand>>, String> {
    source
        .lines()
        .enumerate()
        .map(|(lineno, source_line)| match parse_vm_line(source_line) {
            Err(err) => Err(format!("error on line {lineno}: {err}")),
            Ok(Some(item)) => Ok(Some(SourceLine { lineno, item })),
            Ok(None) => Ok(None),
        })
        .collect::<Result<Vec<Option<SourceLine<VMCommand>>>, String>>()
        .map(|p| p.into_iter().flatten().collect())
}

#[test]
fn test_parse_vm_source() {
    let src1 = "
// Comment yo

push local 1 // Comment after
eq

gt
";
    let expect1 = vec![
        SourceLine {
            lineno: 3,
            item: VMCommand::Push(PushCommand {
                segment: Segment::Local,
                index: 1,
            }),
        },
        SourceLine {
            lineno: 4,
            item: VMCommand::Equal,
        },
        SourceLine {
            lineno: 6,
            item: VMCommand::GreaterThan,
        },
    ];

    assert_eq!(parse_vm_source(&src1.to_string()), Ok(expect1));
}

/// Parses a line of VM source code into a `VMCommand`. If the line is empty or
/// just a comment, return `None`.
fn parse_vm_line(line: &str) -> Result<Option<VMCommand>, String> {
    // Iterate over the line and split into space-delimited words. If we see a
    // comment (`//`), ignore everything after.
    let words: Vec<&str> = line
        .trim()
        .split_whitespace()
        .take_while(|inst| !inst.starts_with("//"))
        .collect();

    let (first_word, rest): (&str, &[&str]) = match words.split_first() {
        None => return Ok(None),
        Some((first, rest)) => (first, rest),
    };

    match first_word {
        "push" => {
            let (segment, index) = parse_push_pop_args("push", rest)?;
            Ok(Some(VMCommand::Push(PushCommand { segment, index })))
        }
        "pop" => {
            let (segment, index) = parse_push_pop_args("pop", rest)?;
            Ok(Some(VMCommand::Pop(PopCommand { segment, index })))
        }
        "add" => expect_no_args("add", VMCommand::Add, rest),
        "sub" => expect_no_args("sub", VMCommand::Subtract, rest),
        "neg" => expect_no_args("neg", VMCommand::Negate, rest),
        "eq" => expect_no_args("eq", VMCommand::Equal, rest),
        "gt" => expect_no_args("gt", VMCommand::GreaterThan, rest),
        "lt" => expect_no_args("lt", VMCommand::LessThan, rest),
        "and" => expect_no_args("and", VMCommand::And, rest),
        "or" => expect_no_args("or", VMCommand::Or, rest),
        "not" => expect_no_args("not", VMCommand::Not, rest),
        _ => Err("unknown VM command".to_string()),
    }
}

fn parse_push_pop_args(push_or_pop: &str, args: &[&str]) -> Result<(Segment, u16), String> {
    match args {
        [segment_str, index_str] => {
            let segment = Segment::from_str(segment_str)
                .map_err(|err| format!("error parsing {push_or_pop} segment: {err}"))?;
            let index = u16::from_str(index_str)
                .map_err(|err| format!("error parsing {push_or_pop} index: {err}"))?;
            Ok((segment, index))
        }
        _ => Err(format!(
            "expected <segment> <index> after {push_or_pop}, got {args:?}"
        )),
    }
}

fn expect_no_args<A>(command_str: &str, command: A, args: &[&str]) -> Result<Option<A>, String> {
    if args.len() == 0 {
        Ok(Some(command))
    } else {
        Err(format!(
            "expected no args after {command_str}, got got {args:?}"
        ))
    }
}

#[test]
fn test_parse_vm_line() {
    assert_eq!(
        parse_vm_line("push local 1"),
        Ok(Some(VMCommand::Push(PushCommand {
            segment: Segment::Local,
            index: 1,
        })))
    );

    assert_eq!(
        parse_vm_line("push nonsense"),
        Err("expected <segment> <index> after push, got [\"nonsense\"]".to_string())
    );

    assert_eq!(parse_vm_line("eq"), Ok(Some(VMCommand::Equal)));
    assert_eq!(
        parse_vm_line("eq stuff"),
        Err("expected no args after eq, got got [\"stuff\"]".to_string())
    );
}

/// Compiles VM commands to assembly source code.
pub fn vm_to_asm(commands: Vec<SourceLine<VMCommand>>) -> Result<Vec<String>, String> {
    commands
        .iter()
        .map(|cmd| vm_command_to_asm(cmd))
        .collect::<Result<Vec<Vec<String>>, String>>()
        .map(|vecs| vecs.into_iter().flatten().collect())
}

pub fn vm_command_to_asm(command: &SourceLine<VMCommand>) -> Result<Vec<String>, String> {
    match &command.item {
        VMCommand::Push(cmd) => push_to_asm(&cmd),
        VMCommand::Pop(_) => Err(format!("unsupported command {command:?}")),

        VMCommand::Add => binary_op_asm("add", vec!["D=D+M".to_string()]),
        VMCommand::Subtract => binary_op_asm("sub", vec!["D=D-M".to_string()]),
        VMCommand::Negate => unary_op_asm("neg", "M=-M".to_string()),

        VMCommand::Equal => Err(format!("unsupported command {command:?}")),
        VMCommand::GreaterThan => Err(format!("unsupported command {command:?}")),
        VMCommand::LessThan => Err(format!("unsupported command {command:?}")),

        VMCommand::And => binary_op_asm("and", vec!["D=D&M".to_string()]),
        VMCommand::Or => binary_op_asm("or", vec!["D=D|M".to_string()]),
        VMCommand::Not => unary_op_asm("not", "M=!M".to_string()),
    }
}

fn push_to_asm(command: &PushCommand) -> Result<Vec<String>, String> {
    let PushCommand { segment, index } = command;
    let segment_str = segment.to_string();

    let mut lines = vec![format!("// push {segment_str} {index}")];

    // Fetch the value from *(segment + index) and put into D
    let mut fetch_lines = match segment {
        Segment::Argument => asm_fetch_memory_offset("@ARG".to_string(), *index),
        Segment::Local => asm_fetch_memory_offset("@LCL".to_string(), *index),
        Segment::Static => asm_fetch_memory_offset("@16".to_string(), *index),
        Segment::This => asm_fetch_memory_offset("@THIS".to_string(), *index),
        Segment::That => asm_fetch_memory_offset("@THAT".to_string(), *index),
        Segment::Pointer => asm_fetch_memory_offset("@3".to_string(), *index),
        Segment::Temp => asm_fetch_memory_offset("@5".to_string(), *index),

        // Constant is a virtual segment, used to fetch constant values
        Segment::Constant => vec![
            format!("@{index}").to_string(),
            "D=A".to_string(),
        ],
    };
    lines.append(&mut fetch_lines);

    // Store D onto the stack
    lines.push("@SP".to_string()); // Set A to SP (RAM[0])
    lines.push("A=M".to_string()); // Follow pointer
    lines.push("M=D".to_string()); // Store D in pointer location

    // Increment SP
    lines.push("@SP".to_string());
    lines.push("M=M+1".to_string());

    Ok(lines)
}

/// ASM code to fetch memory from a given base address + offset to the D
/// register.
fn asm_fetch_memory_offset(base: String, offset: u16) -> Vec<String> {
    vec![
        // Set A to base
        base,
        // Set D to point to base memory location
        "D=M".to_string(),
        // Load offset into A and add to D
        format!("@{offset}"),
        "D=D+A".to_string(),
        // Follow the pointer by setting A to D and then reading M
        "A=D".to_string(),
        "D=M".to_string(),
    ]
}

/// Updates the top of the stack in-place with the given operation. When the
/// operation is applied, A is pointing at SP-1, so you can do the op on M.
fn unary_op_asm(op_name: &str, op_code: String) -> Result<Vec<String>, String> {
    Ok(vec![
        format!("// {op_name}"),
        // Point A at SP-1
        "@SP".to_string(),
        "A=M-1".to_string(),
        op_code,
    ])
}

/// Pops two items off the stack, applies the given operation, and pushes the
/// result back onto the stack. When the operation is applied, D contains the
/// value at *(SP-2) and A is at SP-1, so for example addition would be D=D+M.
fn binary_op_asm(op_name: &str, op_code: Vec<String>) -> Result<Vec<String>, String> {
    // Decrement SP by 1
    let mut lines = vec![
        format!("// {op_name}"),
        "@SP".to_string(),
        "M=M-1".to_string(),
    ];

    // First arg for operation is now at SP-1. Fetch that and put into D.
    lines.push("A=M-1".to_string());
    lines.push("D=M".to_string());

    // Get second arg at SP and perform op, storing result in D
    lines.push("A=A+1".to_string());
    lines.extend(op_code);

    // Store result in SP-1
    lines.push("@SP".to_string());
    lines.push("A=M-1".to_string());
    lines.push("M=D".to_string());

    Ok(lines)
}
