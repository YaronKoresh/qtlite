#include "snippetmanager.h"

SnippetManager::SnippetManager() {
    initializeSnippets();
}

QString SnippetManager::getSnippet(const QString& name) {
    for (const auto& snippetList : std::as_const(languageSnippets)) {
        if (snippetList.contains(name)) {
            return getSnippetContent(name);
        }
    }
    return "";
}

QStringList SnippetManager::getSnippetNames(const QString& language) {
    return languageSnippets.value(language, QStringList());
}

QStringList SnippetManager::getAllLanguages() {
    return languageSnippets.keys();
}

QString SnippetManager::getSnippetContent(const QString& name) {
    if (name == "C++ Main") {
        return "#include <iostream>\n\n"
               "int main(int argc, char *argv[]) {\n"
               "    std::cout << \"Hello, World!\" << std::endl;\n"
               "    return 0;\n"
               "}\n";
    }
    else if (name == "C Main") {
        return "#include <stdio.h>\n\n"
               "int main(int argc, char *argv[]) {\n"
               "    printf(\"Hello, World!\\n\");\n"
               "    return 0;\n"
               "}\n";
    }
    else if (name == "Python Main") {
        return "def main():\n"
               "    print(\"Hello, World!\")\n\n"
               "if __name__ == \"__main__\":\n"
               "    main()\n";
    }
    else if (name == "For Loop") {
        return "for(int i = 0; i < 10; ++i) {\n"
               "    \n"
               "}\n";
    }
    else if (name == "Python For Loop") {
        return "for i in range(10):\n"
               "    pass\n";
    }
    else if (name == "Python If/Else") {
        return "if True:\n"
               "    pass\n"
               "else:\n"
               "    pass\n";
    }
    else if (name == "If/Else") {
        return "if (true) {\n"
               "    \n"
               "} else {\n"
               "    \n"
               "}\n";
    }
    else if (name == "C++ Class") {
        return "class MyClass {\n"
               "public:\n"
               "    MyClass() {}\n"
               "    ~MyClass() {}\n"
               "\n"
               "private:\n"
               "\n"
               "};\n";
    }
    else if (name == "Python Function") {
        return "def my_function(param1, param2):\n"
               "    return None\n";
    }
    else if (name == "Python Class") {
        return "class MyClass:\n"
               "    def __init__(self):\n"
               "        pass\n"
               "\n"
               "    def method(self):\n"
               "        pass\n";
    }
    else if (name == "Python Try/Except") {
        return "try:\n"
               "    pass\n"
               "except Exception as e:\n"
               "    print(f\"Error: {e}\")\n";
    }
    else if (name == "C++ Struct") {
        return "struct MyStruct {\n"
               "    int member1;\n"
               "    double member2;\n"
               "};\n";
    }
    else if (name == "C++ Template") {
        return "template<typename T>\n"
               "T max(T a, T b) {\n"
               "    return (a > b) ? a : b;\n"
               "}\n";
    }
    else if (name == "Python List Comprehension") {
        return "result = [x for x in range(10) if x % 2 == 0]\n";
    }
    else if (name == "Python Dictionary") {
        return "my_dict = {\n"
               "    'key1': 'value1',\n"
               "    'key2': 'value2',\n"
               "}\n";
    }
    return "";
}

void SnippetManager::initializeSnippets() {
    QStringList cppQtSnippets;
    cppQtSnippets << "C++ Main" << "For Loop" << "If/Else"
                << "C++ Class" << "C++ Struct" << "C++ Template";
    languageSnippets["C++ (Qt)"] = cppQtSnippets;

    QStringList cCppSnippets;
    cCppSnippets << "C++ Main" << "C Main" << "For Loop" << "If/Else"
                << "C++ Class" << "C++ Struct" << "C++ Template";
    languageSnippets["C/C++ (Console)"] = cCppSnippets;

    QStringList pythonSnippets;
    pythonSnippets << "Python Main" << "Python For Loop" << "Python If/Else"
                   << "Python Function" << "Python Class" << "Python Try/Except"
                   << "Python List Comprehension" << "Python Dictionary";
    languageSnippets["Python"] = pythonSnippets;
}
