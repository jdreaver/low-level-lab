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
    // Counter used for jump labels for comparison operations
    let mut comparison_jump_label_counter = 0;

    commands
        .iter()
        .map(|cmd| vm_command_to_asm(cmd, &mut comparison_jump_label_counter))
        .collect::<Result<Vec<Vec<String>>, String>>()
        .map(|vecs| {
            // Flatten Vec<Vec<...>> into Vec<...>
            let mut parsed: Vec<String> = vecs.into_iter().flatten().collect();

            // Add footer that ends program with infinite loop
            parsed.push("(_vm_END)".to_string());
            parsed.push("@_vm_END".to_string());
            parsed.push("0;JMP".to_string());

            parsed
        })
}

pub fn vm_command_to_asm(
    command: &SourceLine<VMCommand>,
    comparison_jump_label_counter: &mut usize,
) -> Result<Vec<String>, String> {
    match &command.item {
        VMCommand::Push(cmd) => push_to_asm(&cmd),
        VMCommand::Pop(cmd) => pop_to_asm(&cmd),

        VMCommand::Add => binary_op_asm("add", vec!["D=D+M".to_string()]),
        VMCommand::Subtract => binary_op_asm("sub", vec!["D=D-M".to_string()]),
        VMCommand::Negate => unary_op_asm("neg", "M=-M".to_string()),

        VMCommand::Equal => binary_op_asm(
            "eq",
            comparison_op_asm("JEQ", comparison_jump_label_counter),
        ),
        VMCommand::GreaterThan => binary_op_asm(
            "lt",
            comparison_op_asm("JGT", comparison_jump_label_counter),
        ),
        VMCommand::LessThan => binary_op_asm(
            "gt",
            comparison_op_asm("JLT", comparison_jump_label_counter),
        ),

        VMCommand::And => binary_op_asm("and", vec!["D=D&M".to_string()]),
        VMCommand::Or => binary_op_asm("or", vec!["D=D|M".to_string()]),
        VMCommand::Not => unary_op_asm("not", "M=!M".to_string()),
    }
}

fn push_to_asm(command: &PushCommand) -> Result<Vec<String>, String> {
    let PushCommand { segment, index } = command;
    let segment_str = segment.to_string();

    let mut lines = vec![format!("// push {segment_str} {index}")];

    // Compute *(segment + index) and store in D
    lines.append(&mut asm_fetch_segment(segment, *index));

    // Follow the pointer by setting A to D and then reading M
    if *segment != Segment::Constant {
        lines.push("A=D".to_string());
        lines.push("D=M".to_string());
    }

    // Store D onto the stack
    lines.push("@SP".to_string()); // Set A to SP (RAM[0])
    lines.push("A=M".to_string()); // Follow pointer
    lines.push("M=D".to_string()); // Store D in pointer location

    // Increment SP
    lines.push("@SP".to_string());
    lines.push("M=M+1".to_string());

    Ok(lines)
}

/// ASM code to compute location of given memory segment and index and store
/// result in D.
fn asm_fetch_segment(segment: &Segment, index: u16) -> Vec<String> {
    match segment {
        Segment::Argument => asm_fetch_memory_offset("@ARG".to_string(), index),
        Segment::Local => asm_fetch_memory_offset("@LCL".to_string(), index),
        Segment::This => asm_fetch_memory_offset("@THIS".to_string(), index),
        Segment::That => asm_fetch_memory_offset("@THAT".to_string(), index),

        // TODO: This mapping doesn't take filename into account, which might be
        // a problem later. Here is what the book says on the topic:
        //
        // Static variables are mapped on addresses 16 to 255 of the host RAM.
        // The VM translator can realize this mapping automatically, as follows.
        // Each reference to static i in a VM program stored in file Foo.vm can
        // be translated to the assembly symbol Foo.i. According to the Hack
        // machine language specification (chapter 6), the Hack assembler will
        // map these symbolic variables on the host RAM, starting at address 16.
        // This convention will cause the static variables that appear in a VM
        // program to be mapped on addresses 16 and onward, in the order in
        // which they appear in the VM code. For example, suppose that a VM
        // program starts with the code push constant 100, push constant 200,
        // pop static 5, pop static 2. The translation scheme described above
        // will cause static 5 and static 2 to be mapped on RAM addresses 16 and
        // 17, respectively
        Segment::Static => vec![
            format!("@_vm_static{index}"),
            "D=A".to_string(),
        ],

        // pointer is RAM[3-4]
        Segment::Pointer => {
            let location = 3 + index;
            vec![format!("@{location}").to_string(), "D=A".to_string()]
        }

        // temp is always RAM[5-12]
        Segment::Temp => {
            let location = 5 + index;
            vec![format!("@{location}").to_string(), "D=A".to_string()]
        }

        // Constant is a virtual segment, used to fetch constant values
        Segment::Constant => vec![format!("@{index}").to_string(), "D=A".to_string()],
    }
}

/// ASM code to compute memory segment with offset and store in D
fn asm_fetch_memory_offset(base: String, offset: u16) -> Vec<String> {
    vec![
        // Set A to base
        base,
        // Set D to point to base memory location
        "D=M".to_string(),
        // Load offset into A and add to D
        format!("@{offset}"),
        "D=D+A".to_string(),
    ]
}

fn pop_to_asm(command: &PopCommand) -> Result<Vec<String>, String> {
    let PopCommand { segment, index } = command;
    let segment_str = segment.to_string();

    let mut lines = vec![format!("// pop {segment_str} {index}")];

    // Compute *(segment + index) and store in D
    lines.append(&mut asm_fetch_segment(segment, *index));

    // Store segment target in R13 for now
    lines.push("@R13".to_string());
    lines.push("M=D".to_string());

    // Decrement SP and point A there
    lines.push("@SP".to_string());
    lines.push("AM=M-1".to_string());

    // Store the popped value in D, and then R13
    lines.push("D=M".to_string());
    lines.push("@R13".to_string());
    lines.push("A=M".to_string());
    lines.push("M=D".to_string());

    Ok(lines)
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

/// Implements necessary ASM to compute jump results and stores value in D.
/// Assumes that before this ASM is entered, D contains the value at *(SP-2) and
/// A is at SP-1, like inside `binary_op_asm`.
fn comparison_op_asm(jump_op: &str, comparison_jump_label_counter: &mut usize) -> Vec<String> {
    let success_label = format!("_vm_comp_success_{comparison_jump_label_counter}");
    let fail_label = format!("_vm_comp_fail_{comparison_jump_label_counter}");
    let end_label = format!("_vm_comp_end_{comparison_jump_label_counter}");
    let lines = vec![
        // Run comparison operation on D and M and jump to appropriate location
        "D=D-M".to_string(),
        format!("@{success_label}"),
        format!("D;{jump_op}"),
        format!("@{fail_label}"),
        "0;JMP".to_string(),
        // If the comparison operation succeeds, then jump here and we will set
        // *(SP-1) to 0b1111... (which is -1 in decimal).
        format!("({success_label})"),
        "D=-1".to_string(),
        format!("@{end_label}"),
        "0;JMP".to_string(),
        // If the comparison operation fails, then jump here and we will set
        // *(SP-1) to 0.
        format!("({fail_label})"),
        "D=0".to_string(),
        // End label, just to continue execution
        format!("({end_label})"),
    ];

    *comparison_jump_label_counter += 1;

    lines
}
