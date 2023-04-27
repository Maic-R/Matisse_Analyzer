#ifndef TreeData_h
#define TreeData_h

#include <TTreeReaderValue.h>

#include <vector>

template<typename T>
using Map = std::unordered_map<std::string, TTreeReaderValue<std::vector<T>> >;

template<typename T>
struct TreeData{

    static TreeData<T>* GetInstance();

    ~TreeData(){}

    const std::vector<T>* Get(const std::string& branchName);
    void Add(const TTreeReaderValue<std::vector<T>>& reader);

    void PrintMap();

    private:

    // private constructor : singleton pattern
    TreeData(){}

    // map to store data. Data are stored as pair (branchName, TTreeReaderValue)
    Map<T> data;
    static TreeData<T>* treeData;   // static object instance

};

#include "TreeData.hpp"

#endif