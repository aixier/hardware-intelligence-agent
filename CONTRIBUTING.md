# Contributing to Hardware Intelligence Agent 🤝

Thank you for your interest in contributing to Hardware Intelligence Agent! This document provides guidelines and information for contributors.

## 🚀 Ways to Contribute

### 🐛 Bug Reports
- Use the [bug report template](.github/ISSUE_TEMPLATE/bug_report.md)
- Include detailed reproduction steps
- Provide system and hardware information
- Attach logs and relevant outputs

### 💡 Feature Requests
- Use the [feature request template](.github/ISSUE_TEMPLATE/feature_request.md)
- Explain the use case and benefits
- Provide mockups or examples if applicable

### 📖 Documentation
- Fix typos and improve clarity
- Add missing documentation
- Translate content to other languages
- Create tutorials and guides

### 💻 Code Contributions
- Implement new features
- Fix bugs and issues
- Improve performance
- Add tests and examples

## 🛠️ Development Setup

### Prerequisites
- Git
- CSK6 development board (recommended)
- lisa-zep development tools
- C/C++ development environment

### Local Setup
```bash
# Fork and clone your fork
git clone https://github.com/yourusername/hardware-intelligence-agent.git
cd hardware-intelligence-agent

# Add upstream remote
git remote add upstream https://github.com/original-owner/hardware-intelligence-agent.git

# Create development branch
git checkout -b feature/your-feature-name
```

### Building and Testing
```bash
# Install dependencies
lisa zep install

# Build examples
cd examples/basic
lisa zep build

# Run tests (if available)
lisa zep test
```

## 📝 Coding Standards

### C/C++ Style Guide
- Use 4 spaces for indentation
- Follow naming conventions:
  - Functions: `snake_case`
  - Variables: `camelCase`
  - Constants: `UPPER_CASE`
  - Files: `snake_case.c/.h`

### Code Organization
```c
/*
 * File header with description
 * Author: Your Name
 * License: MIT
 */

#include <system_headers.h>
#include "local_headers.h"

// Constants and macros
#define MAX_BUFFER_SIZE 1024

// Type definitions
typedef struct {
    int field1;
    char field2[32];
} example_struct_t;

// Function declarations
int example_function(int param);

// Implementation
int example_function(int param)
{
    // Function body with clear comments
    return param * 2;
}
```

### Documentation Standards
- Document all public APIs
- Use clear, concise comments
- Include usage examples
- Update README when adding features

## 🔄 Pull Request Process

### Before Submitting
1. **Update your fork**:
   ```bash
   git fetch upstream
   git rebase upstream/main
   ```

2. **Test your changes**:
   - Build successfully
   - Run existing tests
   - Test on actual hardware if possible

3. **Check code style**:
   - Follow coding standards
   - Use consistent formatting
   - Add appropriate comments

### PR Guidelines
- Create a clear, descriptive title
- Fill out the PR template completely
- Include screenshots/videos for UI changes
- Link related issues using "Fixes #123"
- Keep changes focused and atomic

### Review Process
1. Automated checks must pass
2. At least one maintainer review required
3. Address feedback promptly
4. Maintainer will merge when approved

## 🏷️ Commit Message Format

Use conventional commits for better automation:

```
type(scope): short description

Optional longer description explaining the change in detail.

Fixes #123
```

### Types
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code refactoring
- `test`: Adding tests
- `chore`: Maintenance tasks

### Examples
```
feat(audio): add noise reduction algorithm

Implement adaptive noise reduction for audio processing
with configurable sensitivity levels.

Fixes #45

docs(readme): update installation instructions

Add Windows-specific setup steps and troubleshooting
section for common installation issues.

fix(gpio): resolve interrupt handling race condition

Fix race condition in GPIO interrupt handler that caused
missed events under high load conditions.

Fixes #67
```

## 🧪 Testing Guidelines

### Unit Tests
- Write tests for new functions
- Maintain >80% code coverage
- Use descriptive test names
- Test edge cases and error conditions

### Integration Tests
- Test complete workflows
- Use actual hardware when possible
- Document test hardware requirements
- Provide test data and expected outputs

### Example Test Structure
```c
#include <unity.h>
#include "example_module.h"

void setUp(void) {
    // Setup before each test
}

void tearDown(void) {
    // Cleanup after each test
}

void test_example_function_normal_case(void) {
    int result = example_function(5);
    TEST_ASSERT_EQUAL(10, result);
}

void test_example_function_edge_case(void) {
    int result = example_function(0);
    TEST_ASSERT_EQUAL(0, result);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_example_function_normal_case);
    RUN_TEST(test_example_function_edge_case);
    return UNITY_END();
}
```

## 🌍 Community Guidelines

### Be Respectful
- Use inclusive language
- Be patient with beginners
- Provide constructive feedback
- Respect different perspectives

### Communication Channels
- **GitHub Issues**: Bug reports, feature requests
- **GitHub Discussions**: General questions, ideas
- **Discord**: Real-time chat and collaboration
- **Email**: security@hardware-intelligence.dev for security issues

### Getting Help
- Check existing issues and documentation first
- Provide enough context in your questions
- Be patient - maintainers are volunteers
- Consider offering help to others

## 🎯 Contribution Focus Areas

### High Priority
- Hardware compatibility improvements
- Performance optimizations
- Documentation and examples
- Bug fixes and stability

### Medium Priority
- New AI model support
- Additional hardware peripherals
- Developer tooling improvements
- Internationalization

### Future Considerations
- Advanced IDE integration
- Visual programming interface
- Cloud service integrations
- Mobile app connectivity

## 🏆 Recognition

Contributors will be recognized in:
- README contributors section
- Release notes
- Hall of fame on project website
- Special contributor badges

### Contribution Levels
- **Contributor**: Any accepted PR
- **Regular Contributor**: 5+ merged PRs
- **Core Contributor**: Significant feature additions
- **Maintainer**: Long-term project stewardship

## 📞 Contact

- **General Questions**: [GitHub Discussions](https://github.com/yourusername/hardware-intelligence-agent/discussions)
- **Bug Reports**: [GitHub Issues](https://github.com/yourusername/hardware-intelligence-agent/issues)
- **Security Issues**: security@hardware-intelligence.dev
- **Maintainer Contact**: maintainers@hardware-intelligence.dev

---

Thank you for contributing to Hardware Intelligence Agent! Together we're building the future of intelligent embedded systems. 🚀