use std::str::FromStr;

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

/// Parses a VM source file.
pub fn parse_vm_source(source: &String) -> Result<Vec<SourceLine<VMCommand>>, String> {
    let parsed: Result<Vec<Option<SourceLine<VMCommand>>>, String> = source
        .lines()
        .enumerate()
        .map(|(lineno, source_line)| match parse_vm_line(source_line) {
            Err(err) => Err(format!("error on line {lineno}: {err}")),
            Ok(Some(item)) => Ok(Some(SourceLine { lineno, item })),
            Ok(None) => Ok(None),
        })
        .collect();
    parsed.map(|p| p.into_iter().flatten().collect())
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
