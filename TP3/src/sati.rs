use std::cell::RefCell;
use std::collections::HashMap;
use std::ffi::CStr;
use std::fs::File;
use std::io::{stdout, Write};
use std::os::raw::{c_char, c_int};
use std::thread_local;

use itertools::Itertools;

pub struct Sati {
    database: HashMap<String, Word>,
    current_word: Option<String>,
    untitled_number: usize,
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

pub struct Config {
    output: Box<dyn Write>,
    split: bool,
}

impl Default for Config {
    fn default() -> Self {
        Config {
            output: Box::new(stdout()),
            split: false,
        }
    }
}

thread_local!(
    static INSTANCE: RefCell<Sati> = RefCell::new(Sati {
        database: HashMap::new(),
        current_word: None,
        untitled_number: 0,
    });
    static CONFIG: RefCell<Config> = RefCell::new(Config::default());
);

#[no_mangle]
pub extern "C" fn sati_set_output(file: *const c_char) -> c_int {
    let file = match File::create(c_char_to_string(file)) {
        Ok(f) => f,
        Err(_) => return SatiError::IOError as c_int,
    };
    CONFIG.with(|c| c.borrow_mut().output = Box::new(file));
    0
}

#[no_mangle]
pub extern "C" fn sati_set_split() {
    CONFIG.with(|c| c.borrow_mut().split = true);
}

#[no_mangle]
pub extern "C" fn sati_start() -> c_int {
    let header = include_str!("../assets/template.tex");
    CONFIG
        .with(|c| {
            if !c.borrow().split {
                writeln!(c.borrow_mut().output, "{}", header).map_err(SatiError::from)
            } else {
                Ok(())
            }
        })
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub extern "C" fn sati_end() -> c_int {
    CONFIG
        .with(|c| {
            if !c.borrow().split {
                writeln!(c.borrow_mut().output, r#"\end{{document}}"#).map_err(SatiError::from)
            } else {
                Ok(())
            }
        })
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub extern "C" fn sati_add_word(word: *const c_char) -> c_int {
    let word = c_char_to_string(word);
    INSTANCE
        .with(|s| s.borrow_mut().add_word(word))
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub extern "C" fn sati_add_meaning(word: *const c_char) -> c_int {
    let word = c_char_to_string(word);
    INSTANCE
        .with(|s| s.borrow_mut().add_meaning(word))
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub extern "C" fn sati_add_english_name(word: *const c_char) -> c_int {
    let word = c_char_to_string(word);
    INSTANCE
        .with(|s| s.borrow_mut().add_english_name(word))
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub extern "C" fn sati_add_synonym(word: *const c_char) -> c_int {
    let word = c_char_to_string(word);
    INSTANCE
        .with(|s| s.borrow_mut().add_synonym(word))
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub extern "C" fn sati_parse_text(title: *const c_char, text: *const c_char) -> c_int {
    let title = match c_char_to_string(title) {
        ref x if x.is_empty() => None,
        s => Some(s),
    };
    let text = c_char_to_string(text);
    INSTANCE
        .with(|s| s.borrow_mut().parse_text(title, text))
        .map(|_| 0)
        .unwrap_or_else(|x| x as i32)
}

#[no_mangle]
pub extern "C" fn sati_dump() {
    INSTANCE.with(|s| s.borrow().dump());
}

fn c_char_to_string(w: *const c_char) -> String {
    unsafe { CStr::from_ptr(w).to_string_lossy().into_owned() }
}

impl Sati {
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
        CONFIG.with(|c| {
            if c.borrow().split {
                let header = include_str!("../assets/template.tex");
                let filename = title.as_ref().map_or_else(
                    || {
                        self.untitled_number += 1;
                        format!("Untitled_{}.tex", self.untitled_number)
                    },
                    |t| format!("{}.tex", t),
                );
                let mut file = File::create(filename).unwrap();
                writeln!(file, "{}", header)?;
                writeln!(
                    file,
                    "\\chapter{{{}}}\n{}",
                    title.unwrap_or_else(|| String::from("Untitled")),
                    text
                )?;
                writeln!(file, r#"\end{{document}}"#)?;
            } else {
                writeln!(
                    c.borrow_mut().output,
                    "\\chapter{{{}}}\n{}",
                    title.unwrap_or_else(|| String::from("Untitled")),
                    text
                )?;
            }
            Ok(())
        })
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
