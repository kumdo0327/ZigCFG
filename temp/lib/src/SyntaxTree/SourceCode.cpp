#include "DiffInk/SyntaxTree/SourceCode.h"

namespace diffink {

SourceCode::SourceCode(std::string &&Content)
    : Content(std::move(Content)), Lines{0} {
  constexpr unsigned char OneByteMask{0b10000000};
  constexpr unsigned char OneByteExpected{0};
  constexpr unsigned char ContinuationMask{0b11000000};
  constexpr unsigned char ContinuationExpected{0b10000000};
  constexpr unsigned char TwoByteMask{0b11100000};
  constexpr unsigned char TwoByteExpected{0b11000000};
  constexpr unsigned char ThreeByteMask{0b11110000};
  constexpr unsigned char ThreeByteExpected{0b11100000};
  constexpr unsigned char FourByteMask{0b11111000};
  constexpr unsigned char FourByteExpected{0b11110000};

  Pos.reserve(getSize() + 1);
  UTF8Pos.reserve(getSize() + 1);

  uint32_t Row{0}, Col{0};
  uint32_t UTF8Row{0}, UTF8Col{0};
  auto CStr = getContent();
  auto NullOffset = Content.size();

  for (std::string::size_type i = 0; i != Content.size();) {
    Pos.push_back(TSPoint{Row, Col});
    UTF8Pos.push_back(TSPoint{Row, Col});
    auto Byte = static_cast<unsigned char>(CStr[i]);

    if ((Byte & OneByteMask) == OneByteExpected) {
      if (CStr[i] == '\n') {
        ++Row;
        ++UTF8Row;
        Col = 0;
        UTF8Col = 0;
        Lines.push_back(i + 1);
      } else {
        ++UTF8Col;
        ++Col;
      }
      ++i;
      continue;
    }

    if ((Byte & TwoByteMask) == TwoByteExpected) {
      if (i + 1 >= NullOffset)
        throw std::runtime_error(
            "Invalid source code: only UTF-8 is supported");
      if ((static_cast<unsigned char>(CStr[i + 1]) & ContinuationMask) !=
          ContinuationExpected)
        throw std::runtime_error(
            "Invalid source code: only UTF-8 is supported");
      i += 2;
      Pos.push_back(TSPoint{Row, Col + 1});
      Col += 2;
      UTF8Pos.push_back(TSPoint{UTF8Row, UTF8Col});
      ++UTF8Col;
      continue;
    }

    if ((Byte & ThreeByteMask) == ThreeByteExpected) {
      if (i + 2 >= NullOffset)
        throw std::runtime_error(
            "Invalid source code: only UTF-8 is supported");
      if ((static_cast<unsigned char>(CStr[i + 1]) & ContinuationMask) !=
              ContinuationExpected ||
          (static_cast<unsigned char>(CStr[i + 2]) & ContinuationMask) !=
              ContinuationExpected)
        throw std::runtime_error(
            "Invalid source code: only UTF-8 is supported");
      i += 3;
      Pos.push_back(TSPoint{Row, Col + 1});
      Pos.push_back(TSPoint{Row, Col + 2});
      Col += 3;
      UTF8Pos.push_back(TSPoint{UTF8Row, UTF8Col});
      UTF8Pos.push_back(TSPoint{UTF8Row, UTF8Col});
      ++UTF8Col;
      continue;
    }

    if ((Byte & FourByteMask) == FourByteExpected) {
      if (i + 3 >= NullOffset)
        throw std::runtime_error(
            "Invalid source code: only UTF-8 is supported");
      if ((static_cast<unsigned char>(CStr[i + 1]) & ContinuationMask) !=
              ContinuationExpected ||
          (static_cast<unsigned char>(CStr[i + 2]) & ContinuationMask) !=
              ContinuationExpected ||
          (static_cast<unsigned char>(CStr[i + 3]) & ContinuationMask) !=
              ContinuationExpected)
        throw std::runtime_error(
            "Invalid source code: only UTF-8 is supported");
      i += 4;
      Pos.push_back(TSPoint{Row, Col + 1});
      Pos.push_back(TSPoint{Row, Col + 2});
      Pos.push_back(TSPoint{Row, Col + 3});
      Col += 4;
      UTF8Pos.push_back(TSPoint{UTF8Row, UTF8Col});
      UTF8Pos.push_back(TSPoint{UTF8Row, UTF8Col});
      UTF8Pos.push_back(TSPoint{UTF8Row, UTF8Col});
      ++UTF8Col;
      continue;
    }
    throw std::runtime_error("Invalid source code: only UTF-8 is supported");
  }
  Lines.push_back(getSize());
  Pos.push_back(TSPoint{Row, Col});
  UTF8Pos.push_back(TSPoint{UTF8Row, UTF8Col});
}

} // namespace diffink