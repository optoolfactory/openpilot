#pragma once

#include <QPushButton>
#include <QLineEdit>
#include <QSoundEffect>

#include "selfdrive/hardware/hw.h"
#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/ui.h"
#include <QComboBox>
#include <QAbstractItemView>

class SwitchOpenpilot : public ButtonControl {
  Q_OBJECT

public:
  SwitchOpenpilot();

private:
  Params params;

  QString githubid;
  QString githubrepo;
  QString githubbranch;

  void refresh();
  void getUserID(const QString &userid);
  void getRepoID(const QString &repoid);
  void getBranchID(const QString &branchid);
};

class SshLegacyToggle : public ToggleControl {
  Q_OBJECT

public:
  SshLegacyToggle() : ToggleControl("기존 공개KEY 사용", "SSH 접속시 기존 공개KEY(0.8.2이하)를 사용합니다.", "", Params().getBool("OpkrSSHLegacy")) {
    QObject::connect(this, &SshLegacyToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrSSHLegacy", status);
    });
  }
};

class AutoResumeToggle : public ToggleControl {
  Q_OBJECT

public:
  AutoResumeToggle() : ToggleControl("자동출발 기능 사용", "SCC 사용중 정차시 자동출발 기능을 사용합니다.", "../assets/offroad/icon_shell.png", Params().getBool("OpkrAutoResume")) {
    QObject::connect(this, &AutoResumeToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrAutoResume", status);
    });
  }
};

class VariableCruiseToggle : public ToggleControl {
  Q_OBJECT

public:
  VariableCruiseToggle() : ToggleControl("가변 크루즈 사용", "SCC 사용중 크루즈 버튼을 이용하여 가감속을 보조합니다.", "../assets/offroad/icon_shell.png", Params().getBool("OpkrVariableCruise")) {
    QObject::connect(this, &VariableCruiseToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrVariableCruise", status);
    });
  }
};

class CruiseGapAdjustToggle : public ToggleControl {
  Q_OBJECT

public:
  CruiseGapAdjustToggle() : ToggleControl("정차시 크루즈 갭 자동변경", "정차시 빠른 출발을 위해 크루즈갭을 1칸으로 변경하며, 출발후 일정조건에 따라 다시 원래의 크루즈갭으로 돌아갑니다.", "../assets/offroad/icon_shell.png", Params().getBool("CruiseGapAdjust")) {
    QObject::connect(this, &CruiseGapAdjustToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("CruiseGapAdjust", status);
    });
  }
};

class AutoEnabledToggle : public ToggleControl {
  Q_OBJECT

public:
  AutoEnabledToggle() : ToggleControl("자동 인게이지 활성", "디스인게이지 상태에서 크루즈 버튼상태가 대기상태(CRUISE표시만 나오고 속도 지정이 안된상태)일 경우 자동 인게이지를 활성화 합니다.", "../assets/offroad/icon_shell.png", Params().getBool("AutoEnable")) {
    QObject::connect(this, &AutoEnabledToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("AutoEnable", status);
    });
  }
};

class CruiseAutoResToggle : public ToggleControl {
  Q_OBJECT

public:
  CruiseAutoResToggle() : ToggleControl("자동RES 사용", "크루즈 사용중 브레이크를 밟아 대기모드 상태로 변한경우(CANCEL은 해당없음) 브레이크 페달 해제/가속페달조작 시 다시 이전 속도로 세팅합니다. 크루즈 속도가 세팅되어 있고 차량속도가 30km/h 이상 혹은 앞차가 인식될 경우 작동합니다.", "../assets/offroad/icon_shell.png", Params().getBool("CruiseAutoRes")) {
    QObject::connect(this, &CruiseAutoResToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("CruiseAutoRes", status);
    });
  }
};

class BatteryChargingControlToggle : public ToggleControl {
  Q_OBJECT

public:
  BatteryChargingControlToggle() : ToggleControl("배터리 충전 제어기능 사용", "배터리 충전제어 기능을 사용합니다.", "../assets/offroad/icon_shell.png", Params().getBool("OpkrBatteryChargingControl")) {
    QObject::connect(this, &BatteryChargingControlToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrBatteryChargingControl", status);
    });
  }
};

class BlindSpotDetectToggle : public ToggleControl {
  Q_OBJECT

public:
  BlindSpotDetectToggle() : ToggleControl("후측방 감지 아이콘 표시", "후측방에 차가 감지되면 화면에 아이콘을 띄웁니다.", "../assets/offroad/icon_shell.png", Params().getBool("OpkrBlindSpotDetect")) {
    QObject::connect(this, &BlindSpotDetectToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrBlindSpotDetect", status);
      if (state) {
        QUIState::ui_state.scene.nOpkrBlindSpotDetect = true;
      } else {
        QUIState::ui_state.scene.nOpkrBlindSpotDetect = false;
      }
    });
  }
};

class MadModeEnabledToggle : public ToggleControl {
  Q_OBJECT

public:
  MadModeEnabledToggle() : ToggleControl("MainSW 오픈파일럿 ON/OFF", "크루즈 MainSW를 이용하여 오파를 활성화 합니다.", "../assets/offroad/icon_shell.png", Params().getBool("MadModeEnabled")) {
    QObject::connect(this, &MadModeEnabledToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("MadModeEnabled", status);
    });
  }
};

class WhitePandaSupportToggle : public ToggleControl {
  Q_OBJECT

public:
  WhitePandaSupportToggle() : ToggleControl("화이트판다 지원", "화이트판다 사용시 기능을 켜십시오", "../assets/offroad/icon_shell.png", Params().getBool("WhitePandaSupport")) {
    QObject::connect(this, &WhitePandaSupportToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("WhitePandaSupport", status);
    });
  }
};

class SteerWarningFixToggle : public ToggleControl {
  Q_OBJECT

public:
  SteerWarningFixToggle() : ToggleControl("조향경고 해제", "차량에서 조향에러가 발생하여 오파 실행이 불가능할 때 기능을 켜십시오(일부 차량 Only). 주행 중 정상적인 에러환경에서 발생시에는 기능을 켜지 마십시오.", "../assets/offroad/icon_shell.png", Params().getBool("SteerWarningFix")) {
    QObject::connect(this, &SteerWarningFixToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("SteerWarningFix", status);
    });
  }
};

class LiveSteerRatioToggle : public ToggleControl {
  Q_OBJECT

public:
  LiveSteerRatioToggle() : ToggleControl("Live SteerRatio 사용", "가변/고정 SR 대신 Live SteerRatio를 사용합니다.", "../assets/offroad/icon_shell.png", Params().getBool("OpkrLiveSteerRatio")) {
    QObject::connect(this, &LiveSteerRatioToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrLiveSteerRatio", status);
    });
  }
};

class VariableSteerMaxToggle : public ToggleControl {
  Q_OBJECT

public:
  VariableSteerMaxToggle() : ToggleControl("가변 SteerMax 사용", "곡률에 따른 가변 SteerMax을 사용합니다.", "../assets/offroad/icon_shell.png", Params().getBool("OpkrVariableSteerMax")) {
    QObject::connect(this, &VariableSteerMaxToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrVariableSteerMax", status);
    });
  }
};

class VariableSteerDeltaToggle : public ToggleControl {
  Q_OBJECT

public:
  VariableSteerDeltaToggle() : ToggleControl("가변 SteerDelta 사용", "곡률에 따른 가변 SteerDelta를 사용합니다.( DeltaUp ~ 5까지 변화, DeltaDown ~ 10까지 변화", "../assets/offroad/icon_shell.png", Params().getBool("OpkrVariableSteerDelta")) {
    QObject::connect(this, &VariableSteerDeltaToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrVariableSteerDelta", status);
    });
  }
};

class DrivingRecordToggle : public ToggleControl {
  Q_OBJECT

public:
  DrivingRecordToggle() : ToggleControl("자동 화면녹화 기능 사용", "운전 중 화면 녹화/중지를 자동으로 수행합니다. 출발 후 녹화가 시작되며 차량이 정지하면 녹화가 종료됩니다.", "../assets/offroad/icon_shell.png", Params().getBool("OpkrDrivingRecord")) {
    QObject::connect(this, &DrivingRecordToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrDrivingRecord", status);
      if (state) {
        QUIState::ui_state.scene.driving_record = true;
      } else {
        QUIState::ui_state.scene.driving_record = false;
      }
    });
  }
};

class TurnSteeringDisableToggle : public ToggleControl {
  Q_OBJECT

public:
  TurnSteeringDisableToggle() : ToggleControl("방향지시등 사용시 조향해제 사용", "차선변경속도 이하로 주행할 때 방향지시등 사용시 자동조향을 일시해제 합니다.", "../assets/offroad/icon_shell.png", Params().getBool("OpkrTurnSteeringDisable")) {
    QObject::connect(this, &TurnSteeringDisableToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrTurnSteeringDisable", status);
    });
  }
};

class HotspotOnBootToggle : public ToggleControl {
  Q_OBJECT

public:
  HotspotOnBootToggle() : ToggleControl("부팅시 핫스팟 자동실행", "부팅 후 핫스팟을 자동으로 실행합니다.", "", Params().getBool("OpkrHotspotOnBoot")) {
    QObject::connect(this, &HotspotOnBootToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrHotspotOnBoot", status);
    });
  }
};

class CruiseOverMaxSpeedToggle : public ToggleControl {
  Q_OBJECT

public:
  CruiseOverMaxSpeedToggle() : ToggleControl("설정속도를 초과속도에 동기화", "현재속도가 설정속도를 넘어설 경우 설정속도를 현재속도에 동기화합니다.", "../assets/offroad/icon_shell.png", Params().getBool("CruiseOverMaxSpeed")) {
    QObject::connect(this, &CruiseOverMaxSpeedToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("CruiseOverMaxSpeed", status);
    });
  }
};

class DebugUiOneToggle : public ToggleControl {
  Q_OBJECT

public:
  DebugUiOneToggle() : ToggleControl("DEBUG UI 1", "", "../assets/offroad/icon_shell.png", Params().getBool("DebugUi1")) {
    QObject::connect(this, &DebugUiOneToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("DebugUi1", status);
      if (state) {
        QUIState::ui_state.scene.nDebugUi1 = true;
      } else {
        QUIState::ui_state.scene.nDebugUi1 = false;
      }
    });
  }
};

class DebugUiTwoToggle : public ToggleControl {
  Q_OBJECT

public:
  DebugUiTwoToggle() : ToggleControl("DEBUG UI 2", "", "../assets/offroad/icon_shell.png", Params().getBool("DebugUi2")) {
    QObject::connect(this, &DebugUiTwoToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("DebugUi2", status);
      if (state) {
        QUIState::ui_state.scene.nDebugUi2 = true;
      } else {
        QUIState::ui_state.scene.nDebugUi2 = false;
      }
    });
  }
};

class DebugUiThreeToggle : public ToggleControl {
  Q_OBJECT

public:
  DebugUiThreeToggle() : ToggleControl("DEBUG UI 3", "", "../assets/offroad/icon_shell.png", Params().getBool("DebugUi3")) {
    QObject::connect(this, &DebugUiThreeToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("DebugUi3", status);
      if (state) {
        QUIState::ui_state.scene.nDebugUi3 = true;
      } else {
        QUIState::ui_state.scene.nDebugUi3 = false;
      }
    });
  }
};

class LongLogToggle : public ToggleControl {
  Q_OBJECT

public:
  LongLogToggle() : ToggleControl("LONG LOG 표시", "화면에 가변크루즈 로그 대신 롱 튜닝 디버그용 로그를 표시합니다.", "../assets/offroad/icon_shell.png", Params().getBool("LongLogDisplay")) {
    QObject::connect(this, &LongLogToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("LongLogDisplay", status);
    });
  }
};

class PrebuiltToggle : public ToggleControl {
  Q_OBJECT

public:
  PrebuiltToggle() : ToggleControl("스마트 프리빌트 사용", "미리 빌드된 파일을 만들고 부팅 속도를 높입니다. 이 기능이 켜져 있으면 캐시를 이용하여 부팅 속도가 빨라지며, 코드 수정 후 메뉴에서 업데이트 버튼을 누르거나 커맨드 창에서 'gi' 명령어로 재부팅을 하면 자동으로 제거하고 그것을 컴파일하십시오.", "../assets/offroad/icon_shell.png", Params().getBool("PutPrebuiltOn")) {
    QObject::connect(this, &PrebuiltToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("PutPrebuiltOn", status);
    });
  }
};

class LDWSToggle : public ToggleControl {
  Q_OBJECT

public:
  LDWSToggle() : ToggleControl("LDWS 차량 설정", "", "../assets/offroad/icon_shell.png", Params().getBool("LdwsCarFix")) {
    QObject::connect(this, &LDWSToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("LdwsCarFix", status);
    });
  }
};

class FPTwoToggle : public ToggleControl {
  Q_OBJECT

public:
  FPTwoToggle() : ToggleControl("FingerPrint 2.0 설정", "핑거프린트2.0을 활성화 합니다. ECU인식으로 차량을 활성화 합니다.", "../assets/offroad/icon_shell.png", Params().getBool("FingerprintTwoSet")) {
    QObject::connect(this, &FPTwoToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("FingerprintTwoSet", status);
    });
  }
};

class GearDToggle : public ToggleControl {
  Q_OBJECT

public:
  GearDToggle() : ToggleControl("드라이브기어 강제인식", "기어인식문제로 인게이지가 되지 않을 때 사용합니다. 근본적으로 CABANA데이터를 분석해야 하지만, 임시적으로 해결합니다.", "../assets/offroad/icon_shell.png", Params().getBool("JustDoGearD")) {
    QObject::connect(this, &GearDToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("JustDoGearD", status);
    });
  }
};

class ComIssueToggle : public ToggleControl {
  Q_OBJECT

public:
  ComIssueToggle() : ToggleControl("프로세스간 통신오류 알람 끄기", "화이트판다 사용시 프로세스간 통신오류 알람을 끄기 위해 이옵션을 켜십시오.", "../assets/offroad/icon_shell.png", Params().getBool("ComIssueGone")) {
    QObject::connect(this, &ComIssueToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("ComIssueGone", status);
    });
  }
};

class RunNaviOnBootToggle : public ToggleControl {
  Q_OBJECT

public:
  RunNaviOnBootToggle() : ToggleControl("네비 자동 실행", "부팅 후 주행화면 전환 시 네비게이션(맵피)을 자동 실행합니다.", "../assets/offroad/icon_shell.png", Params().getBool("OpkrRunNaviOnBoot")) {
    QObject::connect(this, &RunNaviOnBootToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrRunNaviOnBoot", status);
    });
  }
};

class BattLessToggle : public ToggleControl {
  Q_OBJECT

public:
  BattLessToggle() : ToggleControl("배터리리스 사용", "배터리리스 이온용 토글입니다. 관련 설정이 적용됩니다.", "../assets/offroad/icon_shell.png", Params().getBool("OpkrBattLess")) {
    QObject::connect(this, &BattLessToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrBattLess", status);
    });
  }
};

class LiveTunePanelToggle : public ToggleControl {
  Q_OBJECT

public:
  LiveTunePanelToggle() : ToggleControl("라이브튠 사용 및 UI 표시", "화면에 라이브 튜닝 관련 UI를 표시합니다. 각종 튜닝 수치를 주행화면에서 라이브로 조정가능합니다. 조정 시 파라미터에 반영되며 토글을 끈 후 재부팅 해도 값이 유지됩니다.", "../assets/offroad/icon_shell.png", Params().getBool("OpkrLiveTunePanelEnable")) {
    QObject::connect(this, &LiveTunePanelToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OpkrLiveTunePanelEnable", status);
      if (state) {
        QUIState::ui_state.scene.live_tune_panel_enable = true;
        QUIState::ui_state.scene.opkr_livetune_ui = true;
      } else {
        QUIState::ui_state.scene.live_tune_panel_enable = false;
        QUIState::ui_state.scene.opkr_livetune_ui = false;
      }
    });
  }
};

class GitPullOnBootToggle : public ToggleControl {
  Q_OBJECT

public:
  GitPullOnBootToggle() : ToggleControl("부팅 시 Git Pull 자동실행", "부탕 후 업데이트가 있는 경우에 Git Pull을 자동 실행 후 재부팅합니다.", "", Params().getBool("GitPullOnBoot")) {
    QObject::connect(this, &GitPullOnBootToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("GitPullOnBoot", status);
    });
  }
};

class StoppingDistAdjToggle : public ToggleControl {
  Q_OBJECT

public:
  StoppingDistAdjToggle() : ToggleControl("정지거리 조정", "레이더 정지거리보다 조금 더 앞에 정지합니다. 일부 울컥거림 현상이 나타날 수 있으니 불편하신분들은 기능을 끄십시오.", "../assets/offroad/icon_shell.png", Params().getBool("StoppingDistAdj")) {
    QObject::connect(this, &StoppingDistAdjToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("StoppingDistAdj", status);
    });
  }
};

class ShowErrorToggle : public ToggleControl {
  Q_OBJECT

public:
  ShowErrorToggle() : ToggleControl("Show TMUX Error", "Display the error on the EON screen when a process error occurs while driving or off-road.", "../assets/offroad/icon_shell.png", Params().getBool("ShowError")) {
    QObject::connect(this, &ShowErrorToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("ShowError", status);
      if (state) {
        QUIState::ui_state.scene.show_error = true;
      } else {
        QUIState::ui_state.scene.show_error = false;
      }
    });
  }
};

class StockNaviSpeedToggle : public ToggleControl {
  Q_OBJECT

public:
  StockNaviSpeedToggle() : ToggleControl("순정네비 과속카메라 사용", "안전구간 감속 시 차량 순정 내비게이션의 안전속도를 이용(해당 데이터가 있는 일부 차량에 한함) .", "../assets/offroad/icon_shell.png", Params().getBool("StockNaviSpeedEnabled")) {
    QObject::connect(this, &StockNaviSpeedToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("StockNaviSpeedEnabled", status);
    });
  }
};

class E2ELongToggle : public ToggleControl {
  Q_OBJECT

public:
  E2ELongToggle() : ToggleControl("Enable E2E Long", "Activate E2E Long. It may work unexpectedly. Be careful.", "../assets/offroad/icon_shell.png", Params().getBool("E2ELong")) {
    QObject::connect(this, &E2ELongToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("E2ELong", status);
    });
  }
};

class GoogleMapEnabledToggle : public ToggleControl {
  Q_OBJECT

public:
  GoogleMapEnabledToggle() : ToggleControl("Use GoogleMap for Mapbox", "Use GoogleMap when you search a destination.", "../assets/offroad/icon_shell.png", Params().getBool("GoogleMapEnabled")) {
    QObject::connect(this, &GoogleMapEnabledToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("GoogleMapEnabled", status);
    });
  }
};

class OSMEnabledToggle : public ToggleControl {
  Q_OBJECT

public:
  OSMEnabledToggle() : ToggleControl("Enable OSM", "This enables OSM.", "../assets/offroad/icon_shell.png", Params().getBool("OSMEnable")) {
    QObject::connect(this, &OSMEnabledToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OSMEnable", status);
    });
  }
};

class OSMSpeedLimitEnabledToggle : public ToggleControl {
  Q_OBJECT

public:
  OSMSpeedLimitEnabledToggle() : ToggleControl("Enable OSM SpeedLimit", "This enables OSM speedlimit.", "../assets/offroad/icon_shell.png", Params().getBool("OSMSpeedLimitEnable")) {
    QObject::connect(this, &OSMSpeedLimitEnabledToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("OSMSpeedLimitEnable", status);
    });
  }
};

class IgnoreCanErroronISGToggle : public ToggleControl {
  Q_OBJECT

public:
  IgnoreCanErroronISGToggle() : ToggleControl("ISG 작동시 캔에러 무시", "ISG 작동시 캔에러가 발생하면 이 옵션을 켜세요", "../assets/offroad/icon_shell.png", Params().getBool("IgnoreCANErroronISG")) {
    QObject::connect(this, &IgnoreCanErroronISGToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("IgnoreCANErroronISG", status);
    });
  }
};

class StockLKASEnabledatDisenagedStatusToggle : public ToggleControl {
  Q_OBJECT

public:
  StockLKASEnabledatDisenagedStatusToggle() : ToggleControl("해제시 순정 LKAS 활성", "OP 해제 상태에서 Stock LKAS를 사용하려면 이 옵션을 켜십시오. PANDA를 통한 Stock CAN 메시지 여부에 따라 OP 활성 시 클러스터 오류와 관련이 있는 것 같습니다. ", "../assets/offroad/icon_shell.png", Params().getBool("StockLKASEnabled")) {
    QObject::connect(this, &StockLKASEnabledatDisenagedStatusToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("StockLKASEnabled", status);
    });
  }
};

class FCA11MessageToggle : public ToggleControl {
  Q_OBJECT

public:
  FCA11MessageToggle() : ToggleControl("FCA11 사용", "전방 추돌 신호를 SCC12 대신 FCA11을 사용합니다. 인게이지 혹은 부팅시 전방충돌오류가 날 때 사용합니다. 신호가 없는차는 캔오류가 날 수 있으니 주의바랍니다.(재부팅 필요)", "../assets/offroad/icon_shell.png", Params().getBool("FCA11Message")) {
    QObject::connect(this, &FCA11MessageToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("FCA11Message", status);
    });
  }
};

class StandstillResumeAltToggle : public ToggleControl {
  Q_OBJECT

public:
  StandstillResumeAltToggle() : ToggleControl("자동재출발 옵션", "정지 상태에서 자동 출발이 작동하지 않는 경우 이 기능을 켜십시오. 일부 차량 전용(ex. GENESIS). 활성화하기 전에 위의 RES 메시지 수를 조정하십시오.(재부팅 필요) ", "../assets/offroad/icon_shell.png", Params().getBool("StandstillResumeAlt")) {
    QObject::connect(this, &StandstillResumeAltToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("StandstillResumeAlt", status);
    });
  }
};

class MapboxEnabledToggle : public ToggleControl {
  Q_OBJECT

public:
  MapboxEnabledToggle() : ToggleControl("Enable Mapbox", "If you want to use Mapbox, turn on and then connect to device using web browser http://<device ip>:8082  Mapbox setting will show up and type mapbox pk and sk token(you can created this on mapbox.com website). If you want to search destinations with googlemap, first, you should create google api key and enable Enable GoogleMap for Mapbox", "../assets/offroad/icon_shell.png", Params().getBool("MapboxEnabled")) {
    QObject::connect(this, &MapboxEnabledToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("MapboxEnabled", status);
    });
  }
};

class UseRadarTrackToggle : public ToggleControl {
  Q_OBJECT

public:
  UseRadarTrackToggle() : ToggleControl("Use Radar Track", "Some cars have known radar tracks(from comma) for long control. This uses radar track directly instead of scc can message. Before you go, you must need to run hyundai_enable_radar_points.py in /data/openpilot/selfdrive/debug dir to enable your radar track. (Reboot required)", "../assets/offroad/icon_shell.png", Params().getBool("UseRadarTrack")) {
    QObject::connect(this, &UseRadarTrackToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("UseRadarTrack", status);
    });
  }
};

class RadarDisableToggle : public ToggleControl {
  Q_OBJECT

public:
  RadarDisableToggle() : ToggleControl("Disable Radar", "This is pre-requisite for LongControl of HKG. It seems that this affects AEB. So do not use this if you have any concern.", "../assets/offroad/icon_shell.png", Params().getBool("RadarDisable")) {
    QObject::connect(this, &RadarDisableToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("RadarDisable", status);
    });
  }
};

class C2WithCommaPowerToggle : public ToggleControl {
  Q_OBJECT

public:
  C2WithCommaPowerToggle() : ToggleControl("C2 with CommaPower", "This is for C2 users with Comma Power.", "../assets/offroad/icon_shell.png", Params().getBool("C2WithCommaPower")) {
    QObject::connect(this, &C2WithCommaPowerToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("C2WithCommaPower", status);
    });
  }
};

class CustomTRToggle : public ToggleControl {
  Q_OBJECT

public:
  CustomTRToggle() : ToggleControl("Custom TR Enable", "to use Custom TR not 1.45(comma default).", "../assets/offroad/icon_shell.png", Params().getBool("CustomTREnabled")) {
    QObject::connect(this, &CustomTRToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("CustomTREnabled", status);
    });
  }
};

class RoutineDriveOnToggle : public ToggleControl {
  Q_OBJECT

public:
  RoutineDriveOnToggle() : ToggleControl("Routine Drive by RoadName", "This will adjust the camera offset(for now) by roadname. If you want to use, edit the file, /data/params/d/RoadList. modify like this RoadName1,offset1(ex:+0.05),RoadName2,offset2(ex:-0.05),...", "../assets/offroad/icon_shell.png", Params().getBool("RoutineDriveOn")) {
    QObject::connect(this, &RoutineDriveOnToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("RoutineDriveOn", status);
    });
  }
};

class CloseToRoadEdgeToggle : public ToggleControl {
  Q_OBJECT

public:
  CloseToRoadEdgeToggle() : ToggleControl("Driving Close to RoadEdge", "This will adjust the camera offset to get close to road edge if the car is on the first or last lane.", "../assets/offroad/icon_shell.png", Params().getBool("CloseToRoadEdge")) {
    QObject::connect(this, &CloseToRoadEdgeToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("CloseToRoadEdge", status);
    });
  }
};

class ToAvoidLKASFaultToggle : public ToggleControl {
  Q_OBJECT

public:
  ToAvoidLKASFaultToggle() : ToggleControl("To Avoid LKAS Fault", "to avoid LKAS fault above max angle limit(car specific). This is live value. Find out your maxframe while driving.", "../assets/offroad/icon_shell.png", Params().getBool("AvoidLKASFaultEnabled")) {
    QObject::connect(this, &ToAvoidLKASFaultToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("AvoidLKASFaultEnabled", status);
    });
  }
};

class ToAvoidLKASFaultBeyondToggle : public ToggleControl {
  Q_OBJECT

public:
  ToAvoidLKASFaultBeyondToggle() : ToggleControl("To Avoid LKAS Fault with More Steer", "This is just in case you are using other panda setting.(delta up&down, maxsteer, rtdelta and etc).", "../assets/offroad/icon_shell.png", Params().getBool("AvoidLKASFaultBeyond")) {
    QObject::connect(this, &ToAvoidLKASFaultBeyondToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("AvoidLKASFaultBeyond", status);
    });
  }
};

class StockDecelonCamToggle : public ToggleControl {
  Q_OBJECT

public:
  StockDecelonCamToggle() : ToggleControl("Use Stock Decel on SaftySection", "Use stock deceleration on safety section.(the vehicle equipped with Stock Navigation)", "../assets/offroad/icon_shell.png", Params().getBool("UseStockDecelOnSS")) {
    QObject::connect(this, &StockDecelonCamToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("UseStockDecelOnSS", status);
    });
  }
};

class JoystickModeToggle : public ToggleControl {
  Q_OBJECT

public:
  JoystickModeToggle() : ToggleControl("JoyStick Debug Mode", "How to use: https://github.com/commaai/openpilot/tree/master/tools/joystick", "../assets/offroad/icon_shell.png", Params().getBool("JoystickDebugMode")) {
    QObject::connect(this, &JoystickModeToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("JoystickDebugMode", status);
    });
  }
};

class RPMAnimatedToggle : public ToggleControl {
  Q_OBJECT

public:
  RPMAnimatedToggle() : ToggleControl("RPM Animated", "Animated RPM", "../assets/offroad/icon_shell.png", Params().getBool("AnimatedRPM")) {
    QObject::connect(this, &RPMAnimatedToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("AnimatedRPM", status);
    });
  }
};

class ShowStopLineToggle : public ToggleControl {
  Q_OBJECT

public:
  ShowStopLineToggle() : ToggleControl("Show Stop Line", "Show stop line on the screen.", "../assets/offroad/icon_shell.png", Params().getBool("ShowStopLine")) {
    QObject::connect(this, &ShowStopLineToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("ShowStopLine", status);
    });
  }
};

// openpilot preview
class OpenpilotView : public AbstractControl {
  Q_OBJECT

public:
  OpenpilotView();

private:
  QPushButton btn;
  QPushButton btnc;
  Params params;
  
  void refresh();
};

class CarSelectCombo : public AbstractControl 
{
  Q_OBJECT

public:
  CarSelectCombo();

private:
  QPushButton btn;
  QComboBox combobox;
  Params params;

  void refresh();
};

class BranchSelectCombo : public AbstractControl 
{
  Q_OBJECT

public:
  BranchSelectCombo();

private:
  QComboBox combobox;
  Params params;
};

class TimeZoneSelectCombo : public AbstractControl 
{
  Q_OBJECT

public:
  TimeZoneSelectCombo();

private:
  QPushButton btn;
  QComboBox combobox;
  Params params;

  void refresh();
};


// UI
class AutoShutdown : public AbstractControl {
  Q_OBJECT

public:
  AutoShutdown();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class ForceShutdown : public AbstractControl {
  Q_OBJECT

public:
  ForceShutdown();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class AutoScreenOff : public AbstractControl {
  Q_OBJECT

public:
  AutoScreenOff();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class VolumeControl : public AbstractControl {
  Q_OBJECT

public:
  VolumeControl();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  QSoundEffect effect;
  
  void refresh();
  void playsound();
};

class BrightnessControl : public AbstractControl {
  Q_OBJECT

public:
  BrightnessControl();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class BrightnessOffControl : public AbstractControl {
  Q_OBJECT

public:
  BrightnessOffControl();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


class ChargingMin : public AbstractControl {
  Q_OBJECT

public:
  ChargingMin();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};
class ChargingMax : public AbstractControl {
  Q_OBJECT

public:
  ChargingMax();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


// Driving
class CruisemodeSelInit : public AbstractControl {
  Q_OBJECT

public:
  CruisemodeSelInit();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class VariableCruiseProfile : public AbstractControl {
  Q_OBJECT

public:
  VariableCruiseProfile();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class LaneChangeSpeed : public AbstractControl {
  Q_OBJECT

public:
  LaneChangeSpeed();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class LaneChangeDelay : public AbstractControl {
  Q_OBJECT

public:
  LaneChangeDelay();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class LeftCurvOffset : public AbstractControl {
  Q_OBJECT

public:
  LeftCurvOffset();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};
class RightCurvOffset : public AbstractControl {
  Q_OBJECT

public:
  RightCurvOffset();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class MaxAngleLimit : public AbstractControl {
  Q_OBJECT

public:
  MaxAngleLimit();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class SpeedLimitOffset : public AbstractControl {
  Q_OBJECT

public:
  SpeedLimitOffset();

private:
  QPushButton btn;
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

// 튜닝 설정
class CameraOffset : public AbstractControl {
  Q_OBJECT

public:
  CameraOffset();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class PathOffset : public AbstractControl {
  Q_OBJECT

public:
  PathOffset();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class SRBaseControl : public AbstractControl {
  Q_OBJECT

public:
  SRBaseControl();

private:
  QPushButton btndigit;
  QPushButton btnminus;
  QPushButton btnplus;
  QLabel label;
  Params params;
  float digit = 0.01;
  
  void refresh();
};

class SRMaxControl : public AbstractControl {
  Q_OBJECT

public:
  SRMaxControl();

private:
  QPushButton btndigit;
  QPushButton btnminus;
  QPushButton btnplus;
  QLabel label;
  Params params;
  float digit = 0.01;
  
  void refresh();
};

class SteerActuatorDelay : public AbstractControl {
  Q_OBJECT

public:
  SteerActuatorDelay();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class SteerRateCost : public AbstractControl {
  Q_OBJECT

public:
  SteerRateCost();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class SteerLimitTimer : public AbstractControl {
  Q_OBJECT

public:
  SteerLimitTimer();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class TireStiffnessFactor : public AbstractControl {
  Q_OBJECT

public:
  TireStiffnessFactor();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class SteerMax : public AbstractControl {
  Q_OBJECT

public:
  SteerMax();

private:
  QPushButton btnplusl;
  QPushButton btnminusl;
  QPushButton btnplusr;
  QPushButton btnminusr;
  QLabel labell1;
  QLabel labelr1;
  QLabel labell;
  QLabel labelr;
  Params params;
  
  void refreshl();
  void refreshr();
};

class SteerDeltaUp : public AbstractControl {
  Q_OBJECT

public:
  SteerDeltaUp();

private:
  QPushButton btnplusl;
  QPushButton btnminusl;
  QPushButton btnplusr;
  QPushButton btnminusr;
  QLabel labell1;
  QLabel labelr1;
  QLabel labell;
  QLabel labelr;
  Params params;
  
  void refreshl();
  void refreshr();
};

class SteerDeltaDown : public AbstractControl {
  Q_OBJECT

public:
  SteerDeltaDown();

private:
  QPushButton btnplusl;
  QPushButton btnminusl;
  QPushButton btnplusr;
  QPushButton btnminusr;
  QLabel labell1;
  QLabel labelr1;
  QLabel labell;
  QLabel labelr;
  Params params;
  
  void refreshl();
  void refreshr();
};

// control
class LateralControl : public AbstractControl {
  Q_OBJECT

public:
  LateralControl();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class PidKp : public AbstractControl {
  Q_OBJECT

public:
  PidKp();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class PidKi : public AbstractControl {
  Q_OBJECT

public:
  PidKi();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class PidKd : public AbstractControl {
  Q_OBJECT

public:
  PidKd();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class PidKf : public AbstractControl {
  Q_OBJECT

public:
  PidKf();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class IgnoreZone : public AbstractControl {
  Q_OBJECT

public:
  IgnoreZone();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class OuterLoopGain : public AbstractControl {
  Q_OBJECT

public:
  OuterLoopGain();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class InnerLoopGain : public AbstractControl {
  Q_OBJECT

public:
  InnerLoopGain();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class TimeConstant : public AbstractControl {
  Q_OBJECT

public:
  TimeConstant();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class ActuatorEffectiveness : public AbstractControl {
  Q_OBJECT

public:
  ActuatorEffectiveness();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class Scale : public AbstractControl {
  Q_OBJECT

public:
  Scale();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class LqrKi : public AbstractControl {
  Q_OBJECT

public:
  LqrKi();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class DcGain : public AbstractControl {
  Q_OBJECT

public:
  DcGain();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class TorqueKp : public AbstractControl {
  Q_OBJECT

public:
  TorqueKp();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class TorqueKf : public AbstractControl {
  Q_OBJECT

public:
  TorqueKf();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class TorqueKi : public AbstractControl {
  Q_OBJECT

public:
  TorqueKi();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class SteerAngleCorrection : public AbstractControl {
  Q_OBJECT

public:
  SteerAngleCorrection();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class MaxSteer : public AbstractControl {
  Q_OBJECT

public:
  MaxSteer();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class MaxRTDelta : public AbstractControl {
  Q_OBJECT

public:
  MaxRTDelta();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class MaxRateUp : public AbstractControl {
  Q_OBJECT

public:
  MaxRateUp();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class MaxRateDown : public AbstractControl {
  Q_OBJECT

public:
  MaxRateDown();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class SteerThreshold : public AbstractControl {
  Q_OBJECT

public:
  SteerThreshold();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class RecordCount : public AbstractControl {
  Q_OBJECT

public:
  RecordCount();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class RecordQuality : public AbstractControl {
  Q_OBJECT

public:
  RecordQuality();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class GitHash : public AbstractControl {
  Q_OBJECT

public:
  GitHash();

private:
  QLabel local_hash;
  QLabel remote_hash;
  Params params;
};

class RESChoice : public AbstractControl {
  Q_OBJECT

public:
  RESChoice();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class MonitoringMode : public AbstractControl {
  Q_OBJECT

public:
  MonitoringMode();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class MonitorEyesThreshold : public AbstractControl {
  Q_OBJECT

public:
  MonitorEyesThreshold();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class NormalEyesThreshold : public AbstractControl {
  Q_OBJECT

public:
  NormalEyesThreshold();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class BlinkThreshold : public AbstractControl {
  Q_OBJECT

public:
  BlinkThreshold();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class FanSpeedGain : public AbstractControl {
  Q_OBJECT

public:
  FanSpeedGain();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class CruiseGapTR : public AbstractControl {
  Q_OBJECT

public:
  CruiseGapTR();

private:
  QPushButton btn1;
  QPushButton btn2;
  QPushButton btn3;
  QPushButton btn4;
  QLabel label1;
  QLabel label2;
  QLabel label3;
  QLabel label4;
  QLabel label1a;
  QLabel label2a;
  QLabel label3a;
  QLabel label4a;
  Params params;
  
  void refresh1();
  void refresh2();
  void refresh3();
  void refresh4();
};

class DynamicTRGap : public AbstractControl {
  Q_OBJECT

public:
  DynamicTRGap();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class DynamicTRUD : public AbstractControl {
  Q_OBJECT

public:
  DynamicTRUD();
};

class LCTimingFactor : public AbstractControl {
  Q_OBJECT

public:
  LCTimingFactor();

private:
  QPushButton btn1;
  QPushButton btn2;
  QPushButton btn3;
  QPushButton btn4;
  QLabel label1;
  QLabel label2;
  QLabel label3;
  QLabel label4;
  QLabel label1a;
  QLabel label2a;
  QLabel label3a;
  QLabel label4a;
  Params params;

  void refresh1();
  void refresh2();
  void refresh3();
  void refresh4();
};

class LCTimingFactorUD : public AbstractControl {
  Q_OBJECT

public:
  LCTimingFactorUD();

private:
  QPushButton btn;
  QPushButton btn2;
  Params params;
  
  void refresh();
  void refresh2();
};

class AutoResCondition : public AbstractControl {
  Q_OBJECT

public:
  AutoResCondition();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class AutoResLimitTime : public AbstractControl {
  Q_OBJECT

public:
  AutoResLimitTime();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class AutoEnableSpeed : public AbstractControl {
  Q_OBJECT

public:
  AutoEnableSpeed();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class CamDecelDistAdd : public AbstractControl {
  Q_OBJECT

public:
  CamDecelDistAdd();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class LiveSRPercent : public AbstractControl {
  Q_OBJECT

public:
  LiveSRPercent();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class VCurvSpeedUD : public AbstractControl {
  Q_OBJECT

public:
  VCurvSpeedUD();
};

class VCurvSpeed : public AbstractControl {
  Q_OBJECT

public:
  VCurvSpeed();

private:
  QPushButton btn;
  QLineEdit edit1;
  QLineEdit edit2;
  Params params;

  void refresh();
};

class OCurvSpeedUD : public AbstractControl {
  Q_OBJECT

public:
  OCurvSpeedUD();
};

class OCurvSpeed : public AbstractControl {
  Q_OBJECT

public:
  OCurvSpeed();

private:
  QPushButton btn;
  QLineEdit edit1;
  QLineEdit edit2;
  Params params;

  void refresh();
};

class GetOffAlert : public AbstractControl {
  Q_OBJECT

public:
  GetOffAlert();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class OPKRNaviSelect : public AbstractControl {
  Q_OBJECT

public:
  OPKRNaviSelect();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class OPKRServerSelect : public AbstractControl {
  Q_OBJECT

public:
  OPKRServerSelect();

private:
  QPushButton btn1;
  QPushButton btn2;
  QPushButton btn3;
  Params params;
  
  void refresh();
};

class OPKRServerAPI : public AbstractControl {
  Q_OBJECT

public:
  OPKRServerAPI();

private:
  QLabel label;
  QPushButton btn;
  Params params;

  void refresh();
};

class OPKRMapboxStyle : public AbstractControl {
  Q_OBJECT

public:
  OPKRMapboxStyle();

private:
  QPushButton btn1;
  QPushButton btn2;
  QPushButton btn3;
  Params params;
  
  void refresh();
};

class RESCountatStandstill : public AbstractControl {
  Q_OBJECT

public:
  RESCountatStandstill();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class SpeedLimitSignType : public AbstractControl {
  Q_OBJECT

public:
  SpeedLimitSignType();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class RadarLongHelperOption : public AbstractControl {
  Q_OBJECT

public:
  RadarLongHelperOption();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class CurvDecelSelect : public AbstractControl {
  Q_OBJECT

public:
  CurvDecelSelect();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class AutoRESDelay : public AbstractControl {
  Q_OBJECT

public:
  AutoRESDelay();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class OSMCustomSpeedLimitUD : public AbstractControl {
  Q_OBJECT

public:
  OSMCustomSpeedLimitUD();
};

class OSMCustomSpeedLimit : public AbstractControl {
  Q_OBJECT

public:
  OSMCustomSpeedLimit();

private:
  QPushButton btn;
  QLineEdit edit1;
  QLineEdit edit2;
  Params params;

  void refresh();
};

class DesiredCurvatureLimit : public AbstractControl {
  Q_OBJECT

public:
  DesiredCurvatureLimit();

private:
  QPushButton btndigit;
  QPushButton btnminus;
  QPushButton btnplus;
  QLabel label;
  Params params;
  float digit = 0.01;
  
  void refresh();
};

class DynamicTRBySpeed : public AbstractControl {
  Q_OBJECT

public:
  DynamicTRBySpeed();

private:
  QPushButton btn;
  QLineEdit edit1;
  QLineEdit edit2;
  Params params;

  void refresh();
};

class LaneWidth : public AbstractControl {
  Q_OBJECT

public:
  LaneWidth();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class SpeedLaneWidthUD : public AbstractControl {
  Q_OBJECT

public:
  SpeedLaneWidthUD();
};

class SpeedLaneWidth : public AbstractControl {
  Q_OBJECT

public:
  SpeedLaneWidth();

private:
  QPushButton btn;
  QLineEdit edit1;
  QLineEdit edit2;
  Params params;

  void refresh();
};

class OPKRTopTextView : public AbstractControl {
  Q_OBJECT

public:
  OPKRTopTextView();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class OPKREdgeOffset : public AbstractControl {
  Q_OBJECT

public:
  OPKREdgeOffset();

private:
  QPushButton btnplusl;
  QPushButton btnminusl;
  QPushButton btnplusr;
  QPushButton btnminusr;
  QLabel labell1;
  QLabel labelr1;
  QLabel labell;
  QLabel labelr;
  Params params;
  
  void refreshl();
  void refreshr();
};

class ToAvoidLKASFault : public AbstractControl {
  Q_OBJECT

public:
  ToAvoidLKASFault();

private:
  QPushButton btnplusl;
  QPushButton btnminusl;
  QPushButton btnplusr;
  QPushButton btnminusr;
  QLabel labell1;
  QLabel labelr1;
  QLabel labell;
  QLabel labelr;
  Params params;
  
  void refreshl();
  void refreshr();
};

class RoutineDriveOption : public AbstractControl {
  Q_OBJECT

public:
  RoutineDriveOption();

private:
  QPushButton btn0;
  QPushButton btn1;
  Params params;
  
  void refresh();
};

class RPMAnimatedMaxValue : public AbstractControl {
  Q_OBJECT

public:
  RPMAnimatedMaxValue();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};
