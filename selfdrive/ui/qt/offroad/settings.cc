#include "selfdrive/ui/qt/offroad/settings.h"

#include <cassert>
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
      "CommunityFeaturesToggle",
      "커뮤니티 기능 사용",
      "comma.ai에서 유지 또는 지원하지 않고 표준 안전 모델에 부합하는 것으로 확인되지 않은 오픈 소스 커뮤니티의 기능을 사용하십시오. 이러한 기능에는 커뮤니티 지원 자동차와 커뮤니티 지원 하드웨어가 포함됩니다. 이러한 기능을 사용할 때는 각별히 주의해야 합니다.",
      "../assets/offroad/icon_shell.png",
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
  Params params = Params();

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
        desc += QString(" Your device is pointed %1° %2 and %3° %4.")
                    .arg(QString::number(std::abs(pitch), 'g', 1), pitch > 0 ? "up" : "down",
                         QString::number(std::abs(yaw), 'g', 1), yaw > 0 ? "right" : "left");
      }
    } catch (kj::Exception) {
      qInfo() << "invalid CalibrationParams";
    }
  }
  qobject_cast<ButtonControl *>(sender())->setDescription(desc);
}

void DevicePanel::reboot() {
  if (QUIState::ui_state.status == UIStatus::STATUS_DISENGAGED) {
    if (ConfirmationDialog::confirm("Are you sure you want to reboot?", this)) {
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
    if (ConfirmationDialog::confirm("Are you sure you want to power off?", this)) {
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
  lastUpdateLbl = new LabelControl("Last Updates Date", "", "");
  updateBtn = new ButtonControl("Check for Updates", "");
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
    desc += QString("LOCAL: %1\nREMOT: %2%3%4\n").arg(commit_local, commit_remote, empty, empty);
    
    if (!last_ping.length()) {
      desc += QString("Network connection is missing or unstable. Check the connection.");
    } else if (commit_local == commit_remote) {
      desc += QString("Local and remote match. No update required.");
    } else {
      desc += QString("There's an update. Press the OK button to go.");
    }
    if (ConfirmationDialog::confirm(desc, this)) {
      std::system("/data/openpilot/selfdrive/assets/addon/script/gitpull.sh");
    }
  });


  auto uninstallBtn = new ButtonControl("Uninstall " + getBrand(), "UNINSTALL");
  connect(uninstallBtn, &ButtonControl::clicked, [&]() {
    if (ConfirmationDialog::confirm("Are you sure you want to uninstall?", this)) {
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

  QPushButton *presetoneload_btn = new QPushButton("Load Preset1");
  presetoneload_btn->setStyleSheet("height: 120px;border-radius: 15px;background-color: #393939;");
  presetone_layout->addWidget(presetoneload_btn);
  QObject::connect(presetoneload_btn, &QPushButton::clicked, [=]() {
    if (ConfirmationDialog::confirm("Do you want to load Preset1?", this)) {
      QProcess::execute("/data/openpilot/selfdrive/assets/addon/script/load_preset1.sh");
    }
  });

  QPushButton *presetonesave_btn = new QPushButton("Save Preset1");
  presetonesave_btn->setStyleSheet("height: 120px;border-radius: 15px;background-color: #393939;");
  presetone_layout->addWidget(presetonesave_btn);
  QObject::connect(presetonesave_btn, &QPushButton::clicked, [=]() {
    if (ConfirmationDialog::confirm("Do you want to save Preset1?", this)) {
      QProcess::execute("/data/openpilot/selfdrive/assets/addon/script/save_preset1.sh");
    }
  });

  // preset2 buttons
  QHBoxLayout *presettwo_layout = new QHBoxLayout();
  presettwo_layout->setSpacing(50);

  QPushButton *presettwoload_btn = new QPushButton("Load Preset2");
  presettwoload_btn->setStyleSheet("height: 120px;border-radius: 15px;background-color: #393939;");
  presettwo_layout->addWidget(presettwoload_btn);
  QObject::connect(presettwoload_btn, &QPushButton::clicked, [=]() {
    if (ConfirmationDialog::confirm("Do you want to load Preset2?", this)) {
      QProcess::execute("/data/openpilot/selfdrive/assets/addon/script/load_preset2.sh");
    }
  });

  QPushButton *presettwosave_btn = new QPushButton("Save Preset2");
  presettwosave_btn->setStyleSheet("height: 120px;border-radius: 15px;background-color: #393939;");
  presettwo_layout->addWidget(presettwosave_btn);
  QObject::connect(presettwosave_btn, &QPushButton::clicked, [=]() {
    if (ConfirmationDialog::confirm("Do you want to save Preset2?", this)) {
      QProcess::execute("/data/openpilot/selfdrive/assets/addon/script/save_preset2.sh");
    }
  });

  auto paraminit_btn = new ButtonControl("Parameters Init", "RUN");
  QObject::connect(paraminit_btn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("Initialize parameters. Changes in the EON menu are changed to the initial set value. Do you want to proceed?", this)){
      QProcess::execute("/data/openpilot/selfdrive/assets/addon/script/init_param.sh");
    }
  });

  addItem(presetone_layout);
  addItem(presettwo_layout);

  addItem(paraminit_btn);

  const char* git_reset = "/data/openpilot/selfdrive/assets/addon/script/git_reset.sh ''";
  auto gitresetbtn = new ButtonControl("Git Reset", "RUN");
  QObject::connect(gitresetbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("Apply the latest commitment details of Remote Git after forced initialization of local changes. Do you want to proceed?", this)){
      std::system(git_reset);
    }
  });
  addItem(gitresetbtn);

  const char* gitpull_cancel = "/data/openpilot/selfdrive/assets/addon/script/gitpull_cancel.sh ''";
  auto gitpullcanceltbtn = new ButtonControl("Cancel Git Pull", "RUN");
  QObject::connect(gitpullcanceltbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("Return to the state before GitPull. Do you want to proceed?", this)){
      std::system(gitpull_cancel);
    }
  });
  addItem(gitpullcanceltbtn);

  const char* panda_flashing = "/data/openpilot/selfdrive/assets/addon/script/panda_flashing.sh ''";
  auto pandaflashingtbtn = new ButtonControl("Panda Flashing", "RUN");
  QObject::connect(pandaflashingtbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("Panda's green LED blinks quickly during panda flashing. Never turn off or disconnect the device arbitrarily. Do you want to proceed?", this)) {
      std::system(panda_flashing);
    }
  });

  addItem(pandaflashingtbtn);
  addItem(new SwitchOpenpilot()); // opkr
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

QWidget * network_panel(QWidget * parent) {
#ifdef QCOM
  QWidget *w = new QWidget(parent);
  QVBoxLayout *layout = new QVBoxLayout(w);
  layout->setContentsMargins(50, 0, 50, 0);

  ListWidget *list = new ListWidget();
  list->setSpacing(30);
  // wifi + tethering buttons
  auto wifiBtn = new ButtonControl("Wi-Fi Settings", "OPEN");
  QObject::connect(wifiBtn, &ButtonControl::clicked, [=]() { HardwareEon::launch_wifi(); });
  list->addItem(wifiBtn);

  auto tetheringBtn = new ButtonControl("Tethering Settings", "OPEN");
  QObject::connect(tetheringBtn, &ButtonControl::clicked, [=]() { HardwareEon::launch_tethering(); });
  list->addItem(tetheringBtn);

  list->addItem(new HotspotOnBootToggle());

  // SSH key management
  list->addItem(new SshToggle());
  list->addItem(new SshControl());
  list->addItem(new SshLegacyToggle());

  layout->addWidget(list);
  layout->addStretch(1);
#else
  Networking *w = new Networking(parent);
#endif
  return w;
}

UserPanel::UserPanel(QWidget *parent) : QFrame(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  layout->setContentsMargins(50, 0, 50, 0);
  layout->setSpacing(30);

  // OPKR
  layout->addWidget(new LabelControl("〓〓〓〓〓〓〓〓【 U I Menu 】〓〓〓〓〓〓〓〓", ""));
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
  auto recorddelbtn = new ButtonControl("Delete All Recorded Files", "RUN");
  QObject::connect(recorddelbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("Delete all saved recorded files. Do you want to proceed?", this)){
      std::system(record_del);
    }
  });
  layout->addWidget(recorddelbtn);
  const char* realdata_del = "rm -rf /storage/emulated/0/realdata/*";
  auto realdatadelbtn = new ButtonControl("Delete All Driving Logs", "RUN");
  QObject::connect(realdatadelbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("Delete all saved driving logs. Do you want to proceed?", this)){
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
  layout->addWidget(new KRDateToggle());
  layout->addWidget(new KRTimeToggle());
  layout->addWidget(new OPKRServerSelect());
  layout->addWidget(new OPKRServerAPI());
  layout->addWidget(new OPKRMapboxStyle());

  layout->addWidget(horizontal_line());
  layout->addWidget(new LabelControl("〓〓〓〓〓〓〓〓【 DRIVING 】〓〓〓〓〓〓〓〓", ""));
  layout->addWidget(new AutoResumeToggle());
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
  layout->addWidget(new OSMSpeedLimitEnabledToggle());
  layout->addWidget(new StockNaviSpeedToggle());
  layout->addWidget(new SpeedLimitOffset());
  layout->addWidget(new SpeedLimitSignType());
  layout->addWidget(new CamDecelDistAdd());
  layout->addWidget(new CruiseGapAdjustToggle());
  layout->addWidget(new CurvDecelSelect());
  layout->addWidget(new VCurvSpeedUD());
  layout->addWidget(new VCurvSpeed());
  layout->addWidget(new OCurvOffset());
  layout->addWidget(new AutoEnabledToggle());
  layout->addWidget(new AutoEnableSpeed());
  layout->addWidget(new CruiseAutoResToggle());
  layout->addWidget(new RESChoice());
  layout->addWidget(new AutoResCondition());
  layout->addWidget(new AutoResLimitTime());
  layout->addWidget(new RESCountatStandstill());
  layout->addWidget(new SteerWindDownToggle());
  layout->addWidget(new MadModeEnabledToggle());

  layout->addWidget(horizontal_line());
  layout->addWidget(new LabelControl("〓〓〓〓〓〓〓〓【 DEVELOPER 】〓〓〓〓〓〓〓〓", ""));
  layout->addWidget(new DebugUiOneToggle());
  layout->addWidget(new DebugUiTwoToggle());
  layout->addWidget(new ShowErrorToggle());
  layout->addWidget(new LongLogToggle());
  layout->addWidget(new PrebuiltToggle());
  layout->addWidget(new FPTwoToggle());
  layout->addWidget(new LDWSToggle());
  layout->addWidget(new GearDToggle());
  layout->addWidget(new ComIssueToggle());
  layout->addWidget(new WhitePandaSupportToggle());
  layout->addWidget(new SteerWarningFixToggle());
  layout->addWidget(new IgnoreCanErroronISGToggle());
  layout->addWidget(new BattLessToggle());
  layout->addWidget(new GoogleMapEnabledToggle());
  layout->addWidget(new StockLKASEnabledatDisenagedStatusToggle());
  layout->addWidget(new TimeZoneSelectCombo());
  const char* cal_ok = "cp -f /data/openpilot/selfdrive/assets/addon/param/CalibrationParams /data/params/d/";
  auto calokbtn = new ButtonControl("Enable Calibration by Force", "RUN");
  QObject::connect(calokbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("Force calibration to be set. It's only for checking engagement, so please reset it when you actually drive.", this)){
      std::system(cal_ok);
    }
  });
  layout->addWidget(calokbtn);
  const char* open_settings = "am start -a android.intent.action.MAIN -n com.android.settings/.Settings";
  auto open_settings_btn = new ButtonControl("Open Android Settings", "RUN");
  QObject::connect(open_settings_btn, &ButtonControl::clicked, [=]() {
    emit closeSettings();
    std::system(open_settings);
  });
  layout->addWidget(open_settings_btn);
  const char* softkey = "am start com.gmd.hidesoftkeys/com.gmd.hidesoftkeys.MainActivity";
  auto softkey_btn = new ButtonControl("SoftKey RUN/SET", "RUN");
  QObject::connect(softkey_btn, &ButtonControl::clicked, [=]() {
    emit closeSettings();
    std::system(softkey);
  });
  layout->addWidget(softkey_btn);
  auto mixplorer_btn = new ButtonControl("RUN Mixplorer", "RUN");
  QObject::connect(mixplorer_btn, &ButtonControl::clicked, [=]() {
	  emit closeSettings();
    std::system("/data/openpilot/selfdrive/assets/addon/script/run_mixplorer.sh");
  });
  layout->addWidget(mixplorer_btn, 0);
  layout->addWidget(horizontal_line());
  layout->addWidget(new CarSelectCombo());

  layout->addWidget(horizontal_line());
  layout->addWidget(new LabelControl("〓〓〓〓〓〓〓【 Panda Values 】〓〓〓〓〓〓〓", ""));
  layout->addWidget(new MaxSteer());
  layout->addWidget(new MaxRTDelta());
  layout->addWidget(new MaxRateUp());
  layout->addWidget(new MaxRateDown());
  const char* p_edit_go = "/data/openpilot/selfdrive/assets/addon/script/p_edit.sh ''";
  auto peditbtn = new ButtonControl("Change Panda Values", "RUN");
  QObject::connect(peditbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm("Apply the changed panda value. Do you want to proceed? It automatically reboots.", this)){
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
  layout->addWidget(new SteerMaxBase());
  layout->addWidget(new SteerMaxMax());
  layout->addWidget(new SteerMaxv());
  layout->addWidget(new VariableSteerMaxToggle());
  layout->addWidget(new SteerDeltaUpBase());
  layout->addWidget(new SteerDeltaUpMax());
  layout->addWidget(new SteerDeltaDownBase());
  layout->addWidget(new SteerDeltaDownMax());
  layout->addWidget(new VariableSteerDeltaToggle());
  layout->addWidget(new SteerThreshold());

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
  }

  layout->addWidget(horizontal_line());

  layout->addWidget(new LabelControl("〓〓〓〓〓〓〓【 LONGCONTROL 】〓〓〓〓〓〓〓", ""));
  layout->addWidget(new DynamicTR());
  layout->addWidget(new CruiseGapTR());
  layout->addWidget(new RadarLongHelperOption());
  layout->addWidget(new StoppingDistAdjToggle());
  layout->addWidget(new E2ELongToggle());
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
  sidebar_layout->addSpacing(35);
  sidebar_layout->addWidget(close_btn, 0, Qt::AlignCenter);
  QObject::connect(close_btn, &QPushButton::clicked, this, &SettingsWindow::closeSettings);

  // setup panels
  DevicePanel *device = new DevicePanel(this);
  UserPanel *user = new UserPanel(this);
  QObject::connect(device, &DevicePanel::reviewTrainingGuide, this, &SettingsWindow::reviewTrainingGuide);
  QObject::connect(device, &DevicePanel::showDriverView, this, &SettingsWindow::showDriverView);
  QObject::connect(user, &UserPanel::closeSettings, this, &SettingsWindow::closeSettings);

  QList<QPair<QString, QWidget *>> panels = {
    {"Device", device},
    {"Network", network_panel(this)},
    {"Toggles", new TogglesPanel(this)},
    {"Software", new SoftwarePanel(this)},
    {"UserMenu", user},
    {"Tuning", new TuningPanel(this)},
  };

  sidebar_layout->addSpacing(35);

#ifdef ENABLE_MAPS
  auto map_panel = new MapPanel(this);
  panels.push_back({"Navigation", map_panel});
  QObject::connect(map_panel, &MapPanel::closeSettings, this, &SettingsWindow::closeSettings);
#endif

  const int padding = panels.size() > 3 ? 8 : 18;

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
        font-size: 60px;
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
