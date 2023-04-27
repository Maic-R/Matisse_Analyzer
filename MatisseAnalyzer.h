#ifndef MatisseAnalyzer_h
#define MatisseAnalyzer_h

#include <string>
#include <vector>

#include "Parameters.h"
#include "AnalysisObject.h"
#include "CommandLine.h"
#include "TreeHandler.h"

#include <TApplication.h>
#include <TFile.h>

// run all the analysis for MATISSE data
class MatisseAnalyzer{

    public:

    MatisseAnalyzer() = delete;
    MatisseAnalyzer(const MatisseAnalyzer&) = delete;
    MatisseAnalyzer(int argc, char** argv);
    ~MatisseAnalyzer();

    void beginAnalysis();
    void analyze();
    void endAnalysis();

    void runApplication(TApplication& app);

    private:

    std::string GetPath(std::string fileName);

    // to retrieve analysis information
    CommandLine m_CommandLine;

    // analysis paramters retrieved in the file name
    Parameters m_Parameters;
    std::vector<std::string> m_AnalysisInfo;
    std::vector<AnalysisObject*> m_vAnalysisObj;

    TreeHandler* m_TreeHandler;
    TFile* m_RootFile;

};

#endif