#include "CommandLineInterface.h"

void CommandLineInterface::initArguments() {
  Program.add_argument("original").help("locates original source code file");
  Program.add_argument("modified").help("locates modified source code file");

  std::vector<std::string> Languages;
#ifdef DIFFINK_LANGUAGE_SUPPORT_C
  Languages.emplace_back("c");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CSHARP
  Languages.emplace_back("csharp");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CPP
  Languages.emplace_back("cpp");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CSS
  Languages.emplace_back("css");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_GO
  Languages.emplace_back("go");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_HASKELL
  Languages.emplace_back("haskell");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_HTML
  Languages.emplace_back("html");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JAVA
  Languages.emplace_back("java");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JAVASCRIPT
  Languages.emplace_back("javascript");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JSON
  Languages.emplace_back("json");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JULIA
  Languages.emplace_back("julia");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_PYTHON
  Languages.emplace_back("python");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_RUBY
  Languages.emplace_back("ruby");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_RUST
  Languages.emplace_back("rust");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_SCALA
  Languages.emplace_back("scala");
#endif

  std::string LangHelp{"specifies language of input source codes\n"
                       "supported languages: "};
  if (Languages.empty())
    LangHelp.append("none\n");
  else {
    for (const auto &lang : Languages)
      LangHelp.append(lang).append(", ");
    LangHelp.pop_back();
    LangHelp.pop_back();
  }
  {
    auto &Temp =
        Program.add_argument("-l", "--language").required().help(LangHelp);
    for (const auto &lang : Languages)
      Temp.add_choice(lang);
  }

  Program.add_argument("-m", "--matcher")
      .help("selects matching algorithm")
      .choices("diffink-simple", "gumtree-simple", "lgmatcher-simple",
               "bytel-simple", "diffink-opt", "gumtree-opt")
      .default_value(std::string{"diffink-simple"});

  Program.add_argument("-f", "--format")
      .help("selects diff report formats: text, html, json, full-json")
      .nargs(argparse::nargs_pattern::any)
      .default_value(std::vector<std::string>{"text"});

  Program.add_argument("-o", "--output")
      .help("specifies directory to save diffink outputs")
      .default_value(std::string{"diffink_reports/"});

  Program.add_argument("--log")
      .help("save log file as \"${OUTPUT}/log\"")
      .implicit_value(true)
      .default_value(false);
}

std::string
CommandLineInterface::read(const std::filesystem::path &Path) const {
  std::ifstream File(Path, std::ios::binary);
  if (!File)
    throw std::runtime_error("Failed to open file: " + Path.string());
  const auto FileSize = std::filesystem::file_size(Path);
  std::string Content(FileSize, '\0');
  if (!File.read(Content.data(), FileSize))
    throw std::runtime_error("Failed to read file: " + Path.string());
  return Content;
}

#if defined(FRONTEND_PLATFORM_LINUX)
void CommandLineInterface::setDiffinkDirectory() {
  char Buffer[PATH_MAX];
  if (auto Size = readlink("/proc/self/exe", Buffer, PATH_MAX);
      Size != -1 && Size < PATH_MAX)
    DiffinkDirectory = std::filesystem::path(
                           std::string(Buffer, static_cast<std::size_t>(Size)))
                           .parent_path()
                           .parent_path()
                           .parent_path()
                           .parent_path();
}

#elif defined(FRONTEND_PLATFORM_MACOS)
void CommandLineInterface::setDiffinkDirectory() {
  if (uint32_t Size{0}; _NSGetExecutablePath(nullptr, &Size) == -1) {
    std::string buffer(Size, '\0');
    if (_NSGetExecutablePath(buffer.data(), &Size) == 0)
      DiffinkDirectory = std::filesystem::path(buffer.data())
                             .parent_path()
                             .parent_path()
                             .parent_path()
                             .parent_path();
  }
}
#endif

void CommandLineInterface::setParser(const std::string &Arg) {
#ifdef DIFFINK_LANGUAGE_SUPPORT_C
  if (Arg == "c") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_c);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CSHARP
  if (Arg == "csharp") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_c_sharp);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CPP
  if (Arg == "cpp") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_cpp);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CSS
  if (Arg == "css") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_css);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_GO
  if (Arg == "go") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_go);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_HASKELL
  if (Arg == "haskell") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_haskell);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_HTML
  if (Arg == "html") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_html);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JAVA
  if (Arg == "java") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_java);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JAVASCRIPT
  if (Arg == "javascript") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_javascript);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JSON
  if (Arg == "json") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_json);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JULIA
  if (Arg == "julia") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_julia);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_PYTHON
  if (Arg == "python") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_python);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_RUBY
  if (Arg == "ruby") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_ruby);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_RUST
  if (Arg == "rust") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_rust);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_SCALA
  if (Arg == "scala") {
    Parser = std::make_unique<diffink::TSParserWrapper>(tree_sitter_scala);
    return;
  }
#endif
  throw std::invalid_argument("Invalid language: " + Arg);
}

void CommandLineInterface::setMatcher(const std::string &Arg) {
  if (Arg == "gumtree-simple") {
    ApplyDiffink = false;
    Matcher = diffink::gumtree::Framework::makeSimple();
  } else if (Arg == "diffink-simple") {
    ApplyDiffink = true;
    Matcher = diffink::makeDiffinkSimple();
  } else if (Arg == "lgmatcher-simple") {
    ApplyDiffink = false;
    Matcher = diffink::makeLGMatcherSimple(*OldCode, *NewCode);
  } else if (Arg == "bytel-simple") {
    ApplyDiffink = true;
    Matcher = diffink::makeByteLSimple();
  } else if (Arg == "diffink-opt") {
    ApplyDiffink = true;
    Matcher = diffink::makeDiffinkOptimal();
  } else if (Arg == "gumtree-opt") {
    ApplyDiffink = false;
    Matcher = diffink::gumtree::Framework::makeOptimal();
  } else
    throw std::invalid_argument("Invalid matcher: " + Arg);
}

void CommandLineInterface::setFormats(const std::vector<std::string> &Args) {
  for (const auto &Arg : Args) {
    if (Arg == "text")
      Formats.emplace_back(
          [this](const ScriptExporter &Exporter) { exportAsText(Exporter); });
    else if (Arg == "html")
      Formats.emplace_back(
          [this](const ScriptExporter &Exporter) { exportAsHTML(Exporter); });
    else if (Arg == "json")
      Formats.emplace_back(
          [this](const ScriptExporter &Exporter) { exportAsJSON(Exporter); });
    else if (Arg == "full-json")
      Formats.emplace_back([this](const ScriptExporter &Exporter) {
        exportAsFullJSON(Exporter);
      });
    else
      throw std::invalid_argument("Invalid format: " + Arg);
  }
}

void CommandLineInterface::setOutputDirectory(const std::string &Arg) {
  OutputDirectory = Arg;
  if (!std::filesystem::exists(OutputDirectory))
    std::filesystem::create_directories(OutputDirectory);
}

void CommandLineInterface::exportAsText(const ScriptExporter &Exporter) const {
  std::ofstream File(OutputDirectory / "diff.txt");
  if (!File)
    throw std::runtime_error("Failed to open file: " +
                             (OutputDirectory / "diff.txt").string());
  File << Exporter.exportAsString();
}

void CommandLineInterface::setLogger() {
  auto LogFile = std::make_unique<std::ofstream>(OutputDirectory / "log");
  if (!LogFile->is_open())
    throw std::runtime_error("Failed to open file: " +
                             (OutputDirectory / "log").string());
  Log.setStream(std::move(LogFile));
}

void CommandLineInterface::exportAsHTML(const ScriptExporter &Exporter) const {
  const auto &[Old, New] = Exporter.exportAsHTML(*OldCode, *NewCode);
  std::ofstream FileOld(OutputDirectory / "diff_original.html");
  if (!FileOld)
    throw std::runtime_error("Failed to open file: " +
                             (OutputDirectory / "diff_original.html").string());
  std::ofstream FileNew(OutputDirectory / "diff_modified.html");
  if (!FileNew)
    throw std::runtime_error("Failed to open file: " +
                             (OutputDirectory / "diff_modified.html").string());
  FileOld << Old;
  FileNew << New;
}

void CommandLineInterface::exportAsJSON(const ScriptExporter &Exporter) const {
  std::ofstream File(OutputDirectory / "diff.json");
  if (!File)
    throw std::runtime_error("Failed to open file: " +
                             (OutputDirectory / "diff.json").string());
  File << Exporter.exportAsJSON().dump(-1);
}

void CommandLineInterface::exportAsFullJSON(
    const ScriptExporter &Exporter) const {
  std::ofstream File(OutputDirectory / "diff_full.json");
  if (!File)
    throw std::runtime_error("Failed to open file: " +
                             (OutputDirectory / "diff_full.json").string());
  File << Exporter.exportAsFullJSON().dump(-1);
}

ScriptExporter::DiffResult CommandLineInterface::runDiff() {
  Log.start();
  OldTree.parse(Parser->get(), *OldCode);
  Log.logElapsedTime("Parsing original source code");

  if (ApplyDiffink) {
    Log.start();
    auto EditedTree = diffink::TextDiff(*OldCode, *NewCode)
                          .makeEditedTree(OldTree.getRawTree());
    Log.logElapsedTime("Text diffing");
    if (!EditedTree)
      Log.log("! DiffInk process will be skipped");

    Log.start();
    NewTree.parse(Parser->get(), *NewCode, EditedTree.get());
    Log.logElapsedTime("Parsing modified source code");

    Log.start();
    NewTree.identifyCommons(OldTree, EditedTree);
    Log.logElapsedTime("Matching common nodes");
  }

  else {
    Log.start();
    NewTree.parse(Parser->get(), *NewCode);
    Log.logElapsedTime("Parsing modified source code");
  }

  Log.start();
  auto Diff = diffink::TreeDiff::run(Matcher.get(), OldTree, NewTree);
  Log.logElapsedTime("Running matcher");
  return Diff;
}

void CommandLineInterface::logMeta() {
  Log.log(std::format("* Matcher: {}", Program.get<std::string>("--matcher")));
  Log.log(
      std::format("* Language: {}", Program.get<std::string>("--language")));
  Log.log(std::format("* Original source code: \"{}\"",
                      Program.get<std::string>("original")));
  Log.log(std::format("* Modified source code: \"{}\"\n",
                      Program.get<std::string>("modified")));
}

void CommandLineInterface::makeReport(ScriptExporter::DiffResult &&Diff) const {
  ScriptExporter Exporter(OldTree, NewTree, std::move(Diff));
  for (const auto &Format : Formats)
    Format(Exporter);
}

void CommandLineInterface::run() {
  initArguments();
  setDiffinkDirectory();
  Program.parse_args(argc, argv);
  OldCode = std::make_unique<diffink::SourceCode>(
      read(Program.get<std::string>("original")));
  NewCode = std::make_unique<diffink::SourceCode>(
      read(Program.get<std::string>("modified")));
  setParser(Program.get<std::string>("--language"));
  setMatcher(Program.get<std::string>("--matcher"));
  setFormats(Program.get<std::vector<std::string>>("--format"));
  setOutputDirectory(Program.get<std::string>("--output"));
  if (Program.get<bool>("--log"))
    setLogger();
  logMeta();
  makeReport(runDiff());
  std::exit(0);
}