/** \file
 * \brief Example code for Simple Open EtherCAT master
 *
 * Usage : slaveinfo [ifname] [-sdo] [-map]
 * Ifname is NIC interface, f.e. eth0.
 * Optional -sdo to display CoE object dictionary.
 * Optional -map to display slave PDO mapping
 *
 * This shows the configured slave data.
 *
 * (c)Arthur Ketels 2010 - 2011
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

#include "soem/soem/ethercat.h"

char IOmap[4096];
ec_ODlistt ODlist;
ec_OElistt OElist;
boolean printSDO = FALSE;
boolean printMAP = FALSE;
char usdo[128];
char hstr[1024];

// External data
extern ecx_contextt ecat_context;

char* dtype2string(uint16 dtype)
{
    switch(dtype)
    {
        case ECT_BOOLEAN:
            sprintf(hstr, "BOOLEAN");
            break;
        case ECT_INTEGER8:
            sprintf(hstr, "INTEGER8");
            break;
        case ECT_INTEGER16:
            sprintf(hstr, "INTEGER16");
            break;
        case ECT_INTEGER32:
            sprintf(hstr, "INTEGER32");
            break;
        case ECT_INTEGER24:
            sprintf(hstr, "INTEGER24");
            break;
        case ECT_INTEGER64:
            sprintf(hstr, "INTEGER64");
            break;
        case ECT_UNSIGNED8:
            sprintf(hstr, "UNSIGNED8");
            break;
        case ECT_UNSIGNED16:
            sprintf(hstr, "UNSIGNED16");
            break;
        case ECT_UNSIGNED32:
            sprintf(hstr, "UNSIGNED32");
            break;
        case ECT_UNSIGNED24:
            sprintf(hstr, "UNSIGNED24");
            break;
        case ECT_UNSIGNED64:
            sprintf(hstr, "UNSIGNED64");
            break;
        case ECT_REAL32:
            sprintf(hstr, "REAL32");
            break;
        case ECT_REAL64:
            sprintf(hstr, "REAL64");
            break;
        case ECT_BIT1:
            sprintf(hstr, "BIT1");
            break;
        case ECT_BIT2:
            sprintf(hstr, "BIT2");
            break;
        case ECT_BIT3:
            sprintf(hstr, "BIT3");
            break;
        case ECT_BIT4:
            sprintf(hstr, "BIT4");
            break;
        case ECT_BIT5:
            sprintf(hstr, "BIT5");
            break;
        case ECT_BIT6:
            sprintf(hstr, "BIT6");
            break;
        case ECT_BIT7:
            sprintf(hstr, "BIT7");
            break;
        case ECT_BIT8:
            sprintf(hstr, "BIT8");
            break;
        case ECT_VISIBLE_STRING:
            sprintf(hstr, "VISIBLE_STRING");
            break;
        case ECT_OCTET_STRING:
            sprintf(hstr, "OCTET_STRING");
            break;
        default:
            sprintf(hstr, "Type 0x%4.4X", dtype);
    }
    return hstr;
}

char* SDO2string(uint16 slave, uint16 index, uint8 subidx, uint16 dtype)
{
   int l = sizeof(usdo) - 1, i;
   uint8 *u8;
   int8 *i8;
   uint16 *u16;
   int16 *i16;
   uint32 *u32;
   int32 *i32;
   uint64 *u64;
   int64 *i64;
   float *sr;
   double *dr;
   char es[32];

   memset(&usdo, 0, 128);
   ecx_SDOread(&ecat_context, slave, index, subidx, FALSE, &l, &usdo, EC_TIMEOUTRXM);
   if (*ecat_context.ecaterror)
   {
      return ecx_elist2string(&ecat_context);
   }
   else
   {
      switch(dtype)
      {
         case ECT_BOOLEAN:
            u8 = (uint8*) &usdo[0];
            if (*u8) sprintf(hstr, "TRUE");
             else sprintf(hstr, "FALSE");
            break;
         case ECT_INTEGER8:
            i8 = (int8*) &usdo[0];
            sprintf(hstr, "0x%2.2x %d", *i8, *i8);
            break;
         case ECT_INTEGER16:
            i16 = (int16*) &usdo[0];
            sprintf(hstr, "0x%4.4x %d", *i16, *i16);
            break;
         case ECT_INTEGER32:
         case ECT_INTEGER24:
            i32 = (int32*) &usdo[0];
            sprintf(hstr, "0x%8.8x %d", *i32, *i32);
            break;
         case ECT_INTEGER64:
            i64 = (int64*) &usdo[0];
            sprintf(hstr, "0x%16.16"PRIx64" %"PRId64, *i64, *i64);
            break;
         case ECT_UNSIGNED8:
            u8 = (uint8*) &usdo[0];
            sprintf(hstr, "0x%2.2x %u", *u8, *u8);
            break;
         case ECT_UNSIGNED16:
            u16 = (uint16*) &usdo[0];
            sprintf(hstr, "0x%4.4x %u", *u16, *u16);
            break;
         case ECT_UNSIGNED32:
         case ECT_UNSIGNED24:
            u32 = (uint32*) &usdo[0];
            sprintf(hstr, "0x%8.8x %u", *u32, *u32);
            break;
         case ECT_UNSIGNED64:
            u64 = (uint64*) &usdo[0];
            sprintf(hstr, "0x%16.16"PRIx64" %"PRIu64, *u64, *u64);
            break;
         case ECT_REAL32:
            sr = (float*) &usdo[0];
            sprintf(hstr, "%f", *sr);
            break;
         case ECT_REAL64:
            dr = (double*) &usdo[0];
            sprintf(hstr, "%f", *dr);
            break;
         case ECT_BIT1:
         case ECT_BIT2:
         case ECT_BIT3:
         case ECT_BIT4:
         case ECT_BIT5:
         case ECT_BIT6:
         case ECT_BIT7:
         case ECT_BIT8:
            u8 = (uint8*) &usdo[0];
            sprintf(hstr, "0x%x", *u8);
            break;
         case ECT_VISIBLE_STRING:
            strcpy(hstr, usdo);
            break;
         case ECT_OCTET_STRING:
            hstr[0] = 0x00;
            for (i = 0 ; i < l ; i++)
            {
               sprintf(es, "0x%2.2x ", usdo[i]);
               strcat( hstr, es);
            }
            break;
         default:
            sprintf(hstr, "Unknown type");
      }
      return hstr;
   }
}

/** Read PDO assign structure */
int si_PDOassign(uint16 slave, uint16 PDOassign, int mapoffset, int bitoffset)
{
    uint16 idxloop, nidx, subidxloop, rdat, idx, subidx;
    uint8 subcnt;
    int wkc, bsize = 0, rdl;
    int32 rdat2;
    uint8 bitlen, obj_subidx;
    uint16 obj_idx;
    int abs_offset, abs_bit;

    rdl = sizeof(rdat); rdat = 0;
    /* read PDO assign subindex 0 ( = number of PDO's) */
    wkc = ecx_SDOread(&ecat_context, slave, PDOassign, 0x00, FALSE, &rdl, &rdat, EC_TIMEOUTRXM);
    rdat = etohs(rdat);
    /* positive result from slave ? */
    if ((wkc > 0) && (rdat > 0))
    {
        /* number of available sub indexes */
        nidx = rdat;
        bsize = 0;
        /* read all PDO's */
        for (idxloop = 1; idxloop <= nidx; idxloop++)
        {
            rdl = sizeof(rdat); rdat = 0;
            /* read PDO assign */
            wkc = ecx_SDOread(&ecat_context, slave, PDOassign, (uint8)idxloop, FALSE, &rdl, &rdat, EC_TIMEOUTRXM);
            /* result is index of PDO */
            idx = etohl(rdat);
            if (idx > 0)
            {
                rdl = sizeof(subcnt); subcnt = 0;
                /* read number of subindexes of PDO */
                wkc = ecx_SDOread(&ecat_context, slave,idx, 0x00, FALSE, &rdl, &subcnt, EC_TIMEOUTRXM);
                subidx = subcnt;
                /* for each subindex */
                for (subidxloop = 1; subidxloop <= subidx; subidxloop++)
                {
                    rdl = sizeof(rdat2); rdat2 = 0;
                    /* read SDO that is mapped in PDO */
                    wkc = ecx_SDOread(&ecat_context, slave, idx, (uint8)subidxloop, FALSE, &rdl, &rdat2, EC_TIMEOUTRXM);
                    rdat2 = etohl(rdat2);
                    /* extract bitlength of SDO */
                    bitlen = LO_BYTE(rdat2);
                    bsize += bitlen;
                    obj_idx = (uint16)(rdat2 >> 16);
                    obj_subidx = (uint8)((rdat2 >> 8) & 0x000000ff);
                    abs_offset = mapoffset + (bitoffset / 8);
                    abs_bit = bitoffset % 8;
                    ODlist.Slave = slave;
                    ODlist.Index[0] = obj_idx;
                    OElist.Entries = 0;
                    wkc = 0;
                    /* read object entry from dictionary if not a filler (0x0000:0x00) */
                    if(obj_idx || obj_subidx)
                        wkc = ecx_readOEsingle(&ecat_context, 0, obj_subidx, &ODlist, &OElist);
                    printf("  [0x%4.4X.%1d] 0x%4.4X:0x%2.2X 0x%2.2X", abs_offset, abs_bit, obj_idx, obj_subidx, bitlen);
                    if((wkc > 0) && OElist.Entries)
                    {
                        printf(" %-12s %s\n", dtype2string(OElist.DataType[obj_subidx]), OElist.Name[obj_subidx]);
                    }
                    else
                        printf("\n");
                    bitoffset += bitlen;
                };
            };
        };
    };
    /* return total found bitlength (PDO) */
    return bsize;
}

int si_map_sdo(int slave)
{
    int wkc, rdl;
    int retVal = 0;
    uint8 nSM, iSM, tSM;
    int Tsize, outputs_bo, inputs_bo;
    uint8 SMt_bug_add;

    printf("PDO mapping according to CoE :\n");
    SMt_bug_add = 0;
    outputs_bo = 0;
    inputs_bo = 0;
    rdl = sizeof(nSM); nSM = 0;
    /* read SyncManager Communication Type object count */
    wkc = ecx_SDOread(&ecat_context, slave, ECT_SDO_SMCOMMTYPE, 0x00, FALSE, &rdl, &nSM, EC_TIMEOUTRXM);
    /* positive result from slave ? */
    if ((wkc > 0) && (nSM > 2))
    {
        /* make nSM equal to number of defined SM */
        nSM--;
        /* limit to maximum number of SM defined, if true the slave can't be configured */
        if (nSM > EC_MAXSM)
            nSM = EC_MAXSM;
        /* iterate for every SM type defined */
        for (iSM = 2 ; iSM <= nSM ; iSM++)
        {
            rdl = sizeof(tSM); tSM = 0;
            /* read SyncManager Communication Type */
            wkc = ecx_SDOread(&ecat_context, slave, ECT_SDO_SMCOMMTYPE, iSM + 1, FALSE, &rdl, &tSM, EC_TIMEOUTRXM);
            if (wkc > 0)
            {
                if((iSM == 2) && (tSM == 2)) // SM2 has type 2 == mailbox out, this is a bug in the slave!
                {
                    SMt_bug_add = 1; // try to correct, this works if the types are 0 1 2 3 and should be 1 2 3 4
                    printf("Activated SM type workaround, possible incorrect mapping.\n");
                }
                if(tSM)
                    tSM += SMt_bug_add; // only add if SMt > 0

                if (tSM == 3) // outputs
                {
                    /* read the assign RXPDO */
                    printf("  SM%1d outputs\n     addr b   index: sub bitl data_type    name\n", iSM);
                    Tsize = si_PDOassign(slave, ECT_SDO_PDOASSIGN + iSM, (int)(ecat_context.slavelist[slave].outputs - (uint8 *)&IOmap[0]), outputs_bo );
                    outputs_bo += Tsize;
                }
                if (tSM == 4) // inputs
                {
                    /* read the assign TXPDO */
                    printf("  SM%1d inputs\n     addr b   index: sub bitl data_type    name\n", iSM);
                    Tsize = si_PDOassign(slave, ECT_SDO_PDOASSIGN + iSM, (int)(ecat_context.slavelist[slave].inputs - (uint8 *)&IOmap[0]), inputs_bo );
                    inputs_bo += Tsize;
                }
            }
        }
    }

    /* found some I/O bits ? */
    if ((outputs_bo > 0) || (inputs_bo > 0))
        retVal = 1;
    return retVal;
}

int si_siiPDO(uint16 slave, uint8 t, int mapoffset, int bitoffset)
{
    uint16 a , w, c, e, er, Size;
    uint8 eectl;
    uint16 obj_idx;
    uint8 obj_subidx;
    uint8 obj_name;
    uint8 obj_datatype;
    uint8 bitlen;
    int totalsize;
    ec_eepromPDOt eepPDO;
    ec_eepromPDOt *PDO;
    int abs_offset, abs_bit;
    char str_name[EC_MAXNAME + 1];

    eectl = ecat_context.slavelist[slave].eep_pdi;
    Size = 0;
    totalsize = 0;
    PDO = &eepPDO;
    PDO->nPDO = 0;
    PDO->Length = 0;
    PDO->Index[1] = 0;
    for (c = 0 ; c < EC_MAXSM ; c++) PDO->SMbitsize[c] = 0;
    if (t > 1)
        t = 1;
    PDO->Startpos = ecx_siifind(&ecat_context, slave, ECT_SII_PDO + t);
    if (PDO->Startpos > 0)
    {
        a = PDO->Startpos;
        w = ecx_siigetbyte(&ecat_context, slave, a++);
        w += (ecx_siigetbyte(&ecat_context, slave, a++) << 8);
        PDO->Length = w;
        c = 1;
        /* traverse through all PDOs */
        do
        {
            PDO->nPDO++;
            PDO->Index[PDO->nPDO] = ecx_siigetbyte(&ecat_context, slave, a++);
            PDO->Index[PDO->nPDO] += (ecx_siigetbyte(&ecat_context, slave, a++) << 8);
            PDO->BitSize[PDO->nPDO] = 0;
            c++;
            /* number of entries in PDO */
            e = ecx_siigetbyte(&ecat_context, slave, a++);
            PDO->SyncM[PDO->nPDO] = ecx_siigetbyte(&ecat_context, slave, a++);
            a++;
            obj_name = ecx_siigetbyte(&ecat_context, slave, a++);
            a += 2;
            c += 2;
            if (PDO->SyncM[PDO->nPDO] < EC_MAXSM) /* active and in range SM? */
            {
                str_name[0] = 0;
                if(obj_name)
                  ecx_siistring(&ecat_context, str_name, slave, obj_name);
                if (t)
                  printf("  SM%1d RXPDO 0x%4.4X %s\n", PDO->SyncM[PDO->nPDO], PDO->Index[PDO->nPDO], str_name);
                else
                  printf("  SM%1d TXPDO 0x%4.4X %s\n", PDO->SyncM[PDO->nPDO], PDO->Index[PDO->nPDO], str_name);
                printf("     addr b   index: sub bitl data_type    name\n");
                /* read all entries defined in PDO */
                for (er = 1; er <= e; er++)
                {
                    c += 4;
                    obj_idx = ecx_siigetbyte(&ecat_context, slave, a++);
                    obj_idx += (ecx_siigetbyte(&ecat_context, slave, a++) << 8);
                    obj_subidx = ecx_siigetbyte(&ecat_context, slave, a++);
                    obj_name = ecx_siigetbyte(&ecat_context, slave, a++);
                    obj_datatype = ecx_siigetbyte(&ecat_context, slave, a++);
                    bitlen = ecx_siigetbyte(&ecat_context, slave, a++);
                    abs_offset = mapoffset + (bitoffset / 8);
                    abs_bit = bitoffset % 8;

                    PDO->BitSize[PDO->nPDO] += bitlen;
                    a += 2;

                    /* skip entry if filler (0x0000:0x00) */
                    if(obj_idx || obj_subidx)
                    {
                       str_name[0] = 0;
                       if(obj_name)
                          ecx_siistring(&ecat_context, str_name, slave, obj_name);

                       printf("  [0x%4.4X.%1d] 0x%4.4X:0x%2.2X 0x%2.2X", abs_offset, abs_bit, obj_idx, obj_subidx, bitlen);
                       printf(" %-12s %s\n", dtype2string(obj_datatype), str_name);
                    }
                    bitoffset += bitlen;
                    totalsize += bitlen;
                }
                PDO->SMbitsize[ PDO->SyncM[PDO->nPDO] ] += PDO->BitSize[PDO->nPDO];
                Size += PDO->BitSize[PDO->nPDO];
                c++;
            }
            else /* PDO deactivated because SM is 0xff or > EC_MAXSM */
            {
                c += 4 * e;
                a += 8 * e;
                c++;
            }
            if (PDO->nPDO >= (EC_MAXEEPDO - 1)) c = PDO->Length; /* limit number of PDO entries in buffer */
        }
        while (c < PDO->Length);
    }
    if (eectl) ecx_eeprom2pdi(&ecat_context, slave); /* if eeprom control was previously pdi then restore */
    return totalsize;
}


int si_map_sii(int slave)
{
    int retVal = 0;
    int Tsize, outputs_bo, inputs_bo;

    printf("PDO mapping according to SII :\n");

    outputs_bo = 0;
    inputs_bo = 0;
    /* read the assign RXPDOs */
    Tsize = si_siiPDO(slave, 1, (int)(ecat_context.slavelist[slave].outputs - (uint8*)&IOmap), outputs_bo );
    outputs_bo += Tsize;
    /* read the assign TXPDOs */
    Tsize = si_siiPDO(slave, 0, (int)(ecat_context.slavelist[slave].inputs - (uint8*)&IOmap), inputs_bo );
    inputs_bo += Tsize;
    /* found some I/O bits ? */
    if ((outputs_bo > 0) || (inputs_bo > 0))
        retVal = 1;
    return retVal;
}

void si_sdo(int cnt)
{
    int i, j;

    ODlist.Entries = 0;
    memset(&ODlist, 0, sizeof(ODlist));
    if( ecx_readODlist(&ecat_context, cnt, &ODlist))
    {
        printf(" CoE Object Description found, %d entries.\n",ODlist.Entries);
        for( i = 0 ; i < ODlist.Entries ; i++)
        {
            ecx_readODdescription(&ecat_context, i, &ODlist);
            while(ecat_context.ecaterror) printf("%s", ecx_elist2string(&ecat_context));
            printf(" Index: %4.4x Datatype: %4.4x Objectcode: %2.2x Name: %s\n",
                ODlist.Index[i], ODlist.DataType[i], ODlist.ObjectCode[i], ODlist.Name[i]);
            memset(&OElist, 0, sizeof(OElist));
            ecx_readOE(&ecat_context, i, &ODlist, &OElist);
            while(ecat_context.ecaterror) printf("%s", ecx_elist2string(&ecat_context));
            for( j = 0 ; j < ODlist.MaxSub[i]+1 ; j++)
            {
                if ((OElist.DataType[j] > 0) && (OElist.BitLength[j] > 0))
                {
                    printf("  Sub: %2.2x Datatype: %4.4x Bitlength: %4.4x Obj.access: %4.4x Name: %s\n",
                        j, OElist.DataType[j], OElist.BitLength[j], OElist.ObjAccess[j], OElist.Name[j]);
                    if ((OElist.ObjAccess[j] & 0x0007))
                    {
                        printf("          Value :%s\n", SDO2string(cnt, ODlist.Index[i], j, OElist.DataType[j]));
                    }
                }
            }
        }
    }
    else
    {
        while(ecat_context.ecaterror) printf("%s", ecx_elist2string(&ecat_context));
    }
}

int slaveinfo(char *ifname)
{
   int cnt, i, j, nSM;
    uint16 ssigen;
    int expectedWKC;

   printf("Starting slaveinfo on interface %s.\n", ifname);

   /* initialise SOEM, bind socket to ifname */
   if (ecx_init(&ecat_context, ifname))
   {
      printf("ec_init on %s succeeded.\n",ifname);
      /* find and auto-config slaves */
      if ( ecx_config_init(&ecat_context, FALSE) > 0)
      {
         ecx_configdc(&ecat_context);
         while(*ecat_context.ecaterror) printf("%s", ecx_elist2string(&ecat_context));
         printf("%d slaves found and configured.\n", *ecat_context.slavecount);
         expectedWKC = (ecat_context.grouplist[0].outputsWKC * 2) + ecat_context.grouplist[0].inputsWKC;
         printf("Calculated workcounter %d\n", expectedWKC);
         /* wait for all slaves to reach SAFE_OP state */
         ecx_statecheck(&ecat_context, 0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 3);
         if (ecat_context.slavelist[0].state != EC_STATE_SAFE_OP )
         {
            printf("Not all slaves reached safe operational state.\n");
            ecx_readstate(&ecat_context);
            for(i = 1; i<=*ecat_context.slavecount ; i++)
            {
               if(ecat_context.slavelist[i].state != EC_STATE_SAFE_OP)
               {
                  printf("Slave %d State=%2x StatusCode=%4x : %s\n",
                     i, ecat_context.slavelist[i].state, ecat_context.slavelist[i].ALstatuscode, ec_ALstatuscode2string(ecat_context.slavelist[i].ALstatuscode));
               }
            }
         }


         ecx_readstate(&ecat_context);
         for( cnt = 1 ; cnt <= *ecat_context.slavecount ; cnt++)
         {
            printf("\nSlave: %d\n Name: %s\n Output size: %dbits\n Input size: %dbits\n State: %d\n Delay: %d[ns]\n Has DC: %d\n",
                  cnt, ecat_context.slavelist[cnt].name, ecat_context.slavelist[cnt].Obits, ecat_context.slavelist[cnt].Ibits,
                  ecat_context.slavelist[cnt].state, ecat_context.slavelist[cnt].pdelay, ecat_context.slavelist[cnt].hasdc);
            if (ecat_context.slavelist[cnt].hasdc) printf(" DCParentport: %d\n", ecat_context.slavelist[cnt].parentport);
            printf(" Activeports: %d.%d.%d.%d\n", (ecat_context.slavelist[cnt].activeports & 0x01) > 0 ,
                                         (ecat_context.slavelist[cnt].activeports & 0x02) > 0 ,
                                         (ecat_context.slavelist[cnt].activeports & 0x04) > 0 ,
                                         (ecat_context.slavelist[cnt].activeports & 0x08) > 0 );
            printf(" Configured address: %4.4x\n", ecat_context.slavelist[cnt].configadr);
            printf(" Man: %8.8x ID: %8.8x Rev: %8.8x\n", (int)ecat_context.slavelist[cnt].eep_man, (int)ecat_context.slavelist[cnt].eep_id, (int)ecat_context.slavelist[cnt].eep_rev);
            for(nSM = 0 ; nSM < EC_MAXSM ; nSM++)
            {
               if(ecat_context.slavelist[cnt].SM[nSM].StartAddr > 0)
                  printf(" SM%1d A:%4.4x L:%4d F:%8.8x Type:%d\n",nSM, ecat_context.slavelist[cnt].SM[nSM].StartAddr, ecat_context.slavelist[cnt].SM[nSM].SMlength,
                         (int)ecat_context.slavelist[cnt].SM[nSM].SMflags, ecat_context.slavelist[cnt].SMtype[nSM]);
            }
            for(j = 0 ; j < ecat_context.slavelist[cnt].FMMUunused ; j++)
            {
               printf(" FMMU%1d Ls:%8.8x Ll:%4d Lsb:%d Leb:%d Ps:%4.4x Psb:%d Ty:%2.2x Act:%2.2x\n", j,
                       (int)ecat_context.slavelist[cnt].FMMU[j].LogStart, ecat_context.slavelist[cnt].FMMU[j].LogLength, ecat_context.slavelist[cnt].FMMU[j].LogStartbit,
                       ecat_context.slavelist[cnt].FMMU[j].LogEndbit, ecat_context.slavelist[cnt].FMMU[j].PhysStart, ecat_context.slavelist[cnt].FMMU[j].PhysStartBit,
                       ecat_context.slavelist[cnt].FMMU[j].FMMUtype, ecat_context.slavelist[cnt].FMMU[j].FMMUactive);
            }
            printf(" FMMUfunc 0:%d 1:%d 2:%d 3:%d\n",
                     ecat_context.slavelist[cnt].FMMU0func, ecat_context.slavelist[cnt].FMMU2func, ecat_context.slavelist[cnt].FMMU2func, ecat_context.slavelist[cnt].FMMU3func);
            printf(" MBX length wr: %d rd: %d MBX protocols : %2.2x\n", ecat_context.slavelist[cnt].mbx_l, ecat_context.slavelist[cnt].mbx_rl, ecat_context.slavelist[cnt].mbx_proto);
            ssigen = ecx_siifind(&ecat_context, cnt, ECT_SII_GENERAL);
            /* SII general section */
            if (ssigen)
            {
               ecat_context.slavelist[cnt].CoEdetails = ecx_siigetbyte(&ecat_context, cnt, ssigen + 0x07);
               ecat_context.slavelist[cnt].FoEdetails = ecx_siigetbyte(&ecat_context, cnt, ssigen + 0x08);
               ecat_context.slavelist[cnt].EoEdetails = ecx_siigetbyte(&ecat_context, cnt, ssigen + 0x09);
               ecat_context.slavelist[cnt].SoEdetails = ecx_siigetbyte(&ecat_context, cnt, ssigen + 0x0a);
               if((ecx_siigetbyte(&ecat_context, cnt, ssigen + 0x0d) & 0x02) > 0)
               {
                  ecat_context.slavelist[cnt].blockLRW = 1;
                  ecat_context.slavelist[0].blockLRW++;
               }
               ecat_context.slavelist[cnt].Ebuscurrent = ecx_siigetbyte(&ecat_context, cnt, ssigen + 0x0e);
               ecat_context.slavelist[cnt].Ebuscurrent += ecx_siigetbyte(&ecat_context, cnt, ssigen + 0x0f) << 8;
               ecat_context.slavelist[0].Ebuscurrent += ecat_context.slavelist[cnt].Ebuscurrent;
            }
            printf(" CoE details: %2.2x FoE details: %2.2x EoE details: %2.2x SoE details: %2.2x\n",
                    ecat_context.slavelist[cnt].CoEdetails, ecat_context.slavelist[cnt].FoEdetails, ecat_context.slavelist[cnt].EoEdetails, ecat_context.slavelist[cnt].SoEdetails);
            printf(" Ebus current: %d[mA]\n only LRD/LWR:%d\n",
                    ecat_context.slavelist[cnt].Ebuscurrent, ecat_context.slavelist[cnt].blockLRW);
            if ((ecat_context.slavelist[cnt].mbx_proto & ECT_MBXPROT_COE) && printSDO)
                    si_sdo(cnt);
                if(printMAP)
            {
                    if (ecat_context.slavelist[cnt].mbx_proto & ECT_MBXPROT_COE)
                        si_map_sdo(cnt);
                    else
                        si_map_sii(cnt);
            }
         }
      }
      else
      {
         printf("No slaves found!\n");
      }
      printf("End slaveinfo, close socket\n");
      /* stop SOEM, close socket */
      ecx_close(&ecat_context);
      return EXIT_SUCCESS;
   }
   else
   {
      printf("No socket connection on %s\nExecute as root\n",ifname);
      return EXIT_FAILURE;
   }
}

char ifbuf[1024];

int main(int argc, char *argv[])
{
   ec_adaptert * adapter = NULL;
   printf("SOEM (Simple Open EtherCAT Master)\nSlaveinfo\n");
   int return_code = EXIT_FAILURE;

   if (argc > 1)
   {
      if ((argc > 2) && (strncmp(argv[2], "-sdo", sizeof("-sdo")) == 0)) printSDO = TRUE;
      if ((argc > 2) && (strncmp(argv[2], "-map", sizeof("-map")) == 0)) printMAP = TRUE;
      /* start slaveinfo */
      strcpy(ifbuf, argv[1]);
      return_code = slaveinfo(ifbuf);
   }
   else
   {
      printf("Usage: slaveinfo ifname [options]\nifname = eth0 for example\nOptions :\n -sdo : print SDO info\n -map : print mapping\n");

      printf ("Available adapters\n");
      adapter = ec_find_adapters ();
      while (adapter != NULL)
      {
         printf ("Description : %s, Device to use for wpcap: %s\n", adapter->desc,adapter->name);
         adapter = adapter->next;
      }
   }

   printf("End program\n");
   return return_code;
}
