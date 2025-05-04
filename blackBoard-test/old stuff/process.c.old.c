/*
 * process.c
 *
 *  Created on: May 2, 2025
 *      Author: spencer
 */

#include "process.h"
#include <string.h>
#include <stdio.h>

/* ---------- static buffer pool ------------------------------------- */
static float  bufPool [MAX_BUFFERS][VECTOR_SIZE];
static bool   bufUsed [MAX_BUFFERS];
static int8_t freeStack[MAX_BUFFERS];
static int    freeTop, nextBuf;

/* map numeric node‑ID to buffer index */
static int8_t nodeBufId[MAX_NODES];

/* -------------------------------------------------- helpers -------- */
static int8_t allocBuf(void)
{
    if (freeTop) return freeStack[--freeTop];
    if (nextBuf < MAX_BUFFERS) return nextBuf++;
    while (1);               /* fatal – out of buffers */
}
static void freeBuf(int8_t id)
{
    if (id<0) return;
    bufUsed[id]=false;
    freeStack[freeTop++] = id;
}

/* small in‑place effects (demo only) -------------------------------- */
static inline void effAdd(const float* in,float* out,int p)
{ for (int i=0;i<VECTOR_SIZE;i++) out[i]=in[i]+p; }
static inline void effSub(const float* in,float* out,int p)
{ for (int i=0;i<VECTOR_SIZE;i++) out[i]=in[i]-p; }
static inline void effMul(const float* in,float* out,int p)
{ for (int i=0;i<VECTOR_SIZE;i++) out[i]=in[i]*p; }
static inline void effDiv(const float* in,float* out,int p)
{ for (int i=0;i<VECTOR_SIZE;i++) out[i]= p?in[i]/p:0.f; }

static void effMix(const float* in[],const uint8_t* w,uint8_t n,float* out)
{
    for (int i=0;i<VECTOR_SIZE;i++){
        float acc=0,ws=0;
        for(int j=0;j<n;j++){ float wt=w[j]==255?1:w[j]; acc+=in[j][i]*wt; ws+=wt;}
        out[i]= ws?acc/ws:0;
    }
}

/* ------------------------------------------------ topo sort -------- */
static uint8_t topo(uint8_t order[],const dataLink* net,uint8_t indeg[])
{
    memset(indeg,0,MAX_NODES);
    for(uint8_t i=0;i<net->nodeCount;i++)
        for(uint8_t j=0;j<net->nodes[i].numInputs;j++)
            indeg[i]++;

    uint8_t Q[MAX_NODES],h=0,t=0,nout=0;
    for(uint8_t i=0;i<net->nodeCount;i++) if(!indeg[i]) Q[t++]=i;

    while(h<t){
        uint8_t n=Q[h++]; order[nout++]=n;
        uint8_t id=net->nodes[n].nodeId;
        for(uint8_t c=0;c<net->nodeCount;c++)
            for(uint8_t j=0;j<net->nodes[c].numInputs;j++)
                if(net->nodes[c].inputs[j]==id)
                    if(--indeg[c]==0) Q[t++]=c;
    }
    return nout;
}

/* ------------------------------------------------ main runner ------ */
void processNetwork(const dataLink* net,
                    const float* sysIn0,const float* sysIn1,
                    uint8_t len,float* out2,float* out3)
{
    if(len>VECTOR_SIZE) len=VECTOR_SIZE;

    memset(bufUsed,0,sizeof(bufUsed));
    nextBuf=freeTop=0;
    for(int i=0;i<MAX_NODES;i++) nodeBufId[i]=-1;

    /* seed system inputs node‑0 and node‑1 */
    int8_t b0=allocBuf(); memcpy(bufPool[b0],sysIn0,len*sizeof(float));
    int8_t b1=allocBuf(); memcpy(bufPool[b1],sysIn1,len*sizeof(float));
    nodeBufId[0]=b0; nodeBufId[1]=b1;

    /* build consumer ref‑counts */
    uint8_t need[MAX_NODES]={0};
    for(uint8_t i=0;i<net->nodeCount;i++)
        for(uint8_t j=0;j<net->nodes[i].numInputs;j++){
            uint8_t pid = net->nodes[i].inputs[j];
            if(pid<MAX_NODES) need[pid]++;
        }

    /* topological order */
    uint8_t order[MAX_NODES];
    uint8_t N=topo(order,net,need); (void)N; /* assume DAG */

    /* execute nodes */
    for(uint8_t k=0;k<net->nodeCount;k++){
        const node* nd=&net->nodes[ order[k] ];
        if(nd->effectId==255) continue;          /* placeholder */

        /* collect input ptrs */
        const float* in[4]={0};
        for(uint8_t j=0;j<nd->numInputs;j++)
            in[j]=bufPool[ nodeBufId[ nd->inputs[j] ] ];

        int8_t outB=allocBuf();
        switch(nd->effectId){
            case 0: effAdd(in[0],bufPool[outB],nd->params[0]); break;
            case 1: effSub(in[0],bufPool[outB],nd->params[0]); break;
            case 2: effMul(in[0],bufPool[outB],nd->params[0]); break;
            case 3: effDiv(in[0],bufPool[outB],nd->params[0]); break;
            case 6: effMix(in,nd->params,nd->numInputs,bufPool[outB]); break;
            default: memcpy(bufPool[outB],in[0],len*sizeof(float)); break;
        }
        nodeBufId[ nd->nodeId ] = outB;

        /* release parents */
        for(uint8_t j=0;j<nd->numInputs;j++){
            uint8_t pid = nd->inputs[j];
            if(pid<MAX_NODES && --need[pid]==0)
                freeBuf( nodeBufId[pid] );
        }
    }

    if(out2) memcpy(out2,bufPool[ nodeBufId[2] ],len*sizeof(float));
    if(out3) memcpy(out3,bufPool[ nodeBufId[3] ],len*sizeof(float));
}
