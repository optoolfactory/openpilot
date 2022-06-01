
#include <QDialog>
#include <QDateTime>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QVBoxLayout>

#include "selfdrive/ui/qt/params/steerview.h"



CSteerview::CSteerview(QWidget *parent) : QFrame(parent) 
{
  m_nSelect = 0;


  main_layout = new QVBoxLayout(this);
  main_layout->setMargin(0);


  hlayout = new QHBoxLayout;
  hlayout->setMargin(0);
  hlayout->setSpacing(20);

  // left icon 
  QString icon = "../assets/offroad/icon_menu.png";   // icon_minus.png icon_plus.png  icon_menu.png
  QPixmap pix(icon);
  QLabel *icon_label = new QLabel();
  icon_label->setPixmap(pix.scaledToWidth(80, Qt::SmoothTransformation));
  icon_label->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  hlayout->addWidget(icon_label);

  // title
  QString  title = "Steering Menu";
  title_label = new QPushButton(title);
  title_label->setFixedHeight(120);
  title_label->setStyleSheet("font-size: 50px; font-weight: 400; text-align: left");
  hlayout->addWidget(title_label);

  connect(title_label, &QPushButton::clicked, [=]() {
    m_nSelect += 1;
    if( m_nSelect > 1 )
      m_nSelect = 0;

    refresh();
  });

  // label
  label.setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
  label.setStyleSheet(R"(
  padding: 0;
  border-radius: 50px;
  font-size: 35px;
  font-weight: 500;
  color: #E4E4E4;
  background-color: #00A12E;
  )");  
  hlayout->addWidget(&label);
  main_layout->addLayout(hlayout);


  // 1. layer menu
  m_pChildFrame = new QFrame(); 
  m_pChildFrame->setContentsMargins(40, 10, 40, 50);
  m_pChildFrame->setStyleSheet(R"(
    * {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: black;
    } 
  )");
  
 main_layout->addWidget(m_pChildFrame);


  QVBoxLayout *menu_layout = new QVBoxLayout(m_pChildFrame);
 // menu_layout->setContentsMargins(32, 5, 32, 32);
  menu_layout->addWidget( new MaxSteerAngle() , 1, Qt::AlignTop | Qt::AlignHCenter ); 

  // Update + Cancel buttons
  QHBoxLayout *btn_layout = new QHBoxLayout();
  btn_layout->setSpacing(30);
  menu_layout->addLayout(btn_layout);

    QPushButton* confirm_btn = new QPushButton("confirm");
   // confirm_btn->setFixedHeight(120);
    confirm_btn->setStyleSheet(R"(
      QPushButton {
        height: 120px;
        border-radius: 15px;
        background-color: gray;
      }
      * {
        font-size: 50px; 
        font-weight: 400; 
        text-align: left;
      }
    )");  
    btn_layout->addWidget(confirm_btn);
    QObject::connect(confirm_btn, &QPushButton::clicked, [=]() {
     // pmyWidget->setVisible(false);
     m_pChildFrame->hide();
    });    



    QPushButton* cancel_btn = new QPushButton("cancel");
   // cancel_btn->setFixedHeight(120);
    cancel_btn->setStyleSheet(R"(
      QPushButton {
        height: 120px;
        border-radius: 15px;
        background-color: gray;
      }
      * {
        font-size: 50px; 
        font-weight: 400; 
        text-align: left;
      }
    )");  
    btn_layout->addWidget(cancel_btn);
    QObject::connect(cancel_btn, &QPushButton::clicked, [=]() {
     // pmyWidget->setVisible(false);
     m_pChildFrame->hide();
    });




  main_layout->addStretch();
  refresh();
}

CSteerview::~CSteerview()
{

}

void CSteerview::showEvent(QShowEvent *event) 
{
  refresh();
}

void CSteerview::hideEvent(QHideEvent *event) 
{

}

void CSteerview::refresh() 
{
  QString str;
  str.sprintf("NO[%d]", m_nSelect );

  label.setText( str );

  if(  m_nSelect == 0 )
  {
    // pmyWidget->setVisible(false);
    m_pChildFrame->hide();
  }
  else
  {
    m_pChildFrame->show();
    //pmyWidget->setVisible(true);
  }

}




MaxSteerAngle::MaxSteerAngle() 
  : AbstractControl("driver to openpilot Steer", 
                    "mprove the edge between the driver and the openpilot.", 
                    "../assets/offroad/icon_shell.png") 
{

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  auto str = QString::fromStdString( params.get("OpkrMaxSteerAngle") );
  float value = str.toDouble();
  m_dMaxSteerAngle = value;

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");

  btnminus.setFixedSize(150, 100);
  btnminus.setText("－");
  hlayout->addWidget(&btnminus);
  
  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    m_dMaxSteerAngle -= 10;
    if (m_dMaxSteerAngle <= 80) {
      m_dMaxSteerAngle = 80;
    }
    QString values = QString::number(m_dMaxSteerAngle);
    params.put("OpkrMaxSteerAngle", values.toStdString());

    refresh();
  });


  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setFixedSize(150, 100);
  btnplus.setText("＋");
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    m_dMaxSteerAngle += 10;
    if (m_dMaxSteerAngle >= 360) {
      m_dMaxSteerAngle = 360;
    }
    QString values = QString::number(m_dMaxSteerAngle);
    params.put("OpkrMaxSteerAngle", values.toStdString());
    refresh();
  });
  refresh();
}

void MaxSteerAngle::refresh() 
{
  if (m_dMaxSteerAngle <= 80 ) {
    label.setText(QString::fromStdString("NoLimit"));
  } else {
    QString values = QString::number(m_dMaxSteerAngle);
    label.setText( values );
  }
}