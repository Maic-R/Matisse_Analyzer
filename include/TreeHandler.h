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
    TreeHandler(std::string fileName_RAW);
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

    std::string m_FileName_CLZ;

    // A TChain is a collection of TFile objects. The first parameter "name" is the name of the 
    // TTree object in the files added with Add method
    TChain m_Data;
    TChain m_Header;

    TTreeReader* m_Reader;

};

#endif