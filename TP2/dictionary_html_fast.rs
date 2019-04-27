use std::collections::{HashMap, HashSet, BTreeMap};
use std::io::{self, BufReader, BufRead, Write};
use std::fs::File;
use std::env;

#[derive(Debug)]
struct Lemas {
    pos: HashMap<String, HashSet<String>>,
}

impl Lemas {
    fn new(lema: &str, pos: &str) -> Self {
        let mut lemas = HashMap::new();
        let mut set = HashSet::new();
        set.insert(pos.into());
        lemas.insert(lema.into(),set);
        Self {
            pos: lemas
        }
    }

    fn add_pos(&mut self, lema: &str, pos: &str) {
        self.pos.entry(lema.into())
            .and_modify(|p| { p.insert(pos.into()); })
            .or_insert_with(|| {
                let mut set = HashSet::new();
                set.insert(pos.into());
                return set;
            });
    }
}

#[derive(Debug)]
struct Dictionary {
    words: BTreeMap<String, Lemas>,
}

fn main() -> io::Result<()> {
    let file = env::args().skip(1).next().expect("Missing filename");
    let mut dict = Dictionary { words: Default::default() };
    BufReader::new(File::open(file)?)
        .lines()
        .filter_map(|l| l.ok())
        .for_each(|line|
                  {
                      let fields = line.split("\t").collect::<Vec<_>>();
                      dict.words.entry(fields[0].to_string())
                          .and_modify(|l| l.add_pos(fields[1], fields[2]))
                          .or_insert_with(|| Lemas::new(fields[1], fields[2]));
                  });
    let mut idx = File::create("dictionary/index.html")?;
    writeln!(idx, "<html>")?;
    writeln!(idx, "<head>")?;
    writeln!(idx, "  <meta charset='Utf-8'/>")?;
    writeln!(idx, "</head>")?;
    writeln!(idx, "<body>")?;
    writeln!(idx, "  <h1>Dicionario</h1>")?;
    writeln!(idx, "  <h2>{} palavras</h2>", dict.words.len())?;
    writeln!(idx, "  <table>")?;
    writeln!(idx, "    <tr>")?;
    for c in "ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÑÒÓÔÕÖØÙÚÛÜÝ".chars() {
        writeln!(idx, "    <td><a href='#{0}'>{0}</a></td>", c)?;
    }
    writeln!(idx, "    </tr>")?;
    writeln!(idx, "  </table>")?;
    writeln!(idx, "  <table>")?;
    writeln!(idx, "   <tr>")?;
    let mut i = 0;
    let mut last_leter = '[';
    for (mut word, lemas) in dict.words.into_iter() {
        word = word.replace("/", " ");
        word = word.replace("=", " ");
        if last_leter != word.chars().next().unwrap_or(last_leter) {
            last_leter = word.chars().next().unwrap();
            writeln!(idx, "    </tr>")?;
            writeln!(idx, "  </table>")?;
            writeln!(idx, "  <h3><a name='{0}'>{0}</a></h3>", last_leter)?;
            writeln!(idx, "  <table>")?;
            writeln!(idx, "    <tr>")?;
            i = 0
        }
        if i % 4 == 0 {
            writeln!(idx, "   </tr>")?;
            writeln!(idx, "   <tr>")?;
        }
        i += 1;
        writeln!(idx, "      <td style='border-right: 1px solid #000?;'>")?;
        writeln!(idx, "        <a href='{0}.html'>{0}</a>", word)?;
        writeln!(idx, "      </td>")?;

        let mut word_file = File::create(format!("dictionary/{}.html", word))?;
        writeln!(word_file, "<html>")?;
        writeln!(word_file, "<head>")?;
        writeln!(word_file, "  <meta charset='Utf-8'/>")?;
        writeln!(word_file, "</head>")?;
        writeln!(word_file, "<body>")?;
        writeln!(word_file, "  <h1>{}</h1>", word)?;
        writeln!(word_file, "  <table>")?;
        for (lema, pos) in lemas.pos.into_iter() {
            write!(word_file, "<tr><td>{}</td>", lema)?;
            for p in pos {
                write!(word_file, "<td>{}</td>", p)?;
            }
            writeln!(word_file, "</tr>")?;
        }
        writeln!(word_file, "</table>")?;
        writeln!(word_file, "</body>")?;
        writeln!(word_file, "</html>")?;
    }
    Ok(())
}
