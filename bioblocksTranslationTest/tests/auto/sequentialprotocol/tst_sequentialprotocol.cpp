#include <QtTest>
#include <QTemporaryFile>
#include <QFile>

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

private slots:
    void oneOperationTest();
    void twoOperationsLinkedTest();

};

SequentialProtocol::SequentialProtocol()
{

}

SequentialProtocol::~SequentialProtocol()
{

}

void SequentialProtocol::oneOperationTest()
{
    QTemporaryFile tempFile;
    if (tempFile.open()) {
        QFile::copy(":/protocols/protocols/sequential.json", tempFile.fileName());

        try {
            BioBlocksTranslator translator;
            //std::shared_ptr<ProtocolGraph> protocol = translator.translateFile(tempFile.fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol = translator.translateFile("X:/bioblocksTranslation/bioblocksTranslationTest/bioblocksTranslationTest/tests/auto/sequentialprotocol/protocolos/sequential.json");

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "setTimeStep(1000ms);setContinuosFlow(A,B,10ml/h);timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();timeStep();stopContinuosFlow(A,B);timeStep();timeStep();";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            QFAIL(e.what());
        }
    } else {
        QFAIL("imposible to create temporary file");
    }
}

void SequentialProtocol::twoOperationsLinkedTest() {
    QTemporaryFile tempFile;
    if (tempFile.open()) {
        QFile::copy(":/protocols/protocols/twoOperationsLinked.json", tempFile.fileName());

        try {
            BioBlocksTranslator translator;
            //std::shared_ptr<ProtocolGraph> protocol = translator.translateFile(tempFile.fileName().toStdString());
            std::shared_ptr<ProtocolGraph> protocol = translator.translateFile("X:/bioblocksTranslation/bioblocksTranslationTest/bioblocksTranslationTest/tests/auto/sequentialprotocol/protocolos/twoOperationsLinked.json");

            qDebug() << protocol->toString().c_str();

            StringActuatorsInterface* interface = new StringActuatorsInterface(std::vector<double>{});
            executeProtocol(protocol, interface);

            std::string execution = interface->getStream().str();
            qDebug() << "protocol execution";
            qDebug() << execution.c_str();

            std::string expected = "";
            qDebug() << "protocol expected execution";
            qDebug() << expected.c_str();

            QVERIFY2(execution.compare(expected) == 0, "Execution and expected execution are not the same, check debug data for seeing where");
        } catch (std::exception & e) {
            QFAIL(e.what());
        }
    } else {
        QFAIL("imposible to create temporary file");
    }
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
                nodes2process.push_back(edge->getIdTarget());
            }
        }
    }
}

QTEST_APPLESS_MAIN(SequentialProtocol)

#include "tst_sequentialprotocol.moc"
