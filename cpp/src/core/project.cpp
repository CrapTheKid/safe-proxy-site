#include "core/project.hpp"

#include "core/logger.hpp"

namespace safe_proxy::core {

Project& Project::instance() {
    static Project project;
    return project;
}

bool Project::initialize() {
    Logger::instance().initialize();

    if (!hook_manager_.initialize()) {
        SP_LOG_ERROR("Failed to initialize hook manager");
        return false;
    }

    running_.store(true);
    SP_LOG_INFO("Project initialized");
    return true;
}

void Project::shutdown() {
    if (!running_.exchange(false)) {
        return;
    }

    feature_manager_.clear();
    render_loop_.clear_callbacks();
    hook_manager_.shutdown();
    Logger::instance().shutdown();
}

void Project::on_frame() {
    if (!running_.load()) {
        return;
    }

    hook_manager_.dispatch();
    feature_manager_.on_frame();
    render_loop_.on_frame();
}

} // namespace safe_proxy::core
