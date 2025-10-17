// Agent Zero Native Node.js Module
const agentZeroNative = require('./build/Release/agent_zero_native.node');

module.exports = agentZeroNative;

// Example usage (can be tested separately):
if (require.main === module) {
    const { Agent } = agentZeroNative;
    
    console.log('=== Agent Zero Node.js Bindings ===');
    console.log('Version:', agentZeroNative.version);
    
    const agent = new Agent('NodeAgent', 'gpt-4');
    console.log('Created agent:', agent.name, 'using model:', agent.model);
    
    const response = agent.processMessage('Hello from Node.js!');
    console.log('Response:', response);
    
    agent.setContext({ user: 'test', session: '123' });
    const response2 = agent.processMessage('What is my session?');
    console.log('Response with context:', response2);
}
