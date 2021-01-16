#include "pennygui.h"
#include "ui_pennygui.h"

PennyGUI::PennyGUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PennyGUI)
{
    ui->setupUi(this);
    pennyGUIUpdateTimer = new QTimer(this);
}

PennyGUI::~PennyGUI()
{
    dbg->quit();
    dbg->wait();
    delete dbg;
    delete ui;
}

void PennyGUI::InitDbg(PPennyDbgStruct dbgStruct) {
    dbg = new PennyDbg(dbgStruct, this);

    //Console related dbg emitter
    connect(dbg, &PennyDbg::console_log, this, &PennyGUI::on_console_log);
    connect(dbg, &PennyDbg::console_err, this, &PennyGUI::on_console_err);
    //Modules tab related dbg emitter
    connect(dbg, &PennyDbg::modules_update, this, &PennyGUI::on_modules_update);

    dbg->start();

    connect(pennyGUIUpdateTimer, &QTimer::timeout, dbg, &PennyDbg::on_gui_update);

    pennyGUIUpdateTimer->start(PENNY_GUI_UPDATE_TIMER);
}

//--------DBG SLOTS

void PennyGUI::on_console_log(QString txt) {
    ui->ConsoleBrowser->setText(ui->ConsoleBrowser->toPlainText()+txt+"\n");
}

void PennyGUI::on_console_err(QString txt) {
    QMessageBox alertBox;
    alertBox.setText(txt);
    alertBox.exec();
    ui->ConsoleBrowser->setText(ui->ConsoleBrowser->toPlainText()+txt+"\n");
}

void PennyGUI::on_modules_update(std::map<LPVOID, PNMODULE> modules) {
    modulesListModel = new ModulesListModel(this);
    std::for_each(modules.begin(),modules.end(), [this](std::pair<const LPVOID, PNMODULE> &pair){
        ModuleListStruct moduleListStruct;
        moduleListStruct.moduleName = QString::fromStdWString(pair.second.modulePath);
        moduleListStruct.baseAdress = QString("0x%1").arg((quintptr)pair.second.moduleBaseAddr, QT_POINTER_SIZE * 2, 16,QChar('0'));
        moduleListStruct.moduleSize = QString::number(10);
        moduleListStruct.Description = "connard";
        modulesListModel->AddItem(moduleListStruct);
    });
    ui->ModulesTable->setModel(modulesListModel);
}

//--------FILE MENU

void PennyGUI::on_actionOpen_Process_triggered()
{
    QFileDialog openProcessDialog(this);
    openProcessDialog.setFileMode(QFileDialog::ExistingFile);

    QStringList fileNames;

    if (openProcessDialog.exec()) {
        fileNames = openProcessDialog.selectedFiles();
    }

    if (fileNames.size() > 0) {
        PennyDbgStruct dbgStruct;
        dbgStruct.startMode = Open;
        dbgStruct.path = fileNames.at(0).toStdWString();

        InitDbg(&dbgStruct);
    }
}

//--------WINDOW MENU

void PennyGUI::on_actionConsole_triggered()
{
    ui->ConsoleWindow->show();
}
