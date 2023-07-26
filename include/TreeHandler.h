#ifndef TreeHandler_h
#define TreeHandler_h

#include <TChain.h>
#include <TTreeReader.h>

#include "Parameters.h"
#include "TreeData.h"

class TreeHandler{

    public:

    TreeHandler() = delete;
    TreeHandler(const TreeHandler&) = delete;
    TreeHandler(std::string fileName_RAW, bool doPreAnalysis);
    ~TreeHandler();

    inline
    const TChain& GetDataChain() const { return m_Data; };

    inline
    const TChain& GetHeaderChain() const { return m_Header; };

    inline TTreeReader* GetReader() const { return m_Reader; };

    private:
    // changes the cluster file number attached to the cluster file name
    std::string UpdateClusterFileName(unsigned int iFile);
    void ReadTree(TTreeReader* reader);

    void GetTriggerFrames(std::string clzFile);
    int PreAnalyzeClzFiles(std::string path, std::string fileNameRAW);

    std::string m_FileName_RAW;
    std::string m_FileName_CLZ;
    std::string m_preAnalysisDir;

    // A TChain is a collection of TFile objects. The first parameter "name" is the name of the 
    // TTree object in the files added with Add method
    TChain m_Data;
    TChain m_Header;

    TTreeReader* m_Reader;

    // vectors used to select trigger frames from each CLZ file
    std::vector<int> m_rowseed;
    std::vector<int> m_colseed;
    std::vector<unsigned int> m_fidval;
    std::vector<float> m_xval;
    std::vector<float> m_yval;
    std::vector<float> m_maxval;
    std::vector<float> m_ph;
    std::vector<float> m_noise;


};

#endif