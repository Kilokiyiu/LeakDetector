namespace LeakDetector.Core.Model;

public sealed record LeakEntry(
    string Ptr,
    ulong Size,
    string File,
    int Line,
    bool IsArray);
