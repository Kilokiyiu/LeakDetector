namespace LeakDetector.Core.Model;

/// <summary>
/// 释放事件，表示释放了一个内存，包括释放的指针、类型等信息
/// </summary>
/// <param name="Id">The id of the memory event.</param>
/// <param name="Ptr">The pointer of the freed memory.</param>
/// <param name="Kind">The kind of the freed memory.</param>
public sealed record FreeEvent(
    int Id,
    string Ptr,
    string Kind) : MemoryEvent(Id);
