#include "Parameters.h"
#include "Debug.h"

NameParser::NameParser(std::string fileName_RAW){

    DBPRINT("Name Parser called with argument : ", fileName_RAW);

    // retrieve file name starting from the complete path by erasing it
    size_t name_position = fileName_RAW.find_last_of('/');
    fileName_RAW.erase(fileName_RAW.begin(), fileName_RAW.begin() + name_position + 1);

    // save file name with path erased
    m_FileName = fileName_RAW;

    DBPRINT("File name with path erased is : ", m_FileName);

    #if VERBOSE
        DBPRINT("Parameters set to default value : ", &m_Parameters);
        m_Parameters.Print();
    #endif

}

NameParser::~NameParser(){
    m_Token.clear();
}

void NameParser::ParseFileName(){

    // retrieving run parameters from file name, using a delimiter between words
    size_t starting_pos = 0;
    size_t delimiter_pos = 0;
    while( ( delimiter_pos = m_FileName.find(m_Delimiter, starting_pos) ) != std::string::npos ){
        m_Token.emplace_back( m_FileName.substr(starting_pos, delimiter_pos - starting_pos) );
        starting_pos = delimiter_pos + 1;
    }

    // add the last word
    size_t last_delimiter = m_FileName.find_last_of(m_Delimiter);
    m_Token.emplace_back( m_FileName.substr(last_delimiter + 1) );

    DBPRINT("Number of words found = ", m_Token.size());
    #if VERBOSE
        std::cout << "Words are: \n";
        for(auto word : m_Token)
            std::cout << word << std::endl;
    #endif

    return;
}

void NameParser::SetParameters(){

    NameParser::ParseFileName();

    // check that the last word is "RAW.root"
    if( m_Token.back() != "RAW.root" ){
        std::cerr << "Last word is not RAW.root!\n";
        std::cerr << "Returning a default parameters set\n";
        return;
    }

    size_t id_pos = 0;

    for(auto word = m_Token.begin(); word < m_Token.end(); ++word){
        if( (id_pos = word->find( (char)IDCHAR::CHIP ) ) != std::string::npos ){
            word->erase(id_pos, id_pos + 1);
            m_Parameters.chipID = std::stoi(*word);
            id_pos = 0;
        }
        else if( ( id_pos = word->find( (char)IDCHAR::SOURCE ) ) != std::string::npos ){
            m_Parameters.radioactive_source = *word;
            id_pos = 0;
        }
        else if( ( id_pos = word->find( (char)IDCHAR::BIAS ) ) != std::string::npos ){
            word->erase(id_pos, id_pos + 1);
            m_Parameters.bias_voltage = std::stoi(*word);
            id_pos = 0;
        }
        else if( word->find( (char)IDCHAR::ENDFILE ) != std::string::npos )
            continue;
        else
            continue;
               
    }

    // parameters goodness is always set to true, even if not all
    // the paramters are given correctly
    m_Parameters.goodness = true;

    return;
}
