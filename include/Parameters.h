#ifndef Parameters_h
#define Parameters_h

#include <string>
#include <vector>
#include <iostream>

struct Parameters{

    // set everything to deafult values
    Parameters(){
        chipID = -1;
        radioactive_source = "none";
        bias_voltage = -999;
        seed_thr = -9;
        goodness = false;
    }

    int chipID;
    std::string radioactive_source;
    int bias_voltage;
    int seed_thr;
    bool goodness;

    inline void Print() const{
        std::cout << "ChipID : " << chipID << std::endl;
        std::cout << "Radioactive source : " << radioactive_source << std::endl;
        std::cout << "Bias voltage = " << bias_voltage << std::endl;
        std::cout << "Seed threshold = " << seed_thr << std::endl;
        std::cout << "Parameters goodness : " << goodness << std::endl;
        std::cout << "---------------------\n";
        std::cout << std::endl;
        return;
    }
};

// enum class of char to identify key words in the file name 
enum class IDCHAR : char{
    CHIP = 'B',
    SOURCE = '-',
    BIAS = 'V',
    ENDFILE = '.'
};

class NameParser{

    public:

    NameParser(std::string fileName_RAW);
    NameParser(const NameParser&) = delete;
    ~NameParser();

    void SetParameters();

    inline const std::string& GetFileName() const { return m_FileName; };
    inline Parameters& GetParameters() { return m_Parameters; };
    inline const Parameters& GetParameters() const { return m_Parameters; };

    private:

    std::string m_FileName;
    Parameters m_Parameters;
    std::vector<std::string> m_Token;
    const std::string m_Delimiter = "_";

    void ParseFileName();

};

#endif