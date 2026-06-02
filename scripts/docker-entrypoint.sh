#!/usr/bin/env bash
set -euo pipefail

export DISPLAY="${DISPLAY:-:1}"
export VNC_PASSWORD="${VNC_PASSWORD:-gba}"

if [[ "${ENABLE_VNC:-1}" == "1" ]]; then
    if ! pgrep -f "Xvfb ${DISPLAY}" >/dev/null 2>&1; then
        Xvfb "${DISPLAY}" -screen 0 1024x768x24 -nolisten tcp >/tmp/xvfb.log 2>&1 &
    fi

    sleep 0.4

    if ! pgrep -x fluxbox >/dev/null 2>&1; then
        fluxbox >/tmp/fluxbox.log 2>&1 &
    fi

    mkdir -p /tmp/gba-vnc
    x11vnc -storepasswd "${VNC_PASSWORD}" /tmp/gba-vnc/passwd >/dev/null 2>&1

    if ! pgrep -f "x11vnc.*${DISPLAY}" >/dev/null 2>&1; then
        x11vnc -display "${DISPLAY}" -rfbauth /tmp/gba-vnc/passwd -forever -shared -rfbport 5900 >/tmp/x11vnc.log 2>&1 &
    fi

    if ! pgrep -f "websockify.*6080" >/dev/null 2>&1; then
        websockify --web=/usr/share/novnc/ 6080 localhost:5900 >/tmp/novnc.log 2>&1 &
    fi
fi

exec "$@"
