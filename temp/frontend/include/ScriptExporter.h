#ifndef FRONTEND_SCRIPTEXPORTER_H
#define FRONTEND_SCRIPTEXPORTER_H

#include "DiffInk/TreeDiff/TreeDiff.h"
#include <cmath>
#include <nlohmann/json.hpp>

class ScriptExporter {
public:
  using DiffResult =
      std::pair<diffink::TreeDiff::Mappings, diffink::EditScript>;

private:
  const diffink::MerkleTree &OldTree;
  const diffink::MerkleTree &NewTree;
  diffink::TreeDiff::Mappings Map;
  diffink::EditScript Script;

private:
  nlohmann::ordered_json fromTreeToJSON(
      const diffink::HashNode &Node,
      std::unordered_map<const diffink::HashNode *, std::size_t> &IdMap,
      std::size_t &Id) const;

  nlohmann::ordered_json fromMappingToJSON(
      const std::unordered_map<const diffink::HashNode *, std::size_t> &IdMap)
      const;

  nlohmann::ordered_json fromMappingToScript(
      const std::unordered_map<const diffink::HashNode *, std::size_t> &IdMap)
      const;

public:
  ScriptExporter(const diffink::MerkleTree &OldTree,
                 const diffink::MerkleTree &NewTree, DiffResult &&Diff) noexcept
      : OldTree{OldTree}, NewTree{NewTree}, Map(std::move(Diff.first)),
        Script(std::move(Diff.second)) {}

  std::string exportAsString() const;

  nlohmann::ordered_json exportAsJSON() const;

  std::pair<std::string, std::string>
  exportAsHTML(const diffink::SourceCode &OldSrc,
               const diffink::SourceCode &NewSrc) const;

  nlohmann::ordered_json exportAsFullJSON() const;

  std::string exportAsStringEX() const;
};

#endif // FRONTEND_SCRIPTEXPORTER_H