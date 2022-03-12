#include "./type.h"
#include "./activation.h"



#ifndef GRU_UNIT
#define GRU_UNIT



void readdatatile(hls::stream<MAILonchip> & mailstreamout,
hls::stream<MEMonchip> & vmemstreamint,
DataType datatile1[FACTORL][GRUUROLLF][GRUUROLLF],
DataType datatile2[FACTORL][GRUUROLLF][GRUUROLLF],
DataType datatile3[FACTORL][GRUUROLLF][GRUUROLLF],
DataType datatile4[FACTORL][GRUUROLLF][GRUUROLLF],
hls::stream<IndexType> & vidstream
){


    for(int j = 0 ; j < GRUUROLLF ; j++){
        for(int i = 0; i < (int((MAILLEN)/GRUUROLLF) + 1) ; i++ ){
            #pragma HLS pipeline II=1


            MAILonchip tmpmailslice;
            #pragma HLS aggregate variable=tmpmailslice

            tmpmailslice = mailstreamout.read();
            if(i == 0){vidstream.write(tmpmailslice.VID);}

            for(int k = 0; k < GRUUROLLF; k++){
                datatile1[i][k][j] = tmpmailslice.mail[k];
                datatile2[i][k][j] = tmpmailslice.mail[k];
                datatile3[i][k][j] = tmpmailslice.mail[k];
                datatile4[i][k][j] = tmpmailslice.mail[k];
            }
        }


        for(int i = 0; i < (int((MEMLEN)/GRUUROLLF) + 1) ; i++ ){
            #pragma HLS pipeline II=1

        	MEMonchip tmpmemslice;
            #pragma HLS aggregate variable=tmpmemslice

            tmpmemslice = vmemstreamint.read();

            for(int k = 0; k < GRUUROLLF;k++){
                datatile1[(int(MAILLEN)/GRUUROLLF) + i][k][j] = tmpmemslice.mem[k];
                datatile2[(int(MAILLEN)/GRUUROLLF) + i][k][j] = tmpmemslice.mem[k];
                datatile3[(int(MAILLEN)/GRUUROLLF) + i][k][j] = tmpmemslice.mem[k];
                datatile4[(int(MAILLEN)/GRUUROLLF) + i][k][j] = tmpmemslice.mem[k];
            }
        }


    }

}



void grucmputezrt(
    DataType datatile1[FACTORL][GRUUROLLF][GRUUROLLF],
    DataType zrt[FACTORH][GRUUROLLF][GRUUROLLF]
){

    static DataType Wz[FACTORL][FACTORH][GRUUROLLF][GRUUROLLF];
#pragma HLS BIND_STORAGE variable=Wz type=rom_2p impl=bram
    #pragma HLS ARRAY_PARTITION variable=Wz dim=3 complete
    #pragma HLS ARRAY_PARTITION variable=Wz dim=4 complete


    for(int i = 0; i < FACTORL ; i++){
        for(int j = 0; j< FACTORH; j++ ){
            #pragma HLS pipeline II=1
            #pragma HLS DEPENDENCE variable=zrt inter false
            for(int ii = 0 ; ii < GRUUROLLF; ii++){
                for(int jj = 0; jj < GRUUROLLF; jj++){
                    DataType tmp = datatile1[j][ii][jj]*Wz[i][j][ii][jj];
                    if (i == 0) {
                        zrt[j][ii][jj] = tmp;
                    }
                    else{
                        zrt[j][ii][jj] = zrt[j][ii][jj]  + tmp;
                    }
                }
            }
        }
    }


    for(int j = 0; j< FACTORH; j++ ){
        for(int ii = 0 ; ii < GRUUROLLF; ii++){
            #pragma HLS pipeline II=1
            #pragma HLS DEPENDENCE variable=zrt inter false
            for(int jj = 0; jj < GRUUROLLF; jj++){
                zrt[j][ii][jj] = mysigmoid(zrt[j][ii][jj]);
            }
        }
    }

}


void grucmputerrt(
    DataType datatile2[FACTORL][GRUUROLLF][GRUUROLLF],
    DataType rrt[FACTORH][GRUUROLLF][GRUUROLLF]
){

    static DataType Wr[FACTORL][FACTORH][GRUUROLLF][GRUUROLLF];
	#pragma HLS BIND_STORAGE variable=Wr type=rom_2p impl=bram
    #pragma HLS ARRAY_PARTITION variable=Wr dim=3 complete
    #pragma HLS ARRAY_PARTITION variable=Wr dim=4 complete


    for(int i = 0; i < FACTORL ; i++){
        for(int j = 0; j< FACTORH; j++ ){
            #pragma HLS pipeline II=1
            #pragma HLS DEPENDENCE variable=rrt inter false
            for(int ii = 0 ; ii < GRUUROLLF; ii++){
                for(int jj = 0; jj < GRUUROLLF; jj++){
                    DataType tmp = datatile2[i][ii][jj]*Wr[i][j][ii][jj];
                    if(i == 0){
                        rrt[j][ii][jj] = tmp;
                    }
                    else{
                        rrt[j][ii][jj] = rrt[j][ii][jj]  + tmp;
                    }
                }
            }
        }
    }

    for(int j = 0; j< FACTORH; j++ ){
        for(int ii = 0 ; ii < GRUUROLLF; ii++){
            #pragma HLS pipeline II=1
            #pragma HLS DEPENDENCE variable=rrt inter false
            for(int jj = 0; jj < GRUUROLLF; jj++){
                rrt[j][ii][jj] = mysigmoid(rrt[j][ii][jj]);
            }
        }
    }

}



void grucmputehrt(
    DataType rrt[FACTORH][GRUUROLLF][GRUUROLLF],
    DataType datatile3[FACTORL][GRUUROLLF][GRUUROLLF],
    DataType hpt[FACTORH][GRUUROLLF][GRUUROLLF]
){

    static DataType Wh[FACTORL][FACTORH][GRUUROLLF][GRUUROLLF];
	#pragma HLS BIND_STORAGE variable=Wh type=rom_2p impl=bram
    #pragma HLS ARRAY_PARTITION variable=Wh dim=3 complete
    #pragma HLS ARRAY_PARTITION variable=Wh dim=4 complete


    DataType copydatatile3[FACTORL][GRUUROLLF][GRUUROLLF];
	#pragma HLS ARRAY_PARTITION variable=copydatatile3 dim=2 complete
    #pragma HLS ARRAY_PARTITION variable=copydatatile3 dim=3 complete


    for(int i = 0; i < FACTORH ; i++){
        #pragma HLS pipeline II=1
        #pragma HLS DEPENDENCE variable=datatile3 inter false
        for(int ii = 0 ; ii < GRUUROLLF; ii++){
            for(int jj = 0; jj < GRUUROLLF; jj++){
            	copydatatile3[i+ FACTORH][ii][jj] = datatile3[i+ FACTORH][ii][jj]  + rrt[i][ii][jj];
            }
        }
    }

    for(int i = 0; i < FACTORL ; i++){
        for(int j = 0; j< FACTORH; j++ ){
            #pragma HLS pipeline II=1
            #pragma HLS DEPENDENCE variable=hpt inter false
            for(int ii = 0 ; ii < GRUUROLLF; ii++){
                for(int jj = 0; jj < GRUUROLLF; jj++){
                    DataType tmp = copydatatile3[i][ii][jj]*Wh[i][j][ii][jj];
                    if (i == 0) {
                        hpt[j][ii][jj] = tmp;
                    }
                    else{
                        hpt[j][ii][jj] = hpt[j][ii][jj]  + tmp;
                    }
                }
            }
        }
    }


    for(int j = 0; j< FACTORH; j++ ){
        for(int ii = 0 ; ii < GRUUROLLF; ii++){
            #pragma HLS pipeline II=1
            #pragma HLS DEPENDENCE variable=hpt inter false
            for(int jj = 0; jj < GRUUROLLF; jj++){
                hpt[j][ii][jj] = mytanh(hpt[j][ii][jj]);
            }
        }
    }

}



void computefinalht(
    DataType zrt[FACTORH][GRUUROLLF][GRUUROLLF],
    DataType hpt[FACTORH][GRUUROLLF][GRUUROLLF],
    DataType datatile4[FACTORL][GRUUROLLF][GRUUROLLF],
    hls::stream<MEMonchip> & vmemstreamout,
    hls::stream<IndexType> & vidstream
){


    for(int ii = 0 ; ii < GRUUROLLF; ii++){
        IndexType tmpvid = vidstream.read();
        for(int j = 0; j< FACTORH; j++ ){
            #pragma HLS pipeline II=1
            #pragma HLS DEPENDENCE variable=hpt inter false
            MEMonchip tmpmem;
            #pragma HLS aggregate variable=tmpmem 
            for(int jj = 0; jj < GRUUROLLF; jj++){
                tmpmem.mem[jj] = (1 - zrt[j][ii][jj])*datatile4[(int((MAILLEN)/GRUUROLLF) + 1) + j][ii][jj] + zrt[j][ii][jj]*hpt[j][ii][jj];
            }
            tmpmem.field = j;
            tmpmem.VID = tmpvid;
            vmemstreamout.write(tmpmem);
        }
    }

}


void GRUunit(
    hls::stream<TIMEonchip> & tstreamint,
    hls::stream<MEMonchip> & vmemstreamint,
    hls::stream<MAILonchip> & mailstreamout,
    hls::stream<MEMonchip> & vmemstreamout
){
    #pragma HLS aggregate variable=tstreamint
    #pragma HLS aggregate variable=vmemstreamint
    #pragma HLS aggregate variable=mailstreamout
    #pragma HLS aggregate variable=vmemstreamout

    #pragma HLS dataflow


    DataType datatile1[FACTORL][GRUUROLLF][GRUUROLLF];
    #pragma HLS ARRAY_PARTITION variable=datatile1 dim=2 complete
    #pragma HLS ARRAY_PARTITION variable=datatile1 dim=3 complete

    DataType datatile2[FACTORL][GRUUROLLF][GRUUROLLF];
    #pragma HLS ARRAY_PARTITION variable=datatile2 dim=2 complete
    #pragma HLS ARRAY_PARTITION variable=datatile2 dim=3 complete

    DataType datatile3[FACTORL][GRUUROLLF][GRUUROLLF];
    #pragma HLS ARRAY_PARTITION variable=datatile3 dim=2 complete
    #pragma HLS ARRAY_PARTITION variable=datatile3 dim=3 complete


    DataType datatile4[FACTORL][GRUUROLLF][GRUUROLLF];
    #pragma HLS ARRAY_PARTITION variable=datatile4 dim=2 complete
    #pragma HLS ARRAY_PARTITION variable=datatile4 dim=3 complete

    DataType zrt[FACTORH][GRUUROLLF][GRUUROLLF];
    #pragma HLS ARRAY_PARTITION variable=zrt dim=2 complete
    #pragma HLS ARRAY_PARTITION variable=zrt dim=3 complete

    DataType rrt[FACTORH][GRUUROLLF][GRUUROLLF];
    #pragma HLS ARRAY_PARTITION variable=rrt dim=2 complete
    #pragma HLS ARRAY_PARTITION variable=rrt dim=3 complete

    DataType hpt[FACTORH][GRUUROLLF][GRUUROLLF];
    #pragma HLS ARRAY_PARTITION variable=hpt dim=2 complete
    #pragma HLS ARRAY_PARTITION variable=hpt dim=3 complete

    hls::stream<IndexType> vidstream;
    #pragma HLS STREAM variable=vidstream depth=24

    readdatatile(mailstreamout, vmemstreamint, datatile1, datatile2, datatile3, datatile4, vidstream);

    grucmputezrt(datatile1, zrt);

    grucmputerrt(datatile2, rrt);

    grucmputehrt(rrt, datatile3, hpt);

    computefinalht(zrt, hpt, datatile4, vmemstreamout, vidstream);

}




#endif