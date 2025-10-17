#ifndef AGENT_ZERO_MULTI_LANG_BRIDGE_HPP
#define AGENT_ZERO_MULTI_LANG_BRIDGE_HPP

#include <string>
#include <memory>
#include <map>
#include <functional>

namespace agent_zero {

// Language types supported
enum class Language {
    PYTHON,
    JAVASCRIPT,
    GO,
    RUST,
    RUBY,
    NATIVE_CPP
};

// Language bridge result
struct BridgeResult {
    bool success;
    std::string result;
    std::string error;
    Language language;
};

// Multi-language bridge for cross-language communication
class MultiLangBridge {
public:
    MultiLangBridge();
    ~MultiLangBridge();
    
    // Execute code in different languages
    BridgeResult execute_python(const std::string& code, 
                                const std::map<std::string, std::string>& vars = {});
    BridgeResult execute_javascript(const std::string& code,
                                    const std::map<std::string, std::string>& vars = {});
    BridgeResult execute_go(const std::string& code,
                           const std::map<std::string, std::string>& vars = {});
    
    // Generic execution interface
    BridgeResult execute(Language lang, const std::string& code,
                        const std::map<std::string, std::string>& vars = {});
    
    // Check language availability
    bool is_language_available(Language lang) const;
    std::vector<Language> get_available_languages() const;
    
    // Register language interpreters
    using LanguageExecutor = std::function<BridgeResult(const std::string&, 
                                                        const std::map<std::string, std::string>&)>;
    void register_language_executor(Language lang, LanguageExecutor executor);
    
private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace agent_zero

#endif // AGENT_ZERO_MULTI_LANG_BRIDGE_HPP
