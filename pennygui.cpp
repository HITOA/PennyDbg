#include "pennygui.h"
#include "ui_pennygui.h"

PennyGUI::PennyGUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PennyGUI)
{
    ui->setupUi(this);
}

PennyGUI::~PennyGUI()
{
    dbg->quit();
    dbg->wait();
    delete dbg;
    delete ui;
}

void PennyGUI::InitDbg(LPDebuggedProcessData pPData) {
    dbg = new PennyDbg(pPData, this);

    //Console related dbg emitter
    connect(dbg, &PennyDbg::console_log, this, &PennyGUI::on_console_log);
    connect(dbg, &PennyDbg::console_err, this, &PennyGUI::on_console_err);
    //GUI update related dbg emitter
    connect(dbg, &PennyDbg::loadedDll_GUI_update, this, &PennyGUI::on_loadedDll_GUI_update);

    dbg->start();

    lpLoadedDllTableModel = new LoadedDllTableModel(dbg->GetPtrToProcessData(),this);
    ui->ModulesTable->setModel(lpLoadedDllTableModel);
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

//GUI update related

void PennyGUI::on_loadedDll_GUI_update() {
    lpLoadedDllTableModel->updateGUI();
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
        DebuggedProcessData pData(fileNames.at(0).toStdWString());

        InitDbg(&pData);
    }
}

//--------WINDOW MENU

void PennyGUI::on_actionConsole_triggered()
{
    ui->ConsoleWindow->show();
}
