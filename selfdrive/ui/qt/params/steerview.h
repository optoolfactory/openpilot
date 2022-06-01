

#pragma once

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>
#include <QTimer>
#include <QWidget>

#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/ui.h"

class MaxSteerAngle : public AbstractControl {
  Q_OBJECT

public:
  MaxSteerAngle();

private:
  void refresh();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;

  float  m_dMaxSteerAngle;
  
  


};

class CSteerview : public QFrame 
{
  Q_OBJECT

public:
  explicit CSteerview(QWidget *parent = 0);
  ~CSteerview();

private:
  void showEvent(QShowEvent *event) override;
  void hideEvent(QHideEvent *event) override;

public slots:  
  void refresh();


 private:

  QVBoxLayout *main_layout;
  QPushButton *title_label;
  QHBoxLayout *hlayout;
  QLabel *description = nullptr;  


  QLabel label;
  int    m_nSelect;

   QFrame *m_pChildFrame; 
};

