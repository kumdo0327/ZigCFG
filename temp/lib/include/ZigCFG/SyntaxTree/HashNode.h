#ifndef DIFFINK_SYNTAXTREE_HASHNODE_H
#define DIFFINK_SYNTAXTREE_HASHNODE_H

#include "DiffInk/SyntaxTree/SourceCode.h"
#include "DiffInk/Utils/XxhStl.h"
#include <format>
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace diffink {

constexpr std::size_t DefaultIndent{2};

struct BuildConfig {
  std::unordered_set<std::string> Flattened;
  std::unordered_map<std::string, std::string> Aliased;
  std::unordered_set<std::string> Ignored;
};

class HashNode {
public:
  struct UTF8Range {
    TSPoint StartPos;
    TSPoint EndPos;

    std::string stringify() const;
  };

private:
  const std::string Type;
  const std::string Label;
  const std::pair<uint32_t, uint32_t> ByteRange;
  const UTF8Range PosRange;
  std::list<HashNode> Children;
  uint32_t Height{0};
  XXH64_hash_t ExactHash;
  XXH64_hash_t StructuralHash;

private:
  HashNode(const TSNode &RawNode, std::string &&Type, const SourceCode &Code,
           bool IsLeaf) noexcept;

  HashNode(const HashNode &Rhs) = delete;

  void stringifySubtree(std::string &Buffer, std::size_t Depth,
                        std::size_t Indent) const;

  void makeMetadata();

  void makePostOrder(std::vector<const HashNode *> &PostOrder,
                     const HashNode *Iter) const;

  static void build(const SourceCode &Code, TSTreeCursor &Cursor,
                    HashNode &Parent);

  static void build(const SourceCode &Code, TSTreeCursor &Cursor,
                    HashNode &Parent, const BuildConfig &Config);

public:
  bool isLeaf() const noexcept { return Children.empty(); }

  uint32_t getHeight() const noexcept { return Height; }

  const std::string &getType() const noexcept { return Type; }

  const std::string &getLabel() const noexcept { return Label; }

  const decltype(Children) &getChildren() const noexcept { return Children; }

  const decltype(ByteRange) &getByteRange() const noexcept { return ByteRange; }

  const decltype(PosRange) &getUTF8PosRange() const noexcept {
    return PosRange;
  }

  XXH64_hash_t getTypeHash() const noexcept { return xxhString64(Type); }

  XXH64_hash_t getExactHash() const noexcept { return ExactHash; }

  XXH64_hash_t getStructuralHash() const noexcept { return StructuralHash; }

  std::vector<const HashNode *> makePostOrder() const;

  std::string stringifyNode() const;

  std::string stringifySubtree(std::size_t Indent = DefaultIndent) const;

  static std::unique_ptr<HashNode> build(TSNode RootNode,
                                         const SourceCode &Code,
                                         const BuildConfig *Config = nullptr);

  static bool equalExactly(const HashNode &Left,
                           const HashNode &Right) noexcept {
    return Left.ExactHash == Right.ExactHash;
  }

  static bool equalStructurally(const HashNode &Left,
                                const HashNode &Right) noexcept {
    return Left.StructuralHash == Right.StructuralHash;
  }
};

} // namespace diffink

#endif // DIFFINK_SYNTAXTREE_HASHNODE_H