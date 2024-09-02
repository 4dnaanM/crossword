#include "grid.cpp"
#include <fstream>
int main(){
    std::vector<std::vector<std::string>> grids; 
    std::vector<std::string> gridStrs;
    std::ifstream file("grids.txt");
    std::string line;
    while (std::getline(file, line)) {
        if(line.size()>4)gridStrs.push_back(line.substr(4,line.size()-4));
        else{
            grids.push_back(gridStrs);
            gridStrs.clear();
        }
    }
    // std::vector<std::string> ws{"DENNIS","BARD","ATTIC","NOTES","ERR","MURDER","UNCLOG","PHI","SMART","NEIGH","DUKE","SLIPUP","DRAWER","NUT","INCOMPLETE","ANTWERP","DOSS","UNORIGINAL","RANSACK","BISHOP","USED","IMP"};
    // std::vector<std::string> ws{"LANDSLIDE","DENNIS","BARD","ATTIC","NOTES","ERR","MURDER","UNCLOG","PHI","SMART","NEIGH","DUKE","SLIPUP","DRAWER","NUT","INCOMPLETE","ANTWERP","DOSS","UNORIGINAL","RANSACK","BISHOP","USED","IMP","KISSED","SEWERS","AMUSED","COLLABORATED","PROMPT","RIOJA","POINTERS","MINT","SURVEY","INKLINGS","DANGLE","SELECTION","SUPPOSED","FISHANDCHIPS","DISSATISFACTION","EXPRESSION","HINGE","ZEAL","INSPIRED","DWELLS","DISTRIBUTE","NOON","QUINCE","MINI","IDEALIST","STANDOFF"};
    std::vector<std::string> ws;
    std::ifstream dictfile("dictionaries/wordsIwant");
    std::string dictline;
    while (std::getline(dictfile, dictline)) {
        ws.push_back(dictline);
    }
    for(std::vector<std::string> gridStr: grids){
        GridObject grid(gridStr);
        grid.fitWords(ws);
    }
    

}