#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <QCommonStyle>
#include <QCryptographicHash>
#include <QDesktopWidget>
#include <QDir>
#include <QtConcurrent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStringList>
#include <QProgressDialog>
#include <QStringListIterator>

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentDirectoryScanned(false)
    , hashingInProgress(false)
    , hashingCancelled(false)
    , filesHashed(0)
{
    ui->setupUi(this);

    setWindowTitle("Duplicate deleter");
    ui->treeWidget->setHeaderLabel("Duplicates found in current directory");
    ui->statusBar->showMessage("Choose a directory to find duplicate files");

    connect(ui->actionSelectDirectory, &QAction::triggered, this, &MainWindow::selectDirectory);
    connect(ui->actionHandleDirectory, &QAction::triggered, this, &MainWindow::handleDirectory);
    connect(ui->actionRequestConfirmThreadCount, &QAction::triggered, this, &MainWindow::confirmThreadCount);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QTreeWidgetItem* MainWindow::addGroup()
{
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
    ui->treeWidget->addTopLevelItem(item);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    return item;
}

QTreeWidgetItem* MainWindow::addGroupItem(QTreeWidgetItem *group, QString path)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, path);
    group->addChild(item);
    return item;
}

QString MainWindow::toHumanSize(quint64 value)
{
    QStringList sizes;
    sizes << "KB" << "MB" << "GB" << "TB";
    QStringListIterator it(sizes);
    QString unit = "bytes";

    while (value >= 1024 && it.hasNext())
    {
        unit = it.next();
        value = (value + 512) / 1024;
    }

    return QString::number(value) + " " + unit;
}

void MainWindow::updateProgressBarAndTreeWidget(QVector<QPair<quint64, QByteArray> > &hashes,
                                 QVector<QString> &filePaths,
                                 quint64 &offset)
{
    filesHashed += hashes.size() - offset;
    for (quint64 i = offset; i < hashes.size(); ++i)
    {
        if (hashedFiles.contains(hashes[i]))
        {
            if (treePlacement.contains(hashes[i]))
            {
                QTreeWidgetItem* group = treePlacement[hashes[i]];
                addGroupItem(group, filePaths[i]);
                group->setText(0, QString::number(group->childCount()) + " duplicate(s) found (" +
                               toHumanSize(hashes[i].first) + " each)");
            }
            else
            {
                treePlacement.insert(hashes[i], addGroup());
                QTreeWidgetItem* group = treePlacement[hashes[i]];
                addGroupItem(group, hashedFiles[hashes[i]]);
                addGroupItem(group, filePaths[i]);
                group->setText(0, QString::number(group->childCount()) + " duplicate(s) found (" +
                               toHumanSize(hashes[i].first) + " each)");
            }
        }
        else
        {
            hashedFiles.insert(hashes[i], filePaths[i]);
        }
    }
    offset = hashes.size();
    emit hashingWatcher.progressValueChanged(filesHashed);
}

QVector<QString> MainWindow::requestFilesRecursive(QString directoryPath)
{
    QDir directory(directoryPath);
    QFileInfoList fileList = directory.entryInfoList(QDir::Files | QDir::NoSymLinks);
    QFileInfoList dirList = directory.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    QVector<QString> recursiveList;
    for (QFileInfo file : fileList)
    {
        recursiveList.push_back(file.fileName());
    }
    for (QFileInfo dir : dirList)
    {
        if (!dir.isReadable() || !dir.exists())
        {
            continue;
        }

        QVector<QString> subdirFileList = requestFilesRecursive(dir.absoluteFilePath());
        for (QString file : subdirFileList)
        {
            recursiveList.push_back(dir.fileName() + "/" + file);
        }
    }

    return recursiveList;
}

QVector<QVector<QString> > MainWindow::distributeFiles(QVector<QString> filePaths)
{
    quint64 threadCount = qMin(filePaths.size(), ui->threads_lineedit->text().toInt());
    QVector<QVector<QString> > distribution(threadCount);
    QVector<quint64> distribution_sizes(threadCount, 0);

    quint64 id = 0, total_size = 0;
    for (QString filePath : filePaths)
    {
        QFile currentFile(currentDirectory + "/" + filePath);
        total_size += currentFile.size();
    }
    quint64 average = total_size / threadCount;
    for (quint64 i = 0; id != filePaths.size(); i = (i + 1) % threadCount)
    {
        if (distribution_sizes[i] < average)
        {
            QString filePath = filePaths[id];
            QFile currentFile(currentDirectory + "/" + filePath);
            distribution[i].push_back(filePath);
            distribution_sizes[i] += currentFile.size();
            ++id;
        }
    }

    return distribution;
}

QByteArray MainWindow::hashFile(QFile &file)
{
    QCryptographicHash hasher(QCryptographicHash::Sha512);
    file.open(QIODevice::ReadOnly);

    qint64 blockCount = (file.size() + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (qint64 i = 0; i < blockCount; ++i)
    {
        hasher.addData(file.read(BLOCK_SIZE));
        if (hashingCancelled)
        {
            return QByteArray();
        }
    }

    file.close();
    return hasher.result();
}

void MainWindow::hashFileBundle(QVector<QString> &filePaths)
{
    quint64 offset = 0;
    QVector<QPair<quint64, QByteArray> > fileHashes;
    for (QString filePath : filePaths)
    {
        QFile currentFile(currentDirectory + "/" + filePath);
        QByteArray fileHash = hashFile(currentFile);

        if (hashingCancelled)
        {
            return;
        }

        fileHashes.push_back({ currentFile.size(), fileHash });
        if (hashingMutex.try_lock())
        {
            updateProgressBarAndTreeWidget(fileHashes, filePaths, offset);
            hashingMutex.unlock();
        }
    }
    hashingMutex.lock();
    updateProgressBarAndTreeWidget(fileHashes, filePaths, offset);
    hashingMutex.unlock();
}

void MainWindow::confirmThreadCount()
{
    qint32 threadCount = ui->threads_lineedit->text().toInt();
    if (threadCount < 1)
    {
        ui->threads_lineedit->setText("1");
    }
    else if (threadCount > 99)
    {
        ui->threads_lineedit->setText("99");
    }
}

void MainWindow::selectDirectory()
{
    currentDirectory = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                    QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!QDir(currentDirectory).exists())
    {
        QMessageBox::critical(this, "Duplicate deleter", "Chosen directory cannot be opened");
        return;
    }
    ui->directory_linedit->setText(currentDirectory);
    ui->directory_button->setDisabled(false);
    ui->handle_button->setText("Scan directory for duplicates");
    ui->statusBar->showMessage("Choose a directory to find duplicate files");
    ui->treeWidget->clear();

    filesHashed = 0;
    hashedFiles.clear();
    treePlacement.clear();
    currentDirectoryScanned = false;
    hashingCancelled = false;
}

void MainWindow::handleDirectory()
{
    if (currentDirectoryScanned)
    {
        deleteSelected();
    }
    else
    {
        scanDirectory();
    }
}

void MainWindow::cancelHashing()
{
    hashingCancelled = true;
}

void MainWindow::deleteSelected()
{
    QList<QTreeWidgetItem*> selected = ui->treeWidget->selectedItems();
    if (selected.isEmpty())
    {
        QMessageBox::information(this, "Duplicate deleter", "Please select some files to delete. Or don't, it up to you, really");
        return;
    }

    QMessageBox message;
    message.setIcon(QMessageBox::Question);
    message.setText("Are you sure you want to delete " + QString::number(selected.size()) + " file(s)?");
    message.setWindowTitle("Duplicate deleter");
    message.addButton(QMessageBox::Yes);
    message.addButton(QMessageBox::No);
    message.setDefaultButton(QMessageBox::No);

    if (message.exec() == QMessageBox::No)
    {
        return;
    }

    QVector<QTreeWidgetItem*> success, fail;
    for (QTreeWidgetItem* &item : selected)
    {
        QFile currentFile(currentDirectory + "/" + item->text(0));
        if (!currentFile.exists())
        {
            fail.push_back(item);
        }
        else
        {
            if (currentFile.remove())
            {
                success.push_back(item);
            }
            else
            {
                fail.push_back(item);
            }
        }
    }

    ui->treeWidget->clearSelection();

    for (QTreeWidgetItem* &item : success)
    {
        item->setBackgroundColor(0, Qt::darkGreen);
    }
    for (QTreeWidgetItem* &item : fail)
    {
        item->setBackgroundColor(0, Qt::darkRed);
    }

    ui->statusBar->showMessage(QString::number(success.size()) + " file(s) successfully removed; "
                               + QString::number(fail.size()) + " file(s) couldn't be removed");
}

void MainWindow::scanDirectory()
{
    //scan ui stuff

    ui->directory_button->setDisabled(true);
    ui->treeWidget->clear();
    if (currentDirectory.isEmpty())
    {
        QMessageBox::critical(this, "Duplicate deleter", "Please choose a directory");
        return;
    }
    if (ui->threads_lineedit->text().isEmpty())
    {
        ui->threads_lineedit->setText("4");
    }
    ui->threads_lineedit->setReadOnly(true);
    QString path = currentDirectory;
    ui->handle_button->setDisabled(true);

    //indexation

    QFuture<QVector<QString> > indexationFuture;
    indexationFuture = QtConcurrent::run(this, &MainWindow::requestFilesRecursive, path);
    ui->statusBar->showMessage("Indexation in progress, please wait...");
    QVector<QString> filePaths = indexationFuture.result();
    filesTotal = filePaths.size();

    //hashing

    hashingInProgress = true;
    ui->statusBar->showMessage("Hashing in progress, please wait...");
    QVector<QVector<QString> > distribution = distributeFiles(filePaths);

    QProgressDialog progressBar;
    progressBar.setWindowTitle("Duplicate deleter");
    progressBar.setLabelText("Hashing in progress...");
    progressBar.setRange(0, filesTotal);

    connect(&hashingWatcher, SIGNAL(progressValueChanged(int)), &progressBar, SLOT(setValue(int)));
    connect(&progressBar, SIGNAL(canceled()), &hashingWatcher, SLOT(cancel()));
    connect(&hashingWatcher, SIGNAL(finished()), &progressBar, SLOT(reset()));
    connect(&progressBar, SIGNAL(canceled()), this, SLOT(cancelHashing()));

    hashingWatcher.setFuture(QtConcurrent::map(distribution, [this](QVector<QString> &paths) { hashFileBundle(paths); } ));
    progressBar.exec();
    hashingWatcher.waitForFinished();

    //deletion ui stuff

    hashingInProgress = false;
    currentDirectoryScanned = true;
    ui->threads_lineedit->setReadOnly(false);
    ui->directory_button->setDisabled(false);
    ui->handle_button->setDisabled(false);
    ui->statusBar->showMessage("Select the files you want to delete");
    ui->handle_button->setText("Delete selected files");
}
