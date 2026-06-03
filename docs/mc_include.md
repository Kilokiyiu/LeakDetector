# 堆追踪模块（racking the heap）

> 学习 Valgrind `mc_include.h` 时的笔记。源码：`学习用/valgrind/memcheck/mc_include.h`

---
## 1.模块概述
#### 堆追踪模块用于追踪堆上的数据有没有被释放

## 2.代码解释
### 2.1 RedZone
```c
#define MC_MALLOC_DEFAULT_REDZONE_SZB    16
```
#### 在每个堆内存前后各设置16字节的警戒区，这个警戒区对应影子内存中的NOACCESS，表示这个16字节的区域是不可访问的

### 2.2 MC_AllocKind
```c
typedef
   enum {
      MC_AllocMalloc = 0, //malloc
      MC_AllocNew    = 1, //new
      MC_AllocNewVec = 2, //new[]
      MC_AllocCustom = 3 //自定义分配器
   }
   MC_AllocKind;
```
#### 这是一个枚举，我们讲了，c++程序，新声明的新的对象，在不使用后都需要通过delete或者其他方法将它释放掉。而 new 和 delete 必须配对，new[] 必须配 delete[]。这个枚举就是检测“用 new 分配却用 delete[]”释放这种错误，将他记录在MC_CHunk中

#### 2.3 MC_Chunk
```C
typedef
   struct _MC_Chunk {
      struct _MC_Chunk* next; // 哈希表链表指针
      Addr         data; // 实际数据的地址            
      SizeT        szB : (sizeof(SizeT)*8)-2; // 分配大小（位域压缩）
      MC_AllocKind allockind : 2; // 分配方式（位域压缩）  
      SizeT        alignB;  // 对齐要求
      ExeContext*  where[0]; // 调用栈（可变长数组）
   }
   MC_Chunk;
```
#### MC_Chunk可认为是每个堆内存的档案卡,MC_Chunk就是堆追踪模块的核心数据结构。
#### 2.3.1 `struct _MC_Chunk* next`：工具使用一个通用的哈希表VgHashNode (L135)来存储所有的堆块，每一个MC_Chunk就被挂在哈希桶的链表中，next就是链表的下一个节点的指针
#### 2.3.2 `data`：是 malloc(100) 返回给用户程序的那个指针地址。
> |<- 16B 警戒区 ->|<----- 用户数据区(100B) ------>|<- 16B 警戒区 ->|
>                             ↑
>                     data 指向这里
#### 2.3.3 `SizeT szB : (sizeof(SizeT)*8)-2` 和 `MC_AllocKind allockind : 2`：`szB`是你申请的内存的大小，`allockind`表示它的分配方式：
| 二进制 | 值 | 含义 |
|--|--|--|
| 00 | 0 | malloc()分配 |
| 01 | 1 | new分配 |
| 10 | 2 | new[]分配 |
| 11 | 3 | 自定义分配 |
> 这样分配的意义：这样当用户写了 int* p = new int[100] 但最后却用了 delete p（漏了 []），Memcheck 在 free(p) 时查到这张卡片的 allockind = 2 (NewVec)，但释放操作用的是普通 delete 而不是 delete[] → 报错！
##### 前者声明了SizeT类型的对象szB，(sizeof(SizeT)*8)-2位，表示szB占了62位，后者allockind表示占用2位，合起来总共占用64位，即8个字节。
> 为什么要确定它的位数呢？有如下两端代码：
```c
struct Normal {
    SizeT        szB;       // 8 字节
    MC_AllocKind allockind; // 枚举，通常也是 4 或 8 字节
};
// 总计：16 字节
```
```c
struct _MC_Chunk {
    SizeT        szB : (sizeof(SizeT)*8)-2;   // 占 62 位
    MC_AllocKind allockind : 2;               // 占 2 位
};
// 总计：8 字节 —— 两个变量共享这 8 个字节
```
#### 如果我们不自己定义对象的位数，那么程序默认会占用更多的空间，对于一个64位的系统，我们只分配64位的空间是完全够用的，因为2^62=4.6亿bit，这个内存远大于当今世界上最大的计算机内存(同理，分配更高的位也完全用不到)，32位系统同理，2^30=1GB左右，也是足够使用的
#### 2.3.4 `alignB`：某些分配有对齐要求（比如 SIMD 指令需要 16 或 32 字节对齐）。alignB 记录了分配时的对齐参数，用于后续的合法性检查。
#### 2.3.5 `where[0]`：记录谁、在哪一行代码申请了这块内存。
### 总结：MC_Chunk 就是解决"怎么知道堆中数据有没有被释放"这个问题的：每次分配都建卡、每次释放都标记，程序退出时遍历所有卡片，找到那些分配了但没释放的——这就是泄漏。

### 2.3 MC_Mempool
#### 概述：自定义内存池的档案卡，MC_Chunk是系统malloc的档案卡，MC_Mempool是用户自定义内存池的档案卡。对于前者，每次都会调用malloc由系统分配一块内存空间；后者是一次性向系统申请一大块内存空间，再自己从中切小块分发：
┌──────────────────────────────────────────────┐</BR>
│         一次性申请的大块（"池"）               │</BR>
│  ┌──────┬──────┬──────┬──────┬──────────┐   │</BR>
│  │ 分块1 │ 分块2 │ 分块3 │ 分块4 │  剩余空间 │   │</BR>
│  └──────┴──────┴──────┴──────┴──────────┘   │</BR>
└──────────────────────────────────────────────┘</BR>
#### 游戏引擎，数据库，网络库都用这种方式来提高效率
#### 2.3.1 `is_zeroed`：从这个池里分配出来的块，是否自动清零（类似 calloc 的效果）。True 时 Memcheck 会将分块标记为 DEFINED（已初始化），False 则标记为 UNDEFINED。
#### 2.3.2 `auto_free`: 当"块"（block）被释放时，是否自动释放块里所有的分块（chunk）。适用于"整块一起回收"的池设计。
#### 2.3.3 `metapool `: 这个池本身是用 VALGRIND_MALLOC_LIKE 申请来的，也就是说池自身的内存也是被 Memcheck 追踪的。是"池中池"的高级场景。
