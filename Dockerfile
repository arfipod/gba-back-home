ARG DEVKITPRO_IMAGE=devkitpro/devkitarm:20260221
FROM ${DEVKITPRO_IMAGE}

ARG BUTANO_REF=21.6.0
ARG DEBIAN_FRONTEND=noninteractive

ENV DEVKITPRO=/opt/devkitpro \
    DEVKITARM=/opt/devkitpro/devkitARM \
    LIBBUTANO=/opt/butano/butano \
    DISPLAY=:1 \
    VNC_PASSWORD=gba \
    ENABLE_VNC=1

RUN apt-get update && apt-get install -y --no-install-recommends \
    bash \
    ca-certificates \
    curl \
    fluxbox \
    git \
    make \
    mgba-qt \
    mgba-sdl \
    novnc \
    python3 \
    python-is-python3 \
    unzip \
    websockify \
    x11vnc \
    xauth \
    xdotool \
    xvfb \
    && rm -rf /var/lib/apt/lists/*

RUN git clone --depth 1 --branch "${BUTANO_REF}" https://github.com/GValiente/butano.git /opt/butano \
    || (rm -rf /opt/butano && git clone --depth 1 https://github.com/GValiente/butano.git /opt/butano)

WORKDIR /workspace
COPY scripts/docker-entrypoint.sh /usr/local/bin/gba-entrypoint
RUN chmod +x /usr/local/bin/gba-entrypoint

EXPOSE 5900 6080
ENTRYPOINT ["/usr/local/bin/gba-entrypoint"]
CMD ["bash", "-lc", "tail -f /dev/null"]
