#include "CommandLine.h"
#include "Parameters.h"
#include "Debug.h"

using namespace std;

// default value to return if a word is not found
string CommandLine::m_DefaultString = "Error in the key word";

// file name error string
string CommandLine::m_FileNameError = "Unable to retrieve file name";

// number of words to ignore, exe name and input file.
// By default ignore the exe name
unsigned int CommandLine::m_FirstWords = 1;

CommandLine::CommandLine(int argc, char** argv){

  m_Words.reserve(argc);
  for(int i=m_FirstWords; i < argc; ++i)
    m_Words.emplace_back(argv[i]);

  m_AnalysisInfo.reserve( m_Words.size() );

  for(unsigned int i=0; i < m_AnalysisInfo.capacity() - m_FirstWords; ++i){
    // skip file name
    string word = m_Words.at(i + m_FirstWords);
    m_AnalysisInfo.emplace_back( word );
  }

  DBPRINT("Number of words found in command line = ", m_Words.size());
  #if VERBOSE
    cout << "Words found are : \n";
    for(auto i : m_Words)
      cout << i << endl;
  #endif

}

CommandLine::~CommandLine(){
  m_Words.clear();
  m_AnalysisInfo.clear();
}

bool CommandLine::contains(const string& word) const{
  for(auto it : m_Words){
    if( (it == word) || (it.find(word)!=std::string::npos) )
      return true; 
  }
  return false;
}

const vector<string>& CommandLine::GetInfoForAnalysis() const {

  DBPRINT("Number of keywords found for the analysis : ", m_AnalysisInfo.size());
  #if VERBOSE
    cout << "Keywords are : \n";
      for(auto i : m_AnalysisInfo)
        cout << i << endl;
  #endif

  return m_AnalysisInfo;
}

const string& CommandLine::getFileNameRAW() const {
  for(auto word : m_Words){
    if( word.find( (char)IDCHAR::ENDFILE ) != string::npos ){
      m_FileName = word;
      return m_FileName;
    }
  }
  return m_FileNameError;
}

const unsigned int CommandLine::getValue(std::string keyword) const{

  if(!CommandLine::contains(keyword)) return 0;

  std::string word;
  // loop over all the words passed in command line
  for(auto w : m_AnalysisInfo){
    // when word contains a certain keyword, it checks for "=" position
    if( w.find(keyword) != std::string::npos ){
      word = w;
      for(auto it = word.begin(); it != word.end(); ++it){
        if(*it == '='){
          // erase all the letters to get the value after the "=" sign
          word.erase(word.begin(), it + 1);
          break;
        }
      }
      break;
    }
  }

  return std::stoul(word);

}

