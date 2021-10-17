#ifndef BABYLON_MISC_CODE_STRING_PARSING_TOOLS_H
#define BABYLON_MISC_CODE_STRING_PARSING_TOOLS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Helpers to parse code strings.
 */
struct BABYLON_SHARED_EXPORT CodeStringParsingTools {

  /**
   * @brief Extracts the characters between two markers (for eg, between "(" and ")"). The function
   * handles nested markers as well as markers inside strings (delimited by ", ' or `) and comments.
   * @param markerOpen opening marker
   * @param markerClose closing marker
   * @param block code block to parse
   * @param startIndex starting index in block where the extraction must start. The character at
   * block[startIndex] should be the markerOpen character!
   * @returns index of the last character for the extraction (or -1 if the string is invalid - no
   * matching closing marker found). The string to extract (without the markers) is the string
   * between startIndex + 1 and the returned value (exclusive)
   */
  static int ExtractBetweenMarkers(char markerOpen, char markerClose, const std::string& block,
                                   size_t startIndex);

  /**
   * @brief Parses a string and skip whitespaces.
   * @param s string to parse
   * @param index index where to start parsing
   * @returns the index after all whitespaces have been skipped
   */
  static size_t SkipWhitespaces(const std::string& s, size_t index);

  /**
   * @brief Checks if a character is an identifier character (meaning, if it is 0-9, A-Z, a-z or _).
   * @param c character to check
   * @returns true if the character is an identifier character
   */
  static bool IsIdentifierChar(const std::string& c);

  /**
   * @brief Removes the comments of a code block.
   * @param block code block to parse
   * @returns block with the comments removed
   */
  static std::string RemoveComments(const std::string& block);

  /**
   * @brief Finds the first occurrence of a character in a string going backward.
   * @param s the string to parse
   * @param index starting index in the string
   * @param c the character to find
   * @returns the index of the character if found, else -1
   */
  static int FindBackward(const std::string& s, int index, char c);

  /**
   * @brief Escapes a string so that it is usable as a regular expression.
   * @param s string to escape
   * @returns escaped string
   */
  static std::string EscapeRegExp(const std::string& s);

}; // end of struct CodeStringParsingTools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_CODE_STRING_PARSING_TOOLS_H
