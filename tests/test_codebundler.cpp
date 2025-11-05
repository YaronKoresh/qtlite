#include <QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include "../src/core/codebundler.h"

class TestCodeBundler : public QObject
{
    Q_OBJECT

private slots:
    void testBundleSimpleFile();
    void testBundleWithLocalIncludes();
    void testBundleWithSystemIncludes();
    void testBundleWithCircularIncludes();
};

void TestCodeBundler::testBundleSimpleFile()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    QString mainFilePath = tempDir.path() + "/main.cpp";
    QFile mainFile(mainFilePath);
    QVERIFY(mainFile.open(QIODevice::WriteOnly | QIODevice::Text));
    
    QTextStream out(&mainFile);
    out << "#include <iostream>\n";
    out << "\n";
    out << "int main() {\n";
    out << "    std::cout << \"Hello, World!\" << std::endl;\n";
    out << "    return 0;\n";
    out << "}\n";
    mainFile.close();
    
    CodeBundler bundler;
    QString outputPath;
    bool success = bundler.bundleCode(mainFilePath, outputPath);
    
    QVERIFY(success);
    QVERIFY(QFile::exists(outputPath));
    
    QFile bundledFile(outputPath);
    QVERIFY(bundledFile.open(QIODevice::ReadOnly | QIODevice::Text));
    QString content = bundledFile.readAll();
    bundledFile.close();
    
    QVERIFY(content.contains("#include <iostream>"));
    QVERIFY(content.contains("int main()"));
}

void TestCodeBundler::testBundleWithLocalIncludes()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    // Create a header file
    QString headerPath = tempDir.path() + "/myheader.h";
    QFile headerFile(headerPath);
    QVERIFY(headerFile.open(QIODevice::WriteOnly | QIODevice::Text));
    
    QTextStream headerOut(&headerFile);
    headerOut << "#ifndef MYHEADER_H\n";
    headerOut << "#define MYHEADER_H\n";
    headerOut << "\n";
    headerOut << "void myFunction();\n";
    headerOut << "\n";
    headerOut << "#endif\n";
    headerFile.close();
    
    // Create main file that includes the header
    QString mainFilePath = tempDir.path() + "/main.cpp";
    QFile mainFile(mainFilePath);
    QVERIFY(mainFile.open(QIODevice::WriteOnly | QIODevice::Text));
    
    QTextStream out(&mainFile);
    out << "#include <iostream>\n";
    out << "#include \"myheader.h\"\n";
    out << "\n";
    out << "int main() {\n";
    out << "    myFunction();\n";
    out << "    return 0;\n";
    out << "}\n";
    mainFile.close();
    
    CodeBundler bundler;
    QString outputPath;
    bool success = bundler.bundleCode(mainFilePath, outputPath);
    
    QVERIFY(success);
    QVERIFY(QFile::exists(outputPath));
    
    QFile bundledFile(outputPath);
    QVERIFY(bundledFile.open(QIODevice::ReadOnly | QIODevice::Text));
    QString content = bundledFile.readAll();
    bundledFile.close();
    
    QVERIFY(content.contains("#include <iostream>"));
    QVERIFY(content.contains("void myFunction()"));
    QVERIFY(content.contains("Start of bundled file: myheader.h"));
}

void TestCodeBundler::testBundleWithSystemIncludes()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    QString mainFilePath = tempDir.path() + "/main.cpp";
    QFile mainFile(mainFilePath);
    QVERIFY(mainFile.open(QIODevice::WriteOnly | QIODevice::Text));
    
    QTextStream out(&mainFile);
    out << "#include <iostream>\n";
    out << "#include <vector>\n";
    out << "#include <string>\n";
    out << "\n";
    out << "int main() {\n";
    out << "    std::vector<int> v;\n";
    out << "    return 0;\n";
    out << "}\n";
    mainFile.close();
    
    CodeBundler bundler;
    QString outputPath;
    bool success = bundler.bundleCode(mainFilePath, outputPath);
    
    QVERIFY(success);
    
    QFile bundledFile(outputPath);
    QVERIFY(bundledFile.open(QIODevice::ReadOnly | QIODevice::Text));
    QString content = bundledFile.readAll();
    bundledFile.close();
    
    // System includes should appear only once
    int iostreamCount = content.count("#include <iostream>");
    int vectorCount = content.count("#include <vector>");
    int stringCount = content.count("#include <string>");
    
    QCOMPARE(iostreamCount, 1);
    QCOMPARE(vectorCount, 1);
    QCOMPARE(stringCount, 1);
}

void TestCodeBundler::testBundleWithCircularIncludes()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    // Create header A that includes B
    QString headerAPath = tempDir.path() + "/headerA.h";
    QFile headerAFile(headerAPath);
    QVERIFY(headerAFile.open(QIODevice::WriteOnly | QIODevice::Text));
    
    QTextStream headerAOut(&headerAFile);
    headerAOut << "#ifndef HEADERA_H\n";
    headerAOut << "#define HEADERA_H\n";
    headerAOut << "\n";
    headerAOut << "#include \"headerB.h\"\n";
    headerAOut << "void functionA();\n";
    headerAOut << "\n";
    headerAOut << "#endif\n";
    headerAFile.close();
    
    // Create header B that includes A (circular)
    QString headerBPath = tempDir.path() + "/headerB.h";
    QFile headerBFile(headerBPath);
    QVERIFY(headerBFile.open(QIODevice::WriteOnly | QIODevice::Text));
    
    QTextStream headerBOut(&headerBFile);
    headerBOut << "#ifndef HEADERB_H\n";
    headerBOut << "#define HEADERB_H\n";
    headerBOut << "\n";
    headerBOut << "#include \"headerA.h\"\n";
    headerBOut << "void functionB();\n";
    headerBOut << "\n";
    headerBOut << "#endif\n";
    headerBFile.close();
    
    // Create main file
    QString mainFilePath = tempDir.path() + "/main.cpp";
    QFile mainFile(mainFilePath);
    QVERIFY(mainFile.open(QIODevice::WriteOnly | QIODevice::Text));
    
    QTextStream out(&mainFile);
    out << "#include \"headerA.h\"\n";
    out << "\n";
    out << "int main() {\n";
    out << "    functionA();\n";
    out << "    return 0;\n";
    out << "}\n";
    mainFile.close();
    
    CodeBundler bundler;
    QString outputPath;
    bool success = bundler.bundleCode(mainFilePath, outputPath);
    
    QVERIFY(success);
    
    QFile bundledFile(outputPath);
    QVERIFY(bundledFile.open(QIODevice::ReadOnly | QIODevice::Text));
    QString content = bundledFile.readAll();
    bundledFile.close();
    
    // Should contain markers for skipping already included files
    QVERIFY(content.contains("Skipped already included file") || 
            (content.contains("functionA") && content.contains("functionB")));
}

QTEST_MAIN(TestCodeBundler)
#include "test_codebundler.moc"
