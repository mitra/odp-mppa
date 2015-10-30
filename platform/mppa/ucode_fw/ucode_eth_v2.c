/** \brief ucode_eth_v2
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
unsigned long long ucode_eth_v2[] __attribute__((aligned(128) )) = {
0x0000000000022000ULL,  /* C_0: SEND_IT(); WAIT_TOKEN();*/
0x0000001000600000ULL,  /* C_1: dcnt0=R[0];*/
0x0000003000680000ULL,  /* C_2: dcnt1=R[1];*/
0x000000000008004aULL,  /* C_3: if(dcnt1==0) goto C_18;*/
0x000000000040001eULL,  /* C_4: if(dcnt0==0) goto C_7; dcnt0--;*/
0x0000000803c00017ULL,  /* C_5: READ8(ptr_0,chan_0); ptr_0+=8; if(dcnt0!=0) goto C_5; dcnt0--;*/
0x0000000000008000ULL,  /* C_6: FLUSH(chan_0);*/
0x0000000000480041ULL,  /* C_7: goto C_16; dcnt1--;*/
0x0000000000001000ULL,  /* C_8: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_9: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_10: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_11: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_12: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_13: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_14: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_15: STOP(); ALIGN DROP ADDRESS */
0x0000000802480043ULL,  /* C_16: READ1(ptr_0,chan_0); ptr_0+=1; if(dcnt1!=0) goto C_16; dcnt1--;*/
0x0000000000004059ULL,  /* C_17: SEND_EOT(chan_0); goto C_22;*/
0x000000000040005aULL,  /* C_18: if(dcnt0==0) goto C_22; dcnt0--;*/
0x0000000000400000ULL,  /* C_19: dcnt0--;*/
0x0000000803c00053ULL,  /* C_20: READ8(ptr_0,chan_0); ptr_0+=8; if(dcnt0!=0) goto C_20; dcnt0--;*/
0x0000000803804000ULL,  /* C_21: SEND_EOT(chan_0); READ8(ptr_0,chan_0); ptr_0+=8;*/
0x0000005000600000ULL,  /* C_22: dcnt0=R[2];*/
0x0000007000680081ULL,  /* C_23: goto C_32; dcnt1=R[3];*/
0x0000000000001000ULL,  /* C_24: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_25: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_26: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_27: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_28: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_29: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_30: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_31: STOP(); ALIGN DROP ADDRESS */
0x00000000000800c2ULL,  /* C_32: if(dcnt1==0) goto C_48;*/
0x0000000000400092ULL,  /* C_33: if(dcnt0==0) goto C_36; dcnt0--;*/
0x0000000843c0008bULL,  /* C_34: READ8(ptr_1,chan_0); ptr_1+=8; if(dcnt0!=0) goto C_34; dcnt0--;*/
0x0000000000008000ULL,  /* C_35: FLUSH(chan_0);*/
0x0000000000480000ULL,  /* C_36: dcnt1--;*/
0x0000000842480097ULL,  /* C_37: READ1(ptr_1,chan_0); ptr_1+=1; if(dcnt1!=0) goto C_37; dcnt1--;*/
0x00000000000040d1ULL,  /* C_38: SEND_EOT(chan_0); goto C_52;*/
0x00000000000000c1ULL,  /* C_39: goto C_48;*/
0x0000000000001000ULL,  /* C_40: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_41: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_42: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_43: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_44: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_45: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_46: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_47: STOP(); ALIGN DROP ADDRESS */
0x00000000004000d2ULL,  /* C_48: if(dcnt0==0) goto C_52; dcnt0--;*/
0x0000000000400000ULL,  /* C_49: dcnt0--;*/
0x0000000843c000cbULL,  /* C_50: READ8(ptr_1,chan_0); ptr_1+=8; if(dcnt0!=0) goto C_50; dcnt0--;*/
0x0000000843804000ULL,  /* C_51: SEND_EOT(chan_0); READ8(ptr_1,chan_0); ptr_1+=8;*/
0x0000009000600000ULL,  /* C_52: dcnt0=R[4];*/
0x000000b000680000ULL,  /* C_53: dcnt1=R[5];*/
0x000000000008011aULL,  /* C_54: if(dcnt1==0) goto C_70;*/
0x0000000000000101ULL,  /* C_55: goto C_64;*/
0x0000000000001000ULL,  /* C_56: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_57: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_58: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_59: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_60: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_61: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_62: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_63: STOP(); ALIGN DROP ADDRESS */
0x000000000040010eULL,  /* C_64: if(dcnt0==0) goto C_67; dcnt0--;*/
0x0000000883c00107ULL,  /* C_65: READ8(ptr_2,chan_0); ptr_2+=8; if(dcnt0!=0) goto C_65; dcnt0--;*/
0x0000000000008000ULL,  /* C_66: FLUSH(chan_0);*/
0x0000000000480000ULL,  /* C_67: dcnt1--;*/
0x0000000882480113ULL,  /* C_68: READ1(ptr_2,chan_0); ptr_2+=1; if(dcnt1!=0) goto C_68; dcnt1--;*/
0x0000000000004149ULL,  /* C_69: SEND_EOT(chan_0); goto C_82;*/
0x000000000040014aULL,  /* C_70: if(dcnt0==0) goto C_82; dcnt0--;*/
0x0000000000400141ULL,  /* C_71: goto C_80; dcnt0--;*/
0x0000000000001000ULL,  /* C_72: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_73: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_74: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_75: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_76: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_77: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_78: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_79: STOP(); ALIGN DROP ADDRESS */
0x0000000883c00143ULL,  /* C_80: READ8(ptr_2,chan_0); ptr_2+=8; if(dcnt0!=0) goto C_80; dcnt0--;*/
0x0000000883804000ULL,  /* C_81: SEND_EOT(chan_0); READ8(ptr_2,chan_0); ptr_2+=8;*/
0x000000d000600000ULL,  /* C_82: dcnt0=R[6];*/
0x000000f000680000ULL,  /* C_83: dcnt1=R[7];*/
0x000000000008018eULL,  /* C_84: if(dcnt1==0) goto C_99;*/
0x0000000000400182ULL,  /* C_85: if(dcnt0==0) goto C_96; dcnt0--;*/
0x00000008c3c0015bULL,  /* C_86: READ8(ptr_3,chan_0); ptr_3+=8; if(dcnt0!=0) goto C_86; dcnt0--;*/
0x0000000000008181ULL,  /* C_87: FLUSH(chan_0); goto C_96;*/
0x0000000000001000ULL,  /* C_88: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_89: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_90: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_91: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_92: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_93: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_94: STOP(); ALIGN DROP ADDRESS */
0x0000000000001000ULL,  /* C_95: STOP(); ALIGN DROP ADDRESS */
0x0000000000480000ULL,  /* C_96: dcnt1--;*/
0x00000008c2480187ULL,  /* C_97: READ1(ptr_3,chan_0); ptr_3+=1; if(dcnt1!=0) goto C_97; dcnt1--;*/
0x000000000000419dULL,  /* C_98: SEND_EOT(chan_0); goto C_103;*/
0x000000000040019eULL,  /* C_99: if(dcnt0==0) goto C_103; dcnt0--;*/
0x0000000000400000ULL,  /* C_100: dcnt0--;*/
0x00000008c3c00197ULL,  /* C_101: READ8(ptr_3,chan_0); ptr_3+=8; if(dcnt0!=0) goto C_101; dcnt0--;*/
0x00000008c3804000ULL,  /* C_102: SEND_EOT(chan_0); READ8(ptr_3,chan_0); ptr_3+=8;*/
0x0000000000000001ULL}; /* C_103: goto C_0;*/

