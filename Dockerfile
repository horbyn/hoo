FROM ubuntu:jammy

ARG V=2.8
ARG BOCH_VERSION=bochs-${V}

RUN apt-get clean && apt-get update && apt-get install -yqq ca-certificates && \
    # 更新源
    cp /etc/apt/sources.list /etc/apt/sources.list.bak && \
    sed -i 's@//.*archive.ubuntu.com@//mirrors.ustc.edu.cn@g' /etc/apt/sources.list && \
    sed -i 's/security.ubuntu.com/mirrors.ustc.edu.cn/g' /etc/apt/sources.list && \
    sed -i 's/http:/https:/g' /etc/apt/sources.list && \
    apt-get update && apt-get install -yqq --no-install-recommends \
        sudo vim build-essential autoconf automake pkg-config libtool bison flex \
        check git gdb gdbserver qemu-system-x86 libgtk2.0-dev libreadline-dev wget && \
    rm -rf /var/lib/apt/lists/* && \
    apt-get clean && \
    # 创建调试版 gdb
    printf '#!/bin/bash\n\nsudo /usr/bin/gdb $@\n' > /usr/bin/gdb_sudo && \
    chmod +x /usr/bin/gdb_sudo && \
    # 安装 bochs
    cd /tmp && \
    wget -O ${BOCH_VERSION}.tar.gz \
        https://sourceforge.net/projects/bochs/files/bochs/${V}/${BOCH_VERSION}.tar.gz/download && \
    tar -zxf ${BOCH_VERSION}.tar.gz && \
    cd ${BOCH_VERSION} && \
    ./configure --enable-readline --enable-debugger --enable-debugger-gui \
        --enable-iodebug --with-x11 --enable-xpm --enable-show-ips --enable-logging \
        --enable-assert-checks --enable-cpu-level=6 --enable-fpu --enable-alignment-check \
        --enable-a20-pin --enable-large-ramfile --enable-cdrom --enable-pci && \
    make -j 4 && \
    make install && \
    rm -rf /tmp/* && \
    ldconfig

CMD ["/bin/bash"]
