use std::cell::RefCell;
use std::collections::HashMap;
use std::ffi::CStr;
use std::os::raw::c_char;
use std::os::raw::c_int;
use std::thread_local;

pub struct Sati {
    database: HashMap<String, Word>,
    current_word: Option<String>,
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
}

thread_local!(
    static INSTANCE: RefCell<Sati> = RefCell::new(Sati {
        database: HashMap::new(),
        current_word: None,
    });
);

#[no_mangle]
pub extern "C" fn sati_start() {
    println!(r#"\documentclass[a4paper]{{report}}"#);
    println!(r#"\begin{{document}}"#);
}

#[no_mangle]
pub extern "C" fn sati_end() {
    println!(r#"\end{{document}}"#);
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
pub extern "C" fn sati_parse_text(title: *const c_char, text: *const c_char) {
    let title = match c_char_to_string(title) {
        ref x if x.is_empty() => String::from("Untitled"),
        s => s,
    };
    let text = c_char_to_string(text);
    INSTANCE.with(|s| s.borrow().parse_text(title, text));
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

    fn parse_text(&self, title: String, text: String) {
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
            .collect::<Vec<_>>()
            .join(" ");
        println!("\\chapter{{{}}}\n{}", title, text);
    }

    fn dump(&self) {
        for v in self.database.values() {
            println!(
                "{} : {}",
                v.wd,
                v.meaning.as_ref().unwrap_or(&"\"\"".to_string())
            );
            println!(
                "\t| {}",
                v.english_name.as_ref().unwrap_or(&"\"\"".to_string())
            );
            print!("\t| [");
            for syn in v.synonyms.iter() {
                print!(" {}, ", syn);
            }
            println!("]");
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
            "\\footnote{{\\textbf{{{}}}: Meaining: {}, English Name: {}, Synonyms: {:?}}}",
            self.wd,
            self.meaning.as_ref().unwrap(),
            self.english_name.as_ref().unwrap(),
            self.synonyms
        )
    }
}
