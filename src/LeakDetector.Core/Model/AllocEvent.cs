namespace LeakDetector.Core.Model;

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
