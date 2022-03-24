#include "selfdrive/ui/qt/widgets/opkr.h"

#include <algorithm>
#include <iterator>

#include <QHBoxLayout>
#include <QTextStream>
#include <QFile>
#include <QNetworkReply>

#include <QProcess>
#include <QAction>
#include <QMenu>
#include <QDateTime>

#include "selfdrive/common/params.h"
#include "selfdrive/ui/qt/api.h"
#include "selfdrive/ui/qt/widgets/input.h"

#include "selfdrive/ui/ui.h"

SwitchOpenpilot::SwitchOpenpilot() : ButtonControl("Change Repo/Branch", "", "Change to another open pilot code. You can change it by entering ID/repository/branch.") {

  QObject::connect(this, &ButtonControl::clicked, [=]() {
    if (text() == "CHANGE") {
      QString userid = InputDialog::getText("First: Input the Git ID.", this);
      if (userid.length() > 0) {
        getUserID(userid);
        QString repoid = InputDialog::getText("Second: Input the repository.", this);
        if (repoid.length() > 0) {
          getRepoID(repoid);
          QString branchid = InputDialog::getText("Last: Input the branch name.", this);
          if (branchid.length() > 0) {
            getBranchID(branchid);
            githubbranch = branchid;
            QString cmd0 = QString::fromStdString("Change repository/branch. It takes a little time to complete. Do you want to proceed?\n") + QString::fromStdString("https://github.com/") + githubid + QString::fromStdString("/") + githubrepo + QString::fromStdString(".git\n") + QString::fromStdString("Branch: ") + githubbranch;
            if (ConfirmationDialog::confirm(cmd0, this)) {
              setText("DONE");
              setEnabled(true);
              QString time_format = "yyyyMMddHHmmss";
              QDateTime a = QDateTime::currentDateTime();
              QString as = a.toString(time_format);
              QString cmd1 = "mv /data/openpilot /data/openpilot_" + as;
              QString cmd2 = "git clone -b " + githubbranch + " --single-branch https://github.com/" + githubid + "/" + githubrepo + ".git /data/openpilot";
              QProcess::execute("pkill -f thermald");
              QProcess::execute(cmd1);
              QProcess::execute(cmd2);
              QProcess::execute("chmod -R g-rwx /data/openpilot");
              QProcess::execute("chmod -R o-rwx /data/openpilot");
              QProcess::execute("chmod 755 /data/openpilot");
              QProcess::execute("chmod 755 /data/openpilot/cereal");
              QProcess::execute("reboot");
            }
          }
        }
      }
    } else {
      refresh();
    }
  });
  refresh();
}

void SwitchOpenpilot::refresh() {
  setText("CHANGE");
  setEnabled(true);
}

void SwitchOpenpilot::getUserID(const QString &userid) {
  HttpRequest *request = new HttpRequest(this, false);
  QObject::connect(request, &HttpRequest::requestDone, [=](const QString &resp, bool success) {
    if (success) {
      if (!resp.isEmpty()) {
        githubid = userid;
      } else {
        ConfirmationDialog::alert(userid + " The ID does not exist. Return to the input window, press the cancel button, and try again from the beginning.", this);
      }
    } else {
      if (request->timeout()) {
        ConfirmationDialog::alert("The requested time has exceeded.", this);
      } else {
        ConfirmationDialog::alert("The ID does not exist. Return to the input window, press the cancel button, and try again from the beginning.", this);
      }
    }

    refresh();
    request->deleteLater();
  });
  request->sendRequest("https://github.com/" + userid);
}

void SwitchOpenpilot::getRepoID(const QString &repoid) {
  HttpRequest *request = new HttpRequest(this, false);
  QObject::connect(request, &HttpRequest::requestDone, [=](const QString &resp, bool success) {
    if (success) {
      if (!resp.isEmpty()) {
        githubrepo = repoid;
      } else {
        ConfirmationDialog::alert(repoid + " The repository does not exist. Return to the input window, press the cancel button, and try again from the beginning.", this);
      }
    } else {
      if (request->timeout()) {
        ConfirmationDialog::alert("The requested time has exceeded.", this);
      } else {
        ConfirmationDialog::alert("The Repository does not exist. Return to the input window, press the cancel button, and try again from the beginning.", this);
      }
    }

    refresh();
    request->deleteLater();
  });
  request->sendRequest("https://github.com/" + githubid + "/" + repoid);
}

void SwitchOpenpilot::getBranchID(const QString &branchid) {
  HttpRequest *request = new HttpRequest(this, false);
  QObject::connect(request, &HttpRequest::requestDone, [=](const QString &resp, bool success) {
    if (success) {
      if (!resp.isEmpty()) {
        githubbranch = branchid;
      } else {
        ConfirmationDialog::alert(branchid + " The branch does not exist. Press the cancel button and try again from the beginning.", this);
      }
    } else {
      if (request->timeout()) {
        ConfirmationDialog::alert("The requested time has exceeded.", this);
      } else {
        ConfirmationDialog::alert("The Branch does not exist. Return to the input window, press the cancel button, and try again from the beginning.", this);
      }
    }

    refresh();
    request->deleteLater();
  });
  request->sendRequest("https://github.com/" + githubid + "/" + githubrepo + "/tree/" + branchid);
}

GitHash::GitHash() : AbstractControl("Commit (Local/Remote)", "", "") {

  QString lhash = QString::fromStdString(params.get("GitCommit").substr(0, 10));
  QString rhash = QString::fromStdString(params.get("GitCommitRemote").substr(0, 10));
  hlayout->addStretch(1);
  
  local_hash.setText(QString::fromStdString(params.get("GitCommit").substr(0, 10)));
  remote_hash.setText(QString::fromStdString(params.get("GitCommitRemote").substr(0, 10)));
  //local_hash.setAlignment(Qt::AlignVCenter);
  remote_hash.setAlignment(Qt::AlignVCenter);
  local_hash.setStyleSheet("color: #aaaaaa");
  if (lhash == rhash) {
    remote_hash.setStyleSheet("color: #aaaaaa");
  } else {
    remote_hash.setStyleSheet("color: #0099ff");
  }
  hlayout->addWidget(&local_hash);
  hlayout->addWidget(&remote_hash);
}

OpenpilotView::OpenpilotView() : AbstractControl("오픈파일럿 주행화면 미리보기", "오픈파일럿 주행화면을 미리보기 합니다.", "") {

  // setup widget
  hlayout->addStretch(1);

  btn.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");

  btnc.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");

  btn.setFixedSize(250, 100);
  btnc.setFixedSize(250, 100);
  hlayout->addWidget(&btnc);
  hlayout->addWidget(&btn);

  QObject::connect(&btn, &QPushButton::clicked, [=]() {
    bool stat = params.getBool("IsOpenpilotViewEnabled");
    if (stat) {
      params.putBool("IsOpenpilotViewEnabled", false);
      QUIState::ui_state.scene.cal_view = false;
    } else {
      params.putBool("IsOpenpilotViewEnabled", true);
      QUIState::ui_state.scene.cal_view = false;
    }
    refresh();
  });
  QObject::connect(&btnc, &QPushButton::clicked, [=]() {
    bool stat = params.getBool("IsOpenpilotViewEnabled");
    if (stat) {
      params.putBool("IsOpenpilotViewEnabled", false);
      QUIState::ui_state.scene.cal_view = false;
    } else {
      params.putBool("IsOpenpilotViewEnabled", true);
      QUIState::ui_state.scene.cal_view = true;
    }
    refresh();
  });
  refresh();
}

void OpenpilotView::refresh() {
  bool param = params.getBool("IsOpenpilotViewEnabled");
  QString car_param = QString::fromStdString(params.get("CarParams"));
  if (param) {
    btn.setText("미리보기해제");
    btnc.setText("미리보기해제");
  } else {
    btn.setText("미리보기");
    btnc.setText("CALVIEW");
  }
  if (car_param.length()) {
    btn.setEnabled(false);
    btnc.setEnabled(false);
  } else {
    btn.setEnabled(true);
    btnc.setEnabled(true);
  }
}

CarSelectCombo::CarSelectCombo() : AbstractControl("", "", "") 
{
  combobox.setStyleSheet(R"(
    subcontrol-origin: padding;
    subcontrol-position: top left;
    selection-background-color: #111;
    selection-color: yellow;
    color: white;
    background-color: #393939;
    border-style: solid;
    border: 0px solid #1e1e1e;
    border-radius: 0;
    width: 100px;
  )");

  combobox.addItem("Select Your Car");
  QFile carlistfile("/data/params/d/CarList");
  if (carlistfile.open(QIODevice::ReadOnly)) {
    QTextStream carname(&carlistfile);
    while (!carname.atEnd()) {
      QString line = carname.readLine();
      combobox.addItem(line);
    }
    carlistfile.close();
  }

  combobox.setFixedWidth(1055);

  btn.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");

  btn.setFixedSize(150, 100);

  QObject::connect(&btn, &QPushButton::clicked, [=]() {
    if (btn.text() == "UNSET") {
      if (ConfirmationDialog::confirm("Do you want to unset?", this)) {
        params.remove("CarModel");
        combobox.setCurrentIndex(0);
        refresh();
      }
    }
  });

  //combobox.view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  hlayout->addWidget(&combobox);
  hlayout->addWidget(&btn, Qt::AlignRight);

  QObject::connect(&combobox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [=](int index)
  {
    combobox.itemData(combobox.currentIndex());
    QString str = combobox.currentText();
    if (combobox.currentIndex() != 0) {
      if (ConfirmationDialog::confirm("Press OK to set your car as\n" + str, this)) {
        params.put("CarModel", str.toStdString());
        int indi_cars[] = {1, 2, 3, 4, 10, 29, 30, 37}; //R-MDPS type such as Genesis, Sonata Turbo, Sorento, Mohave
        int selected_car = combobox.currentIndex();
        bool go_indi = std::find(std::begin(indi_cars), std::end(indi_cars), selected_car) != std::end(indi_cars);
        if (go_indi) {
          params.put("LateralControlMethod", "1");
        }
      }
    }
    refresh();
  });
  refresh();
}

void CarSelectCombo::refresh() {
  QString selected_carname = QString::fromStdString(params.get("CarModel"));
  int index = combobox.findText(selected_carname);
  if (index >= 0) combobox.setCurrentIndex(index);
  if (selected_carname.length()) {
    btn.setEnabled(true);
    btn.setText("UNSET");
  } else {
    btn.setEnabled(false);
    btn.setText("SET");
  }
}

BranchSelectCombo::BranchSelectCombo() : AbstractControl("", "", "") 
{
  combobox.setStyleSheet(R"(
    subcontrol-origin: padding;
    subcontrol-position: top left;
    selection-background-color: #111;
    selection-color: yellow;
    color: white;
    background-color: #393939;
    border-style: solid;
    border: 0px solid #1e1e1e;
    border-radius: 0;
    width: 100px;
    height: 120px;
  )");

  combobox.addItem("Select Branch you want to change");
  QFile branchlistfile("/data/openpilot/selfdrive/assets/addon/script/branches");
  if (branchlistfile.open(QIODevice::ReadOnly)) {
    QTextStream carname(&branchlistfile);
    while (!carname.atEnd()) {
      QString line = carname.readLine();
      combobox.addItem(line);
    }
    branchlistfile.close();
  }

  combobox.setFixedWidth(1205);

  hlayout->addWidget(&combobox);

  QObject::connect(&combobox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [=](int index)
  {
    combobox.itemData(combobox.currentIndex());
    QString str = combobox.currentText();
    QString current_branch = QString::fromStdString(params.get("GitBranch"));
    if (combobox.currentIndex() != 0 && str != current_branch) {
      if (ConfirmationDialog::confirm("Now will checkout the branch, <" + str + ">. The device will be rebooted if completed.", this)) {
        QString cmd1 = "git -C /data/openpilot remote set-branches --add origin " + str;
        QString cmd2 = "git -C /data/openpilot checkout --track origin/" + str;
        QString cmd3 = "git -C /data/openpilot checkout " + str;
        QProcess::execute(cmd1);
        QProcess::execute("git -C /data/openpilot fetch origin");
        QProcess::execute(cmd2);
        QProcess::execute(cmd3);
        QProcess::execute("git -C /data/openpilot pull");
        QProcess::execute("pkill -f thermald");
        QProcess::execute("rm -f /data/openpilot/prebuilt");
        QProcess::execute("reboot");
      }
    } else if (combobox.currentIndex() != 0 && str == current_branch) {
      if (ConfirmationDialog::alert("Your branch is already <" + current_branch + ">.", this)) {combobox.setCurrentIndex(0);}
    }
  });
}

TimeZoneSelectCombo::TimeZoneSelectCombo() : AbstractControl("", "", "") 
{
  combobox.setStyleSheet(R"(
    subcontrol-origin: padding;
    subcontrol-position: top left;
    selection-background-color: #111;
    selection-color: yellow;
    color: white;
    background-color: #393939;
    border-style: solid;
    border: 0px solid #1e1e1e;
    border-radius: 0;
    width: 100px;
  )");

  combobox.addItem("Select Your TimeZone");
  QFile timezonelistfile("/data/openpilot/selfdrive/assets/addon/param/TimeZone");
  if (timezonelistfile.open(QIODevice::ReadOnly)) {
    QTextStream timezonename(&timezonelistfile);
    while (!timezonename.atEnd()) {
      QString line = timezonename.readLine();
      combobox.addItem(line);
    }
    timezonelistfile.close();
  }

  combobox.setFixedWidth(1055);

  btn.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");

  btn.setFixedSize(150, 100);

  QObject::connect(&btn, &QPushButton::clicked, [=]() {
    if (btn.text() == "UNSET") {
      if (ConfirmationDialog::confirm("Do you want to set default?", this)) {
        params.put("OPKRTimeZone", "UTC");
        combobox.setCurrentIndex(0);
        refresh();
      }
    }
  });

  //combobox.view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  hlayout->addWidget(&combobox);
  hlayout->addWidget(&btn, Qt::AlignRight);

  QObject::connect(&combobox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [=](int index)
  {
    combobox.itemData(combobox.currentIndex());
    QString str = combobox.currentText();
    if (combobox.currentIndex() != 0) {
      if (ConfirmationDialog::confirm("Press OK to set your timezone as\n" + str, this)) {
        params.put("OPKRTimeZone", str.toStdString());
      }
    }
    refresh();
  });
  refresh();
}

void TimeZoneSelectCombo::refresh() {
  QString selected_timezonename = QString::fromStdString(params.get("OPKRTimeZone"));
  int index = combobox.findText(selected_timezonename);
  if (index >= 0) combobox.setCurrentIndex(index);
  if (selected_timezonename.length()) {
    btn.setEnabled(true);
    btn.setText("UNSET");
  } else {
    btn.setEnabled(false);
    btn.setText("SET");
  }
}

//UI
AutoShutdown::AutoShutdown() : AbstractControl("EON 자동 종료", "운행(온로드) 후 시동을 끈 상태(오프로드)에서 설정시간 이후에 자동으로 이온이 꺼집니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrAutoShutdown"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 0 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("OpkrAutoShutdown", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrAutoShutdown"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 10 ) {
      value = 10;
    }
    QString values = QString::number(value);
    params.put("OpkrAutoShutdown", values.toStdString());
    refresh();
  });
  refresh();
}

void AutoShutdown::refresh() {
  QString option = QString::fromStdString(params.get("OpkrAutoShutdown"));
  if (option == "0") {
    label.setText(QString::fromStdString("항상켜기"));
  } else if (option == "1") {
    label.setText(QString::fromStdString("바로끄기"));
  } else if (option == "2") {
    label.setText(QString::fromStdString("30초"));
  } else if (option == "3") {
    label.setText(QString::fromStdString("1분"));
  } else if (option == "4") {
    label.setText(QString::fromStdString("3분"));
  } else if (option == "5") {
    label.setText(QString::fromStdString("5분"));
  } else if (option == "6") {
    label.setText(QString::fromStdString("10분"));
  } else if (option == "7") {
    label.setText(QString::fromStdString("30분"));
  } else if (option == "8") {
    label.setText(QString::fromStdString("1시간"));
  } else if (option == "9") {
    label.setText(QString::fromStdString("3시간"));
  } else if (option == "10") {
    label.setText(QString::fromStdString("5시간"));
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}

ForceShutdown::ForceShutdown() : AbstractControl("EON 강제 종료", "운행을 하지 않고(온로드 진입X) 오프로드상태에서 화면이 꺼진경우 일정시간 이후에 강제로 꺼지게 합니다. 터치이벤트 발생시 꺼지는 시간이 리셋됩니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrForceShutdown"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 0 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("OpkrForceShutdown", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrForceShutdown"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 5 ) {
      value = 5;
    }
    QString values = QString::number(value);
    params.put("OpkrForceShutdown", values.toStdString());
    refresh();
  });
  refresh();
}

void ForceShutdown::refresh() {
  QString option = QString::fromStdString(params.get("OpkrForceShutdown"));
  if (option == "0") {
    label.setText(QString::fromStdString("항상켜기"));
  } else if (option == "1") {
    label.setText(QString::fromStdString("1분"));
  } else if (option == "2") {
    label.setText(QString::fromStdString("3분"));
  } else if (option == "3") {
    label.setText(QString::fromStdString("5분"));
  } else if (option == "4") {
    label.setText(QString::fromStdString("10분"));
  } else if (option == "5") {
    label.setText(QString::fromStdString("30분"));
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}


VolumeControl::VolumeControl() : AbstractControl("EON 볼륨 조절(%)", "EON의 볼륨을 조절합니다. 안드로이드 기본값/수동설정", "../assets/offroad/icon_shell.png") {

  effect.setSource(QUrl::fromLocalFile("/data/openpilot/selfdrive/assets/addon/sound/ding.wav"));
  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrUIVolumeBoost"));
    int value = str.toInt();
    value = value - 5;
    if (value <= -5 ) {
      value = -5;
    }
    QString values = QString::number(value);
    QUIState::ui_state.scene.nVolumeBoost = value;
    params.put("OpkrUIVolumeBoost", values.toStdString());
    playsound();
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrUIVolumeBoost"));
    int value = str.toInt();
    value = value + 5;
    if (value >= 100 ) {
      value = 100;
    }
    QString values = QString::number(value);
    QUIState::ui_state.scene.nVolumeBoost = value;
    params.put("OpkrUIVolumeBoost", values.toStdString());
    playsound();
    refresh();
  });
  refresh();
}

void VolumeControl::refresh() {
  QString option = QString::fromStdString(params.get("OpkrUIVolumeBoost"));
  if (option == "0") {
    label.setText(QString::fromStdString("기본값"));
  } else if (option == "-5") {
    label.setText(QString::fromStdString("음소거"));
  } else {
    label.setText(QString::fromStdString(params.get("OpkrUIVolumeBoost")));
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}

void VolumeControl::playsound() {
  float value = QUIState::ui_state.scene.nVolumeBoost;
  if (value > 1 ) {
    effect.setVolume(value * 0.01);
    effect.play();
  } else if (value >= 0) {
    effect.setVolume(0.5);
    effect.play();
  }
}

BrightnessControl::BrightnessControl() : AbstractControl("EON 밝기 조절(%)", "EON화면의 밝기를 수동 조절합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrUIBrightness"));
    int value = str.toInt();
    value = value - 5;
    if (value <= 0 ) {
      value = 0;
    }
    QUIState::ui_state.scene.brightness = value;
    QString values = QString::number(value);
    params.put("OpkrUIBrightness", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrUIBrightness"));
    int value = str.toInt();
    value = value + 5;
    if (value >= 100 ) {
      value = 100;
    }
    QUIState::ui_state.scene.brightness = value;
    QString values = QString::number(value);
    params.put("OpkrUIBrightness", values.toStdString());
    refresh();
  });
  refresh();
}

void BrightnessControl::refresh() {
  QString option = QString::fromStdString(params.get("OpkrUIBrightness"));
  if (option == "0") {
    label.setText(QString::fromStdString("자동조절"));
  } else {
    label.setText(QString::fromStdString(params.get("OpkrUIBrightness")));
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}

BrightnessOffControl::BrightnessOffControl() : AbstractControl("EON 화면끄기 밝기(%)", "EON 화면 꺼짐 기능 사용시 자동밝기 비율에 따라 밝기를 감소시킵니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrUIBrightnessOff"));
    int value = str.toInt();
    value = value - 10;
    if (value <= 0 ) {
      value = 0;
    }
    QUIState::ui_state.scene.brightness_off = value;
    QString values = QString::number(value);
    params.put("OpkrUIBrightnessOff", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrUIBrightnessOff"));
    int value = str.toInt();
    value = value + 10;
    if (value >= 100 ) {
      value = 100;
    }
    QUIState::ui_state.scene.brightness_off = value;
    QString values = QString::number(value);
    params.put("OpkrUIBrightnessOff", values.toStdString());
    refresh();
  });
  refresh();
}

void BrightnessOffControl::refresh() {
  QString option = QString::fromStdString(params.get("OpkrUIBrightnessOff"));
  if (option == "0") {
    label.setText(QString::fromStdString("완전꺼짐"));
  } else {
    label.setText(QString::fromStdString(params.get("OpkrUIBrightnessOff")));
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}

AutoScreenOff::AutoScreenOff() : AbstractControl("EON 화면끄기 시간", "주행 시작 후 화면보호를 위해 이온화면이 꺼지게 하거나 밝기를 줄입니다. 터치나 이벤트 발생시 자동으로 밝아지거나 켜집니다.", "../assets/offroad/icon_shell.png") 
{

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrAutoScreenOff"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -2 ) {
      value = -2;
    }
    QUIState::ui_state.scene.autoScreenOff = value;
    QString values = QString::number(value);
    params.put("OpkrAutoScreenOff", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrAutoScreenOff"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 10 ) {
      value = 10;
    }
    QUIState::ui_state.scene.autoScreenOff = value;
    QString values = QString::number(value);
    params.put("OpkrAutoScreenOff", values.toStdString());
    refresh();
  });
  refresh();
}

void AutoScreenOff::refresh() 
{
  QString option = QString::fromStdString(params.get("OpkrAutoScreenOff"));
  if (option == "-2") {
    label.setText(QString::fromStdString("항상켜기"));
  } else if (option == "-1") {
    label.setText(QString::fromStdString("15초"));
  } else if (option == "0") {
    label.setText(QString::fromStdString("30초"));
  } else {
    label.setText(QString::fromStdString(params.get("OpkrAutoScreenOff")) + "분");
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}

ChargingMin::ChargingMin() : AbstractControl("배터리 최소 충전 값", "배터리 최소 충전값을 설정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrBatteryChargingMin"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 10 ) {
      value = 10;
    }
    QString values = QString::number(value);
    params.put("OpkrBatteryChargingMin", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrBatteryChargingMin"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 90 ) {
      value = 90;
    }
    QString values = QString::number(value);
    params.put("OpkrBatteryChargingMin", values.toStdString());
    refresh();
  });
  refresh();
}

void ChargingMin::refresh() {
  label.setText(QString::fromStdString(params.get("OpkrBatteryChargingMin")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

ChargingMax::ChargingMax() : AbstractControl("배터리 최대 충전 값", "배터리 최대 충전값을 설정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrBatteryChargingMax"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 10 ) {
      value = 10;
    }
    QString values = QString::number(value);
    params.put("OpkrBatteryChargingMax", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrBatteryChargingMax"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 90 ) {
      value = 90;
    }
    QString values = QString::number(value);
    params.put("OpkrBatteryChargingMax", values.toStdString());
    refresh();
  });
  refresh();
}

void ChargingMax::refresh() {
  label.setText(QString::fromStdString(params.get("OpkrBatteryChargingMax")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

RecordCount::RecordCount() : AbstractControl("녹화파일 최대 개수 설정", "녹화 파일 최대 개수를 설정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("RecordingCount"));
    int value = str.toInt();
    value = value - 5;
    if (value <= 5 ) {
      value = 5;
    }
    QString values = QString::number(value);
    params.put("RecordingCount", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("RecordingCount"));
    int value = str.toInt();
    value = value + 5;
    if (value >= 300 ) {
      value = 300;
    }
    QString values = QString::number(value);
    params.put("RecordingCount", values.toStdString());
    refresh();
  });
  refresh();
}

void RecordCount::refresh() {
  label.setText(QString::fromStdString(params.get("RecordingCount")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

RecordQuality::RecordQuality() : AbstractControl("녹화 화질 설정", "녹화 화질을 설정합니다. 저화질/중화질/고화질/초고화질", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("RecordingQuality"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -1 ) {
      value = 3;
    }
    QString values = QString::number(value);
    params.put("RecordingQuality", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("RecordingQuality"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 4 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("RecordingQuality", values.toStdString());
    refresh();
  });
  refresh();
}

void RecordQuality::refresh() {
  QString option = QString::fromStdString(params.get("RecordingQuality"));
  if (option == "0") {
    label.setText(QString::fromStdString("저화질"));
  } else if (option == "1") {
    label.setText(QString::fromStdString("중화질"));
  } else if (option == "2") {
    label.setText(QString::fromStdString("고화질"));
  } else {
    label.setText(QString::fromStdString("초고화질"));
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

MonitoringMode::MonitoringMode() : AbstractControl("모니터링 모드 설정", "모니터링 모드를 설정합니다. 기본설정/졸음방지, 졸음방지의 경우 아래 Threshold 값을 조정(낮춤)하여 좀더 빨리 경고메시지를 보낼 수 있습니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrMonitoringMode"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("OpkrMonitoringMode", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrMonitoringMode"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 2 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("OpkrMonitoringMode", values.toStdString());
    refresh();
  });
  refresh();
}

void MonitoringMode::refresh() {
  QString option = QString::fromStdString(params.get("OpkrMonitoringMode"));
  if (option == "0") {
    label.setText(QString::fromStdString("기본설정"));
  } else if (option == "1") {
    label.setText(QString::fromStdString("졸음방지"));
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

MonitorEyesThreshold::MonitorEyesThreshold() : AbstractControl("E2E EYE Threshold", "눈감지 범위에 대한 기준값을 조정합니다. 자신에게 맞는 값을 기준값을 설정합니다. 눈을 감고 있을 때 distratedEyes값 보다 낮게 설정해야 합니다. 기본값:0.75", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrMonitorEyesThreshold"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("OpkrMonitorEyesThreshold", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrMonitorEyesThreshold"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 100 ) {
      value = 100;
    }
    QString values = QString::number(value);
    params.put("OpkrMonitorEyesThreshold", values.toStdString());
    refresh();
  });
  refresh();
}

void MonitorEyesThreshold::refresh() {
  auto strs = QString::fromStdString(params.get("OpkrMonitorEyesThreshold"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

NormalEyesThreshold::NormalEyesThreshold() : AbstractControl("Normal EYE Threshold", "눈 인식 기준값을 조정합니다. 인식률이 낮은경우 값을 낮춥니다. 기본값:0.5", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrMonitorNormalEyesThreshold"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("OpkrMonitorNormalEyesThreshold", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrMonitorNormalEyesThreshold"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 100 ) {
      value = 100;
    }
    QString values = QString::number(value);
    params.put("OpkrMonitorNormalEyesThreshold", values.toStdString());
    refresh();
  });
  refresh();
}

void NormalEyesThreshold::refresh() {
  auto strs = QString::fromStdString(params.get("OpkrMonitorNormalEyesThreshold"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

BlinkThreshold::BlinkThreshold() : AbstractControl("Blink Threshold", "눈 깜빡임 정도에 대한 인식값을 조정합니다. 눈을 감고있을 때 BlinkProb를 확인후 값을 낮춰야 합니다. 기본값:0.5", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrMonitorBlinkThreshold"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("OpkrMonitorBlinkThreshold", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrMonitorBlinkThreshold"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 100 ) {
      value = 100;
    }
    QString values = QString::number(value);
    params.put("OpkrMonitorBlinkThreshold", values.toStdString());
    refresh();
  });
  refresh();
}

void BlinkThreshold::refresh() {
  auto strs = QString::fromStdString(params.get("OpkrMonitorBlinkThreshold"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

//Driving
CruisemodeSelInit::CruisemodeSelInit() : AbstractControl("크루즈 시작모드 설정", "크루즈 시작모드를 설정합니다. 오파모드/차간+커브/차간Only/커브Only/편도1차선/맵감속Only  오파모드:버튼속도제어 사용안함, 차간+커브:버튼속도제어를 차간거리와 커브구간에서 사용, 차간Only:버튼감속을 차간거리에서만 사용, 커브Only:버튼감속을 커브구간에서만 사용, 편도1차선:편도1차선 구간에서 카메라오프셋을 낮춰 오른쪽으로 붙여서 주행, 맵감속Only:맵피 감속만 사용", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("CruiseStatemodeSelInit"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -1 ) {
      value = 5;
    }
    QString values = QString::number(value);
    params.put("CruiseStatemodeSelInit", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("CruiseStatemodeSelInit"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 6 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("CruiseStatemodeSelInit", values.toStdString());
    refresh();
  });
  refresh();
}

void CruisemodeSelInit::refresh() {
  QString option = QString::fromStdString(params.get("CruiseStatemodeSelInit"));
  if (option == "0") {
    label.setText(QString::fromStdString("오파모드"));
  } else if (option == "1") {
    label.setText(QString::fromStdString("차간+커브"));
  } else if (option == "2") {
    label.setText(QString::fromStdString("차간Only"));
  } else if (option == "3") {
    label.setText(QString::fromStdString("커브Only"));
  } else if (option == "4") {
    label.setText(QString::fromStdString("편도1차선"));
  } else {
    label.setText(QString::fromStdString("맵감속Only"));
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

LaneChangeSpeed::LaneChangeSpeed() : AbstractControl("차선변경 속도 설정", "차선변경 가능 속도를 설정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrLaneChangeSpeed"));
    int value = str.toInt();
    value = value - 5;
    if (value <= 20 ) {
      value = 20;
    }
    QString values = QString::number(value);
    params.put("OpkrLaneChangeSpeed", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrLaneChangeSpeed"));
    int value = str.toInt();
    value = value + 5;
    if (value >= 160 ) {
      value = 160;
    }
    QString values = QString::number(value);
    params.put("OpkrLaneChangeSpeed", values.toStdString());
    refresh();
  });
  refresh();
}

void LaneChangeSpeed::refresh() {
  label.setText(QString::fromStdString(params.get("OpkrLaneChangeSpeed")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

LaneChangeDelay::LaneChangeDelay() : AbstractControl("차선변경 지연시간 설정", "턴시그널 작동후 차선변경전 지연시간을 설정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrAutoLaneChangeDelay"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 0 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("OpkrAutoLaneChangeDelay", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrAutoLaneChangeDelay"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 5 ) {
      value = 5;
    }
    QString values = QString::number(value);
    params.put("OpkrAutoLaneChangeDelay", values.toStdString());
    refresh();
  });
  refresh();
}

void LaneChangeDelay::refresh() {
  QString option = QString::fromStdString(params.get("OpkrAutoLaneChangeDelay"));
  if (option == "0") {
    label.setText(QString::fromStdString("수동"));
  } else if (option == "1") {
    label.setText(QString::fromStdString("즉시"));
  } else if (option == "2") {
    label.setText(QString::fromStdString("0.5초"));
  } else if (option == "3") {
    label.setText(QString::fromStdString("1초"));
  } else if (option == "4") {
    label.setText(QString::fromStdString("1.5초"));
  } else {
    label.setText(QString::fromStdString("2초"));
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}

LeftCurvOffset::LeftCurvOffset() : AbstractControl("오프셋조정(왼쪽 커브)", "커브구간에서 차량위치를 조정합니다.(-값: 차를 왼쪽으로 이동, +값:차를 오른쪽으로 이동)", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LeftCurvOffsetAdj"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -50 ) {
      value = -50;
    }
    QString values = QString::number(value);
    params.put("LeftCurvOffsetAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LeftCurvOffsetAdj"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 50 ) {
      value = 50;
    }
    QString values = QString::number(value);
    params.put("LeftCurvOffsetAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void LeftCurvOffset::refresh() {
  label.setText(QString::fromStdString(params.get("LeftCurvOffsetAdj")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

RightCurvOffset::RightCurvOffset() : AbstractControl("오프셋조정(오른쪽 커브)", "커브구간에서 차량위치를 조정합니다.(-값: 차를 왼쪽으로 이동, +값:차를 오른쪽으로 이동)", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("RightCurvOffsetAdj"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -50 ) {
      value = -50;
    }
    QString values = QString::number(value);
    params.put("RightCurvOffsetAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("RightCurvOffsetAdj"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 50 ) {
      value = 50;
    }
    QString values = QString::number(value);
    params.put("RightCurvOffsetAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void RightCurvOffset::refresh() {
  label.setText(QString::fromStdString(params.get("RightCurvOffsetAdj")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

MaxAngleLimit::MaxAngleLimit() : AbstractControl("최대 조향각 설정(각도)", "오파 가능한 핸들의 최대 조향각을 설정합니다. 각도를 90도이상 설정시 일부차량에서 오류가 발생할 수 있으니 참고하시기 바랍니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrMaxAngleLimit"));
    int value = str.toInt();
    value = value - 10;
    if (value <= 80 ) {
      value = 80;
    }
    QString values = QString::number(value);
    params.put("OpkrMaxAngleLimit", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrMaxAngleLimit"));
    int value = str.toInt();
    value = value + 10;
    if (value >= 360 ) {
      value = 360;
    }
    QString values = QString::number(value);
    params.put("OpkrMaxAngleLimit", values.toStdString());
    refresh();
  });
  refresh();
}

void MaxAngleLimit::refresh() {
  QString option = QString::fromStdString(params.get("OpkrMaxAngleLimit"));
  if (option == "80") {
    label.setText(QString::fromStdString("제한없음"));
  } else {
    label.setText(QString::fromStdString(params.get("OpkrMaxAngleLimit")));
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}

SteerAngleCorrection::SteerAngleCorrection() : AbstractControl("스티어앵글 영점 조정", "직선주로에서 현재조향각이 0이 아닐겨우 SteerAngle 영점을 조정하여 0으로 맞춥니다. ex) 직선주로시 0.5도 인경우, 0.5로 세팅, -0.5도인경우 -0.5로 세팅", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrSteerAngleCorrection"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -50 ) {
      value = -50;
    }
    QString values = QString::number(value);
    params.put("OpkrSteerAngleCorrection", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrSteerAngleCorrection"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 50 ) {
      value = 50;
    }
    QString values = QString::number(value);
    params.put("OpkrSteerAngleCorrection", values.toStdString());
    refresh();
  });
  refresh();
}

void SteerAngleCorrection::refresh() {
  auto strs = QString::fromStdString(params.get("OpkrSteerAngleCorrection"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.1;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

SpeedLimitOffset::SpeedLimitOffset() : AbstractControl("MAP기반 제한속도 오프셋(%)", "맵기반 감속시 GPS속도와 실속도차이를 보상하여 감속합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btn.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn.setFixedSize(110, 100);
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btn);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btn, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrSpeedLimitOffsetOption"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 3 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("OpkrSpeedLimitOffsetOption", values.toStdString());
    refresh();
  });

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrSpeedLimitOffset"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -30 ) {
      value = -30;
    }
    QString values = QString::number(value);
    //QUIState::ui_state.speed_lim_off = value;
    params.put("OpkrSpeedLimitOffset", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrSpeedLimitOffset"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 30 ) {
      value = 30;
    }
    QString values = QString::number(value);
    //QUIState::ui_state.speed_lim_off = value;
    params.put("OpkrSpeedLimitOffset", values.toStdString());
    refresh();
  });
  refresh();
}

void SpeedLimitOffset::refresh() {
  auto strs = QString::fromStdString(params.get("OpkrSpeedLimitOffsetOption"));
  if (strs == "0") {
    btn.setText("%");
  } else if (strs == "1") {
    btn.setText("±");
  } else {
    btn.setText("C");
  }
  label.setText(QString::fromStdString(params.get("OpkrSpeedLimitOffset")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

RESChoice::RESChoice() : AbstractControl("자동RES 옵션", "자동RES옵션을 설정합니다. 1. 일시적 크루즈속도 조정, 2. 설정속도 자체를 조정  ※자동 RES는 조건에 따라 동작이 잘 되지 않을 수도 있으니 참고하시고 사용바랍니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("AutoResOption"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -1 ) {
      value = 2;
    }
    QString values = QString::number(value);
    params.put("AutoResOption", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("AutoResOption"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 3 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("AutoResOption", values.toStdString());
    refresh();
  });
  refresh();
}

void RESChoice::refresh() {
  QString option = QString::fromStdString(params.get("AutoResOption"));
  if (option == "0") {
    label.setText(QString::fromStdString("CruiseSet"));
  } else if (option == "1") {
    label.setText(QString::fromStdString("MaxSpeedSet"));
  } else {
    label.setText(QString::fromStdString("AUTO(LeadCar)"));
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

AutoResCondition::AutoResCondition() : AbstractControl("자동RES 조건", "자동RES조건을 설정합니다. 브레이크해제시 작동/가속페달조작시 작동.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("AutoResCondition"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("AutoResCondition", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("AutoResCondition"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 2 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("AutoResCondition", values.toStdString());
    refresh();
  });
  refresh();
}

void AutoResCondition::refresh() {
  QString option = QString::fromStdString(params.get("AutoResCondition"));
  if (option == "0") {
    label.setText(QString::fromStdString("브레이크해제시"));
  } else {
    label.setText(QString::fromStdString("가속페달조작시"));
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

AutoResLimitTime::AutoResLimitTime() : AbstractControl("AutoRES 허용(초)", "자동 RES 허용 시간을 조정합니다. 자동 RES는 크루즈가 해제된 후 설정된 시간 내에만 작동합니다. ", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("AutoResLimitTime"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 0 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("AutoResLimitTime", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("AutoResLimitTime"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 60 ) {
      value = 60;
    }
    QString values = QString::number(value);
    params.put("AutoResLimitTime", values.toStdString());
    refresh();
  });
  refresh();
}

void AutoResLimitTime::refresh() {
  QString option = QString::fromStdString(params.get("AutoResLimitTime"));
  if (option == "0") {
    label.setText(QString::fromStdString("NoLimit"));
  } else {
    label.setText(QString::fromStdString(params.get("AutoResLimitTime")));
  }
  btnminus.setText("-");
  btnplus.setText("+");
}

AutoEnableSpeed::AutoEnableSpeed() : AbstractControl("자동 인게이지 속도(km/h)", "자동 인게이지 속도를 설정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("AutoEnableSpeed"));
    int value = str.toInt();
    value = value - 3;
    if (value <= 0 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("AutoEnableSpeed", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("AutoEnableSpeed"));
    int value = str.toInt();
    value = value + 3;
    if (value >= 30 ) {
      value = 30;
    }
    QString values = QString::number(value);
    params.put("AutoEnableSpeed", values.toStdString());
    refresh();
  });
  refresh();
}

void AutoEnableSpeed::refresh() {
  QString option = QString::fromStdString(params.get("AutoEnableSpeed"));
  if (option == "0") {
    label.setText(QString::fromStdString("출발시"));
  } else {
    label.setText(QString::fromStdString(params.get("AutoEnableSpeed")));
  }
  btnminus.setText("-");
  btnplus.setText("+");
}

CamDecelDistAdd::CamDecelDistAdd() : AbstractControl("안전감속시작거리 조정(%)", "안전구간 감속시 감속시작 거리를 줄이거나 늘립니다.(+값: 원거리에서 감속시작, -값: 근거리에서 감속시작) ※감속시작 거리 = 현재속도에 따른 보간값 X 현재속도와 캠속도 차이에 따른 보간값 X 줄임/늘림 비율", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SafetyCamDecelDistGain"));
    int value = str.toInt();
    value = value - 5;
    if (value <= -100 ) {
      value = -100;
    }
    QString values = QString::number(value);
    params.put("SafetyCamDecelDistGain", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SafetyCamDecelDistGain"));
    int value = str.toInt();
    value = value + 5;
    if (value >= 100 ) {
      value = 100;
    }
    QString values = QString::number(value);
    params.put("SafetyCamDecelDistGain", values.toStdString());
    refresh();
  });
  refresh();
}

void CamDecelDistAdd::refresh() {
  QString option = QString::fromStdString(params.get("SafetyCamDecelDistGain"));
  if (option == "0") {
    label.setText(QString::fromStdString("기본값"));
  } else {
    label.setText(QString::fromStdString(params.get("SafetyCamDecelDistGain")));
  }
  btnminus.setText("-");
  btnplus.setText("+");
}

//Panda
MaxSteer::MaxSteer() : AbstractControl("MAX_STEER", "판다 MAX_STEER 값을 수정합니다. 적용하려면 아래 실행 버튼을 누르세요.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("MaxSteer"));
    int value = str.toInt();
    value = value - 2;
    if (value <= 384 ) {
      value = 384;
    }
    QString values = QString::number(value);
    params.put("MaxSteer", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("MaxSteer"));
    int value = str.toInt();
    value = value + 2;
    if (value >= 1024 ) {
      value = 1024;
    }
    QString values = QString::number(value);
    params.put("MaxSteer", values.toStdString());
    refresh();
  });
  refresh();
}

void MaxSteer::refresh() {
  label.setText(QString::fromStdString(params.get("MaxSteer")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

MaxRTDelta::MaxRTDelta() : AbstractControl("RT_DELTA", "판다 RT_DELTA 값을 수정합니다. 적용하려면 아래 실행 버튼을 누르세요.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("MaxRTDelta"));
    int value = str.toInt();
    value = value - 2;
    if (value <= 50 ) {
      value = 50;
    }
    QString values = QString::number(value);
    params.put("MaxRTDelta", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("MaxRTDelta"));
    int value = str.toInt();
    value = value + 2;
    if (value >= 500 ) {
      value = 500;
    }
    QString values = QString::number(value);
    params.put("MaxRTDelta", values.toStdString());
    refresh();
  });
  refresh();
}

void MaxRTDelta::refresh() {
  label.setText(QString::fromStdString(params.get("MaxRTDelta")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

MaxRateUp::MaxRateUp() : AbstractControl("MAX_RATE_UP", "판다 MAX_RATE_UP 값을 수정합니다. 적용하려면 아래 실행 버튼을 누르세요.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("MaxRateUp"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 3 ) {
      value = 3;
    }
    QString values = QString::number(value);
    params.put("MaxRateUp", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("MaxRateUp"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 7 ) {
      value = 7;
    }
    QString values = QString::number(value);
    params.put("MaxRateUp", values.toStdString());
    refresh();
  });
  refresh();
}

void MaxRateUp::refresh() {
  label.setText(QString::fromStdString(params.get("MaxRateUp")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

MaxRateDown::MaxRateDown() : AbstractControl("MAX_RATE_DOWN", "판다 MAX_RATE_DOWN 값을 수정합니다. 적용하려면 아래 실행 버튼을 누르세요.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("MaxRateDown"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 7 ) {
      value = 7;
    }
    QString values = QString::number(value);
    params.put("MaxRateDown", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("MaxRateDown"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 15 ) {
      value = 15;
    }
    QString values = QString::number(value);
    params.put("MaxRateDown", values.toStdString());
    refresh();
  });
  refresh();
}

void MaxRateDown::refresh() {
  label.setText(QString::fromStdString(params.get("MaxRateDown")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

//튜닝
CameraOffset::CameraOffset() : AbstractControl("CameraOffset", "CameraOffset값을 설정합니다. (+값:왼쪽이동, -값:오른쪽이동)", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("CameraOffsetAdj"));
    int value = str.toInt();
    value = value - 5;
    if (value <= -1000 ) {
      value = -1000;
    }
    QString values = QString::number(value);
    params.put("CameraOffsetAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("CameraOffsetAdj"));
    int value = str.toInt();
    value = value + 5;
    if (value >= 1000 ) {
      value = 1000;
    }
    QString values = QString::number(value);
    params.put("CameraOffsetAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void CameraOffset::refresh() {
  auto strs = QString::fromStdString(params.get("CameraOffsetAdj"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.001;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

PathOffset::PathOffset() : AbstractControl("PathOffset", "PathOffset값을 설정합니다. (+값:왼쪽이동, -값:오른쪽이동)", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("PathOffsetAdj"));
    int value = str.toInt();
    value = value - 5;
    if (value <= -1000 ) {
      value = -1000;
    }
    QString values = QString::number(value);
    params.put("PathOffsetAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("PathOffsetAdj"));
    int value = str.toInt();
    value = value + 5;
    if (value >= 1000 ) {
      value = 1000;
    }
    QString values = QString::number(value);
    params.put("PathOffsetAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void PathOffset::refresh() {
  auto strs = QString::fromStdString(params.get("PathOffsetAdj"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.001;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

SRBaseControl::SRBaseControl() : AbstractControl("SteerRatio", "SteerRatio 기본값을 설정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btndigit.setStyleSheet(R"(
    QPushButton {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    }
    QPushButton:pressed {
      background-color: #ababab;
    }
  )");
  btnminus.setStyleSheet(R"(
    QPushButton {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    }
    QPushButton:pressed {
      background-color: #ababab;
    }
  )");
  btnplus.setStyleSheet(R"(
    QPushButton {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    }
    QPushButton:pressed {
      background-color: #ababab;
    }
  )");
  btndigit.setFixedSize(100, 100);
  btnminus.setFixedSize(100, 100);
  btnplus.setFixedSize(100, 100);
  hlayout->addWidget(&btndigit);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);
  btndigit.setText("0.01");
  btnminus.setText("-");
  btnplus.setText("+");

  QObject::connect(&btndigit, &QPushButton::clicked, [=]() {
    digit = digit * 10;
    if (digit >= 11 ) {
      digit = 0.01;
    }
    QString level = QString::number(digit);
    btndigit.setText(level);
  });

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerRatioAdj"));
    int value = str.toInt();
    value = value - (digit*100);
    if (value <= 800) {
      value = 800;
    }
    QString values = QString::number(value);
    params.put("SteerRatioAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerRatioAdj"));
    int value = str.toInt();
    value = value + (digit*100);
    if (value >= 2000) {
      value = 2000;
    }
    QString values = QString::number(value);
    params.put("SteerRatioAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void SRBaseControl::refresh() {
  auto strs = QString::fromStdString(params.get("SteerRatioAdj"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
}

SRMaxControl::SRMaxControl() : AbstractControl("SteerRatioMax", "SteerRatio 최대값을 설정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btndigit.setStyleSheet(R"(
    QPushButton {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    }
    QPushButton:pressed {
      background-color: #ababab;
    }
  )");
  btnminus.setStyleSheet(R"(
    QPushButton {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    }
    QPushButton:pressed {
      background-color: #ababab;
    }
  )");
  btnplus.setStyleSheet(R"(
    QPushButton {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    }
    QPushButton:pressed {
      background-color: #ababab;
    }
  )");
  btndigit.setFixedSize(100, 100);
  btnminus.setFixedSize(100, 100);
  btnplus.setFixedSize(100, 100);
  hlayout->addWidget(&btndigit);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);
  btndigit.setText("0.01");
  btnminus.setText("-");
  btnplus.setText("+");

  QObject::connect(&btndigit, &QPushButton::clicked, [=]() {
    digit = digit * 10;
    if (digit >= 11 ) {
      digit = 0.01;
    }
    QString level = QString::number(digit);
    btndigit.setText(level);
  });

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerRatioMaxAdj"));
    int value = str.toInt();
    value = value - (digit*100);
    if (value <= 800) {
      value = 800;
    }
    QString values = QString::number(value);
    params.put("SteerRatioMaxAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerRatioMaxAdj"));
    int value = str.toInt();
    value = value + (digit*100);
    if (value >= 2000) {
      value = 2000;
    }
    QString values = QString::number(value);
    params.put("SteerRatioMaxAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void SRMaxControl::refresh() {
  auto strs = QString::fromStdString(params.get("SteerRatioMaxAdj"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
}

SteerActuatorDelay::SteerActuatorDelay() : AbstractControl("SteerActuatorDelay", "SteerActuatorDelay값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerActuatorDelayAdj"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("SteerActuatorDelayAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerActuatorDelayAdj"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 100 ) {
      value = 100;
    }
    QString values = QString::number(value);
    params.put("SteerActuatorDelayAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void SteerActuatorDelay::refresh() {
  auto strs = QString::fromStdString(params.get("SteerActuatorDelayAdj"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

SteerRateCost::SteerRateCost() : AbstractControl("SteerRateCost", "SteerRateCost값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerRateCostAdj"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("SteerRateCostAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerRateCostAdj"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 200 ) {
      value = 200;
    }
    QString values = QString::number(value);
    params.put("SteerRateCostAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void SteerRateCost::refresh() {
  auto strs = QString::fromStdString(params.get("SteerRateCostAdj"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

SteerLimitTimer::SteerLimitTimer() : AbstractControl("SteerLimitTimer", "SteerLimitTimer값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerLimitTimerAdj"));
    int value = str.toInt();
    value = value - 5;
    if (value <= 0 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("SteerLimitTimerAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerLimitTimerAdj"));
    int value = str.toInt();
    value = value + 5;
    if (value >= 300 ) {
      value = 300;
    }
    QString values = QString::number(value);
    params.put("SteerLimitTimerAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void SteerLimitTimer::refresh() {
  auto strs = QString::fromStdString(params.get("SteerLimitTimerAdj"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

TireStiffnessFactor::TireStiffnessFactor() : AbstractControl("TireStiffnessFactor", "TireStiffnessFactor값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("TireStiffnessFactorAdj"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("TireStiffnessFactorAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("TireStiffnessFactorAdj"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 200 ) {
      value = 200;
    }
    QString values = QString::number(value);
    params.put("TireStiffnessFactorAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void TireStiffnessFactor::refresh() {
  auto strs = QString::fromStdString(params.get("TireStiffnessFactorAdj"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

SteerMaxBase::SteerMaxBase() : AbstractControl("SteerMax기본값", "SteerMax기본값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerMaxBaseAdj"));
    int value = str.toInt();
    value = value - 2;
    if (value <= 200 ) {
      value = 200;
    }
    QString values = QString::number(value);
    params.put("SteerMaxBaseAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerMaxBaseAdj"));
    int value = str.toInt();
    value = value + 2;
    if (value >= 512 ) {
      value = 512;
    }
    QString values = QString::number(value);
    params.put("SteerMaxBaseAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void SteerMaxBase::refresh() {
  label.setText(QString::fromStdString(params.get("SteerMaxBaseAdj")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

SteerMaxMax::SteerMaxMax() : AbstractControl("SteerMax최대값", "SteerMax최대값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerMaxAdj"));
    int value = str.toInt();
    value = value - 2;
    if (value <= 254 ) {
      value = 254;
    }
    QString values = QString::number(value);
    params.put("SteerMaxAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerMaxAdj"));
    int value = str.toInt();
    value = value + 2;
    if (value >= 1024 ) {
      value = 1024;
    }
    QString values = QString::number(value);
    params.put("SteerMaxAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void SteerMaxMax::refresh() {
  label.setText(QString::fromStdString(params.get("SteerMaxAdj")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

SteerMaxv::SteerMaxv() : AbstractControl("SteerMaxV", "SteerMaxV값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerMaxvAdj"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 10 ) {
      value = 10;
    }
    QString values = QString::number(value);
    params.put("SteerMaxvAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerMaxvAdj"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 30 ) {
      value = 30;
    }
    QString values = QString::number(value);
    params.put("SteerMaxvAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void SteerMaxv::refresh() {
  auto strs = QString::fromStdString(params.get("SteerMaxvAdj"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.1;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

SteerDeltaUpBase::SteerDeltaUpBase() : AbstractControl("SteerDeltaUp기본값", "SteerDeltaUp기본값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerDeltaUpBaseAdj"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 2 ) {
      value = 2;
    }
    QString values = QString::number(value);
    params.put("SteerDeltaUpBaseAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerDeltaUpBaseAdj"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 7 ) {
      value = 7;
    }
    QString values = QString::number(value);
    params.put("SteerDeltaUpBaseAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void SteerDeltaUpBase::refresh() {
  label.setText(QString::fromStdString(params.get("SteerDeltaUpBaseAdj")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

SteerDeltaUpMax::SteerDeltaUpMax() : AbstractControl("SteerDeltaUp최대값", "SteerDeltaUp최대값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerDeltaUpAdj"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 3 ) {
      value = 3;
    }
    QString values = QString::number(value);
    params.put("SteerDeltaUpAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerDeltaUpAdj"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 7 ) {
      value = 7;
    }
    QString values = QString::number(value);
    params.put("SteerDeltaUpAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void SteerDeltaUpMax::refresh() {
  label.setText(QString::fromStdString(params.get("SteerDeltaUpAdj")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

SteerDeltaDownBase::SteerDeltaDownBase() : AbstractControl("SteerDeltaDown기본값", "SteerDeltaDown기본값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerDeltaDownBaseAdj"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 3 ) {
      value = 3;
    }
    QString values = QString::number(value);
    params.put("SteerDeltaDownBaseAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerDeltaDownBaseAdj"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 15 ) {
      value = 15;
    }
    QString values = QString::number(value);
    params.put("SteerDeltaDownBaseAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void SteerDeltaDownBase::refresh() {
  label.setText(QString::fromStdString(params.get("SteerDeltaDownBaseAdj")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

SteerDeltaDownMax::SteerDeltaDownMax() : AbstractControl("SteerDeltaDown최대값", "SteerDeltaDown최대값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerDeltaDownAdj"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 7 ) {
      value = 7;
    }
    QString values = QString::number(value);
    params.put("SteerDeltaDownAdj", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerDeltaDownAdj"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 15 ) {
      value = 15;
    }
    QString values = QString::number(value);
    params.put("SteerDeltaDownAdj", values.toStdString());
    refresh();
  });
  refresh();
}

void SteerDeltaDownMax::refresh() {
  label.setText(QString::fromStdString(params.get("SteerDeltaDownAdj")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

SteerThreshold::SteerThreshold() : AbstractControl("SteerThreshold", "SteerThreshold값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerThreshold"));
    int value = str.toInt();
    value = value - 10;
    if (value <= 50 ) {
      value = 50;
    }
    QString values = QString::number(value);
    params.put("SteerThreshold", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("SteerThreshold"));
    int value = str.toInt();
    value = value + 10;
    if (value >= 300 ) {
      value = 300;
    }
    QString values = QString::number(value);
    params.put("SteerThreshold", values.toStdString());
    refresh();
  });
  refresh();
}

void SteerThreshold::refresh() {
  label.setText(QString::fromStdString(params.get("SteerThreshold")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

//제어
LateralControl::LateralControl() : AbstractControl("조향제어(Reboot)", "조향제어 방법을 설정합니다(PID/INDI/LQR). Reboot Required.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LateralControlMethod"));
    int latcontrol = str.toInt();
    latcontrol = latcontrol - 1;
    if (latcontrol <= -1 ) {
      latcontrol = 2;
    }
    QString latcontrols = QString::number(latcontrol);
    params.put("LateralControlMethod", latcontrols.toStdString());
    refresh();
  });

  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LateralControlMethod"));
    int latcontrol = str.toInt();
    latcontrol = latcontrol + 1;
    if (latcontrol >= 3 ) {
      latcontrol = 0;
    }
    QString latcontrols = QString::number(latcontrol);
    params.put("LateralControlMethod", latcontrols.toStdString());
    refresh();
  });
  refresh();
}

void LateralControl::refresh() {
  QString latcontrol = QString::fromStdString(params.get("LateralControlMethod"));
  if (latcontrol == "0") {
    label.setText(QString::fromStdString("PID"));
  } else if (latcontrol == "1") {
    label.setText(QString::fromStdString("INDI"));
  } else if (latcontrol == "2") {
    label.setText(QString::fromStdString("LQR"));
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

PidKp::PidKp() : AbstractControl("Kp", "Kp값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("PidKp"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("PidKp", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("PidKp"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 50 ) {
      value = 50;
    }
    QString values = QString::number(value);
    params.put("PidKp", values.toStdString());
    refresh();
  });
  refresh();
}

void PidKp::refresh() {
  auto strs = QString::fromStdString(params.get("PidKp"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

PidKi::PidKi() : AbstractControl("Ki", "Ki값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("PidKi"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("PidKi", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("PidKi"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 100 ) {
      value = 100;
    }
    QString values = QString::number(value);
    params.put("PidKi", values.toStdString());
    refresh();
  });
  refresh();
}

void PidKi::refresh() {
  auto strs = QString::fromStdString(params.get("PidKi"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.001;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

PidKd::PidKd() : AbstractControl("Kd", "Kd값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("PidKd"));
    int value = str.toInt();
    value = value - 5;
    if (value <= 0 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("PidKd", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("PidKd"));
    int value = str.toInt();
    value = value + 5;
    if (value >= 300 ) {
      value = 300;
    }
    QString values = QString::number(value);
    params.put("PidKd", values.toStdString());
    refresh();
  });
  refresh();
}

void PidKd::refresh() {
  auto strs = QString::fromStdString(params.get("PidKd"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

PidKf::PidKf() : AbstractControl("Kf", "Kf값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("PidKf"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("PidKf", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("PidKf"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 50 ) {
      value = 50;
    }
    QString values = QString::number(value);
    params.put("PidKf", values.toStdString());
    refresh();
  });
  refresh();
}

void PidKf::refresh() {
  auto strs = QString::fromStdString(params.get("PidKf"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.00001;
  QString valuefs = QString::number(valuef, 'f', 5);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

OuterLoopGain::OuterLoopGain() : AbstractControl("OuterLoopGain", "OuterLoopGain값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OuterLoopGain"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("OuterLoopGain", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OuterLoopGain"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 200 ) {
      value = 200;
    }
    QString values = QString::number(value);
    params.put("OuterLoopGain", values.toStdString());
    refresh();
  });
  refresh();
}

void OuterLoopGain::refresh() {
  auto strs = QString::fromStdString(params.get("OuterLoopGain"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.1;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

InnerLoopGain::InnerLoopGain() : AbstractControl("InnerLoopGain", "InnerLoopGain값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("InnerLoopGain"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("InnerLoopGain", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("InnerLoopGain"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 200 ) {
      value = 200;
    }
    QString values = QString::number(value);
    params.put("InnerLoopGain", values.toStdString());
    refresh();
  });
  refresh();
}

void InnerLoopGain::refresh() {
  auto strs = QString::fromStdString(params.get("InnerLoopGain"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.1;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

TimeConstant::TimeConstant() : AbstractControl("TimeConstant", "TimeConstant값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("TimeConstant"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("TimeConstant", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("TimeConstant"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 200 ) {
      value = 200;
    }
    QString values = QString::number(value);
    params.put("TimeConstant", values.toStdString());
    refresh();
  });
  refresh();
}

void TimeConstant::refresh() {
  auto strs = QString::fromStdString(params.get("TimeConstant"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.1;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

ActuatorEffectiveness::ActuatorEffectiveness() : AbstractControl("ActuatorEffectiveness", "ActuatorEffectiveness값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("ActuatorEffectiveness"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("ActuatorEffectiveness", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("ActuatorEffectiveness"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 200 ) {
      value = 200;
    }
    QString values = QString::number(value);
    params.put("ActuatorEffectiveness", values.toStdString());
    refresh();
  });
  refresh();
}

void ActuatorEffectiveness::refresh() {
  auto strs = QString::fromStdString(params.get("ActuatorEffectiveness"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.1;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

Scale::Scale() : AbstractControl("Scale", "Scale값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("Scale"));
    int value = str.toInt();
    value = value - 50;
    if (value <= 50 ) {
      value = 50;
    }
    QString values = QString::number(value);
    params.put("Scale", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("Scale"));
    int value = str.toInt();
    value = value + 50;
    if (value >= 5000 ) {
      value = 5000;
    }
    QString values = QString::number(value);
    params.put("Scale", values.toStdString());
    refresh();
  });
  refresh();
}

void Scale::refresh() {
  label.setText(QString::fromStdString(params.get("Scale")));
  btnminus.setText("－");
  btnplus.setText("＋");
}

LqrKi::LqrKi() : AbstractControl("LqrKi", "ki값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LqrKi"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("LqrKi", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LqrKi"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 100 ) {
      value = 100;
    }
    QString values = QString::number(value);
    params.put("LqrKi", values.toStdString());
    refresh();
  });
  refresh();
}

void LqrKi::refresh() {
  auto strs = QString::fromStdString(params.get("LqrKi"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.001;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

DcGain::DcGain() : AbstractControl("DcGain", "DcGain값을 조정합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("DcGain"));
    int value = str.toInt();
    value = value - 5;
    if (value <= 5 ) {
      value = 5;
    }
    QString values = QString::number(value);
    params.put("DcGain", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("DcGain"));
    int value = str.toInt();
    value = value + 5;
    if (value >= 500 ) {
      value = 500;
    }
    QString values = QString::number(value);
    params.put("DcGain", values.toStdString());
    refresh();
  });
  refresh();
}

void DcGain::refresh() {
  auto strs = QString::fromStdString(params.get("DcGain"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.00001;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

CruiseGapTR::CruiseGapTR() : AbstractControl("크루즈갭", "크루즈갭에 따른 차간거리(TR)를 조절 합니다. TR은 앞차와 추돌시간(초)을 말하며 커질수록 앞차와 더 먼 간격을 유지합니다.", "") {
  QString dtr = QString::fromStdString(params.get("DynamicTRGap"));
  if (dtr == "0") {
    btn1.setStyleSheet(R"(
      padding: -10;
      border-radius: 35px;
      font-size: 30px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    )");
    btn2.setStyleSheet(R"(
      padding: -10;
      border-radius: 35px;
      font-size: 30px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    )");
    btn3.setStyleSheet(R"(
      padding: -10;
      border-radius: 35px;
      font-size: 30px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    )");
    btn4.setStyleSheet(R"(
      padding: -10;
      border-radius: 35px;
      font-size: 30px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    )");
  } else {
    btn1.setStyleSheet(R"(
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    )");
    btn2.setStyleSheet(R"(
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    )");
    btn3.setStyleSheet(R"(
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    )");
    btn4.setStyleSheet(R"(
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    )");
  }
  label1.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label1.setStyleSheet("color: #e0e879");
  label2.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label2.setStyleSheet("color: #e0e879");
  label3.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label3.setStyleSheet("color: #e0e879");
  label4.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label4.setStyleSheet("color: #e0e879");
  label1a.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label2a.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label3a.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label4a.setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  if (dtr != "1") {
    hlayout->addWidget(&label1a);
    hlayout->addWidget(&label1);
    if (dtr == "0") {
      btn1.setFixedSize(60, 100);
      label1a.setText("1:");
    } else {
      btn1.setFixedSize(100, 100);
      label1a.setText("1S:");
    }
    hlayout->addWidget(&btn1);
  }
  if (dtr != "2") {
    hlayout->addWidget(&label2a);
    hlayout->addWidget(&label2);
    if (dtr == "0") {
      btn2.setFixedSize(60, 100);
      label2a.setText("2:");
    } else {
      btn2.setFixedSize(100, 100);
      label2a.setText("2S:");
    }
    hlayout->addWidget(&btn2);
  }
  if (dtr != "3") {
    hlayout->addWidget(&label3a);
    hlayout->addWidget(&label3);
    if (dtr == "0") {
      btn3.setFixedSize(60, 100);
      label3a.setText("3:");
    } else {
      btn3.setFixedSize(100, 100);
      label3a.setText("3S:");
    }
    hlayout->addWidget(&btn3);
  }
  if (dtr != "4") {
    hlayout->addWidget(&label4a);
    hlayout->addWidget(&label4);
    if (dtr == "0") {
      btn4.setFixedSize(60, 100);
      label4a.setText("4:");
    } else {
      btn4.setFixedSize(100, 100);
      label4a.setText("4S:");
    }
    hlayout->addWidget(&btn4);
  }

  QObject::connect(&btn1, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("CruiseGap1"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 13 ) {
      value = 7;
    }
    QString values = QString::number(value);
    params.put("CruiseGap1", values.toStdString());
    refresh1();
  });

  QObject::connect(&btn2, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("CruiseGap2"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 16 ) {
      value = 8;
    }
    QString values = QString::number(value);
    params.put("CruiseGap2", values.toStdString());
    refresh2();
  });
  
  QObject::connect(&btn3, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("CruiseGap3"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 20 ) {
      value = 10;
    }
    QString values = QString::number(value);
    params.put("CruiseGap3", values.toStdString());
    refresh3();
  });

  QObject::connect(&btn4, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("CruiseGap4"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 30 ) {
      value = 12;
    }
    QString values = QString::number(value);
    params.put("CruiseGap4", values.toStdString());
    refresh4();
  });

  refresh1();
  refresh2();
  refresh3();
  refresh4();
}

void CruiseGapTR::refresh1() {
  auto strs = QString::fromStdString(params.get("CruiseGap1"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.1;
  QString valuefs = QString::number(valuef);
  label1.setText(QString::fromStdString(valuefs.toStdString()));
  btn1.setText("▲");
}
void CruiseGapTR::refresh2() {
  auto strs = QString::fromStdString(params.get("CruiseGap2"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.1;
  QString valuefs = QString::number(valuef);
  label2.setText(QString::fromStdString(valuefs.toStdString()));
  btn2.setText("▲");
}
void CruiseGapTR::refresh3() {
  auto strs = QString::fromStdString(params.get("CruiseGap3"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.1;
  QString valuefs = QString::number(valuef);
  label3.setText(QString::fromStdString(valuefs.toStdString()));
  btn3.setText("▲");
}
void CruiseGapTR::refresh4() {
  auto strs = QString::fromStdString(params.get("CruiseGap4"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.1;
  QString valuefs = QString::number(valuef);
  label4.setText(QString::fromStdString(valuefs.toStdString()));
  btn4.setText("▲");
}

DynamicTRGap::DynamicTRGap() : AbstractControl("다이나믹TR 사용(갭할당)", "DynamicTR을 사용 및 해당갭에 할당합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("DynamicTRGap"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -1 ) {
      value = 4;
    }
    QString values = QString::number(value);
    params.put("DynamicTRGap", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("DynamicTRGap"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 5 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("DynamicTRGap", values.toStdString());
    refresh();
  });
  refresh();
}

void DynamicTRGap::refresh() {
  QString option = QString::fromStdString(params.get("DynamicTRGap"));
  if (option == "0") {
    label.setText(QString::fromStdString("사용안함"));
  } else if (option == "1") {
    label.setText(QString::fromStdString("■"));
  } else if (option == "2") {
    label.setText(QString::fromStdString("■■"));
  } else if (option == "3") {
    label.setText(QString::fromStdString("■■■"));
  } else {
    label.setText(QString::fromStdString("■■■■"));
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

LCTimingFactor::LCTimingFactor() : AbstractControl("", "", "") {

  btn1.setStyleSheet(R"(
    padding: -10;
    border-radius: 35px;
    font-size: 30px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn2.setStyleSheet(R"(
    padding: -10;
    border-radius: 35px;
    font-size: 30px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn3.setStyleSheet(R"(
    padding: -10;
    border-radius: 35px;
    font-size: 30px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn4.setStyleSheet(R"(
    padding: -10;
    border-radius: 35px;
    font-size: 30px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  label1.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  label1.setStyleSheet("color: #e0e879");
  label2.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  label2.setStyleSheet("color: #e0e879");
  label3.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  label3.setStyleSheet("color: #e0e879");
  label4.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  label4.setStyleSheet("color: #e0e879");
  label1a.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  label2a.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  label3a.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  label4a.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  hlayout->addWidget(&label1a);
  hlayout->addWidget(&label1);
  btn1.setFixedSize(50, 100);
  label1a.setText("30:");
  hlayout->addWidget(&btn1);
  hlayout->addWidget(&label2a);
  hlayout->addWidget(&label2);
  btn2.setFixedSize(50, 100);
  label2a.setText("60:");
  hlayout->addWidget(&btn2);
  hlayout->addWidget(&label3a);
  hlayout->addWidget(&label3);
  btn3.setFixedSize(50, 100);
  label3a.setText("80:");
  hlayout->addWidget(&btn3);
  hlayout->addWidget(&label4a);
  hlayout->addWidget(&label4);
  btn4.setFixedSize(50, 100);
  label4a.setText("110:");
  hlayout->addWidget(&btn4);

  QObject::connect(&btn1, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LCTimingFactor30"));
    int value = str.toInt();
    auto str2 = QString::fromStdString(params.get("LCTimingFactor60"));
    int value2 = str2.toInt();
    auto str_ud = QString::fromStdString(params.get("LCTimingFactorUD"));
    if (str_ud == "1") {
      value = value + 5;
    } else {
      value = value - 5;
    }
    if (value >= value2 ) {
      value = value2;
    } else if (value <= 5 ) {
      value = 5;
    }
    QString values = QString::number(value);
    params.put("LCTimingFactor30", values.toStdString());
    refresh1();
  });

  QObject::connect(&btn2, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LCTimingFactor60"));
    int value = str.toInt();
    auto str0 = QString::fromStdString(params.get("LCTimingFactor30"));
    int value0 = str0.toInt();
    auto str2 = QString::fromStdString(params.get("LCTimingFactor80"));
    int value2 = str2.toInt();
    auto str_ud = QString::fromStdString(params.get("LCTimingFactorUD"));
    if (str_ud == "1") {
      value = value + 5;
    } else {
      value = value - 5;
    }
    if (value >= value2 ) {
      value = value2;
    } else if (value <= value0 ) {
      value = value0;
    }
    QString values = QString::number(value);
    params.put("LCTimingFactor60", values.toStdString());
    refresh2();
  });
  
  QObject::connect(&btn3, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LCTimingFactor80"));
    int value = str.toInt();
    auto str0 = QString::fromStdString(params.get("LCTimingFactor60"));
    int value0 = str0.toInt();
    auto str2 = QString::fromStdString(params.get("LCTimingFactor110"));
    int value2 = str2.toInt();
    auto str_ud = QString::fromStdString(params.get("LCTimingFactorUD"));
    if (str_ud == "1") {
      value = value + 5;
    } else {
      value = value - 5;
    }
    if (value >= value2 ) {
      value = value2;
    } else if (value <= value0 ) {
      value = value0;
    }
    QString values = QString::number(value);
    params.put("LCTimingFactor80", values.toStdString());
    refresh3();
  });

  QObject::connect(&btn4, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LCTimingFactor110"));
    int value = str.toInt();
    auto str0 = QString::fromStdString(params.get("LCTimingFactor80"));
    int value0 = str0.toInt();
    auto str_ud = QString::fromStdString(params.get("LCTimingFactorUD"));
    if (str_ud == "1") {
      value = value + 5;
    } else {
      value = value - 5;
    }
    if (value <= value0 ) {
      value = value0;
    } else if (value >= 300 ) {
      value = 300;
    }
    QString values = QString::number(value);
    params.put("LCTimingFactor110", values.toStdString());
    refresh4();
  });

  refresh1();
  refresh2();
  refresh3();
  refresh4();
}

void LCTimingFactor::refresh1() {
  auto strs = QString::fromStdString(params.get("LCTimingFactor30"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label1.setText(QString::fromStdString(valuefs.toStdString()));
  btn1.setText("↕");
}
void LCTimingFactor::refresh2() {
  auto strs = QString::fromStdString(params.get("LCTimingFactor60"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label2.setText(QString::fromStdString(valuefs.toStdString()));
  btn2.setText("↕");
}
void LCTimingFactor::refresh3() {
  auto strs = QString::fromStdString(params.get("LCTimingFactor80"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label3.setText(QString::fromStdString(valuefs.toStdString()));
  btn3.setText("↕");
}
void LCTimingFactor::refresh4() {
  auto strs = QString::fromStdString(params.get("LCTimingFactor110"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label4.setText(QString::fromStdString(valuefs.toStdString()));
  btn4.setText("↕");
}

LCTimingFactorUD::LCTimingFactorUD() : AbstractControl("차선변경 타이밍(km/h: 정도값)", "차선변경 시 해당 속도별 차선변경 타이밍을 조절합니다. 빠른 차선변경을 원할경우 값을 높이고 느린 차선변경을 원할경우 값을 낮추세요.", "../assets/offroad/icon_shell.png") {

  btn.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn.setFixedSize(125, 100);
  btn2.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn2.setFixedSize(150, 100);
  hlayout->addWidget(&btn2);
  hlayout->addWidget(&btn);

  QObject::connect(&btn2, &QPushButton::clicked, [=]() {
    bool stat = params.getBool("LCTimingFactorEnable");
    if (stat) {
      params.putBool("LCTimingFactorEnable", false);
    } else {
      params.putBool("LCTimingFactorEnable", true);
    }
    refresh2();
  });

  QObject::connect(&btn, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LCTimingFactorUD"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 2 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("LCTimingFactorUD", values.toStdString());
    refresh();
  });
  refresh();
  refresh2();
}

void LCTimingFactorUD::refresh() {
  auto strs = QString::fromStdString(params.get("LCTimingFactorUD"));
  if (strs == "1") {
    btn.setText("↑");
  } else {
    btn.setText("↓");
  }
}

void LCTimingFactorUD::refresh2() {
  bool param = params.getBool("LCTimingFactorEnable");
  if (param) {
    btn2.setText("사용중");
    btn2.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #00A12E;
    )");
  } else {
    btn2.setText("미사용");
    btn2.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  }
}

LiveSRPercent::LiveSRPercent() : AbstractControl("LiveSR 비율조정(%)", "LiveSR 사용시 학습된 값을 임의로 조정(%)하여 사용합니다. -값:학습된값에서 낮춤, +값:학습된값에서 높임", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LiveSteerRatioPercent"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -50 ) {
      value = -50;
    }
    QString values = QString::number(value);
    params.put("LiveSteerRatioPercent", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LiveSteerRatioPercent"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 50 ) {
      value = 50;
    }
    QString values = QString::number(value);
    params.put("LiveSteerRatioPercent", values.toStdString());
    refresh();
  });
  refresh();
}

void LiveSRPercent::refresh() {
  QString option = QString::fromStdString(params.get("LiveSteerRatioPercent"));
  if (option == "0") {
    label.setText(QString::fromStdString("기본값"));
  } else {
    label.setText(QString::fromStdString(params.get("LiveSteerRatioPercent")));
  }
  btnminus.setText("-");
  btnplus.setText("+");
}

VCurvSpeedUD::VCurvSpeedUD() : AbstractControl("VisionCurvDecel([CV] [TargetSpeed])", "Adjust the curve deceleration speed according to the model speed(curvature). (interpolation and list value)", "../assets/offroad/icon_shell.png") {
}

VCurvSpeed::VCurvSpeed() : AbstractControl("", "", "") {
  btn.setStyleSheet(R"(
    padding: -10;
    border-radius: 35px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  edit1.setStyleSheet(R"(
    background-color: grey;
    font-size: 55px;
    font-weight: 500;
    height: 120px;
  )");
  edit2.setStyleSheet(R"(
    background-color: grey;
    font-size: 55px;
    font-weight: 500;
    height: 120px;
  )");
  btn.setFixedSize(150, 100);
  edit1.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  edit2.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  hlayout->addWidget(&edit1);
  hlayout->addWidget(&edit2);
  hlayout->addWidget(&btn);

  QObject::connect(&btn, &QPushButton::clicked, [=]() {
    int list_count1 = 0;
    int list_count2 = 0;
    QString targetvalue1 = InputDialog::getText("Set CV values with comma", this, "ex) 30,50,70,90,110", false, 1, QString::fromStdString(params.get("VCurvSpeedC")));
    if (targetvalue1.length() > 0 && targetvalue1 != QString::fromStdString(params.get("VCurvSpeedC"))) {
      QStringList list1 = targetvalue1.split(",");
      list_count1 = list1.size();
      params.put("VCurvSpeedC", targetvalue1.toStdString());
      refresh();
    } else {
      QStringList list1 = QString::fromStdString(params.get("VCurvSpeedC")).split(",");
      list_count1 = list1.size();
    }
    QString targetvalue2 = InputDialog::getText("Set TS values with comma", this, "ex) 45,55,65,75,85", false, 1, QString::fromStdString(params.get("VCurvSpeedT")));
    if (targetvalue2.length() > 0 && targetvalue2 != QString::fromStdString(params.get("VCurvSpeedT"))) {
      QStringList list2 = targetvalue2.split(",");
      list_count2 = list2.size();
      params.put("VCurvSpeedT", targetvalue2.toStdString());
      refresh();
    } else {
      QStringList list2 = QString::fromStdString(params.get("VCurvSpeedT")).split(",");
      list_count2 = list2.size();
    }
    if (list_count1 != list_count2) {
      ConfirmationDialog::alert("Index count does not match. Check your input again.", this);
    }
  });
  refresh();
}

void VCurvSpeed::refresh() {
  auto strs1 = QString::fromStdString(params.get("VCurvSpeedC"));
  auto strs2 = QString::fromStdString(params.get("VCurvSpeedT"));
  edit1.setText(QString::fromStdString(strs1.toStdString()));
  edit2.setText(QString::fromStdString(strs2.toStdString()));
  btn.setText("EDIT");
}

OCurvSpeedUD::OCurvSpeedUD() : AbstractControl("OSMCurvDecel([TSL] [TargetSpeed])", "Adjust the curve deceleration speed according to turn speed limit of OSM. (interpolation value)", "../assets/offroad/icon_shell.png") {
}

OCurvSpeed::OCurvSpeed() : AbstractControl("", "", "") {
  btn.setStyleSheet(R"(
    padding: -10;
    border-radius: 35px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  edit1.setStyleSheet(R"(
    background-color: grey;
    font-size: 55px;
    font-weight: 500;
    height: 120px;
  )");
  edit2.setStyleSheet(R"(
    background-color: grey;
    font-size: 55px;
    font-weight: 500;
    height: 120px;
  )");
  btn.setFixedSize(150, 100);
  edit1.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  edit2.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  hlayout->addWidget(&edit1);
  hlayout->addWidget(&edit2);
  hlayout->addWidget(&btn);

  QObject::connect(&btn, &QPushButton::clicked, [=]() {
    int list_count1 = 0;
    int list_count2 = 0;
    QString targetvalue1 = InputDialog::getText("Set TSL values with comma", this, "ex) 30,40,50,60,70", false, 1, QString::fromStdString(params.get("OCurvSpeedC")));
    if (targetvalue1.length() > 0 && targetvalue1 != QString::fromStdString(params.get("OCurvSpeedC"))) {
      QStringList list1 = targetvalue1.split(",");
      list_count1 = list1.size();
      params.put("OCurvSpeedC", targetvalue1.toStdString());
      refresh();
    } else {
      QStringList list1 = QString::fromStdString(params.get("OCurvSpeedC")).split(",");
      list_count1 = list1.size();
    }
    QString targetvalue2 = InputDialog::getText("Set TS values with comma", this, "ex) 35,45,60,70,80", false, 1, QString::fromStdString(params.get("OCurvSpeedT")));
    if (targetvalue2.length() > 0 && targetvalue2 != QString::fromStdString(params.get("OCurvSpeedT"))) {
      QStringList list2 = targetvalue2.split(",");
      list_count2 = list2.size();
      params.put("OCurvSpeedT", targetvalue2.toStdString());
      refresh();
    } else {
      QStringList list2 = QString::fromStdString(params.get("OCurvSpeedT")).split(",");
      list_count2 = list2.size();
    }
    if (list_count1 != list_count2) {
      ConfirmationDialog::alert("Index count does not match. Check your input again.", this);
    }
  });
  refresh();
}

void OCurvSpeed::refresh() {
  auto strs1 = QString::fromStdString(params.get("OCurvSpeedC"));
  auto strs2 = QString::fromStdString(params.get("OCurvSpeedT"));
  edit1.setText(QString::fromStdString(strs1.toStdString()));
  edit2.setText(QString::fromStdString(strs2.toStdString()));
  btn.setText("EDIT");
}

GetOffAlert::GetOffAlert() : AbstractControl("이온 탈착 경고음", "시동이 꺼지면 장치가 이온을 분리하도록 경보를 알려줍니다.(알리지 않음/한국어/영어)", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrEnableGetoffAlert"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -1 ) {
      value = 2;
    }
    QString values = QString::number(value);
    params.put("OpkrEnableGetoffAlert", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrEnableGetoffAlert"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 3 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("OpkrEnableGetoffAlert", values.toStdString());
    refresh();
  });
  refresh();
}

void GetOffAlert::refresh() {
  QString option = QString::fromStdString(params.get("OpkrEnableGetoffAlert"));
  if (option == "0") {
    label.setText(QString::fromStdString("None"));
  } else if (option == "1") {
    label.setText(QString::fromStdString("KOR"));
  } else {
    label.setText(QString::fromStdString("ENG"));
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

OPKRNaviSelect::OPKRNaviSelect() : AbstractControl("Navigation Select", "Select the navigation you want to use.(Mappy/Waze/None)", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OPKRNaviSelect"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -1 ) {
      value = 2;
    }
    QString values = QString::number(value);
    params.put("OPKRNaviSelect", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OPKRNaviSelect"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 3 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("OPKRNaviSelect", values.toStdString());
    refresh();
  });
  refresh();
}

void OPKRNaviSelect::refresh() {
  QString option = QString::fromStdString(params.get("OPKRNaviSelect"));
  if (option == "0") {
    label.setText(QString::fromStdString("Mappy"));
  } else if (option == "1") {
    label.setText(QString::fromStdString("Waze"));
  } else {
    label.setText(QString::fromStdString("None"));
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

OPKRServerSelect::OPKRServerSelect() : AbstractControl("API Server", "Set API server to Retropilot/Comma/User's", "../assets/offroad/icon_shell.png") {
  btn1.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn1.setFixedSize(250, 100);
  btn2.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn2.setFixedSize(250, 100);
  btn3.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn3.setFixedSize(250, 100);
  hlayout->addWidget(&btn1);
  hlayout->addWidget(&btn2);
  hlayout->addWidget(&btn3);
  btn1.setText("Retropilot");
  btn2.setText("Comma");
  btn3.setText("User's");

  QObject::connect(&btn1, &QPushButton::clicked, [=]() {
    params.put("OPKRServer", "0");
    refresh();
  });
  QObject::connect(&btn2, &QPushButton::clicked, [=]() {
    params.put("OPKRServer", "1");
    if (ConfirmationDialog::alert("You've chosen comma server. Your uploads might be ignored if you upload your data. I highly recommend you should reset the device to avoid be banned.", this)) {}
    refresh();
  });
  QObject::connect(&btn3, &QPushButton::clicked, [=]() {
    params.put("OPKRServer", "2");
    if (ConfirmationDialog::alert("You've chosen own server. Please set your api host at the menu below.", this)) {}
    refresh();
  });
  refresh();
}

void OPKRServerSelect::refresh() {
  QString option = QString::fromStdString(params.get("OPKRServer"));
  if (option == "0") {
    btn1.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #00A12E;
    )");
    btn2.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
    )");
    btn3.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
    )");
  } else if (option == "1") {
    btn1.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
    )");
    btn2.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #00A12E;
    )");
    btn3.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
    )");
  } else {
    btn1.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
    )");
    btn2.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
    )");
    btn3.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #00A12E;
    )");
  }
}

OPKRServerAPI::OPKRServerAPI() : AbstractControl("User's API", "Set Your API server URL or IP", "") {
  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);
  btn.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn.setFixedSize(150, 100);
  hlayout->addWidget(&btn);

  QObject::connect(&btn, &QPushButton::clicked, [=]() {
    if (btn.text() == "SET") {
      QString users_api_host = InputDialog::getText("Input Your API(url or ip):", this);
      if (users_api_host.length() > 0) {
        QString cmd0 = QString::fromStdString("Your Input is\n") + users_api_host + QString::fromStdString("\nPress OK to apply&reboot");
        if (ConfirmationDialog::confirm(cmd0, this)) {
          params.put("OPKRServerAPI", users_api_host.toStdString());
          params.put("OPKRServer", "2");
          QProcess::execute("rm -f /data/params/d/DongleId");
          QProcess::execute("rm -f /data/params/d/IMEI");
          QProcess::execute("rm -f /data/params/d/HardwareSerial");
          QProcess::execute("reboot");
        }
      }
    } else if (btn.text() == "UNSET") {
      if (ConfirmationDialog::confirm("Do you want to unset? the API server gets back to Retropilot server and Device will be rebooted now.", this)) {
        params.remove("OPKRServerAPI");
        params.put("OPKRServer", "0");
        QProcess::execute("rm -f /data/params/d/DongleId");
        QProcess::execute("rm -f /data/params/d/IMEI");
        QProcess::execute("rm -f /data/params/d/HardwareSerial");
        QProcess::execute("reboot");
      }
    }
  });
  refresh();
}

void OPKRServerAPI::refresh() {
  auto str = QString::fromStdString(params.get("OPKRServerAPI"));
  if (str.length() > 0) {
    label.setText(QString::fromStdString(params.get("OPKRServerAPI")));
    btn.setText("UNSET");
  } else {
    btn.setText("SET");
  }
}

OPKRMapboxStyle::OPKRMapboxStyle() : AbstractControl("Mapbox Style", "Set the Mapbox sytle to Comma/OPKR/User's", "../assets/offroad/icon_shell.png") {
  btn1.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn1.setFixedSize(250, 100);
  btn2.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn2.setFixedSize(250, 100);
  btn3.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn3.setFixedSize(250, 100);
  hlayout->addWidget(&btn1);
  hlayout->addWidget(&btn2);
  hlayout->addWidget(&btn3);
  btn1.setText("Comma");
  btn2.setText("OPKR");
  btn3.setText("User's");

  QObject::connect(&btn1, &QPushButton::clicked, [=]() {
    params.put("OPKRMapboxStyleSelect", "0");
    refresh();
  });
  QObject::connect(&btn2, &QPushButton::clicked, [=]() {
    params.put("OPKRMapboxStyleSelect", "1");
    refresh();
  });
  QObject::connect(&btn3, &QPushButton::clicked, [=]() {
    params.put("OPKRMapboxStyleSelect", "2");
    if (ConfirmationDialog::alert("You've chosen own style. Please set your mapbox style to the param <MapboxStyleCustom>. File location: /data/params/d/MapboxStyleCustom", this)) {}
    refresh();
  });
  refresh();
}

void OPKRMapboxStyle::refresh() {
  QString option = QString::fromStdString(params.get("OPKRMapboxStyleSelect"));
  if (option == "0") {
    btn1.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #00A12E;
    )");
    btn2.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
    )");
    btn3.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
    )");
  } else if (option == "1") {
    btn1.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
    )");
    btn2.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #00A12E;
    )");
    btn3.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
    )");
  } else {
    btn1.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
    )");
    btn2.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
    )");
    btn3.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #00A12E;
    )");
  }
}

RESCountatStandstill::RESCountatStandstill() : AbstractControl("정지시 RES 카운트", "컴마 기본값: 25, 이 값은 일부 차량에서 허용되지 않습니다. 따라서 원하는 경우 숫자를 조정하십시오. 선행차가 움직일 때 RES CAN 메시지를 생성합니다. 출발에 실패하면 숫자를 늘립니다. 반대로 CAN 에러가 발생하면 숫자를 줄이십시오. ", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("RESCountatStandstill"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("RESCountatStandstill", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("RESCountatStandstill"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 50 ) {
      value = 50;
    }
    QString values = QString::number(value);
    params.put("RESCountatStandstill", values.toStdString());
    refresh();
  });
  refresh();
}

void RESCountatStandstill::refresh() {
  label.setText(QString::fromStdString(params.get("RESCountatStandstill")));
  btnminus.setText("-");
  btnplus.setText("+");
}

SpeedLimitSignType::SpeedLimitSignType() : AbstractControl("과속카메라 표시 유형", "과속카메라 표시 유형을 선택하세요 (원/사각형)", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrSpeedLimitSignType"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -1 ) {
      value = 1;
    }
    QString values = QString::number(value);
    params.put("OpkrSpeedLimitSignType", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrSpeedLimitSignType"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 2 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("OpkrSpeedLimitSignType", values.toStdString());
    refresh();
  });
  refresh();
}

void SpeedLimitSignType::refresh() {
  QString option = QString::fromStdString(params.get("OpkrSpeedLimitSignType"));
  if (option == "0") {
    label.setText(QString::fromStdString("Circle"));
  } else {
    label.setText(QString::fromStdString("Rectangle"));
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

RadarLongHelperOption::RadarLongHelperOption() : AbstractControl("Radar Long Assist", "Vision Only, Vision+Radar, Radar Only, OPKR Custom", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("RadarLongHelper"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -1 ) {
      value = 3;
    }
    QString values = QString::number(value);
    params.put("RadarLongHelper", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("RadarLongHelper"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 4 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("RadarLongHelper", values.toStdString());
    refresh();
  });
  refresh();
}

void RadarLongHelperOption::refresh() {
  QString option = QString::fromStdString(params.get("RadarLongHelper"));
  if (option == "0") {
    label.setText(QString::fromStdString("Vision Only"));
  } else if (option == "1") {
    label.setText(QString::fromStdString("Vision+Radar"));
  } else if (option == "2") {
    label.setText(QString::fromStdString("Radar Only"));
  } else {
    label.setText(QString::fromStdString("OPKR Custom"));
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

CurvDecelSelect::CurvDecelSelect() : AbstractControl("곡선 감속 옵션", "없음, 비전+OSM, 비전, OSM", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("CurvDecelOption"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -1 ) {
      value = 3;
    }
    QString values = QString::number(value);
    params.put("CurvDecelOption", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("CurvDecelOption"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 4 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("CurvDecelOption", values.toStdString());
    refresh();
  });
  refresh();
}

void CurvDecelSelect::refresh() {
  QString option = QString::fromStdString(params.get("CurvDecelOption"));
  if (option == "0") {
    label.setText(QString::fromStdString("없음"));
  } else if (option == "1") {
    label.setText(QString::fromStdString("비전+OSM"));
  } else if (option == "2") {
    label.setText(QString::fromStdString("비전"));
  } else {
    label.setText(QString::fromStdString("OSM Only"));
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

AutoRESDelay::AutoRESDelay() : AbstractControl("AutoRES Delay(sec)", "Give delay time to trigger for AutoRES while driving.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("AutoRESDelay"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 0 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("AutoRESDelay", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("AutoRESDelay"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 10 ) {
      value = 10;
    }
    QString values = QString::number(value);
    params.put("AutoRESDelay", values.toStdString());
    refresh();
  });
  refresh();
}

void AutoRESDelay::refresh() {
  QString option = QString::fromStdString(params.get("AutoRESDelay"));
  if (option == "0") {
    label.setText(QString::fromStdString("No Delay"));
  } else {
    label.setText(QString::fromStdString(params.get("AutoRESDelay")));
  }
  btnminus.setText("-");
  btnplus.setText("+");
}

OSMCustomSpeedLimitUD::OSMCustomSpeedLimitUD() : AbstractControl("OSMCustomSpeedLimit([SL] [TargetSpeed])", "Set the offset speed according to speed limit of OSM. (interpolation value)", "../assets/offroad/icon_shell.png") {
}

OSMCustomSpeedLimit::OSMCustomSpeedLimit() : AbstractControl("", "", "") {
  btn.setStyleSheet(R"(
    padding: -10;
    border-radius: 35px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  edit1.setStyleSheet(R"(
    background-color: grey;
    font-size: 55px;
    font-weight: 500;
    height: 120px;
  )");
  edit2.setStyleSheet(R"(
    background-color: grey;
    font-size: 55px;
    font-weight: 500;
    height: 120px;
  )");
  btn.setFixedSize(150, 100);
  edit1.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  edit2.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  hlayout->addWidget(&edit1);
  hlayout->addWidget(&edit2);
  hlayout->addWidget(&btn);

  QObject::connect(&btn, &QPushButton::clicked, [=]() {
    int list_count1 = 0;
    int list_count2 = 0;
    QString targetvalue1 = InputDialog::getText("Set SL values with comma", this, "ex) 30,40,50,60,70,90", false, 1, QString::fromStdString(params.get("OSMCustomSpeedLimitC")));
    if (targetvalue1.length() > 0 && targetvalue1 != QString::fromStdString(params.get("OSMCustomSpeedLimitC"))) {
      QStringList list1 = targetvalue1.split(",");
      list_count1 = list1.size();
      params.put("OSMCustomSpeedLimitC", targetvalue1.toStdString());
      refresh();
    } else {
      QStringList list1 = QString::fromStdString(params.get("OSMCustomSpeedLimitC")).split(",");
      list_count1 = list1.size();
    }
    QString targetvalue2 = InputDialog::getText("Set CTSL values with comma", this, "ex) 30,40,65,72,80,95", false, 1, QString::fromStdString(params.get("OSMCustomSpeedLimitT")));
    if (targetvalue2.length() > 0 && targetvalue2 != QString::fromStdString(params.get("OSMCustomSpeedLimitT"))) {
      QStringList list2 = targetvalue2.split(",");
      list_count2 = list2.size();
      params.put("OSMCustomSpeedLimitT", targetvalue2.toStdString());
      refresh();
    } else {
      QStringList list2 = QString::fromStdString(params.get("OSMCustomSpeedLimitT")).split(",");
      list_count2 = list2.size();
    }
    if (list_count1 != list_count2) {
      ConfirmationDialog::alert("Index count does not match. Check your input again.", this);
    }
  });
  refresh();
}

void OSMCustomSpeedLimit::refresh() {
  auto strs1 = QString::fromStdString(params.get("OSMCustomSpeedLimitC"));
  auto strs2 = QString::fromStdString(params.get("OSMCustomSpeedLimitT"));
  edit1.setText(QString::fromStdString(strs1.toStdString()));
  edit2.setText(QString::fromStdString(strs2.toStdString()));
  btn.setText("EDIT");
}

DesiredCurvatureLimit::DesiredCurvatureLimit() : AbstractControl("DesiredCurvatureLimit", "Adjust DisiredCurvatureLimit, Default is 0.05(DT_MDL), For HKG, maybe 0.2 is preferred from user's experience. If the steering digs into inside on intersection, upper the value. And then it will limit your scope of steering angle. In case of opposite situation, lower the value. this is multiplier of desired curvature rate not real limit value.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btndigit.setStyleSheet(R"(
    QPushButton {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    }
    QPushButton:pressed {
      background-color: #ababab;
    }
  )");
  btnminus.setStyleSheet(R"(
    QPushButton {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    }
    QPushButton:pressed {
      background-color: #ababab;
    }
  )");
  btnplus.setStyleSheet(R"(
    QPushButton {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    }
    QPushButton:pressed {
      background-color: #ababab;
    }
  )");
  btndigit.setFixedSize(100, 100);
  btnminus.setFixedSize(100, 100);
  btnplus.setFixedSize(100, 100);
  hlayout->addWidget(&btndigit);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);
  btndigit.setText("0.01");
  btnminus.setText("-");
  btnplus.setText("+");

  QObject::connect(&btndigit, &QPushButton::clicked, [=]() {
    digit = digit * 10;
    if (digit >= 2 ) {
      digit = 0.01;
    }
    QString level = QString::number(digit);
    btndigit.setText(level);
  });

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("DesiredCurvatureLimit"));
    int value = str.toInt();
    value = value - (digit*100);
    if (value <= 5) {
      value = 5;
    }
    QString values = QString::number(value);
    params.put("DesiredCurvatureLimit", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("DesiredCurvatureLimit"));
    int value = str.toInt();
    value = value + (digit*100);
    if (value >= 1000) {
      value = 1000;
    }
    QString values = QString::number(value);
    params.put("DesiredCurvatureLimit", values.toStdString());
    refresh();
  });
  refresh();
}

void DesiredCurvatureLimit::refresh() {
  auto strs = QString::fromStdString(params.get("DesiredCurvatureLimit"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  label.setText("＊ " + QString::fromStdString(valuefs.toStdString()));
}

DynamicTRUD::DynamicTRUD() : AbstractControl("DynamicTR: [Speed] [TRs]", "Set TR of each speeds. (Mid range is interpolation values)", "../assets/offroad/icon_shell.png") {
}

DynamicTRBySpeed::DynamicTRBySpeed() : AbstractControl("", "", "") {
  btn.setStyleSheet(R"(
    padding: -10;
    border-radius: 35px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  edit1.setStyleSheet(R"(
    background-color: grey;
    font-size: 55px;
    font-weight: 500;
    height: 120px;
  )");
  edit2.setStyleSheet(R"(
    background-color: grey;
    font-size: 55px;
    font-weight: 500;
    height: 120px;
  )");
  btn.setFixedSize(150, 100);
  edit1.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  edit2.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  hlayout->addWidget(&edit1);
  hlayout->addWidget(&edit2);
  hlayout->addWidget(&btn);

  QObject::connect(&btn, &QPushButton::clicked, [=]() {
    int list_count1 = 0;
    int list_count2 = 0;
    QString targetvalue1 = InputDialog::getText("Set Speed values with comma", this, "ex) 0,20,40,60,110", false, 1, QString::fromStdString(params.get("DynamicTRSpd")));
    if (targetvalue1.length() > 0 && targetvalue1 != QString::fromStdString(params.get("DynamicTRSpd"))) {
      QStringList list1 = targetvalue1.split(",");
      list_count1 = list1.size();
      params.put("DynamicTRSpd", targetvalue1.toStdString());
      refresh();
    } else {
      QStringList list1 = QString::fromStdString(params.get("DynamicTRSpd")).split(",");
      list_count1 = list1.size();
    }
    QString targetvalue2 = InputDialog::getText("Set TR values with comma", this, "ex) 1.2,1.3,1.4,1.5,1.6", false, 1, QString::fromStdString(params.get("DynamicTRSet")));
    if (targetvalue2.length() > 0 && targetvalue2 != QString::fromStdString(params.get("DynamicTRSet"))) {
      QStringList list2 = targetvalue2.split(",");
      list_count2 = list2.size();
      params.put("DynamicTRSet", targetvalue2.toStdString());
      refresh();
    } else {
      QStringList list2 = QString::fromStdString(params.get("DynamicTRSet")).split(",");
      list_count2 = list2.size();
    }
    if (list_count1 != list_count2) {
      ConfirmationDialog::alert("Index count does not match. Check your input again.", this);
    }
  });
  refresh();
}

void DynamicTRBySpeed::refresh() {
  auto strs1 = QString::fromStdString(params.get("DynamicTRSpd"));
  auto strs2 = QString::fromStdString(params.get("DynamicTRSet"));
  edit1.setText(QString::fromStdString(strs1.toStdString()));
  edit2.setText(QString::fromStdString(strs2.toStdString()));
  btn.setText("EDIT");
}

LaneWidth::LaneWidth() : AbstractControl("Set LaneWidth", "Set LaneWidth (default:3.7)", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LaneWidth"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 23 ) {
      value = 23;
    }
    QString values = QString::number(value);
    params.put("LaneWidth", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LaneWidth"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 40 ) {
      value = 40;
    }
    QString values = QString::number(value);
    params.put("LaneWidth", values.toStdString());
    refresh();
  });
  refresh();
}

void LaneWidth::refresh() {
  auto strs = QString::fromStdString(params.get("LaneWidth"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.1;
  QString valuefs = QString::number(valuef);
  label.setText(QString::fromStdString(valuefs.toStdString()));
  btnminus.setText("－");
  btnplus.setText("＋");
}

SpeedLaneWidthUD::SpeedLaneWidthUD() : AbstractControl("Speed LaneWidth: [Spd(m/s)] [LaneWidth]", "Set LaneWidths by speed. Speed is m/s values not kph or mph. (Mid range is interpolation values)", "../assets/offroad/icon_shell.png") {
}

SpeedLaneWidth::SpeedLaneWidth() : AbstractControl("", "", "") {
  btn.setStyleSheet(R"(
    padding: -10;
    border-radius: 35px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  edit1.setStyleSheet(R"(
    background-color: grey;
    font-size: 55px;
    font-weight: 500;
    height: 120px;
  )");
  edit2.setStyleSheet(R"(
    background-color: grey;
    font-size: 55px;
    font-weight: 500;
    height: 120px;
  )");
  btn.setFixedSize(150, 100);
  edit1.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  edit2.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  hlayout->addWidget(&edit1);
  hlayout->addWidget(&edit2);
  hlayout->addWidget(&btn);

  QObject::connect(&btn, &QPushButton::clicked, [=]() {
    int list_count1 = 0;
    int list_count2 = 0;
    QString targetvalue1 = InputDialog::getText("Set Speed(m/s) values with comma", this, "ex) 0,31", false, 1, QString::fromStdString(params.get("SpdLaneWidthSpd")));
    if (targetvalue1.length() > 0 && targetvalue1 != QString::fromStdString(params.get("SpdLaneWidthSpd"))) {
      QStringList list1 = targetvalue1.split(",");
      list_count1 = list1.size();
      params.put("SpdLaneWidthSpd", targetvalue1.toStdString());
      refresh();
    } else {
      QStringList list1 = QString::fromStdString(params.get("SpdLaneWidthSpd")).split(",");
      list_count1 = list1.size();
    }
    QString targetvalue2 = InputDialog::getText("Set LW(m) values with comma", this, "ex) 2.8,3.5", false, 1, QString::fromStdString(params.get("SpdLaneWidthSet")));
    if (targetvalue2.length() > 0 && targetvalue2 != QString::fromStdString(params.get("SpdLaneWidthSet"))) {
      QStringList list2 = targetvalue2.split(",");
      list_count2 = list2.size();
      params.put("SpdLaneWidthSet", targetvalue2.toStdString());
      refresh();
    } else {
      QStringList list2 = QString::fromStdString(params.get("SpdLaneWidthSet")).split(",");
      list_count2 = list2.size();
    }
    if (list_count1 != list_count2) {
      ConfirmationDialog::alert("Index count does not match. Check your input again.", this);
    }
  });
  refresh();
}

void SpeedLaneWidth::refresh() {
  auto strs1 = QString::fromStdString(params.get("SpdLaneWidthSpd"));
  auto strs2 = QString::fromStdString(params.get("SpdLaneWidthSet"));
  edit1.setText(QString::fromStdString(strs1.toStdString()));
  edit2.setText(QString::fromStdString(strs2.toStdString()));
  btn.setText("EDIT");
}

OPKRTopTextView::OPKRTopTextView() : AbstractControl("Top Text View", "Date/Time/OSM Street Name", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("TopTextView"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -1 ) {
      value = 7;
    }
    QString values = QString::number(value);
    params.put("TopTextView", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("TopTextView"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 8 ) {
      value = 0;
    }
    QString values = QString::number(value);
    params.put("TopTextView", values.toStdString());
    refresh();
  });
  refresh();
}

void OPKRTopTextView::refresh() {
  QString option = QString::fromStdString(params.get("TopTextView"));
  if (option == "0") {
    label.setText(QString::fromStdString("None"));
    QUIState::ui_state.scene.top_text_view = 0;
  } else if (option == "1") {
    label.setText(QString::fromStdString("Date+Time"));
    QUIState::ui_state.scene.top_text_view = 1;
  } else if (option == "2") {
    label.setText(QString::fromStdString("Date"));
    QUIState::ui_state.scene.top_text_view = 2;
  } else if (option == "3") {
    label.setText(QString::fromStdString("Time"));
    QUIState::ui_state.scene.top_text_view = 3;
  } else if (option == "4") {
    label.setText(QString::fromStdString("Date+Time+OSM"));
    QUIState::ui_state.scene.top_text_view = 4;
  } else if (option == "5") {
    label.setText(QString::fromStdString("Date+OSM"));
    QUIState::ui_state.scene.top_text_view = 5;
  } else if (option == "6") {
    label.setText(QString::fromStdString("Time+OSM"));
    QUIState::ui_state.scene.top_text_view = 6;
  } else {
    label.setText(QString::fromStdString("OSM"));
    QUIState::ui_state.scene.top_text_view = 7;
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

OPKREdgeOffset::OPKREdgeOffset() : AbstractControl("", "+ value to move car to left, - value to move car to right on each lane.", "") {

  labell1.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  labell1.setText("LeftEdge: ");
  hlayout->addWidget(&labell1);
  labell.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  labell.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&labell);
  btnminusl.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplusl.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminusl.setFixedSize(80, 100);
  btnplusl.setFixedSize(80, 100);
  hlayout->addWidget(&btnminusl);
  hlayout->addWidget(&btnplusl);

  labelr1.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  labelr1.setText("RigtEdge: ");
  hlayout->addWidget(&labelr1);
  labelr.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  labelr.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&labelr);
  btnminusr.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplusr.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminusr.setFixedSize(80, 100);
  btnplusr.setFixedSize(80, 100);
  hlayout->addWidget(&btnminusr);
  hlayout->addWidget(&btnplusr);

  btnminusl.setText("－");
  btnplusl.setText("＋");
  btnminusr.setText("－");
  btnplusr.setText("＋");

  QObject::connect(&btnminusl, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LeftEdgeOffset"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -50 ) {
      value = -50;
    }
    QString values = QString::number(value);
    params.put("LeftEdgeOffset", values.toStdString());
    refreshl();
  });
  
  QObject::connect(&btnplusl, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("LeftEdgeOffset"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 50 ) {
      value = 50;
    }
    QString values = QString::number(value);
    params.put("LeftEdgeOffset", values.toStdString());
    refreshl();
  });
  QObject::connect(&btnminusr, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("RightEdgeOffset"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -50 ) {
      value = -50;
    }
    QString values = QString::number(value);
    params.put("RightEdgeOffset", values.toStdString());
    refreshr();
  });
  
  QObject::connect(&btnplusr, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("RightEdgeOffset"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 50 ) {
      value = 50;
    }
    QString values = QString::number(value);
    params.put("RightEdgeOffset", values.toStdString());
    refreshr();
  });
  refreshl();
  refreshr();
}

void OPKREdgeOffset::refreshl() {
  auto strs = QString::fromStdString(params.get("LeftEdgeOffset"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  labell.setText(QString::fromStdString(valuefs.toStdString()));
}

void OPKREdgeOffset::refreshr() {
  auto strs = QString::fromStdString(params.get("RightEdgeOffset"));
  int valuei = strs.toInt();
  float valuef = valuei * 0.01;
  QString valuefs = QString::number(valuef);
  labelr.setText(QString::fromStdString(valuefs.toStdString()));
}
