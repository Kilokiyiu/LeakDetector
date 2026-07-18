namespace LeakDetector.Core.Model;

public sealed record FreeEvent(
    int Id,
    string Ptr,
    string Kind) : MemoryEvent(Id);
