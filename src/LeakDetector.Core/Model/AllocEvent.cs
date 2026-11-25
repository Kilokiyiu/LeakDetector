namespace LeakDetector.Core.Model;


/// <summary>
/// 分配事件，表示给某个程序分配了一个内存，包括分配的指针、大小、类型、文件、行号等信息
/// </summary>
/// <param name="Id">The id of the memory event.</param>
/// <param name="Ptr">The pointer of the allocated memory.</param>
/// <param name="Size">The size of the allocated memory.</param>
/// <param name="Kind">The kind of the allocated memory.</param>
/// <param name="File">The file of the allocated memory.</param>
/// <param name="Line">The line of the allocated memory.</param>
public sealed record AllocEvent(
    int Id,
    string Ptr,
    ulong Size,
    string Kind,
    string File,
    int Line) : MemoryEvent(Id)
{
    public bool IsArray => Kind == "new[]";
}
