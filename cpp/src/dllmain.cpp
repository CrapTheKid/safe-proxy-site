#include "core/export.hpp"
#include "core/project.hpp"
#include "features/example_feature.hpp"

#include <memory>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace {

void bootstrap_features() {
    auto feature = std::make_shared<safe_proxy::features::ExampleFeature>();
    auto& project = safe_proxy::core::Project::instance();
    project.feature_manager().register_feature(feature);
    project.feature_manager().enable_feature(feature->name());

    project.render_loop().register_callback([] {
        // Render callback placeholder
    });
}

} // namespace

extern "C" SAFE_PROXY_API bool SafeProxy_Initialize() {
    auto& project = safe_proxy::core::Project::instance();
    if (!project.initialize()) {
        return false;
    }

    bootstrap_features();
    return true;
}

extern "C" SAFE_PROXY_API void SafeProxy_Shutdown() {
    safe_proxy::core::Project::instance().shutdown();
}

extern "C" SAFE_PROXY_API void SafeProxy_OnFrame() {
    safe_proxy::core::Project::instance().on_frame();
}

#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    (void)module;
    (void)reserved;

    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
    } else if (reason == DLL_PROCESS_DETACH) {
        SafeProxy_Shutdown();
    }

    return TRUE;
}
#endif
