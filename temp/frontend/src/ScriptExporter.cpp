#include "ScriptExporter.h"

nlohmann::ordered_json ScriptExporter::fromTreeToJSON(
    const diffink::HashNode &Node,
    std::unordered_map<const diffink::HashNode *, std::size_t> &IdMap,
    std::size_t &Id) const {
  using json = nlohmann::ordered_json;
  IdMap.emplace(&Node, Id);
  auto JsonNode = json{{"id", Id++}, {"type", Node.getType()}};
  if (Node.isLeaf())
    JsonNode["label"] = Node.getLabel();
  JsonNode["pos"] =
      std::format("({},{})-({},{})", Node.getUTF8PosRange().StartPos.row + 1,
                  Node.getUTF8PosRange().StartPos.column + 1,
                  Node.getUTF8PosRange().EndPos.row + 1,
                  Node.getUTF8PosRange().EndPos.column + 1);
  JsonNode["byte"] =
      json::array({Node.getByteRange().first, Node.getByteRange().second});

  if (!Node.getChildren().empty()) {
    auto Children = json::array();
    for (const auto &Child : Node.getChildren())
      Children.push_back(fromTreeToJSON(Child, IdMap, Id));
    JsonNode["children"] = Children;
  }
  return JsonNode;
}

nlohmann::ordered_json ScriptExporter::fromMappingToJSON(
    const std::unordered_map<const diffink::HashNode *, std::size_t> &IdMap)
    const {
  using json = nlohmann::ordered_json;
  auto Buffer = json::array();
  for (const auto [OldNode, NewNode] : Map)
    Buffer.push_back(
        json{{"original", IdMap.at(OldNode)}, {"modified", IdMap.at(NewNode)}});
  return Buffer;
}

nlohmann::ordered_json ScriptExporter::fromMappingToScript(
    const std::unordered_map<const diffink::HashNode *, std::size_t> &IdMap)
    const {
  using json = nlohmann::ordered_json;
  auto Buffer = json::array();
  for (const auto &Action : Script) {
    std::visit(
        [&Buffer, &IdMap](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;
          if constexpr (std::is_same_v<T, diffink::edit_action::InsertNode>)
            Buffer.push_back(json{{"action", "insert-node"},
                                  {"modified", IdMap.at(&Action.Leaf)}});

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteNode>)
            Buffer.push_back(json{{"action", "delete-node"},
                                  {"original", IdMap.at(&Action.Leaf)}});

          else if constexpr (std::is_same_v<T, diffink::edit_action::MoveTree>)
            Buffer.push_back(
                json{{"action", "move-tree"},
                     {"original", IdMap.at(&Action.Subtree)},
                     {"modified", IdMap.at(&Action.MovedSubtree)}});

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::UpdateNode>)
            Buffer.push_back(json{{"action", "update-node"},
                                  {"original", IdMap.at(&Action.Leaf)},
                                  {"modified", IdMap.at(&Action.UpdatedLeaf)}});

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::InsertTree>)
            Buffer.push_back(json{{"action", "insert-tree"},
                                  {"modified", IdMap.at(&Action.Subtree)}});

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteTree>)
            Buffer.push_back(json{{"action", "delete-tree"},
                                  {"original", IdMap.at(&Action.Subtree)}});
        },
        Action);
  }
  return Buffer;
}

std::string ScriptExporter::exportAsString() const {
  std::string Buffer;
  for (const auto &Action : Script) {
    std::visit(
        [&Buffer](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;
          constexpr std::string_view BigSeperator = "===\n";
          constexpr std::string_view SmallSeperator = "---\n";

          if constexpr (std::is_same_v<T, diffink::edit_action::InsertNode>)
            Buffer.append(BigSeperator)
                .append("insert-node\n")
                .append(SmallSeperator)
                .append(Action.Leaf.toString())
                .append("\nto\n")
                .append(Action.Parent.toString())
                .append("\nat ")
                .append(std::to_string(Action.Index + 1));

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteNode>)
            Buffer.append(BigSeperator)
                .append("delete-node\n")
                .append(SmallSeperator)
                .append(Action.Leaf.toString());

          else if constexpr (std::is_same_v<T, diffink::edit_action::MoveTree>)
            Buffer.append(BigSeperator)
                .append("move-tree\n")
                .append(SmallSeperator)
                .append(Action.Subtree.toStringRecursively())
                .append("\nto\n")
                .append(Action.Parent.toString())
                .append("\nat ")
                .append(std::to_string(Action.Index + 1));

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::UpdateNode>)
            Buffer.append(BigSeperator)
                .append("update-node\n")
                .append(SmallSeperator)
                .append(Action.Leaf.toString())
                .append("\nreplace \"")
                .append(Action.Leaf.getLabel())
                .append("\" by \"")
                .append(Action.UpdatedLeaf.getLabel())
                .push_back('"');

          Buffer.push_back('\n');
        },
        Action);
  }

  if (!Buffer.empty())
    Buffer.pop_back();
  return Buffer;
}

nlohmann::ordered_json ScriptExporter::exportAsJSON() const {
  using json = nlohmann::ordered_json;
  auto Buffer = json::array();
  for (auto &Action : diffink::simplifyEditScript(Script)) {
    std::visit(
        [&Buffer](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;
          if constexpr (std::is_same_v<T, diffink::edit_action::InsertNode>)
            Buffer.push_back(
                json{{"action", "insert-node"},
                     {"modified", Action.Leaf.toString()},
                     {"mod-range",
                      std::format("({},{})", Action.Leaf.getByteRange().first,
                                  Action.Leaf.getByteRange().second)}});

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteNode>)
            Buffer.push_back(
                json{{"action", "delete-node"},
                     {"original", Action.Leaf.toString()},
                     {"orig-range",
                      std::format("({},{})", Action.Leaf.getByteRange().first,
                                  Action.Leaf.getByteRange().second)}});

          else if constexpr (std::is_same_v<T, diffink::edit_action::MoveTree>)
            Buffer.push_back(json{
                {"action", "move-tree"},
                {"original", Action.Subtree.toString()},
                {"orig-range",
                 std::format("({},{})", Action.Subtree.getByteRange().first,
                             Action.Subtree.getByteRange().second)},
                {"modified", Action.MovedSubtree.toString()},
                {"mod-range",
                 std::format("({},{})",
                             Action.MovedSubtree.getByteRange().first,
                             Action.MovedSubtree.getByteRange().second)}});

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::UpdateNode>)
            Buffer.push_back(json{
                {"action", "update-node"},
                {"original", Action.Leaf.toString()},
                {"orig-range",
                 std::format("({},{})", Action.Leaf.getByteRange().first,
                             Action.Leaf.getByteRange().second)},
                {"modified", Action.UpdatedLeaf.toString()},
                {"mod-range",
                 std::format("({},{})", Action.UpdatedLeaf.getByteRange().first,
                             Action.UpdatedLeaf.getByteRange().second)}});
        },
        Action);
  }
  return Buffer;
}

std::pair<std::string, std::string>
ScriptExporter::exportAsHTML(const diffink::SourceCode &OldSrc,
                             const diffink::SourceCode &NewSrc) const {
  constexpr std::string_view BeginTemplate{R"(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <style>
    body { background-color: rgb(255, 255, 255);}
  </style>
</head>
<body>
  <pre><code>)"};

  constexpr std::string_view EndTemplate{R"(</code></pre>
</body>
</html>
)"};

  constexpr std::string_view InsertAction{
      R"(<span style="background-color:rgb(191, 255, 191);">)"};
  constexpr std::string_view DeleteAction{
      R"(<span style="background-color:rgb(255, 191, 191);">)"};
  constexpr std::string_view UpdateAction{
      R"(<span style="background-color:rgb(239, 239, 0);">)"};
  constexpr std::string_view MoveAction{
      R"(<span style="background-color:rgb(255, 159, 255);">)"};
  constexpr std::string_view ActionEnd{"</span>"};

  constexpr std::string_view AmpChar{"&amp;"};
  constexpr std::string_view LessChar{"&lt;"};
  constexpr std::string_view GreaterChar{"&gt;"};
  constexpr std::string_view DoubleQuoteChar{"&quot;"};
  constexpr std::string_view SingleQuoteChar{"&apos;"};

  struct TagInfo {
    const std::string_view *Tag;
    uint32_t Start;
    uint32_t End;
  };
  std::vector<TagInfo> OldTag(OldSrc.getSize(), {nullptr});
  std::vector<TagInfo> NewTag(NewSrc.getSize(), {nullptr});
  auto assignTag = [&](TagInfo &Lhs, TagInfo Rhs) {
    if (Lhs.Tag) {
      if (Lhs.Tag != &UpdateAction && Lhs.Start <= Rhs.Start &&
          Rhs.End <= Lhs.End)
        Lhs = Rhs;
    } else
      Lhs = Rhs;
  };

  for (const auto &Action : Script) {
    std::visit(
        [&](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;
          if constexpr (std::is_same_v<T, diffink::edit_action::InsertNode>) {
            const auto [Start, End] = Action.Leaf.getByteRange();
            for (auto i = Start; i != End; ++i)
              assignTag(NewTag[i], {&InsertAction, Start, End});
          }

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteNode>) {
            const auto [Start, End] = Action.Leaf.getByteRange();
            for (auto i = Start; i != End; ++i)
              assignTag(OldTag[i], {&DeleteAction, Start, End});
          }

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::UpdateNode>) {
            const auto [Start, End] = Action.Leaf.getByteRange();
            const auto [NewStart, NewEnd] = Action.UpdatedLeaf.getByteRange();
            for (auto i = Start; i != End; ++i)
              assignTag(OldTag[i], {&UpdateAction, Start, End});
            for (auto i = NewStart; i != NewEnd; ++i)
              assignTag(NewTag[i], {&UpdateAction, NewStart, NewEnd});
          }

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::MoveTree>) {
            const auto [Start, End] = Action.Subtree.getByteRange();
            const auto [NewStart, NewEnd] = Action.MovedSubtree.getByteRange();
            for (auto i = Start; i != End; ++i)
              assignTag(OldTag[i], {&MoveAction, Start, End});
            for (auto i = NewStart; i != NewEnd; ++i)
              assignTag(NewTag[i], {&MoveAction, NewStart, NewEnd});
          }
        },
        Action);
  }

  auto showLineNumber = [](uint32_t &LineNum,
                           uint32_t DigitCount) -> std::string {
    auto Line = std::to_string(LineNum++);
    return R"(<span style="background-color:rgb(223, 223, 223);">)" +
           std::string(DigitCount - Line.size(), ' ') + Line + R"( |</span> )";
  };

  const uint32_t OldLineEnd{OldSrc.getLOC()};
  const uint32_t OldDigitCount = std::to_string(OldLineEnd).size();
  uint32_t OldLineNum{1};
  std::string OldBuf(BeginTemplate);
  OldBuf.append(showLineNumber(OldLineNum, OldDigitCount));
  auto OldCStr = OldSrc.getContent();
  const std::string_view *CurrentTag{nullptr};

  for (std::size_t i{0}; i != OldSrc.getSize(); ++i) {
    if (CurrentTag != OldTag[i].Tag) {
      if (CurrentTag)
        OldBuf.append(ActionEnd);
      CurrentTag = OldTag[i].Tag;
      if (CurrentTag)
        OldBuf.append(*CurrentTag);
    }

    switch (OldCStr[i]) {
    case '\n':
      // OldBuf.append(" \n");
      OldBuf.push_back(OldCStr[i]);
      OldBuf.append(showLineNumber(OldLineNum, OldDigitCount));
      break;
    case '&':
      OldBuf.append(AmpChar);
      break;
    case '<':
      OldBuf.append(LessChar);
      break;
    case '>':
      OldBuf.append(GreaterChar);
      break;
    case '"':
      OldBuf.append(DoubleQuoteChar);
      break;
    case '\'':
      OldBuf.append(SingleQuoteChar);
      break;
    default:
      OldBuf.push_back(OldCStr[i]);
    }
  }
  if (CurrentTag)
    OldBuf.append(ActionEnd);

  const uint32_t NewLineEnd{NewSrc.getLOC()};
  const uint32_t NewDigitCount = std::to_string(NewLineEnd).size();
  uint32_t NewLineNum{1};
  std::string NewBuf(BeginTemplate);
  NewBuf.append(showLineNumber(NewLineNum, NewDigitCount));
  auto NewCStr = NewSrc.getContent();
  CurrentTag = nullptr;

  for (std::size_t i{0}; i != NewSrc.getSize(); ++i) {
    if (CurrentTag != NewTag[i].Tag) {
      if (CurrentTag)
        NewBuf.append(ActionEnd);
      CurrentTag = NewTag[i].Tag;
      if (CurrentTag)
        NewBuf.append(*CurrentTag);
    }

    switch (NewCStr[i]) {
    case '\n':
      NewBuf.push_back(NewCStr[i]);
      NewBuf.append(showLineNumber(NewLineNum, NewDigitCount));
      break;
    case '&':
      NewBuf.append(AmpChar);
      break;
    case '<':
      NewBuf.append(LessChar);
      break;
    case '>':
      NewBuf.append(GreaterChar);
      break;
    case '"':
      NewBuf.append(DoubleQuoteChar);
      break;
    case '\'':
      NewBuf.append(SingleQuoteChar);
      break;
    default:
      NewBuf.push_back(NewCStr[i]);
    }
  }
  if (CurrentTag)
    NewBuf.append(ActionEnd);

  OldBuf.append(EndTemplate);
  NewBuf.append(EndTemplate);
  return {OldBuf, NewBuf};
}

nlohmann::ordered_json ScriptExporter::exportAsFullJSON() const {
  using json = nlohmann::ordered_json;
  auto Buffer = json::object();
  std::unordered_map<const diffink::HashNode *, std::size_t> IdMap;
  std::size_t Id{0};
  Buffer["original-tree"] = fromTreeToJSON(OldTree.getRoot(), IdMap, Id);
  Id = 0;
  Buffer["modified-tree"] = fromTreeToJSON(NewTree.getRoot(), IdMap, Id);
  Buffer["mappings"] = fromMappingToJSON(IdMap);
  Buffer["edit-script"] = fromMappingToScript(IdMap);
  return Buffer;
}

std::string ScriptExporter::exportAsStringEX() const {
  std::string Buffer;
  for (const auto &Action : diffink::simplifyEditScript(Script)) {
    std::visit(
        [&Buffer](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;
          constexpr std::string_view BigSeperator = "===\n";
          constexpr std::string_view SmallSeperator = "---\n";

          if constexpr (std::is_same_v<T, diffink::edit_action::InsertNode>)
            Buffer.append(BigSeperator)
                .append("insert-node\n")
                .append(SmallSeperator)
                .append(Action.Leaf.toString())
                .append("\nto\n")
                .append(Action.Parent.toString())
                .append("\nat ")
                .append(std::to_string(Action.Index + 1));

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteNode>)
            Buffer.append(BigSeperator)
                .append("delete-node\n")
                .append(SmallSeperator)
                .append(Action.Leaf.toString());

          else if constexpr (std::is_same_v<T, diffink::edit_action::MoveTree>)
            Buffer.append(BigSeperator)
                .append("move-tree\n")
                .append(SmallSeperator)
                .append(Action.Subtree.toStringRecursively())
                .append("\nto\n")
                .append(Action.Parent.toString())
                .append("\nat ")
                .append(std::to_string(Action.Index + 1));

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::UpdateNode>)
            Buffer.append(BigSeperator)
                .append("update-node\n")
                .append(SmallSeperator)
                .append(Action.Leaf.toString())
                .append("\nreplace \"")
                .append(Action.Leaf.getLabel())
                .append("\" by \"")
                .append(Action.UpdatedLeaf.getLabel())
                .push_back('"');

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::InsertTree>)
            Buffer.append(BigSeperator)
                .append("insert-tree\n")
                .append(SmallSeperator)
                .append(Action.Subtree.toStringRecursively())
                .append("\nto\n")
                .append(Action.Parent.toString())
                .append("\nat ")
                .append(std::to_string(Action.Index + 1));

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteTree>)
            Buffer.append(BigSeperator)
                .append("delete-tree\n")
                .append(SmallSeperator)
                .append(Action.Subtree.toStringRecursively());

          Buffer.push_back('\n');
        },
        Action);
  }

  if (!Buffer.empty())
    Buffer.pop_back();
  return Buffer;
}