#include <napi.h>
#include "agent_zero/agent_core.hpp"
#include "agent_zero/memory_manager.hpp"
#include "agent_zero/multi_lang_bridge.hpp"

using namespace agent_zero;

// Agent wrapper for Node.js
class AgentWrapper : public Napi::ObjectWrap<AgentWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports) {
        Napi::Function func = DefineClass(env, "Agent", {
            InstanceMethod("processMessage", &AgentWrapper::ProcessMessage),
            InstanceMethod("setContext", &AgentWrapper::SetContext),
            InstanceMethod("getState", &AgentWrapper::GetState),
            InstanceAccessor("name", &AgentWrapper::GetName, &AgentWrapper::SetName),
            InstanceAccessor("model", &AgentWrapper::GetModel, &AgentWrapper::SetModel)
        });
        
        Napi::FunctionReference* constructor = new Napi::FunctionReference();
        *constructor = Napi::Persistent(func);
        env.SetInstanceData(constructor);
        
        exports.Set("Agent", func);
        return exports;
    }
    
    AgentWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<AgentWrapper>(info) {
        Napi::Env env = info.Env();
        
        if (info.Length() < 2) {
            Napi::TypeError::New(env, "Expected name and model").ThrowAsJavaScriptException();
            return;
        }
        
        std::string name = info[0].As<Napi::String>();
        std::string model = info[1].As<Napi::String>();
        
        agent_ = std::make_unique<Agent>(name, model);
    }
    
private:
    Napi::Value ProcessMessage(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        
        if (info.Length() < 1) {
            Napi::TypeError::New(env, "Expected message").ThrowAsJavaScriptException();
            return env.Null();
        }
        
        std::string message = info[0].As<Napi::String>();
        std::string response = agent_->process_message(message);
        
        return Napi::String::New(env, response);
    }
    
    Napi::Value SetContext(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        
        if (info.Length() < 1 || !info[0].IsObject()) {
            Napi::TypeError::New(env, "Expected object").ThrowAsJavaScriptException();
            return env.Undefined();
        }
        
        Napi::Object obj = info[0].As<Napi::Object>();
        std::map<std::string, std::string> context;
        
        Napi::Array keys = obj.GetPropertyNames();
        for (uint32_t i = 0; i < keys.Length(); i++) {
            Napi::Value key = keys[i];
            std::string keyStr = key.As<Napi::String>();
            std::string valueStr = obj.Get(key).As<Napi::String>();
            context[keyStr] = valueStr;
        }
        
        agent_->set_context(context);
        return env.Undefined();
    }
    
    Napi::Value GetState(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        return Napi::Number::New(env, static_cast<int>(agent_->get_state()));
    }
    
    Napi::Value GetName(const Napi::CallbackInfo& info) {
        return Napi::String::New(info.Env(), agent_->get_name());
    }
    
    void SetName(const Napi::CallbackInfo& info, const Napi::Value& value) {
        agent_->set_name(value.As<Napi::String>());
    }
    
    Napi::Value GetModel(const Napi::CallbackInfo& info) {
        return Napi::String::New(info.Env(), agent_->get_model());
    }
    
    void SetModel(const Napi::CallbackInfo& info, const Napi::Value& value) {
        agent_->set_model(value.As<Napi::String>());
    }
    
    std::unique_ptr<Agent> agent_;
};

// Module initialization
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    AgentWrapper::Init(env, exports);
    
    // Export version
    exports.Set("version", Napi::String::New(env, "0.9.6"));
    
    return exports;
}

NODE_API_MODULE(agent_zero_native, Init)
