#include "selfdrive/ui/qt/widgets/ssh_keys.h"

#include "selfdrive/common/params.h"
#include "selfdrive/ui/qt/api.h"
#include "selfdrive/ui/qt/widgets/input.h"

SshControl::SshControl() : ButtonControl("SSH 키 설정", "", "경고: 이렇게 하면 GitHub 설정의 모든 공개 키에 대한 SSH 액세스 권한이 부여됩니다. 사용자 이외의 GitHub 사용자 이름을 입력하지 마십시오. 콤마 직원은 절대 GitHub 사용자 이름을 추가하라는 요청을 하지 않습니다.") {
  username_label.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  username_label.setStyleSheet("color: #aaaaaa");
  hlayout->insertWidget(1, &username_label);

  QObject::connect(this, &ButtonControl::clicked, [=]() {
    if (text() == "ADD") {
      QString username = InputDialog::getText("GitHub 아이디를 입력하세요", this);
      if (username.length() > 0) {
        setText("LOADING");
        setEnabled(false);
        getUserKeys(username);
      }
    } else {
      params.remove("GithubUsername");
      params.remove("GithubSshKeys");
      params.put("OpkrSSHLegacy", "0", 1);
      refresh();
    }
  });

  refresh();
}

void SshControl::refresh() {
  QString param = QString::fromStdString(params.get("GithubSshKeys"));
  QString isUsername = QString::fromStdString(params.get("GithubUsername"));
  bool legacy_stat = params.getBool("OpkrSSHLegacy");
  if (param.length()) {
    if (isUsername.length()) {
      username_label.setText(QString::fromStdString(params.get("GithubUsername")));
    } else if (legacy_stat) {
      username_label.setText("공개KEY 사용중");
    }
    setText("REMOVE");
  } else {
    username_label.setText("");
    setText("ADD");
  }
  setEnabled(true);
}

void SshControl::getUserKeys(const QString &username) {
  HttpRequest *request = new HttpRequest(this, false);
  QObject::connect(request, &HttpRequest::requestDone, [=](const QString &resp, bool success) {
    if (success) {
      if (!resp.isEmpty()) {
        params.put("GithubUsername", username.toStdString());
        params.put("GithubSshKeys", resp.toStdString());
      } else {
        ConfirmationDialog::alert(QString("Username '%1'사용자에 대한 키가 GitHub에 존재하지 않습니다").arg(username), this);
      }
    } else {
      if (request->timeout()) {
        ConfirmationDialog::alert("Request timed out", this);
      } else {
        ConfirmationDialog::alert(QString("Username '%1' 의 GitHub아이디가 존재하지 않습니다").arg(username), this);
      }
    }

    refresh();
    request->deleteLater();
  });

  request->sendRequest("https://github.com/" + username + ".keys");
}
