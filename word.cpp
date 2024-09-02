#include <string>
#include <unordered_map>
#include <regex>
#include <assert.h>
class WordSlot{ 
    // clue 1D with length 5 starting at grid index 3 means slotNum = 1, length = 5, startIdx = 3, isAcross = false
    int slotNum;  
    int startIdx; 
    std::string state; 
    std::unordered_map<WordSlot*,int> intersectingWords; 
public: 
    bool isAcross;
    WordSlot(int slotNum, int length, int startIdx, bool isAcross){
        this->slotNum = slotNum;
        this->isAcross = isAcross;
        this->state = std::string(length, '.');
        this->startIdx = startIdx;
        this->intersectingWords = std::unordered_map<WordSlot*,int>();
    }
    int getSlotNum(){ 
        return slotNum; 
    }
    std::string getSlotStr(){
        std::string slotStr = std::to_string(slotNum);
        slotStr += isAcross?"A":"D";
        return slotStr;
    }
    int getLength(){ 
        return state.length(); 
    }
    int getStartIdx(){ 
        return startIdx; 
    }
    std::string getState(){ 
        return state; 
    }
    char getLetter(int idxA){ 
        return state[idxA]; 
    }
    void putLetter(int idxA, char letterA){ 
        state[idxA] = letterA; 
    }
    void addIntersectingWord(WordSlot* wordB, int idxA){ 
        intersectingWords[wordB] = idxA; 
    }
    bool fitsWord(std::string word){
        return std::regex_match(word,std::regex("("+state+")"));
    }
    void updateState(std::string newWord){
        assert (this->fitsWord(state));
        this->state = newWord;
        for(std::pair<WordSlot*, int> wordidxpair : this->intersectingWords){
            WordSlot* otherWord = wordidxpair.first;
            int thisIdx = wordidxpair.second; 
            int otherIdx = otherWord->intersectingWords[this];
            otherWord->putLetter(otherIdx, newWord[thisIdx]);
        }
    }
};