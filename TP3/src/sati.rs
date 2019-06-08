use std::collections::HashMap;
use std::ffi::CStr;
use std::fs::File;
use std::io::{stdout, Write};
use std::os::raw::{c_char, c_int};

const HEADER: &str = include_str!("../assets/header.tex");
const FOOTER: &str = include_str!("../assets/footer.tex");

#[derive(Default)]
pub struct Sati {
    dictionary: HashMap<String, Word>,
    current_word: Option<String>,
    untitled_number: usize,
    output: Option<Box<dyn Write>>,
    wrapped: bool,
}

#[derive(Debug)]
struct Word {
    wd: String,
    meaning: Option<String>,
    english_name: Option<String>,
    synonyms: Vec<String>,
}

pub enum SatiError {
    NoCurrentWord = 1,
    WordAlreadyDefined = 2,
    MeaningAlreadyDefined = 3,
    EnglishNameAlreadyDefined = 4,
    IOError = 5,
}

impl From<std::io::Error> for SatiError {
    fn from(_: std::io::Error) -> Self {
        SatiError::IOError
    }
}

impl Sati {
    fn new(wrapped: bool) -> Result<Sati, SatiError> {
        if wrapped {
            writeln!(std::io::stdout(), "{}", HEADER).map_err(SatiError::from)?;
        }
        Ok(Self {
            output: Some(Box::new(stdout())),
            wrapped,
            ..Default::default()
        })
    }

    fn new_with_output(mut file: File, wrapped: bool) -> Result<Sati, SatiError> {
        if wrapped {
            writeln!(file, "{}", HEADER).map_err(SatiError::from)?;
        }
        Ok(Self {
            output: Some(Box::new(file)),
            wrapped,
            ..Default::default()
        })
    }

    fn new_split(wrapped: bool) -> Sati {
        Self {
            wrapped,
            ..Default::default()
        }
    }

    fn current_word(&mut self) -> Result<&mut Word, SatiError> {
        let cw = self.current_word.as_ref().ok_or(SatiError::NoCurrentWord)?;
        Ok(self.dictionary.get_mut(cw).unwrap())
    }

    fn add_word(&mut self, word: String) -> Result<(), SatiError> {
        if self.dictionary.contains_key(word.to_uppercase().as_str()) {
            Err(SatiError::WordAlreadyDefined)
        } else {
            let w = Word::new(word);
            let key = w.wd.to_uppercase();
            self.current_word = Some(key.clone());
            self.dictionary.insert(key, w);
            Ok(())
        }
    }

    fn add_meaning(&mut self, meaning: String) -> Result<(), SatiError> {
        let wd = self.current_word()?;
        match wd.meaning {
            Some(_) => Err(SatiError::MeaningAlreadyDefined),
            None => {
                wd.meaning = Some(meaning);
                Ok(())
            }
        }
    }

    fn add_english_name(&mut self, english_name: String) -> Result<(), SatiError> {
        let wd = self.current_word()?;
        match wd.english_name {
            Some(_) => Err(SatiError::EnglishNameAlreadyDefined),
            None => {
                wd.english_name = Some(english_name);
                Ok(())
            }
        }
    }

    fn add_synonym(&mut self, synonym: String) -> Result<(), SatiError> {
        self.current_word()?.synonyms.push(synonym);
        Ok(())
    }

    fn annotate(&mut self, title: Option<String>, text: String) -> Result<(), SatiError> {
        let text = SplitPreserveWhitespace::new(&text)
            .map_words(|x| {
                match self.dictionary.get(
                    x.to_uppercase()
                        .trim_matches(|c: char| !c.is_alphanumeric()),
                ) {
                    Some(w) => format!("{}{}", x, w.to_footnote()),
                    None => x.to_string(),
                }
            })
            .collect::<String>();
        match self.output.as_mut() {
            None => {
                let filename = title.as_ref().map_or_else(
                    || {
                        self.untitled_number += 1;
                        format!("Untitled_{}.tex", self.untitled_number)
                    },
                    |t| format!("{}.tex", t),
                );
                let mut file = File::create(filename)?;
                if self.wrapped {
                    writeln!(file, "{}", HEADER)?;
                }
                writeln!(
                    file,
                    "\\chapter{{{}}}\n{}",
                    title.unwrap_or_else(|| String::from("Untitled")),
                    text
                )?;
                if self.wrapped {
                    writeln!(file, "{}", FOOTER)?;
                }
                Ok(())
            }
            Some(o) => {
                writeln!(
                    o,
                    "\\chapter{{{}}}\n{}",
                    title.unwrap_or_else(|| String::from("Untitled")),
                    text
                )?;
                Ok(())
            }
        }
    }

    fn dump(&self) {
        for (k, v) in self.dictionary.iter() {
            eprintln!(
                "{} : {}",
                k,
                v.meaning.as_ref().unwrap_or(&"\"\"".to_string())
            );
            eprintln!(
                "\t| {}",
                v.english_name.as_ref().unwrap_or(&"\"\"".to_string())
            );
            eprint!("\t| [");
            for syn in v.synonyms.iter() {
                eprint!(" {}, ", syn);
            }
            eprintln!("]");
        }
    }
}

impl Word {
    fn new(word: String) -> Self {
        Word {
            wd: word,
            meaning: None,
            english_name: None,
            synonyms: vec![],
        }
    }

    fn to_footnote(&self) -> String {
        format!(
            "\\footnote{{\\textbf{{{}}}: Significado: {}, Nome inglês: {}\\\\\\indent \\indent Synonyms: {}}}",
            self.wd,
            self.meaning.as_ref().unwrap(),
            self.english_name.as_ref().unwrap(),
            self.synonyms.join(", ")
            )
    }
}

pub struct SplitPreserveWhitespace<'a> {
    string: Option<Token<'a>>,
}

pub enum Token<'a> {
    Whitespace(&'a str),
    Other(&'a str),
}

impl<'a> SplitPreserveWhitespace<'a> {
    fn new(string: &'a str) -> Self {
        if string.is_empty() {
            Self { string: None }
        } else if string.starts_with(char::is_whitespace) {
            Self {
                string: Some(Token::Whitespace(string)),
            }
        } else {
            Self {
                string: Some(Token::Other(string)),
            }
        }
    }

    fn map_words<S>(self, mut f: S) -> std::iter::Map<Self, impl FnMut(Token<'a>) -> String>
    where
        S: FnMut(&str) -> String,
    {
        self.map(move |t: Token<'a>| match t {
            Token::Other(s) => f(s),
            Token::Whitespace(s) => s.to_string(),
        })
    }
}

impl<'a> Iterator for SplitPreserveWhitespace<'a> {
    type Item = Token<'a>;

    fn next(&mut self) -> Option<Self::Item> {
        self.string.take().map(|t| match t {
            Token::Whitespace(s) => {
                let (token, rest) = match s.find(|c: char| !c.is_whitespace()) {
                    Some(i) => {
                        let (a, b) = s.split_at(i);
                        (a, Some(Token::Other(b)))
                    }
                    None => (s, None),
                };
                self.string = rest;
                Token::Whitespace(token)
            }
            Token::Other(s) => {
                let (token, rest) = match s.find(char::is_whitespace) {
                    Some(i) => {
                        let (a, b) = s.split_at(i);
                        (a, Some(Token::Whitespace(b)))
                    }
                    None => (s, None),
                };
                self.string = rest;
                Token::Other(token)
            }
        })
    }
}

#[no_mangle]
pub unsafe extern "C" fn sati_start(wrapped: bool) -> *mut Sati {
    match Sati::new(wrapped) {
        Ok(s) => Box::into_raw(Box::new(s)),
        Err(_) => std::ptr::null_mut(),
    }
}

#[no_mangle]
pub unsafe extern "C" fn sati_start_with_output(file: *const c_char, wrapped: bool) -> *mut Sati {
    let file = match File::create(c_char_to_string(file).expect("Null Filename")) {
        Ok(f) => f,
        Err(_) => return std::ptr::null_mut(),
    };
    match Sati::new_with_output(file, wrapped) {
        Ok(s) => Box::into_raw(Box::new(s)),
        Err(_) => std::ptr::null_mut(),
    }
}

#[no_mangle]
pub unsafe extern "C" fn sati_start_split(wrapped: bool) -> *mut Sati {
    Box::into_raw(Box::new(Sati::new_split(wrapped)))
}

#[no_mangle]
pub unsafe extern "C" fn sati_end(sati: *mut Sati) -> c_int {
    let sati = Box::from_raw(sati);
    match (sati.output, sati.wrapped) {
        (Some(mut o), true) => writeln!(o, "{}", FOOTER).map_err(SatiError::from),
        _ => Ok(()),
    }
    .map(|_| 0)
    .unwrap_or_else(|_| SatiError::IOError as c_int)
}

#[no_mangle]
pub unsafe extern "C" fn sati_add_word(sati: *mut Sati, word: *const c_char) -> c_int {
    let word = c_char_to_string(word);
    (&mut *sati)
        .add_word(word.expect("Null word"))
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub unsafe extern "C" fn sati_add_meaning(sati: *mut Sati, word: *const c_char) -> c_int {
    let word = c_char_to_string(word);
    (&mut *sati)
        .add_meaning(word.expect("Null Meaning"))
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub unsafe extern "C" fn sati_add_english_name(sati: *mut Sati, word: *const c_char) -> c_int {
    let word = c_char_to_string(word);
    (&mut *sati)
        .add_english_name(word.expect("Null English Name"))
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub unsafe extern "C" fn sati_add_synonym(sati: *mut Sati, word: *const c_char) -> c_int {
    let word = c_char_to_string(word);
    (&mut *sati)
        .add_synonym(word.expect("Null synonym"))
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub unsafe extern "C" fn sati_annotate(
    sati: *mut Sati,
    title: *const c_char,
    text: *const c_char,
) -> c_int {
    let title = match c_char_to_string(title) {
        Some(ref x) if x.is_empty() => None,
        s => s,
    };
    let text = c_char_to_string(text);
    (&mut *sati)
        .annotate(title, text.expect("Expected a text to annotate"))
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub unsafe extern "C" fn sati_dump(sati: *mut Sati) {
    (&*sati).dump()
}

fn c_char_to_string(w: *const c_char) -> Option<String> {
    if !w.is_null() {
        Some(unsafe { CStr::from_ptr(w).to_string_lossy().into_owned() })
    } else {
        None
    }
}
