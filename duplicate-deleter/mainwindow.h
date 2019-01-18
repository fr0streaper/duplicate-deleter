#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QFile>
#include <QMap>
#include <QMutex>
#include <QThread>
#include <memory>
#include <QFuture>
#include <QFutureWatcher>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTreeWidgetItem* addGroup();
    QTreeWidgetItem* addGroupItem(QTreeWidgetItem *group, QString path);
    QString toHumanSize(quint64 value);
    void updateProgressBarAndTreeWidget(QVector<QPair<quint64, QByteArray> > &hashes, QVector<QString> &filePaths, quint64 &offset);

    QVector<QString> requestFilesRecursive(QString directoryPath);
    QByteArray hashFile(QFile &file);
    QVector<QVector<QString> > distributeFiles(QVector<QString> filePaths);
    void hashFileBundle(QVector<QString> &filePaths);

    void scanDirectory();
    void deleteSelected();

private slots:
    void selectDirectory();
    void handleDirectory();
    void confirmThreadCount();
    void cancelHashing();

private:
    Ui::MainWindow *ui;
    QMutex hashingMutex;
    quint64 filesHashed, filesTotal;

    QString currentDirectory;
    QMap<QPair<quint64, QByteArray>, QString> hashedFiles;
    QMap<QPair<quint64, QByteArray>, QTreeWidgetItem*> treePlacement;
    QFutureWatcher<void> hashingWatcher;
    bool hashingInProgress, currentDirectoryScanned, hashingCancelled;

    const qint64 BLOCK_SIZE = (1 << 12);
};

#endif // MAINWINDOW_H
