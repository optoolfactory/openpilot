#include "selfdrive/ui/qt/offroad/settings.h"

#include <cassert>
#include <cmath>
#include <string>

#include <QDebug>
#include <QProcess> // opkr
#include <QDateTime> // opkr
#include <QTimer> // opkr

#ifndef QCOM
#include "selfdrive/ui/qt/offroad/networking.h"
#endif

#ifdef ENABLE_MAPS
#include "selfdrive/ui/qt/maps/map_settings.h"
#endif

#include "selfdrive/common/params.h"
#include "selfdrive/common/util.h"
#include "selfdrive/hardware/hw.h"
#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/qt/widgets/input.h"
#include "selfdrive/ui/qt/widgets/scrollview.h"
#include "selfdrive/ui/qt/widgets/ssh_keys.h"
#include "selfdrive/ui/qt/widgets/toggle.h"
#include "selfdrive/ui/ui.h"
#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/qt_window.h"
#include "selfdrive/ui/qt/widgets/opkr.h"

TogglesPanel::TogglesPanel(SettingsWindow *parent) : ListWidget(parent) {
  // param, title, desc, icon
  std::vector<std::tuple<QString, QString, QString, QString>> toggles{
    {
      "OpenpilotEnabledToggle",
      "오픈파일럿 사용",
      "어댑티브 크루즈 컨트롤 및 차선 유지 지원을 위해 오픈파일럿 시스템을 사용하십시오. 이 기능을 사용하려면 항상 주의를 기울여야 합니다. 이 설정을 변경하는 것은 자동차의 전원이 꺼졌을 때 적용됩니다.",
      "../assets/offroad/icon_openpilot.png",
    },
    {
      "IsLdwEnabled",
      "차선이탈 경보 사용",
      "50km/h이상의 속도로 주행하는 동안 방향 지시등이 활성화되지 않은 상태에서 차량이 감지된 차선 위를 넘어갈 경우 원래 차선으로 다시 방향을 전환하도록 경고를 보냅니다.",
      "../assets/offroad/icon_warning.png",
    },
    {
      "IsRHD",
      "우핸들 운전방식 사용",
      "오픈파일럿이 좌측 교통 규칙을 준수하도록 허용하고 우측 운전석에서 운전자 모니터링을 수행하십시오.",
      "../assets/offroad/icon_openpilot_mirrored.png",
    },
    {
      "IsMetric",
      "미터법 사용",
      "mi/h 대신 km/h 단위로 속도를 표시합니다.",
      "../assets/offroad/icon_metric.png",
    },
    {
      "UploadRaw",
      "주행 로그 업로드",
      "업로드 프로세스 활성화 시 모든 로그 및 풀 해상도 비디오를 업로드합니다.(WiFi 사용중에만 작동) 기능이 꺼진 경우, my.comma.ai/useradmin에 업로드를 위해 개별 로그는 기록될 수 있습니다.",
      "../assets/offroad/icon_network.png",
    },
    {
      "RecordFront",
      "운전자 영상 녹화 및 업로드",
      "운전자 모니터링 카메라에서 데이터를 업로드하고 운전자 모니터링 알고리즘을 개선하십시오.",
      "../assets/offroad/icon_monitoring.png",
    },
    {
      "EndToEndToggle",
      "\U0001f96c 차선 선택 모드 \U0001f96c",
      "화면에서 차선과 차선 비활성화 모드를 선택할수 있습니다.",
      "../assets/offroad/icon_road.png",
    },
#ifdef ENABLE_MAPS
    {
      "NavSettingTime24h",
      "네비게이션 시간 설정",
      "AM/PM 대신 24시간 설정으로 변경",
      "../assets/offroad/icon_metric.png",
    },
#endif

    {
      "OpkrEnableDriverMonitoring",
      "운전자 모니터링 사용",
      "운전자 감시 모니터링을 사용합니다.",
      "../assets/offroad/icon_shell.png",
    },
    {
      "OpkrEnableLogger",
      "주행로그 기록 사용",
      "로컬에서 데이터 분석을 위해 주행로그를 기록합니다. 로거만 활성화 되며 서버로 업로드 되지 않습니다.",
      "../assets/offroad/icon_shell.png",
    },
    {
      "OpkrEnableUploader",
      "주행로그 서버 전송",
      "시스템로그 및 기타 주행데이터를 서버로 전송하기 위해 업로드 프로세스를 활성화 합니다. 오프로드 상태에서만 업로드 합니다.",
      "../assets/offroad/icon_shell.png",
    },
    {
      "CommaStockUI",
      "Comma 기본 UI 사용",
      "주행화면을 콤마의 순정 UI를 사용합니다. 주행화면 좌측상단의 박스를 눌러도 실시간 전환 가능합니다.",
      "../assets/offroad/icon_shell.png",
    },
  };

  Params params;

  if (params.getBool("DisableRadar_Allow")) {
    toggles.push_back({
      "DisableRadar",
      "오픈파일럿 가감속 컨트롤",
      "openpilot은 차량의 레이더를 비활성화하고 엑셀레이터와 브레이크를 제어합니다. 경고: 이것은 AEB를 비활성화합니다! ",
      "../assets/offroad/icon_speed_limit.png",
    });
  }

  for (auto &[param, title, desc, icon] : toggles) {
    auto toggle = new ParamControl(param, title, desc, icon, this);
    //bool locked = params.getBool((param + "Lock").toStdString());
    //toggle->setEnabled(true);
    //connect(parent, &SettingsWindow::offroadTransition, toggle, &ParamControl::setEnabled);
    addItem(toggle);
  }
}

DevicePanel::DevicePanel(SettingsWindow *parent) : ListWidget(parent) {
  setSpacing(50);
  addItem(new LabelControl("Dongle ID", getDongleId().value_or("N/A")));
  addItem(new LabelControl("Serial", params.get("HardwareSerial").c_str()));

  addItem(new OpenpilotView());

  // offroad-only buttons

  auto dcamBtn = new ButtonControl("운전자 영상", "미리보기",
                                   "운전자 모니터링 카메라를 미리 보고 장치 장착 위치를 최적화하여 최상의 운전자 모니터링 환경을 제공하십시오. (차량이 꺼져 있어야 합니다.)");
  connect(dcamBtn, &ButtonControl::clicked, [=]() { emit showDriverView(); });
  addItem(dcamBtn);

  if (!params.getBool("Passive")) {
    auto retrainingBtn = new ButtonControl("트레이닝가이드 보기", "다시보기", "오픈파일럿에 대한 규칙, 기능, 제한내용 등을 확인하세요.");
    connect(retrainingBtn, &ButtonControl::clicked, [=]() {
      if (ConfirmationDialog::confirm("트레이닝 가이드를 다시 확인하시겠습니까?", this)) {
        emit reviewTrainingGuide();
      }
    });
    addItem(retrainingBtn);
  }

  if (Hardware::TICI()) {
    auto regulatoryBtn = new ButtonControl("Regulatory", "VIEW", "");
    connect(regulatoryBtn, &ButtonControl::clicked, [=]() {
      const std::string txt = util::read_file("../assets/offroad/fcc.html");
      RichTextDialog::alert(QString::fromStdString(txt), this);
    });
    addItem(regulatoryBtn);
  }

  auto resetCalibBtn = new ButtonControl("캘리브레이션 초기화", "실행", " ");
  connect(resetCalibBtn, &ButtonControl::showDescription, this, &DevicePanel::updateCalibDescription);
  connect(resetCalibBtn, &ButtonControl::clicked, [&]() {
    if (ConfirmationDialog::confirm("캘리브레이션을 초기화 하시겠습니까? 자동 재부팅됩니다.", this)) {
      params.remove("CalibrationParams");
      params.remove("LiveParameters");
      QTimer::singleShot(1000, []() {
        Hardware::reboot();
      });
    }
  });
  addItem(resetCalibBtn);

  QObject::connect(parent, &SettingsWindow::offroadTransition, [=](bool offroad) {
    for (auto btn : findChildren<ButtonControl *>()) {
      btn->setEnabled(offroad);
    }
    resetCalibBtn->setEnabled(true);
  });

  // power buttons
  QHBoxLayout *power_layout = new QHBoxLayout();
  power_layout->setSpacing(30);

  QPushButton *reboot_btn = new QPushButton("재시작");
  reboot_btn->setObjectName("reboot_btn");
  power_layout->addWidget(reboot_btn);
  QObject::connect(reboot_btn, &QPushButton::clicked, this, &DevicePanel::reboot);

  QPushButton *poweroff_btn = new QPushButton("전원 종료");
  poweroff_btn->setObjectName("poweroff_btn");
  power_layout->addWidget(poweroff_btn);
  QObject::connect(poweroff_btn, &QPushButton::clicked, this, &DevicePanel::poweroff);

  setStyleSheet(R"(
    QPushButton {
      height: 120px;
      border-radius: 15px;
    }
    #reboot_btn { background-color: #393939; }
    #reboot_btn:pressed { background-color: #4a4a4a; }
    #poweroff_btn { background-color: #E22C2C; }
    #poweroff_btn:pressed { background-color: #FF2424; }
  )");
  addItem(power_layout);
}

void DevicePanel::updateCalibDescription() {
  QString desc =
      "openpilot requires the device to be mounted within 4° left or right and "
      "within 5° up or down. openpilot is continuously calibrating, resetting is rarely required.";
  std::string calib_bytes = Params().get("CalibrationParams");
  if (!calib_bytes.empty()) {
    try {
      AlignedBuffer aligned_buf;
      capnp::FlatArrayMessageReader cmsg(aligned_buf.align(calib_bytes.data(), calib_bytes.size()));
      auto calib = cmsg.getRoot<cereal::Event>().getLiveCalibration();
      if (calib.getCalStatus() != 0) {
        double pitch = calib.getRpyCalib()[1] * (180 / M_PI);
        double yaw = calib.getRpyCalib()[2] * (180 / M_PI);
        desc += QString(" \n장치가 %1° %2 그리고 %3° %4 위치해 있습니다.")
                    .arg(QString::number(std::abs(pitch), 'g', 1), pitch > 0 ? "위로" : "아래로",
                         QString::number(std::abs(yaw), 'g', 1), yaw > 0 ? "오른쪽으로" : "왼쪽으로");
      }
    } catch (kj::Exception) {
      qInfo() << "invalid CalibrationParams";
    }
  }
  qobject_cast<ButtonControl *>(sender())->setDescription(desc);
}

void DevicePanel::reboot() {
  if (QUIState::ui_state.status == UIStatus::STATUS_DISENGAGED) {
    if (ConfirmationDialog::confirm("재시작 하시겠습니까?", this)) {
      // Check engaged again in case it changed while the dialog was open
      if (QUIState::ui_state.status == UIStatus::STATUS_DISENGAGED) {
        Params().putBool("DoReboot", true);
      }
    }
  } else {
    ConfirmationDialog::alert("Disengage to Reboot", this);
  }
}

void DevicePanel::poweroff() {
  if (QUIState::ui_state.status == UIStatus::STATUS_DISENGAGED) {
    if (ConfirmationDialog::confirm("전원을 끄시겠습니까", this)) {
      // Check engaged again in case it changed while the dialog was open
      if (QUIState::ui_state.status == UIStatus::STATUS_DISENGAGED) {
        Params().putBool("DoShutdown", true);
      }
    }
  } else {
    ConfirmationDialog::alert("Disengage to Power Off", this);
  }
}

SoftwarePanel::SoftwarePanel(QWidget* parent) : ListWidget(parent) {
  gitRemoteLbl = new LabelControl("Git Remote");
  gitBranchLbl = new LabelControl("Git Branch");
  gitCommitLbl = new LabelControl("Git Commit");
  osVersionLbl = new LabelControl("OS Version");
  versionLbl = new LabelControl("Version");
  lastUpdateLbl = new LabelControl("최근업데이트 확인", "", "");
  updateBtn = new ButtonControl("업데이트 체크 및 적용", "");
  connect(updateBtn, &ButtonControl::clicked, [=]() {
    if (params.getBool("IsOffroad")) {
      fs_watch->addPath(QString::fromStdString(params.getParamPath("LastUpdateTime")));
      fs_watch->addPath(QString::fromStdString(params.getParamPath("UpdateFailedCount")));
    }
    std::system("/data/openpilot/selfdrive/assets/addon/script/gitcommit.sh");
    std::system("date '+%F %T' > /data/params/d/LastUpdateTime");
    QString last_ping = QString::fromStdString(params.get("LastAthenaPingTime"));
    QString desc = "";
    QString commit_local = QString::fromStdString(Params().get("GitCommit").substr(0, 10));
    QString commit_remote = QString::fromStdString(Params().get("GitCommitRemote").substr(0, 10));
    QString empty = "";
    desc += QString("로    컬: %1\n리모트: %2%3%4\n").arg(commit_local, commit_remote, empty, empty);
    
    if (!last_ping.length()) {
      desc += QString("인터넷에 연결되어 있지 않습니다. 업데이트확인을 위해 WiFi를 연결하세요.");
    } else if (commit_local == commit_remote) {
      desc += QString("로컬과 리모트가 일치합니다. 업데이트가 필요 없습니다.");
    } else {
      desc += QString("업데이트가 있습니다. 적용하려면 확인버튼을 누르세요.");
    }
    if (ConfirmationDialog::confirm(desc, this)) {
      std::system("/data/openpilot/selfdrive/assets/addon/script/gitpull.sh");
    }
  });


  auto uninstallBtn = new ButtonControl("Uninstall " + getBrand(), "제거");
  connect(uninstallBtn, &ButtonControl::clicked, [&]() {
    if (ConfirmationDialog::confirm("제거 하시겠습니까?", this)) {
      params.putBool("DoUninstall", true);
    }
  });
  connect(parent, SIGNAL(offroadTransition(bool)), uninstallBtn, SLOT(setEnabled(bool)));

  QWidget *widgets[] = {osVersionLbl, versionLbl, gitRemoteLbl, gitBranchLbl, lastUpdateLbl, updateBtn};
  for (QWidget* w : widgets) {
    addItem(w);
  }

  addItem(new GitHash());
  addItem(new GitPullOnBootToggle());

  // preset1 buttons
  QHBoxLayout *presetone_layout = new QHBoxLayout();
  presetone_layout->setSpacing(50);

  QPushButton *presetoneload_btn = new QPushButton("프리셋1 불러오기");
  presetoneload_btn->setStyleSheet("height: 120px;border-radius: 15px;background-color: #393939;");
  presetone_layout->addWidget(presetoneload_btn);
  QObject::connect(presetoneload_btn, &QPushButton::clicked, [=]() {
    if (ConfirmationDialog::confirm("프리셋1을 불러올까요?", this)) {
      QProcess::execute("/data/openpilot/selfdrive/assets/addon/script/load_preset1.sh");
    }
  });

  QPushButton *presetonesave_btn = new QPushButton("프리셋1 저장하기");
  presetonesave_btn->setStyleSheet("height: 120px;border-radius: 15px;background-color: #393939;");
  presetone_layout->addWidget(presetonesave_btn);
  QObject::connect(presetonesave_btn, &QPushButton::clicked, [=]() {
    if (ConfirmationDialog::confirm("프리셋1을 저장할까요?", this)) {
      QProcess::execute("/data/openpilot/selfdrive/assets/addon/script/save_preset1.sh");
    }
  });

  // preset2 buttons
  QHBoxLayout *presettwo_layout = new QHBoxLayout();
  presettwo_layout->setSpacing(50);

  QPushButton *presettwoload_btn = new QPushButton("프리셋2 불러오기");
  presettwoload_btn->setStyleSheet("height: 120px;border-radius: 15px;background-color: #393939;");
  presettwo_layout->addWidget(presettwoload_btn);
  QObject::connect(presettwoload_btn, &QPushButton::clicked, [=]() {
    if (ConfirmationDialog::confirm("프리셋2을 불러올까요?", this)) {
      QProcess::execute("/data/openpilot/selfdrive/assets/addon/script/load_preset2.sh");
    }
  });

  QPushButton *presettwosave_btn = new QPushButton("프리셋2 저장하기");
  presettwosave_btn->setStyleSheet("height: 120px;border-radius: 15px;background-color: #393939;");
  presettwo_layout->addWidget(presettwosave_btn);
  QObject::connect(presettwosave_btn, &QPushButton::clicked, [=]() {
    if (ConfirmationDialog::confirm("프리셋2을 저장할까요?", this)) {
      QProcess::execute("/data/openpilot/selfdrive/assets/addon/script/save_preset2.sh");
    }
  });

  auto paraminit_btn = new ButtonControl("파라미터 초기화", "초기화");
  QObject::connect(paraminit_btn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("파라미터를 초기화 합니다. 이온 메뉴의 각종 변경값들이 최초 설정된 값으로 바뀝니다. 진행하시겠습니까?", this)){
      QProcess::execute("/data/openpilot/selfdrive/assets/addon/script/init_param.sh");
    }
  });

  addItem(presetone_layout);
  addItem(presettwo_layout);

  addItem(paraminit_btn);

  const char* git_reset = "/data/openpilot/selfdrive/assets/addon/script/git_reset.sh ''";
  auto gitresetbtn = new ButtonControl("Git Reset", "실행");
  QObject::connect(gitresetbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("로컬변경사항을 강제 초기화 후 리모트Git의 최신 커밋내역을 적용합니다. 진행하시겠습니까?", this)){
      std::system(git_reset);
    }
  });
  addItem(gitresetbtn);

  const char* gitpull_cancel = "/data/openpilot/selfdrive/assets/addon/script/gitpull_cancel.sh ''";
  auto gitpullcanceltbtn = new ButtonControl("Git Pull 취소", "실행");
  QObject::connect(gitpullcanceltbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("GitPull 이전 상태로 되돌립니다. 진행하시겠습니까?", this)){
      std::system(gitpull_cancel);
    }
  });
  addItem(gitpullcanceltbtn);

  const char* panda_flashing = "/data/openpilot/selfdrive/assets/addon/script/panda_flashing.sh ''";
  auto pandaflashingtbtn = new ButtonControl("판다 플래싱", "실행");
  QObject::connect(pandaflashingtbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("Panda's green LED blinks quickly during panda flashing. Never turn off or disconnect the device arbitrarily. Do you want to proceed?", this)) {
      std::system(panda_flashing);
    }
  });

  addItem(pandaflashingtbtn);
  addItem(new SwitchOpenpilot()); // opkr
  addItem(new BranchSelectCombo()); // opkr

  const char* open_settings = "am start -a android.intent.action.MAIN -n com.android.settings/.Settings";
  auto open_settings_btn = new ButtonControl("안드로이드 설정", "실행");
  QObject::connect(open_settings_btn, &ButtonControl::clicked, [=]() {
    emit closeSettings();
    std::system(open_settings);
  });
  addItem(open_settings_btn);
  const char* softkey = "am start com.gmd.hidesoftkeys/com.gmd.hidesoftkeys.MainActivity";
  auto softkey_btn = new ButtonControl("소프트키 실행/설정", "실행");
  QObject::connect(softkey_btn, &ButtonControl::clicked, [=]() {
    emit closeSettings();
    std::system(softkey);
  });
  addItem(softkey_btn);
  auto mixplorer_btn = new ButtonControl("Mixplorer 실행", "실행");
  QObject::connect(mixplorer_btn, &ButtonControl::clicked, [=]() {
	  emit closeSettings();
    std::system("/data/openpilot/selfdrive/assets/addon/script/run_mixplorer.sh");
  });
  addItem(mixplorer_btn);
  addItem(uninstallBtn);
  fs_watch = new QFileSystemWatcher(this);
  QObject::connect(fs_watch, &QFileSystemWatcher::fileChanged, [=](const QString path) {
    if (path.contains("UpdateFailedCount") && std::atoi(params.get("UpdateFailedCount").c_str()) > 0) {
      lastUpdateLbl->setText("failed to fetch update");
      updateBtn->setText("CHECK");
      updateBtn->setEnabled(true);
    } else if (path.contains("LastUpdateTime")) {
      updateLabels();
    }
  });
}

void SoftwarePanel::showEvent(QShowEvent *event) {
  updateLabels();
}

void SoftwarePanel::updateLabels() {
  QString lastUpdate = "";
  QString tm = QString::fromStdString(params.get("LastUpdateTime").substr(0, 19));
  if (tm != "") {
    lastUpdate = timeAgo(QDateTime::fromString(tm, "yyyy-MM-dd HH:mm:ss"));
  }

  versionLbl->setText(getBrandVersion());
  lastUpdateLbl->setText(lastUpdate);
  updateBtn->setText("CHECK");
  updateBtn->setEnabled(true);
  gitRemoteLbl->setText(QString::fromStdString(params.get("GitRemote").substr(19)));
  gitBranchLbl->setText(QString::fromStdString(params.get("GitBranch")));
  gitCommitLbl->setText(QString::fromStdString(params.get("GitCommit")).left(10));
  osVersionLbl->setText(QString::fromStdString(Hardware::get_os_version()).trimmed());
}

C2NetworkPanel::C2NetworkPanel(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(50, 0, 50, 0);

  ListWidget *list = new ListWidget();
  list->setSpacing(30);
  // wifi + tethering buttons
#ifdef QCOM
  auto wifiBtn = new ButtonControl("Wi-Fi 설정", "열기");
  QObject::connect(wifiBtn, &ButtonControl::clicked, [=]() { HardwareEon::launch_wifi(); });
  list->addItem(wifiBtn);

  auto tetheringBtn = new ButtonControl("테더링 설정", "열기");
  QObject::connect(tetheringBtn, &ButtonControl::clicked, [=]() { HardwareEon::launch_tethering(); });
  list->addItem(tetheringBtn);
#endif
  ipaddress = new LabelControl("IP Address", "");
  list->addItem(ipaddress);

  list->addItem(new HotspotOnBootToggle());

  // SSH key management
  list->addItem(new SshToggle());
  list->addItem(new SshControl());
  list->addItem(new SshLegacyToggle());

  layout->addWidget(list);
  layout->addStretch(1);
}

void C2NetworkPanel::showEvent(QShowEvent *event) {
  ipaddress->setText(getIPAddress());
}

QString C2NetworkPanel::getIPAddress() {
  std::string result = util::check_output("ifconfig wlan0");
  if (result.empty()) return "";

  const std::string inetaddrr = "inet addr:";
  std::string::size_type begin = result.find(inetaddrr);
  if (begin == std::string::npos) return "";

  begin += inetaddrr.length();
  std::string::size_type end = result.find(' ', begin);
  if (end == std::string::npos) return "";

  return result.substr(begin, end - begin).c_str();
}

QWidget *network_panel(QWidget *parent) {
#ifdef QCOM
  return new C2NetworkPanel(parent);
#else
  return new Networking(parent);
#endif
}

UIPanel::UIPanel(QWidget *parent) : QFrame(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  layout->setContentsMargins(50, 0, 50, 0);
  layout->setSpacing(30);

  // OPKR
  layout->addWidget(new AutoShutdown());
  layout->addWidget(new ForceShutdown());
  layout->addWidget(new VolumeControl());
  layout->addWidget(new BrightnessControl());
  layout->addWidget(new AutoScreenOff());
  layout->addWidget(new BrightnessOffControl());
  layout->addWidget(new GetOffAlert());
  layout->addWidget(new BatteryChargingControlToggle());
  layout->addWidget(new ChargingMin());
  layout->addWidget(new ChargingMax());
  layout->addWidget(new DrivingRecordToggle());
  layout->addWidget(new RecordCount());
  layout->addWidget(new RecordQuality());
  const char* record_del = "rm -f /storage/emulated/0/videos/*";
  auto recorddelbtn = new ButtonControl("녹화파일 전부 삭제", "실행");
  QObject::connect(recorddelbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("저장된 녹화파일을 모두 삭제합니다. 진행하시겠습니까?", this)){
      std::system(record_del);
    }
  });
  layout->addWidget(recorddelbtn);
  const char* realdata_del = "rm -rf /storage/emulated/0/realdata/*";
  auto realdatadelbtn = new ButtonControl("주행로그 전부 삭제", "실행");
  QObject::connect(realdatadelbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("저장된 주행로그를 모두 삭제합니다. 진행하시겠습니까?", this)){
      std::system(realdata_del);
    }
  });
  layout->addWidget(realdatadelbtn);
  layout->addWidget(new MonitoringMode());
  layout->addWidget(new MonitorEyesThreshold());
  layout->addWidget(new NormalEyesThreshold());
  layout->addWidget(new BlinkThreshold());
  layout->addWidget(new OPKRNaviSelect());
  layout->addWidget(new RunNaviOnBootToggle());
  layout->addWidget(new OPKRServerSelect());
  layout->addWidget(new OPKRServerAPI());
  layout->addWidget(new MapboxEnabledToggle());
  layout->addWidget(new OPKRMapboxStyle());
  layout->addWidget(new GoogleMapEnabledToggle());
  layout->addWidget(new OPKRTopTextView());
  layout->addWidget(new RPMAnimatedToggle());
  layout->addWidget(new RPMAnimatedMaxValue());
  layout->addWidget(new ShowStopLineToggle());
}

DrivingPanel::DrivingPanel(QWidget *parent) : QFrame(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  layout->setContentsMargins(50, 0, 50, 0);
  layout->setSpacing(30);

  // OPKR
  layout->addWidget(new AutoResumeToggle());
  layout->addWidget(new RESCountatStandstill());
  layout->addWidget(new CruiseGapAdjustToggle());
  layout->addWidget(new StandstillResumeAltToggle());
  layout->addWidget(new VariableCruiseToggle());
  layout->addWidget(new CruisemodeSelInit());
  layout->addWidget(new LaneChangeSpeed());
  layout->addWidget(new LaneChangeDelay());
  layout->addWidget(new LCTimingFactorUD());
  layout->addWidget(new LCTimingFactor());
  layout->addWidget(new LeftCurvOffset());
  layout->addWidget(new RightCurvOffset());
  layout->addWidget(new BlindSpotDetectToggle());
  layout->addWidget(new MaxAngleLimit());
  layout->addWidget(new SteerAngleCorrection());
  layout->addWidget(new TurnSteeringDisableToggle());
  layout->addWidget(new CruiseOverMaxSpeedToggle());
  layout->addWidget(new OSMEnabledToggle());
  layout->addWidget(new OSMSpeedLimitEnabledToggle());
  layout->addWidget(new StockNaviSpeedToggle());
  layout->addWidget(new SpeedLimitOffset());
  layout->addWidget(new OSMCustomSpeedLimitUD());
  layout->addWidget(new OSMCustomSpeedLimit());
  layout->addWidget(new SpeedLimitSignType());
  layout->addWidget(new CamDecelDistAdd());
  layout->addWidget(new CurvDecelSelect());
  layout->addWidget(new VCurvSpeedUD());
  layout->addWidget(new VCurvSpeed());
  layout->addWidget(new OCurvSpeedUD());
  layout->addWidget(new OCurvSpeed());
  layout->addWidget(new AutoEnabledToggle());
  layout->addWidget(new AutoEnableSpeed());
  layout->addWidget(new CruiseAutoResToggle());
  layout->addWidget(new RESChoice());
  layout->addWidget(new AutoResCondition());
  layout->addWidget(new AutoResLimitTime());
  layout->addWidget(new AutoRESDelay());
  layout->addWidget(new LaneWidth());
  layout->addWidget(new SpeedLaneWidthUD());
  layout->addWidget(new SpeedLaneWidth());
  layout->addWidget(new RoutineDriveOnToggle());
  layout->addWidget(new RoutineDriveOption());
  layout->addWidget(new CloseToRoadEdgeToggle());
  layout->addWidget(new OPKREdgeOffset());
  layout->addWidget(new ToAvoidLKASFaultToggle());
  layout->addWidget(new ToAvoidLKASFault());
}

DeveloperPanel::DeveloperPanel(QWidget *parent) : QFrame(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  layout->setContentsMargins(50, 0, 50, 0);
  layout->setSpacing(30);

  // OPKR
  layout->addWidget(new DebugUiOneToggle());
  layout->addWidget(new DebugUiTwoToggle());
  layout->addWidget(new DebugUiThreeToggle());
  layout->addWidget(new ShowErrorToggle());
  layout->addWidget(new LongLogToggle());
  layout->addWidget(new PrebuiltToggle());
  layout->addWidget(new FPTwoToggle());
  layout->addWidget(new WhitePandaSupportToggle());
  layout->addWidget(new BattLessToggle());
  layout->addWidget(new ComIssueToggle());
  layout->addWidget(new LDWSToggle());
  layout->addWidget(new GearDToggle());
  layout->addWidget(new SteerWarningFixToggle());
  layout->addWidget(new IgnoreCanErroronISGToggle());
  layout->addWidget(new FCA11MessageToggle());
  layout->addWidget(new MadModeEnabledToggle());
  layout->addWidget(new StockLKASEnabledatDisenagedStatusToggle());
  layout->addWidget(new C2WithCommaPowerToggle());
  layout->addWidget(new JoystickModeToggle());
  layout->addWidget(new TimeZoneSelectCombo());
  const char* cal_ok = "cp -f /data/openpilot/selfdrive/assets/addon/param/CalibrationParams /data/params/d/";
  auto calokbtn = new ButtonControl("캘리브레이션 강제 활성화", "실행");
  QObject::connect(calokbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("캘리브레이션을 강제로 설정합니다. 인게이지 확인용이니 실 주행시에는 초기화 하시기 바랍니다.", this)){
      std::system(cal_ok);
    }
  });
  layout->addWidget(calokbtn);

  layout->addWidget(horizontal_line());
  layout->addWidget(new CarSelectCombo());

  layout->addWidget(horizontal_line());
  layout->addWidget(new LabelControl("〓〓〓〓〓〓〓【 Panda Values 】〓〓〓〓〓〓〓", ""));
  layout->addWidget(new MaxSteer());
  layout->addWidget(new MaxRTDelta());
  layout->addWidget(new MaxRateUp());
  layout->addWidget(new MaxRateDown());
  const char* p_edit_go = "/data/openpilot/selfdrive/assets/addon/script/p_edit.sh ''";
  auto peditbtn = new ButtonControl("판다값 변경 적용", "실행");
  QObject::connect(peditbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("변경된 판다값을 적용합니다. 진행하시겠습니까? 자동 재부팅됩니다.", this)){
      std::system(p_edit_go);
    }
  });
  layout->addWidget(peditbtn);
}

TuningPanel::TuningPanel(QWidget *parent) : QFrame(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  layout->setContentsMargins(50, 0, 50, 0);
  layout->setSpacing(30);

  // OPKR
  layout->addWidget(new LabelControl("〓〓〓〓〓〓〓〓【 TUNING 】〓〓〓〓〓〓〓〓", ""));
  layout->addWidget(new CameraOffset());
  layout->addWidget(new PathOffset());
  layout->addWidget(new LiveSteerRatioToggle());
  layout->addWidget(new LiveSRPercent());
  layout->addWidget(new SRBaseControl());
  layout->addWidget(new SRMaxControl());
  layout->addWidget(new SteerActuatorDelay());
  layout->addWidget(new SteerRateCost());
  layout->addWidget(new SteerLimitTimer());
  layout->addWidget(new TireStiffnessFactor());
  layout->addWidget(new VariableSteerMaxToggle());
  layout->addWidget(new SteerMax());
  layout->addWidget(new VariableSteerDeltaToggle());
  layout->addWidget(new SteerDeltaUp());
  layout->addWidget(new SteerDeltaDown());
  layout->addWidget(new ToAvoidLKASFaultBeyondToggle());
  layout->addWidget(new SteerThreshold());
  layout->addWidget(new DesiredCurvatureLimit());

  layout->addWidget(horizontal_line());

  layout->addWidget(new LabelControl("〓〓〓〓〓〓〓〓【 CONTROL 】〓〓〓〓〓〓〓〓", ""));
  layout->addWidget(new LateralControl());
  layout->addWidget(new LiveTunePanelToggle());
  QString lat_control = QString::fromStdString(Params().get("LateralControlMethod", false));
  if (lat_control == "0") {
    layout->addWidget(new PidKp());
    layout->addWidget(new PidKi());
    layout->addWidget(new PidKd());
    layout->addWidget(new PidKf());
  } else if (lat_control == "1") {
    layout->addWidget(new InnerLoopGain());
    layout->addWidget(new OuterLoopGain());
    layout->addWidget(new TimeConstant());
    layout->addWidget(new ActuatorEffectiveness());
  } else if (lat_control == "2") {
    layout->addWidget(new Scale());
    layout->addWidget(new LqrKi());
    layout->addWidget(new DcGain());
  } else if (lat_control == "3") {
    layout->addWidget(new TorqueMaxLatAccel());
    layout->addWidget(new TorqueKp());
    layout->addWidget(new TorqueKf());
    layout->addWidget(new TorqueKi());
    layout->addWidget(new TorqueFriction());
    layout->addWidget(new TorqueUseAngle());
  }

  layout->addWidget(horizontal_line());

  layout->addWidget(new LabelControl("〓〓〓〓〓〓〓【 LONGCONTROL 】〓〓〓〓〓〓〓", ""));
  layout->addWidget(new CustomTRToggle());
  layout->addWidget(new CruiseGapTR());
  layout->addWidget(new DynamicTRGap());
  layout->addWidget(new DynamicTRUD());
  layout->addWidget(new DynamicTRBySpeed());
  layout->addWidget(new RadarLongHelperOption());
  layout->addWidget(new StoppingDistAdjToggle());
  layout->addWidget(new E2ELongToggle());
  layout->addWidget(new StockDecelonCamToggle());
  //layout->addWidget(new RadarDisableToggle());
  //layout->addWidget(new UseRadarTrackToggle());
}

void SettingsWindow::showEvent(QShowEvent *event) {
  panel_widget->setCurrentIndex(0);
  nav_btns->buttons()[0]->setChecked(true);
}

SettingsWindow::SettingsWindow(QWidget *parent) : QFrame(parent) {

  // setup two main layouts
  sidebar_widget = new QWidget;
  QVBoxLayout *sidebar_layout = new QVBoxLayout(sidebar_widget);
  sidebar_layout->setMargin(0);
  panel_widget = new QStackedWidget();
  panel_widget->setStyleSheet(R"(
    border-radius: 30px;
    background-color: #292929;
  )");

  // close button
  QPushButton *close_btn = new QPushButton("×");
  close_btn->setStyleSheet(R"(
    QPushButton {
      font-size: 140px;
      padding-bottom: 20px;
      font-weight: bold;
      border 1px grey solid;
      border-radius: 50px;
      background-color: #292929;
      font-weight: 400;
    }
    QPushButton:pressed {
      background-color: #3B3B3B;
    }
  )");
  close_btn->setFixedSize(220, 130);
  sidebar_layout->addSpacing(5);
  sidebar_layout->addWidget(close_btn, 0, Qt::AlignCenter);
  QObject::connect(close_btn, &QPushButton::clicked, this, &SettingsWindow::closeSettings);

  // setup panels
  DevicePanel *device = new DevicePanel(this);
  SoftwarePanel *software = new SoftwarePanel(this);
  QObject::connect(device, &DevicePanel::reviewTrainingGuide, this, &SettingsWindow::reviewTrainingGuide);
  QObject::connect(device, &DevicePanel::showDriverView, this, &SettingsWindow::showDriverView);
  QObject::connect(software, &SoftwarePanel::closeSettings, this, &SettingsWindow::closeSettings);

  QList<QPair<QString, QWidget *>> panels = {
    {"장치", device},
    {"네트워크", network_panel(this)},
    {"토글메뉴", new TogglesPanel(this)},
    {"소프트웨어", software},
    {"UI메뉴", new UIPanel(this)},
    {"주행", new DrivingPanel(this)},
    {"사용자설정", new DeveloperPanel(this)},
    {"튜닝", new TuningPanel(this)},
  };

  sidebar_layout->addSpacing(45);

#ifdef ENABLE_MAPS
  auto map_panel = new MapPanel(this);
  panels.push_back({"Navigation", map_panel});
  QObject::connect(map_panel, &MapPanel::closeSettings, this, &SettingsWindow::closeSettings);
#endif

  const int padding = panels.size() > 3 ? 0 : 15;

  nav_btns = new QButtonGroup(this);
  for (auto &[name, panel] : panels) {
    QPushButton *btn = new QPushButton(name);
    btn->setCheckable(true);
    btn->setChecked(nav_btns->buttons().size() == 0);
    btn->setStyleSheet(QString(R"(
      QPushButton {
        color: grey;
        border: none;
        background: none;
        font-size: 54px;
        font-weight: 500;
        padding-top: %1px;
        padding-bottom: %1px;
      }
      QPushButton:checked {
        color: white;
      }
      QPushButton:pressed {
        color: #ADADAD;
      }
    )").arg(padding));

    nav_btns->addButton(btn);
    sidebar_layout->addWidget(btn, 0, Qt::AlignRight);

    const int lr_margin = name != "Network" ? 50 : 0;  // Network panel handles its own margins
    panel->setContentsMargins(lr_margin, 25, lr_margin, 25);

    ScrollView *panel_frame = new ScrollView(panel, this);
    panel_widget->addWidget(panel_frame);

    QObject::connect(btn, &QPushButton::clicked, [=, w = panel_frame]() {
      btn->setChecked(true);
      panel_widget->setCurrentWidget(w);
    });
  }
  sidebar_layout->setContentsMargins(50, 50, 100, 50);

  // main settings layout, sidebar + main panel
  QHBoxLayout *main_layout = new QHBoxLayout(this);

  sidebar_widget->setFixedWidth(500);
  main_layout->addWidget(sidebar_widget);
  main_layout->addWidget(panel_widget);

  setStyleSheet(R"(
    * {
      color: white;
      font-size: 50px;
    }
    SettingsWindow {
      background-color: black;
    }
  )");
}

void SettingsWindow::hideEvent(QHideEvent *event) {
#ifdef QCOM
  HardwareEon::close_activities();
#endif
}
