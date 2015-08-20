/** \brief ucode_eth
 * Ethernet microcode
 * arg0: number of 64 bits elements for packet 1
 * arg1: number of 8 bits elements (payload size) for packet 1
 * arg2: number of 64 bits elements for packet 2
 * arg3: number of 8 bits elements (payload size) for packet 2
 * arg4: number of 64 bits elements for packet 3
 * arg5: number of 8 bits elements (payload size) for packet 3
 * arg6: number of 64 bits elements for packet 4
 * arg7: number of 8 bits elements (payload size) for packet 4
 * p0: address of packet 1
 * p1: address of packet 2
 * p2: address of packet 3
 * p3: address of packet 4
 */
unsigned long long ucode_eth[] __attribute__((aligned(128) )) = {
0x0000001000600000ULL,  /* C_0: dcnt0=R[0];*/
0x000000000040000eULL,  /* C_1: if(dcnt0==0) goto C_3; dcnt0--;*/
0x0000000803c0000bULL,  /* C_2: READ8(ptr_0,chan_0); ptr_0+=8; if(dcnt0!=0) goto C_2; dcnt0--;*/
0x0000003000608000ULL,  /* C_3: FLUSH(chan_0); dcnt0=R[1];*/
0x000000000040001aULL,  /* C_4: if(dcnt0==0) goto C_6; dcnt0--;*/
0x0000000802400017ULL,  /* C_5: READ1(ptr_0,chan_0); ptr_0+=1; if(dcnt0!=0) goto C_5; dcnt0--;*/
0x0000000000004000ULL,  /* C_6: SEND_EOT(chan_0);*/
0x0000005000600041ULL,  /* C_7: goto C_16; dcnt0=R[2];*/
0x0000000000001000ULL,  /* C_8: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_9: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_10: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_11: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_12: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_13: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_14: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_15: STOP(); ALIGN DROP ADDRESS */
0x000000000040004aULL,  /* C_16: if(dcnt0==0) goto C_18; dcnt0--;*/
0x0000000843c00047ULL,  /* C_17: READ8(ptr_1,chan_0); ptr_1+=8; if(dcnt0!=0) goto C_17; dcnt0--;*/
0x0000007000608000ULL,  /* C_18: FLUSH(chan_0); dcnt0=R[3];*/
0x0000000000400056ULL,  /* C_19: if(dcnt0==0) goto C_21; dcnt0--;*/
0x0000000842400053ULL,  /* C_20: READ1(ptr_1,chan_0); ptr_1+=1; if(dcnt0!=0) goto C_20; dcnt0--;*/
0x0000000000004000ULL,  /* C_21: SEND_EOT(chan_0);*/
0x0000009000600081ULL,  /* C_22: goto C_32; dcnt0=R[4];*/
0x0000000000000081ULL,  /* C_23: goto C_32;*/
0x0000000000001000ULL,  /* C_24: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_25: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_26: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_27: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_28: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_29: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_30: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_31: STOP(); ALIGN DROP ADDRESS */
0x000000000040008aULL,  /* C_32: if(dcnt0==0) goto C_34; dcnt0--;*/
0x0000000883c00087ULL,  /* C_33: READ8(ptr_2,chan_0); ptr_2+=8; if(dcnt0!=0) goto C_33; dcnt0--;*/
0x000000b000608000ULL,  /* C_34: FLUSH(chan_0); dcnt0=R[5];*/
0x0000000000400096ULL,  /* C_35: if(dcnt0==0) goto C_37; dcnt0--;*/
0x0000000882400093ULL,  /* C_36: READ1(ptr_2,chan_0); ptr_2+=1; if(dcnt0!=0) goto C_36; dcnt0--;*/
0x0000000000004000ULL,  /* C_37: SEND_EOT(chan_0);*/
0x000000d0006000c1ULL,  /* C_38: goto C_48; dcnt0=R[6];*/
0x00000000000000c1ULL,  /* C_39: goto C_48;*/
0x0000000000001000ULL,  /* C_40: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_41: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_42: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_43: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_44: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_45: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_46: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_47: STOP(); ALIGN DROP ADDRESS */
0x00000000004000caULL,  /* C_48: if(dcnt0==0) goto C_50; dcnt0--;*/
0x00000008c3c000c7ULL,  /* C_49: READ8(ptr_3,chan_0); ptr_3+=8; if(dcnt0!=0) goto C_49; dcnt0--;*/
0x000000f000608000ULL,  /* C_50: FLUSH(chan_0); dcnt0=R[7];*/
0x00000000004000d6ULL,  /* C_51: if(dcnt0==0) goto C_53; dcnt0--;*/
0x00000008c24000d3ULL,  /* C_52: READ1(ptr_3,chan_0); ptr_3+=1; if(dcnt0!=0) goto C_52; dcnt0--;*/
0x0000000000004000ULL,  /* C_53: SEND_EOT(chan_0);*/
0x0000000000003000ULL}; /* C_54: STOP(); SEND_IT();*/

