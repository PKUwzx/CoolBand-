#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include<QButtonGroup>
#include<qsoundeffect.h>
#include<QPalette>
#include<QtWidgets>
#include<QAction>
#include<QActionGroup>
#include<QPixmap>
#include<QDialog>
#include<QLabel>
#include<QLineEdit>
#include<QSoundEffect>
#include<QCoreApplication>
#include<QMessageBox>
#include<QKeyEvent>
#include<QIcon>
#include<QUrl>
#include<QDir>
#include<vector>
#include<QFont>
#include<cmath>
#include<cstring>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString path = QDir::currentPath();

private slots:
    void metronome_Activation();
    void metronome_SettingSlot();
    void metronomeOut();
    void openActionSlot();
    void saveActionSlot();
    void volumeActionSlot();
    void pianoKeySetting();
    void newActionSlot();
    void deleteActionSlot();


private:
    class MySoundEffect: public QSoundEffect{
    public:
        MySoundEffect(QObject *parent = nullptr): QSoundEffect(parent){}
        void play()
        {
            if (this->isPlaying()){
                QSoundEffect *tempEffect = new QSoundEffect;
                tempEffect->setVolume(this->volume());
                tempEffect->setSource(this->source());
                tempEffect->play();
            }
            else
            {
                this->QSoundEffect::play();
            }
        }
    };
    class keySettingDialog: public QDialog{
    private:
        int* key;
    public:
        keySettingDialog(QWidget *parent = nullptr, int *k = nullptr): QDialog(parent), key(k){}
        void keyPressEvent(QKeyEvent *k)
        {
            if((k->key() >= Qt::Key_0 && k->key() <= Qt::Key_9) || (k->key() >= Qt::Key_A && k->key() <= Qt::Key_Z))
            {
                *key = k->key();
                close();
            }
            else if(k->key() == Qt::Key_Escape)
            {
                close();
            }
            else
            {
                QMessageBox::warning(this,"警告","按键不合法！");
            }
            QDialog::keyPressEvent(k);
        }
        void run()
        {
            QLabel *tempL = new QLabel;
            tempL->setText(QString("你想要修改成的键位：(0-9, A-Z)"));
            QHBoxLayout *settingLayout = new QHBoxLayout;
            settingLayout->addWidget(tempL);
            this->setLayout(settingLayout);
            this->exec();

        }
    };
    class scoreSection: public QWidget //keyValue = -10000 = rest
    {
    private:
        QHBoxLayout *sectionLayout = new QHBoxLayout(this);
        QVBoxLayout *checkboxesLayout = new QVBoxLayout(this);
        QVBoxLayout *labelAndSpinboxLayout = new QVBoxLayout;
        QSpinBox *bpmSpinBox = new QSpinBox;
        QGraphicsScene *notesScene = new QGraphicsScene;
        QGraphicsView *notesView = new QGraphicsView(this);
        QVBoxLayout *buttonsLayout = new QVBoxLayout;
        QCheckBox *editButton = new QCheckBox;
        QPushButton *playButton = new QPushButton;
        QPushButton *pauseButton = new QPushButton;
        QPushButton *startOverButton = new QPushButton;
        QLabel *nameLabel = new QLabel(this);
        int bpm;
        class note{
        private:
            int timeValue;
            int keyValue;
        public:
            note(int t, int k): timeValue(t), keyValue(k) {};
            int getTime(){return timeValue;}
            int getKey(){return keyValue;}
        };
        class graphicNote: public QGraphicsItem {
        private:
            int timeValue;
            int keyValue;
            friend scoreSection;
            class MyGraphicsSimpleTextItem: public QGraphicsSimpleTextItem
            {
            public:
                MyGraphicsSimpleTextItem(QGraphicsItem *parent = nullptr, const QString &str = ""): QGraphicsSimpleTextItem(parent){
                    QPen pen(Qt::white);
                    this->setPen(pen);
                    this->setFont(QFont("Microsoft Yahei UI", 12, QFont::Bold));
                    this->setText(str);
                }
            };
            int numU = 0;
            int numDB = 0;
            int numDA = 0;
            bool hasS = false;
            QList<QGraphicsItem*> components;
            QGraphicsItemGroup *note = NULL;
        public:
            graphicNote(QGraphicsItem *parent = nullptr, QGraphicsScene *qS = nullptr, int t = -1, int k = -10000): timeValue(t), keyValue(k){
                //t一定是2^int, int max = 4
                MyGraphicsSimpleTextItem *graphic = NULL;
                int numUnderlines = 0;
                int numDotBelow = 0;
                int numDotAbove = 0;
                bool hasSharp = 0;
                int tt = timeValue, tk = keyValue;
                while(tt < 4)
                {
                    numUnderlines += 1, tt *= 2;
                }
                numU = numUnderlines;
                if(tk > -1 && tk < 100) //piano
                {
                    while(tk > 38) //大于B2
                    {
                        tk -= 12, numDotAbove += 1;
                    }
                    numDA = numDotAbove;
                    while(tk < 27) //小于C2
                    {
                        tk += 12, numDotBelow += 1;
                    }
                    numDB = numDotBelow;
                    tk -= 26;
                    hasSharp = tk == 2 || tk == 4 || tk == 7 || tk == 9 || tk == 11;
                    hasS = hasSharp;
                    QString baseText;
                    if(tk == 1 || tk == 2) baseText = "1";
                    else if (tk == 3 || tk == 4) baseText = "2";
                    else if (tk == 5) baseText = "3";
                    else if (tk == 6 || tk == 7) baseText = "4";
                    else if (tk == 8 || tk == 9) baseText = "5";
                    else if (tk == 10 || tk == 11) baseText = "6";
                    else if (tk == 12) baseText = "7";
                    graphic = new MyGraphicsSimpleTextItem(parent,baseText);
                    components.append(graphic);
                    if(hasSharp)
                    {
                        MyGraphicsSimpleTextItem *sharp = new MyGraphicsSimpleTextItem(graphic, "#");
                        sharp->moveBy(-6, 0);
                        sharp->setScale(0.75);
                        components.append(sharp);
                    }
                    for(int i  = 0; i < numUnderlines; i++)
                    {
                        MyGraphicsSimpleTextItem *underline = new MyGraphicsSimpleTextItem(graphic, "-");
                        underline->moveBy(2,10 + 5 * i);
                        components.append(underline);
                    }
                    for(int i = 0; i < numDotBelow; i++)
                    {
                        MyGraphicsSimpleTextItem *dot = new MyGraphicsSimpleTextItem(graphic, "·");
                        dot->moveBy(2, 12 + 5 * numUnderlines + 7 * i);
                        components.append(dot);
                    }
                    for(int i = 0; i< numDotAbove; i++)
                    {
                        MyGraphicsSimpleTextItem *dot = new MyGraphicsSimpleTextItem(graphic, "·");
                        dot->moveBy(2, -12 + -7 * i);
                        components.append(dot);
                    }
                }

                else if (tk == 2000) // 延长符
                {
                    graphic = new MyGraphicsSimpleTextItem(parent, "-");
                    components.append(graphic);
                }
                else if(tk == 5000) // 小节分隔符
                {
                    graphic = new MyGraphicsSimpleTextItem(parent, "|");
                    components.append(graphic);
                }
                else if(tk < -1)
                {
                    QString drumNames[8] = { "撞击镲", "踩镲", "骑镲", "小军鼓", "高音鼓", "中音鼓", "地面鼓", "低音鼓"};
                    graphic = new MyGraphicsSimpleTextItem(parent, "X");
                    components.append(graphic);
                    for(int i  = 0; i < numUnderlines; i++)
                    {
                        MyGraphicsSimpleTextItem *underline = new MyGraphicsSimpleTextItem(graphic, "-");
                        underline->moveBy(2,10 + 5 * i);
                        components.append(underline);
                    }
                    MyGraphicsSimpleTextItem *name = new MyGraphicsSimpleTextItem(parent, drumNames[tk + 9]);
                    name->setScale(0.75);
                    name->moveBy((tk + 9 == 2 || tk + 9) == 1 ? -6: -18, -50);
                    components.append(name);
                }
                else
                {
                    graphic = new MyGraphicsSimpleTextItem(parent, "0");
                    components.append(graphic);
                    for(int i  = 0; i < numUnderlines; i++)
                    {
                        MyGraphicsSimpleTextItem *underline = new MyGraphicsSimpleTextItem(graphic, "-");
                        underline->moveBy(2,10 + 5 * i);
                        components.append(underline);
                    }
                }
                note = qS->createItemGroup(components);
            }
            ~graphicNote() {}
            QGraphicsItemGroup* getNote()
            {
                return note;
            }
            QRectF boundingRect() const
            {
                return note->boundingRect();
            }
            void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
            {
                note->paint(painter, option, widget);
            }
            void moveBy(qreal dx, qreal dy)
            {
                note->moveBy(dx, dy);
            }
        };

        class bar{
        public:
        private:
            int length;
            int remainingLength;
        public:
            std::vector<note*> noteInBar;
            std::vector<note*>::iterator curNote;
            bar(int l): length(l) {
                curNote = noteInBar.begin();
                remainingLength = length;
            }
            ~bar()
            {
                if (!noteInBar.empty()) noteInBar.clear();
            }
            int addNote(int t, int k)
            {
                if (t <= remainingLength)
                {
                    noteInBar.push_back(new note(t, k));
                    remainingLength -= t;
                    return 0;
                }
                else
                {
                    noteInBar.push_back((new note(remainingLength, k)));
                    int remain = t - remainingLength;
                    remainingLength -= remainingLength;
                    return remain;
                }
            }
            void deleteNote()
            {
                note* note = noteInBar.back();
                remainingLength += note->getTime();
                noteInBar.pop_back();
            }
            bool hasSpace()
            {
                return remainingLength;
            }
            bool empty()
            {
                return noteInBar.empty();
            }
            int remainLength()
            {
                return remainingLength;
            }
            std::vector<note*>::iterator firstNote()
            {
                return noteInBar.begin();
            }
            std::vector<note*>::iterator end()
            {
                return noteInBar.end();
            }
        };
        int perBarLength;
        int type;
        qreal curDx;
        std::vector<bar*> bars;
        std::vector<bar*>::iterator curBar;
        std::vector<graphicNote*> graphicNotes;
        void graphicAddNote(int t, int k)
        {
            int timeValue = t, keyValue = k;
            int length[5] = {0,0,0,0,0};
            for(int i = 4; i >= 0; i--)
            {
                while(int(timeValue / int(pow(2, i))))
                {
                    timeValue -= int(pow(2,i));
                    length[i]++;
                }
            }
            for(int i = 4; i >= 0; i--)
            {
                while(length[i])
                {
                    if(i >= 2)
                    {
                        int enlen = int(pow(2,i-2)) - 1;
                        graphicNote *note = new graphicNote(nullptr,notesScene, 4, keyValue);
                        graphicNotes.push_back(note);
                        notesScene->addItem(note);
                        note->moveBy(curDx,0);
                        curDx += 30;
                        while(enlen)
                        {
                            graphicNote *note = new graphicNote(nullptr,notesScene, 4, 2000);
                            notesScene->addItem(note);
                            graphicNotes.push_back(note);
                            note->moveBy(curDx, 0);
                            curDx += 30;
                            enlen--;
                        }
                    }
                    else
                    {
                        graphicNote *note = new graphicNote(nullptr, notesScene, int(pow(2,i)), keyValue);
                        notesScene->addItem(note);
                        graphicNotes.push_back(note);
                        note->moveBy(curDx,0);
                        curDx += 30;
                    }
                    length[i]--;
                }
            }
        }
        QTimer *inBarTimer = new QTimer;
        QTimer *envTimer = new QTimer;
        friend QVBoxLayout;
    public:
        scoreSection(QWidget *parent = nullptr, const QString &str = "", int l = 16, int t = -1): QWidget(parent)
        {
            type = t;
            perBarLength = l;
            bpm = 60;
            curDx = 0;
            bars.push_back(new bar(perBarLength));
            curBar = bars.begin();
            nameLabel->setText(str);
            editButton->setText("编辑模式");
            bpmSpinBox->setMaximum(400), bpmSpinBox->setMinimum(10);
            bpmSpinBox->setPrefix("BPM:");
            bpmSpinBox->setSingleStep(2);
            bpmSpinBox->setValue(60);
            connect(bpmSpinBox, &QSpinBox::valueChanged, this, [=](){
                bpm = bpmSpinBox->value();
            });
            bpmSpinBox->setMinimumWidth(150);
            checkboxesLayout->addWidget(editButton);
            playButton->setGeometry(playButton->x(), playButton->y(), 30,30);
            pauseButton->setGeometry(pauseButton->x(), pauseButton->y(), 30,30);
            startOverButton->setGeometry(startOverButton->x(), startOverButton->y(), 30,30);
            playButton->setIcon(QIcon(QString("C:\\Users\\DamienWang\\Documents\\CoolBand\\icon\\")  + QString("start.png")));
            pauseButton->setIcon(QIcon(QString("C:\\Users\\DamienWang\\Documents\\CoolBand\\icon\\")  + QString("pause.png")));
            startOverButton->setIcon(QIcon(QString("C:\\Users\\DamienWang\\Documents\\CoolBand\\icon\\")  + QString("restart.png")));
            //connect(playButton, SIGNAL(pressed()), this, SLOT(play()));
            //connect(pauseButton, SIGNAL(pressed()), this, SLOT(pause()));
            //connect(startOverButton, SLOT(pressed()), this, SLOT(restart()));
            buttonsLayout->addWidget(playButton);
            buttonsLayout->addWidget(pauseButton);
            buttonsLayout->addWidget(startOverButton);
            labelAndSpinboxLayout->addWidget(nameLabel);
            labelAndSpinboxLayout->addWidget(bpmSpinBox);
            sectionLayout->addLayout(labelAndSpinboxLayout);
            sectionLayout->addLayout(checkboxesLayout);
            sectionLayout->addLayout(buttonsLayout);
            notesView->setScene(notesScene);
            sectionLayout->addWidget(notesView);
            editButton->setChecked(true);
        }
        ~scoreSection()
        {
            nameLabel->setVisible(false);
            editButton->setVisible(false);
            playButton->setVisible(false);
            this->setVisible(false);
            checkboxesLayout->removeWidget(editButton), checkboxesLayout->removeWidget(playButton);
            notesScene->clear();
            notesView->close();
        }
        void addNote(int t, int k)
        {
            int timeValue = t, keyValue = k;
            if(!(*curBar)->hasSpace())
            {
                graphicAddNote(4, 5000); //分隔符
                bars.push_back(new bar(perBarLength));
                curBar = bars.end() - 1;
            }
            while(timeValue > (*curBar)->remainLength())
            {
                timeValue -= (*curBar)->remainLength();
                graphicAddNote((*curBar)->remainLength(), k);
                (*curBar)->addNote((*curBar)->remainLength(), k);
                graphicAddNote(4, 5000); //分隔符
                bars.push_back(new bar(perBarLength));
                curBar = bars.end() - 1;
            }
            if(timeValue <= (*curBar)->remainLength())
            {
                graphicAddNote(timeValue, keyValue);
                (*curBar)->addNote(timeValue,keyValue);
            }
        }
        void deleteNote() //delete latest note
        {
            bool isEnlen = true;
            auto barIt = bars.end() - 1;
            if ((*barIt)->empty() && barIt > bars.begin()) bars.pop_back(), barIt--;
            if((*barIt)->empty() && barIt <= bars.begin()) return;
            while(isEnlen)
            {
                graphicNote* gNote = graphicNotes.back();
                if(gNote->keyValue == 2000 || gNote->keyValue == 5000) {
                    qDebug() << "deletion1";
                    gNote->note->setVisible(false);
                    isEnlen = true;
                    notesScene->removeItem(gNote->note);
                    graphicNotes.pop_back();
                    curDx -= 20;
                }
                else
                {
                    qDebug() << "deletion2";
                    gNote->note->setVisible(false);
                    isEnlen = false;
                    notesScene->removeItem(gNote->note);
                    graphicNotes.pop_back();
                    (*barIt)->deleteNote();
                    curDx -= 20;
                }
            }
        }
        int getType()
        {
            return type;
        }
        bool editable()
        {
            return editButton->isChecked();
        }
    };

    Ui::MainWindow *ui;
    //钢琴音效 + 架子鼓音效
    std::vector<scoreSection*> scores;
    std::vector<MySoundEffect*> pianoEffects;
    std::vector<MySoundEffect*> drumEffects;
    QButtonGroup *pianoButtons = new QButtonGroup;
    QButtonGroup *drumButtons = new QButtonGroup;
    int pianoKeys[88];
    int drumKeys[8] = {Qt::Key_Q, Qt::Key_A, Qt::Key_E, Qt::Key_Z, Qt::Key_S, Qt::Key_D, Qt::Key_C, Qt::Key_X};
    QSoundEffect metronomeStress;
    QSoundEffect metronomeClick;
    //用于地址
    QString audioPath = "C:\\Users\\DamienWang\\Documents\\CoolBand\\audio\\";
    QString iconPath = "C:\\Users\\DamienWang\\Documents\\CoolBand\\icon\\";
    QString sheetmusicPath = "C:\\Users\\DamienWang\\Documents\\CoolBand\\sheetmusic\\";
    //键盘事件（演奏）
    void keyPressEvent(QKeyEvent *k);
    //节拍器
    QTimer metronomeTimer;
    int metronomeTimeout; //一拍时长
    unsigned int metronomeCount; //节拍器节拍数
    unsigned short metronomeStressTime; //一小节拍数
    int noteLength = 4;
    QActionGroup *noteLengthActionGroup = new QActionGroup(this);
    void moreSetting(QWidget *p = nullptr);
    scoreSection *pianoScore = NULL;
    scoreSection *drumScore = NULL;
    bool isRest = false;
};

#endif // MAINWINDOW_H
