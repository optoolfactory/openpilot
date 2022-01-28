#include "selfdrive/ui/qt/home.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QProcess> // opkr
#include <QSoundEffect> // opkr

#include "selfdrive/common/params.h"
#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/widgets/drive_stats.h"
#include "selfdrive/ui/qt/widgets/prime.h"

// HomeWindow: the container for the offroad and onroad UIs

HomeWindow::HomeWindow(QWidget* parent) : QWidget(parent) {
  QHBoxLayout *main_layout = new QHBoxLayout(this);
  main_layout->setMargin(0);
  main_layout->setSpacing(0);

  sidebar = new Sidebar(this);
  main_layout->addWidget(sidebar);
  QObject::connect(sidebar, &Sidebar::openSettings, this, &HomeWindow::openSettings);

  slayout = new QStackedLayout();
  main_layout->addLayout(slayout);

  home = new OffroadHome();
  slayout->addWidget(home);

  onroad = new OnroadWindow(this);
  slayout->addWidget(onroad);

  driver_view = new DriverViewWindow(this);
  connect(driver_view, &DriverViewWindow::done, [=] {
    showDriverView(false);
  });
  slayout->addWidget(driver_view);
  setAttribute(Qt::WA_NoSystemBackground);
  QObject::connect(uiState(), &UIState::offroadTransition, this, &HomeWindow::offroadTransition);
}

void HomeWindow::showSidebar(bool show) {
  sidebar->setVisible(show);
}

void HomeWindow::offroadTransition(bool offroad) {
  sidebar->setVisible(offroad);
  if (offroad) {
    slayout->setCurrentWidget(home);
  } else {
    slayout->setCurrentWidget(onroad);
  }
}

void HomeWindow::showDriverView(bool show) {
  if (show) {
    emit closeSettings();
    slayout->setCurrentWidget(driver_view);
  } else {
    slayout->setCurrentWidget(home);
  }
  sidebar->setVisible(show == false);
}

void HomeWindow::mousePressEvent(QMouseEvent* e) {
  // OPKR add map
  if (uiState()->scene.started && map_overlay_btn.ptInRect(e->x(), e->y())) {
    QSoundEffect effect1;
    effect1.setSource(QUrl::fromLocalFile("/data/openpilot/selfdrive/assets/addon/sound/click.wav"));
    //effect1.setLoopCount(1);
    //effect1.setLoopCount(QSoundEffect::Infinite);
    float volume1 = 0.5;
    if (uiState()->scene.nVolumeBoost < 0) {
      volume1 = 0.0;
    } else if (uiState()->scene.nVolumeBoost > 1) {
      volume1 = uiState()->scene.nVolumeBoost * 0.01;
    }
    effect1.setVolume(volume1);
    effect1.play();
    if (!uiState()->scene.mapbox_running) {
      QProcess::execute("am start --activity-task-on-home com.opkr.maphack/com.opkr.maphack.MainActivity");
    } else if (uiState()->scene.mapbox_running && !uiState()->scene.map_on_top && uiState()->scene.map_on_overlay) {
      Params().remove("NavDestination");
    } else {
      QProcess::execute("pkill com.android.chrome");
      QProcess::execute("rm -rf /data/data/com.android.chrome/app_tabs/0");
    }
    uiState()->scene.map_on_top = false;
    uiState()->scene.map_on_overlay = true;
    return;
  }
  if (uiState()->scene.started && !sidebar->isVisible() && !uiState()->scene.map_on_top && map_btn.ptInRect(e->x(), e->y()) && !uiState()->scene.mapbox_running) {
    QSoundEffect effect2;
    effect2.setSource(QUrl::fromLocalFile("/data/openpilot/selfdrive/assets/addon/sound/click.wav"));
    //effect1.setLoopCount(1);
    //effect1.setLoopCount(QSoundEffect::Infinite);
    float volume2 = 0.5;
    if (uiState()->scene.nVolumeBoost < 0) {
      volume2 = 0.0;
    } else if (uiState()->scene.nVolumeBoost > 1) {
      volume2 = uiState()->scene.nVolumeBoost * 0.01;
    }
    effect2.setVolume(volume2);
    effect2.play();
    uiState()->scene.map_is_running = !uiState()->scene.map_is_running;
    if (uiState()->scene.map_is_running) {
      if (uiState()->scene.navi_select == 0) {
        QProcess::execute("am start com.mnsoft.mappyobn/com.mnsoft.mappy.MainActivity");
      } else if (uiState()->scene.navi_select == 1) {
        QProcess::execute("am start com.waze/com.waze.MainActivity");
      }
      uiState()->scene.map_on_top = true;
      uiState()->scene.map_is_running = true;
      uiState()->scene.map_on_overlay = false;
      Params().putBool("OpkrMapEnable", true);
    } else {
      if (uiState()->scene.navi_select == 0) {
        QProcess::execute("pkill com.mnsoft.mappyobn");
      } else if (uiState()->scene.navi_select == 1) {
        QProcess::execute("pkill com.waze");
      }
      uiState()->scene.map_on_top = false;
      uiState()->scene.map_on_overlay = false;
      uiState()->scene.map_is_running = false;
      Params().putBool("OpkrMapEnable", false);
    }
    return;
  }
  if (uiState()->scene.started && !sidebar->isVisible() && !uiState()->scene.map_on_top && mapbox_btn.ptInRect(e->x(), e->y()) && uiState()->scene.mapbox_running) {
    QSoundEffect effect4;
    effect4.setSource(QUrl::fromLocalFile("/data/openpilot/selfdrive/assets/addon/sound/click.wav"));
    //effect1.setLoopCount(1);
    //effect1.setLoopCount(QSoundEffect::Infinite);
    float volume2 = 0.5;
    if (uiState()->scene.nVolumeBoost < 0) {
      volume2 = 0.0;
    } else if (uiState()->scene.nVolumeBoost > 1) {
      volume2 = uiState()->scene.nVolumeBoost * 0.01;
    }
    effect4.setVolume(volume2);
    effect4.play();
    QProcess::execute("am start -n com.android.chrome/org.chromium.chrome.browser.ChromeTabbedActivity -d \"http://localhost:8082\" --activity-clear-task");
    uiState()->scene.map_on_top = true;
    uiState()->scene.map_on_overlay = false;
    return;
  }
  if (uiState()->scene.started && uiState()->scene.map_is_running && map_return_btn.ptInRect(e->x(), e->y()) && !uiState()->scene.mapbox_running) {
    QSoundEffect effect3;
    effect3.setSource(QUrl::fromLocalFile("/data/openpilot/selfdrive/assets/addon/sound/click.wav"));
    //effect1.setLoopCount(1);
    //effect1.setLoopCount(QSoundEffect::Infinite);
    float volume3 = 0.5;
    if (uiState()->scene.nVolumeBoost < 0) {
      volume3 = 0.0;
    } else if (uiState()->scene.nVolumeBoost > 1) {
      volume3 = uiState()->scene.nVolumeBoost * 0.01;
    }
    effect3.setVolume(volume3);
    effect3.play();
    if (uiState()->scene.navi_select == 0) {
      QProcess::execute("am start --activity-task-on-home com.mnsoft.mappyobn/com.mnsoft.mappy.MainActivity");
    } else if (uiState()->scene.navi_select == 1) {
      QProcess::execute("am start --activity-task-on-home com.waze/com.waze.MainActivity");
    }
    uiState()->scene.map_on_top = true;
    uiState()->scene.map_on_overlay = false;
    return;
  }
  // OPKR REC
  if (uiState()->scene.started && !sidebar->isVisible() && !uiState()->scene.map_on_top && !uiState()->scene.comma_stock_ui && rec_btn.ptInRect(e->x(), e->y()) && !uiState()->scene.mapbox_running) {
    uiState()->scene.touched = true;
    return;
  }
  // Laneless mode
  if (uiState()->scene.started && !sidebar->isVisible() && !uiState()->scene.map_on_top && uiState()->scene.end_to_end && !uiState()->scene.comma_stock_ui && laneless_btn.ptInRect(e->x(), e->y()) && !uiState()->scene.mapbox_running) {
    uiState()->scene.laneless_mode = uiState()->scene.laneless_mode + 1;
    if (uiState()->scene.laneless_mode > 2) {
      uiState()->scene.laneless_mode = 0;
    }
    if (uiState()->scene.laneless_mode == 0) {
      Params().put("LanelessMode", "0", 1);
    } else if (uiState()->scene.laneless_mode == 1) {
      Params().put("LanelessMode", "1", 1);
    } else if (uiState()->scene.laneless_mode == 2) {
      Params().put("LanelessMode", "2", 1);
    }
    return;
  }
  // Monitoring mode
  if (uiState()->scene.started && !sidebar->isVisible() && !uiState()->scene.map_on_top && monitoring_btn.ptInRect(e->x(), e->y()) && !uiState()->scene.mapbox_running) {
    uiState()->scene.monitoring_mode = !uiState()->scene.monitoring_mode;
    if (uiState()->scene.monitoring_mode) {
      Params().putBool("OpkrMonitoringMode", true);
    } else {
      Params().putBool("OpkrMonitoringMode", false);
    }
    return;
  }
  // Stock UI Toggle
  if (uiState()->scene.started && !sidebar->isVisible() && !uiState()->scene.map_on_top && stockui_btn.ptInRect(e->x(), e->y())) {
    uiState()->scene.comma_stock_ui = !uiState()->scene.comma_stock_ui;
    if (uiState()->scene.comma_stock_ui) {
      Params().putBool("CommaStockUI", true);
    } else {
      Params().putBool("CommaStockUI", false);
    }
    return;
  }
  // LiveTune UI Toggle
  if (uiState()->scene.started && !sidebar->isVisible() && !uiState()->scene.map_on_top && tuneui_btn.ptInRect(e->x(), e->y()) && !uiState()->scene.mapbox_running) {
    uiState()->scene.opkr_livetune_ui = !uiState()->scene.opkr_livetune_ui;
    if (uiState()->scene.opkr_livetune_ui) {
      Params().putBool("OpkrLiveTunePanelEnable", true);
      uiState()->scene.live_tune_panel_enable = true;
    } else {
      Params().putBool("OpkrLiveTunePanelEnable", false);
      uiState()->scene.live_tune_panel_enable = false;
    }
    return;
  }
  // SpeedLimit Decel on/off Toggle
  if (uiState()->scene.started && !sidebar->isVisible() && !uiState()->scene.map_on_top && speedlimit_btn.ptInRect(e->x(), e->y())) {
    uiState()->scene.sl_decel_off = !uiState()->scene.sl_decel_off;
    if (uiState()->scene.sl_decel_off) {
      Params().putBool("SpeedLimitDecelOff", true);
    } else {
      Params().putBool("SpeedLimitDecelOff", false);
    }
    return;
  }
  // opkr live ui tune
  if (uiState()->scene.live_tune_panel_enable) {
    if (uiState()->scene.started && !sidebar->isVisible() && !uiState()->scene.map_on_top && livetunepanel_left_btn.ptInRect(e->x(), e->y())) {
      if (uiState()->scene.live_tune_panel_list == 0) {
        uiState()->scene.cameraOffset = uiState()->scene.cameraOffset - 5;
        if (uiState()->scene.cameraOffset <= -1000) uiState()->scene.cameraOffset = -1000;
        QString value = QString::number(uiState()->scene.cameraOffset);
        Params().put("CameraOffsetAdj", value.toStdString());
        return;
      }
      if (uiState()->scene.live_tune_panel_list == 1) {
        uiState()->scene.pathOffset = uiState()->scene.pathOffset - 5;
        if (uiState()->scene.pathOffset <= -1000) uiState()->scene.pathOffset = -1000;
        QString value = QString::number(uiState()->scene.pathOffset);
        Params().put("PathOffsetAdj", value.toStdString());
        return;
      }
      if (uiState()->scene.live_tune_panel_list == 2) {
        uiState()->scene.osteerRateCost = uiState()->scene.osteerRateCost - 1;
        if (uiState()->scene.osteerRateCost <= 1) uiState()->scene.osteerRateCost = 1;
        QString value = QString::number(uiState()->scene.osteerRateCost);
        Params().put("SteerRateCostAdj", value.toStdString());
        return;
      }
      if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+0) && uiState()->scene.lateralControlMethod == 0) {
        uiState()->scene.pidKp = uiState()->scene.pidKp - 1;
        if (uiState()->scene.pidKp <= 1) uiState()->scene.pidKp = 1;
        QString value = QString::number(uiState()->scene.pidKp);
        Params().put("PidKp", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+1) && uiState()->scene.lateralControlMethod == 0) {
        uiState()->scene.pidKi = uiState()->scene.pidKi - 1;
        if (uiState()->scene.pidKi <= 1) uiState()->scene.pidKi = 1;
        QString value = QString::number(uiState()->scene.pidKi);
        Params().put("PidKi", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+2) && uiState()->scene.lateralControlMethod == 0) {
        uiState()->scene.pidKd = uiState()->scene.pidKd - 5;
        if (uiState()->scene.pidKd <= 0) uiState()->scene.pidKd = 0;
        QString value = QString::number(uiState()->scene.pidKd);
        Params().put("PidKd", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+3) && uiState()->scene.lateralControlMethod == 0) {
        uiState()->scene.pidKf = uiState()->scene.pidKf - 1;
        if (uiState()->scene.pidKf <= 1) uiState()->scene.pidKf = 1;
        QString value = QString::number(uiState()->scene.pidKf);
        Params().put("PidKf", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+0) && uiState()->scene.lateralControlMethod == 1) {
        uiState()->scene.indiInnerLoopGain = uiState()->scene.indiInnerLoopGain - 1;
        if (uiState()->scene.indiInnerLoopGain <= 1) uiState()->scene.indiInnerLoopGain = 1;
        QString value = QString::number(uiState()->scene.indiInnerLoopGain);
        Params().put("InnerLoopGain", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+1) && uiState()->scene.lateralControlMethod == 1) {
        uiState()->scene.indiOuterLoopGain = uiState()->scene.indiOuterLoopGain - 1;
        if (uiState()->scene.indiOuterLoopGain <= 1) uiState()->scene.indiOuterLoopGain = 1;
        QString value = QString::number(uiState()->scene.indiOuterLoopGain);
        Params().put("OuterLoopGain", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+2) && uiState()->scene.lateralControlMethod == 1) {
        uiState()->scene.indiTimeConstant = uiState()->scene.indiTimeConstant - 1;
        if (uiState()->scene.indiTimeConstant <= 1) uiState()->scene.indiTimeConstant = 1;
        QString value = QString::number(uiState()->scene.indiTimeConstant);
        Params().put("TimeConstant", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+3) && uiState()->scene.lateralControlMethod == 1) {
        uiState()->scene.indiActuatorEffectiveness = uiState()->scene.indiActuatorEffectiveness - 1;
        if (uiState()->scene.indiActuatorEffectiveness <= 1) uiState()->scene.indiActuatorEffectiveness = 1;
        QString value = QString::number(uiState()->scene.indiActuatorEffectiveness);
        Params().put("ActuatorEffectiveness", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+0) && uiState()->scene.lateralControlMethod == 2) {
        uiState()->scene.lqrScale = uiState()->scene.lqrScale - 50;
        if (uiState()->scene.lqrScale <= 50) uiState()->scene.lqrScale = 50;
        QString value = QString::number(uiState()->scene.lqrScale);
        Params().put("Scale", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+1) && uiState()->scene.lateralControlMethod == 2) {
        uiState()->scene.lqrKi = uiState()->scene.lqrKi - 1;
        if (uiState()->scene.lqrKi <= 1) uiState()->scene.lqrKi = 1;
        QString value = QString::number(uiState()->scene.lqrKi);
        Params().put("LqrKi", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+2) && uiState()->scene.lateralControlMethod == 2) {
        uiState()->scene.lqrDcGain = uiState()->scene.lqrDcGain - 5;
        if (uiState()->scene.lqrDcGain <= 5) uiState()->scene.lqrDcGain = 5;
        QString value = QString::number(uiState()->scene.lqrDcGain);
        Params().put("DcGain", value.toStdString());
        return;
      }
    }
    if (uiState()->scene.started && !sidebar->isVisible() && !uiState()->scene.map_on_top && livetunepanel_right_btn.ptInRect(e->x(), e->y())) {
      if (uiState()->scene.live_tune_panel_list == 0) {
        uiState()->scene.cameraOffset = uiState()->scene.cameraOffset + 5;
        if (uiState()->scene.cameraOffset >= 1000) uiState()->scene.cameraOffset = 1000;
        QString value = QString::number(uiState()->scene.cameraOffset);
        Params().put("CameraOffsetAdj", value.toStdString());
        return;
      }
      if (uiState()->scene.live_tune_panel_list == 1) {
        uiState()->scene.pathOffset = uiState()->scene.pathOffset + 5;
        if (uiState()->scene.pathOffset >= 1000) uiState()->scene.pathOffset = 1000;
        QString value = QString::number(uiState()->scene.pathOffset);
        Params().put("PathOffsetAdj", value.toStdString());
        return;
      }
      if (uiState()->scene.live_tune_panel_list == 2) {
        uiState()->scene.osteerRateCost = uiState()->scene.osteerRateCost + 1;
        if (uiState()->scene.osteerRateCost >= 200) uiState()->scene.osteerRateCost = 200;
        QString value = QString::number(uiState()->scene.osteerRateCost);
        Params().put("SteerRateCostAdj", value.toStdString());
        return;
      }
      if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+0) && uiState()->scene.lateralControlMethod == 0) {
        uiState()->scene.pidKp = uiState()->scene.pidKp + 1;
        if (uiState()->scene.pidKp >= 50) uiState()->scene.pidKp = 50;
        QString value = QString::number(uiState()->scene.pidKp);
        Params().put("PidKp", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+1) && uiState()->scene.lateralControlMethod == 0) {
        uiState()->scene.pidKi = uiState()->scene.pidKi + 1;
        if (uiState()->scene.pidKi >= 100) uiState()->scene.pidKi = 100;
        QString value = QString::number(uiState()->scene.pidKi);
        Params().put("PidKi", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+2) && uiState()->scene.lateralControlMethod == 0) {
        uiState()->scene.pidKd = uiState()->scene.pidKd + 5;
        if (uiState()->scene.pidKd >= 300) uiState()->scene.pidKd = 300;
        QString value = QString::number(uiState()->scene.pidKd);
        Params().put("PidKd", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+3) && uiState()->scene.lateralControlMethod == 0) {
        uiState()->scene.pidKf = uiState()->scene.pidKf + 1;
        if (uiState()->scene.pidKf >= 50) uiState()->scene.pidKf = 50;
        QString value = QString::number(uiState()->scene.pidKf);
        Params().put("PidKf", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+0) && uiState()->scene.lateralControlMethod == 1) {
        uiState()->scene.indiInnerLoopGain = uiState()->scene.indiInnerLoopGain + 1;
        if (uiState()->scene.indiInnerLoopGain >= 200) uiState()->scene.indiInnerLoopGain = 200;
        QString value = QString::number(uiState()->scene.indiInnerLoopGain);
        Params().put("InnerLoopGain", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+1) && uiState()->scene.lateralControlMethod == 1) {
        uiState()->scene.indiOuterLoopGain = uiState()->scene.indiOuterLoopGain + 1;
        if (uiState()->scene.indiOuterLoopGain >= 200) uiState()->scene.indiOuterLoopGain = 200;
        QString value = QString::number(uiState()->scene.indiOuterLoopGain);
        Params().put("OuterLoopGain", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+2) && uiState()->scene.lateralControlMethod == 1) {
        uiState()->scene.indiTimeConstant = uiState()->scene.indiTimeConstant + 1;
        if (uiState()->scene.indiTimeConstant >= 200) uiState()->scene.indiTimeConstant = 200;
        QString value = QString::number(uiState()->scene.indiTimeConstant);
        Params().put("TimeConstant", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+3) && uiState()->scene.lateralControlMethod == 1) {
        uiState()->scene.indiActuatorEffectiveness = uiState()->scene.indiActuatorEffectiveness + 1;
        if (uiState()->scene.indiActuatorEffectiveness >= 200) uiState()->scene.indiActuatorEffectiveness = 200;
        QString value = QString::number(uiState()->scene.indiActuatorEffectiveness);
        Params().put("ActuatorEffectiveness", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+0) && uiState()->scene.lateralControlMethod == 2) {
        uiState()->scene.lqrScale = uiState()->scene.lqrScale + 50;
        if (uiState()->scene.lqrScale >= 5000) uiState()->scene.lqrScale = 5000;
        QString value = QString::number(uiState()->scene.lqrScale);
        Params().put("Scale", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+1) && uiState()->scene.lateralControlMethod == 2) {
        uiState()->scene.lqrKi = uiState()->scene.lqrKi + 1;
        if (uiState()->scene.lqrKi >= 100) uiState()->scene.lqrKi = 100;
        QString value = QString::number(uiState()->scene.lqrKi);
        Params().put("LqrKi", value.toStdString());
        return;
      } else if (uiState()->scene.live_tune_panel_list == (uiState()->scene.list_count+2) && uiState()->scene.lateralControlMethod == 2) {
        uiState()->scene.lqrDcGain = uiState()->scene.lqrDcGain + 5;
        if (uiState()->scene.lqrDcGain >= 500) uiState()->scene.lqrDcGain = 500;
        QString value = QString::number(uiState()->scene.lqrDcGain);
        Params().put("DcGain", value.toStdString());
        return;
      }
    }
    if (uiState()->scene.started && !sidebar->isVisible() && !uiState()->scene.map_on_top && livetunepanel_left_above_btn.ptInRect(e->x(), e->y())) {
      uiState()->scene.live_tune_panel_list = uiState()->scene.live_tune_panel_list - 1;
      if (uiState()->scene.lateralControlMethod == 2 && uiState()->scene.live_tune_panel_list < 0) {
        uiState()->scene.live_tune_panel_list = uiState()->scene.list_count+2;
      } else if (uiState()->scene.live_tune_panel_list < 0) {
        uiState()->scene.live_tune_panel_list = uiState()->scene.list_count+3;
      }
      return;
    }
    if (uiState()->scene.started && !sidebar->isVisible() && !uiState()->scene.map_on_top && livetunepanel_right_above_btn.ptInRect(e->x(), e->y())) {
      uiState()->scene.live_tune_panel_list = uiState()->scene.live_tune_panel_list + 1;
      if (uiState()->scene.lateralControlMethod == 2 && uiState()->scene.live_tune_panel_list > (uiState()->scene.list_count+2)) {
        uiState()->scene.live_tune_panel_list = 0;
      } else if (uiState()->scene.live_tune_panel_list > (uiState()->scene.list_count+3)) {
        uiState()->scene.live_tune_panel_list = 0;
      }
      return;
    }
  }
  // Handle sidebar collapsing
  if (onroad->isVisible() && (!sidebar->isVisible() || e->x() > sidebar->width())) {
    sidebar->setVisible(!sidebar->isVisible() && !onroad->isMapVisible());
    uiState()->sidebar_view = !uiState()->sidebar_view;
  }

  if (uiState()->scene.started && uiState()->scene.autoScreenOff != -2) {
    uiState()->scene.touched2 = true;
    QTimer::singleShot(500, []() { uiState()->scene.touched2 = false; });
  }
}

// OffroadHome: the offroad home page

OffroadHome::OffroadHome(QWidget* parent) : QFrame(parent) {
  QVBoxLayout* main_layout = new QVBoxLayout(this);
  main_layout->setContentsMargins(40, 40, 40, 45);

  // top header
  QHBoxLayout* header_layout = new QHBoxLayout();
  header_layout->setContentsMargins(15, 15, 15, 0);
  header_layout->setSpacing(16);

  date = new QLabel();
  header_layout->addWidget(date, 1, Qt::AlignHCenter | Qt::AlignLeft);

  update_notif = new QPushButton("UPDATE");
  update_notif->setVisible(false);
  update_notif->setStyleSheet("background-color: #364DEF;");
  QObject::connect(update_notif, &QPushButton::clicked, [=]() { center_layout->setCurrentIndex(1); });
  header_layout->addWidget(update_notif, 0, Qt::AlignHCenter | Qt::AlignRight);

  alert_notif = new QPushButton();
  alert_notif->setVisible(false);
  alert_notif->setStyleSheet("background-color: #E22C2C;");
  QObject::connect(alert_notif, &QPushButton::clicked, [=] { center_layout->setCurrentIndex(2); });
  header_layout->addWidget(alert_notif, 0, Qt::AlignHCenter | Qt::AlignRight);

  header_layout->addWidget(new QLabel(getBrandVersion()), 0, Qt::AlignHCenter | Qt::AlignRight);

  main_layout->addLayout(header_layout);

  // main content
  main_layout->addSpacing(25);
  center_layout = new QStackedLayout();

  QWidget* statsAndSetupWidget = new QWidget(this);
  QHBoxLayout* statsAndSetup = new QHBoxLayout(statsAndSetupWidget);
  statsAndSetup->setMargin(0);
  statsAndSetup->setSpacing(30);
  statsAndSetup->addWidget(new DriveStats, 1);
  statsAndSetup->addWidget(new SetupWidget);

  center_layout->addWidget(statsAndSetupWidget);

  // add update & alerts widgets
  update_widget = new UpdateAlert();
  QObject::connect(update_widget, &UpdateAlert::dismiss, [=]() { center_layout->setCurrentIndex(0); });
  center_layout->addWidget(update_widget);
  alerts_widget = new OffroadAlert();
  QObject::connect(alerts_widget, &OffroadAlert::dismiss, [=]() { center_layout->setCurrentIndex(0); });
  center_layout->addWidget(alerts_widget);

  main_layout->addLayout(center_layout, 1);

  // set up refresh timer
  timer = new QTimer(this);
  timer->callOnTimeout(this, &OffroadHome::refresh);

  setStyleSheet(R"(
    * {
     color: white;
    }
    OffroadHome {
      background-color: black;
    }
    OffroadHome > QPushButton {
      padding: 15px 30px;
      border-radius: 5px;
      font-size: 40px;
      font-weight: 500;
    }
    OffroadHome > QLabel {
      font-size: 55px;
    }
  )");
}

void OffroadHome::showEvent(QShowEvent *event) {
  refresh();
  timer->start(10 * 1000);
}

void OffroadHome::hideEvent(QHideEvent *event) {
  timer->stop();
}

void OffroadHome::refresh() {
  date->setText(QDateTime::currentDateTime().toString("dddd, MMMM d, hh:mm"));

  bool updateAvailable = update_widget->refresh();
  int alerts = alerts_widget->refresh();

  // pop-up new notification
  int idx = center_layout->currentIndex();
  if (!updateAvailable && !alerts) {
    idx = 0;
  } else if (updateAvailable && (!update_notif->isVisible() || (!alerts && idx == 2))) {
    idx = 1;
  } else if (alerts && (!alert_notif->isVisible() || (!updateAvailable && idx == 1))) {
    idx = 2;
  }
  center_layout->setCurrentIndex(idx);

  update_notif->setVisible(updateAvailable);
  alert_notif->setVisible(alerts);
  if (alerts) {
    alert_notif->setText(QString::number(alerts) + (alerts > 1 ? " ALERTS" : " ALERT"));
  }
}
