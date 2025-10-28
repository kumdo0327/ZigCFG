#ifndef FRONTEND_COMMANDLINEINTERFACE_H
#define FRONTEND_COMMANDLINEINTERFACE_H

#include "DiffInk/Api.h"
#include "DiffInk/TreeDiff/LGMatcherFramework.h"
#include "IntervalLog.h"
#include "ScriptExporter.h"
#include <argparse.hpp>
#include <filesystem>
#include <fstream>

#if defined(_WIN32) || defined(_WIN64)
#define FRONTEND_PLATFORM_UNSUPPORTED
// #define FRONTEND_PLATFORM_WINDOWS
// #include <windows.h>
#elif defined(__linux__)
#define FRONTEND_PLATFORM_LINUX
#include <unistd.h>
#elif defined(__APPLE__)
#define FRONTEND_PLATFORM_MACOS
#include <mach-o/dyld.h>
#else
#define FRONTEND_PLATFORM_UNSUPPORTED
#endif

#ifdef DIFFINK_LANGUAGE_SUPPORT_C
#include <tree-sitter-c.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CSHARP
#include <tree-sitter-csharp.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CPP
#include <tree-sitter-cpp.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CSS
#include <tree-sitter-css.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_GO
#include <tree-sitter-go.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_HASKELL
#include <tree-sitter-haskell.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_HTML
#include <tree-sitter-html.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JAVA
#include <tree-sitter-java.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JAVASCRIPT
#include <tree-sitter-javascript.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JSON
#include <tree-sitter-json.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JULIA
#include <tree-sitter-julia.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_PYTHON
#include <tree-sitter-python.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_RUBY
#include <tree-sitter-ruby.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_RUST
#include <tree-sitter-rust.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_SCALA
#include <tree-sitter-scala.h>
#endif

class CommandLineInterface {
private:
  constexpr static const char *ProjectName = "DiffInk";
  constexpr static const char *ProjectVersion = "v0.0.0";

  using ExporterType = std::function<void(const ScriptExporter &)>;

  int argc;
  char **argv;
  argparse::ArgumentParser Program;
  std::filesystem::path DiffinkDirectory;
  std::unique_ptr<diffink::TSParserWrapper> Parser;
  bool ApplyDiffink;
  std::unique_ptr<diffink::TreeDiff::Matcher> Matcher;
  std::vector<ExporterType> Formats;
  std::filesystem::path OutputDirectory;
  IntervalLog Log;

  std::unique_ptr<diffink::SourceCode> OldCode;
  std::unique_ptr<diffink::SourceCode> NewCode;
  diffink::MerkleTree OldTree;
  diffink::MerkleTree NewTree;

private:
  void initArguments();

  std::string read(const std::filesystem::path &Path) const;

  void setDiffinkDirectory();

  void setParser(const std::string &Arg);

  void setMatcher(const std::string &Arg);

  void setFormats(const std::vector<std::string> &Args);

  void setOutputDirectory(const std::string &Arg);

  void setLogger();

  void exportAsText(const ScriptExporter &Exporter) const;

  void exportAsHTML(const ScriptExporter &Exporter) const;

  void exportAsJSON(const ScriptExporter &Exporter) const;

  void exportAsFullJSON(const ScriptExporter &Exporter) const;

  ScriptExporter::DiffResult runDiff();

  void logMeta();

  void makeReport(ScriptExporter::DiffResult &&Diff) const;

public:
  CommandLineInterface(int argc, char *argv[]) noexcept
      : argc{argc}, argv{argv}, Program(ProjectName, ProjectVersion) {}

  void run();
};

#endif // FRONTEND_COMMANDLINEINTERFACE_H