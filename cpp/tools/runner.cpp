#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace {

using InitializeFn = bool (*)();
using TickFn = void (*)();
using ShutdownFn = void (*)();

struct Api {
    InitializeFn initialize{};
    TickFn on_frame{};
    ShutdownFn shutdown{};
};

#ifdef _WIN32
Api load_api(HMODULE module) {
    Api api{};
    api.initialize = reinterpret_cast<InitializeFn>(GetProcAddress(module, "SafeProxy_Initialize"));
    api.on_frame = reinterpret_cast<TickFn>(GetProcAddress(module, "SafeProxy_OnFrame"));
    api.shutdown = reinterpret_cast<ShutdownFn>(GetProcAddress(module, "SafeProxy_Shutdown"));
    return api;
}
#else
Api load_api(void* module) {
    Api api{};
    api.initialize = reinterpret_cast<InitializeFn>(dlsym(module, "SafeProxy_Initialize"));
    api.on_frame = reinterpret_cast<TickFn>(dlsym(module, "SafeProxy_OnFrame"));
    api.shutdown = reinterpret_cast<ShutdownFn>(dlsym(module, "SafeProxy_Shutdown"));
    return api;
}
#endif

bool validate(const Api& api) {
    return api.initialize != nullptr && api.on_frame != nullptr && api.shutdown != nullptr;
}

} // namespace

int main(int argc, char** argv) {
    const std::filesystem::path executable_path = (argc > 0) ? std::filesystem::absolute(argv[0]) : std::filesystem::current_path();
    const std::filesystem::path executable_dir = executable_path.parent_path();

#ifdef _WIN32
    const auto library_path = (executable_dir / "safe_proxy_internal.dll").string();
    HMODULE module = LoadLibraryA(library_path.c_str());
    if (module == nullptr) {
        std::cerr << "Failed to load " << library_path << '\n';
        return 1;
    }

    Api api = load_api(module);
#else
#if __APPLE__
    const auto library_path = (executable_dir / "libsafe_proxy_internal.dylib").string();
#else
    const auto library_path = (executable_dir / "libsafe_proxy_internal.so").string();
#endif

    void* module = dlopen(library_path.c_str(), RTLD_NOW);
    if (module == nullptr) {
        std::cerr << "Failed to load " << library_path << ": " << dlerror() << '\n';
        return 1;
    }

    Api api = load_api(module);
#endif

    if (!validate(api)) {
        std::cerr << "Failed to resolve exported API symbols\n";
#ifdef _WIN32
        FreeLibrary(module);
#else
        dlclose(module);
#endif
        return 1;
    }

    if (!api.initialize()) {
        std::cerr << "SafeProxy_Initialize failed\n";
#ifdef _WIN32
        FreeLibrary(module);
#else
        dlclose(module);
#endif
        return 1;
    }

    for (int i = 0; i < 3; ++i) {
        api.on_frame();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    api.shutdown();

#ifdef _WIN32
    FreeLibrary(module);
#else
    dlclose(module);
#endif

    std::cout << "Runner finished successfully\n";
    return 0;
}
