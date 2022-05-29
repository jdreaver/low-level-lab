/// A `SourceLine` is a parsed line of assembly source code.
#[derive(Debug, PartialEq, Clone)]
pub struct SourceLine<A> {
    pub lineno: usize,
    pub item: A,
}
