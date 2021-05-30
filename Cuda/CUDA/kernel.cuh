




extern "C" {
    __declspec(dllexport) void initCuda();
    __declspec(dllexport) const char* doAdd();
    __declspec(dllexport) const char* doPreformanceTest(int* AndOldStatus, int* AndNewStatus,
        int* OrOldStatus, int* OrNewStatus,
        int* NotOldStatus, int* NotNewStatus, int arraySize, int testTimes);
}