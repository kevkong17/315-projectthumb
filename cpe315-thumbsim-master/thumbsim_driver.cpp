#include "thumbsim.hpp"
#include <iostream>

template<>
void Memory<Data8, Data32>::write(const unsigned int addr, const Data32 data) {
  // data is in native format
  //   if little-endian, 3210 (MSB=3)
  // memory is in big-endian format (0123, MSB=0)
  int i;
  unsigned int myAddr = addr - base;
  if (size() < myAddr + 4) {
    m.resize(myAddr + 4, 0);
  }
  if (myAddr < lowest) lowest = myAddr;
  if (myAddr > highest) highest = myAddr;
  if (opts.writes) {
    cout << hex << addr << ": " << data << " ( ";
    for (i = 0 ; i < 4 ; i++) {
      cout << static_cast<unsigned int>(data.data_ubyte4(i)) << ' ';
    }
    cout << ")" << endl;
  }
  for (i = 0 ; i < 4 ; i++) {
    m[myAddr+i] = Data8(data.data_ubyte4(i));
  }
}

template<>
void Memory<Data16, Data16>::write(const unsigned int addr, const Data16 data) {
  // data is in native format
  int i;
  unsigned short myAddr = addr - base;
  if (size() < myAddr + 2) {
    m.resize(myAddr + 2, 0);
  }
  if (myAddr < lowest) lowest = myAddr;
  if (myAddr > highest) highest = myAddr;
  if (opts.writes) {
    cout << hex << addr << ": " << data << " ( ";
    for (i = 0 ; i < 2 ; i++) {
      cout << static_cast<unsigned int>(data.data_ubyte2(i)) << ' ';
    }
    cout << ")" << endl;
  }

  m[myAddr] = data;
}

template<>
void Memory<Data32, Data32>::write(const unsigned int addr, const Data32 data) {
  unsigned int myAddr = addr - base;
  // cout << hex << addr << ": " << data << endl;
  m[myAddr] = data;
}

template<>
const Data32 Memory<Data8, Data32>::operator[](const unsigned int addr) const {
  unsigned int myAddr = addr - base;
  return Data32(m[myAddr], m[myAddr+1], m[myAddr+2], m[myAddr+3]);
}

template<>
const Data16 Memory<Data16, Data16>::operator[](const unsigned int addr) const {
  unsigned short myAddr = addr - base;
  return m[myAddr];
}

template<>
const Data32 Memory<Data32, Data32>::operator[](const unsigned int addr) const {
  unsigned int myAddr = addr - base;
  return m[myAddr];
}

template<>
void Memory<Data8, Data32>::dump(DataType dt) const {
  Data32 d(0);
  unsigned int addr;
  for (addr = lowest ; addr <= highest + 3 ; addr++) {
    unsigned int byte = addr & 0x3;
    d.set_data_ubyte4(byte, m[addr]);
    if ((byte == 0x3) && (dt == INSTRUCTIONS)) {
      cout << hex << d << endl;
    } else if ((byte == 0x3) && (dt == DATA) && (d.data_uint() != 0)) {
      cout << hex << addr - 3 + getBase() << ": " << d << endl;
    }
  }
}

template<>
void Memory<Data16, Data16>::dump(DataType dt) const {
  if (dt == DATA) {
    for_each(m.begin(), m.end(), Data16::printD);
  }
}

template<>
void Memory<Data32, Data32>::dump(DataType dt) const {
  if (dt == DATA) {
    for_each(m.begin(), m.end(), Data32::printD);
  }
}

// CPE 315: You must implement and call this function for each 
// memory address (dmem only) accessed by the program. It should return 
// true for a cache hit and false for a cache miss, and on a cache miss, 
// should update the cache tags. The "entries" vector contains the cache
// tags, so if you want to put the tag "t" into cache block "b", then
// evaluate "entries[b] = t;". The locals you have available to help
// you make this decision are "blocksize" (in bytes) and "size" (total
// cache size in blocks). You should also update the "hits" and
// "misses" counters.
bool Cache::access(unsigned int address) {
  // least sig: byte select, index, then tag
  //std::cout << std::dec << "Size is " << size << "\n";
  //cout << "Size is " << size << "\n";
  //cout << "Blocksize is " << blocksize << "\n";
  /*if (size != 256){
     cout << size;
     exit(1);
  }
  */
  //unsigned int entries = size / blocksize;
  unsigned int numbitsentries = (unsigned int)log2((int)size/blocksize);
  unsigned int numbitsbyteselect = (unsigned int)log2(blocksize);
  unsigned int tag = address;
  unsigned int index = address;
  //cout << "Address is  " << address << "\n";
  //std::cout << std::hex << "Address in hex is " << address << "\n";
  
  tag = tag >> (numbitsentries + numbitsbyteselect);
  //tag = tag >> (numbitsentries);
  //cout << "Number of bits for size is " << numbitsentries << "\n";
  //cout << "Number of bits for blocksize is " << numbitsbyteselect << "\n";
  
  //unsigned int numbitsfortag = 32 - (numbitsentries);
  unsigned int numbitsfortag = 32 - (numbitsentries + numbitsbyteselect);
  //std::cout << std::dec << "Number of bits for tag is " << numbitsfortag << "\n";
  index >>= numbitsbyteselect;
  //std::cout << std::dec << "Secondary index is " << index << "\n";
  unsigned int numbitsforeb = 32 - numbitsentries;
  //std::cout << std::dec << "Number of bits for eb " << numbitsforeb << "\n";
  index <<= numbitsforeb;
  index >>= numbitsforeb;

  /*std::cout << std::hex << "Final index is " << index << "\n";
  std::cout << std::hex << "Final tag is " << tag << "\n";
  std::cout << std::hex << "Entries[index]  is " << entries[index] << "\n";
  */
  //cout << "hits is " << hits << "\n";
  /*if(hits == 108){
    exit(1);
  }
  */
  //exit(1);
  if(entries[index] == tag){
     hits++;
     return true;
  } else{
     entries[index] = tag;
     //cout << "Number of hits is " << hits << "\n";
     misses++;
  }
  //exit(1);
  return false;
  /*
  if (entries[index] == tag)
  Caches c;
  for(int i = 0; i < c.size; i++){
    Cache temp = c.caches[i];
    int numOfEntries = (temp.size)/(temp.blocksize);
    double entriesIndexBits;
    entriesIndexBits = log2(numOfEntries);
    // isnt this the tag?
    unsigned int addressTemp = address;
    unsigned int blockindex = address;
    addressTemp >>= entriesIndexBits;
    
    while(entriesIndexBits > 0){
      addressTemp = addressTemp >> 1;
      entriesIndexBits--;
    }
    
    // get index by throwing out the extra bits
    blockindex <<= (32 - entriesIndexBits);
    blockindex >>= (32 - entriesIndexBits);

    // check to see if tag exists in current cache
    // the further the cache, the less blocks, less indices, bigger tag?
    if(temp.entries[blockindex] == addressTemp){
       // exists in cache
       temp.hits++;
       return true;
    } else{
       // update cache to contain this tag
       temp.entries[blockindex] = addressTemp;
       temp.misses++;
    }
  }
  */

/*bool Cache::access(unsigned int address) {
  int tag = 0;
  Caches c;
  for(int i = 0; i < Caches->size; i++){
    Cache temp = [i];
    int numOfEntries = (temp->size)/(temp->blocksize);
    double entriesIndexBits;
    entriesIndexBits = log2(numOfEntries);
    unsigned int addressTemp = address;
    while(entriesIndexBits){
      addressTemp = addressTemp >> 1;
    }
    
  }
*/

}

void Stats::print() {
  cout << dec
       << "Total number of dynamic instructions: " << instrs << endl
       << "Number of Memory Reads: " << numMemReads << endl
       << "Number of Memory Writes: " << numMemWrites << endl
       << "Number of Register Reads: " << numRegReads << endl
       << "Number of Register Writes: " << numRegWrites << endl
       << "Branches: " << endl
       << "  Forward:" << endl
       << "    Taken: " << numForwardBranchesTaken << endl
       << "    Not taken: " << numForwardBranchesNotTaken << endl
       << "  Backward:" << endl
       << "    Taken: " << numBackwardBranchesTaken << endl
       << "    Not taken: " << numBackwardBranchesNotTaken << endl;
}

unsigned int swizzle(unsigned int d) {
  return (((d >> 24) & 0xff) |
          ((d >> 8)  & 0xff00) |
          ((d << 8)  & 0xff0000) |
          ((d << 24) & 0xff000000));
}
