#include <vector> 
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <iostream>
#include <regex>
#include "word.cpp"
#include <assert.h>

#define set_iter std::__tree_const_iterator<std::string, std::__tree_node<std::string, void *> *, long>
class GridObject {
    int n; 
    std::vector<std::vector<char>> grid; 
    std::vector<std::vector<std::vector<char>>> maxGrids; 
    std::vector<std::vector<WordSlot*>> slots;
    std::unordered_map<int,WordSlot*> acrossSlots; 
    std::unordered_map<int,WordSlot*> downSlots;
    int minWordsLeft; 
    std::set<std::string> allWords;

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

                    // std::cout<<"Intersection at "<<r<<","<<c<<" between clues "<<acrossSlot->getSlotStr()<<" and "<<downSlot->getSlotStr()<<"\n";
                }
            }
        }
    }

    void insertWordInSlot(std::string word, WordSlot* slot){
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
        slot->updateState(state);
    }

    // set iterator type: std::__tree_const_iterator<std::string, std::__tree_node<std::string, void *> *, long>
    void backTrack(int wordsLeft, set_iter begin_it){
        if(minWordsLeft==0)return;
        for(auto it = begin_it; it != allWords.end();it++){
            std::string word = *it;
            for(WordSlot* slot: slots[word.length()]){
                if(slot->fitsWord(word)){
                    std::string oldState = slot->getState();
                    insertWordInSlot(word,slot);
                    if(wordsLeft-1< minWordsLeft){
                        this->maxGrids.clear();
                    }
                    if(wordsLeft-1 <= minWordsLeft){
                        printGrid();
                        std::cout<<"\n";
                        minWordsLeft = wordsLeft-1;
                        this->maxGrids.push_back(this->grid);
                    }
                    backTrack(wordsLeft-1,std::next(it));
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

    void printGrid(){
        int a = this->n;
        for(int r = 0; r<a; r++){
            for(int c = 0; c<a; c++){
                std::cout<<(this->grid[r][c]);
            }
            std::cout<<std::endl;
        }
    }
    void printMaxGrids(){
        int a = this->n;
        for(std::vector<std::vector<char>> maxGrid: maxGrids){
            for(int r = 0; r<a; r++){
                for(int c = 0; c<a; c++){
                    std::cout<<(maxGrid[r][c]);
                }
                std::cout<<std::endl; 
            }
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
        for(std::pair<int, WordSlot*> p : acrossSlots){
            std::cout<<p.second->getSlotStr()<<": "<<p.second->getState()<<"\n";
        }
        for(std::pair<int, WordSlot*> p : downSlots){
            std::cout<<p.second->getSlotStr()<<": "<<p.second->getState()<<"\n";
        }
    }
    
    void fitWords(std::vector<std::string> words){
        for(std::string word: words){
            if(slots.size()>word.size()&&slots[word.size()].size()>0)this->allWords.insert(word);
        }
        this->minWordsLeft = std::min(allWords.size(),acrossSlots.size() + downSlots.size());
        // this->minWordsLeft = std::min(allWords.size(),acrossSlots.size() + downSlots.size());
        backTrack(this->minWordsLeft,allWords.begin());
        printMaxGrids();
    }
};
// later: dont see all words, only the right size
int main(){
    std::vector<std::string> gridStrs{
        "      #    ",
        " # # # # # ",
        "     #     ",
        " ### # # # ",
        "   #      #",
        " # # # # # ",
        "#      #   ",
        " # # # ### ",
        "     #     ",
        " # # # # # ",
        "    #      "
    };
    // std::vector<std::string> gridStrs{
    //     "      # #      ",
    //     "# # # # # # # #",
    //     "        #      ",
    //     "# # # # # # # #",
    //     "###            ",
    //     "# # # # # ### #",
    //     "    ###        ",
    //     "# # # # # # # #",
    //     "        ###    ",
    //     "# ### # # # # #",
    //     "            ###",
    //     "# # # # # # # #",
    //     "      #        ",
    //     "# # # # # # # #",
    //     "      # #      "
    // };
    // std::vector<std::string> gridStrs{
    //     "# #",
    //     "   ",
    //     "# #"
    //     };
    // std::vector<std::string> ws{"DENNIS","BARD","ATTIC","NOTES","ERR","MURDER","UNCLOG","PHI","SMART","NEIGH","DUKE","SLIPUP","DRAWER","NUT","INCOMPLETE","ANTWERP","DOSS","UNORIGINAL","RANSACK","BISHOP","USED","IMP"};
    std::vector<std::string> ws{"AMUSED","SURVEY","INKLINGS","PROMPT","COLLABORATED","MINI","IDEALIST","STANDOFF","NOON","FISHANDCHIPS","QUINCE","INSPIRED","SEWERS","DWELLS","MINT","SELECTION","DANGLE","DISSATISFACTION","SUPPOSED","EXPRESSION","DISTRIBUTE","POINTERS","KISSED", "HINGE","ZEAL"};
    // std::vector<std::string> ws{"ABA", "DBD"};
    GridObject grid(gridStrs);
    grid.fitWords(ws);
    // grid.printGrid();
    // grid.printSlots();
    // grid.insertWord("ACROSS", "1A");
    // grid.printGrid(); 
    // grid.printSlots();
    // grid.insertWord("ANTLER", "1D");
    // grid.printGrid();
    // grid.printSlots();
    // grid.insertWord("CLUE", "16D");
    // grid.printGrid();
    // grid.printSlots();

}
// DENNIS BARD ATTIC NOTES ERR MURDER UNCLOG PHI SMART NEIGH DUKE SLIPUP DRAWER NUT INCOMPLETE ANTWERP DOSS UNORIGINAL RANSACK BISHOP USED IMP
_ _ _ _ _ I N S _ 