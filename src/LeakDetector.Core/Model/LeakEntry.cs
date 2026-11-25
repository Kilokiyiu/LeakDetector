namespace LeakDetector.Core.Model;

/// <summary>
/// 泄漏条目，表示一个泄漏的条目，包括泄漏的指针、大小、文件、行号、是否是数组等信息
/// 当程序使用了一段内存，没有回收，分析出有泄漏时，会创建一个泄漏条目，用于存储泄漏的指针、大小、文件、行号、是否是数组等信息，
/// 用于后续的分析，这里需要和内存事件进行区分，内存事件是程序使用了一段内存，
/// 而泄漏条目是程序使用了一段内存，没有回收，分析出有泄漏时，会创建一个泄漏条目，
/// 用于存储泄漏的指针、大小、文件、行号、是否是数组等信息，用于后续的分析
/// </summary>
/// <param name="Ptr">The pointer of the leak.</param>
/// <param name="Size">The size of the leak.</param>
/// <param name="File">The file of the leak.</param>
/// <param name="Line">The line of the leak.</param>
/// <param name="IsArray">Whether the leak is an array.</param>
public sealed record LeakEntry(
    string Ptr,
    ulong Size,
    string File,
    int Line,
    bool IsArray);
