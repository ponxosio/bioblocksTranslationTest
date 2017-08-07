#include <QtTest>
#include <QTemporaryFile>
#include <QFile>

#include <algorithm>
#include <vector>

#include <bioblocksTranslation/bioblockstranslator.h>

// add necessary includes here

#include "stringactuatorsinterface.h"

class SequentialProtocol : public QObject
{
    Q_OBJECT

public:
    SequentialProtocol();
    ~SequentialProtocol();

private:
    void executeProtocol(std::shared_ptr<ProtocolGraph> protocol,ActuatorsExecutionInterface* actuatorInterfaz);
    void copyResourceFile(const QString & resourcePath, QTemporaryFile* file) throw(std::invalid_argument);

private slots:
    void oneOperationTest();
    void twoOperationsLinkedTest();
    void twoOperationsParalelTest();
    void twoOperationsUnknowDurationLinkedTest();
    void twoOperationsUnknowDurationParalelTest();
    void simpleIfYesTest();
    void simpleIfNoTest();
    void complexIfYesTest();
    void complexIfNoTest();
    void elifB2Test();
    void elifB1Test();
    void elifNoBTest();
    void ifElseElseTest();
    void ifElseIfTest();
    void nestedIfsTestNoBTest();
    void nestedIfsTestB1Test();
    void nestedIfsTestB1N1Test();
    void loopTest();
    void thermocycling();
    void turbidostat2();

};

SequentialProtocol::SequentialProtocol()
{

}

SequentialProtocol::~SequentialProtocol()
{

}

/*
 * setContinuosFlow[0s:30s](A,B,10ml/hr);
 */
void SequentialProtocol::oneOperationTest()
{
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/sequential.json", tempFile);

            BioBlocksTranslator translator(10*units::s, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(10000ms);loadContainer(A,1ml);loadContainer(B,0ml);setContinuosFlow(A,B,10ml/h);timeStep();timeStep();timeStep();stopContinuosFlow(A,B);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * continuousFlow[0s:10s](A,B,10ml/h);
 * continuosFlow[-:10s](B,C,20ml/ms);
 */
void SequentialProtocol::twoOperationsLinkedTest() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/twoOperationsLinked.json", tempFile);

            BioBlocksTranslator translator(1*units::s, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(1000ms);loadContainer(A,1ml);loadContainer(B,0ml);loadContainer(C,0ml);setContinuosFlow(A,B,10ml/h);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopContinuosFlow(A,B);setContinuosFlow(B,C,7.2e+07ml/h);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopContinuosFlow(B,C);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * continuosFlow[0s:10s](A,B,10ml/hr);
 * continuousFlow[5s:5s](C,D,20ml/hr);
 */
void SequentialProtocol::twoOperationsParalelTest() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/twoOperationsParalel.json", tempFile);

            BioBlocksTranslator translator(1*units::s, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(1000ms);loadContainer(A,0ml);loadContainer(B,0ml);loadContainer(C,0ml);loadContainer(D,0ml);setContinuosFlow(A,B,10ml/h);timeStep();timeStep();timeStep();timeStep();timeStep();setContinuosFlow(C,D,20ml/h);timeStep();timeStep();timeStep();timeStep();timeStep();stopContinuosFlow(A,B);stopContinuosFlow(C,D);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * transfer[0s:](A,B,5ml);
 * transfer[-:](B,C,7ml);
 */
void SequentialProtocol::twoOperationsUnknowDurationLinkedTest() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/unknowDurationLinked.json", tempFile);

            BioBlocksTranslator translator(1*units::s, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(1000ms);loadContainer(A,0ml);loadContainer(B,0ml);loadContainer(C,0ml);transfer(A,B,5ml);timeStep();timeStep();timeStep();timeStep();timeStep();stopTransfer(A,B);transfer(B,C,7ml);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopTransfer(B,C);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * transfer[0s:](A,B,5ml);
 * transfer[0s:](C,D,7ml);
 */
void SequentialProtocol::twoOperationsUnknowDurationParalelTest() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/unknowDurationParalel.json", tempFile);

            BioBlocksTranslator translator(1*units::s, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(1000ms);loadContainer(A,0ml);loadContainer(B,0ml);loadContainer(C,0ml);loadContainer(D,0ml);transfer(A,B,5ml);transfer(C,D,7ml);timeStep();timeStep();timeStep();timeStep();timeStep();stopTransfer(A,B);timeStep();timeStep();stopTransfer(C,D);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * if[0s:](true) {
 *  continuosFlow[-:3s](A,B,10ml/hr);
 * }
 * continuosFlow[-:5s](B,C,7ml/hr);
 */
void SequentialProtocol::simpleIfYesTest() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/simpleIfYes.json", tempFile);

            BioBlocksTranslator translator(1*units::s, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(1000ms);loadContainer(A,0ml);loadContainer(B,0ml);loadContainer(C,0ml);setContinuosFlow(A,B,10ml/h);timeStep();timeStep();timeStep();stopContinuosFlow(A,B);setContinuosFlow(B,C,7ml/h);timeStep();timeStep();timeStep();timeStep();timeStep();stopContinuosFlow(B,C);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * if[0s:](false) {
 *  continuosFlow[-:3s](A,B,10ml/hr);
 * }
 * continuosFlow[-:5s](B,C,7ml/hr);
 */
void SequentialProtocol::simpleIfNoTest() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/simpleIfNo.json", tempFile);

            BioBlocksTranslator translator(1*units::s, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(1000ms);loadContainer(A,0ml);loadContainer(B,0ml);loadContainer(C,0ml);setContinuosFlow(B,C,7ml/h);timeStep();timeStep();timeStep();timeStep();timeStep();stopContinuosFlow(B,C);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * OD = measureOd[0s:2s](A,0Hz,650nm);
 * if[-:](OD < 600) {
 *  transfer[-:](B,A,2ml);
 *  incubate[-:3s](A,26ºC,5Hz);
 * }
 * centrifugation[-:5s](A,50kHz,26ºC);
 *
 */
void SequentialProtocol::complexIfYesTest() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/complexIf.json", tempFile);

            BioBlocksTranslator translator(200*units::ms, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{550});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(200ms);loadContainer(A,1ml);loadContainer(B,5ml);measureOD(A,0Hz,650nm);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();getMeasureOD(A);timeStep();transfer(B,A,2ml);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopTransfer(B,A);applyTemperature(A,26Cº);shake(A,5Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopShake(A);applyTemperature(A,26Cº);centrifugate(A,50000Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopCentrifugate(A);timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * OD = measureOd[0s:2s](A,0Hz,650nm);
 * if[-:](OD < 600) {
 *  transfer[-:](B,A,2ml);
 *  incubate[-:3s](A,26ºC,5Hz);
 * }
 * centrifugation[-:5s](A,50kHz,26ºC);
 *
 */
void SequentialProtocol::complexIfNoTest() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/complexIf.json", tempFile);

            BioBlocksTranslator translator(200*units::ms, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{650});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(200ms);loadContainer(A,1ml);loadContainer(B,5ml);measureOD(A,0Hz,650nm);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();getMeasureOD(A);timeStep();applyTemperature(A,26Cº);centrifugate(A,50000Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopCentrifugate(A);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * flag = 0;
 * if[-:](flag != 0) {
 *  continuosFlow[-:2s](B,A,5ml/hr);
 * } else if (flag == 0) {
 *  mix[-:3s](A,vortex,50Hz);
 * }
 * centrifugation[-:5s](A,50hz,26ºC)
 *
 */
void SequentialProtocol::elifB2Test() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/elifB2.json", tempFile);

            BioBlocksTranslator translator(200*units::ms, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{650});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(200ms);loadContainer(B,0ml);loadContainer(A,0ml);stir(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopStir(A);applyTemperature(A,26Cº);centrifugate(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopCentrifugate(A);timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * flag = 1;
 * if[-:](flag != 0) {
 *  continuosFlow[-:2s](B,A,5ml/hr);
 * } else if (flag == 0) {
 *  mix[-:3s](A,vortex,50Hz);
 * }
 * centrifugation[-:5s](A,50hz,26ºC)
 *
 */
void SequentialProtocol::elifB1Test() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/elifB1.json", tempFile);

            BioBlocksTranslator translator(200*units::ms, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{650});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(200ms);loadContainer(B,0ml);loadContainer(A,0ml);setContinuosFlow(B,A,5ml/h);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopContinuosFlow(B,A);applyTemperature(A,26Cº);centrifugate(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopCentrifugate(A);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * flag = -1;
 * if[-:](flag > 0) {
 *  continuosFlow[-:2s](B,A,5ml/hr);
 * } else if (flag == 0) {
 *  mix[-:3s](A,vortex,50Hz);
 * }
 * dentrifugation[-:5s](A,50hz,26ºC)
 *
 */
void SequentialProtocol::elifNoBTest() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/elifNoB.json", tempFile);

            BioBlocksTranslator translator(200*units::ms, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{650});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(200ms);loadContainer(B,0ml);loadContainer(A,0ml);applyTemperature(A,26Cº);centrifugate(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopCentrifugate(A);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * flag = -1;
 * if[-:](flag > 0) {
 *  continuosFlow[-:2s](B,A,5ml/hr);
 * } else {
 *  mix[-:3s](A,vortex,50Hz);
 * }
 * centrifugation[-:5s](A,50hz,26ºC)
 *
 */
void SequentialProtocol::ifElseElseTest() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/ifElseElse.json", tempFile);

            BioBlocksTranslator translator(200*units::ms, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{650});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(200ms);loadContainer(B,0ml);loadContainer(A,0ml);stir(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopStir(A);applyTemperature(A,26Cº);centrifugate(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopCentrifugate(A);timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * flag = 1;
 * if[-:](flag > 0) {
 *  continuosFlow[-:2s](B,A,5ml/hr);
 *  incubate[-:3s](A,50Hz,26ºC);
 * } else {
 *  mix[-:3s](A,vortex,50Hz);
 * }
 * centrifugation[-:5s](A,50hz,26ºC)
 *
 */
void SequentialProtocol::ifElseIfTest() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/ifElseIf.json", tempFile);

            BioBlocksTranslator translator(200.0 * units::ms, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol =
                    translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{650});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(200ms);loadContainer(B,0ml);loadContainer(A,0ml);setContinuosFlow(B,A,5ml/h);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopContinuosFlow(B,A);applyTemperature(A,26Cº);shake(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopShake(A);applyTemperature(A,26Cº);centrifugate(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopCentrifugate(A);timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * od = measurementOd[0s:2s](A,50Hz,650nm);
 * if[_:](od < 600) {
 *  transfer[-:](B,A,0.5ml);
 *  incubate[-:5s](A,50Hz,26ºC);
 *  flur = measurementFluorescence[-:2s](A,50Hz,650nm,650nm);
 *  if[-:](flur < 600) {
 *      transfer[-:](A,C,1.5ml);
 *  }
 * }
 * bands = electrophoresis[-:2s](A,2v/cm);
 */
void SequentialProtocol::nestedIfsTestNoBTest() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/nestedIf.json", tempFile);

            BioBlocksTranslator translator(200*units::ms, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol = translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{650});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(200ms);loadContainer(A,1ml);loadContainer(B,1.5ml);loadContainer(C,0ml);measureOD(A,50Hz,650nm);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();getMeasureOD(A);timeStep();startElectrophoresis(A,2V/cm);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopElectrophoresis(A);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * od = measurementOd[0s:2s](A,50Hz,650nm);
 * if[_:](od < 600) {
 *  transfer[-:](B,A,0.5ml);
 *  incubate[-:5s](A,50Hz,26ºC);
 *  flur = measurementFluorescence[-:2s](A,50Hz,650nm,650nm);
 *  if[-:](flur < 600) {
 *      transfer[-:](A,C,1.5ml);
 *  }
 * }
 * bands = electrophoresis[-:2s](A,2v/cm);
 */
void SequentialProtocol::nestedIfsTestB1Test() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/nestedIf.json", tempFile);

            BioBlocksTranslator translator(200*units::ms, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol = translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{590,650});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(200ms);loadContainer(A,1ml);loadContainer(B,1.5ml);loadContainer(C,0ml);measureOD(A,50Hz,650nm);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();getMeasureOD(A);timeStep();transfer(B,A,0.5ml);timeStep();timeStep();timeStep();stopTransfer(B,A);applyTemperature(A,26Cº);shake(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopShake(A);measureFluorescence(A,50Hz,650nm, 650nm);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();getMeasureFluorescence(A);timeStep();startElectrophoresis(A,2V/cm);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopElectrophoresis(A);timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * od = measurementOd[0s:2s](A,50Hz,650nm);
 * if[_:](od < 600) {
 *  transfer[-:](B,A,0.5ml);
 *  incubate[-:5s](A,50Hz,26ºC);
 *  flur = measurementFluorescence[-:2s](A,50Hz,650nm,650nm);
 *  if[-:](flur < 600) {
 *      transfer[-:](A,C,1.5ml);
 *  }
 * }
 * bands = electrophoresis[-:2s](A,2v/cm);
 */
void SequentialProtocol::nestedIfsTestB1N1Test() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/nestedIf.json", tempFile);

            BioBlocksTranslator translator(200*units::ms, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol = translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{590,500});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(200ms);loadContainer(A,1ml);loadContainer(B,1.5ml);loadContainer(C,0ml);measureOD(A,50Hz,650nm);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();getMeasureOD(A);timeStep();transfer(B,A,0.5ml);timeStep();timeStep();timeStep();stopTransfer(B,A);applyTemperature(A,26Cº);shake(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopShake(A);measureFluorescence(A,50Hz,650nm, 650nm);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();getMeasureFluorescence(A);timeStep();transfer(A,C,1.5ml);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopTransfer(A,C);startElectrophoresis(A,2V/cm);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopElectrophoresis(A);timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * od = 0;
 * while[0s:](od <= 600) {
 *  incubate[x:5s](A,26ºC,50Hz,50%);
 *  od = measurement[x:1s](A,5Hz,650nm);
 * }
 * transfer[x:](A,B,1ml);
 * centrifugation[x:3s](B,50Hz,26ºC);
 */
void SequentialProtocol::loopTest() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/loop.json", tempFile);

            BioBlocksTranslator translator(200*units::ms, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol = translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{200,400,500,580,620});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(200ms);loadContainer(A,10ml);loadContainer(B,0ml);applyTemperature(A,26Cº);shake(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopShake(A);measureOD(A,5Hz,650nm);timeStep();timeStep();timeStep();timeStep();timeStep();getMeasureOD(A);timeStep();applyTemperature(A,26Cº);shake(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopShake(A);measureOD(A,5Hz,650nm);timeStep();timeStep();timeStep();timeStep();timeStep();getMeasureOD(A);timeStep();applyTemperature(A,26Cº);shake(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopShake(A);measureOD(A,5Hz,650nm);timeStep();timeStep();timeStep();timeStep();timeStep();getMeasureOD(A);timeStep();applyTemperature(A,26Cº);shake(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopShake(A);measureOD(A,5Hz,650nm);timeStep();timeStep();timeStep();timeStep();timeStep();getMeasureOD(A);timeStep();applyTemperature(A,26Cº);shake(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopShake(A);measureOD(A,5Hz,650nm);timeStep();timeStep();timeStep();timeStep();timeStep();getMeasureOD(A);timeStep();transfer(A,B,1ml);timeStep();timeStep();timeStep();timeStep();timeStep();stopTransfer(A,B);applyTemperature(B,26Cº);centrifugate(B,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(B);stopCentrifugate(B);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * cycles = 3;
 * thermocycling[0s:](A,cycles, steps[{60ºC,2s},{30ºC,5s}]);
 * centrifugation[x:5s](A,50Hz,26ºC);
 */
void SequentialProtocol::thermocycling() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/thermocycling.json", tempFile);

            BioBlocksTranslator translator(200*units::ms, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol = translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{200,400,500,580,620});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(200ms);loadContainer(A,1ml);applyTemperature(A,60Cº);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);applyTemperature(A,30Cº);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);timeStep();applyTemperature(A,60Cº);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);applyTemperature(A,30Cº);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);timeStep();applyTemperature(A,60Cº);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);applyTemperature(A,30Cº);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);timeStep();applyTemperature(A,26Cº);centrifugate(A,50Hz);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopApplyTemperature(A);stopCentrifugate(A);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

/*
 * rate =50;
 * OD = 0;
 * while(OD < 1 || OD > 1.1) {
 *  OD = measureOD[x:2s]();
 *  rate = rate - (rate * (1-OD));
 *  SetContinuousFlow([media,cell,waste], rate ml/hr)[x:10s];
 * }
 */
void SequentialProtocol::turbidostat2() {
    QTemporaryFile* tempFile = new QTemporaryFile();
    if (tempFile->open()) {
        try {
            copyResourceFile(":/protocol/protocolos/turbidostat2.json", tempFile);

            BioBlocksTranslator translator(1*units::s, tempFile->fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol = translator.translateFile();

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{0.5,0.8,1.2,1.05});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(1000ms);loadContainer(Waste,0ml);loadContainer(cell,50ml);loadContainer(media,100ml);measureOD(cell,20Hz,600nm);timeStep();timeStep();getMeasureOD(cell);setContinuosFlow(media,cell,150ml/h);setContinuosFlow(cell,Waste,150ml/h);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopContinuosFlow(media,cell);stopContinuosFlow(cell,Waste);timeStep();measureOD(cell,20Hz,600nm);timeStep();timeStep();getMeasureOD(cell);setContinuosFlow(media,cell,120ml/h);setContinuosFlow(cell,Waste,120ml/h);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopContinuosFlow(media,cell);stopContinuosFlow(cell,Waste);timeStep();measureOD(cell,20Hz,600nm);timeStep();timeStep();getMeasureOD(cell);setContinuosFlow(media,cell,144ml/h);setContinuosFlow(cell,Waste,144ml/h);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopContinuosFlow(media,cell);stopContinuosFlow(cell,Waste);timeStep();measureOD(cell,20Hz,600nm);timeStep();timeStep();getMeasureOD(cell);setContinuosFlow(media,cell,151.2ml/h);setContinuosFlow(cell,Waste,151.2ml/h);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopContinuosFlow(media,cell);stopContinuosFlow(cell,Waste);timeStep();timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            delete tempFile;
            QFAIL(e.what());
        }
    } else {
        delete tempFile;
        QFAIL("imposible to create temporary file");
    }
    delete tempFile;
}

void SequentialProtocol::executeProtocol(std::shared_ptr<ProtocolGraph> protocol,ActuatorsExecutionInterface* actuatorInterfaz) {
    std::vector<int> nodes2process = {protocol->getStart()->getContainerId()};
    while(!nodes2process.empty()) {
        int nextId = nodes2process.back();
        nodes2process.pop_back();

        if (protocol->isCpuOperation(nextId)) {
            protocol->getCpuOperation(nextId)->execute();
        } else if (protocol->isActuatorOperation(nextId)) {
            protocol->getActuatorOperation(nextId)->execute(actuatorInterfaz);
        }

        ProtocolGraph::ProtocolEdgeVectorPtr leaving = protocol->getProjectingEdges(nextId);
        for(const ProtocolGraph::ProtocolEdgePtr & edge: *leaving.get()) {
            if (edge->conditionMet()) {
                int nextop = edge->getIdTarget();
                if (find(nodes2process.begin(),nodes2process.end(), nextop) == nodes2process.end()) {
                    nodes2process.push_back(nextop);
                }
            }
        }
    }
}

void SequentialProtocol::copyResourceFile(const QString & resourcePath, QTemporaryFile* tempFile) throw(std::invalid_argument) {
    QFile resourceFile(resourcePath);
    if(!resourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw(std::invalid_argument("imposible to open" + resourcePath.toStdString()));
    }

    QTextStream out(tempFile);

    QTextStream in(&resourceFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        out << line;
    }
    out.flush();
}

QTEST_APPLESS_MAIN(SequentialProtocol)

#include "tst_sequentialprotocol.moc"
