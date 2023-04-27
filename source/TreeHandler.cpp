#include "TreeHandler.h"
#include "Debug.h"

#include <string>
#include <vector>

#include <TSystem.h>

// The name passed to instatiate the TChain must be the same of the TTrees 
// in the TFile
TreeHandler::TreeHandler(std::string fileName_RAW):
    m_Data("hitstree", "Data chain"),
    m_Header("headertree", "Header chain")
{
    size_t pos = fileName_RAW.find("_0_RAW.root");
    // erase the last part
    fileName_RAW.erase( fileName_RAW.begin() + pos, fileName_RAW.end() );
    m_FileName_CLZ = fileName_RAW;

    // Loops over files in the directory. As files are found, link them to 
    // the chain and go to the next one.
    unsigned int iFile = 0;
    // TSystem::AccessPathName returns false if one can access the file, so if 
    // the file is found we have to negate the expression
    while( !gSystem->AccessPathName( UpdateClusterFileName(iFile).c_str() ) ){
        m_Data.Add( UpdateClusterFileName(iFile).c_str() );
        ++iFile;
    }

    std::cout << "Total cluster files attached to data chain = " << iFile << std::endl;

    DBPRINT("Data chain entries = ", m_Data.GetEntries());

    // The data chain contains all the informations stored in the Trees of the CLZ files.
    // To read these data we can use a TTreeReader object
    m_Reader = new TTreeReader(&m_Data);

    ReadTree(m_Reader);

    DBPRINT("Reading data chain: ", m_Data.GetName() );

    // variable for header chain
    float header_data[16];

    // open the header, link it and set the branch address
    m_Header.Add( UpdateClusterFileName(1).c_str() );       // first cluster file
    m_Header.SetBranchAddress("run header", &header_data);
}

TreeHandler::~TreeHandler(){
    delete m_Reader;
};


// fileName_CLZ is obtained starting from the RAW file, by removing the last part and by
// adding the file number and "_CLZ.root" at the end
std::string TreeHandler::UpdateClusterFileName(unsigned int iFile){
    
    std::string fileName_CLZ;
    fileName_CLZ = m_FileName_CLZ + "_" + std::to_string(iFile) + "_CLZ.root";

    return fileName_CLZ;
}

void TreeHandler::ReadTree(TTreeReader* reader){

    TTreeReaderValue< std::vector<int> > row_seed(*reader, "row_seed");     // Seed y
    TTreeReaderValue< std::vector<int> > col_seed(*reader, "col_seed");     // Seed x
    TTreeReaderValue< std::vector<float> > x_val(*reader, "clusterx");      // center of gravity X for cluster
    TTreeReaderValue< std::vector<float> > y_val(*reader, "clustery");      // center of gravity Y for cluster
    TTreeReaderValue< std::vector<float> > max_val(*reader, "clusterh");    // max value for cluster
    TTreeReaderValue< std::vector<float> > ph_nxn(*reader, "ph_nxn");       // pulse height for matrix cluster
    TTreeReaderValue< std::vector<float> > noise_nxn(*reader, "noise_nxn"); // noise for matrix cluster

    TreeData<int>* dataInt = TreeData<int>::GetInstance();
    TreeData<float>* dataFloat = TreeData<float>::GetInstance();

    dataInt->Add(row_seed);
    dataInt->Add(col_seed);
    dataFloat->Add(x_val);
    dataFloat->Add(y_val);
    dataFloat->Add(max_val);
    dataFloat->Add(ph_nxn);
    dataFloat->Add(noise_nxn);

    return;
}