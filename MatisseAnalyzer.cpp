#include "MatisseAnalyzer.h"
#include "Parameters.h"
#include "CommandLine.h"
#include "Debug.h"
#include "Seed.h"
#include "Cluster.h"
#include "Noise.h"
#include "TreeHandler.h"
#include "TreeData.h"

#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>

MatisseAnalyzer::MatisseAnalyzer(int argc, char** argv):
m_CommandLine(argc, argv),  // retrieve information from command line
m_Parameters(),             // set default parameters
m_TreeHandler(nullptr),
m_RootFile(nullptr)
{

    gROOT->SetStyle("Plain");
    gStyle->SetPalette();
    gStyle->SetOptFit(0011);        // prints only errors and name/values of parameters
    gStyle->SetOptStat("nem");      // prints histogram name, number of entries and mean

    // reserve memory for 3 different "analysis tasks": seeds, clusters and noise analysis
    m_vAnalysisObj.reserve(3);

    // save info for analysis
    m_AnalysisInfo = m_CommandLine.GetInfoForAnalysis();

    std::cout << "Starting Matisse analysis for file: " << m_CommandLine.getFileNameRAW() << std::endl;
    std::cout << std::endl;

    DBPRINT("Number of key words found for the analysis = ", m_AnalysisInfo.size());
    #if VERBOSE
        std::cout << "Key words found are :\n";
        for(auto i : m_AnalysisInfo)
            std::cout << i << std::endl;
    #endif

    std::cout << "List of information for the analysis :\n";
    for(auto i : m_AnalysisInfo)
        std::cout << i << "\n";
    std::cout << std::endl;

    // set parameters from file name
    NameParser parser( m_CommandLine.getFileNameRAW() );
    parser.SetParameters();
    // save parameters
    m_Parameters = parser.GetParameters();

    DBPRINT("Parameters retrieved are : ", &m_Parameters);
    #if VERBOSE
        m_Parameters.Print();
    #endif

    std::cout << std::endl;
    std::cout << "Run parameters:\n";
    m_Parameters.Print();

    // set the chains and read the trees to get data
    m_TreeHandler = new TreeHandler( m_CommandLine.getFileNameRAW() );

}


MatisseAnalyzer::~MatisseAnalyzer(){

    for(auto& obj : m_vAnalysisObj)
        delete obj;

    m_AnalysisInfo.clear();
    m_vAnalysisObj.clear();

    delete m_TreeHandler;
}

// crates analysis objects depending on the information retrieved in the command line
void MatisseAnalyzer::beginAnalysis(){

    // loop over words passed in command line
    for(auto word : m_AnalysisInfo){
        // words that begin with "--" are interpreted as options for
        // each specific analysis task.
        if( word.find("--") != std::string::npos ) m_vAnalysisObj.back()->SetPlotSettings(word);
        // seed analysis
        else if(word == "seed"){
            m_vAnalysisObj.emplace_back( new Seed() );
        }

        else if(word == "cluster"){
            m_vAnalysisObj.emplace_back( new Cluster() );
        }

        else if(word == "noise"){
            m_vAnalysisObj.emplace_back( new Noise() );
        }

        else if(word == "all"){
            m_vAnalysisObj.emplace_back( new Seed() );
            m_vAnalysisObj.emplace_back( new Cluster() );
            m_vAnalysisObj.emplace_back( new Noise() );
        }

        else
            continue;
    }

    for(auto& obj : m_vAnalysisObj)
        obj->Begin(m_CommandLine);

    return;
}

// calculations for the analysis, fill histogram, fit etc
void MatisseAnalyzer::analyze(){

    unsigned int seed_thr;

    // default value for seed threshold = 6
    seed_thr = (m_CommandLine.contains("seedThr")) ? (m_CommandLine.getValue("seedThr")) : 6 ;

    TreeData<int>* dataInt = TreeData<int>::GetInstance();
    TreeData<float>* dataFloat = TreeData<float>::GetInstance();

    // loop over the entries, one entry per frame
    while(m_TreeHandler->GetReader()->Next()){

        // get the total number of clusters for this frame
        unsigned int Nclusters = dataFloat->Get("clusterx")->size();

        // loop over all the clusters
        for(unsigned int iClz=0; iClz < Nclusters; ++iClz){

            // reset
            Clz::Mult = 0;
            Clz::Height = 0;
            Clz::MatrixHeight = 0;

            // get sector number
            unsigned int iSec = (unsigned int)( dataInt->Get("col_seed")->at(iClz) / 6 );

            // cut seeds on first row
            if( dataInt->Get("row_seed")->at(iClz) == 1 ) continue;

            unsigned int noiseIndex = Clz::MatrixSize*iClz + (Clz::MatrixSize-1)/2;

            float max_val = dataFloat->Get("clusterh")->at(iClz);
            float noise_val = dataFloat->Get("noise_nxn")->at(noiseIndex);

            if( max_val/noise_val > seed_thr ){
                // select seeds only in the 2 central columns of each sector
                // columns 3-4 , 9-10 , 15-16, 21-22
                unsigned int mod = dataInt->Get("col_seed")->at(iClz)%6;
                if( (mod>1) && (mod<4) ){
                    //std::cout << "Loop over analysis objects :\n";
                    for(auto& obj : m_vAnalysisObj){
                        // analyze iClz cluster for iSec sector 
                        obj->Analyze(iClz, iSec);
                    }

                }

                // if value/noise > seed_thr we have a cluster (of one pixel at least)
                ++Clz::Count;
            }

        }
    }
    
    return;
}

// TTree, TFile saving and plot
void MatisseAnalyzer::endAnalysis(){

    // Saving routine

    // Get the path for the data file
    std::string path = GetPath(m_CommandLine.getFileNameRAW());
    path += "Analysis";

    // Create the directory path/Analysis if it doesn't exist
    system( ("mkdir -p " + path).c_str() );

    std::string fileName_out;
    std::string fileName = "output";        // DA MODIFICARE 

    if(m_Parameters.goodness)
        fileName_out = path + "/" + fileName + ".root";

    m_RootFile = new TFile(fileName_out.c_str(), "RECREATE");

    std::cout << "Saving file " << fileName_out << " ... " << std::endl;

    // write all the canvases and the histograms
    for(auto& obj : m_vAnalysisObj){

        auto& histoList = obj->GetListOfHistograms();
        for(auto& hist : histoList)
            m_RootFile->WriteTObject(hist);

    }

    for(auto& obj : m_vAnalysisObj)
        obj->End();

    m_RootFile->Close();

    std::cout << "File " << fileName_out << " saved.\n";

    return;
}

std::string MatisseAnalyzer::GetPath(std::string fileName){
    size_t pos = fileName.find_last_of('/');
    fileName.erase(fileName.begin()+pos+1, fileName.end());
    return fileName;
}

void MatisseAnalyzer::runApplication(TApplication& app){
    app.Run();
    std::cout << app.IsRunning() << std::endl;
    // while application is running retrieves inputs from std::cin
    while(app.IsRunning()){
        std::cout << "I'm running!\n";
    }

    return;
}