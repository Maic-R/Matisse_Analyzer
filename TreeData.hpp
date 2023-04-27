#include "TreeData.h"
#include "Debug.h"

#include <unordered_map>
#include <string>
#include <iostream>


template<typename T>
TreeData<T>* TreeData<T>::treeData = nullptr;

template<typename T>
TreeData<T>* TreeData<T>::GetInstance(){

    DBPRINT("Instance of TreeData : ", treeData);

    if(treeData == nullptr){
        treeData = new TreeData<T>;
        return treeData;
    }
        
    else return treeData;
}

// returns the values stored in a TBranch given the branch name
template<typename T>
const std::vector<T>* TreeData<T>::Get(const std::string& branchName){
   return data.at( branchName ).Get();
}

// add values stored in a branch to data map
template<typename T>
void TreeData<T>::Add(const TTreeReaderValue<std::vector<T>>& reader){
    std::string branchName = reader.GetBranchName();
    data.insert( std::make_pair(branchName, reader) );
    DBPRINT("Name of TreeReader added : ", branchName);
    return;
}

template<typename T>
void TreeData<T>::PrintMap(){
    for(auto it=data.begin(); it!=data.end(); ++it){
        std::cout << "Branch name : " << it->first << std::endl;
        std::cout << "Tree Reader : " << &(it->second) << std::endl;
        std::cout << "Vector size = " << it->second->size() << std::endl;
        std::cout << std::endl;
    }
    return;
}