#include <vector> 
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <iostream>
#include <regex>
#include "word.cpp"
#include <assert.h>

template <typename T> 
struct CustomComparator{
    const bool operator()(T a, T b) const {
        if(a.size()!=b.size()){
            return a.size()>b.size();
        }
        return std::less<T>()(a,b);
    }
};

#define set_iter std::__tree_const_iterator<std::string, std::__tree_node<std::string, void *> *, long>
class GridObject {
    int n; 
    std::vector<std::vector<char>> grid; 
    std::vector<std::vector<std::vector<char>>> maxGrids; 
    std::vector<std::vector<std::vector<char>>> maxIntersectGrids; 
    std::vector<std::vector<WordSlot*>> slots;
    std::unordered_map<int,WordSlot*> acrossSlots; 
    std::unordered_map<int,WordSlot*> downSlots;
    int minWordsLeft; 
    int maxIntersects; 
    std::set<std::string,CustomComparator<std::string>> allWords;

    std::vector<std::vector<std::pair<int,int>>> findSlots(){
        std::vector<std::vector<std::pair<int,int>>> clueMarkers(this->n, std::vector<std::pair<int,int>>(this->n, std::pair<int,int>(0,0))); 
        int Alength, Blength = 0; 
        int cluenum = 0; 
        for(int r = 0; r<this->n; r++){
            for(int c = 0; c<this->n; c++){
                if(this->grid[r][c] == '#') continue;
                Alength = 0;
                Blength = 0; 
                if(clueMarkers[r][c].first == 0){
                    //look across
                    while(c+Alength < this->n && this->grid[r][c+Alength] != '#'){
                        Alength++;
                    }
                }
                if(clueMarkers[r][c].second == 0){
                    // look down
                    while(r+Blength < this->n && this->grid[r+Blength][c] != '#'){
                        Blength++;
                    }
                }
                if(Alength > 1 || Blength > 1)cluenum++;
                if(Alength>1){
                    for(int i = 0; i<Alength; i++){
                        clueMarkers[r][c+i].first = cluenum;
                    }
                    acrossSlots[cluenum] = new WordSlot(cluenum, Alength, r*this->n+c, true);
                    slots[Alength].push_back(acrossSlots[cluenum]);
                }
                if(Blength>1){
                    for(int i = 0; i<Blength; i++){
                        clueMarkers[r+i][c].second = cluenum;
                    }
                    downSlots[cluenum] = new WordSlot(cluenum, Blength, r*this->n+c, false);
                    slots[Blength].push_back(downSlots[cluenum]);
                }
            }
        }
        return clueMarkers;
    }

    void findIntersections(std::vector<std::vector<std::pair<int,int>>>& clueMarkers){
        for(int r = 0; r < this->n; r++){
            for(int c = 0; c < this->n; c++){
                if(clueMarkers[r][c].first != 0 && clueMarkers[r][c].second != 0){
                    WordSlot* acrossSlot = acrossSlots[clueMarkers[r][c].first]; 
                    WordSlot* downSlot = downSlots[clueMarkers[r][c].second];
                    int acrossIntersection = c-acrossSlot->getStartIdx()%this->n;
                    int downIntersection = r-downSlot->getStartIdx()/this->n;

                    acrossSlot->addIntersectingWord(downSlot,acrossIntersection);
                    downSlot->addIntersectingWord(acrossSlot,downIntersection);
                }
            }
        }
    }

    int insertWordInSlot(std::string word, WordSlot* slot){
        assert(word.length() == slot->getLength());
        assert(slot->fitsWord(word));
        int startIdx = slot->getStartIdx();
        int startRow = startIdx/this->n;
        int startCol = startIdx%this->n;
        int dx = slot->isAcross?1:0;
        int dy = slot->isAcross?0:1;
        for(int i = 0; i < word.length(); i++){
            grid[startRow+i*dy][startCol+i*dx] = word[i];
        }
        int intersects = slot->updateState(word);
        return intersects;
    }

    void replaceStateOfSlot(std::string state, WordSlot* slot){
        assert(state.length() == slot->getLength());
        int startIdx = slot->getStartIdx();
        int startRow = startIdx/this->n;
        int startCol = startIdx%this->n;
        int dx = slot->isAcross?1:0;
        int dy = slot->isAcross?0:1;
        for(int i = 0; i < state.length(); i++){
            grid[startRow+i*dy][startCol+i*dx] = state[i]=='.'?' ':state[i];
        }
        slot->replaceState(state);
    }

    // set iterator type: std::__tree_const_iterator<std::string, std::__tree_node<std::string, void *> *, long>
    void backTrack(int wordsLeft, set_iter begin_it, int intersects){
        if(wordsLeft==0){
            printGrid(this->grid);
            std::cout<<"\n";
        }
        for(auto it = begin_it; it != allWords.end();it++){
            std::string word = *it;
            for(WordSlot* slot: slots[word.length()]){
                if(slot->fitsWord(word)){
                    std::string oldState = slot->getState();
                    int newIntersects = insertWordInSlot(word,slot);
                    if(maxIntersects < intersects+newIntersects){
                        this->maxIntersects = intersects+newIntersects;
                        // std::cout<<this->maxIntersects<<"Intersects\n";
                        this->maxIntersectGrids.clear();
                    }
                    if(maxIntersects <= intersects+newIntersects){
                        this->maxIntersectGrids.push_back(this->grid);
                    }
                    if(wordsLeft-1< this->minWordsLeft){
                        this->minWordsLeft = wordsLeft-1;
                        this->maxGrids.clear();
                        // std::cout<<this->minWordsLeft<<" Words left to fill\n";
                    }
                    if(wordsLeft-1 <= this->minWordsLeft){
                        // printGrid(this->grid);
                        // std::cout<<"\n";
                        this->maxGrids.push_back(this->grid);
                    }
                    backTrack(wordsLeft-1,std::next(it),intersects+newIntersects);
                    replaceStateOfSlot(oldState,slot);
                }
            }
        }
    }

public:

    GridObject(std::vector<std::string>& gridStrs){
        assert (gridStrs.size() == gridStrs[0].size());
        this->n = gridStrs.size();
        this->grid = std::vector<std::vector<char>>(n, std::vector<char>(n));
        this->slots = std::vector<std::vector<WordSlot*>>(n+1,std::vector<WordSlot*>());
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                grid[i][j] = gridStrs[i][j]=='#'?'#':' ';
            }
        }
        std::vector<std::vector<std::pair<int,int>>> clueMarkers = findSlots();
        findIntersections(clueMarkers);
    }

    void printGrid(std::vector<std::vector<char>> grid){
        int a = grid.size();
        for(int r = 0; r<a; r++){
            for(int c = 0; c<a; c++){
                std::cout<<(grid[r][c]);
            }
            std::cout<<std::endl;
        }
    }
    void printMaxGrids(){
        int a = this->n;
        std::cout<<"Max Fills: "<<maxIntersects<<"\n";
        for(std::vector<std::vector<char>> maxGrid: maxGrids){
            printGrid(maxGrid);
            std::cout<<"\n";
        }
        std::cout<<"Max Intersects: "<<maxIntersects<<"\n";
        for(std::vector<std::vector<char>> maxGrid: maxIntersectGrids){
            printGrid(maxGrid);
            std::cout<<"\n";
        }
    }

    void insertWord(std::string word, std::string slotStr){
        char AorD = (char)slotStr[slotStr.length()-1];
        slotStr.pop_back();
        std::unordered_map<int,WordSlot*> slotList = (AorD=='A')?acrossSlots:downSlots;
        assert(slotList.find(stoi(slotStr)) != slotList.end());
        WordSlot* slot = slotList[stoi(slotStr)];
        assert(word.length() == slot->getLength());
        assert(slot->fitsWord(word));
        int startIdx = slot->getStartIdx();
        int startRow = startIdx/this->n;
        int startCol = startIdx%this->n;
        int dx = slot->isAcross?1:0;
        int dy = slot->isAcross?0:1;
        for(int i = 0; i < word.length(); i++){
            grid[startRow+i*dy][startCol+i*dx] = word[i];
        }
        slot->updateState(word);
    }

    void printSlots(){
        std::cout<<"Across Slots: ";  
        std::map<int,int> clueLengths; 
        for(std::pair<int, WordSlot*> p : acrossSlots){
            std::cout<<p.second->getSlotStr()<<": "<<p.second->getLength()<<", ";
            clueLengths[p.second->getLength()]++;
        }
        std::cout<<"\n";
        std::cout<<"Down Slots: ";
        for(std::pair<int, WordSlot*> p : downSlots){
            std::cout<<p.second->getSlotStr()<<": "<<p.second->getLength()<<", ";
            clueLengths[p.second->getLength()]++;
        }
        std::cout<<"\n";
        std::cout<<"Clue Lengths: ";
        for(std::pair<int,int> p : clueLengths){
            std::cout<<p.first<<": "<<p.second<<", ";
        }
    }
    
    void fitWords(std::vector<std::string> words){
        for(std::string word: words){
            if(slots.size()>word.size()&&slots[word.size()].size()>0)this->allWords.insert(word);
        }
        this->minWordsLeft = std::min(allWords.size(),acrossSlots.size() + downSlots.size());

        backTrack(this->minWordsLeft,allWords.begin(),0);
        printMaxGrids();
    }
};