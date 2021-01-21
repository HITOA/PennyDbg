#include "pennygui.h"
#include "ui_pennygui.h"

PennyGUI::PennyGUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PennyGUI)
{
    ui->setupUi(this);
    modulegui = new ModuleGUI(this);

    for (int i = 0; i < ui->MainTab->count(); i++) {
        mainTabWidgetList.push_back(ui->MainTab->widget(i));
    }
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
    connect(dbg, &PennyDbg::threads_GUI_update, this, &PennyGUI::on_threads_GUI_update);

    connect(this, &PennyGUI::dump_process_memory, dbg, &PennyDbg::on_dump_process_memory);
    connect(this, &PennyGUI::dump_module_memory, dbg, &PennyDbg::on_dump_module_memory);
    connect(this, &PennyGUI::memory_scan, dbg, &PennyDbg::on_memory_scan);

    dbg->start();

    lpLoadedDllTableModel = new LoadedDllTableModel(dbg->GetPtrToProcessData(), this);
    ui->ModulesTable->setModel(lpLoadedDllTableModel);

    lpThreadTableModel = new ThreadTableModel(dbg->GetPtrToProcessData(), this);
    ui->ThreadsTable->setModel(lpThreadTableModel);
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

void PennyGUI::on_threads_GUI_update() {
    lpThreadTableModel->updateGUI();
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

void PennyGUI::on_actionModules_triggered()
{
    ui->ModulesWindow->show();
}


void PennyGUI::on_actionThreads_triggered()
{
    ui->ThreadsWindow->show();
}

void PennyGUI::on_actionHex_view_triggered()
{
    ui->MainTab->addTab(mainTabWidgetList.at(0), "Hex view");
}

void PennyGUI::on_MainTab_tabCloseRequested(int index)
{
    ui->MainTab->removeTab(index);
}

void PennyGUI::on_actionDump_debugged_process_triggered()
{
    QFileDialog saveDumpFileDialog(this);
    saveDumpFileDialog.setFileMode(QFileDialog::AnyFile);

    QStringList fileNames;

    if (saveDumpFileDialog.exec()) {
        fileNames = saveDumpFileDialog.selectedFiles();
    }

    if (fileNames.size() > 0) {
        emit dump_process_memory(fileNames.at(0).toStdWString());
    }
}

void PennyGUI::on_ModulesTable_doubleClicked(const QModelIndex &index)
{
    DebuggedProcessData *ppData = dbg->GetPtrToProcessData();

    LPLoadedDllData data = ppData->GetLoadedDllData(index.row());

    modulegui->SetModuleInformation(QString::fromStdWString(data->fullName),
                                    "?",
                                    QString::fromStdWString(data->fullPath),
                                    QString::number(data->dllMappedSize),
                                    QString::number(data->dllDiskSize),
                                    ParseSystemTime(data->creationTime),
                                    ParseSystemTime(data->lastWriteTime),
                                    ParseSystemTime(data->lastAcessTime));
    modulegui->show();
    //DebuggedProcessData *ppData = dbg->GetPtrToProcessData();
    //emit dump_module_memory((LPBYTE)ppData->GetLoadedDllData(index.row())->lpBaseOfDll + ppData->GetLoadedDllData(index.row())->entryPoint);
}

void PennyGUI::on_ScanMemoryBtn_clicked()
{
    char* buffer = (char*)"Chess";
    size_t bSize = 5;
    emit memory_scan(buffer, bSize);
}
//utils

QString PennyGUI::ParseSystemTime(SYSTEMTIME sTime) {
    return QString::number(sTime.wDay) + "/" + QString::number(sTime.wMonth) + "/" + QString::number(sTime.wYear) + ", " + QString::number(sTime.wHour) + ":" + QString::number(sTime.wMinute) + ":" + QString::number(sTime.wSecond);
}
