#pragma once
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>
#include <cstdint>
#include <ostream>
#include <istream>

struct BitWriter {
    std::ostream& os; uint8_t buf=0; int bits=0;
    explicit BitWriter(std::ostream& o):os(o){}
    void putBit(int b){
        buf = (buf<<1)|(b&1);
        if(++bits==8){ os.put(char(buf)); buf=0; bits=0; }
    }
    void putBits(uint32_t v,int n){
        for(int i=n-1;i>=0;--i) putBit((v>>i)&1);
    }
    void flush(){
        if(bits){ buf <<= (8-bits); os.put(char(buf)); bits=0; }
    }
};

struct BitReader {
    std::istream& is; uint8_t buf=0; int bits=0;
    explicit BitReader(std::istream& i):is(i){}
    int getBit(){
        if(bits==0){
            int c = is.get();
            if(c==EOF) return -1;
            buf = uint8_t(c);
            bits = 8;
        }
        int b = (buf>>7)&1;
        buf <<= 1; --bits;
        return b;
    }
};

inline void Huffman_encode(const std::vector<int16_t>& data, BitWriter& bw) {
    // build frequencies
    std::unordered_map<int16_t,int> freq;
    for(auto v:data) ++freq[v];
    struct Node{ int16_t s; int f; Node* l,*r; };
    struct Cmp{ bool operator()(Node* a,Node* b){ return a->f>b->f; } };
    std::priority_queue<Node*,std::vector<Node*>,Cmp> pq;
    std::vector<Node*> nodes;
    for(auto& kv:freq){
        nodes.push_back(new Node{kv.first,kv.second,nullptr,nullptr});
        pq.push(nodes.back());
    }
    while(pq.size()>1){
        Node* a=pq.top(); pq.pop();
        Node* b=pq.top(); pq.pop();
        nodes.push_back(new Node{-1,a->f+b->f,a,b});
        pq.push(nodes.back());
    }
    Node* root = pq.top();
    std::unordered_map<int16_t,std::pair<uint32_t,int>> table;
    std::function<void(Node*,uint32_t,int)> dfs = [&](Node* n,uint32_t code,int len){
        if(!n->l && !n->r){
            table[n->s] = {code,len};
            return;
        }
        dfs(n->l, code<<1, len+1);
        dfs(n->r, code<<1|1, len+1);
    };
    dfs(root,0,0);
    // serialize table
    bw.putBits(uint32_t(table.size()),16);
    for(auto& kv:table){
        bw.putBits(uint16_t(kv.first),16);
        bw.putBits(uint8_t(kv.second.second),8);
        bw.putBits(kv.second.first, kv.second.second);
    }
    // write data
    for(auto v:data){
        auto [code,len] = table[v];
        bw.putBits(code,len);
    }
    bw.flush();
}

inline std::vector<int16_t> Huffman_decode(BitReader& br) {
    uint32_t tableSize=0;
    for(int i=0;i<16;++i) tableSize = (tableSize<<1) | br.getBit();
    struct PairHash {
        size_t operator()(std::pair<uint32_t,int> const& p) const noexcept {
            return (size_t(p.first)<<32) ^ size_t(p.second);
        }
    };
    std::unordered_map<std::pair<uint32_t,int>,int16_t,PairHash> decodeTable;
    for(uint32_t i=0;i<tableSize;++i){
        uint16_t sym=0; for(int j=0;j<16;++j) sym = (sym<<1)|br.getBit();
        uint8_t len=0; for(int j=0;j<8;++j) len = (len<<1)|br.getBit();
        uint32_t code=0; for(int j=0;j<len;++j) code = (code<<1)|br.getBit();
        decodeTable[{code,len}] = sym;
    }
    std::vector<int16_t> out;
    uint32_t cur=0; int clen=0;
    while(true){
        int b = br.getBit();
        if(b<0) break;
        cur = (cur<<1)|b; ++clen;
        auto it = decodeTable.find({cur,clen});
        if(it!=decodeTable.end()){
            out.push_back(it->second);
            cur=0; clen=0;
        }
    }
    return out;
}
