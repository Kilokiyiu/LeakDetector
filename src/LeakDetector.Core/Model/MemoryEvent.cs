namespace LeakDetector.Core.Model;

/// <summary>
/// 内存事件，对于每一个内存事件，都用一个ID来标记，后续所有的内存分析都是在这个内存事件来进行分析
/// </summary>
/// <param name="Id">The id of the memory event.</param>
public abstract record MemoryEvent(int Id);
