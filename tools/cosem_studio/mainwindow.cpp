
#include <iostream>

#include <QtWidgets>

#include "mainwindow.h"

#define APP_TITLE "Shaddam"

ScriptConsole::ScriptConsole(QWidget *parent) :
  QPlainTextEdit(parent)
{
    QPalette p = palette();
    p.setColor(QPalette::Active, QPalette::Base, Qt::black);
    p.setColor(QPalette::Inactive, QPalette::Base, Qt::black);
    p.setColor(QPalette::Active, QPalette::Text, Qt::white);
    p.setColor(QPalette::Inactive, QPalette::Text, Qt::white);
    setPalette(p);

    QFont f;
    #ifdef Q_OS_LINUX
    f.setFamily("Monospace");
    f.setPointSize(10);
    #else
    f.setFamily("Lucida Console");
    f.setPointSize(10);
    #endif
    f.setFixedPitch(true);
    setFont(f);

    setCursorWidth(QFontMetrics(font()).width(QChar('x')));
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setUndoRedoEnabled(false);
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setWordWrapMode(QTextOption::NoWrap);
    setBackgroundVisible(false);
    setFrameStyle(QFrame::NoFrame);
    setTabStopWidth(40);
    setAcceptDrops(false);
    setReadOnly(true);
}

MainWindow::MainWindow(IScript &i_script)
    : mScript(i_script)
{
    init();
    setCurrentFile("");
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About " APP_TITLE),
                tr("<p><b>" APP_TITLE "</b> is the successor of Manitoo, " \
                   "a scripable automated tool to test DLMS/Cosem meters.</p>"));

}

void MainWindow::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    textEdit = new QTextEdit;
    textEdit->setFont(font);

    highlighter = new Highlighter(textEdit->document());

    QFile file(":/help.lua");
    if (file.open(QFile::ReadOnly | QFile::Text))
        textEdit->setPlainText(file.readAll());
}

class ScriptMessageEvent : public QEvent
{
public:

    static const QEvent::Type staticType = static_cast<QEvent::Type>(QEvent::User + 462);
    ScriptMessageEvent()
        : QEvent(staticType)
    {

    }
    std::string message;
};

class ScriptResultEvent : public QEvent
{
public:

    static const QEvent::Type staticType = static_cast<QEvent::Type>(QEvent::User + 463);
    ScriptResultEvent()
        : QEvent(staticType)
    {

    }
    int code;
};


void MainWindow::Print(const std::string &message)
{
    ScriptMessageEvent *ev = new ScriptMessageEvent();
    ev->message = message;
    QApplication::postEvent(this, reinterpret_cast<QEvent*>(ev));
}

void MainWindow::Result(int code)
{
    ScriptResultEvent *ev = new ScriptResultEvent();
    ev->code = code;
    QApplication::postEvent(this, reinterpret_cast<QEvent*>(ev));
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "Lua Files (*.lua)");
    if (!fileName.isEmpty()) {
        if (isUntitled && textEdit->document()->isEmpty()
                && !isWindowModified()) {
            loadFile(fileName);
        }
    }
}

bool MainWindow::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile,
                                                    "Lua Files (*.lua)");
    if (fileName.isEmpty())
    {
        return false;
    }

    return saveFile(fileName);
}

void MainWindow::documentWasModified()
{
    setWindowModified(true);
}

void MainWindow::runScript()
{
    mScript.Execute(textEdit->toPlainText().toStdString());
}

void MainWindow::stopScript()
{

}

void MainWindow::init()
{
    isUntitled = true;

    setupEditor();
    setCentralWidget(textEdit);

    setWindowTitle(tr(APP_TITLE " [*]"));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();

    readSettings();

    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    setUnifiedTitleAndToolBarOnMac(true);

    /*
    // Timer to refresh console
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotConsoleUpdate()));
    timer->start(100); // 100ms
    */
}

/*
 * Enable this to buffurize the output if spammed
 *
void MainWindow::slotConsoleUpdate()
{

    if (!consoleBuffer.isEmpty())
    {
        scriptConsole->moveCursor(QTextCursor::End);
        scriptConsole->appendPlainText(consoleBuffer);
        scriptConsole->moveCursor(QTextCursor::End);
        consoleBuffer.clear();
    }


}

*/

void MainWindow::createActions()
{
    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    closeAct = new QAction(tr("&Close"), this);
    closeAct->setShortcut(tr("Ctrl+W"));
    closeAct->setStatusTip(tr("Close this window"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(close()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));


    // --------------------------  SCRIPT ACTIONS --------------------------
    runAct = new QAction(QIcon(":/images/play.png"), tr("&Copy"), this);
    runAct->setShortcut(tr("F5"));
    runAct->setStatusTip(tr("Run the script"));
    connect(runAct, SIGNAL(triggered()), this, SLOT(runScript()));

    stopAct = new QAction(QIcon(":/images/stop.png"), tr("&Stop"), this);
    stopAct->setShortcut(tr("F10"));
    stopAct->setStatusTip(tr("Stop the execution of the script"));
    connect(stopAct, SIGNAL(triggered()), this, SLOT(stopScript()));


    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
}

//! [implicit tr context]
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));

    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAct);
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    viewMenu = menuBar()->addMenu(tr("&View"));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);

    scriptToolBar = addToolBar(tr("Script"));
    scriptToolBar->addAction(runAct);
    scriptToolBar->addAction(stopAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createDockWindows()
{
    dockConsole = new QDockWidget(tr("Lua script console"), this);
    dockConsole->setAllowedAreas(Qt::BottomDockWidgetArea);

    scriptConsole = new ScriptConsole(this);

    dockConsole->setWidget(scriptConsole);

    addDockWidget(Qt::BottomDockWidgetArea, dockConsole);
    viewMenu->addAction(dockConsole->toggleViewAction());
}

void MainWindow::readSettings()
{
    QSettings settings;
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool MainWindow::maybeSave()
{
    if (textEdit->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr(APP_TITLE),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard
                     | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr(APP_TITLE),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    textEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr(APP_TITLE),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << textEdit->toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    static int sequenceNumber = 1;

    isUntitled = fileName.isEmpty();
    if (isUntitled) {
        curFile = tr("script%1.lua").arg(sequenceNumber++);
    } else {
        curFile = QFileInfo(fileName).canonicalFilePath();
    }

    textEdit->document()->setModified(false);
    setWindowModified(false);
    setWindowFilePath(curFile);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::customEvent(QEvent *e)
{
    if (e->type() == ScriptMessageEvent::staticType)
    {
        ScriptMessageEvent *ev = dynamic_cast<ScriptMessageEvent *>(e);
        if (ev != NULL)
        {
            QString message(ev->message.c_str());
            if (message.endsWith('\n'))
            {
                message.chop(1);
            }
         //   scriptConsole->moveCursor(QTextCursor::End);
            scriptConsole->appendPlainText(message);
         //   scriptConsole->moveCursor(QTextCursor::End);

            //consoleBuffer.append(ev->message.c_str());
        }
    }
    else if (e->type() == ScriptResultEvent::staticType)
    {
        ScriptResultEvent *ev = dynamic_cast<ScriptResultEvent *>(e);
        if (ev != NULL)
        {
            statusBar()->showMessage(tr("Script result code: %1").arg(ev->code));
        }
    }

}

