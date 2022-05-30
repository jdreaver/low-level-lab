use std::str::FromStr;
use std::string::ToString;

use super::misc::{SourceLine, Symbol};

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

    Label(Symbol),
    Goto(Symbol),
    IfGoto(Symbol),

    Function(FunctionCommand),
    Call(CallCommand),
    Return,
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

/// Define a function with a given label and number of local vars.
#[derive(Debug, PartialEq, Clone)]
pub struct FunctionCommand {
    pub name: Symbol,
    pub nvars: u16,
}

/// Call a function with a given label and number of args.
#[derive(Debug, PartialEq, Clone)]
pub struct CallCommand {
    pub name: Symbol,
    pub nargs: u16,
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
        "label" => {
            let symbol = single_label_arg("label", rest)?;
            Ok(Some(VMCommand::Label(symbol)))
        }
        "goto" => {
            let symbol = single_label_arg("goto", rest)?;
            Ok(Some(VMCommand::Goto(symbol)))
        }
        "if-goto" => {
            let symbol = single_label_arg("if-goto", rest)?;
            Ok(Some(VMCommand::IfGoto(symbol)))
        }
        "function" => {
            let (name, nvars) = parse_function_args("function", "nvars", rest)?;
            Ok(Some(VMCommand::Function(FunctionCommand { name, nvars })))
        }
        "call" => {
            let (name, nargs) = parse_function_args("function", "nargs", rest)?;
            Ok(Some(VMCommand::Call(CallCommand { name, nargs })))
        }
        "return" => expect_no_args("return", VMCommand::Return, rest),
        _ => Err(format!("unknown VM command {first_word}")),
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

fn single_label_arg(command_str: &str, args: &[&str]) -> Result<Symbol, String> {
    match args {
        [label] => {
            let symbol = Symbol::from_str(label)
                .map_err(|err| format!("error {command_str} label symbol: {err}"))?;
            Ok(symbol)
        }
        _ => Err(format!(
            "expected just one arg to {command_str}, but got {args:?}"
        )),
    }
}

fn parse_function_args(
    command_str: &str,
    nvars_or_nargs: &str,
    args: &[&str],
) -> Result<(Symbol, u16), String> {
    match args {
        [name_str, nargs_str] => {
            let name = Symbol::from_str(name_str)
                .map_err(|err| format!("error parsing {command_str} function name: {err}"))?;
            let nargs = u16::from_str(nargs_str)
                .map_err(|err| format!("error parsing {command_str} {nvars_or_nargs}: {err}"))?;
            Ok((name, nargs))
        }
        _ => Err(format!(
            "expected <name> <{nvars_or_nargs}> after {command_str}, got {args:?}"
        )),
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
pub fn vm_to_asm(
    filename: &String,
    commands: Vec<SourceLine<VMCommand>>,
) -> Result<Vec<String>, String> {
    // Used to prefix labels
    let mut current_function_name: String = "".to_string();

    // Counter used for jump labels for comparison operations
    let mut label_counter = 0;

    commands
        .iter()
        .map(|cmd| {
            vm_command_to_asm(
                filename,
                cmd,
                &mut current_function_name,
                &mut label_counter,
            )
        })
        .collect::<Result<Vec<Vec<String>>, String>>()
        .map(|vecs| vecs.into_iter().flatten().collect())
}

pub fn vm_command_to_asm(
    filename: &String,
    command: &SourceLine<VMCommand>,
    current_function_name: &mut String,
    label_counter: &mut usize,
) -> Result<Vec<String>, String> {
    match &command.item {
        VMCommand::Push(cmd) => push_to_asm(filename, &cmd),
        VMCommand::Pop(cmd) => pop_to_asm(filename, &cmd),

        VMCommand::Add => binary_op_asm("add", vec!["D=D+M".to_string()]),
        VMCommand::Subtract => binary_op_asm("sub", vec!["D=D-M".to_string()]),
        VMCommand::Negate => unary_op_asm("neg", "M=-M".to_string()),

        VMCommand::Equal => binary_op_asm("eq", comparison_op_asm("JEQ", label_counter)),
        VMCommand::GreaterThan => binary_op_asm("lt", comparison_op_asm("JGT", label_counter)),
        VMCommand::LessThan => binary_op_asm("gt", comparison_op_asm("JLT", label_counter)),

        VMCommand::And => binary_op_asm("and", vec!["D=D&M".to_string()]),
        VMCommand::Or => binary_op_asm("or", vec!["D=D|M".to_string()]),
        VMCommand::Not => unary_op_asm("not", "M=!M".to_string()),

        VMCommand::Label(Symbol(sym)) => Ok(vec![format!("({current_function_name}${sym})")]),
        VMCommand::Goto(Symbol(sym)) => Ok(vec![
            format!("@{current_function_name}${sym}"),
            "0;JMP".to_string(),
        ]),
        VMCommand::IfGoto(Symbol(sym)) => Ok(vec![
            format!("// if-goto {sym}"),
            // Pop stack: Point A at SP-1 and decrement SP
            "@SP".to_string(),
            "AM=M-1".to_string(),
            // Store M into D and do comparison
            "D=M".to_string(),
            format!("@{current_function_name}${sym}"),
            "D;JNE".to_string(),
        ]),

        VMCommand::Function(FunctionCommand { name, nvars }) => {
            // Change the current function name
            let Symbol(name_str) = name;
            *current_function_name = name_str.clone();

            // Add label for function. N.B. It appears the convention is for
            // function names to already include the file name, at least
            // according to the example VM commands I see.
            let mut lines = vec![
                format!("// function {name_str} {nvars}"),
                format!("({name_str})"),
            ];
            lines.extend((0..*nvars).flat_map(|i| {
                vec![
                    format!("// push local {i}"),
                    // Get value from LCL + offset and store in D
                    "@LCL".to_string(),
                    "D=M".to_string(),
                    format!("@{i}"),
                    "A=D+A".to_string(),
                    "D=M".to_string(),
                    // Store D onto the stack
                    "@SP".to_string(), // Set A to SP (RAM[0])
                    "A=M".to_string(), // Follow pointer
                    "M=D".to_string(), // Store D in pointer location
                    // Increment SP
                    "@SP".to_string(),
                    "M=M+1".to_string(),
                ]
            }));

            Ok(lines)
        }
        VMCommand::Call(CallCommand { name, nargs }) => {
            let Symbol(name_str) = name;
            // TODO: This uses the global label counter, but the spec says each
            // function should have its own label counter.
            let return_label = format!("{current_function_name}$ret.{label_counter}");

            // Reusable code for pushing A to the stack
            let push_d_to_stack = vec![
                "@SP".to_string(),
                "A=M".to_string(),
                "M=D".to_string(),
                "@SP".to_string(),
                "M=M+1".to_string(),
            ];

            let mut lines = vec![format!("// call {name_str} {nargs}")];

            // push returnAddress, generate a label and push it to the stack
            lines.push(format!("@{return_label}"));
            lines.push("D=A".to_string());
            lines.extend(push_d_to_stack.clone());

            // push LCL, save LCL of caller
            lines.push("@LCL".to_string());
            lines.push("D=M".to_string());
            lines.extend(push_d_to_stack.clone());

            // push ARG, save ARG of caller
            lines.push("@ARG".to_string());
            lines.push("D=M".to_string());
            lines.extend(push_d_to_stack.clone());

            // push THIS, save THIS of caller
            lines.push("@THIS".to_string());
            lines.push("D=M".to_string());
            lines.extend(push_d_to_stack.clone());

            // push THAT, save THAT of caller
            lines.push("@THAT".to_string());
            lines.push("D=M".to_string());
            lines.extend(push_d_to_stack.clone());

            // ARG = SP - 5 - nargs, reposition ARG
            lines.push("@SP".to_string());
            lines.push("D=M".to_string());
            lines.push(format!("@{}", 5 + nargs));
            lines.push("D=D-A".to_string());
            lines.push("@ARG".to_string());
            lines.push("M=D".to_string());

            // LCL = SP, reposition LCL
            lines.push("@SP".to_string());
            lines.push("D=M".to_string());
            lines.push("@LCL".to_string());
            lines.push("M=D".to_string());

            // goto f
            lines.push(format!("@{name_str}"));
            lines.push("0;JMP".to_string());

            // Record the return address label
            lines.push(format!("({return_label})"));

            *label_counter += 1;

            Ok(lines)
        }
        VMCommand::Return => Ok(vec![
            "// return".to_string(),
            // frame = LCL, store frame as temporary variable in R13
            "@LCL".to_string(),
            "D=M".to_string(),
            "@R13".to_string(),
            "M=D".to_string(),
            // retAddr = *(frame - 5), store return address in R14
            "@5".to_string(),
            "A=D-A".to_string(), // D still points to frame
            "D=M".to_string(),
            "@R14".to_string(),
            "M=D".to_string(),
            // *ARG = pop(), reposition return value for caller
            "@SP".to_string(), // First store top of stack in D
            "A=M-1".to_string(),
            "D=M".to_string(),
            "@ARG".to_string(), // Then store D in *ARG
            "A=M".to_string(),
            "M=D".to_string(),
            // SP = ARG + 1, reposition SP for caller
            "D=A+1".to_string(), // A is still pointing to ARG
            "@SP".to_string(),
            "M=D".to_string(),
            // THAT = *(frame - 1), restores THAT for caller
            "@R13".to_string(),
            "D=M".to_string(), // D = frame
            "@1".to_string(),
            "A=D-A".to_string(),
            "D=M".to_string(), // D = *(frame - 1)
            "@THAT".to_string(),
            "M=D".to_string(),
            // THIS = *(frame - 2), restores THIS for caller
            "@R13".to_string(),
            "D=M".to_string(), // D = frame
            "@2".to_string(),
            "A=D-A".to_string(),
            "D=M".to_string(), // D = *(frame - 2)
            "@THIS".to_string(),
            "M=D".to_string(),
            // ARG = *(frame - 3), restores ARG for caller
            "@R13".to_string(),
            "D=M".to_string(), // D = frame
            "@3".to_string(),
            "A=D-A".to_string(),
            "D=M".to_string(), // D = *(frame - 3)
            "@ARG".to_string(),
            "M=D".to_string(),
            // LCL = *(frame - 4), restores LCL for caller
            "@R13".to_string(),
            "D=M".to_string(), // D = frame
            "@4".to_string(),
            "A=D-A".to_string(),
            "D=M".to_string(), // D = *(frame - 4)
            "@LCL".to_string(),
            "M=D".to_string(),
            // goto retAddr
            "@R14".to_string(),
            "A=M".to_string(),
            "0;JMP".to_string(),
        ]),
    }
}

fn push_to_asm(filename: &String, command: &PushCommand) -> Result<Vec<String>, String> {
    let PushCommand { segment, index } = command;
    let segment_str = segment.to_string();

    let mut lines = vec![format!("// push {segment_str} {index}")];

    // Compute *(segment + index) and store in A
    lines.append(&mut asm_fetch_segment(filename, segment, *index));

    if *segment == Segment::Constant {
        lines.push("D=A".to_string());
    } else {
        // Follow the pointer by reading M into D
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
/// result in A.
fn asm_fetch_segment(filename: &String, segment: &Segment, index: u16) -> Vec<String> {
    match segment {
        Segment::Argument => asm_fetch_memory_offset("@ARG".to_string(), index),
        Segment::Local => asm_fetch_memory_offset("@LCL".to_string(), index),
        Segment::This => asm_fetch_memory_offset("@THIS".to_string(), index),
        Segment::That => asm_fetch_memory_offset("@THAT".to_string(), index),

        // Static variables are mapped on addresses 16 to 255 of the host RAM.
        // The book says each reference to static i in a VM program stored in
        // file Foo.vm can be translated to the assembly symbol Foo.i, so the
        // Hack assembler will map these symbolic variables on the host RAM,
        // starting at address 16. This convention will cause the static
        // variables that appear in a VM program to be mapped on addresses 16
        // and onward, in the order in which they appear in the VM code. For
        // example, suppose that a VM program starts with the code push constant
        // 100, push constant 200, pop static 5, pop static 2. The translation
        // scheme described above will cause static 5 and static 2 to be mapped
        // on RAM addresses 16 and 17, respectively.
        Segment::Static => vec![format!("@{filename}.{index}")],

        // pointer is RAM[3-4]
        Segment::Pointer => {
            let location = 3 + index;
            vec![format!("@{location}").to_string()]
        }

        // temp is always RAM[5-12]
        Segment::Temp => {
            let location = 5 + index;
            vec![format!("@{location}").to_string()]
        }

        // Constant is a virtual segment, used to fetch constant values
        Segment::Constant => vec![format!("@{index}").to_string()],
    }
}

/// ASM code to compute memory segment with offset and store in D
fn asm_fetch_memory_offset(base: String, offset: u16) -> Vec<String> {
    // Set A to base
    let mut lines = vec![base];
    if offset == 0 {
        // Follow pointer, store in A
        lines.push("A=M".to_string());
    } else {
        // Set D to point to base memory location so we can add to it
        lines.push("D=M".to_string());
        // Load offset into A and add to base from D
        lines.push(format!("@{offset}"));
        lines.push("A=D+A".to_string());
    }
    lines
}

fn pop_to_asm(filename: &String, command: &PopCommand) -> Result<Vec<String>, String> {
    let PopCommand { segment, index } = command;
    let segment_str = segment.to_string();

    let mut lines = vec![format!("// pop {segment_str} {index}")];

    // Compute *(segment + index) and store in A
    lines.append(&mut asm_fetch_segment(filename, segment, *index));

    // Store segment target in R13 for now
    lines.push("D=A".to_string());
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
fn comparison_op_asm(jump_op: &str, label_counter: &mut usize) -> Vec<String> {
    let success_label = format!("_vm_comp_success_{label_counter}");
    let fail_label = format!("_vm_comp_fail_{label_counter}");
    let end_label = format!("_vm_comp_end_{label_counter}");
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

    *label_counter += 1;

    lines
}
