#include "MagicBitboard.hpp"

#include <iostream>
#include <math.h> 
#include <fstream>
#include <string>
#include <vector>

using Bitboard = std::bitset<64>;
typedef unsigned long long uint64; 
Bitboard notAFile = 0xfefefefefefefefe;
Bitboard notHFile = 0x7f7f7f7f7f7f7f7f;
Bitboard notABFile = 0xfcfcfcfcfcfcfcfc;
Bitboard notGHFile = 0x3f3f3f3f3f3f3f3f;
Bitboard RANK_1 = 0xff;
Bitboard RANK_8 = 0xff00000000000000;


uint64 precalculated_magics_bishop[64] = {
    0x2027002422c0002, 0x2011090200044040, 0x81c18401884000, 
    0x881a10044080, 0x8405010821810100, 0x10302204000084b, 0x8000040100400230, 
    0x310402806880100, 0x8401208242010810, 0x10801804002, 0x40200a40c0081400,
    0x6000020081042008, 0x1a081800010, 0x10040904088a840, 0x2a00880848014020, 
    0x8000000201088080, 0x14000c201820094, 0x2808100421482, 0x2020810408402500,
    0x82100420043500, 0x900400180420202, 0x4000100202002000, 0x882c001001008a0, 
    0x1008480a80080,0x30021048190044, 0x2200208010042882, 0x210820080401208, 
    0x410208001400c098, 0x801040006002100, 0x2032006002005050, 0x420048518a060, 0x9000900044018b4, 0x80200150283010, 0x2158800040a00, 0x4084010124080, 0x2040200800010050, 0x128020400001100, 0x2084411008001000, 0x809040040400, 0x40088e040010400, 0x3a00098090000400, 0x200805000800200, 0x1402208022d00080, 0x380060a010481, 0x4000400802000030, 0x481410203910, 0xc0300020800c0d00, 0x1864008842000840, 0x82034100004, 0x1886208060080, 0x8900049024214000, 0x8010001204140808, 0x810008089008614, 0x21106002a2002004, 0xa24020090440080, 0x804044081050002, 0x10112802102210, 0x840000222404105,
    0x4000051042001003, 0x4020001042221200, 0x180000800450100,
    0x80002802100c10, 0x1284900040c0, 0x14111000884088
};

uint64 precalculated_magics_rook[64] = {
    0x2080018040002038, 0x420080301601001, 0x4220010020900418, 
    0x408040090001008, 0x420420060040a204, 0x100084081000400, 0x40044000810401,
    0x4300009841000022, 0x4020801100401c0, 0x400200110080020, 0x540100410022090, 
    0x910010890022013, 0x20080023060008, 0xc000400480010620, 0x1200421000500700, 0x680800080002100, 
    0x7088102000201001, 0x5410804000084011, 0x1202000830801c, 0x800b0002d00020, 0x20010092044, 
    0xa00200400111041, 0x804022000469003,0x1028a003810040, 0x8780108100110020, 
    0x4021010002404, 0x300802104042000, 0x30200901008802, 0x18001404043100, 
    0x4040488820200400, 0x80080080c100060, 0x40a80008004c100, 0x80008410200160, 0x2208015060020410, 0x402920001a080040, 
    0x2000020206100800, 0x14010240882004, 0x1008120502400490, 0xe000010a04008410, 0x2140084281000222, 0x800640050404000, 
    0x10884000080480, 0x8044005908016800, 0x1008001000120a0a, 0x10440080040800, 0x20070200404002, 
    0x1100400080201010, 0x810188c00a20001, 0x8004100019610420,
    0x3400084240200020, 0x800801084084011, 0x310500004080080, 0x4402010802341240, 
    0x8108821840, 0x48820018800102c0, 0x88c408042802040, 0x1508001044021, 
    0x24000809040a029, 0x3200820010c0041a, 0x1080a04840300402, 0xc02040120920006,
    0x1042004082010812, 0x4084020240800b, 0x6c02002040810402,
};
// Create a random bitboard with a few amount of ones  uniform distributed: amount of ones is a value that generates the bitboards faster found by trail and error 
Bitboard randomBitboard(){
    Bitboard b;
    int randamount_of_one = rand()%(12-6 + 1) + 6;
    for (int i = 0; i < randamount_of_one; i++) {
        int randNum = rand()%(64-0 + 1) + 0;
        b[randNum] = 1;
    }
    return b;
}
MagicBitboard::MagicBitboard(){
    bool skip_construction = false;
    for (int i = 0; i < 64; i++) {
        //std :: cout << "___________________START________________________: " << i << std :: endl;
        Bitboard used[512];
        Bitboard attacks[512];
        Bitboard occupied[512];
        //std::cout << "Finding a magic number for bishop on square "<< i << std::endl;
        std::vector<Bitboard> BpossibleMasks = possibleOccupied(Bmask_square(*(Square::fromIndex(i))));
        Bmask[i] = Bmask_square(*(Square::fromIndex(i)));
        int n = Bitboard(Bmask[i]).count();
        for(int k = 0; k < (1<<n); k++){
            occupied[k] = BpossibleMasks[k];
            attacks[k] = Battack_square(*(Square::fromIndex(i)), occupied[k]);
           
        }
        Bitboard magic = Bitboard(precalculated_magics_bishop[i]);
        for(int z = 0; z < 512; z++) used[z] = 0ULL;
        int depth = 0;
        while ( depth < (1<<n)) {
            int j = (int)((occupied[depth].to_ullong()  * (magic.to_ullong())) >> (64 - 9));
            if (used[j] == 0ULL) {
                used[j] = attacks[depth];
                depth++;
            } 
            else{
                regenerateMagicBitboard();
                skip_construction = true;
                break;
            }
        }
       
       if (skip_construction){
              break;
       }
       Bmagics[i] = magic;
       for (int j = 0; j < 512; j++) {
            BmoveDB[i][j] = used[j];
        }
        
    }
    for (int i = 0; i < 64; i++) {
        Bitboard used[4096];
        Bitboard attacks[4096];
        Bitboard occupied[4096];
        //std :: cout << "___________________START________________________: " << i << std :: endl;
        //std::cout << "Finding a magic number for rook on square "<< i << std::endl;
        std::vector<Bitboard> RpossibleMasks = possibleOccupied(Rmask_square(*(Square::fromIndex(i))));
        Rmask[i] = Rmask_square(*(Square::fromIndex(i)));
        int n = Bitboard(Rmask[i]).count();
        for(int k = 0; k < (1<<n); k++){
            occupied[k] = RpossibleMasks[k];
            attacks[k] = Rattack_square(*(Square::fromIndex(i)), occupied[k]);
        }
        
        
        Bitboard magic = Bitboard(precalculated_magics_rook[i]);
        for(int z = 0; z < 4096; z++) used[z] = 0ULL;
        int depth = 0;
        while ( depth < (1<<n)) {
            int j = (int)((occupied[depth].to_ullong()  * (magic.to_ullong())) >> (64 - 12));
            if (used[j] == 0ULL) {
                used[j] = attacks[depth];
                depth++;
            } 
            else{
                regenerateMagicBitboard();
                skip_construction = true;
                depth = 0;
                break;
            }
        }
        if (skip_construction){
              break;
        }
        Rmagics[i] = magic;
        for (int j = 0; j < 4096; j++) {
            RmoveDB[i][j] = used[j];
        }
        
    }
    //add knight moves
    for (int i = 0; i < 64; i++) {
        Bitboard dummyknight = 0ULL;
        dummyknight[i] = true;
        Bitboard attacks = (((dummyknight << 17) & notAFile) | ((dummyknight << 15) & notHFile)| ((dummyknight << 10) & notABFile )| ((dummyknight << 6) & notGHFile) | ((dummyknight >> 17) & notHFile) | ((dummyknight >> 15) & notAFile) | ((dummyknight >> 10) & notGHFile) | ((dummyknight >> 6) & notABFile));
        NmoveDB[i] = attacks; 
    }
    //add king moves
    for (int i = 0; i < 64; i++) {
        Bitboard dummyking = 0ULL;
        dummyking[i] = true;
        Bitboard attacks = ((dummyking)<< 8) | ((dummyking)>> 8) | ((dummyking & notHFile) << 1) | ((dummyking & notAFile) >> 1) | ((dummyking & notHFile) << 9) | ((dummyking & notAFile) << 7) | ((dummyking & notHFile) >> 7) | ((dummyking & notAFile) >> 9);
        KmoveDB[i] = attacks;
    }       
}
//function that created the bitboard by generating new magic numbers 
void MagicBitboard::regenerateMagicBitboard(){
    std :: cout << "START GENERATION OF MAGICS" << std :: endl;
    bool go_next= false;
    int amunt_of_one = 0;
    //bishops first generate move set for each square and store in BmoveDB, generate mask for each square and store in Bmask, generate magic for each square and store in Bmagics.
    printf("\nbishops\n");
    for (int i = 0; i < 64; i++) {
        //std :: cout << "___________________START________________________: " << i << std :: endl;
        Bitboard used[512];
        Bitboard attacks[512];
        Bitboard occupied[512];
        //std::cout << "Finding a magic number for bishop on square "<< i << std::endl;
        std::vector<Bitboard> BpossibleMasks = possibleOccupied(Bmask_square(*(Square::fromIndex(i))));
        Bmask[i] = Bmask_square(*(Square::fromIndex(i)));
        int n = Bitboard(Bmask[i]).count();
        for(int k = 0; k < (1<<n); k++){
            occupied[k] = BpossibleMasks[k];
            attacks[k] = Battack_square(*(Square::fromIndex(i)), occupied[k]);
           
        }
        for(int k = 0; k < 100000000; k++) { 
            amunt_of_one ++;
            Bitboard magic = randomBitboard();
            for(int z = 0; z < 512; z++) used[z] = 0ULL;
            int depth = 0;
            while ( depth < (1<<n)) {
                int j = (int)((occupied[depth].to_ullong()  * (magic.to_ullong())) >> (64 - 9));
                if (used[j] == 0ULL) {
                    used[j] = attacks[depth];
                    depth++;
                } 
                else{
                    go_next = true;
                    depth = 0;
                    break;
                }
            }
            if (go_next) {
                go_next = false;
                continue;
            }
            
            printf("%#llx, ", magic.to_ullong());
            Bmagics[i] = magic;
            go_next = false;
            k = 100000000;
            
        }
        for (int j = 0; j < 512; j++) {
            BmoveDB[i][j] = used[j];
        }


        
        amunt_of_one = 0;
        
    }
    
    for (int i = 0; i < 64; i++) {
        Bitboard used[4096];
        Bitboard attacks[4096];
        Bitboard occupied[4096];
        
        std::vector<Bitboard> RpossibleMasks = possibleOccupied(Rmask_square(*(Square::fromIndex(i))));
        Rmask[i] = Rmask_square(*(Square::fromIndex(i)));
        int n = Bitboard(Rmask[i]).count();
        for(int k = 0; k < (1<<n); k++){
            occupied[k] = RpossibleMasks[k];
            attacks[k] = Rattack_square(*(Square::fromIndex(i)), occupied[k]);
        }
        for(int k = 0; k < 100000000; k++) { 
            amunt_of_one ++;
            Bitboard magic = randomBitboard();
            for(int z = 0; z < 4096; z++) used[z] = 0ULL;
            int depth = 0;
            while ( depth < (1<<n)) {
                int j = (int)((occupied[depth].to_ullong()  * (magic.to_ullong())) >> (64 - 12));
                if (used[j] == 0ULL) {
                    used[j] = attacks[depth];
                    depth++;
                } 
                else{
                    //std :: cout << "failed , had a few more to go: " << (1<<n) - ( depth) << std :: endl;
                    go_next = true;
                    depth = 0;
                    break;
                }
            }
            if (go_next) {
                go_next = false;
                continue;
            }
            
            
            printf("%#llx, ",magic.to_ullong());
            go_next = false;
            k = 100000000;
            

        }
        for (int j = 0; j < 4096; j++) {
            RmoveDB[i][j] = used[j];
        }

        //std :: cout << "___________________END________________________"<< i << std :: endl;
        //std :: cout << "___________________TOTAL AMOUNT OF TRIES________________________"<< amunt_of_one << std :: endl;
        amunt_of_one = 0;
        }
std :: cout << "MAGIC BITBOARD FINISHED CONSTRUCTION" << std :: endl;

}
//reading in data takes longer then constructing the magic bitboards on the fly :/ an donvert the data wrong dont use 
void MagicBitboard::read_in_data() {
    std::ifstream t("MAGICS.txt");
    std::string content((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
    std :: cout << "READING IN DATA" << std :: endl;
    std :: cout << "READING IN BmoveDB" << std :: endl; 
    // constuct BmoveDB for file 
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 512; j++) {
            // std :: cout << "READING IN BmoveDB[" << i << "][" << j << "]" << std :: endl;
            std::string substring = "BmoveDB[" + std::to_string(i) + "]["+  std::to_string(j)+"] = ";
            std::string line = content.substr(content.find(substring)+ substring.length() , content.find(";\n"));
            // std :: cout << line << std :: endl;
            BmoveDB[i][j] = Bitboard(std::stoull(line));
        }
    }
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 4096; j++) {
            //std :: cout << "READING IN RmoveDB[" << i << "][" << j << "]" << std :: endl;
            std::string substring = "RmoveDB[" + std::to_string(i) + "]["+  std::to_string(j)+"] = ";
            std::string line = content.substr(content.find(substring)+ substring.length() , content.find(";\n"));
            //std :: cout << line << std :: endl;
            RmoveDB[i][j] = Bitboard(std::stoull(line));
        }
        
    }
    // constuct Bmask for file
    for (int i = 0; i < 64; i++) {
        std::string line = content.substr(content.find("Bmask[" + std::to_string(i) + "] = "), content.find(";\n"));
        std::string line2 = line.substr(line.find("=") + 1, line.find(";\n"));
        Bmask[i] = Bitboard(std::stoull(line2));
    }
    // constuct Rmask for file
    for (int i = 0; i < 64; i++) {
        std::string line = content.substr(content.find("Rmask[" + std::to_string(i) + "] = "), content.find(";\n"));
        std::string line2 = line.substr(line.find("=") + 1, line.find(";\n"));
        Rmask[i] = Bitboard(std::stoull(line2));
    }
    // constuct Bmagics for file
    for (int i = 0; i < 64; i++) {
        std::string line = content.substr(content.find("Bmagics[" + std::to_string(i) + "] = "), content.find(";\n"));
        std::string line2 = line.substr(line.find("=") + 1, line.find(";\n"));
        Bmagics[i] = Bitboard(std::stoull(line2));
    }
    // constuct Rmagics for file
    for (int i = 0; i < 64; i++) {
        std::string line = content.substr(content.find("Rmagics[" + std::to_string(i) + "] = "), content.find(";\n"));
        std::string line2 = line.substr(line.find("=") + 1, line.find(";\n"));
        Rmagics[i] = Bitboard(std::stoull(line2));
    }

}

    




// adds one to only specific bit indeces to generate all possible masks for a square 
Bitboard MagicBitboard::add_1_to_indeces(const Bitboard b,const std::vector<int> indeces,const int index ) const{
    Bitboard result = b;
    //index overload no need to carry over add 
    if ((size_t) index == indeces.size()){
        return result;
    }
    //index is one add needs to be carried over 
    else if (b[indeces[index]] == 1){
        result = add_1_to_indeces(result,indeces,index+1);
    }
    result.flip(indeces[index]);

    return result;
}

std::vector<int> MagicBitboard::get_indeces(const Bitboard b) const{
    std::vector<int> indeces;
    Bitboard copy = b;
    while (copy.to_ullong() != 0) {
        int new_one = copy._Find_first();
        indeces.push_back(new_one);
        copy.flip(new_one);
    }
    return indeces;
}

 

//function that returns a vector of all possible occupied bitboards for a given mask
std::vector<Bitboard> MagicBitboard::possibleOccupied(const Bitboard& mask) const{
    std::vector<Bitboard> possibleOccupied;
    std::vector<int> indices = get_indeces(mask);
    std::bitset<64> result = 0;
    int index = mask.count();
    while (result[index] != 1) {    
        Bitboard added_to_vector = 0;
        for (int i = 0; i < index; i++) {
            if (result[i] == 1) {
                added_to_vector.flip(indices[i]);
            }

        }
        possibleOccupied.push_back(added_to_vector);
        result = result.to_ullong() + 1ULL;
        
    }
    return possibleOccupied;
}


//generate diagonal mask for a given square for the bishop piece 
Bitboard MagicBitboard::Bmask_square(const Square& square) const{
    Bitboard mask;
    int sq = square.index();
    int row = sq/8;
    int col = sq%8;
    int i = 0;
    int j = 0;
    for (i = row + 1, j = col + 1; i <= 6 && j <= 6; i++, j++) {
        mask.flip(i*8 + j);
    }
    for (i = row + 1, j = col - 1; i <= 6 && j >= 1; i++, j--) {
        mask.flip(i*8 + j);
    }
    for (i = row - 1, j = col + 1; i >= 1 && j <= 6; i--, j++) {
        mask.flip(i*8 + j);
    }
    for (i = row - 1, j = col - 1; i >= 1 && j >= 1; i--, j--) {
        mask.flip(i*8 + j);
    }
    return mask;
}
uint64 rmask(int sq) {
  uint64 result = 0ULL;
  int rk = sq/8, fl = sq%8, r, f;
  for(r = rk+1; r <= 6; r++) result |= (1ULL << (fl + r*8));
  for(r = rk-1; r >= 1; r--) result |= (1ULL << (fl + r*8));
  for(f = fl+1; f <= 6; f++) result |= (1ULL << (f + rk*8));
  for(f = fl-1; f >= 1; f--) result |= (1ULL << (f + rk*8));
  return result;
}
Bitboard MagicBitboard::Rmask_square(const Square& square) const{
    Bitboard mask;
    int sq = square.index();
    int row = sq/8;
    int col = sq%8;
    int i = 0;
    int j = 0;
    for (i = row + 1; i <= 6; i++) {
        mask.flip(i*8 + col);
    }
    for (i = row - 1; i >= 1; i--) {
        mask.flip(i*8 + col);
    }
    for (j = col + 1; j <= 6; j++) {
        mask.flip(row*8 + j);
    }
    for (j = col - 1; j >= 1; j--) {
        mask.flip(row*8 + j);
    }
    return mask;

}

Bitboard MagicBitboard::Rattack_square(const Square& square, Bitboard occupied) const{
    Bitboard attack;
    int sq = square.index();
    int row = sq/8;
    int col = sq%8;
    int i = 0;
    int j = 0;
    for(i = row+1; i < 8; i++){
        attack.set(i*8+col);
        if(occupied.test(i*8+col)){
            break;
        }
    }
    i = 0;
    j = 0;
    for(i = row-1; i >= 0; i--){
        attack.set(i*8+col);
        if(occupied.test(i*8+col)){
            break;
        }
    }
    i = 0;
    j = 0;
    for(j = col+1; j < 8; j++){
        attack.set(row*8+j);
        if(occupied.test(row*8+j)){
            break;
        }
    }
    i = 0;
    j = 0;
    for(j = col-1; j >= 0; j--){
        attack.set(row*8+j);
        if(occupied.test(row*8+j)){
            break;
        }
    }
    return attack;
}
//generate bishop attacks for a given square and occupied bitboard
Bitboard MagicBitboard::Battack_square(const Square& square, Bitboard occupied) const{
    Bitboard attack;
    int sq = square.index();
    int row = sq/8;
    int col = sq%8;
    int i = 0;
    int j = 0;
    for (i = row + 1, j = col + 1; i < 8 && j < 8; i++, j++) {
        attack.flip(i*8 + j);
        if(occupied.test(i*8 + j)){
            break;
        }
    }
    i = 0;
    j = 0;
    for (i = row + 1, j = col - 1; i < 8 && j >= 0; i++, j--) {
        attack.flip(i*8 + j);
        if(occupied.test(i*8 + j)){
            break;
        }
    }
    i = 0;
    j = 0;
    for (i = row - 1, j = col + 1; i >= 0 && j < 8; i--, j++) {
        attack.flip(i*8 + j);
        if(occupied.test(i*8 + j)){
            break;
        }
    }
    i = 0;
    j = 0;
    for (i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
        attack.flip(i*8 + j);
        if(occupied.test(i*8 + j)){
            break;
        }
    }
    return attack;

}

//function that prints a bitboard in a 8x8 grid with 1's and 0's starting with the index 0 in the bottem left corner
void MagicBitboard::printBitboard(const Bitboard& b) const{
    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0) {
            std::cout << std::endl;
        }
        std::cout << b[i];
    }
    std::cout << std::endl;
}
Bitboard MagicBitboard:: Bmoves(const Square& square, const Bitboard& Occ ) const{
    auto magic = Bmagics[square.index()];
    int j = (int)((Occ.to_ullong()*(magic.to_ullong())) >> (64 - 9));
    return BmoveDB[square.index()][j];
}
Bitboard MagicBitboard:: Rmoves(const Square& square, const Bitboard& Occ ) const{
    auto magic = Rmagics[square.index()];
    int j = (int)((Occ.to_ullong()  * (magic.to_ullong())) >> (64 - 12));
    return RmoveDB[square.index()][j];
}
Bitboard MagicBitboard:: Nmoves(const Square& square) const{
    return NmoveDB[square.index()];
}
Bitboard MagicBitboard:: Kmoves(const Square& square) const{
    return KmoveDB[square.index()];
}

Bitboard MagicBitboard::get_Bmagic(const Square& square) const{
    return Rmagics[square.index()];
}
Bitboard MagicBitboard::get_Rmagic(const Square& square) const{
    return Rmagics[square.index()];
}

Bitboard MagicBitboard::get_Bmask(const Square& square) const{
    return Bmask[square.index()];
}
Bitboard MagicBitboard::get_Rmask(const Square& square) const{
    return Rmask[square.index()];
}



