#include "Cluster.h"
#include "Debug.h"
#include "TreeData.h"

// static variable of Clz struct
unsigned int Clz::Count = 0;
unsigned int Clz::Mult = 0;
unsigned int Clz::Height = 0;
unsigned int Clz::MatrixHeight = 0;

Cluster::Cluster():
AnalysisObject(),
hSingle_pxl_cluster(nullptr), hMultiplicity(nullptr), 
cSingle_pxl_cluster(nullptr), cMultiplicity(nullptr),
hMatrix_sector{nullptr}, hCluster_sector{nullptr},  
hMultiplicity_sector{nullptr}, 
// hRow_pxl_sector{nullptr}, hCol_pxl_sector{nullptr},
cMatrix_sector(nullptr), cCluster_sector(nullptr),  cMultiplicity_sector(nullptr)
// ,cRow_pxl_sector(nullptr), cCol_pxl_sector(nullptr)
{

}

Cluster::~Cluster(){
}

void Cluster::Book(){

    // single pixel clusters
    Save( hSingle_pxl_cluster = new TH1F("hSingle_pxl_cluster", "Single pxl cluster distr", 100, 0, 3000) );
    hSingle_pxl_cluster->GetXaxis()->SetTitle("Signal [ADC]");
    Save( CreateCanvas(cSingle_pxl_cluster, hSingle_pxl_cluster) );

    // cluster multiplicity
    Save( hMultiplicity = new TH1F("hMultiplicity", "Cluster multiplicity", m_Nrows, 0, m_Nrows) );
    hMultiplicity->GetXaxis()->SetTitle("Cluster size [# of pixels]");
    Save( CreateCanvas(cMultiplicity, hMultiplicity) );

    // histogram sum
    Save( hsum = new TH1F("hsum", "histogram sum", 100, 0, 50) );
    hsum->GetXaxis()->SetTitle("Signal [ADC]");
    Save( CreateCanvas(csum, hsum) );

  
    // creation of histograms for sectors
    for(unsigned int i=0; i < m_Nsectors; ++i){

        Save( hCluster_sector[i] = new TH1F ( ("hCluster_sector_" + std::to_string(i)).c_str(),
            ("Cluster signal sector # " + std::to_string(i)).c_str(), 100, 0, 50) );   
        hCluster_sector[i]->GetXaxis()->SetTitle("Signal [ADC]");

        Save( hMatrix_sector[i] = new TH1F ( ("hMatrix_sector_" + std::to_string(i)).c_str(),
            ("Matrix sector # " + std::to_string(i)).c_str(), 100, 0, 3500) );
        hMatrix_sector[i]->GetXaxis()->SetTitle("Signal [ADC]");

        Save( hMultiplicity_sector[i] = new TH1F ( ("hMultiplicity_sector_" + std::to_string(i)).c_str(),
            ("Multiplicity sector # " + std::to_string(i)).c_str(), m_Nrows, 0, m_Nrows) );
        hMultiplicity_sector[i]->GetXaxis()->SetTitle("Number of pixels");

        //Save( hRow_pxl_sector[i] = new TH1F ( ("hRow_pxl_sector_" + std::to_string(i)).c_str(),
        //    ("Cluster size along rows sector # " + std::to_string(i)).c_str(), m_MatrixDim, 0, m_MatrixDim) );
        //hRow_pxl_sector[i]->GetXaxis()->SetTitle("Pixel row");

        //Save( hCol_pxl_sector[i] = new TH1F ( ("hCol_pxl_sector_" + std::to_string(i)).c_str(),
        //    ("Cluster size along cols sector # " + std::to_string(i)).c_str(), m_MatrixDim, 0, m_MatrixDim) );
        //hCol_pxl_sector[i]->GetXaxis()->SetTitle("Pixel column");

    }

    Save( CreateCanvasForSector(cCluster_sector, hCluster_sector) );
    Save( CreateCanvasForSector(cMatrix_sector, hMatrix_sector) );
    Save( CreateCanvasForSector(cMultiplicity_sector, hMultiplicity_sector) );

    return;
}

void Cluster::Begin(const CommandLine& cl){

    std::cout << "Starting cluster analysis ... \n";
    std::cout << std::endl;

    // default value for pixel threshold = 4
    m_Pixel_thr = (cl.contains("pixelThr")) ? (cl.getValue("pixelThr")) : 4;
    DBPRINT("Pixel threshold set to : ", m_Pixel_thr);

    Cluster::Book();

    Cluster::calParam param0(0.0135729,-1.32072e-12,0);
    Cluster::calParam param1(0.0131647,-1.15197e-12   ,1);
    Cluster::calParam param2(0.0135495,-2.55329e-12,2);
    Cluster::calParam param3(0.0116646,1.89448e-12,3);

    calParameters.insert( std::make_pair(0, param0) );
    calParameters.insert( std::make_pair(1, param1) );
    calParameters.insert( std::make_pair(2, param2) );
    calParameters.insert( std::make_pair(3, param3) );

    //calParameters[0] = param0;
    //calParameters[1] = param1;
    //calParameters[2] = param2;
    //calParameters[3] = param3;

    return;
}

void Cluster::Analyze(unsigned int iClz, unsigned int iSec){

    TreeData<float>* dataFloat = TreeData<float>::GetInstance();

    //Clz::Height = 0;

    // loop over the matrix (sub-matrix 5x5)
    for(unsigned int k=0; k < Clz::MatrixSize; ++k){
        // set threshold on neighbor pixels
        unsigned int nieghborIndex = Clz::MatrixSize*iClz + k;
        float pulseHeight = dataFloat->Get("ph_nxn")->at(nieghborIndex);
        float noise = dataFloat->Get("noise_nxn")->at(nieghborIndex);
        if( pulseHeight/noise > m_Pixel_thr ){
            // update cluster mass
            Clz::Height += pulseHeight;
            ++Clz::Mult;
        }

        //std::cout << "Clz height = " << Clz::Height << std::endl;

        // update matrix mass
        Clz::MatrixHeight += pulseHeight;
    }

    // seed height + all overthreshold pixel height
    //hCluster_sector[iSec]->Fill(Clz::Height); 
    
    // fill histogram as sum of histo from sect 0, 1, 2
    //if (iSec != 3) hsum -> Fill(Clz::Height)

    // Calibrated histo (Clz for each sector + hsum)
    CalibrateHisto(hCluster_sector[iSec], hsum , Clz::Height, iSec);

    // sum of all pixel heights in the sub-matrix
    hMatrix_sector[iSec]->Fill(Clz::MatrixHeight);

    // multiplicity distribution: how many pixels are over threshold
    hMultiplicity_sector[iSec]->Fill(Clz::Mult);
    hMultiplicity->Fill(Clz::Mult);

    // cluster with single pixel, i.e. with seed only
    if(Clz::Mult == 1) hSingle_pxl_cluster->Fill(Clz::Height);

    return;
}

void Cluster::End(){

    std::cout << "Cluster analysis completed. Plotting the results ... \n";

    m_Already_analyzed = true;

    for(auto it = calParameters.begin(); it!=calParameters.end(); ++it){
        std::cout << "Sector = " << it->first << std::endl;
        std::cout << "a = " << it->second.a << std::endl;
        std::cout << "b = " << it->second.b << std::endl;
    }





    if(m_PlotAll){
        for(auto it = m_PlotPairs.begin(); it != m_PlotPairs.end(); ++it)
            Cluster::Plot(it->first, it->second);
        for(auto it = m_PlotPairsSectors.begin(); it != m_PlotPairsSectors.end(); ++it)
            Cluster::PlotForSector(it->first, it->second);
    }

    else if(m_PlotSec){
        for(auto it = m_PlotPairsSectors.begin(); it != m_PlotPairsSectors.end(); ++it)
            Cluster::PlotForSector(it->first, it->second);
    }

    else{
        for(auto it = m_PlotPairs.begin(); it != m_PlotPairs.end(); ++it)
            Cluster::Plot(it->first, it->second);
    }

    return;
}

void Cluster::CalibrateHisto(TH1F* hist, TH1F* hist2, unsigned int val, unsigned int sec){
    //hist->SetAxisRange(0, 50);
    
    hist->Fill( val * calParameters.at(sec).b + calParameters.at(sec).a );
    hist->GetXaxis()->SetTitle("Energy [keV]");

    // fill histogram sum of cluster from sector 0, 1, 2 to have more statistic
    // for now sect 3 not useds
    if (sec != 3) hist2 -> Fill(val * calParameters.at(sec).b + calParameters.at(sec).a );
    hist2->GetXaxis()->SetTitle("Energy [keV]");

}

