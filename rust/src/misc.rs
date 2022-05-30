use std::str::FromStr;

/// A `SourceLine` is a parsed line of assembly source code.
#[derive(Debug, PartialEq, Clone)]
pub struct SourceLine<A> {
    pub lineno: usize,
    pub item: A,
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
