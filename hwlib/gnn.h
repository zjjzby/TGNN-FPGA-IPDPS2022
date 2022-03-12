#include "./type.h"
#include "./activation.h"


#ifndef GNN_UNIT
#define GNN_UNIT


void aggregate(
    hls::stream<DataType> & attentionscore,
    hls::stream<MEMonchip> & vmemstreamint,
    DataType datatile[FACTORH][GRUUROLLF][GRUUROLLF]
){

//    for(int j =0;j < FACTORH; j++){
//        for(int i =0;i < NNeighbor; i++){
//            DataType attscore = attentionscore.read();
//            for(int p = 0; p < GRUUROLLF; p++){
//				#pragma HLS LOOP_FLATTEN
//                #pragma HLS pipeline II=1
//                #pragma HLS DEPENDENCE variable=datatile inter false
//
//                MEMonchip tmpmem;
//                #pragma HLS aggregate variable=tmpmem
//                tmpmem = vmemstreamint.read();
//                for(int k = 0; k < GRUUROLLF; k++){
//                    DataType tmp = tmpmem.mem[k]*attscore;
//                    if(i == 0){datatile[j][p][k] = tmp;}
//                    else { datatile[j][p][k] = datatile[j][p][k] + tmp;}
//                }
//            }
//        }
//    }

		int i,j,p;
		i = 0;
		j = 0;
		p = 0;
		DataType attscore;

	    for(int u =0;u < FACTORH*NNeighbor*GRUUROLLF; u++){
			#pragma HLS LOOP_FLATTEN
	        #pragma HLS pipeline II=1
	        #pragma HLS DEPENDENCE variable=datatile inter false


	    	if(u % GRUUROLLF == 0){
	    		i++;
	    		attscore = attentionscore.read();
	    	}

	    	if(i == NNeighbor){
	    		j++;
	    		i = 0;
	    	}

			MEMonchip tmpmem;
			#pragma HLS aggregate variable=tmpmem

			tmpmem = vmemstreamint.read();

			for(int k = 0; k < GRUUROLLF; k++){
				DataType tmp = tmpmem.mem[k]*attscore;
				if(i == 0){datatile[j][p][k] = tmp;}
				else { datatile[j][p][k] = datatile[j][p][k] + tmp;}
			}


	    }



}

void transform(
    DataType datatile[FACTORH][GRUUROLLF][GRUUROLLF],
    DataType result[FACTORH][GRUUROLLF][GRUUROLLF]
){

    static DataType WW[FACTORH][FACTORH][GRUUROLLF][GRUUROLLF];
	#pragma HLS BIND_STORAGE variable=WW type=rom_2p impl=bram
    #pragma HLS ARRAY_PARTITION variable=WW dim=3 complete
    #pragma HLS ARRAY_PARTITION variable=WW dim=4 complete


    for(int i =0; i < FACTORH; i++){
        for(int j =0; j < FACTORH; j++){
            #pragma HLS pipeline II=1
            #pragma HLS DEPENDENCE variable=result inter false
            for(int ii = 0; ii < GRUUROLLF; ii++){
                for(int jj =0; jj < GRUUROLLF; jj++){
                    DataType tmp = datatile[j][ii][jj]*WW[i][j][ii][jj];
                    if(i == 0){result[j][ii][jj]  = tmp;}
                    else{result[j][ii][jj] = result[j][ii][jj] + tmp;}
                }
            }
        }
    }

}


void sendout(
    DataType result[FACTORH][GRUUROLLF][GRUUROLLF],
    hls::stream<MEMonchip> & vmemstreamout
){
    for(int i = 0; i < FACTORH;i++){
        for(int j = 0; j < GRUUROLLF; j++){
            #pragma HLS pipeline II=1

            MEMonchip tmpmem;
            #pragma HLS aggregate variable=tmpmem
            for(int k = 0; k < GRUUROLLF; k++){
                tmpmem.mem[k] = result[i][j][k];
            }
            vmemstreamout.write(tmpmem);
        }
    }


}



void GNNunit(
	hls::stream<DataType> & attentionscore,
    hls::stream<IndexType> vidstreamin,
    hls::stream<MEMonchip> & vmemstreamint,
    hls::stream<MEMonchip> & vmemstreamout
){
	#pragma HLS aggregate variable=attentionscore
	#pragma HLS aggregate variable=vidstreamin
    #pragma HLS aggregate variable=vmemstreamint
    #pragma HLS aggregate variable=vmemstreamout

    #pragma HLS dataflow


    DataType datatile[FACTORH][GRUUROLLF][GRUUROLLF];
    #pragma HLS ARRAY_PARTITION variable=datatile dim=2 complete
    #pragma HLS ARRAY_PARTITION variable=datatile dim=3 complete


    DataType result[FACTORH][GRUUROLLF][GRUUROLLF];
    #pragma HLS ARRAY_PARTITION variable=result dim=2 complete
    #pragma HLS ARRAY_PARTITION variable=result dim=3 complete

    aggregate(attentionscore, vmemstreamint, datatile);

    transform(datatile, result);

    sendout(result, vmemstreamout);

}


#endif