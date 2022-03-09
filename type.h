#include "hls_half.h"
#include <hls_stream.h>
#include <ap_int.h>
#include "ap_axi_sdata.h"


typedef float DataType;
typedef int IndexType;


#ifndef AXI_VAL_DEF
#define AXI_VAL_DEF

/// define the datatype for IO


#define NumDataIO 16
#define NumberIndexIO 16
#define EdgeWidth 64
#define VIDWIDTH 32
#define NNeighbor 10

typedef qdma_axis<EdgeWidth, 0, 0, 0> edgesIO;
typedef ap_axiu<VIDWIDTH, 0, 0, 0> vidIN;




/// define the datatype for Mailbox, memory in DDR

#define MEMLEN 100 // define the length of vertex memory
#define EDGELEN 172 // define the length of edge attribute
#define MAILLEN 372 // define the length of vertex mailbox
#define TIMEENCODELEN 100 // define the length of time encoding


#define GRUUROLLF 8
#define FACTORL (int((MAILLEN)/GRUUROLLF) + 1) + (int((MEMLEN)/GRUUROLLF) + 1)
#define FACTORH (int(MEMLEN/GRUUROLLF) + 1)






struct AXI_datatype_IO{
	DataType data[GRUUROLLF];
};

struct AXI_indextype_IO{
	IndexType index[GRUUROLLF];
};


struct MEMonchip{
    IndexType VID;
    int field;
    ap_uint<1> last;
    DataType mem[GRUUROLLF];
};



struct EMBonchip{
    IndexType VID;
    int field;
    ap_uint<1> last;
    DataType emb[GRUUROLLF];
};

struct EDGEonchip{
    IndexType EID;
    DataType edge[GRUUROLLF];
};

struct MAILonchip{
    IndexType VID;
    int field;
    ap_uint<1> last;
    DataType mail[GRUUROLLF];
};


struct TIMEonchip{
    IndexType VID;
    int field;
    ap_uint<1> last;
    DataType code[GRUUROLLF];
};





// define the datatype of vertex index of the FIFO in DDR

#define FIFOLEN 10




#endif
