use std::collections::HashMap;
use std::ffi::CStr;
use std::fs::File;
use std::io::{stdout, Write};
use std::os::raw::{c_char, c_int};

use itertools::Itertools;

const HEADER: &str = include_str!("../assets/header.tex");
const FOOTER: &str = include_str!("../assets/footer.tex");

pub struct Sati {
    database: HashMap<String, Word>,
    current_word: Option<String>,
    untitled_number: usize,
    output: Option<Box<dyn Write>>,
}

impl Default for Sati {
    fn default() -> Sati {
        Sati::new()
    }
}

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
    fn new() -> Sati {
        Sati {
            database: HashMap::new(),
            current_word: None,
            untitled_number: 0,
            output: Some(Box::new(stdout())),
        }
    }

    fn new_with_output(file: File) -> Sati {
        Sati {
            database: HashMap::new(),
            current_word: None,
            untitled_number: 0,
            output: Some(Box::new(file)),
        }
    }

    fn new_split() -> Sati {
        Sati {
            database: HashMap::new(),
            current_word: None,
            untitled_number: 0,
            output: None,
        }
    }

    fn current_word(&mut self) -> Result<&mut Word, SatiError> {
        let cw = self.current_word.as_ref().ok_or(SatiError::NoCurrentWord)?;
        Ok(self.database.get_mut(cw).unwrap())
    }

    fn add_word(&mut self, word: String) -> Result<(), SatiError> {
        if self.database.contains_key(word.as_str()) {
            Err(SatiError::WordAlreadyDefined)
        } else {
            let w = Word::new(word);
            let key = w.wd.to_uppercase();
            self.current_word = Some(key.clone());
            self.database.insert(key, w);
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

    fn parse_text(&mut self, title: Option<String>, text: String) -> Result<(), SatiError> {
        let text = text
            .split(' ')
            .map(|x| {
                match self.database.get(
                    x.to_uppercase()
                        .trim_start_matches(|c: char| !c.is_alphanumeric())
                        .trim_end_matches(|c: char| !c.is_alphanumeric()),
                ) {
                    Some(w) => format!("{}{}", x, w.to_footnote()),
                    None => x.to_string(),
                }
            })
            .join(" ");
        match self.output.as_mut() {
            None => {
                let filename = title.as_ref().map_or_else(
                    || {
                        self.untitled_number += 1;
                        format!("Untitled_{}.tex", self.untitled_number)
                    },
                    |t| format!("{}.tex", t),
                );
                let mut file = File::create(filename).unwrap();
                writeln!(file, "{}", HEADER)?;
                writeln!(
                    file,
                    "\\chapter{{{}}}\n{}",
                    title.unwrap_or_else(|| String::from("Untitled")),
                    text
                )?;
                writeln!(file, "{}", FOOTER)?;
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
        for v in self.database.values() {
            eprintln!(
                "{} : {}",
                v.wd,
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

#[no_mangle]
pub unsafe extern "C" fn sati_start_with_output(file: *const c_char) -> *mut Sati {
    let mut file = match File::create(c_char_to_string(file)) {
        Ok(f) => f,
        Err(_) => return std::ptr::null_mut(),
    };
    match writeln!(file, "{}", HEADER).map_err(SatiError::from) {
        Ok(_) => Box::into_raw(Box::new(Sati::new_with_output(file))),
        Err(_) => std::ptr::null_mut(),
    }
}

#[no_mangle]
pub unsafe extern "C" fn sati_start_split() -> *mut Sati {
    Box::into_raw(Box::new(Sati::new_split()))
}

#[no_mangle]
pub unsafe extern "C" fn sati_start() -> *mut Sati {
    match writeln!(std::io::stdout(), "{}", HEADER).map_err(SatiError::from) {
        Ok(_) => Box::into_raw(Box::new(Sati::default())),
        Err(_) => std::ptr::null_mut(),
    }
}

#[no_mangle]
pub unsafe extern "C" fn sati_end(sati: *mut Sati) -> c_int {
    let sati = Box::from_raw(sati);
    match sati.output {
        Some(mut o) => writeln!(o, "{}", FOOTER).map_err(SatiError::from),
        None => Ok(()),
    }
    .map(|_| 0)
    .unwrap_or_else(|_| SatiError::IOError as c_int)
}

#[no_mangle]
pub unsafe extern "C" fn sati_add_word(sati: *mut Sati, word: *const c_char) -> c_int {
    let word = c_char_to_string(word);
    (&mut *sati)
        .add_word(word)
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub unsafe extern "C" fn sati_add_meaning(sati: *mut Sati, word: *const c_char) -> c_int {
    let word = c_char_to_string(word);
    (&mut *sati)
        .add_meaning(word)
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub unsafe extern "C" fn sati_add_english_name(sati: *mut Sati, word: *const c_char) -> c_int {
    let word = c_char_to_string(word);
    (&mut *sati)
        .add_english_name(word)
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub unsafe extern "C" fn sati_add_synonym(sati: *mut Sati, word: *const c_char) -> c_int {
    let word = c_char_to_string(word);
    (&mut *sati)
        .add_synonym(word)
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub unsafe extern "C" fn sati_parse_text(
    sati: *mut Sati,
    title: *const c_char,
    text: *const c_char,
) -> c_int {
    let title = match c_char_to_string(title) {
        ref x if x.is_empty() => None,
        s => Some(s),
    };
    let text = c_char_to_string(text);
    (&mut *sati)
        .parse_text(title, text)
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub unsafe extern "C" fn sati_dump(sati: *mut Sati) {
    (&*sati).dump()
}

fn c_char_to_string(w: *const c_char) -> String {
    unsafe { CStr::from_ptr(w).to_string_lossy().into_owned() }
}
