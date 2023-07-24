#include "TreeHandler.h"
#include "Debug.h"
#include "Cluster.h"

#include <string>
#include <vector>
#include <filesystem>

#include <TSystem.h>
#include <TFile.h>

// The name passed to instatiate the TChain must be the same of the TTrees 
// in the TFile
TreeHandler::TreeHandler(std::string fileName_RAW):
    m_Data("hitstree", "Data chain"),
    m_Header("headertree", "Header chain")
{
    // get RAW file path
    size_t last_slash = fileName_RAW.find_last_of('/');
    std::string rawFilePath = fileName_RAW.substr(0, last_slash);
    m_FileName_RAW = fileName_RAW.substr(last_slash+1);
    m_preAnalysisDir = rawFilePath + "/preAnalysis";
    std::filesystem::path preAnalysisPath = std::string(m_preAnalysisDir);
    std::cout << "pre Analysis path = " << preAnalysisPath << std::endl;
    std::cout << "pre Analysis dir = " << m_preAnalysisDir << std::endl;
    //bool preAnalysisExists = std::filesystem::is_directory(preAnalysisPath.parent_path());
    bool preAnalysisExists = std::filesystem::exists(preAnalysisPath);
    std::cout << "pre Analysis exists = " << preAnalysisExists << std::endl;

    // if pre analysis has already been done do not create the directory and the 
    // skip the selection of trigger frames for each clz file
    if(!preAnalysisExists){
        std::string makeDirCommand( "mkdir -p " + rawFilePath + "/preAnalysis" );
        // create a sub directory for new CLZ files
        system( makeDirCommand.c_str() );
    }


    // pre analyze cluser files to get trigger frames
    int preAnalysisStatus = preAnalysisExists ? 0 : TreeHandler::PreAnalyzeClzFiles(rawFilePath, fileName_RAW);

    if(preAnalysisStatus){
        std::cout << "Error during cluster pre analysis!\n";
    }

    size_t pos = fileName_RAW.find("_0_RAW.root");
    // erase the last part
    fileName_RAW.erase( fileName_RAW.begin() + pos, fileName_RAW.end() );
    m_FileName_CLZ = fileName_RAW.substr(last_slash, pos);

    // Loops over files in the directory. As files are found, link them to 
    // the chain and go to the next one.
    // TSystem::AccessPathName returns false if one can access the file, so if 
    // the file is found we have to negate the expression
    unsigned int iFile=0;
    while( !gSystem->AccessPathName( UpdateClusterFileName(iFile).c_str() ) ){
        std::cout << UpdateClusterFileName(iFile) << std::endl;
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
    fileName_CLZ = m_preAnalysisDir + m_FileName_CLZ + "_" + std::to_string(iFile) + "_CLZ.root";

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

void TreeHandler::GetTriggerFrames(std::string clzFile){

    std::cout << "Get trigger frames for cluster file : " << clzFile << std::endl;

    //TFile* Clzfile = new TFile(clzFile.c_str(), "READ");
    TFile Clzfile (clzFile.c_str(), "READ");
    TTree* headertree = dynamic_cast<TTree*>( Clzfile.Get("headertree") );
    TTree* eventtree  = dynamic_cast<TTree*>( Clzfile.Get("eventtree" ) );
    TTree* hitstree = dynamic_cast<TTree*>( Clzfile.Get("hitstree") );
    TTreeReader* reader = new TTreeReader(hitstree); 

    TTreeReaderValue< std::vector<int> > row_seed(*reader, "row_seed");         // Seed y
    TTreeReaderValue< std::vector<int> > col_seed(*reader, "col_seed");         // Seed x
    TTreeReaderValue< std::vector<float> > x_val(*reader, "clusterx");          // center of gravity X for cluster
    TTreeReaderValue< std::vector<float> > y_val(*reader, "clustery");          // center of gravity Y for cluster
    TTreeReaderValue< std::vector<float> > max_val(*reader, "clusterh");        // max value for cluster
    TTreeReaderValue< std::vector<float> > ph_nxn(*reader, "ph_nxn");           // pulse height for matrix cluster
    TTreeReaderValue< std::vector<float> > noise_nxn(*reader, "noise_nxn");     // noise for matrix cluster
    TTreeReaderValue< std::vector<unsigned int> > fid_val(*reader, "fid_val");  // frame ID value 

    std::vector<unsigned int> triggerFrames;

    // loop over Tree entries to get trigger frames. Trigger frames
    // are given by a cluster in the third or fourth column of the sensor
    while(reader->Next()){
        // continue if no cluster is found
        if(col_seed->size() == 0)
            continue;
        // loop over all clusters
        for(unsigned int i=0; i < col_seed->size(); ++i){
            if( col_seed->at(i) != 3 && col_seed->at(i) != 4 )
                continue;
            triggerFrames.push_back( fid_val->at(i) );
        }
    }

    // restart the loop to get all the informations associated to trigger frames
    reader->Restart();
    while(reader->Next()){
        // continue if no cluster is found
        if(col_seed->size() == 0)
            continue;

        // loop over all clusters
        for(unsigned int i=0; i < col_seed->size(); ++i){
            unsigned int frame = fid_val->at(i);
            // check that this specific frame is a trigger frame AND that there is a 
            // cluster in second, third or fourth sector
            if( std::find(triggerFrames.begin(), triggerFrames.end(), frame) != triggerFrames.end() 
                && col_seed->at(i) > 6 ){
                
                // save all the informations associated to this specific trigger frame
                m_rowseed.push_back( row_seed->at(i) );
                m_colseed.push_back( col_seed->at(i) );
                m_fidval.push_back( fid_val->at(i) );
                m_xval.push_back( x_val->at(i) );
                m_yval.push_back( y_val->at(i) );
                m_maxval.push_back( max_val->at(i) );
                for(unsigned int k=0; k<Clz::MatrixSize; ++k){
                    unsigned int j = Clz::MatrixSize*i + k;
                    m_ph.push_back( ph_nxn->at(j) );
                    m_noise.push_back( noise_nxn->at(j) );
                }

                    
            }
        }
    }

    // extract clz file name from clz file path
    std::string clzName = clzFile.substr( clzFile.find_last_of('/') );
    //TFile* newClzFile = new TFile( (m_preAnalysisDir + clzName).c_str(), "RECREATE" );
    TFile newClzFile ( (m_preAnalysisDir + clzName).c_str(), "RECREATE" );

    newClzFile.cd();

    // create new hitstree
    TTree* newhitstree = new TTree("hitstree","Hit Data");
    newhitstree->Branch("row_seed", &m_rowseed);
    newhitstree->Branch("col_seed", &m_colseed);
    newhitstree->Branch("clusterx", &m_xval);
    newhitstree->Branch("clustery", &m_yval);
    newhitstree->Branch("clusterh", &m_maxval);
    newhitstree->Branch("ph_nxn", &m_ph);
    newhitstree->Branch("noise_nxn", &m_noise);
    newhitstree->Branch("fid_val", &m_fidval);

    newhitstree->Fill();

    headertree->CloneTree()->Write();
    eventtree->CloneTree()->Write();
    newhitstree->Write();

    newClzFile.Write();
    newClzFile.Close();
    Clzfile.Close();

    //delete newClzFile;

    // clear data before analysis next clz file
    m_rowseed.clear();
    m_colseed.clear();
    m_fidval.clear();
    m_xval.clear();
    m_yval.clear();
    m_maxval.clear();
    m_ph.clear();
    m_noise.clear();

    return;
}

int TreeHandler::PreAnalyzeClzFiles(std::string path, std::string fileNameRAW){

    for(const auto& entry : std::filesystem::directory_iterator(path) ){
        // skip all the subdirectories
        if( std::filesystem::is_directory(entry) )
            continue;
        // skip raw file
        if( entry.path() == fileNameRAW )
            continue;
        //if( entry.path() == "../Data/metodoDevis/B10_Sr-90_150V_1050_CLZ.root")
        //    continue;
        TreeHandler::GetTriggerFrames(entry.path());
    }

    return 0;
}