#include "agent_zero/multi_lang_bridge.hpp"
#include <stdexcept>
#include <sstream>

namespace agent_zero {

// PIMPL implementation
class MultiLangBridge::Impl {
public:
    std::map<Language, LanguageExecutor> executors;
    
    Impl() {
        // Register default executors (mock implementations)
        // In a real implementation, these would interface with actual language runtimes
        
        executors[Language::NATIVE_CPP] = [](const std::string& code, 
                                             const std::map<std::string, std::string>& vars) {
            BridgeResult result;
            result.success = true;
            result.result = "C++ code execution: " + code;
            result.language = Language::NATIVE_CPP;
            return result;
        };
    }
};

MultiLangBridge::MultiLangBridge()
    : pimpl_(std::make_unique<Impl>())
{
}

MultiLangBridge::~MultiLangBridge() = default;

BridgeResult MultiLangBridge::execute_python(const std::string& code,
                                            const std::map<std::string, std::string>& vars) {
    return execute(Language::PYTHON, code, vars);
}

BridgeResult MultiLangBridge::execute_javascript(const std::string& code,
                                                const std::map<std::string, std::string>& vars) {
    return execute(Language::JAVASCRIPT, code, vars);
}

BridgeResult MultiLangBridge::execute_go(const std::string& code,
                                        const std::map<std::string, std::string>& vars) {
    return execute(Language::GO, code, vars);
}

BridgeResult MultiLangBridge::execute(Language lang, const std::string& code,
                                     const std::map<std::string, std::string>& vars) {
    auto it = pimpl_->executors.find(lang);
    if (it != pimpl_->executors.end()) {
        try {
            return it->second(code, vars);
        } catch (const std::exception& e) {
            BridgeResult result;
            result.success = false;
            result.error = e.what();
            result.language = lang;
            return result;
        }
    }
    
    BridgeResult result;
    result.success = false;
    result.error = "Language not available";
    result.language = lang;
    return result;
}

bool MultiLangBridge::is_language_available(Language lang) const {
    return pimpl_->executors.find(lang) != pimpl_->executors.end();
}

std::vector<Language> MultiLangBridge::get_available_languages() const {
    std::vector<Language> langs;
    for (const auto& [lang, executor] : pimpl_->executors) {
        langs.push_back(lang);
    }
    return langs;
}

void MultiLangBridge::register_language_executor(Language lang, LanguageExecutor executor) {
    pimpl_->executors[lang] = executor;
}

} // namespace agent_zero
