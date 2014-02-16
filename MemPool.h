/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file MemPool.h
 * @author liujun05(com@baidu.com)
 * @date 2014/02/14 15:11:42
 * @brief 
 *  
 **/




#ifndef  __MEMPOOL_H_
#define  __MEMPOOL_H_

#include <cstdio>
#include <vector>
#include <string>
using namespace std;


#define BLOCK_SIZE 102400
#define SLICE_SIZE 1024
#define BLOCK_NUM 1024

#ifndef UNSIGNED_LONG
#define uint32 unsigned int
#endif

#ifndef UNSIGNED_SHORT
#define uint16 unsigned short
#endif

typedef struct mempool_key_t{
    uint32 block_index; //block块的index索引
    uint32 slice_index; //slice片的index偏移
}mempool_key_t;


typedef struct _block_t{
    int block_index;    //block索引
    int slice_num;      //block片数目
    int slice_alloc;    //可分配的片数目
    int block_size;     //block总大小
}block_t;


class MemPool{

private:
    uint32 m_max_block_size;    //最大block大小
    uint32 m_max_slice_size;    //最大slick大小
    uint32 m_max_block_num;     //最大有多少个block
    uint32 m_max_slice_num;     //一个block最大有多少个slice
    vector< block_t* > m_blocks; //全量的blocks
    vector< block_t* > m_work_blocks;   //正在使用的block   
    vector< mempool_key_t* > m_free_block_nodes;    //已经被删除或者可以被重复利用的block节点
public:
    MemPool(uint32 block_size, uint32 block_num, uint32 slice_num); //构造函数
    mempool_key_t alloc_mem(uint32 size);   //分配大小为size的内容
    int free_mem(mempool_key_t key);        //删除内容
    void* addr(mempool_key_t key);          //获得内存地址
    ~MemPool();                             //是析构
};


//非法内存地址
mempool_key_t INVALID_MEM_KEY = {(uint16)(-1), (uint16)(-1)};

#endif  //__MEMPOOL_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
