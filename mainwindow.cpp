#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //鼓初始化音效
    // 鼓按钮参数
    int drumGeometry[8][4] = {{0,0,450,240},
                               {40,250,360,210},
                               {1160,-10,440,400},
                               {130,470,420,310},
                               {460,90,260,260},
                               {830,80,280,270},
                               {1030,410,490,350},
                              {560,480,430,430}};
    QString drumNames[8] = {"leftcrash","hihat", "ride", "snare", "hightom", "midtom", "floortom", "bass" };
    QPixmap drumsetPixmap(iconPath + "drumset.jpg");
    QPalette drumPalette;
    ui->drumScrollAreaWidgetContents->setGeometry(0,0,1600,800);
    drumPalette.setBrush(ui->drumScrollAreaWidgetContents->backgroundRole(), QBrush(drumsetPixmap));
    ui->drumScrollAreaWidgetContents->setPalette(drumPalette);
    for (int i = 0; i < 8; i++)
    {
        drumEffects.push_back(new MySoundEffect);
        drumEffects[i]->setSource(QUrl::fromLocalFile(audioPath + drumNames[i] + ".wav"));
        QPushButton *drumButton = new QPushButton(ui->drumkitTab);
        connect(drumButton, &QPushButton::pressed, this, [=](){
            if(drumScore != NULL && drumScore->editable()) drumScore->addNote(noteLength, isRest? 5000 : -9 + i);
            drumEffects[i]->play();
        });
        drumButton->setGeometry(drumGeometry[i][0],drumGeometry[i][1],drumGeometry[i][2], drumGeometry[i][3]);
        drumButton->setText(QString(char(drumKeys[i] - Qt::Key_A + 'A')));
        drumButton->setStyleSheet("QPushButton {background:transparent; border: none; color: rgb(0,0,0);} "
                                  "QPushButton:focus{outline:none;}");
        drumButton->setFont(QFont("Microsoft Yahei UI", 24, QFont::Bold));
        drumButtons->addButton(drumButton), drumButtons->setId(drumButton, i + 1);
    };
    //钢琴初始化
    int white = 0;
    memset(pianoKeys,0,sizeof(pianoKeys));
    pianoKeys[27] = Qt::Key_A, pianoKeys[28] = Qt::Key_W, pianoKeys[29] = Qt::Key_S, pianoKeys[30] = Qt::Key_E,
    pianoKeys[31] = Qt::Key_D, pianoKeys[32] = Qt::Key_F, pianoKeys[33] = Qt::Key_T, pianoKeys[34] = Qt::Key_G,
    pianoKeys[35] = Qt::Key_Y, pianoKeys[36] = Qt::Key_H, pianoKeys[37] = Qt::Key_U, pianoKeys[38] = Qt::Key_J,
    pianoKeys[39] = Qt::Key_K;
    ui->pianoScrollAreaWidgetContents->setGeometry(0,0,4400,581); //钢琴大小
    for(int i = 1; i <= 88; i++) //钢琴音效设置
    {
        pianoEffects.push_back(new MySoundEffect);
        pianoEffects[i - 1]->setSource(QUrl::fromLocalFile(audioPath + "piano" + QString::number(i) + ".wav"));
        QPushButton *pianoButton = new QPushButton(ui->pianoScrollAreaWidgetContents);
        if(i % 12 == 1|| i % 12 == 3|| i %12 == 4 || i % 12 == 6 || i % 12 == 8 || i % 12 == 9|| i % 12 == 11)
        {
            pianoButton->setGeometry(0 + white++ * 82, 0, 80, 580);
            pianoButton->setStyleSheet("QPushButton{color:rgb(0, 0, 0); background-color: rgba(255,255,255,255); }"
                                       "QPushButton:focus{outline:none;}");
            QPalette whiteKey;
            pianoButton->setPalette(whiteKey);
        }
        else
        {
            pianoButton->setGeometry(0 + (white - 1) * 82 + 51, 0, 60, 340);
            pianoButton->setStyleSheet("QPushButton {background-color: rgba(0, 0, 0, 255); border: none;} "
                                       "QPushButton:hover{background-color: rgba(40, 40, 40, 255);}"
                                       "QPushButton:pressed{background-color: rgba(128, 129, 130, 255);}"
                                       "QPushButton:focus{outline:none;}");
            QPalette blackKey;
            pianoButton->setPalette(blackKey);
        }
        connect(pianoButton,&QPushButton::pressed, this, [=](){
            if(pianoScore && pianoScore->editable()) pianoScore->addNote(noteLength, isRest? 5000: i - 1);
            pianoEffects[i-1]->play();
        });
        pianoButton->setText(pianoKeys[i - 1] != 0 ? QString(char(pianoKeys[i-1] - Qt::Key_A + 'A')): " "); //钢琴键位显示
        pianoButtons->addButton(pianoButton);
        pianoButtons->setId(pianoButton,i);
    }
    for(int i = 1; i <= 88; i++)
    {
        if(!(i % 12 == 1|| i % 12 == 3|| i %12 == 4 || i % 12 == 6 || i % 12 == 8 || i % 12 == 9|| i % 12 == 11))
        {
            pianoButtons->button(i)->raise(); //将黑键优先级调高，显示在白键上方
        }
    }
    ui->pianoScrollArea->horizontalScrollBar()->setValue(743); //初始设置钢琴位置在C2
    //节拍器
    connect(ui->metronomeAction, SIGNAL(triggered()), this, SLOT(metronome_Activation())); //打开节拍器槽
    connect(ui->metronomeSettingAction, SIGNAL(triggered()), this, SLOT(metronome_SettingSlot())); //设置节拍器槽
    connect(ui->newAction, SIGNAL(triggered()), this, SLOT(newActionSlot()));
    connect(ui->pianoAction, SIGNAL(triggered()), this, SLOT(pianoKeySetting())); //设置钢琴键位槽
    connect(&metronomeTimer, SIGNAL(timeout()),this, SLOT(metronomeOut())); //节拍器发出声音槽
    connect(ui->deleteAction, SIGNAL(triggered()), this, SLOT(deleteActionSlot()));
    noteLengthActionGroup->addAction(ui->fullAction), noteLengthActionGroup->addAction(ui->halfAction),
        noteLengthActionGroup->addAction(ui->quarterAction), noteLengthActionGroup->addAction(ui->eighthAction),
        noteLengthActionGroup->addAction(ui->sixteenthAction);
    noteLengthActionGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);
    connect(ui->fullAction, &QAction::triggered, this, [=](){noteLength = 16;});
    connect(ui->halfAction, &QAction::triggered, this, [=](){noteLength = 8;});
    connect(ui->quarterAction, &QAction::triggered, this, [=](){noteLength = 4;});
    connect(ui->eighthAction, &QAction::triggered, this, [=](){noteLength = 2;});
    connect(ui->sixteenthAction, &QAction::triggered, this, [=](){noteLength = 1;});
    ui->quarterAction->setChecked(true), noteLength = 4;
    connect(ui->restAction, &QAction::triggered, this, [=](){isRest = ui->restAction->isChecked();});
    metronomeStress.setSource(QUrl::fromLocalFile(audioPath + "metronomeStress.wav")); //节拍器声音
    metronomeClick.setSource(QUrl::fromLocalFile(audioPath + "metronomeClick.wav"));
    metronomeTimeout = 1000;
    metronomeStressTime = 4;
    //初始化图片信息

    //架子鼓部分槽
    connect(ui->openAction, &QAction::triggered, this, &MainWindow::openActionSlot);
    connect(ui->saveAction, &QAction::triggered, this, &MainWindow::saveActionSlot);
    connect(ui->volumeAction, &QAction::triggered, this, &MainWindow::volumeActionSlot);
    ui->instrumentTabWidget->setCurrentWidget(ui->pianoTab);
    scoreSection *newScore = new scoreSection(this, "钢琴", 16, 0);
    pianoScore = newScore;
    scores.push_back(newScore);
    ui->scoreVerticalLayout->addWidget(newScore);
    //鼓键位显示
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newActionSlot()
{
    QDialog *newActionDialog = new QDialog;
    QVBoxLayout *totalLayout = new QVBoxLayout;
    QHBoxLayout *mainLayout= new QHBoxLayout;
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QLabel *infoLabel = new QLabel;
    infoLabel->setText("选择想添加乐谱的乐器和时值：");
    QVBoxLayout *instruments= new QVBoxLayout;
    QVBoxLayout *times = new QVBoxLayout;
    QRadioButton *pianoRadioButton = new QRadioButton;
    pianoRadioButton->setText("钢琴");
    QRadioButton *drumRadioButton= new QRadioButton;
    drumRadioButton->setText("架子鼓");
    QRadioButton *time1RadioButton = new QRadioButton;
    time1RadioButton->setText("2/4");
    QRadioButton *time2RadioButton = new QRadioButton;
    time2RadioButton->setText("3/4");
    QRadioButton *time3RadioButton = new QRadioButton;
    time3RadioButton->setText("4/4");
    QRadioButton *time4RadioButton = new QRadioButton;
    time4RadioButton->setText("6/4");
    QButtonGroup *instrumentButtons = new QButtonGroup;
    instrumentButtons->addButton(pianoRadioButton), instrumentButtons->addButton(drumRadioButton);
    QButtonGroup *timeButtons = new QButtonGroup;
    timeButtons->addButton(time1RadioButton), timeButtons->addButton(time2RadioButton);
    timeButtons->addButton(time3RadioButton), timeButtons->addButton(time4RadioButton);
    instruments->addWidget(pianoRadioButton), instruments->addWidget(drumRadioButton);
    times->addWidget(time1RadioButton), times->addWidget(time3RadioButton);
    times->addWidget(time2RadioButton), times->addWidget(time4RadioButton);
    mainLayout->addLayout(instruments), mainLayout->addLayout(times);
    QPushButton *confirmButton = new QPushButton, *cancelButton = new QPushButton;
    confirmButton->setText("确认"), cancelButton->setText("取消");
    buttonLayout->addWidget(confirmButton), buttonLayout->addWidget(cancelButton);
    const QString names[2] = {"钢琴","架子鼓"};
    connect(confirmButton, &QPushButton::pressed, newActionDialog, [=](){ newActionDialog->accept();});
    connect(cancelButton, &QPushButton::pressed, newActionDialog, [=](){ newActionDialog->reject();});
    connect(newActionDialog, &QDialog::accepted, this, [=](){
        int idx = 0, l = 16;
        if(pianoRadioButton->isChecked()) idx = 0;
        else if (drumRadioButton->isChecked()) idx = 1;
        else idx = -1;
        if (time1RadioButton->isChecked()) l = 8;
        else if(time2RadioButton->isChecked()) l =12;
        else if(time3RadioButton->isChecked()) l = 16;
        else if(time4RadioButton->isChecked()) l = 24;
        else l = -1;
        if(idx != -1 && l != -1)
            {
            if(!scores.empty()) for (scoreSection *sS: scores)
            {
                if (sS->getType() == idx)
                {
                    QMessageBox::warning(this,"警告","已经存在乐器！");
                    return;
                }
            }
            scoreSection *newScore = new scoreSection(this, names[idx], l, idx);
            if(!idx) pianoScore = newScore;
            else drumScore = newScore;
            scores.push_back(newScore);
            ui->scoreVerticalLayout->addWidget(newScore);
            }
        else
                {
            QMessageBox::warning(newActionDialog, "警告","非法操作！\n（未选择乐器或时值）");
                return;
        }
        });

    totalLayout->addWidget(infoLabel), totalLayout->addLayout(mainLayout), totalLayout->addLayout(buttonLayout);
    newActionDialog->setLayout(totalLayout);
    newActionDialog->exec();
}

void MainWindow::deleteActionSlot()
{
    bool hasPianoScore = false, hasDrumScore = false;
    std::vector<scoreSection*>::iterator pianoidx, drumidx;
    QDialog *deleteActionDialog = new QDialog;
    QVBoxLayout *deleteButtons = new QVBoxLayout;
    QLabel *infoLabel = new QLabel;
    infoLabel->setText("请选择你要删除的工程：");
    QPushButton *pianoButton = new QPushButton;
    pianoButton->setText("钢琴");
    QPushButton *drumButton = new QPushButton;
    drumButton->setText("架子鼓");
    deleteButtons->addWidget(infoLabel), deleteButtons->addWidget(pianoButton), deleteButtons->addWidget(drumButton);
    deleteActionDialog->setLayout(deleteButtons);
    if(scores.empty()) hasDrumScore = false, hasPianoScore = false;
    else for(int i = 0; i < int(scores.size()); i++)
        {
            if(scores[i]->getType() == 0)
            {
                hasPianoScore = true, pianoidx = scores.begin() + i;
            }
            if(scores[i]->getType() == 1)
            {
                hasDrumScore = true, drumidx = scores.begin() + i;
            }
        }
    pianoButton->setEnabled(hasPianoScore), drumButton->setEnabled(hasDrumScore);
    connect(pianoButton, &QPushButton::pressed, deleteActionDialog, [=](){
        QMessageBox::StandardButton button = QMessageBox::question(deleteActionDialog, "删除工程", "您确定要删除钢琴工程？",
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::NoButton);
        if (button == QMessageBox::StandardButton::Yes) {
            ui->scoreVerticalLayout->removeWidget((*pianoidx));
            (*pianoidx)->setVisible(false);
            scores.erase(pianoidx);
            pianoButton->setEnabled(false);
            drumScore = NULL;
        } else if (button == QMessageBox::StandardButton::No) {
            return;
        }
    });
    connect(drumButton, &QPushButton::pressed, deleteActionDialog, [=](){
        QMessageBox::StandardButton button = QMessageBox::question(deleteActionDialog, "删除工程", "您确定要删除架子鼓工程？",
                                                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::NoButton);
        if (button == QMessageBox::StandardButton::Yes) {
            ui->scoreVerticalLayout->removeWidget((*drumidx));
            (*drumidx)->setVisible(false);
            scores.erase(drumidx);
            drumButton->setEnabled(false);
            drumScore = NULL;
        } else if (button == QMessageBox::StandardButton::No) {
            return;
        }
    });
    deleteActionDialog->exec();
}


void MainWindow::keyPressEvent(QKeyEvent *k)
{
    if(k->modifiers() == Qt::ControlModifier)
    {
        if(k->key() == Qt::Key_N) newActionSlot();
        if(k->key() == Qt::Key_S) saveActionSlot();
        if(k->key() == Qt::Key_D) deleteActionSlot();
        if(k->key() == Qt::Key_O) openActionSlot();
        if(k->key() == Qt::Key_V) volumeActionSlot();
    }
    else if(k->modifiers() == Qt::AltModifier){
        if(k->key() == Qt::Key_M) metronome_Activation();
        if(k->key() == Qt::Key_F) ui->fullAction->trigger();
        if(k->key() == Qt::Key_H) ui->halfAction->trigger();
        if(k->key() == Qt::Key_Q) ui->quarterAction->trigger();
        if(k->key() == Qt::Key_E) ui->eighthAction->trigger();
        if(k->key() == Qt::Key_S) ui->sixteenthAction->trigger();
        if(k->key() == Qt::Key_R) ui->restAction->trigger();

    }
    else
    {
        if(k->key() == Qt::Key_Backspace)
        {
            for (scoreSection *s: scores)
            {
                if (s->editable()) s->deleteNote();
            }
        }
        else if(ui->pianoTab->isVisible())
        {
            for(int i = 0; i < 88; i++)
            {
                if(k->key() == pianoKeys[i])
                {
                    pianoButtons->button(i+1)->animateClick();
                }
            }
        }
        else if(ui->drumkitTab->isVisible())
        {
            for(int i = 0; i < 8; i++)
            {
                if(k->key() == drumKeys[i])
                {
                    drumButtons->button(i+1)->animateClick();
                }
            }
        }
    }
}


void MainWindow::openActionSlot()
{
    QString fileName = QFileDialog::getOpenFileName(this,"打开乐谱", QCoreApplication::applicationFilePath(), "乐谱文件 (*.ypu)");
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"警告","未选择文件！");
        return;
    }
    else
    {

    }
}

void MainWindow::saveActionSlot()
{
    QString fileName = QFileDialog::getSaveFileName(this,"请填写文件名", QCoreApplication::applicationFilePath(), "乐谱文件 (*.ypu)" );
        if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"警告","未选择文件！");
        return;
    }
    else
    {

    }
}

//27 = C2
void MainWindow::pianoKeySetting()
{
    QDialog *keySettingDialog = new QDialog(this);
    keySettingDialog->setWindowTitle("键位设置");
    QTabWidget *keySettingTab = new QTabWidget;
    QVBoxLayout *totalLayout = new QVBoxLayout;
    QHBoxLayout *pianoLayout = new QHBoxLayout;
    QVBoxLayout *drumLayout = new QVBoxLayout;
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QVBoxLayout *vPKeyLayout1 = new QVBoxLayout;
    QVBoxLayout *vPKeyLayout2 = new QVBoxLayout;
    QVBoxLayout *vDKeyLayout1 = new QVBoxLayout;
    QVBoxLayout *vDKeyLayout2 = new QVBoxLayout;


    QString pKeyStr[13] = {"C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2", "C3"};
    QString dKeyStr[8] = { "撞击镲", "踩镲", "骑镲", "小军鼓", "高音鼓", "中音鼓", "地面鼓", "低音鼓"};
    for(int i = 0; i < 13; i++)
    {
        QHBoxLayout *tempLayout = new QHBoxLayout;
        QLabel *tempLabel = new QLabel;
        QPushButton *tempButton = new QPushButton;
        tempLabel->setText(pKeyStr[i]);
        tempButton->setText(QString(char(pianoKeys[27 + i] - Qt::Key_A + 'A')));
        connect(tempButton, &QPushButton::pressed, keySettingDialog, [=](){
            class keySettingDialog *tempDialog = new class keySettingDialog(keySettingDialog, &pianoKeys[27+i]);
            tempDialog->run();
            tempButton->setText(QString(char(pianoKeys[27+i] - Qt::Key_A + 'A')));
        });
        tempLayout->addWidget(tempLabel), tempLayout->addWidget(tempButton);
        if(i < 7) vPKeyLayout1->addLayout(tempLayout);
        else vPKeyLayout2->addLayout(tempLayout);
    }
    for(int i = 0; i < 8; i++)
    {
        QHBoxLayout *tempLayout = new QHBoxLayout;
        QLabel *tempLabel = new QLabel;
        QPushButton *tempButton = new QPushButton;
        tempLabel->setText(dKeyStr[i]);
        tempButton->setText(QString(char(drumKeys[i] - Qt::Key_A + 'A')));
        connect(tempButton, &QPushButton::pressed, keySettingDialog, [=](){
            class keySettingDialog *tempDialog = new class keySettingDialog(keySettingDialog, &drumKeys[i]);
            tempDialog->run();
            tempButton->setText(QString(char(drumKeys[i] - Qt::Key_A + 'A')));
        });
        tempLayout->addWidget(tempLabel), tempLayout->addWidget(tempButton);
        if(i < 4) vDKeyLayout1->addLayout(tempLayout);
        else vDKeyLayout2->addLayout(tempLayout);
    }
    pianoLayout->addLayout(vPKeyLayout1), pianoLayout->addLayout(vPKeyLayout2);
    drumLayout->addLayout(vDKeyLayout1), drumLayout->addLayout(vDKeyLayout2);
    QWidget *pianoKey = new QWidget;
    QWidget *drumKey = new QWidget;
    pianoKey->setLayout(pianoLayout);
    drumKey->setLayout(drumLayout);
    keySettingTab->addTab(pianoKey,"钢琴键位设置");
    keySettingTab->addTab(drumKey,"架子鼓键位设置");
    QPushButton *returnButton = new QPushButton;
    returnButton->setText("返回");
    QPushButton *moreSettingButton = new QPushButton;
    moreSettingButton->setText("钢琴更多设置...");
    connect(returnButton,&QPushButton::pressed, keySettingDialog, [=](){
        keySettingDialog->close();
    });
    connect(moreSettingButton, &QPushButton::pressed, keySettingDialog,[=](){
        moreSetting(keySettingDialog);
    });
    buttonLayout->addWidget(moreSettingButton), buttonLayout->addWidget(returnButton);
    totalLayout->addWidget(keySettingTab), totalLayout->addLayout(buttonLayout);
    keySettingDialog->setLayout(totalLayout);
    keySettingDialog->exec();
    for(int i = 0; i < 88; i++) pianoButtons->button(i +1)->setText(pianoKeys[i] != 0 ? QString(char(pianoKeys[i] - Qt::Key_A + 'A')): " ");
    for(int i = 0; i < 8; i++) drumButtons->button(i+1)->setText(QString(char(drumKeys[i] - Qt::Key_A + 'A')));
}

void MainWindow::moreSetting(QWidget *p)
{
    QDialog *moreSettingDialog = new QDialog(p);
    moreSettingDialog->setWindowTitle("钢琴设置");
    QHBoxLayout *msLayout = new QHBoxLayout;
    QComboBox *rangeCombo = new QComboBox;
    QComboBox *noteCombo = new QComboBox;
    QPushButton *settingButton = new QPushButton;
    QStringList fullNotes = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    rangeCombo->clear();
    rangeCombo->insertItems(0,{"音域","-1","0","1","2","3","4","5","6","7"});
    connect(rangeCombo,&QComboBox::activated, moreSettingDialog, [=](){
        noteCombo->clear();

        noteCombo->addItem("音符");
        if (rangeCombo->currentIndex() == 0) ;
        else if(rangeCombo->currentIndex() == 1) noteCombo->insertItems(1,{"A","A#","B"});
        else if(rangeCombo -> currentIndex() == 9) noteCombo->insertItem(1,"C");
        else noteCombo->insertItems(1,fullNotes);
    });
    connect(noteCombo, &QComboBox::activated, moreSettingDialog, [=](){
        int i  = 0;
        if(rangeCombo->currentIndex() == 1) i = noteCombo->currentIndex() - 1;
        else i = (rangeCombo->currentIndex() - 2) * 12 + 3 + noteCombo->currentIndex() - 1;
        if (rangeCombo->currentIndex() != 0 && noteCombo->currentIndex() != 0)
            settingButton->setText(pianoKeys[i] != 0 ? QString(char(pianoKeys[i] - Qt::Key_A + 'A')): " ");
    });
    connect(settingButton, &QPushButton::pressed, moreSettingDialog, [=](){
        int i  = 0;
        if(rangeCombo->currentIndex() == 1) i = noteCombo->currentIndex() - 1;
        else i = (rangeCombo->currentIndex() - 2) * 12 + 3 + noteCombo->currentIndex() - 1;
        settingButton->setText(pianoKeys[i] != 0 ? QString(char(pianoKeys[i] - Qt::Key_A + 'A')): " ");
        class keySettingDialog *tempDialog = new class keySettingDialog(moreSettingDialog, &pianoKeys[i]);
        tempDialog->run();
        settingButton->setText(QString(char(pianoKeys[i] - Qt::Key_A + 'A')));
    });
    settingButton->setText("音符按键");
    msLayout->addWidget(rangeCombo), msLayout->addWidget(noteCombo), msLayout->addWidget(settingButton);
    moreSettingDialog->setLayout(msLayout);
    moreSettingDialog->exec();
}

void MainWindow::volumeActionSlot()
{
    QDialog *volumeSettingDialog = new QDialog(this);
    QTabWidget *volumeSettingTab = new QTabWidget;
    QWidget *pianoVol = new QWidget;
    QWidget *drumVol = new QWidget;
    QWidget *metroVol = new QWidget;
    QHBoxLayout *pianoLayout = new QHBoxLayout;
    QHBoxLayout *drumLayout = new QHBoxLayout;
    QHBoxLayout *metroLayout = new QHBoxLayout;
    QVBoxLayout *pianoV = new QVBoxLayout;
    QVBoxLayout *metroV  = new QVBoxLayout;
    QSlider *pianoVolSlider = new QSlider(Qt::Orientation::Vertical, volumeSettingDialog); pianoVolSlider->setValue(pianoEffects[0]->volume() * 100);
    QSlider *metroVolSlider = new QSlider(Qt::Orientation::Vertical, volumeSettingDialog); metroVolSlider->setValue(metronomeStress.volume() * 100);
    std::vector<QSlider*> drumSliders;
    //leftcrash, hihat, ride, snare, hightom, midtom, floortom, bass
    QSlider *totalDrumSlider = new QSlider;
    totalDrumSlider->setValue(50);
    drumSliders.push_back(totalDrumSlider);
    for(int i = 0; i < 8; i++)
    {
        QSlider *drumVolSlider = new QSlider;
        drumVolSlider->setValue(drumEffects[i]->volume()*100);
        drumSliders.push_back(drumVolSlider);
    }
    QString drumStrings[9] = {"总音量", "撞击镲", "踩镲", "骑镲", "小军鼓", "高音鼓", "中音鼓", "地面鼓", "低音鼓"};
    std::vector<QLabel*> drumLabels;
    for(int i = 0; i < 9; i++)
    {
        QLabel *drumLabel = new QLabel;
        drumLabel->setText(drumStrings[i]);
        drumLabels.push_back(drumLabel);
    }
    for(int i  =0; i < 9; i++)
    {
        QVBoxLayout *drumV = new QVBoxLayout;
        drumV->addWidget(drumSliders[i]); drumV->addWidget(drumLabels[i]);
        drumLayout->addLayout(drumV);
    }
    int pianoVolID = volumeSettingTab->addTab(pianoVol,"钢琴音量设置");
    int drumVolID = volumeSettingTab->addTab(drumVol,"架子鼓音量设置");
    int metroVolID = volumeSettingTab->addTab(metroVol,"节拍器音量设置");
    QLabel *pianoL = new QLabel;
    QLabel *metroL = new QLabel;
    pianoL->setText("钢琴");
    metroL->setText("节拍器");
    pianoV->addWidget(pianoVolSlider);
    metroV->addWidget(metroVolSlider);
    pianoV->addWidget(pianoL);
    metroV->addWidget(metroL);
    pianoLayout->addLayout(pianoV);
    metroLayout->addLayout(metroV);
    connect(pianoVolSlider,&QSlider::valueChanged,this,[=](){
        for(QSoundEffect *s: pianoEffects)
        {
            s->setVolume(double(pianoVolSlider->value())  / 100);
            qDebug() << s->volume();
        }
    });
    connect(metroVolSlider,&QSlider::valueChanged,this,[=](){
        metronomeClick.setVolume(double(metroVolSlider->value()) / 100);
        metronomeStress.setVolume(double(metroVolSlider->value()) / 100);});
    for(int i = 0; i < 9; i++)
    {
        connect(drumSliders[i], &QSlider::valueChanged, this, [=](){
            drumEffects[i]->setVolume(double(drumSliders[i]->value() / 100));
        });
    };
    volumeSettingTab->widget(pianoVolID)->setLayout(pianoLayout);
    volumeSettingTab->widget(drumVolID)->setLayout(drumLayout);
    volumeSettingTab->widget(metroVolID)->setLayout(metroLayout);
    QVBoxLayout *settingLayout = new QVBoxLayout;
    settingLayout->addWidget(volumeSettingTab);
    volumeSettingDialog->setLayout(settingLayout);
    volumeSettingDialog->exec();
}
//槽 节拍器开启/关闭
void MainWindow::metronome_Activation()
{
    if(ui->metronomeAction->isChecked())
    {
        metronomeCount = 0;
        metronomeTimer.start(metronomeTimeout);
    }
    else
    {
        metronomeTimer.stop();
    }
}
//槽 节拍器设置
void MainWindow::metronome_SettingSlot()
{
    QDialog *metronomeSettingDialog = new QDialog(this);
    metronomeSettingDialog->setAttribute(Qt::WA_DeleteOnClose);
    metronomeSettingDialog->setWindowTitle("节拍器设置");
    QLabel *bpmLabel = new QLabel;
    QLineEdit *bpmEdit = new QLineEdit;
    QLabel *countLabel = new QLabel;
    QLineEdit *countEdit = new QLineEdit;
    QPushButton *confirmButton = new QPushButton;
    QPushButton *cancelButton = new QPushButton;
    bpmLabel->setText("设置bpm:");
    countLabel->setText("一小节拍数:");
    bpmEdit->setText(QString::number(60000 / metronomeTimeout));
    countEdit->setText(QString::number(metronomeStressTime));
    confirmButton->setText("确定");
    cancelButton->setText("取消");
    connect(confirmButton,SIGNAL(pressed()),metronomeSettingDialog,SLOT(accept()));
    connect(cancelButton,SIGNAL(pressed()),metronomeSettingDialog,SLOT(reject()));
    connect(metronomeSettingDialog, &QDialog::accepted, this, [=](){
        qDebug() << 1;
        int newBPM = bpmEdit->text().toInt();
        if(newBPM < 10) { QMessageBox::warning(metronomeSettingDialog,"警告","bpm最低为10!"); return; }
        else if(newBPM > 400) { QMessageBox::warning(metronomeSettingDialog,"警告","bpm最高为400!"); return; }
        int newStressTime = countEdit->text().toInt();
        if(newStressTime < 1) { QMessageBox::warning(metronomeSettingDialog,"警告","一小节最少为1拍!"); return; }
        else if(newStressTime > 12) { QMessageBox::warning(metronomeSettingDialog,"警告","一小节最少为12拍!"); return; }
        metronomeTimeout = 60000 / newBPM;
        metronomeStressTime = newStressTime;
        //重新开始节拍器
        metronomeTimer.stop();
        metronome_Activation();
    });
    QHBoxLayout *bpm = new QHBoxLayout;
    bpm->addWidget(bpmLabel);
    bpm->addWidget(bpmEdit);
    QHBoxLayout *count = new QHBoxLayout;
    count->addWidget(countLabel);
    count->addWidget(countEdit);
    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->addWidget(confirmButton);
    buttons->addWidget(cancelButton);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(bpm);
    layout->addLayout(count);
    layout->addLayout(buttons);
    metronomeSettingDialog->setLayout(layout);
    metronomeSettingDialog->exec();
}
//槽 节拍器发声
void MainWindow::metronomeOut()
{
    if(metronomeCount == 0)
    {
        metronomeStress.play();
    }
    else
    {
        metronomeClick.play();
    }
    metronomeCount++;
    if(metronomeCount >= metronomeStressTime)
        metronomeCount = 0;
}
