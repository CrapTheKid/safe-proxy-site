# safe_proxy_internal

Minimal modular C++ internal project scaffold.

## Build

### Linux/macOS (shared object)
```bash
cmake -S cpp -B cpp/build -DCMAKE_BUILD_TYPE=Release
cmake --build cpp/build -j
```

Output library:
- `cpp/build/libsafe_proxy_internal.so` (Linux)
- `cpp/build/libsafe_proxy_internal.dylib` (macOS)

### Windows (DLL)
```powershell
cmake -S cpp -B cpp/build -G "Visual Studio 17 2022" -A x64
cmake --build cpp/build --config Release
```

Output library:
- `cpp/build/Release/safe_proxy_internal.dll`

## Run (quick local check)

This repo includes a tiny runner (`safe_proxy_runner`) that dynamically loads the built library and calls:
- `SafeProxy_Initialize`
- `SafeProxy_OnFrame`
- `SafeProxy_Shutdown`

### Build runner
```bash
cmake -S cpp -B cpp/build -DCMAKE_BUILD_TYPE=Release -DSAFE_PROXY_BUILD_RUNNER=ON
cmake --build cpp/build -j
```

### Run runner
```bash
./cpp/build/safe_proxy_runner
```

Expected output includes logger lines (init/feature/frame/shutdown).

## Use from your own injector/host

The DLL/SO exports:
- `bool SafeProxy_Initialize()`
- `void SafeProxy_OnFrame()`
- `void SafeProxy_Shutdown()`

Call `SafeProxy_Initialize()` once, call `SafeProxy_OnFrame()` every frame/tick, and call `SafeProxy_Shutdown()` before unload.
