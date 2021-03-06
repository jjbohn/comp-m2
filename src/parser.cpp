#include "parser.hpp"

#include <numeric>
#include <string>
#include <vector>

#include "const.hpp"
#include "environment_const_string.hpp"
#include "instruction.hpp"
#include "load.hpp"
#include "ram.hpp"

using namespace std;

string Parser::parse(vector<string> filenamesIn, bool outputChars, 
                     bool inputChars, bool rawInput) {
  vector<Ram> rams = vector<Ram>(filenamesIn.size());
  // Fills rams with contents of files.
  for (size_t i = 0; i < filenamesIn.size(); i++) {
    Load::fillRamWithFile(filenamesIn[i].c_str(), rams[i]);
  }
  string source = SOURCE_INCLUDES + "\n\n";
  if (rawInput) {
    string environment = accumulate(environmentConstString.begin(),
                                    environmentConstString.end(), string(""));
    source += environment;
  }
  source += SOURCE_HEADER + "\n\n";
  if (rawInput) {
    source += PRINT_RAW + "\n\n";
  } else if (outputChars) {
    source += PRINT_OUTPUT_CHARS + "\n\n";
  } else {
    source += PRINT_BASIC + "\n\n";
  }
  if (rawInput) {
    source += F0_RAW + "\n\n";
  } else if (inputChars) {
    source += F0_INPUT_CHARS + "\n\n";
  } else {
    source += F0_BASIC + "\n\n";
  }
  for (size_t i = 0; i < filenamesIn.size(); i++) {
    source += getComputerFunction(rams[i], i)+ "\n\n";
  }
  if (rawInput) {
    source += SOURCE_FOOTER_1_RAW;
  } else {
    source += SOURCE_FOOTER_1;
  }
  source += to_string(filenamesIn.size()) + 
            SOURCE_FOOTER_2 + "\n";
  return source;
}

string Parser::getComputerFunction(Ram ram, int index) {
  string function;
  function += getFunctionHeader(ram.state[DATA], index)+ "\n";
  for (size_t i = 0; i < RAM_SIZE; i++) {
    function += getLineOfCode(ram.state[CODE][i], i, ram) + "\n";
  }
  function += SOURCE_FUNCTION_FOOTER;
  return function;
}

string Parser::getFunctionHeader(vector<vector<bool>> data, int index) {
  return SOURCE_FUNCTION_HEADER_1 + to_string(index+1) + 
         SOURCE_FUNCTION_HEADER_2 + to_string(index) +
         SOURCE_FUNCTION_HEADER_3 + getData(data) +
         SOURCE_FUNCTION_HEADER_4;
}

string Parser::getData(vector<vector<bool>> data) {
  string out;
  bool first = true;
  for (vector<bool> word : data) {
    if (first) {
      first = false;
      out += to_string(Util::getInt(word));
    } else {
      out += ", " + to_string(Util::getInt(word));
    }
  }
  return out;
}

string Parser::getLineOfCode(vector<bool> word, int index, Ram ram) {
  Instruction inst = Instruction(word, EMPTY_WORD, &ram);
  string strIndex;
  if (index < 10) {
    strIndex = "0";
  }
  strIndex += to_string(index);
  string instCode = inst.getCode(index+1);
  return "  a" + strIndex + ": " + instCode;
}