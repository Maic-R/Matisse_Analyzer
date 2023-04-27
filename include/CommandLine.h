#ifndef CommandLine_h
#define CommandLine_h

#include <string>
#include <vector>

class CommandLine{

    public:

    CommandLine(int argc, char** argv);
    CommandLine(const CommandLine&) = delete;
    ~CommandLine();

    // check if the word "word" is present
    bool contains(const std::string& word) const;

    // returns information for the analysis, which have "--" before the name
    const std::vector<std::string>& GetInfoForAnalysis() const;

    // list of all words passed in command line
    inline const std::vector<std::string>& getAllInfo() const { return m_Words; };
    inline std::string& getDefaultString() const { return m_DefaultString; };

    const std::string& getFileNameRAW() const;

    // function to get values given a keyword, using the following syntax:
    // keyWord=value
    const unsigned int getValue(std::string keyword) const;

    private:

    static std::string m_DefaultString;
    static std::string m_FileNameError;
    static unsigned int m_FirstWords;
    mutable std::string m_FileName;
    std::vector<std::string> m_Words;
    std::vector<std::string> m_AnalysisInfo;

};

#endif
