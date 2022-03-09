#include "type.h"

#ifndef LOADER_DEF
#define LOADER_DEF



// this is the function for edge loader

void edgereader(hls::stream<edgesIO> &inputedges, hls::stream<vidIN> &vidstream, int & edgecount){
    #pragma HLS interface axis port = input
    #pragma HLS interface s_axilite port = edgecount bundle = control
    #pragma HLS interface ap_ctrl_none port = return

    int count = 0;

    edgeloadingprocess:
    while (1){
        #pragma HLS pipeline II=2
        ap_uint<1> last;
        edgesIO tmpedgesIO = inputedges.read();
        ap_uint<EdgeWidth> tmpedges = tmpedgesIO.get_data();
        last = tmpedgesIO.get_last();
        vidIN src;
        vidIN dst;

        src.data = tmpedges.range(31,0);
        dst.data = tmpedges.range(63,32);

        src.set_last(0);
        dst.set_last(last);

        vidstream.write(src);
        vidstream.write(dst);

        count++;
        if(last == 1){
            break;
        }

    }

    edgecount = count;

}



void neighborloader(AXI_indextype_IO * adjtable, hls::stream<vidIN> &vidstreamin, hls::stream<vidIN> &vidstreamout){
    
    vidIN tmpvid;

    while (1){
        tmpvid = vidstreamin.read();

        vidstreamout.write(tmpvid);
        ap_uint<1> last;
        ap_uint<VIDWIDTH> id;
        id = tmpvid.get_data();
        last = tmpvid.get_last();
        AXI_indextype_IO tmptablerow = adjtable[id];

        for(int i = 0; i < FIFOLEN; i++){
            #pragma HLS pipeline II=1
            vidIN tmpvid2;
            tmpvid2.set_data(tmptablerow.index[i]);
            if(last == 1 && i == FIFOLEN - 1){
                tmpvid2.set_last(1);
            }
            else{
                tmpvid2.set_last(0);
            }
            vidstreamout.write(tmpvid2);
            
        }
        if(last == 1){
            break
        }

    }
    

}


void mailloader(AXI_datatype_IO * mailbox, hls::stream<vidIN> &vidstreamin, hls::stream<MAILonchip> & mailstreamout){
#pragma HLS aggregate variable=mailstreamout

    vidIN tmpvid;

    while (1){
        tmpvid = vidstreamin.read();
        ap_uint<1> last;
        ap_uint<VIDWIDTH> id;
        id = tmpvid.get_data();
        last = tmpvid.get_last();

        for(int i = 0;i < int((MAILLEN)/GRUUROLLF) + 1); i++){
            #pragma HLS pipeline II=1

            MAILonchip tmpmailslice;
            #pragma HLS aggregate variable=tmpmailslice

            AXI_datatype_IO mailslice;
            #pragma HLS aggregate variable=mailslice
            
            mailslice = mailbox[id*(int((MAILLEN)/GRUUROLLF) + 1) + i];

            tmpmailslice.VID = id;
            tmpmailslice.field = i;

            for(int j; j < 16 ;j ++){
                tmpmailslice.mail[j] = mailslice.data[j];
            }

            if(last == 1 &&  i== int(MAILLEN)/GRUUROLLF)){
                tmpmailslice.last = 1;
            }
            else {
                tmpmailslice.last = 0;
            }
            mailstreamout.write(MAILonchip);
        }

    }

}


void memoryloader(AXI_datatype_IO * vmem, hls::stream<vidIN> &vidstreamin, hls::stream<MEMonchip> & vmemstreamout){
#pragma HLS aggregate variable=vmemstreamout

    vidIN tmpvid;

    while (1){
        tmpvid = vidstreamin.read();
        ap_uint<1> last;
        ap_uint<VIDWIDTH> id;
        id = tmpvid.get_data();
        last = tmpvid.get_last();

        for(int i = 0;i < (int((MEMLEN)/GRUUROLLF) + 1); i++){
            #pragma HLS pipeline II=1

            MEMonchip tmpmailslice;
            #pragma HLS aggregate variable=tmpmailslice

            AXI_datatype_IO mailslice;
            #pragma HLS aggregate variable=mailslice
            
            mailslice = mailbox[id*(int((MEMLEN)/GRUUROLLF) + 1) + i];

            tmpmailslice.VID = id;
            tmpmailslice.field = i;

            for(int j; j < 16 ;j ++){
                tmpmailslice.mail[j] = mailslice.data[j];
            }

            if(last == 1 &&  i== (int(MEMLEN)/GRUUROLLF)){
                tmpmailslice.last = 1;
            }
            else {
                tmpmailslice.last = 0;
            }
            mailstreamout.write(MAILonchip);
        }

    }


}



#endif
