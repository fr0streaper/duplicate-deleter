/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionHandleDirectory;
    QAction *actionSelectDirectory;
    QAction *actionRequestConfirmThreadCount;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *main_vlayout;
    QHBoxLayout *directory_hlayout;
    QPushButton *directory_button;
    QLineEdit *directory_linedit;
    QHBoxLayout *threads_hlayout;
    QLabel *threads_label;
    QLineEdit *threads_lineedit;
    QPushButton *handle_button;
    QHBoxLayout *tree_hlayout;
    QTreeWidget *treeWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(512, 512);
        actionHandleDirectory = new QAction(MainWindow);
        actionHandleDirectory->setObjectName(QStringLiteral("actionHandleDirectory"));
        actionSelectDirectory = new QAction(MainWindow);
        actionSelectDirectory->setObjectName(QStringLiteral("actionSelectDirectory"));
        actionRequestConfirmThreadCount = new QAction(MainWindow);
        actionRequestConfirmThreadCount->setObjectName(QStringLiteral("actionRequestConfirmThreadCount"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        main_vlayout = new QVBoxLayout();
        main_vlayout->setSpacing(6);
        main_vlayout->setObjectName(QStringLiteral("main_vlayout"));
        directory_hlayout = new QHBoxLayout();
        directory_hlayout->setSpacing(5);
        directory_hlayout->setObjectName(QStringLiteral("directory_hlayout"));
        directory_hlayout->setContentsMargins(10, 10, 10, 0);
        directory_button = new QPushButton(centralWidget);
        directory_button->setObjectName(QStringLiteral("directory_button"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(directory_button->sizePolicy().hasHeightForWidth());
        directory_button->setSizePolicy(sizePolicy);
        directory_button->setFlat(false);

        directory_hlayout->addWidget(directory_button);

        directory_linedit = new QLineEdit(centralWidget);
        directory_linedit->setObjectName(QStringLiteral("directory_linedit"));
        directory_linedit->setAlignment(Qt::AlignCenter);
        directory_linedit->setReadOnly(true);

        directory_hlayout->addWidget(directory_linedit);


        main_vlayout->addLayout(directory_hlayout);

        threads_hlayout = new QHBoxLayout();
        threads_hlayout->setSpacing(5);
        threads_hlayout->setObjectName(QStringLiteral("threads_hlayout"));
        threads_hlayout->setContentsMargins(10, 0, 10, 0);
        threads_label = new QLabel(centralWidget);
        threads_label->setObjectName(QStringLiteral("threads_label"));

        threads_hlayout->addWidget(threads_label);

        threads_lineedit = new QLineEdit(centralWidget);
        threads_lineedit->setObjectName(QStringLiteral("threads_lineedit"));
        threads_lineedit->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        threads_lineedit->setInputMethodHints(Qt::ImhPreferNumbers);
        threads_lineedit->setAlignment(Qt::AlignCenter);

        threads_hlayout->addWidget(threads_lineedit);

        handle_button = new QPushButton(centralWidget);
        handle_button->setObjectName(QStringLiteral("handle_button"));

        threads_hlayout->addWidget(handle_button);


        main_vlayout->addLayout(threads_hlayout);

        tree_hlayout = new QHBoxLayout();
        tree_hlayout->setSpacing(6);
        tree_hlayout->setObjectName(QStringLiteral("tree_hlayout"));
        treeWidget = new QTreeWidget(centralWidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setAutoFillBackground(false);
        treeWidget->setAlternatingRowColors(false);
        treeWidget->setSelectionMode(QAbstractItemView::MultiSelection);
        treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

        tree_hlayout->addWidget(treeWidget);


        main_vlayout->addLayout(tree_hlayout);


        horizontalLayout->addLayout(main_vlayout);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);
        QObject::connect(directory_button, SIGNAL(clicked()), actionSelectDirectory, SLOT(trigger()));
        QObject::connect(handle_button, SIGNAL(clicked()), actionHandleDirectory, SLOT(trigger()));
        QObject::connect(threads_lineedit, SIGNAL(editingFinished()), actionRequestConfirmThreadCount, SLOT(trigger()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        actionHandleDirectory->setText(QApplication::translate("MainWindow", "HandleDirectory", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionHandleDirectory->setToolTip(QApplication::translate("MainWindow", "HandleDirectory", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionSelectDirectory->setText(QApplication::translate("MainWindow", "SelectDirectory", Q_NULLPTR));
        actionRequestConfirmThreadCount->setText(QApplication::translate("MainWindow", "RequestConfirmThreadCount", Q_NULLPTR));
        directory_button->setText(QApplication::translate("MainWindow", "Select directory", Q_NULLPTR));
        threads_label->setText(QApplication::translate("MainWindow", "Number of threads", Q_NULLPTR));
        threads_lineedit->setPlaceholderText(QApplication::translate("MainWindow", "1-99; 4 by default", Q_NULLPTR));
        handle_button->setText(QApplication::translate("MainWindow", "Scan directory for duplicates", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
